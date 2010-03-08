/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/
/* File    : T:\wpsvob\wsp\work\codecs\headercodec\hc_encoder.c
   Version : \main\multline\4
   Date    : 13-Jun-00.17:00:54
*/

/*
 * ========================================================================
 *  Name        : hc_encoder.c
 *  Part of     : 
 *  Description : 
 *  Version     : 
 *
 *  Copyright (c) 2000 - 2005 Nokia Corporation.
 *  This material, including documentation and any related
 *  computer programs, is protected by copyright controlled by
 *  Nokia Corporation. All rights are reserved. Copying,
 *  including reproducing, storing, adapting or translating, any
 *  or all of this material requires the prior written consent of
 *  Nokia Corporation. This material also contains confidential
 *  information which may not be disclosed to others without the
 *  prior written consent of Nokia Corporation.
 * ========================================================================
 */


/*
-----------------------------------------------------------------------------
    Includes
-----------------------------------------------------------------------------
*/

#include "hc_util.h"
#include "hc_pages.h"
#include "other_util.h"

#include <string.h>
#include <ctype.h>


/* Temporary  defines here */
/* ------------------------------------------ */

/*  WSP error: Unknown warning code (as in profile-warning) */
#define WPS_HC_ERROR_HTTP_UNKNOWN_WARNING_CODE       (WPS_HC_STATUS_BASE + 59)

/*  WSP error: Invalid parameter */
#define WPS_HC_ERROR_WSP_INVALID_PARAMETER          (WPS_HC_STATUS_BASE + 123)
/*  WSP error: Unknown warning code (as in profile-warning) */
#define WPS_HC_ERROR_WSP_UNKNOWN_WARNING_CODE       (WPS_HC_STATUS_BASE + 125)

/* Passable means, that the return code doesn't indicate severe error. Included instead of 
   ret == WPS_HC_STATUS_OK, because we would like to continue in case of warnings 
*/
#define PASSABLE(x)   (!WPS_HC_STATUS_IS_ERROR(x))


/* Writeable means that the field value can be printed out, but something tructation, correction 
   or something like that happened to the value. Anyway, it is more or less correct. */
#define WRITEABLE(x)      ((x) == WPS_HC_STATUS_OK || \
                           (x) == WPS_HC_WARNING_NUMBER_TRUNCATED)

/* If set, then the encoder/decoder will stop at the first error it encounters, even if 
   the error is recoverable.
   In this case the functions will return the error code, regardless of the setting
   of WPS_HC_OPT_RETURN_ERROR flag.
   (This is the old behaviour of the decoder)
*/
#define WPS_HC_OPT_STOP_AT_ERROR                2


/* If set, this flag indicates that the function should return the _first_ encountered
   error value, instead of the possible warnings, those just indicate the skipping of one
   header field or truncation of one large number. 
   If no error occured, but a warning occured, the function will return the warning value 
   regardless of this flag 
*/
#define WPS_HC_OPT_PREFER_ERROR                 4



/* Default behavour :
    - Content-type like other fields.
    - Encoding/decoding tries to recover from recoverable errors (with sikkping the invalid field)
    - Encoding/decoding returns with the encountered warning value.
*/

/* ----------------------------------------------- */
/* Return value indicating that is not a real error, but the field is to be skipped */

#define INTERNAL_RET_SKIP_FIELD     0xFF00

/*
-----------------------------------------------------------------------------
    Types
-----------------------------------------------------------------------------
*/

/* Options for Date encoding. */
typedef enum _DateOption
{
    NON_DATE_IS_ERROR, 
    NON_DATE_IS_TEXT, 
    NON_DATE_IS_DELTASECONDS, 
} DateOption;

/* Encoding-state structure instead of any kind of global variables */

typedef struct _EncodeData
{
    BOOL LastFieldWasRange;
    BOOL WasContentType;
    HC_STATUS  Warnings;        /* The warnings collected during decoding. */
    HC_STATUS  FirstError;      /* The first encountered error during decoding */
    HC_WapVersion WapVersion;
} EncodeData;

/*
------------------------------------------------
*/

static HC_STATUS SetError(EncodeData* EncodeState, HC_STATUS error_code)
{
    if (EncodeState->FirstError == WPS_HC_STATUS_OK)
        EncodeState->FirstError = error_code;

    return error_code;
}

static void SetWarning(EncodeData* EncodeState, HC_STATUS warning_code)
{
    EncodeState->Warnings |= warning_code;
}

/*
-----------------------------------------------------------------------------
    Local helpers
-----------------------------------------------------------------------------
*/

BOOL HC_IsLWS (char c) {

    switch (c) {
    case '\0':
    case ' ' :
    case '\t':
    case '\r': 
    case '\n': 
        return TRUE; break;
    default:
        return FALSE; break;
    }    
}

BOOL HC_IsCtlorTspecial (char c) {

    switch (c) {
    case '(':
    case ')':
    case '<':
    case '>':
    case '@':
    case ',':
    case ';':
    case ':':
    case '\\':
    case '\"':
    case '/':
    case '[':
    case ']':
    case '?':
    case '=':
    case '{':
    case '}':
        return TRUE; break;
    default:
        return FALSE; break;
    }    
}

/* Returns the pointer to the character _after_ this line (after the last newline) */
const char* HC_SkipLine (const char* pos)
{
    while (*pos) {
        if (*pos == '\n') {
            pos++;
            if (*pos != '\t' && *pos != ' ') {
                break;
            }
        } else {
            pos++;
        }
    }
    return pos;
}


HC_STATUS HC_SkipLWS (char** pcFieldValue) {

    /* If the LWS begins with [CRLF] */
    if (*(*pcFieldValue) == '\r') {
        (*pcFieldValue)++;

        /* Next charachter MUST be LF */
        if (*(*pcFieldValue) == '\n') {
            (*pcFieldValue)++;
        } else {
            return WPS_HC_ERROR_HTTP_ILLEGAL_NEWLINE;
        }
    } else {
        if (*(*pcFieldValue) == '\n') {
            (*pcFieldValue)++;
        }
    }

    /* If we have [CRLF] this is the end of header */
    if (*(*pcFieldValue) == '\r') {
        (*pcFieldValue)++;

        /* Next charachter MUST be LF */
        if (*(*pcFieldValue) == '\n') {
            (*pcFieldValue)++;
            return 1; /* end of the header */
        } else {
            return WPS_HC_ERROR_HTTP_ILLEGAL_NEWLINE;
        }
    } else {
        if (*(*pcFieldValue) == '\n') {
            (*pcFieldValue)++;
            return 1; /* end of the header */
        }
    }

    /* Skip (SP | HT) */
    if ( (*(*pcFieldValue) == ' ') || (*(*pcFieldValue) == '\t')) {
            (*pcFieldValue)++;
    } else {
        return WPS_HC_ERROR_HTTP_ILLEGAL_NEWLINE;
    }

    /* Skip *(SP|HT) */
    while ( (*(*pcFieldValue) == ' ') || (*(*pcFieldValue) == '\t')) {
            (*pcFieldValue)++;
    }

    return 0;
}

/* No bit is set, this is the default modifier */
#define HC_NEXT_TOKEN            0

/* If this bit is set in the modifier, the returned token
 * can be 0 length long. Otherwise the lenght has to be a positive number
 * and error is returned, if there is no next token in the actual field value */
#define HC_OPTIONAL_TOKEN        1


/* The buffer can be a quoted string. If this is not set, quoted string 
   tokens are not accepted */
#define HC_QUOTED_TEXT           2

/* If this modifier is set, the buffer is parsed to the LWS */
#define HC_EXPECT_URI            4

/* If this modifier is set, the next token can be a separator, otherwise not */
#define HC_EXPECT_SEPARATOR      8

HC_STATUS HC_GetNextToken (char** pText, UINT32* tokenLen, UINT32 modifier) {
    char* pTmp;
    *tokenLen = 0;

    switch (*(*pText)) {
    
    /* End of the string */
    case '\0' : {
            *tokenLen=0;
            if (!(modifier & HC_OPTIONAL_TOKEN)) return 1;
            return 0;
                } break;

    /* LWS */
    case ' ' :
    case '\t':
    case '\n':
    case '\r': 
        {
            /* Skip LWS */
            if (HC_SkipLWS(pText) !=0) {
                *tokenLen=0;
                if (((*tokenLen) == 0) && !(modifier & HC_OPTIONAL_TOKEN)) return 1;
                return 0;
            }
        } break;

    default:
        ;
    }

    /* Now pText points to the beginning of the token */
    pTmp = *pText;

    if (modifier & HC_EXPECT_URI) {
        while ( !HC_IsLWS(*pTmp) ) {
                pTmp++; (*tokenLen)++;
            }
            if (((*tokenLen) == 0) && !(modifier & HC_OPTIONAL_TOKEN)) return 1;
    }

    /* Quoted string */
    if (*pTmp == '\"') {
        if (modifier & HC_QUOTED_TEXT) {
            pTmp++; (*tokenLen)++;
            while ( (*pTmp !='\"' || (*pTmp=='\"' && *(pTmp-1) == '\\')) &&  *pTmp !='\0') {
                pTmp++; (*tokenLen)++;
            }

            if (*pTmp =='\"' ) {
                /* Read the ending quote */
                (*tokenLen)++;
                return 0;
            } else {
                return 1;
            }
            
        } else {
            return 1;
        }
    }

    /* If Ctl OR tspecial */
    if (HC_IsCtlorTspecial(*pTmp)) {
        if (modifier & HC_EXPECT_SEPARATOR) {
            *tokenLen=1;
            return 0;
        } else {
            return 1;
        }
    }
    
    while ( !HC_IsCtlorTspecial(*pTmp) && !HC_IsLWS(*pTmp)) {
            pTmp++; (*tokenLen)++;
        }

    if (((*tokenLen) == 0) && !(modifier & HC_OPTIONAL_TOKEN)) 
        return 1;

    return 0;
}

#define CHECK_ERROR(macro_function)\
    if ((macro_function)!=0) return 1;

/*
-----------------------------------------------------------------------------
*/

HC_STATUS N_HC_EncodeUintvar (
    UINT32  nValue, 
    BYTE*   pbDestBuf, 
    UINT32* pnDestBufLen, 
    UINT32  nMaxDestBufLen)
{
    HC_STATUS   nRet            = WPS_HC_STATUS_OK;
    BYTE        abEncodedVal    [5];
    int         nBytes, 
                i;

    if (nValue == 0) {
        abEncodedVal[0] = 0;
        nBytes = 1;
    } else {
        for (nBytes = 0; nValue != 0; nBytes++, nValue = nValue / 128) 
        {
            abEncodedVal[nBytes] = (UINT8)(nValue % 128);
        }
    }

    if (*pnDestBufLen + nBytes <= nMaxDestBufLen) {
        for (i = 0; i < nBytes; i++) 
        {
            pbDestBuf[*pnDestBufLen + i] = abEncodedVal[nBytes - i - 1];
            if (i != nBytes - 1)
            {
                pbDestBuf[*pnDestBufLen + i] += 128;
            }
        }
        *pnDestBufLen += nBytes;
    } else { 
        /* Error - destination buffer is too small! */
        nRet = WPS_CODEC_ERROR_INT_BUFFER_TOO_SMALL;
    }

    return nRet;
} /* HC_EncodeUintvar */

HC_STATUS N_HC_EncodeLength (
    UINT32  nLength,        /* in:  Length value. */
    BYTE*   pbDestBuf,      /* out: The encoded WSP header. */
    UINT32* pnDestBufLen,
    UINT32  nMaxDestBufLen)
{
    if (nLength < WPS_HC_LENGTH_QUOTE) {
        return N_HC_EncodeOctet ((BYTE)nLength, pbDestBuf, pnDestBufLen, nMaxDestBufLen);
    } else  {
        /* Append length as uintvar */
        N_HC_EncodeOctet (WPS_HC_LENGTH_QUOTE, pbDestBuf, pnDestBufLen, nMaxDestBufLen);
        N_HC_EncodeUintvar (nLength, pbDestBuf, pnDestBufLen, nMaxDestBufLen);
    }

    return 0;

} /* N_HC_EncodeLength */


/*
-----------------------------------------------------------------------------
    Extracts a field value from the given HTTP header (field values may be 
    broken into multiple lines).
    The function also adjusts the <*ppcHttpHeader> pointer so that it points 
    to the first character of the next header field, or to the terminating NULL 
    character if there are no more header fields.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_ExtractFieldValue(
    const char**    ppcHttpHeader,  /* in:  Pointer to the actual location in the HTTP header.
                                       out: Pointer to the new location in the HTTP header. */
    BOOL            bCheckMultiVal, /* in:  Determines whether to check for multiple 
                                            field values or not. */
    char*           pcBuf,          /* out: The extracted field value. */
    int*            pnBufLen)       /* in:  The maximum length of the buffer.
                                       out: The actual length of the buffer (without the 
                                            terminating NULL). */
{
    HC_STATUS   nRet        = WPS_HC_STATUS_OK;
    const char* pcAct       = *ppcHttpHeader;
    const char* pcAction    = *ppcHttpHeader + 1; /* Skip the separator char */
    int         nLen, 
                nBufIdx     = 0;
    BOOL        bContinues  = TRUE;
    BOOL        bExistQuot  = FALSE;

    if (*pcAct == WPS_HC_VALUE_SEPARATOR) { /* Have multiple values */
        /* Skip the separator char */
        pcAct++; 

        /* Skip whitespaces */
        pcAct += FindChars (pcAct, WPS_HC_WHITESPACES);

        if ( strncmp(pcAct, WPS_HC_NEWLINE, WPS_HC_NEWLINE_SIZE) ==0 ) {
            const char* pcTmp;

            pcTmp = pcAct + WPS_HC_NEWLINE_SIZE; /* Begining of the continuation line */
            pcAct = pcTmp + FindChars (pcTmp, WPS_HC_WHITESPACES); /* Skip LWS */
            if (pcTmp == pcAct) {
                /* The continuation line MUST begin with LWS */
                return WPS_HC_ERROR_HTTP_ILLEGAL_NEWLINE;
            }
        }

    } else { /* Have a single value  */
        /* Skip the separator char */
        pcAct++; 

        /* Skip whitespaces */
        pcAct += FindChars (pcAct, WPS_HC_WHITESPACES);
    }

    while (bContinues)
    {
        const char* pcValSep    = NULL;

        nLen = strcspn (pcAct, WPS_HC_TERMINATORS);
        if (bCheckMultiVal)
        {   /* There may be multiple values - extract the first only */
            pcAction = pcAct;
            bExistQuot = FALSE;
            pcValSep = NULL;
            /* Runs through the buffer */
            while ( (*pcAction!='\0') && (!pcValSep) )
            {
                /* If the separator is after a quotation sign, skip it */
                if (*pcAction=='\"')
                {
                    if (bExistQuot) 
                        bExistQuot=FALSE;
                    else 
                        bExistQuot=TRUE;
                }else
                {
                    if (*pcAction==WPS_HC_VALUE_SEPARATOR)
                    {
                        if (!bExistQuot) 
                        {
                            pcValSep = pcAction;
                            if (pcValSep && pcValSep - pcAct < nLen)
                            {
                                nLen = pcValSep - pcAct;
                            }else
                            {
                                pcValSep = NULL;
                            }
                        }
                    }
                }
                pcAction++;
            }       
        }
        if (nBufIdx + nLen < *pnBufLen)
        {
            strncpy (pcBuf + nBufIdx, pcAct, nLen);
            nBufIdx += nLen;
            pcAct += nLen;
            if (!pcValSep && *pcAct)
            { /* A new line may come - field value may overlap */
                if (*pcAct == '\r')
                { /* This is supposed to be a \r\n - check it */
                    pcAct++;
                    if (*pcAct == '\n')
                    {
                        pcAct++;
                    }
                    else
                    { /* Error - a single \r ! */
                        bContinues = FALSE;
                        nRet = WPS_HC_ERROR_HTTP_ILLEGAL_NEWLINE;
                    }
                }
                else
                { /* We are on a single \n - skip it */
                    pcAct++;
                }
                /* Set the main pointer to the beginning of next line */
                *ppcHttpHeader = pcAct;
                if (nRet == WPS_HC_STATUS_OK)
                {   /* Check whether the field value continues in the new line */

                    nLen = FindChars (*ppcHttpHeader, WPS_HC_WHITESPACES);
                    
                    bContinues = (nLen > 0);
                    if (bContinues)
                    {
                        pcAct += nLen;
                        /* Substitute white spaces with a single space */
                        if (nBufIdx < *pnBufLen)
                        {
                            pcBuf [nBufIdx++] = ' ';
                        }
                        else
                        { /* Error -  the given buffer is too small! */
                            bContinues = FALSE;
                            nRet = WPS_CODEC_ERROR_INT_BUFFER_TOO_SMALL;
                        }
                    }
                }
            }
            if (pcValSep || *pcAct == '\0')
            { /* Input is terminated with a value separator or a NULL character */
                *ppcHttpHeader = pcAct;
                bContinues = FALSE;
            }
        }
        else
        { /* Error -  the given buffer is too small! */
            bContinues = FALSE;
            nRet = WPS_CODEC_ERROR_INT_BUFFER_TOO_SMALL;
        }
    }
    pcBuf[nBufIdx] = '\0';
    *pnBufLen = nBufIdx;
    return nRet;
} /* HC_ExtractFieldValue */

/*
-----------------------------------------------------------------------------
    BASIC ENCODER FUNCTIONS
-----------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------
    Appends the given number to the WSP buffer in long-int form.
    The number must be greater than 127; otherwise use EncodeShortInt().
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeLongInt(
    UINT32  nValue,         /* in:  Long value. */
    BYTE*   pbWspHeader,    /* out: The encoded WSP header. */
    UINT32* pnWspHdrLen)    /* in:  The actual length of the WSP header.
                               out: The new length of the WSP header. */
{
    HC_STATUS   nRet            = WPS_HC_STATUS_OK;
    BYTE        abEncodedVal    [5];
    int         nBytes, 
                i;

    if (nValue==0)
    {
        nBytes=1;
        abEncodedVal[0] = (BYTE)(nValue % 256);
    }
    else
    {
        for (nBytes = 0; nValue != 0; nBytes++, nValue = nValue / 256) 
        {
            abEncodedVal[nBytes] = (BYTE)(nValue % 256);
        }
    }
    if (*pnWspHdrLen + nBytes < WPS_CODEC_BUF_SIZE)
    {   /* Value fits into the buffer - store its length then copy it */
        pbWspHeader[(*pnWspHdrLen)++] = (BYTE)nBytes;
        for (i = 0; i < nBytes; i++) 
        {
            pbWspHeader[*pnWspHdrLen + i] = abEncodedVal[nBytes - i - 1];
        }
        *pnWspHdrLen += nBytes;
    }
    else
    { /* Error - WSP buffer is too small! */
        nRet = WPS_CODEC_ERROR_INT_BUFFER_TOO_SMALL;
    }
    return nRet;
} /* HC_EncodeLongInt */

/*
-----------------------------------------------------------------------------
    Appends the given integer-value to the WSP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeInteger(
    UINT32  nValue,         /* in:  Integer value. */
    BYTE*   pbWspHeader,    /* out: The encoded WSP header. */
    UINT32* pnWspHdrLen)    /* in:  The actual length of the WSP header.
                               out: The new length of the WSP header. */
{
    HC_STATUS  nRet    = WPS_HC_STATUS_OK;

    if (nValue >= 0 && nValue < 128)
    { /* Constrained-media - fits in one octet */
        nRet = HC_EncodeShortInt (nValue, pbWspHeader, pnWspHdrLen);
    }
    else
    {
        nRet = HC_EncodeLongInt (nValue, pbWspHeader, pnWspHdrLen);
    }
    return nRet;
} /* HC_EncodeInteger */

/*
-----------------------------------------------------------------------------
    Extracts the next field value from the HTTP header, encodes it 
    according to the rules associated with the specific header field,
    then appends the encoded data to the WSP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeFieldCode(
    BYTE*   pbActCodePage,  /* in:  Previous header code page number.
                               out: Actual header code page number. */
    BYTE    bCodePage,      /* in:  Header code page number for this field. */
    BYTE    bFieldCode,     /* in:  Header field code. */
    BYTE*   pbWspHeader,    /* out: The encoded WSP header. */
    UINT32* pnWspHdrLen)    /* in:  The actual length of the WSP header.
                               out: The new length of the WSP header. */
{
    HC_STATUS  nRet    = WPS_HC_STATUS_OK;

    if (bCodePage != *pbActCodePage)
    { /* Shift code page */
        if (bCodePage > WPS_HCP_SHORTCUT_MAX)
        { /* Code page number is too big - add a Shift-delimiter */
            nRet = HC_EncodeOctet (WPS_HC_SHIFT_DELIMITER, pbWspHeader, 
                                   pnWspHdrLen);
        }
        nRet = HC_EncodeOctet (bCodePage, pbWspHeader, pnWspHdrLen);
        *pbActCodePage = bCodePage;
    }
    nRet = HC_EncodeShortInt (bFieldCode, pbWspHeader, pnWspHdrLen);
    return nRet;
} /* HC_EncodeFieldCode */

/*
-----------------------------------------------------------------------------
    Extracts the next field value from the HTTP header, and appends it 
    to the WSP buffer as null-terminated text.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeFieldValueAsText(
    const char**    ppcHttpHeader,  /* in:  Pointer to the actual location in the HTTP header.
                                       out: Pointer to the new location in the HTTP header. */
    BYTE*           pbWspHeader,    /* out: The encoded WSP header. */
    UINT32*         pnWspHdrLen)    /* in:  The actual length of the WSP header.
                                       out: The new length of the WSP header. */
{
    char       acBuf   [WPS_CODEC_BUF_SIZE];
    int        nLen    = WPS_CODEC_BUF_SIZE;
    HC_STATUS  nRet    = HC_ExtractFieldValue (ppcHttpHeader, FALSE, acBuf, &nLen);

    if (nRet == WPS_HC_STATUS_OK)
    {
        nRet = HC_EncodeText (acBuf, pbWspHeader, pnWspHdrLen, TRUE);
    }
    return nRet;
} /* HC_EncodeFieldValueAsText */

/*
-----------------------------------------------------------------------------
    Encodes and appends the given length value to the WSP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeLength(
    UINT32  nLength,        /* in:  Length value. */
    BYTE*   pbWspHeader,    /* out: The encoded WSP header. */
    UINT32* pnWspHdrLen)    /* in:  The actual length of the WSP header.
                               out: The new length of the WSP header. */
{
    HC_STATUS  nRet    = WPS_HC_STATUS_OK;

    if (nLength < WPS_HC_LENGTH_QUOTE)
    {
        nRet = HC_EncodeOctet ((BYTE)nLength, pbWspHeader, pnWspHdrLen);
    }
    else
    {
        nRet = HC_EncodeOctet (WPS_HC_LENGTH_QUOTE, pbWspHeader, 
                               pnWspHdrLen);
        if (nRet == WPS_HC_STATUS_OK)
        { /* Append length as uintvar */
            nRet = HC_EncodeUintvar (nLength, pbWspHeader, 
                                     pnWspHdrLen);
        }
    }
    return nRet;
} /* HC_EncodeLength */

/*
-----------------------------------------------------------------------------
    Encodes the given well-known field name and appends the encoded data to 
    the WSP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeWellknownFieldName(
    const char* pcFieldName,    /* in:  The field name. */
    HC_WapVersion wapVer,       /* in:  WAP verson to comply with */
    BYTE*       pbWspHeader,    /* out: The encoded WSP header. */
    UINT32*     pnWspHdrLen)    /* in:  The actual length of the WSP header.
                                   out: The new length of the WSP header. */
{
    HC_STATUS   nRet        = WPS_HC_STATUS_OK;
    BYTE        bFieldCode;

    /* Only field-names on the default codepage are encoded */
    if (HC_FieldName2FieldCode (WPS_HCP_DEFAULT, NULL, 
                                pcFieldName, wapVer, &bFieldCode))
    {
        nRet = HC_EncodeShortInt (bFieldCode, pbWspHeader, 
                                  pnWspHdrLen);
    }
    else
    {   /* Unknown field name - add as text */
        nRet = HC_EncodeText (pcFieldName, pbWspHeader, 
                              pnWspHdrLen, TRUE);
    }
    return nRet;
} /* HC_EncodeWellknownFieldName */

/*
-----------------------------------------------------------------------------
    Encodes the given value as integer, if possible; otherwise as text.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeUnknown(
    EncodeData* EncodeState,
    const char* pcValue,        /* in:      Value to be encoded. */
    BYTE*       pbWspHeader,    /* out:     The encoded WSP header. */
    UINT32*     pnWspHdrLen)    /* in:      The actual length of the WSP header.
                                   out:     The new length of the WSP header. */
{
    HC_STATUS   nRet    = WPS_HC_STATUS_OK;
    const char* pch;

    for (pch = pcValue; isdigit ((int)(*pch)); pch++);
    if (*pch)
    {   /* Non-numeric value - add as text */
        nRet = HC_EncodeText (pcValue, pbWspHeader, 
                              pnWspHdrLen, TRUE);
    }
    else
    {
        UINT32 nNum = 0;
        if (StrtoUINT32(pcValue, &nNum, '\0')!=WPS_HC_STATUS_OK)
        {
            EncodeState->Warnings |= WPS_HC_WARNING_NUMBER_TRUNCATED;
        }
        nRet = HC_EncodeInteger (nNum, pbWspHeader, pnWspHdrLen);
    }
    return nRet;
} /* HC_EncodeUnknown*/

/*
-----------------------------------------------------------------------------
    PARAMETER ENCODING
-----------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------
    Encodes and appends the given Q-value to the WSP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeQValue(
    const char*     pcQValue,       /* in:  Pointer to the Q-value. */
    BOOL            bEncodeQParam,  /* in:  Determines whether to encode Q-param or not. */
    BYTE*           pbWspHeader,    /* out: The encoded WSP header. */
    UINT32*         pnWspHdrLen)    /* in:  The actual length of the WSP header.
                                       out: The new length of the WSP header. */
{
    HC_STATUS   nRet        = WPS_HC_STATUS_OK;
    int         nFull       = 1, 
                nFraction   = 0, 
                nFields, 
                nQValue, 
                nFracDigits = 0;
    const char*     pcTmp   = NULL; 
    const char* pc          = strchr (pcQValue, '.');

    if (pc)
    {   /* Count the digits after the '.' */
        for (pc++; isdigit ((int)(*pc)); nFracDigits++, pc++);
    }

    nFields = 0;
    pcTmp = strchr(pcQValue, '.');
    
    if (pcTmp)
    {
        pcTmp++;
        if (StrtoINT32(pcTmp, &nFraction, '\0')==WPS_HC_STATUS_OK)
        {
            nFields++;
        }
        pcTmp = pcQValue;
        if (StrtoINT32(pcQValue, &nFull, '.')==WPS_HC_STATUS_OK)
        {
            nFields++;
        }
    }
    else
    {
        if (StrtoINT32(pcQValue, &nFull, '\0')==WPS_HC_STATUS_OK)
        {
            nFields++;
        }
    }


    if (nFraction > 999)
    {
        nRet = WPS_HC_ERROR_WSP_INVALID_Q_CODE;
    }
    if ((nFields == 0) && (nRet==WPS_HC_STATUS_OK))
    {   /* Error - could not parse the Q-value! */
        nRet = WPS_HC_ERROR_HTTP_BAD_Q_VALUE;
    }
    else if ((nFull != 1) && (nRet==WPS_HC_STATUS_OK))/* Q-value of 1 is not encoded */
    {
        if (bEncodeQParam)
        {   /* Encode the Q-param now */
            nRet = HC_EncodeShortInt (WPS_HFC_PARAM_Q, pbWspHeader, 
                                      pnWspHdrLen);
        }
        if (nRet == WPS_HC_STATUS_OK)
        {
            while ( nFraction%10 == 0 && nFraction!=0 )
            {   /* There are some redundant 0 at the end of the number */
                nFraction = nFraction / 10;
                nFracDigits--;
            }

            if ( ( nFields == 1 && nFull == 0 ) || 
                    nFracDigits < 3 )
            {
                if (nFraction < 10 && nFracDigits == 1)
                {
                    nFraction *= 10;
                }
                nQValue = nFraction + 1;
            }
            else
            {
                nQValue = nFraction + 100;
            }
            nRet = HC_EncodeUintvar (nQValue, pbWspHeader, 
                                     pnWspHdrLen);
        }
    }
    return nRet;
} /* HC_EncodeQValue */

/*
-----------------------------------------------------------------------------
    Encodes and appends the given version value to the WSP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeVersionValue(
    const char*     pcVersion,      /* in:  Pointer to the version value. */
    BYTE*           pbWspHeader,    /* out: The encoded WSP header. */
    UINT32*         pnWspHdrLen)    /* in:  The actual length of the WSP header.
                                       out: The new length of the WSP header. */
{
    HC_STATUS   nRet            = WPS_HC_STATUS_OK;
    int         nDots;
    const char* pc;
    int         nMajorVersion, 
                nMinorVersion   = 15;
    char        *pcDot;
    int         nFields;
    const char* pcTmp;

    /* Count the number of dots in the version expression */
    for (pc = pcVersion, nDots = 0; pc; )
    {
        pc = strchr (pc, '.');
        if (pc)
        {
            pc++;
            nDots++;
        }
    }

    /* If there is a dot */
    /* Next to dot there are at least two characters, */
    /* and next to dot there is a 0, */
    /* then encode it as text */
    pcDot = strchr(pcVersion, (int)'.');
    if (pcDot != NULL)
        if ( ((unsigned)(pcDot-pcVersion)) < (strlen(pcVersion))-2 )
            if ( *(pcDot+1) == '0')
                nDots = 2;


    nFields = 0;
    nMinorVersion = 15;
    nMajorVersion = 0;
    pcTmp = strchr(pcVersion, '.');

    if (pcTmp)
    {
        pcTmp++;
        if (StrtoINT32(pcTmp, &nMinorVersion, '\0')==WPS_HC_STATUS_OK)
        {
            nFields++;
        }
        if (StrtoINT32(pcVersion, &nMajorVersion, '.')==WPS_HC_STATUS_OK)
        {
            nFields++;
        }
    }
    else
    {
        if (StrtoINT32(pcVersion, &nMajorVersion, '\0')==WPS_HC_STATUS_OK)
        {
            nFields++;
        }
    }

    if (nFields>= 1 && 
        nMajorVersion >= 1 && 
        nMajorVersion <= 7 &&
        ( (nDots == 1 && nMinorVersion <= 14) || (nDots == 0 && nMinorVersion == 15) ))
    { /* Version fits in one octet */
        BYTE    bCode = (BYTE)(nMajorVersion * 16 + nMinorVersion);

        nRet = HC_EncodeShortInt (bCode, pbWspHeader, 
                                  pnWspHdrLen);
    }
    else
    { /* Encode as text */
        int nLen    = strlen (pcVersion);

        if (*pnWspHdrLen + nLen < WPS_CODEC_BUF_SIZE)
        { /* Field value fits into the buffer - copy it */
            strcpy ((char*)(pbWspHeader + *pnWspHdrLen), pcVersion);
            *pnWspHdrLen += nLen + 1;
        }
        else
        { /* Error - WSP buffer is too small! */
            nRet = WPS_CODEC_ERROR_INT_BUFFER_TOO_SMALL;
        }
    }
    return nRet;
} /* HC_EncodeVersionValue */

/*
-----------------------------------------------------------------------------
    Extracts the next parameter from the HTTP header, encodes it 
    according to the associated rules, then appends the encoded data to 
    the WSP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeParameter(
    EncodeData*     EncodeState,
    const char**    ppcHttpHeader,  /* in:      Pointer to the actual location in the HTTP header.
                                       out:     Pointer to the new location in the HTTP header. */
    BOOL            bEncodeParName, /* in:      Determines whether to encode the param name or not. */
    BYTE*           pbWspHeader,    /* out:     The encoded WSP header. */
    UINT32*         pnWspHdrLen)    /* in:      The actual length of the WSP header.
                                       out:     The new length of the WSP header. */
 
{
    HC_STATUS   nRet    = WPS_HC_STATUS_OK;
    char*       pc2;
    char        *par_sep_pos;
    BYTE        bParamCode;
    UINT32      nNum = 0;

    if (**ppcHttpHeader == WPS_HC_PARAM_SEPARATOR || 
        **ppcHttpHeader == '\0')
    {
        (*ppcHttpHeader)++; /* Skip the parameter delimiter */
    }

    *ppcHttpHeader += FindChars (*ppcHttpHeader, WPS_HC_WHITESPACES);
    
    pc2 = strchr (*ppcHttpHeader, WPS_HC_EQUAL_SIGN);
    par_sep_pos = strchr(*ppcHttpHeader, WPS_HC_PARAM_SEPARATOR);
    if (pc2 && (!par_sep_pos || pc2 < par_sep_pos))
    {
        *pc2 = '\0';
        if (HC_ParamName2ParamCode (*ppcHttpHeader, &bParamCode))
        { /* Typed parameter */
            if (bEncodeParName && bParamCode != WPS_HFC_PARAM_Q)
            {
                nRet = HC_EncodeShortInt (bParamCode, pbWspHeader, 
                                          pnWspHdrLen);
            }
            if (nRet == WPS_HC_STATUS_OK)
            {
                /* Extract the parameter value */
                *ppcHttpHeader = pc2 + 1;
                pc2 = strchr (*ppcHttpHeader, WPS_HC_PARAM_SEPARATOR);
                if (pc2)
                {   /* Another parameter follows - cut it for now */
                    *pc2 = '\0';
                }
                switch (bParamCode)
                {
                case WPS_HFC_PARAM_Q            : /* Q-value */
                    nRet = HC_EncodeQValue (*ppcHttpHeader, bEncodeParName, 
                                            pbWspHeader, pnWspHdrLen);
                    break;
                case WPS_HFC_PARAM_CHARSET      : /* Well-known charset */
                    {
                        WORD    wCharsetCode;

                        if (HC_CharsetName2CharsetCode (*ppcHttpHeader, 
                                                        EncodeState->WapVersion,
                                                        &wCharsetCode))
                        {
                            nRet = HC_EncodeInteger (wCharsetCode, pbWspHeader, 
                                                     pnWspHdrLen);
                        }
                        else
                        {   /* Unknown charset - add as text */
                            nRet = HC_EncodeText (*ppcHttpHeader, pbWspHeader, 
                                                  pnWspHdrLen, TRUE);
                        }
                    }
                    break;
                case WPS_HFC_PARAM_LEVEL        : /* Version-value */
                    nRet = HC_EncodeVersionValue (*ppcHttpHeader, pbWspHeader, 
                                                  pnWspHdrLen);
                    break;
                case WPS_HFC_PARAM_PADDING      : /* Short-integer */
                    if (StrtoUINT32(*ppcHttpHeader, &nNum, '\0')!=WPS_HC_STATUS_OK)
                    {
                        EncodeState->Warnings |= WPS_HC_WARNING_NUMBER_TRUNCATED;
                    }
                    nRet = HC_EncodeShortInt (nNum, pbWspHeader, pnWspHdrLen);

                    break;
                case WPS_HFC_PARAM_TYPE         : /* Integer-value */
                    if (StrtoUINT32(*ppcHttpHeader, &nNum, '\0')!=WPS_HC_STATUS_OK)
                    {
                        EncodeState->Warnings |= WPS_HC_WARNING_NUMBER_TRUNCATED;
                    }
                    nRet = HC_EncodeInteger (nNum, pbWspHeader, pnWspHdrLen);

                    break;
                case WPS_HFC_PARAM_NAME         : /* Text-string */
                case WPS_HFC_PARAM_FILENAME     : /* Text-string */
                    nRet = HC_EncodeText (*ppcHttpHeader, pbWspHeader, 
                                          pnWspHdrLen, TRUE);
                    break;
                case WPS_HFC_PARAM_DIFFERENCES  : /* Field-name */
                    nRet = HC_EncodeWellknownFieldName(*ppcHttpHeader, 
                                                       EncodeState->WapVersion, 
                                                       pbWspHeader, 
                                                       pnWspHdrLen);
                    break;
                }
            }
        }
        else
        { /* Untyped parameter */
            nRet = HC_EncodeText (*ppcHttpHeader, pbWspHeader, 
                                  pnWspHdrLen, TRUE);
            if (nRet == WPS_HC_STATUS_OK)
            {
                /* Extract the parameter value */
                *ppcHttpHeader = pc2 + 1;
                pc2 = strchr (*ppcHttpHeader, WPS_HC_PARAM_SEPARATOR);
                if (pc2)
                {   /* Another parameter follows - cut it for now */
                    *pc2 = '\0';
                }
                nRet = HC_EncodeUnknown (EncodeState,*ppcHttpHeader, pbWspHeader, 
                                          pnWspHdrLen);
            }
        }
    }
    else
    { /* (Untyped) parameter without value */
        pc2 = strchr (*ppcHttpHeader, WPS_HC_PARAM_SEPARATOR);
        if (pc2)
        {   /* Another parameter follows - cut it for now */
            *pc2 = '\0';
        }
        nRet = HC_EncodeText (*ppcHttpHeader, pbWspHeader, 
                              pnWspHdrLen, TRUE);
        /* Add another zero representing "No-value" */
        if (nRet == WPS_HC_STATUS_OK)
            nRet = HC_EncodeOctet(0,pbWspHeader,pnWspHdrLen);

    }
    /* Set the pointer to the next parameter (if there is any) */
    *ppcHttpHeader = pc2;
    return nRet;
} /* HC_EncodeParameter */

/*
-----------------------------------------------------------------------------
    FIELD VALUE ENCODING
-----------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------
    Encodes the given media value and appends the encoded data to 
    the encode buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeMediaValue(
    EncodeData*     EncodeState,
    char*           pcFieldValue,   /* in:      The HTTP media value. */
    BOOL            bEncodeParName, /* in:      Determines whether to encode the parameter names or not. */
    Name2WordFunc   pfLookupFunc,   /* in:      Lookup function for encoding the media value. */
    BYTE*           pbEncodeBuf,    /* out:     The encoded media value. */
    UINT32*         pnBufLen,       /* in:      The maximum length of the encode buffer. 
                                       out:     The number of encoded bytes. */
    BOOL*           pbLenRequired)  /* out:     TRUE if value length is required for the encoding, 
                                                otherwise FALSE. */
{
    HC_STATUS   nRet        = WPS_HC_STATUS_OK;
    char*       pcToParam   = NULL;
    WORD        wValueCode;

    *pnBufLen = 0;
    *pbLenRequired = FALSE;
    /* Encode the field value in a separate buffer */
    pcToParam = strchr (pcFieldValue, WPS_HC_PARAM_SEPARATOR);
    if (pcToParam)
    { /* There are parameters */
        *pcToParam = '\0';
        *pbLenRequired = TRUE;
    }
    if (pfLookupFunc (pcFieldValue, EncodeState->WapVersion, &wValueCode))
    { /* Field value is well-known */
        nRet = HC_EncodeInteger (wValueCode, pbEncodeBuf, pnBufLen);
        if (wValueCode >= 128)
        { /* General form - length will be required */
            *pbLenRequired = TRUE;
        }
    }
    else
    { /* Field value is unknown - encode it as text */
        nRet = HC_EncodeText (pcFieldValue, pbEncodeBuf, pnBufLen, TRUE);
    }
    while (nRet == WPS_HC_STATUS_OK && pcToParam)
    { /* Encode the parameters */
        nRet = HC_EncodeParameter (EncodeState,(const char**)&pcToParam, bEncodeParName, 
                                   pbEncodeBuf, pnBufLen);
    }
    return nRet;
} /* HC_EncodeMediaValue */

/*
-----------------------------------------------------------------------------
    Encodes the Accept field value and appends the encoded data to 
    the WSP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_AppendFieldValue(
    BYTE*   pbFieldValue,       /* in:  The encoded field value. */
    UINT32  nValueLen,          /* in:  The length of the field value. */
    BOOL    bLengthRequired,    /* in:  Determines whether length is to be encoded or not. */
    BYTE*   pbWspHeader,        /* out: The encoded WSP header. */
    UINT32* pnWspHdrLen)        /* in:  The actual length of the WSP header.
                                   out: The new length of the WSP header. */
{
    HC_STATUS  nRet    = WPS_HC_STATUS_OK;

    /* Append the encoded value */
    if (nRet == WPS_HC_STATUS_OK && 
        bLengthRequired)
    {   /* Content-general-form: add the value length */
        nRet = HC_EncodeLength (nValueLen, pbWspHeader, 
                                pnWspHdrLen);
    }
    /* Copy the encoded field value */
    if (nRet == WPS_HC_STATUS_OK && 
        *pnWspHdrLen + nValueLen <= WPS_CODEC_BUF_SIZE)
    {
        memcpy (pbWspHeader + *pnWspHdrLen, 
                pbFieldValue, nValueLen);
        *pnWspHdrLen += nValueLen;
    }
    return nRet;
} /* HC_AppendFieldValue */

/*
-----------------------------------------------------------------------------
    Encodes the Accept field value and appends the encoded data to 
    the WSP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeFieldValue_Accept(
    EncodeData* EncodeState,
    char*       pcFieldValue,   /* in:      Field value. */
    BYTE*       pbWspHeader,    /* out:     The encoded WSP header. */
    UINT32*     pnWspHdrLen)    /* in:      The actual length of the WSP header.
                                   out:     The new length of the WSP header. */
{
    HC_STATUS   nRet            = WPS_HC_STATUS_OK;
    BYTE        abValBuf        [WPS_CODEC_BUF_SIZE];
    UINT32      nValBufLen      = WPS_CODEC_BUF_SIZE;
    BOOL        bLengthRequired = FALSE;

    nRet = HC_EncodeMediaValue (EncodeState,pcFieldValue, TRUE, HC_ContentTypeName2ContentTypeCode, 
                                abValBuf, &nValBufLen, &bLengthRequired);
    if (nRet == WPS_HC_STATUS_OK)
    {
        nRet = HC_AppendFieldValue (abValBuf, nValBufLen, bLengthRequired, 
                                    pbWspHeader, pnWspHdrLen);
    }
    return nRet;
} /* HC_EncodeFieldValue_Accept */

/*
-----------------------------------------------------------------------------
    Encodes the Accept-Charset field value and appends the encoded data to 
    the WSP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeFieldValue_AcceptCharset(
    EncodeData* EncodeState,
    char*       pcFieldValue,   /* in:      Field value. */
    BYTE*       pbWspHeader,    /* out:     The encoded WSP header. */
    UINT32*     pnWspHdrLen)    /* in:      The actual length of the WSP header.
                                   out:     The new length of the WSP header. */
{
    HC_STATUS   nRet            = WPS_HC_STATUS_OK;
    BYTE        abValBuf        [WPS_CODEC_BUF_SIZE];
    UINT32      nValBufLen      = WPS_CODEC_BUF_SIZE;
    BOOL        bLengthRequired = FALSE;

    nRet = HC_EncodeMediaValue (EncodeState,pcFieldValue, FALSE, HC_CharsetName2CharsetCode, 
                                abValBuf, &nValBufLen, &bLengthRequired);
    if (nRet == WPS_HC_STATUS_OK)
    {
        nRet = HC_AppendFieldValue (abValBuf, nValBufLen, bLengthRequired, 
                                    pbWspHeader, pnWspHdrLen);
    }
    return nRet;
} /* HC_EncodeFieldValue_AcceptCharset */

/*
-----------------------------------------------------------------------------
    Encodes the Accept-Language field value and appends the encoded data to 
    the WSP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeFieldValue_AcceptLanguage(
    EncodeData* EncodeState,
    char*       pcFieldValue,   /* in:      Field value. */
    BYTE*       pbWspHeader,    /* out:     The encoded WSP header. */
    UINT32*     pnWspHdrLen)    /* in:      The actual length of the WSP header.
                                   out:     The new length of the WSP header. */
{
    HC_STATUS   nRet            = WPS_HC_STATUS_OK;
    BYTE        abValBuf        [WPS_CODEC_BUF_SIZE];
    UINT32      nValBufLen      = WPS_CODEC_BUF_SIZE;
    BOOL        bLengthRequired = FALSE;

    nRet = HC_EncodeMediaValue (EncodeState,pcFieldValue, FALSE, HC_LangName2LangCode, 
                                abValBuf, &nValBufLen, &bLengthRequired);
    if (nRet == WPS_HC_STATUS_OK)
    {
        nRet = HC_AppendFieldValue (abValBuf, nValBufLen, bLengthRequired, 
                                    pbWspHeader, pnWspHdrLen);
    }
    return nRet;
} /* HC_EncodeFieldValue_AcceptLanguage */

/*
-----------------------------------------------------------------------------
    Encodes the given method name and appends the encoded data to 
    the WSP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeMethodName(
    char*   pcFieldValue,       /* in:  Field value. */
    BOOL    bUnknownIsError,    /* in:  Determines whether an unknown method name 
                                        generates error or is encoded as text. */
    BYTE*   pbWspHeader,        /* out: The encoded WSP header. */
    UINT32* pnWspHdrLen)        /* in:  The actual length of the WSP header.
                                   out: The new length of the WSP header. */
{
    HC_STATUS   nRet        = WPS_HC_STATUS_OK;
    BYTE        bMethodCode;

    if (HC_MethodName2MethodCode (pcFieldValue, &bMethodCode))
    {
        nRet = HC_EncodeShortInt (bMethodCode, pbWspHeader, 
                                  pnWspHdrLen);
    }
    else if (bUnknownIsError)
    {   /* Error - Unknown method! */
        nRet = WPS_HC_ERROR_HTTP_UNKNOWN_METHOD;
    }
    else
    {   /* Unknown method - encode it as text */
        nRet = HC_EncodeText (pcFieldValue, pbWspHeader, 
                              pnWspHdrLen, FALSE);
    }
    return nRet;
} /* HC_EncodeMethodName */

/*
-----------------------------------------------------------------------------
    Encodes the Authorization field value and appends the encoded data to 
    the WSP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeFieldValue_Authorization(
    char*   pcFieldValue,   /* in:  Field value. */
    BYTE*   pbWspHeader,    /* out: The encoded WSP header. */
    UINT32* pnWspHdrLen)    /* in:  The actual length of the WSP header.
                               out: The new length of the WSP header. */
{
    HC_STATUS   nRet        = WPS_HC_STATUS_OK;
    char        acScheme    [WPS_CODEC_BUF_SIZE];
    char        acCookie    [WPS_CODEC_BUF_SIZE];
    BYTE        abValBuf    [WPS_CODEC_BUF_SIZE];
    UINT32      nValBufLen  = 0;
    const char* pcTmp;

    pcTmp = StringCopy(acScheme, pcFieldValue, ' ');
    if (*pcTmp==' ')
    {
        pcTmp++;
        strcpy(acCookie, pcTmp);
    }

    if (HC_stricmp2 (acScheme, WPS_HC_FV_AUTH_SCHEME_BASIC_LC) == 0)
    {   /* Basic-cookie */
        int     nCookieLen;
        char*   pc;

        nRet = HC_EncodeOctet (128, abValBuf, &nValBufLen);
        
        /* Decode the base64 string and put a '\0' at the end */
        nCookieLen = HC_DecodeBase64 (acCookie, (char*)(abValBuf + nValBufLen), 
            HC_Strnlen(acCookie,0));
        *(abValBuf + nValBufLen + nCookieLen) = '\0';
        nCookieLen++;

        if (nCookieLen == 0)
        {
            nRet = WPS_HC_ERROR_HTTP_INVALID_B64ENC_STRING;
        }

        if (nRet == WPS_HC_STATUS_OK)
        {
            pc = strchr ((char*)(abValBuf + nValBufLen), ':');
            if (pc)
            {   /* Put a NULL separator between the userid and a password */
                *pc = '\0';
            }
            nValBufLen += nCookieLen;
        }
    }
    else
    {   /* Authentication scheme */
        nRet = HC_EncodeText (pcFieldValue, abValBuf, &nValBufLen, TRUE);
    }

    if (nRet == WPS_HC_STATUS_OK)
    {
        nRet = HC_AppendFieldValue (abValBuf, nValBufLen, TRUE, 
                                    pbWspHeader, pnWspHdrLen);
    }
    return nRet;
} /* HC_EncodeFieldValue_Authorization */

/*
-----------------------------------------------------------------------------
    Encodes the Cache-Control field value and appends the encoded data to 
    the WSP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
#define WPS_UNKNOWN_CACHECTL_CODE   0xFF

HC_STATUS HC_EncodeFieldValue_CacheControl(
    EncodeData* EncodeState,
    char*       pcFieldValue,   /* in:      Field value. */
    BYTE*       pbWspHeader,    /* out:     The encoded WSP header. */
    UINT32*     pnWspHdrLen)    /* in:      The actual length of the WSP header.
                                   out:     The new length of the WSP header. */

{
    HC_STATUS   nRet            = WPS_HC_STATUS_OK;
    BYTE        abValBuf        [WPS_CODEC_BUF_SIZE];
    UINT32      nValBufLen      = 0;
    BOOL        bLengthRequired = FALSE;
    char*       pc2             = strchr (pcFieldValue, WPS_HC_EQUAL_SIGN);
    BYTE        bValueCode      = WPS_UNKNOWN_CACHECTL_CODE;
    UINT32      nNum            = 0;

    if (pc2)
    {
        *pc2 = '\0';
        bLengthRequired = TRUE;
    }

    if (HC_CacheCtlName2CacheCtlCode (pcFieldValue, &bValueCode))
    {
        nRet = HC_EncodeOctet (bValueCode, abValBuf, &nValBufLen);
    }
    else
    {   /* Cache-extension */
        if (pc2)
        {
            *pc2 = WPS_HC_EQUAL_SIGN;
            bLengthRequired = FALSE;
        }
        pc2 = strchr (pcFieldValue, WPS_HC_PARAM_SEPARATOR);
        if (pc2)
        {
            *pc2 = '\0';
            bLengthRequired = TRUE;
        }

        nRet = HC_EncodeText (pcFieldValue, abValBuf, &nValBufLen, TRUE);

		/* If there is parameter-separator sign */
		if (nRet == WPS_HC_STATUS_OK && pc2)
		{
			/* Encode the parameter */
			nRet = HC_EncodeParameter (EncodeState,(const char**)&pc2, TRUE, abValBuf,
                                        &nValBufLen);
			pc2 = NULL;
		}
    }
    if (nRet == WPS_HC_STATUS_OK && pc2)
    {   /* There is an additional parameter */
        pcFieldValue = ++pc2; /* Skip separator */
        switch (bValueCode)
        {
        case WPS_HFC_CACHECTL_NO_CACHE  :
        case WPS_HFC_CACHECTL_PRIVATE   : /* 1*(Field-name) */
            pc2++; /* Skip <"> */
            pcFieldValue = strtok (pc2, ",\"");
            while (nRet == WPS_HC_STATUS_OK && pcFieldValue)
            {
                nRet = HC_EncodeWellknownFieldName (pcFieldValue, 
                                                    EncodeState->WapVersion, 
                                                    abValBuf, 
                                                    &nValBufLen);
                pcFieldValue = strtok (NULL, ",\"");
            }
            break;
        case WPS_UNKNOWN_CACHECTL_CODE  :
            nRet = HC_EncodeUnknown (EncodeState,pcFieldValue, abValBuf, 
                                     &nValBufLen);
            break;
        case WPS_HFC_CACHECTL_MAX_AGE   :
        case WPS_HFC_CACHECTL_MAX_STALE :
        case WPS_HFC_CACHECTL_MIN_FRESH : /* Delta-second-value */
			if (StrtoUINT32(pcFieldValue, &nNum, '\0')!=WPS_HC_STATUS_OK)
			{
				EncodeState->Warnings |= WPS_HC_WARNING_NUMBER_TRUNCATED;
			}
            nRet = HC_EncodeInteger (nNum, abValBuf, &nValBufLen);

            break;
        }
    }
    if (nRet == WPS_HC_STATUS_OK)
    {
        nRet = HC_AppendFieldValue (abValBuf, nValBufLen, bLengthRequired, 
                                    pbWspHeader, pnWspHdrLen);
    }
    return nRet;
} /* HC_EncodeFieldValue_CacheControl */

/*
-----------------------------------------------------------------------------
    Encodes the Content-Disposition field value and appends 
    the encoded data to the WSP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeFieldValue_ContentDisposition(
    EncodeData* EncodeState,
    char*       pcFieldValue,   /* in:      Field value. */
    BYTE*       pbWspHeader,    /* out:     The encoded WSP header. */
    UINT32*     pnWspHdrLen)    /* in:      The actual length of the WSP header.
                                   out:     The new length of the WSP header. */
{
    HC_STATUS   nRet        = WPS_HC_STATUS_OK;
    BYTE        abValBuf    [WPS_CODEC_BUF_SIZE];
    UINT32      nValBufLen  = 0;
    char*       pcToParam   = NULL;
    BYTE        bValue;

    pcToParam = strchr (pcFieldValue, WPS_HC_PARAM_SEPARATOR);
    if (pcToParam)
    { /* There are parameters */
        *pcToParam = '\0';
    }
    if (HC_Name2Byte_Unord (g_aContentDispValues, DIM (g_aContentDispValues), 
                            TRUE, pcFieldValue, &bValue))
    {
        nRet = HC_EncodeOctet (bValue, abValBuf, &nValBufLen);
    }
    else
    { /* Error - field value is unknown! */
        nRet = WPS_HC_ERROR_HTTP_UNKNOWN_CONTENT_DISP;
    }
    while (nRet == WPS_HC_STATUS_OK && pcToParam)
    { /* Encode the parameters */
        nRet = HC_EncodeParameter (EncodeState,(const char**)&pcToParam, TRUE, abValBuf, 
                                   &nValBufLen);
    }

    if (nRet == WPS_HC_STATUS_OK)
    {
        nRet = HC_AppendFieldValue (abValBuf, nValBufLen, TRUE, 
                                    pbWspHeader, pnWspHdrLen);
    }
    return nRet;
} /* HC_EncodeFieldValue_ContentDisposition */

/*
-----------------------------------------------------------------------------
    Encodes the Content-MD5 field value and appends 
    the encoded data to the WSP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeFieldValue_ContentMD5(
    char*   pcFieldValue,   /* in:  Field value. */
    BYTE*   pbWspHeader,    /* out: The encoded WSP header. */
    UINT32* pnWspHdrLen)    /* in:  The actual length of the WSP header.
                               out: The new length of the WSP header. */
{
    HC_STATUS   nRet        = WPS_HC_STATUS_OK;
    BYTE        abValBuf    [WPS_CODEC_BUF_SIZE];
    UINT32      nValBufLen  = 0;

    /* The terminating \0 can be included ! */
    nValBufLen = HC_DecodeBase64 (pcFieldValue, (char*)abValBuf, 
		HC_Strnlen(pcFieldValue,0));
	
	if (nValBufLen != 16)
	{
		nRet = WPS_HC_ERROR_HTTP_BAD_CONTENTMD5;
	}
	else
	{
		nRet = HC_AppendFieldValue (abValBuf, nValBufLen, TRUE, 
                                pbWspHeader, pnWspHdrLen);
	}
    return nRet;
} /* HC_EncodeFieldValue_ContentMD5 */

/*
-----------------------------------------------------------------------------
    Encodes the Content-Range field value and appends 
    the encoded data to the WSP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeFieldValue_ContentRange(
    char*   pcFieldValue,   /* in:  Field value. */
    BYTE*   pbWspHeader,    /* out: The encoded WSP header. */
    UINT32* pnWspHdrLen)    /* in:  The actual length of the WSP header.
                               out: The new length of the WSP header. */
{
    HC_STATUS   nRet            = WPS_HC_STATUS_OK;
    BYTE        abValBuf        [WPS_CODEC_BUF_SIZE];
    UINT32      nValBufLen      = 0;
    UINT32      nFirstBytePos, 
                nEntityLength;
	UINT32		nFields = 0;
	char*		pcTmp=NULL;
	
	if (strncmp(pcFieldValue, "bytes ", 6)==0)
	{
		pcTmp = &pcFieldValue[6];
		if (StrtoUINT32(pcTmp, &nFirstBytePos, '-')==WPS_HC_STATUS_OK)
		{
			nFields++;
		}
		pcTmp = strchr(pcTmp, '/');
		if (pcTmp)
		{
			pcTmp++;
			if (StrtoUINT32(pcTmp, &nEntityLength, '\0')==WPS_HC_STATUS_OK)
			{
				nFields++;
			}
		}
	}


	if (nFields==2)
    {
        nRet = HC_EncodeUintvar (nFirstBytePos, abValBuf, &nValBufLen);
        if (nRet == WPS_HC_STATUS_OK)
        {
            nRet = HC_EncodeUintvar (nEntityLength, abValBuf, &nValBufLen);
            if (nRet == WPS_HC_STATUS_OK)
            {
                nRet = HC_AppendFieldValue (abValBuf, nValBufLen, TRUE, 
                                            pbWspHeader, pnWspHdrLen);
            }
        }
    }
    else
    {   /* Error - failed to parse Content-Range value! */
        nRet = WPS_HC_ERROR_HTTP_BAD_CONTENT_RANGE;
    }
    return nRet;
} /* HC_EncodeFieldValue_ContentRange */

/*
-----------------------------------------------------------------------------
    Encodes the Content-Type field and appends the encoded data to 
    the WSP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeField_ContentType(
    EncodeData* EncodeState,
    BYTE*       pbActCodePage,  /* in:      Previous header code page number.
                                   out:     Actual header code page number. */
    BYTE        bCodePage,      /* in:      Header code page number for this field. */
    BITF        bFlags,         /* in:      Option flags. */
    char*       pcFieldValue,   /* in:      Field value. */
    BYTE*       pbWspHeader,    /* out:     The encoded WSP header. */
    UINT32*     pnWspHdrLen)    /* in:      The actual length of the WSP header.
                                   out:     The new length of the WSP header. */

{
    HC_STATUS   nRet            = WPS_HC_STATUS_OK;
    BYTE        abValBuf        [WPS_CODEC_BUF_SIZE];
    UINT32      nValBufLen      = WPS_CODEC_BUF_SIZE;
    BOOL        bLengthRequired = FALSE;

    nRet = HC_EncodeMediaValue (EncodeState,pcFieldValue, TRUE, HC_ContentTypeName2ContentTypeCode, 
                                abValBuf, &nValBufLen, &bLengthRequired);

    if (bFlags & WPS_HC_OPT_DONT_SEPARATE_CONTENTTYPE)
    { /* Append the field code first, then the encoded value */
        nRet = HC_EncodeFieldCode (pbActCodePage, bCodePage, WPS_HFC_DEF_CONTENT_TYPE, 
                                   pbWspHeader, pnWspHdrLen);
        if (nRet == WPS_HC_STATUS_OK)
        {
            nRet = HC_AppendFieldValue (abValBuf, nValBufLen, bLengthRequired, 
                                        pbWspHeader, pnWspHdrLen);
        }
    }
    else
    { /* Put the encoded value at the beginning of the buffer */
        int nLenIdx = nValBufLen;

        if (bLengthRequired)
        {   /* Encode the length first to get the full value length */
            nRet = HC_EncodeLength (nValBufLen, abValBuf, &nValBufLen);
        }
        if (nRet == WPS_HC_STATUS_OK && 
            *pnWspHdrLen + nValBufLen <= WPS_CODEC_BUF_SIZE)
        {   /* Make room for the value at the beginning of the WSP buffer */
            memmove (pbWspHeader + nValBufLen, 
                    pbWspHeader, *pnWspHdrLen);
            if (bLengthRequired)
            {   /* Copy the value length */
                memcpy (pbWspHeader, abValBuf + nLenIdx, 
                        nValBufLen - nLenIdx);
            }
            /* Copy the value itself */
            memcpy (pbWspHeader + nValBufLen - nLenIdx, 
                    abValBuf, nLenIdx);
            /* Update WSP buffer length */
            *pnWspHdrLen += nValBufLen;
        }
    }
    
    /* If the Content-type is already included, there is no need to put it in afterwards */
    if (nRet == WPS_HC_STATUS_OK)
        EncodeState->WasContentType = TRUE;

    return nRet;
} /* HC_EncodeField_ContentType */

/*
-----------------------------------------------------------------------------
    Encodes the Date field value and appends the encoded data to 
    the WSP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeFieldValue_Date(
    EncodeData* EncodeState,
    char*       pcFieldValue,   /* in:      The field value. */
    DateOption  eDateFlag,      /* in:      Determines what to do with a non-date value. */
    BYTE*       pbWspHeader,    /* out:     The encoded WSP header. */
    UINT32*     pnWspHdrLen,    /* in:      The actual length of the WSP header.
                                   out:     The new length of the WSP header. */
    BOOL        ZeroIfInvalid)   /* in : If set, then invalid date will be encoded as zero */
{
    HC_STATUS   nRet        = WPS_HC_STATUS_OK;
    int         nFields;
    DateStruct  OrigTime;
    char        acMonth     [WPS_CODEC_BUF_SIZE];
    BOOL        bDateRead   = TRUE;
	const char*	pcTmp		= NULL;
    UINT32      OldHeaderLen  = *pnWspHdrLen;

    /*  RFC 1123 date format (official HTTP/1.1): 
        e.g. Sun, 06 Nov 1994 08:49:37 GMT */
	nFields = 0;
	pcTmp = strchr(pcFieldValue, ',');
	if (pcTmp)
	{
		pcTmp+=2;
		if (StrtoINT32(pcTmp, &OrigTime.day, ' ')==WPS_HC_STATUS_OK)
		{
			nFields++;
		}
		pcTmp = strchr(pcTmp, ' ');
		if (pcTmp)
		{
			pcTmp++;
			pcTmp = StringCopy(acMonth, pcTmp, ' ');
			if (*pcTmp==' ')
			{
				nFields++;
			}
			pcTmp++;
			if (StrtoINT32(pcTmp, &OrigTime.year, ' ')==WPS_HC_STATUS_OK)
			{
				nFields++;
			}
			pcTmp = strchr(pcTmp, ' ');
			if (pcTmp)
			{
				pcTmp++;
				if (StrtoINT32(pcTmp, &OrigTime.hour, ':')==WPS_HC_STATUS_OK)
				{
					nFields++;
				}
				pcTmp = strchr(pcTmp, ':');
				if(pcTmp)
				{
					pcTmp++;
					if (StrtoINT32(pcTmp, &OrigTime.min, ':')==WPS_HC_STATUS_OK)
					{
						nFields++;
					}
					pcTmp = strchr(pcTmp, ':');
					if (pcTmp)
					{
						pcTmp++;
						if (StrtoINT32(pcTmp, &OrigTime.sec, ' ')==WPS_HC_STATUS_OK)
						{
							nFields++;
						}
					}
				}
			}
		}
	}


    if (nFields == 6)
    {
        OrigTime.year -= 1900;
    }
    else
    {   /*  RFC 850 date format (obsolete, 2-digit year): 
            e.g. Sunday, 06-Nov-94 08:49:37 GMT */
		nFields = 0;
		pcTmp = strchr(pcFieldValue, ',');
		if (pcTmp)
		{
			pcTmp+=2;
			if (StrtoINT32(pcTmp, &OrigTime.day, '-')==WPS_HC_STATUS_OK)
			{
				nFields++;
			}
			pcTmp = strchr(pcTmp, '-');
			if (pcTmp)
			{
				pcTmp++;
				pcTmp = StringCopy(acMonth, pcTmp, '-');
				if (*pcTmp=='-')
				{
					nFields++;
				}
				pcTmp++;
				if (StrtoINT32(pcTmp, &OrigTime.year, ' ')==WPS_HC_STATUS_OK)
				{
					nFields++;
				}
				pcTmp = strchr(pcTmp, ' ');
				if (pcTmp)
				{
					pcTmp++;
					if (StrtoINT32(pcTmp, &OrigTime.hour, ':')==WPS_HC_STATUS_OK)
					{
						nFields++;
					}
					pcTmp = strchr(pcTmp, ':');
					if (pcTmp)
					{
						pcTmp++;
						if (StrtoINT32(pcTmp, &OrigTime.min, ':')==WPS_HC_STATUS_OK)
						{
							nFields++;
						}
						pcTmp = strchr(pcTmp, ':');
						if (pcTmp)
						{
							pcTmp++;
							if (StrtoINT32(pcTmp, &OrigTime.sec, ' ')==WPS_HC_STATUS_OK)
							{
								nFields++;
							}
						}
					}
				}
			}
		}

        if (nFields == 6)
        {   
            /* Some robustness here: we allows four digit numbers for years */
            if (OrigTime.year > 1900) {
                OrigTime.year -= 1900;
            }

            /* Quick solution to Y2K problems :-) */
            if (OrigTime.year < 50)
            {
                OrigTime.year += 100;
            }
        }
        else
        {   /*  ANSI C's asctime() format: 
                e.g. Sun Nov 6 08:49:37 1994 */

			nFields = 0;
			pcTmp = strchr(pcFieldValue, ' ');
			if (pcTmp)
			{
				pcTmp++;
				pcTmp = StringCopy(acMonth, pcTmp, ' ');
				if (*pcTmp==' ')
				{
					nFields++;
				}
				pcTmp = strchr(pcTmp, ' ');
				if (pcTmp)
				{
					pcTmp++;
                    /* Bugfix, 2 spaces are allowed between month and day */
                    if (pcTmp && *pcTmp == ' ')
                        pcTmp++;
					if (StrtoINT32(pcTmp, &OrigTime.day, ' ')==WPS_HC_STATUS_OK)
					{
						nFields++;
					}
				}
                if (pcTmp) {
				    pcTmp = strchr(pcTmp, ' ');
				    if (pcTmp)
				    {
					    pcTmp++;
					    if (StrtoINT32(pcTmp, &OrigTime.hour, ':')==WPS_HC_STATUS_OK)
					    {
						    nFields++;
					    }
					    pcTmp = strchr(pcTmp, ':');
					    if(pcTmp)
					    {
						    pcTmp++;
						    if (StrtoINT32(pcTmp, &OrigTime.min, ':')==WPS_HC_STATUS_OK)
						    {
							    nFields++;
						    }
						    pcTmp = strchr(pcTmp, ':');
						    if (pcTmp)
						    {
							    pcTmp++;
							    if (StrtoINT32(pcTmp, &OrigTime.sec, ' ')==WPS_HC_STATUS_OK)
							    {
								    nFields++;
							    }
							    pcTmp = strchr(pcTmp, ' ');
							    if (pcTmp)
							    {
								    pcTmp++;
								    if (StrtoINT32(pcTmp, &OrigTime.year, ' ')==WPS_HC_STATUS_OK)
								    {
									    nFields++;
								    }
							    }
						    }
					    }
				    }
                }
			}
		


            if (nFields == 6)
            {
                OrigTime.year -= 1900;
            }
            else
            {   /* Non-date value */
                bDateRead = FALSE;
                switch (eDateFlag)
                {
                case NON_DATE_IS_TEXT           :
                    nRet = HC_EncodeText (pcFieldValue, pbWspHeader, 
                                          pnWspHdrLen, FALSE);
                    break;
                case NON_DATE_IS_DELTASECONDS   :
                    nRet = HC_EncodeOctet (129, pbWspHeader, 
                                           pnWspHdrLen);
                    if (nRet == WPS_HC_STATUS_OK)
                    {
						UINT32 nNum = 0;
						if (StrtoUINT32(pcFieldValue, &nNum, '\0')!=WPS_HC_STATUS_OK)
						{
							EncodeState->Warnings |= WPS_HC_WARNING_NUMBER_TRUNCATED;
                        }
						nRet = HC_EncodeInteger (nNum, pbWspHeader, pnWspHdrLen);
					}
                    break;
                case NON_DATE_IS_ERROR          :
                default                         :
                    nRet = WPS_HC_ERROR_HTTP_BAD_DATE;
                    break;
                }
            }
        }
    }
    if (bDateRead && nRet == WPS_HC_STATUS_OK)
    {
        BYTE    bValue;

        /* Determine the month */
        if (HC_Name2Byte_Unord (g_aMonths, DIM (g_aMonths), 
                                FALSE, acMonth, &bValue))
        {   /* Convert the date */
			UINT32      GmTime;

            OrigTime.mon = bValue;
			/* Check if the date is valid */
			if (OrigTime.year < 70)
			{
				nRet = WPS_HC_ERROR_HTTP_INVALID_DATE;
			}
			else 
			{
				if(OrigTime.day < 1)
				{
					nRet = WPS_HC_ERROR_HTTP_INVALID_DATE;
				}
				else
				{
					if ((OrigTime.mon==1)
						&& (((OrigTime.year+1900)%4==0 && (OrigTime.year+1900)%100>0) || (OrigTime.year+1900)%400==0 ))
					{
						if(OrigTime.day>29)
						{
							nRet = WPS_HC_ERROR_HTTP_INVALID_DATE;
						}
					}
					else
					{
						static int days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
						
						if (OrigTime.day > days[OrigTime.mon])
						{
							nRet = WPS_HC_ERROR_HTTP_INVALID_DATE;
						}
					}
				}
			}
			if (nRet == WPS_HC_STATUS_OK)
			{
				GmTime = DatetoSec(OrigTime);
				if (eDateFlag == NON_DATE_IS_DELTASECONDS)
				{   /* Side effect: one octet is needed to identify date */
                    nRet = HC_EncodeOctet (128, pbWspHeader, 
										pnWspHdrLen);
				}
				nRet = HC_EncodeLongInt (GmTime, pbWspHeader, pnWspHdrLen);
			}
        }
        else
        { /* Error - illegal month value! */
            nRet = WPS_HC_ERROR_HTTP_BAD_MONTH;
        }
    }
    /* Set to  zero if ZeroIfInvalid flag is set (e.g. Expires) */
    if (nRet != WPS_HC_STATUS_OK && ZeroIfInvalid) {
      *pnWspHdrLen = OldHeaderLen;
      nRet = HC_EncodeLongInt(0, pbWspHeader, pnWspHdrLen);
    }
    
    return nRet;
} /* HC_EncodeFieldValue_Date */

/*
-----------------------------------------------------------------------------
    Encodes the Pragma field value and appends the encoded data to 
    the WSP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeFieldValue_Pragma(
    EncodeData* EncodeState,
    char*       pcFieldValue,   /* in:      The field value. */
    BYTE*       pbWspHeader,    /* out:     The encoded WSP header. */
    UINT32*     pnWspHdrLen)    /* in:      The actual length of the WSP header.
                                   out:     The new length of the WSP header. */
{
    HC_STATUS   nRet            = WPS_HC_STATUS_OK;
    BYTE        abValBuf        [WPS_CODEC_BUF_SIZE];
    UINT32      nValBufLen      = 0;
    BOOL        bLengthRequired = FALSE;

    if (HC_stricmp2 (pcFieldValue, WPS_HC_FV_PRAGMA_NO_CACHE) == 0)
    {
        nRet = HC_EncodeOctet (128, abValBuf, &nValBufLen);
    }
    else
    { /* Encode it as a parameter */
        bLengthRequired = TRUE;
        nRet = HC_EncodeParameter (EncodeState,(const char**)&pcFieldValue, TRUE, abValBuf, 
                                   &nValBufLen);
    }

    if (nRet == WPS_HC_STATUS_OK)
    {
        nRet = HC_AppendFieldValue (abValBuf, nValBufLen, bLengthRequired, 
                                    pbWspHeader, pnWspHdrLen);
    }
    return nRet;
} /* HC_EncodeFieldValue_Pragma */



/*
-----------------------------------------------------------------------------
    Encodes the Proxy-Authenticate field value and appends the encoded data to 
    the WSP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeFieldValue_ProxyAuthenticate(
    char*   pcFieldValue,   /* in:  The field value. */
    BYTE*   pbWspHeader,    /* out: The encoded WSP header. */
    UINT32* pnWspHdrLen,    /* in:  The actual length of the WSP header.
                               out: The new length of the WSP header. */
    UINT32  nMaxWspHdrLen)
{
    UINT32 tokenLen =0;
    BYTE   pcEncodedLength[10];
    UINT32 nLen;
    UINT32 savedIndex = *pnWspHdrLen;
    UINT32 nFieldValueLength;

    CHECK_ERROR (HC_GetNextToken (&pcFieldValue, &tokenLen, HC_NEXT_TOKEN));

    /* Basic authentication sheme */
    if (HC_strnicmp(pcFieldValue, "Basic", tokenLen)==0) {
        pcFieldValue +=tokenLen;

        /* realm */
        CHECK_ERROR ( HC_GetNextToken (&pcFieldValue, &tokenLen, HC_NEXT_TOKEN) );
        if (tokenLen == 0) {
            return WPS_HC_ERROR_HTTP_BAD_AUTHENTICATE;
        }
        CHECK_ERROR ( HC_strnicmp(pcFieldValue, "realm", tokenLen) );
        N_HC_EncodeOctet (0x80, pbWspHeader, pnWspHdrLen, nMaxWspHdrLen);
        pcFieldValue +=tokenLen;

        /* = */
        CHECK_ERROR ( HC_GetNextToken (&pcFieldValue, &tokenLen, HC_EXPECT_SEPARATOR) );
        CHECK_ERROR ( strncmp(pcFieldValue, "=", tokenLen) );
        pcFieldValue +=tokenLen;

        /* realm-value */
        CHECK_ERROR ( HC_GetNextToken (&pcFieldValue, &tokenLen, HC_QUOTED_TEXT ) );
        /* Encode realm value with quotes */
        N_HC_EncodeText (pcFieldValue, tokenLen, pbWspHeader, pnWspHdrLen, nMaxWspHdrLen, TRUE);
        pcFieldValue +=tokenLen;
    } else {
        /* auth-scheme is a general token */

        /* Authentication-scheme */
        CHECK_ERROR ( HC_GetNextToken (&pcFieldValue, &tokenLen, HC_NEXT_TOKEN) );
        N_HC_EncodeText (pcFieldValue, tokenLen, pbWspHeader, pnWspHdrLen, nMaxWspHdrLen, FALSE);
        pcFieldValue +=tokenLen;

        /* realm */
        CHECK_ERROR ( HC_GetNextToken (&pcFieldValue, &tokenLen, HC_NEXT_TOKEN) );
        CHECK_ERROR ( HC_strnicmp(pcFieldValue, "realm", tokenLen) );
        pcFieldValue +=tokenLen;

        /* = */
        CHECK_ERROR ( HC_GetNextToken (&pcFieldValue, &tokenLen, HC_EXPECT_SEPARATOR) );
        CHECK_ERROR ( strncmp(pcFieldValue, "=", tokenLen) );
        pcFieldValue +=tokenLen;

        /* realm-value */
        CHECK_ERROR ( HC_GetNextToken (&pcFieldValue, &tokenLen, HC_QUOTED_TEXT) );
        /* Encode realm value with quotes */
        N_HC_EncodeText (pcFieldValue, tokenLen, pbWspHeader, pnWspHdrLen, nMaxWspHdrLen, TRUE);
        pcFieldValue +=tokenLen;

        /* *Auth-param */
        CHECK_ERROR ( HC_GetNextToken (&pcFieldValue, &tokenLen, HC_EXPECT_SEPARATOR | HC_OPTIONAL_TOKEN) );
        while (tokenLen !=0 && (strncmp(pcFieldValue, ",", tokenLen)==0)) {
            /* auth-param */
            pcFieldValue +=tokenLen;

        /* token */
        CHECK_ERROR ( HC_GetNextToken (&pcFieldValue, &tokenLen, HC_NEXT_TOKEN) );
        N_HC_EncodeText (pcFieldValue, tokenLen, pbWspHeader, pnWspHdrLen, nMaxWspHdrLen, FALSE);
        pcFieldValue +=tokenLen;

        /* = */
        CHECK_ERROR ( HC_GetNextToken (&pcFieldValue, &tokenLen, HC_EXPECT_SEPARATOR) );
        CHECK_ERROR ( strncmp(pcFieldValue, "=", tokenLen) );
        pcFieldValue +=tokenLen;

        /* quoted-string */
        CHECK_ERROR ( HC_GetNextToken (&pcFieldValue, &tokenLen, HC_QUOTED_TEXT) );
        N_HC_EncodeText (pcFieldValue, tokenLen, pbWspHeader, pnWspHdrLen, nMaxWspHdrLen, TRUE);
        pcFieldValue +=tokenLen;

        CHECK_ERROR ( HC_GetNextToken (&pcFieldValue, &tokenLen, HC_EXPECT_SEPARATOR | HC_OPTIONAL_TOKEN) );
        }
    }

    /* Encode the length of the value */
    nLen=0; nFieldValueLength = *pnWspHdrLen - savedIndex;
    N_HC_EncodeLength (nFieldValueLength, pcEncodedLength, &nLen, 10);
    *pnWspHdrLen += nLen;
    if (*pnWspHdrLen <= nMaxWspHdrLen) {
        memmove (pbWspHeader+savedIndex+nLen, pbWspHeader+savedIndex, nFieldValueLength);
        memcpy  (pbWspHeader+savedIndex, pcEncodedLength, nLen);
    }

    return 0;

} /* HC_EncodeFieldValue_ProxyAuthenticate */

/*
-----------------------------------------------------------------------------
    Encodes the Range field value and appends the encoded data to 
    the WSP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeFieldValue_Range(
    EncodeData*  EncodeState,
    char*   pcFieldValue,   /* in:  The field value. */
    BYTE*   pbWspHeader,    /* out: The encoded WSP header. */
    UINT32* pnWspHdrLen)    /* in:  The actual length of the WSP header.
                               out: The new length of the WSP header. */
{
    HC_STATUS   nRet            = WPS_HC_STATUS_OK;
    BYTE        abValBuf        [WPS_CODEC_BUF_SIZE];
    UINT32      nValBufLen      = 0;
    UINT32      nFirstBytePos, 
                nLastBytePos, 
                nSuffixLength;
	INT32		nFields, nFields2;
	const char*	pcTmp=NULL;

	
	nFields = 0;
	if (strncmp(pcFieldValue, "bytes=-", 7)==0)
	{
		pcTmp = strchr(pcFieldValue, '-');
		if (pcTmp)
		{
			pcTmp++;
			if (StrtoUINT32(pcTmp, &nSuffixLength, '\0')==WPS_HC_STATUS_OK)
			{
				nFields++;
			}
		}
	}

	nFields2 = 0;
	pcTmp = pcFieldValue;
	if (*pcTmp=='-')
	{
		pcTmp++;
		if (StrtoUINT32(pcTmp, &nSuffixLength, '\0')==WPS_HC_STATUS_OK)
		{
			nFields2++;
		}
	}


    if( ( !(EncodeState->LastFieldWasRange) && 
          nFields > 0 ) || 
        ( EncodeState->LastFieldWasRange && 
          nFields2 > 0 ) )
    {   /* Suffix-byte-range */
        nRet = HC_EncodeOctet (129, abValBuf, &nValBufLen);
        if (nRet == WPS_HC_STATUS_OK)
        {
            nRet = HC_EncodeUintvar (nSuffixLength, abValBuf, &nValBufLen);
        }
    }
    else
    {
        int nFields;

        if (!(EncodeState->LastFieldWasRange))
        {
			nFields = 0;
			if (strncmp(pcFieldValue, "bytes=", 6)==0)
			{
				pcTmp = strchr(pcFieldValue, '=');
				if (pcTmp)
				{
					pcTmp++;
					if (StrtoUINT32(pcTmp, &nFirstBytePos, '-')==WPS_HC_STATUS_OK)
					{
						nFields++;
					}
					pcTmp = strchr(pcTmp, '-');
					if (pcTmp)
					{
						pcTmp++;
						if (StrtoUINT32(pcTmp, &nLastBytePos, '\0')==WPS_HC_STATUS_OK)
						{
							nFields++;
						}
					}
				}
			}

        }
        else
        {
			nFields = 0;
			if (StrtoUINT32(pcFieldValue, &nFirstBytePos, '-')==WPS_HC_STATUS_OK)
			{
				nFields++;
			}
			pcTmp = strchr(pcFieldValue, '-');
			if (pcTmp)
			{
				pcTmp++;
				if (StrtoUINT32(pcTmp, &nLastBytePos, '\0')==WPS_HC_STATUS_OK)
				{
					nFields++;
				}
			}

        }
        if (nFields > 0)
        {   /* Byte-range */
            nRet = HC_EncodeOctet (128, abValBuf, &nValBufLen);
            if (nRet == WPS_HC_STATUS_OK)
            {
                nRet = HC_EncodeUintvar (nFirstBytePos, abValBuf, &nValBufLen);
                if (nFields == 2 && nRet == WPS_HC_STATUS_OK)
                {
                    nRet = HC_EncodeUintvar (nLastBytePos, abValBuf, &nValBufLen);
                }
            }
        }
        else
        {   /* Error - failed to parse Range value! */
            nRet = WPS_HC_ERROR_HTTP_BAD_RANGE;
        }
    }

    if (nRet == WPS_HC_STATUS_OK)
    {
        nRet = HC_AppendFieldValue (abValBuf, nValBufLen, TRUE, 
                                    pbWspHeader, pnWspHdrLen);
    }
    return nRet;
} /* HC_EncodeFieldValue_Range */

/*
-----------------------------------------------------------------------------
    Encodes the Retry-After field value and appends the encoded data to 
    the WSP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeFieldValue_RetryAfter(
    EncodeData* EncodeState,
    char*       pcFieldValue,   /* in:      Field value. */
    BYTE*       pbWspHeader,    /* out:     The encoded WSP header. */
    UINT32*     pnWspHdrLen)    /* in:      The actual length of the WSP header.
                                   out:     The new length of the WSP header. */
{
    HC_STATUS   nRet        = WPS_HC_STATUS_OK;
    BYTE        abValBuf    [WPS_CODEC_BUF_SIZE];
    UINT32      nValBufLen  = 0;

    nRet = HC_EncodeFieldValue_Date (EncodeState,pcFieldValue, NON_DATE_IS_DELTASECONDS, 
                                     abValBuf, &nValBufLen,FALSE);
    if (nRet == WPS_HC_STATUS_OK)
    {
        nRet = HC_AppendFieldValue (abValBuf, nValBufLen, TRUE, 
                                    pbWspHeader, pnWspHdrLen);
    }
    return nRet;
} /* HC_EncodeFieldValue_RetryAfter */

/*
-----------------------------------------------------------------------------
    Encodes the Via field value and appends the encoded data to 
    the WSP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeFieldValue_Via(
    char*   pcFieldValue,   /* in:  Field value. */
    BYTE*   pbWspHeader,    /* out: The encoded WSP header. */
    UINT32* pnWspHdrLen)    /* in:  The actual length of the WSP header.
                               out: The new length of the WSP header. */
{
    HC_STATUS   nRet        = WPS_HC_STATUS_OK;
    char        acProtocol  [WPS_CODEC_BUF_SIZE];
    char        acRecvdBy   [WPS_CODEC_BUF_SIZE];
	const char* pcTmp = NULL;

    /* Do not encode any comments */

	pcTmp = StringCopy(acProtocol, pcFieldValue, ' ');
	pcTmp++;
	pcTmp = StringCopy(acRecvdBy, pcTmp, ' ');


    strcat (acProtocol, " ");
    strcat (acProtocol, acRecvdBy);

    nRet = HC_EncodeText (acProtocol, pbWspHeader, 
                          pnWspHdrLen, FALSE);
    return nRet;
} /* HC_EncodeFieldValue_Via */

/*
-----------------------------------------------------------------------------
    Encodes the Warning field value and appends the encoded data to 
    the WSP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeFieldValue_Warning(
    char*   pcFieldValue,   /* in:  The field value. */
    BYTE*   pbWspHeader,    /* out: The encoded WSP header. */
    UINT32* pnWspHdrLen)    /* in:  The actual length of the WSP header.
                               out: The new length of the WSP header. */
{
    HC_STATUS   nRet            = WPS_HC_STATUS_OK;
    BYTE        abValBuf        [WPS_CODEC_BUF_SIZE];
    UINT32      nValBufLen      = 0;
    int         nWarnCode       = 0;
    char        acWarnAgent     [WPS_CODEC_BUF_SIZE]="";
    char        acWarnText      [WPS_CODEC_BUF_SIZE]="";
    BOOL        bLengthRequired = FALSE;
	char		*pWarnText;
    int         nFieldsRead;
	const char* pcTmp = NULL;
	
	nFieldsRead = 0;
	if (StrtoINT32(pcFieldValue, &nWarnCode, ' ')==WPS_HC_STATUS_OK)
	{
		nFieldsRead++;
	}
	
	if (nFieldsRead>0)
	{
		pcTmp = strchr(pcFieldValue, ' ');
		if (pcTmp)
		{
			pcTmp++;
			pcTmp = StringCopy(acWarnAgent, pcTmp, ' ');
			nFieldsRead++;
		}
	}


	pWarnText = strchr(pcFieldValue, '\"');

	if (pWarnText) 
	{
		strcpy(acWarnText, pWarnText);
		nFieldsRead++;
	}


    if (nFieldsRead > 0)
    {
        nRet = HC_EncodeShortInt ((BYTE)nWarnCode, abValBuf, &nValBufLen);
        if (nRet == WPS_HC_STATUS_OK && nFieldsRead == 3)
        {
            bLengthRequired = TRUE;
            nRet = HC_EncodeText (acWarnAgent, abValBuf, 
                                  &nValBufLen, TRUE);
            if (nRet == WPS_HC_STATUS_OK)
            {
                nRet = HC_EncodeText (acWarnText, abValBuf, &nValBufLen, 
                                      TRUE);
            }
        }
    }
    else
    {   /* Error - failed to parse Warning value! */
        nRet = WPS_HC_ERROR_HTTP_BAD_WARNING;
    }

    if (nRet == WPS_HC_STATUS_OK)
    {
        nRet = HC_AppendFieldValue (abValBuf, nValBufLen, bLengthRequired, 
                                    pbWspHeader, pnWspHdrLen);
    }
    return nRet;
} /* HC_EncodeFieldValue_Warning */


/*
-----------------------------------------------------------------------------
    Encodes the X-Wap-Application-Id field value and appends the encoded data 
    to the WSP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeFieldValue_ApplicationIdValue (
    char*   pcFieldValue,   /* in:  The field value. */
    BYTE*   pbWspHeader,    /* out: Pointer to the encoded WSP header. */
    UINT32* pnWspHdrLen,    /* in:  The actual length of the WSP header.
                               out: The new length of the WSP header. It can grow out the 
                                    size of the pbWspHeader buffer */
    UINT32  nMaxWspHdrLen)  /* in:  The maximum length of the pbWspHeader binary buffer, 
                                    allocated by the user */
{
    /* Application-id-value = Uri-Value | App-assigned-code 
       App-assigned-code    = Integer-value */

    UINT32 tokenLen =0;
    UINT32 uInt;

    HC_GetNextToken (&pcFieldValue, &tokenLen, HC_EXPECT_URI);

    /* Application-id-value */
    if (*pcFieldValue <= '9' && *pcFieldValue >= '0')  {
        CHECK_ERROR(N_StrtoUINT32 (pcFieldValue, tokenLen, &uInt));

        /* App-assigned-code */
        HC_EncodeInteger (uInt, pbWspHeader, pnWspHdrLen);
       pcFieldValue +=tokenLen;
    } else {
        /* Uri-Value */
       N_HC_EncodeText (pcFieldValue, tokenLen, pbWspHeader, pnWspHdrLen, nMaxWspHdrLen, FALSE);
       pcFieldValue +=tokenLen;
    }

    return 0;

} /* HC_EncodeFieldValue_ApplicationIdValue */


/*
-----------------------------------------------------------------------------
    Encodes the X-Wap-Application-Id field value and appends the encoded data 
    to the WSP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeFieldValue_URIValue (
    char*   pcFieldValue,   /* in:  The field value. */
    BYTE*   pbWspHeader,    /* out: Pointer to the encoded WSP header. */
    UINT32* pnWspHdrLen,    /* in:  The actual length of the WSP header.
                               out: The new length of the WSP header. It can grow out the 
                                    size of the pbWspHeader buffer */
    UINT32  nMaxWspHdrLen)  /* in:  The maximum length of the pbWspHeader binary buffer, 
                                    allocated by the user */
{
    /* Uri-Value */

    UINT32 tokenLen =0;

    HC_GetNextToken (&pcFieldValue, &tokenLen, HC_EXPECT_URI);

    N_HC_EncodeText (pcFieldValue, tokenLen, pbWspHeader, pnWspHdrLen, nMaxWspHdrLen, FALSE);
    pcFieldValue +=tokenLen;

    return 0;

} /* HC_EncodeFieldValue_URIValue */


HC_STATUS HC_EncodeFieldValue_IntegerValue (
    char*   pcFieldValue,   /* in:  The field value. */
    BYTE*   pbWspHeader,    /* out: Pointer to the encoded WSP header. */
    UINT32* pnWspHdrLen,    /* in:  The actual length of the WSP header.
                               out: The new length of the WSP header. It can grow out the 
                                    size of the pbWspHeader buffer */
    UINT32  nMaxWspHdrLen)  /* in:  The maximum length of the pbWspHeader binary buffer, 
                                    allocated by the user */
{
    /* Integer-value */

    UINT32 tokenLen=0;
    UINT32 uInt;

    HC_GetNextToken (&pcFieldValue, &tokenLen, HC_EXPECT_URI);
    CHECK_ERROR(N_StrtoUINT32 (pcFieldValue, tokenLen, &uInt));
    /* todo : nMaxWspHdrLen check -> new function */
    HC_EncodeInteger (uInt, pbWspHeader, pnWspHdrLen);
    pcFieldValue +=tokenLen;

    return 0;

} /* HC_EncodeFieldValue_IntegerValue */

HC_STATUS HC_EncodeFieldValue_AcceptApplicationValue (
    char*   pcFieldValue,   /* in:  The field value. */
    BYTE*   pbWspHeader,    /* out: Pointer to the encoded WSP header. */
    UINT32* pnWspHdrLen,    /* in:  The actual length of the WSP header.
                               out: The new length of the WSP header. It can grow out the 
                                    size of the pbWspHeader buffer */
    UINT32  nMaxWspHdrLen)  /* in:  The maximum length of the pbWspHeader binary buffer, 
                                    allocated by the user */
{
    /* Accept-application-value  = Any-application | Application-id-value 
       Any-application = <Octet 128> */

    UINT32 tokenLen =0;

    HC_GetNextToken (&pcFieldValue, &tokenLen, HC_EXPECT_URI);
    /* Accept-application-value */
    if (*pcFieldValue == '*' && tokenLen == 1) {
        /* Any-application */

        N_HC_EncodeOctet (0x80, pbWspHeader, pnWspHdrLen, nMaxWspHdrLen);
        pcFieldValue +=tokenLen;
    } else {
        HC_EncodeFieldValue_ApplicationIdValue(pcFieldValue, pbWspHeader, pnWspHdrLen, nMaxWspHdrLen);
    }

    return 0;

} /* HC_EncodeFieldValue_IntegerValue */

HC_STATUS HC_EncodeFieldValue_ShortIntegerValue (
    char*   pcFieldValue,   /* in:  The field value. */
    BYTE*   pbWspHeader,    /* out: Pointer to the encoded WSP header. */
    UINT32* pnWspHdrLen,    /* in:  The actual length of the WSP header.
                               out: The new length of the WSP header. It can grow out the 
                                    size of the pbWspHeader buffer */
    UINT32  nMaxWspHdrLen)  /* in:  The maximum length of the pbWspHeader binary buffer, 
                                    allocated by the user */
{
    /* Short-integer-value */

    UINT32 tokenLen=0;
    UINT32 uInt;

    HC_GetNextToken (&pcFieldValue, &tokenLen, HC_EXPECT_URI);
    CHECK_ERROR(N_StrtoUINT32 (pcFieldValue, tokenLen, &uInt));
    if (uInt >= 128) {
        /* Short integer is in [0,127] */
        return 1;
    }
    
    /* todo : nMaxWspHdrLen check -> new function */
    HC_EncodeShortInt (uInt, pbWspHeader, pnWspHdrLen);
     pcFieldValue +=tokenLen;

    return 0;

} /* HC_EncodeFieldValue_IntegerValue */

HC_STATUS HC_EncodeFieldValue_ProfileWarningValue (
    EncodeData* EncodeState,
    char*   pcFieldValue,   /* in:  The field value. */
    BYTE*   pbWspHeader,    /* out: Pointer to the encoded WSP header. */
    UINT32* pnWspHdrLen,    /* in:  The actual length of the WSP header.
                               out: The new length of the WSP header. It can grow out the 
                                    size of the pbWspHeader buffer */
    UINT32  nMaxWspHdrLen)  /* in:  The maximum length of the pbWspHeader binary buffer, 
                                    allocated by the user */
{
    /* Profile-warning-value = Warn-code | (Value-length Warn-code Warn-target *Warn-date) 
       Warn-code = Short-integer 
       Warn-target = Uri-value 
       Warn-date   = Date-value */


    UINT32 tokenLen=0;
    UINT32 uInt;
    UINT32 warn_code;
    UINT32 nErrorCode;
    UINT32 original_len = *pnWspHdrLen;

    /* Warn-code */
    HC_GetNextToken (&pcFieldValue, &tokenLen, HC_NEXT_TOKEN);
    CHECK_ERROR(N_StrtoUINT32 (pcFieldValue, tokenLen, &uInt));
    switch (uInt) {
        case 100: warn_code = 0x10; break;
        case 101: warn_code = 0x11; break;
        case 102: warn_code = 0x12; break;
        case 200: warn_code = 0x20; break;
        case 201: warn_code = 0x21; break;
        case 202: warn_code = 0x22; break;
        case 203: warn_code = 0x23; break;
        default: 
            return 1; 
            break;
    }
    /* todo : nMaxWspHdrLen check -> new function */
    HC_EncodeShortInt (warn_code, pbWspHeader, pnWspHdrLen);
    pcFieldValue +=tokenLen;
    
    /* Warn-target */
    HC_GetNextToken (&pcFieldValue, &tokenLen, HC_EXPECT_URI);
    if (tokenLen > 0) 
    {
        nErrorCode = HC_EncodeFieldValue_URIValue(pcFieldValue, pbWspHeader, pnWspHdrLen, nMaxWspHdrLen);
        if (nErrorCode != WPS_HC_STATUS_OK) {
            return nErrorCode;
        }
        pcFieldValue +=tokenLen;
    
        /* Warn-Text */
        /* See [WAGUAPROF 9.1.3] In the CC/PP-WSP Profile-Warning response header, the warning
        text is not included */
        HC_GetNextToken (&pcFieldValue, &tokenLen, HC_QUOTED_TEXT);
        pcFieldValue +=tokenLen;
    
        /* Warn-Date */
        /* optional */
        HC_GetNextToken (&pcFieldValue, &tokenLen, HC_OPTIONAL_TOKEN | HC_QUOTED_TEXT);
    
        if (tokenLen !=0 ) {
            HC_EncodeFieldValue_Date (EncodeState, pcFieldValue, NON_DATE_IS_ERROR, pbWspHeader, pnWspHdrLen, FALSE);
            pcFieldValue +=tokenLen;
        }

        /* Encode the length of the value */
        {
            /* Length of the encoded value */
            UINT32 nFieldValueLength = *pnWspHdrLen - original_len;
            BYTE   pcEncodedLength[10]; /* The binary encoded length */
            UINT32 nLenOfTheLen = 0;    /* The actual length of the binary encoded length */

            /* Encode the length */
            N_HC_EncodeLength (nFieldValueLength, pcEncodedLength, &nLenOfTheLen, 10);
            *pnWspHdrLen += nLenOfTheLen;

            /* insert the binary length into the binary buffer */
            if (*pnWspHdrLen <= nMaxWspHdrLen) 
            {
                memmove (pbWspHeader + original_len + nLenOfTheLen, pbWspHeader+original_len, nFieldValueLength);
                memcpy  (pbWspHeader + original_len, pcEncodedLength, nLenOfTheLen);
            }
        }


    }    
    return 0;
} /* HC_EncodeFieldValue_ProfileWarning */


/*
-----------------------------------------------------------------------------
    Appends the given field name to the WSP buffer (if the options don't 
    specify differently), then extracts the next field value 
    from the HTTP header, encodes it according to the rules associated with 
    the specific header field, finally appends the encoded data to 
    the WSP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeField(
    EncodeData*     EncodeState,
    BYTE*           pbActCodePage,  /* in:      Previous header code page number.
                                       out:     Actual header code page number. */
    BYTE            bCodePage,      /* in:      Header code page number for this field. */
    BYTE            bFieldCode,     /* in:      Header field code. */
    BITF            bFlags,         /* in:      Option flags. */
    const char**    ppcHttpHeader,  /* in:      Pointer to the actual location in the HTTP header.
                                       out:     Pointer to the new location in the HTTP header. */
    BYTE*           pbWspHeader,    /* out:     The encoded WSP header. */
    UINT32*         pnWspHdrLen,    /* in:      The actual length of the WSP header.
                                       out:     The new length of the WSP header. */
    UINT32*         pnMaxWspHdrLen)    
{
    char        acBuf   [WPS_CODEC_BUF_SIZE];
    int         nLen    = WPS_CODEC_BUF_SIZE;
    BOOL        bMHMV   = HC_MayHaveMultipleValues (bCodePage, bFieldCode);
    HC_STATUS   nRet    = HC_ExtractFieldValue (ppcHttpHeader, bMHMV, acBuf, &nLen);
    BYTE        bValue;
    UINT32      nNum    = 0;

    /* For saving the pointer before encoding */
    UINT32 entryWspHeaderIndex;

    if (nRet == WPS_HC_STATUS_OK)
    {
        if (bFieldCode != WPS_HFC_DEF_CONTENT_TYPE)
        { /* Common case: append the field code first */
            nRet = HC_EncodeFieldCode (pbActCodePage, bCodePage, bFieldCode, 
                                       pbWspHeader, pnWspHdrLen);

            /* Save the pointer before the encoding */
            entryWspHeaderIndex = *pnWspHdrLen;

        }
        if (nRet == WPS_HC_STATUS_OK)
        {
            switch (bFieldCode)
            {
            case WPS_HFC_DEF_ACCEPT                  :
                nRet = HC_EncodeFieldValue_Accept (EncodeState,acBuf, pbWspHeader, 
                                                   pnWspHdrLen);
                break;
            case WPS_HFC_DEF_ACCEPT_CHARSET          :
                nRet = HC_EncodeFieldValue_AcceptCharset (EncodeState,acBuf, pbWspHeader, 
                                                          pnWspHdrLen);
                break;
            case WPS_HFC_DEF_ACCEPT_ENCODING         :
            case WPS_HFC_DEF_CONTENT_ENCODING        :
                if (HC_Name2Byte_Unord (g_aContentEncodingValues, 
                                        DIM (g_aContentEncodingValues), 
                                        TRUE, acBuf, &bValue))
                {
                    nRet = HC_EncodeOctet (bValue, pbWspHeader, 
                                           pnWspHdrLen);
                }
                else
                { /* Unknown value */
                    nRet = HC_EncodeText (acBuf, pbWspHeader, pnWspHdrLen, 
                                          TRUE);
                }
                break;
            case WPS_HFC_DEF_ACCEPT_LANGUAGE         :
                nRet = HC_EncodeFieldValue_AcceptLanguage (EncodeState,acBuf, pbWspHeader, 
                                                           pnWspHdrLen);
                break;
            case WPS_HFC_DEF_ACCEPT_RANGES           :
                if (HC_Name2Byte_Unord (g_aAcceptRangesValues, 
                                        DIM (g_aAcceptRangesValues), 
                                        TRUE, acBuf, &bValue))
                {
                    nRet = HC_EncodeOctet (bValue, pbWspHeader, 
                                           pnWspHdrLen);
                }
                else
                { /* Unknown value */
                    nRet = HC_EncodeText (acBuf, pbWspHeader, pnWspHdrLen, 
                                          TRUE);
                }
                break;
            case WPS_HFC_DEF_AGE                     : /* Delta-seconds-value */
                if (StrtoUINT32(acBuf, &nNum, '\0')!=WPS_HC_STATUS_OK)
                {
                    EncodeState->Warnings |= WPS_HC_WARNING_NUMBER_TRUNCATED;
                }
                if (nNum > 2147483648U) {
                    EncodeState->Warnings |= WPS_HC_WARNING_NUMBER_TRUNCATED;
                    nNum = 2147483648U;
                }
                nRet = HC_EncodeInteger (nNum, pbWspHeader, pnWspHdrLen);

                break;
            case WPS_HFC_DEF_CONTENT_LENGTH          : /* Integer-value */
            case WPS_HFC_DEF_MAX_FORWARDS            : /* Integer-value */
                if (StrtoUINT32(acBuf, &nNum, '\0')!=WPS_HC_STATUS_OK)
                {
                    EncodeState->Warnings |= WPS_HC_WARNING_NUMBER_TRUNCATED;
                }
                nRet = HC_EncodeInteger (nNum, pbWspHeader, pnWspHdrLen);

                break;
            case WPS_HFC_DEF_ALLOW                   :
                nRet = HC_EncodeMethodName (acBuf, TRUE, pbWspHeader, 
                                            pnWspHdrLen);
                break;
            case WPS_HFC_DEF_AUTHORIZATION           :
            case WPS_HFC_DEF_PROXY_AUTHORIZATION     : /* Value-length Credentials */
                nRet = HC_EncodeFieldValue_Authorization (acBuf, pbWspHeader, 
                                                          pnWspHdrLen);
                break;
            case WPS_HFC_DEF_CACHE_CONTROL           :
                nRet = HC_EncodeFieldValue_CacheControl (EncodeState,acBuf, pbWspHeader, 
                                                         pnWspHdrLen);
                break;
            case WPS_HFC_DEF_CONNECTION              :
                if (HC_stricmp2 (acBuf, WPS_HC_FV_CONNECTION_CLOSE) == 0)
                {
                    nRet = HC_EncodeOctet (128, pbWspHeader, pnWspHdrLen);
                }
                else
                { /* Unknown value */
                    nRet = HC_EncodeText (acBuf, pbWspHeader, pnWspHdrLen, 
                                          TRUE);
                }
                break;
            case WPS_HFC_DEF_CONTENT_DISPOSITION     :
                nRet = HC_EncodeFieldValue_ContentDisposition (EncodeState,acBuf, pbWspHeader, 
                                                               pnWspHdrLen);
                                                               
                break;
            case WPS_HFC_DEF_CONTENT_LANGUAGE        :
                {
                    WORD    wLangCode;

                    if (HC_LangName2LangCode (acBuf, EncodeState->WapVersion, &wLangCode))
                    {
                        nRet = HC_EncodeInteger (wLangCode, pbWspHeader, 
                                                 pnWspHdrLen);
                    }
                    else
                    {   /* Unknown language - add as text */
                        nRet = HC_EncodeText (acBuf, pbWspHeader, 
                                              pnWspHdrLen, TRUE);
                    }
                }
                break;
            case WPS_HFC_DEF_CONTENT_MD5             :
                nRet = HC_EncodeFieldValue_ContentMD5 (acBuf, pbWspHeader, 
                                                       pnWspHdrLen);
                break;
            case WPS_HFC_DEF_CONTENT_RANGE           :
                nRet = HC_EncodeFieldValue_ContentRange (acBuf, pbWspHeader, 
                                                         pnWspHdrLen);
                break;
            case WPS_HFC_DEF_CONTENT_TYPE            :
                nRet = HC_EncodeField_ContentType (EncodeState,pbActCodePage, bCodePage, 
                                                   bFlags, acBuf, pbWspHeader, 
                                                   pnWspHdrLen);
                break;
            case WPS_HFC_DEF_EXPIRES                 :
                nRet = HC_EncodeFieldValue_Date (EncodeState,acBuf, NON_DATE_IS_ERROR, pbWspHeader, 
                                                 pnWspHdrLen, TRUE);
                break;
            case WPS_HFC_DEF_DATE                    :
            case WPS_HFC_DEF_IF_MODIFIED_SINCE       :
            case WPS_HFC_DEF_IF_UNMODIFIED_SINCE     :
            case WPS_HFC_DEF_LAST_MODIFIED           : /* Date-value */
                nRet = HC_EncodeFieldValue_Date (EncodeState, acBuf, NON_DATE_IS_ERROR, pbWspHeader, 
                                                 pnWspHdrLen, FALSE);
                break;
            case WPS_HFC_DEF_IF_RANGE                :
                nRet = HC_EncodeFieldValue_Date (EncodeState,acBuf, NON_DATE_IS_TEXT, pbWspHeader, 
                                                 pnWspHdrLen,FALSE);
                break;
            case WPS_HFC_DEF_PRAGMA                  :
                nRet = HC_EncodeFieldValue_Pragma (EncodeState,acBuf, pbWspHeader, 
                                                   pnWspHdrLen);
                break;
            case WPS_HFC_DEF_PROXY_AUTHENTICATE      :
            case WPS_HFC_DEF_WWW_AUTHENTICATE        : /* Value-length Challenge */
                nRet = HC_EncodeFieldValue_ProxyAuthenticate (acBuf, pbWspHeader, 
                                                              pnWspHdrLen, *pnMaxWspHdrLen);
                /* Because of CHECK_ERROR macro, the error value must be assigned here */
                if (nRet != 0)
                    nRet = WPS_HC_ERROR_HTTP_BAD_AUTHENTICATE;
                break;
            case WPS_HFC_DEF_PUBLIC                  :
                nRet = HC_EncodeMethodName (acBuf, FALSE, pbWspHeader, 
                                            pnWspHdrLen);
                break;
            case WPS_HFC_DEF_RANGE                   :
                nRet = HC_EncodeFieldValue_Range (EncodeState, acBuf, pbWspHeader, 
                                                  pnWspHdrLen);
                break;
            case WPS_HFC_DEF_RETRY_AFTER             :
                nRet = HC_EncodeFieldValue_RetryAfter (EncodeState,acBuf, pbWspHeader, 
                                                       pnWspHdrLen);
                break;
            case WPS_HFC_DEF_TRANSFER_ENCODING       :
                if (HC_stricmp2 (acBuf, WPS_HC_FV_TRANSFER_ENC_CHUNKED) == 0)
                {
                    nRet = HC_EncodeOctet (128, pbWspHeader, 
                                           pnWspHdrLen);
                }
                else
                { /* Unknown value */
                    nRet = HC_EncodeText (acBuf, pbWspHeader, 
                                          pnWspHdrLen, TRUE);
                }
                break;
            case WPS_HFC_DEF_VARY                    :
                nRet = HC_EncodeWellknownFieldName (acBuf, 
                                                    EncodeState->WapVersion,
                                                    pbWspHeader, 
                                                    pnWspHdrLen);
                break;
            case WPS_HFC_DEF_VIA                     :
                nRet = HC_EncodeFieldValue_Via (acBuf, pbWspHeader, 
                                                pnWspHdrLen);
                break;
            case WPS_HFC_DEF_WARNING                 :
                nRet = HC_EncodeFieldValue_Warning (acBuf, pbWspHeader, 
                                                    pnWspHdrLen);
                break;
            case WPS_HFC_DEF_CONTENT_BASE            : /* Uri-value */
            case WPS_HFC_DEF_CONTENT_LOCATION        : /* Uri-value */
            case WPS_HFC_DEF_ETAG                    : /* Text-string */
            case WPS_HFC_DEF_FROM                    : /* Text-string */
            case WPS_HFC_DEF_HOST                    : /* Text-string */
            case WPS_HFC_DEF_IF_MATCH                : /* Text-string */
            case WPS_HFC_DEF_IF_NONE_MATCH           : /* Text-string */
            case WPS_HFC_DEF_LOCATION                : /* Uri-value */
            case WPS_HFC_DEF_REFERER                 : /* Uri-value */
            case WPS_HFC_DEF_SERVER                  : /* Text-string */
            case WPS_HFC_DEF_UPGRADE                 : /* Text-string */
            case WPS_HFC_DEF_USER_AGENT              : /* Text-string */
                nRet = HC_EncodeText (acBuf, pbWspHeader, 
                                      pnWspHdrLen, FALSE);
                break;

            default:
                if (EncodeState->WapVersion >= HC_WapVersion1_2) {
                    switch (bFieldCode)
                    {
                    case WPS_HFC_DEF_X_WAP_APPLICATION_ID:
                        nRet = HC_EncodeFieldValue_ApplicationIdValue 
                            (acBuf, pbWspHeader, pnWspHdrLen, *pnMaxWspHdrLen);
                        break;

                    case WPS_HFC_DEF_X_WAP_CONTENT_URI:
                    case WPS_HFC_DEF_X_WAP_INITIATOR_URI:
                    case WPS_HFC_DEF_PROFILE:
                        nRet = HC_EncodeFieldValue_URIValue 
                            (acBuf, pbWspHeader, pnWspHdrLen, *pnMaxWspHdrLen);
                        break;

                    case WPS_HFC_DEF_ACCEPT_APPLICATION:
                        nRet = HC_EncodeFieldValue_AcceptApplicationValue
                            (acBuf, pbWspHeader, pnWspHdrLen, *pnMaxWspHdrLen);
                        break;

                    case WPS_HFC_DEF_BEARER_INDICATION:
                        nRet = HC_EncodeFieldValue_IntegerValue
                            (acBuf, pbWspHeader, pnWspHdrLen, *pnMaxWspHdrLen);
                        break;

                    case WPS_HFC_DEF_PUSH_FLAG:
                        nRet = HC_EncodeFieldValue_ShortIntegerValue 
                            (acBuf, pbWspHeader, pnWspHdrLen, *pnMaxWspHdrLen);
                        break;

                    case WPS_HFC_DEF_PROFILE_DIFF:
                        /* Not implemented yet */
                        nRet = INTERNAL_RET_SKIP_FIELD;
                        break;

                    case WPS_HFC_DEF_PROFILE_WARNING:
                        nRet = HC_EncodeFieldValue_ProfileWarningValue 
                            (EncodeState,acBuf, pbWspHeader, pnWspHdrLen, *pnMaxWspHdrLen);
                        break;

                    default:

                        /* Encode as a tet string */
                        nRet = HC_EncodeText (acBuf, pbWspHeader, 
                                              pnWspHdrLen, FALSE);
                    }
                } else {

                    /* Encode as a tet string */
                    nRet = HC_EncodeText (acBuf, pbWspHeader, 
                                          pnWspHdrLen, FALSE);
                }
            }
            EncodeState->LastFieldWasRange = (bFieldCode == WPS_HFC_DEF_RANGE);

        }
    }
    return nRet;
} /* HC_EncodeField */

/*
-----------------------------------------------------------------------------
    API implementation
-----------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeHeader(
    const char*         pcHttpHeader, 
    const NameBytePair* pCodePages, 
    UINT8               nNumCodePages, 
    BITF                bFlags, 
    HC_WapVersion       wapVersion,
    BYTE*               pbWspHeader, 
    UINT32*             pnWspHdrLen)
{
    HC_STATUS   nRet            = WPS_HC_STATUS_OK; 
    const char* pcLine          = pcHttpHeader;
    const char* pcAct           = NULL;
    BYTE        bActCodePage    = WPS_HCP_DEFAULT;
    char        acTmpBuf        [WPS_CODEC_BUF_SIZE];
    BYTE        abDstBuf        [WPS_CODEC_BUF_SIZE];
    UINT32      nDstBufIdx      = 0;
    int         nFieldNameLen   = 0;
    BYTE        bFieldCode;
    BOOL        bFound          = FALSE;
    char        *pcTmp          = NULL;
    EncodeData  EncodeState;
    UINT32      OldBufIndex;
    
    EncodeState.LastFieldWasRange = FALSE;
    EncodeState.WasContentType = FALSE;
    EncodeState.FirstError       = WPS_HC_STATUS_OK;
    EncodeState.Warnings = WPS_HC_STATUS_OK;
    EncodeState.WapVersion = wapVersion;

    /* Sanity check */
    if (pcHttpHeader==NULL || pbWspHeader == NULL || pnWspHdrLen == NULL) {
        return WPS_CODEC_ERROR_INVALID_PARAMETER;
    }

    while ( nRet == WPS_HC_STATUS_OK && 
            pcLine && 
            *pcLine && 
            !strchr (WPS_HC_NEWLINE, *pcLine) )
    {
        BYTE    bCodePage   = bActCodePage;

        OldBufIndex = nDstBufIdx;

        if (*pcLine != WPS_HC_VALUE_SEPARATOR)
        {   /* A new field is coming */
            pcAct = strchr (pcLine, WPS_HC_FIELD_SEPARATOR);
            pcTmp = strstr (pcLine, "\n");
            if (pcTmp < pcAct && pcTmp != NULL)
            {
                pcAct = pcTmp;
                pcTmp = NULL;
            }

            if (pcAct)
            { /* Found the field name - now extract it */
                int i;
                        
                /* Throw out the whitespaces between the field name and the delimiter */
                nFieldNameLen = strcspn(pcLine,WPS_HC_WHITESPACES);
                if (pcAct - pcLine < nFieldNameLen)
                    nFieldNameLen = pcAct - pcLine;


                /* We need nFieldnameLen+1 because of the trailing zero - 2000.03.07 */
                if (nFieldNameLen + 1 < WPS_CODEC_BUF_SIZE)
                {
                    strncpy (acTmpBuf, pcLine, nFieldNameLen);
                    acTmpBuf[nFieldNameLen] = '\0';

                    /* Get the field code */
                    bFound = HC_FieldName2FieldCode (bCodePage, NULL, 
                                                     acTmpBuf, wapVersion,
                                                     &bFieldCode);
                    if (!bFound)
                    { /* Field is not found in the actual code page - try the WAP code pages */
                        for ( i = WPS_HCP_WAP_FIRST, bFound = FALSE; 
                              i <= WPS_HCP_WAP_LAST; 
                              i++ )
                        {
                            bFound = HC_FieldName2FieldCode(i, NULL, acTmpBuf,
                                                            wapVersion, 
                                                            &bFieldCode);
                            if (bFound)
                            { /* Store the number of the code page containing the field */
                                bCodePage = (BYTE)i;
                            }
                        }
                        if (!bFound)
                        { /* Field is not found in the WAP headers - try the app-specific ones */
                            for (i = 0, bFound = FALSE; 
                                 i < nNumCodePages && !bFound; 
                                 i++)
                            {
                                bFound = HC_FieldName2FieldCode (0, pCodePages[i].i_pcName, 
                                                                 acTmpBuf, wapVersion,
                                                                 &bFieldCode);
                                if (bFound)
                                { /* Store the number of the code page containing the field */
                                    bCodePage = pCodePages[i].i_bValue;
                                }
                            }
                        }
                    }
                }
                else
                { /* Error - buffer is too short for field name! */
                    nRet = WPS_CODEC_ERROR_INT_BUFFER_TOO_SMALL;
                }
            }
            else
            { /* Error - no separator in the field! */
                SetError(&EncodeState,WPS_HC_ERROR_HTTP_NO_FIELD_SEPARATOR);
                SetWarning(&EncodeState,WPS_HC_WARNING_MALFORMED_HEADER_SKIPPED);
                pcLine = HC_SkipLine(pcLine);
                continue;
            }
        } 
        
        if (bFound && nRet == WPS_HC_STATUS_OK)
        { /* Field is well-known - encode it according to the rules */
            if (bCodePage == WPS_HCP_DEFAULT)
            { /* Field is on the default page - there are rules for it */

                UINT32  nMaxWspHdrLen;
                nMaxWspHdrLen = *pnWspHdrLen; /* Is it fix ?? */

                nRet = HC_EncodeField (&EncodeState, &bActCodePage, bCodePage, 
                                       bFieldCode, bFlags, &pcAct, 
                                       abDstBuf, &nDstBufIdx, &nMaxWspHdrLen);
                /* If something unusual has happened */
                
                /* If the field is special and to be skipped */
                if (nRet == INTERNAL_RET_SKIP_FIELD) {
                    nDstBufIdx = OldBufIndex;
                    pcLine = HC_SkipLine(pcLine);
                    SetWarning(&EncodeState,WPS_HC_WARNING_UNKNOWN_HEADER_SKIPPED);
                    nRet = WPS_HC_STATUS_OK;
                }

                if (nRet != WPS_HC_STATUS_OK) {
                    if (WPS_HC_STATUS_IS_ERROR(nRet)) {
                        SetError(&EncodeState,nRet);
                        SetWarning(&EncodeState,WPS_HC_WARNING_MALFORMED_HEADER_SKIPPED);
                    }
                    else
                        SetWarning(&EncodeState,nRet);
                    
                    if ( (!(bFlags & WPS_HC_OPT_STOP_AT_ERROR)) || WPS_HC_STATUS_IS_WARNING(nRet)) {
                        nDstBufIdx = OldBufIndex;
                        nRet = WPS_HC_STATUS_OK;
                        pcLine = HC_SkipLine(pcLine);
                        continue;
                    }
                }
                        

            }
            else
            { /* Add field value as text */
                nRet = HC_EncodeFieldCode (&bActCodePage, bCodePage, bFieldCode, 
                                           abDstBuf, &nDstBufIdx);
                if (nRet == WPS_HC_STATUS_OK)
                {
                    nRet = HC_EncodeFieldValueAsText (&pcAct, abDstBuf, 
                                                      &nDstBufIdx);
                }
            }
        }
        else
        { /* Field is unknown - encode it as text */
            if (nRet == WPS_HC_STATUS_OK && pcAct == NULL)
                nRet = 1;
         
            if (nDstBufIdx + nFieldNameLen < WPS_CODEC_BUF_SIZE)
            { /* Field value fits into the buffer - copy it */
                strcpy ((char*)(abDstBuf + nDstBufIdx), acTmpBuf);
                nDstBufIdx += nFieldNameLen + 1;
            }
            else
            { /* Error - WSP buffer is too small! */
                nRet = WPS_CODEC_ERROR_INT_BUFFER_TOO_SMALL;
            }
            if (nRet == WPS_HC_STATUS_OK)
            {
                nRet = HC_EncodeFieldValueAsText (&pcAct, abDstBuf, 
                                                  &nDstBufIdx);
            }
        }

        pcLine = pcAct;
    }


    /* Check if the content-type is included when OPTION_NONE is selected */
    if (!(EncodeState.WasContentType) && ((bFlags & WPS_HC_OPT_DONT_SEPARATE_CONTENTTYPE) == 0) ) {
        if (nDstBufIdx + 1 <= *pnWspHdrLen) {
            pbWspHeader[0] = 0;
            memcpy(pbWspHeader+1, abDstBuf, nDstBufIdx);
            nDstBufIdx++;
        } else {
            nRet = WPS_CODEC_ERROR_DEST_BUFFER_TOO_SMALL;
            nDstBufIdx++;
        }
    /* If DONT_SEPARATE was selected or content-type is already included */
    } else {
        if (nDstBufIdx <= *pnWspHdrLen)
        {   /* Result fits in the destination buffer - copy it */
          memcpy (pbWspHeader, abDstBuf, nDstBufIdx);
        } else {   
        /* Error - the given WSP buffer is too small! */
        nRet = WPS_CODEC_ERROR_DEST_BUFFER_TOO_SMALL;
        }
    }
    
    *pnWspHdrLen = nDstBufIdx;

    /* If no error in the outside loop - look for the inner errors */
    if (nRet == WPS_HC_STATUS_OK) {
        if (bFlags & WPS_HC_OPT_PREFER_ERROR) {
            if (EncodeState.FirstError != WPS_HC_STATUS_OK)
                nRet = EncodeState.FirstError;
            else
                nRet = EncodeState.Warnings;
        } else {
            nRet = EncodeState.Warnings;
        }
    
        if ((bFlags & WPS_HC_OPT_STOP_AT_ERROR) && EncodeState.FirstError != WPS_HC_STATUS_OK) {
            nRet = EncodeState.FirstError;
        } else {
            nRet = EncodeState.Warnings;
        }

    }

    return nRet;
} /* HC_EncodeHeader */



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
/* File    : T:\wpsvob\wsp\work\codecs\headercodec\hc_decoder.c
   Version : \main\multline\8
   Date    : 02-May-00.15:58:02
*/

/*
 * ========================================================================
 *  Name        : hc_decoder.c
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
    INCLUDES
-----------------------------------------------------------------------------
*/

#include "hc_util.h"
#include "hc_pages.h"
#include "other_util.h"

#include <string.h>
#include <limits.h>

/*
-----------------------------------------------------------------------------
    TYPE DEFINITIONS
-----------------------------------------------------------------------------
*/

/* Represents the information used by the header decoder. */
typedef struct _DecodeData
{
    const BYTE* i_pbWspHeader;                  /* The WSP header. */
    UINT32      i_nWspHdrPos;                   /* The actual position in the WSP header. */
    UINT32      i_nWspHdrLen;                   /* The full length of the WSP header. */
    UINT32      i_nWspHdrActLen;                /* The actual Wsp Header length */
    UINT32      i_nWspHdrDecLen;                /* Decoded length of the Wsp header */
    char        i_acHttpBuf[WPS_CODEC_BUF_SIZE];/* The destination HTTP buffer. */
    UINT32      i_nHttpBufPos;                  /* The actual length of the HTTP buffer. */
    UINT32      i_nContentLen;                  /* The content length (see WSP 8.4.2.23). */
    HC_STATUS   i_nWarnings;                    /* The warnings collected during decoding. */
    UINT16      i_nLastFieldCode;               /* The last field code (for decoding multiple values). */
    UINT16      i_nLastValidFieldCode;          /* The before-last field code 
                                                  (for skipping invalid, but repeated header fields */
    HC_STATUS   i_nFirstError;                  /* The first encountered error during decoding */
    HC_WapVersion i_version;                    /* version to comply with */
} DecodeData;

/*
-----------------------------------------------------------------------------
    LOCAL HELPERS
-----------------------------------------------------------------------------
*/

/* Temporary  defines here */
/* ------------------------------------------ */

/*  WSP error: Unknown warning code (as in profile-warning) */
#define WPS_HC_ERROR_HTTP_UNKNOWN_WARNING_CODE       (WPS_HC_STATUS_BASE + 59)

/*  WSP error: Invalid parameter */
#define WPS_HC_ERROR_WSP_INVALID_PARAMETER          (WPS_HC_STATUS_BASE + 123)
/*  WSP error: Unknown warning code (as in profile-warning) */
#define WPS_HC_ERROR_WSP_UNKNOWN_WARNING_CODE       (WPS_HC_STATUS_BASE + 125)

/* Passable means, that the return code doesn't indicate severe error. 
 * Included instead of ret == WPS_HC_STATUS_OK, because we would like 
 * to continue in case of warnings 
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

/* Internal defines */

#define HC_MALFORMED_FIELD      0xFFFE

/* Is the actual_code field repeated, or is it a new one (it is importatn in the case, when
   there is a invalid field value between to valid ones, corresponding the same field code 
*/
static BOOL HC_RepeatField(DecodeData* poDecode, UINT16 actual_code, BYTE codepage)
{
    /* To avoid repeating an unrepeatable field with commas */
    if (WPS_HCP_WAP_FIRST <= codepage && codepage <= WPS_HCP_WAP_LAST &&
        !HC_MayHaveMultipleValues(codepage, (BYTE)actual_code))
        return FALSE;

    if (actual_code == poDecode->i_nLastFieldCode ||
         (poDecode->i_nLastFieldCode == HC_MALFORMED_FIELD && actual_code == poDecode->i_nLastValidFieldCode))
         return TRUE;
    else
        return FALSE;
}

/* This short functions just sets the last field code, it is needed because of the comma-separation */
static void HC_SetLastFieldCode(DecodeData* poDecode, UINT16 code)
{
    if (poDecode->i_nLastFieldCode != HC_MALFORMED_FIELD)
        poDecode->i_nLastValidFieldCode = poDecode->i_nLastFieldCode;
    poDecode->i_nLastFieldCode = code;
}

/*
------------------------------------------------
*/

static HC_STATUS SetError(DecodeData* poDecode, HC_STATUS error_code)
{
    if (poDecode->i_nFirstError == WPS_HC_STATUS_OK)
        poDecode->i_nFirstError = error_code;

    return error_code;
}

static void SetWarning(DecodeData* poDecode, HC_STATUS warning_code)
{
    poDecode->i_nWarnings |= warning_code;
}

/*
-----------------------------------------------------------------------------
    Read one byte from the i_nWspHdrPos position if it is possible.
    Otherwise it returns with data overflow. 
    (It doesn't change anything in the poDecode structure.)
-----------------------------------------------------------------------------
*/
HC_STATUS HC_ReadOneByte(DecodeData* poDecode, BYTE* pFirstByte)
{
    HC_STATUS nRet = WPS_HC_STATUS_OK;
    
    if (poDecode->i_nWspHdrActLen + poDecode->i_nWspHdrDecLen >= poDecode->i_nWspHdrPos)
    {
        *pFirstByte = poDecode->i_pbWspHeader [poDecode->i_nWspHdrPos];
    }
    else
    {
        nRet = WPS_HC_ERROR_WSP_DATA_OVERFLOW;
    }

    return (nRet);
}


/*
-----------------------------------------------------------------------------
    Appends the given text to the HTTP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_AppendText(
    DecodeData* poDecode,   /* in, out: Decode data. */
    const char* pcText)     /* in:  The text to append. */
{
    HC_STATUS  nRet    = WPS_HC_STATUS_OK;
    INT32       nLen    = HC_Strnlen (pcText, 0);
    
    if (poDecode->i_nHttpBufPos + nLen < WPS_CODEC_BUF_SIZE)
    {
        strcpy (poDecode->i_acHttpBuf + poDecode->i_nHttpBufPos, pcText);
        poDecode->i_nHttpBufPos += nLen;
    }
    else
    {   /* Error - HTTP buffer is too small! */
        nRet = WPS_CODEC_ERROR_INT_BUFFER_TOO_SMALL;
    }
    return nRet;
} /* HC_AppendText */


#ifdef UNDER_CE
#endif

/*
-----------------------------------------------------------------------------
    BASIC DECODER FUNCTIONS
-----------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------
    Parses an UINT32 out of the WSP buffer. Checks for overflow and if 
    it occurs, returns the largest possible value.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_ParseInteger(
    DecodeData* poDecode,   /* in, out: Decode data. */
    UINT32*     pnValue)    /* out:     Parsed value. */
{
    HC_STATUS  nRet    = WPS_HC_STATUS_OK;
    BYTE       bOctet  = 0;
    
    nRet = HC_ReadOneByte(poDecode, &bOctet);
    if (PASSABLE(nRet))
    {
        poDecode->i_nWspHdrPos++;
        if (OCTET_IS_SHORTINT (bOctet))
        {   /* Short-integer */
            *pnValue = DECODE_SHORTINT (bOctet);
        }
        else if (OCTET_IS_BINARY (bOctet) && bOctet > WPS_CODEC_MIN_BINARY)
        {   /* Long-integer - length of 0 is illegal! */
            UINT32  nPrevValue;
            BOOL    bOverflowed = FALSE;
    
            *pnValue = 0;
            for ( ; (bOctet > 0) && ((poDecode->i_nWspHdrActLen+poDecode->i_nWspHdrDecLen) > poDecode->i_nWspHdrPos);
                bOctet--, poDecode->i_nWspHdrPos++)
            {
                if (!bOverflowed)
                {
                    nPrevValue = *pnValue;
                    *pnValue *= 256;
                    *pnValue += poDecode->i_pbWspHeader [poDecode->i_nWspHdrPos];
                    bOverflowed = (nPrevValue > *pnValue / 256);
                }
            }
            if (bOctet > 0)
            {
                nRet = WPS_HC_ERROR_WSP_DATA_OVERFLOW;
            }
            if ((bOverflowed) && (PASSABLE(nRet)))
            {   /* Overflow - set a warning and return the largest possible value */
                poDecode->i_nWarnings |= WPS_HC_WARNING_NUMBER_TRUNCATED;
                *pnValue = ULONG_MAX;
            }
            if (poDecode->i_nWspHdrPos > poDecode->i_nWspHdrLen)
            {   /* Error - Data overflow! */
                nRet = WPS_HC_ERROR_WSP_DATA_OVERFLOW;
            }
        }
        else
        {   /* Error - text value! */
            nRet = WPS_HC_ERROR_WSP_INTEGER_EXPECTED;
        }
    }
    return nRet;
} /* HC_ParseInteger */

/*
-----------------------------------------------------------------------------
    Decodes the actual value as text and appends 
    the decoded data to the HTTP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_DecodeText(
    DecodeData* poDecode,       /* in, out: Decode data. */
    BOOL        bMayBeQuoted,   /* in:      TRUE if the string may be quoted, otherwise FALSE. */
    BOOL        bCheckEmpty)    /* in:      TRUE if empty string is error, otherwise FALSE. */
{
    HC_STATUS   nRet    = WPS_HC_STATUS_OK;
    const char* pcTmp   = (char*)poDecode->i_pbWspHeader + poDecode->i_nWspHdrPos;
    INT32       nLen    = HC_Strnlen (pcTmp, poDecode->i_nWspHdrActLen + poDecode->i_nWspHdrDecLen - poDecode->i_nWspHdrPos);

    if (nLen==-1)
    {   /* Error - Data overflow! */
        nRet = WPS_HC_ERROR_WSP_DATA_OVERFLOW;
    }
    if (PASSABLE(nRet))
    {
        poDecode->i_nWspHdrPos += nLen + 1;
        if (*pcTmp == WPS_CODEC_NON7BIT_QUOTE)
        {   /* Skip the non-7bit-quote byte */
            pcTmp++;
            nLen--;
        }
        if (bCheckEmpty && nLen == 0)
        {   /* Error - Empty text string! */
            nRet = WPS_HC_ERROR_WSP_EMPTY_TEXT;
        }
        else if (poDecode->i_nWspHdrPos > poDecode->i_nWspHdrLen)
        {   /* Error - Data overflow! */
            nRet = WPS_HC_ERROR_WSP_DATA_OVERFLOW;
        }
        else if (poDecode->i_nHttpBufPos + nLen < WPS_CODEC_BUF_SIZE)
        {
            strcpy (poDecode->i_acHttpBuf + poDecode->i_nHttpBufPos, pcTmp);
            poDecode->i_nHttpBufPos += nLen;
            if (bMayBeQuoted && *pcTmp == '\"')
            {   /* Quoted string - append the closing quote */
                if (poDecode->i_nHttpBufPos + 1 < WPS_CODEC_BUF_SIZE)
                {
                    poDecode->i_acHttpBuf [poDecode->i_nHttpBufPos++] = '\"';
                    poDecode->i_acHttpBuf [poDecode->i_nHttpBufPos] = '\0';
                }
                else
                {   /* Error - HTTP buffer is too small! */
                    nRet = WPS_CODEC_ERROR_INT_BUFFER_TOO_SMALL;
                }
            }
        }
        else
        {   /* Error - HTTP buffer is too small! */
            nRet = WPS_CODEC_ERROR_INT_BUFFER_TOO_SMALL;
        }
    }
    return nRet;
} /* HC_DecodeText */

/*
-----------------------------------------------------------------------------
    Decodes the actual value as integer and appends 
    the decoded data to the HTTP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_DecodeInteger(
    DecodeData* poDecode)   /* in, out: Decode data. */
{
    HC_STATUS  nRet    = WPS_HC_STATUS_OK;
    UINT32     nValue;

    nRet = HC_ParseInteger (poDecode, &nValue);
    if (PASSABLE(nRet))
    {   /* Append integer value to the HTTP buffer */
        char    acBuf   [WPS_CODEC_BUF_SIZE];

        nRet = UINT32toStr(nValue, acBuf, WPS_CODEC_BUF_SIZE);
        HC_AppendText (poDecode, acBuf);
    }
    return nRet;
} /* HC_DecodeInteger */

/*
-----------------------------------------------------------------------------
    Parses a length value out of the WSP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_DecodeLength(
    DecodeData* poDecode,   /* in, out: Decode data. */
    UINT32*     pnLength,   /* out:     Parsed length. */
    BOOL        b_InHdr)
{
    HC_STATUS  nRet    = WPS_HC_STATUS_OK;
    BYTE       bOctet  = 0;
    
    if (b_InHdr)
    {   /* The WspHdr exists */
        nRet = HC_ReadOneByte(poDecode, &bOctet);
        poDecode->i_nWspHdrPos++;
    }
    else
    {
        /* The HC_ReadOneByte function can't be used for read a 
        byte, because the WspHdrActLen doesn't exist */
        if (poDecode->i_nWspHdrPos < poDecode->i_nWspHdrLen)
        {
            bOctet = poDecode->i_pbWspHeader [poDecode->i_nWspHdrPos];
            poDecode->i_nWspHdrPos++;
        }
        else
        {
            nRet = WPS_HC_ERROR_WSP_DATA_OVERFLOW;
        }
    }   

    /* Ther is no WspHdrActLen length yet. Only the whole length can be checked. */

    if (poDecode->i_nWspHdrLen <= (poDecode->i_nWspHdrPos-1))
    {
        nRet = WPS_HC_ERROR_WSP_DATA_OVERFLOW;
    }
    
    if (PASSABLE(nRet))
    {

        *pnLength = 0;
        if (bOctet < WPS_HC_LENGTH_QUOTE)
        {   /* Short-length*/
            *pnLength = DECODE_SHORTINT (bOctet);
        }
        else if (bOctet == WPS_HC_LENGTH_QUOTE)
        {   /* Length-quote Length */
            if (b_InHdr==TRUE)
            {   /* If the Actual length the header value is determined */
                nRet = HC_DecodeUintvar (poDecode->i_pbWspHeader, 
                                    &poDecode->i_nWspHdrPos, pnLength, 
                                    poDecode->i_nWspHdrActLen + poDecode->i_nWspHdrDecLen - poDecode->i_nWspHdrPos);
            }
            else
            {   /* If the Actual length the header value is not determined */
                nRet = HC_DecodeUintvar (poDecode->i_pbWspHeader, 
                                    &poDecode->i_nWspHdrPos, pnLength, 
                                    poDecode->i_nWspHdrLen - poDecode->i_nWspHdrPos);
            }
        }
        if (PASSABLE(nRet) && *pnLength == 0)
        {   /* Error - not a length value! */
            nRet = WPS_HC_ERROR_WSP_LENGTH_EXPECTED;
        }
    
        if ( (poDecode->i_nWspHdrPos + *pnLength) > poDecode->i_nWspHdrLen)
        {   /* Error - Data overflow! */
            nRet = WPS_HC_ERROR_WSP_DATA_OVERFLOW;
        }
    }

    return nRet;
} /* HC_DecodeLength */


/*
-----------------------------------------------------------------------------
    Check the Header value if it is fit to the WSP buffer.
    If it is fit then return WPS_HC_STATUS_OK, otherwise 
    WPS_HC_ERROR_WSP_DATA_OVERFLOW.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_CheckLength(DecodeData* poDecode, BOOL bReadFieldName)
{
    HC_STATUS nRet = WPS_HC_STATUS_OK;
    BYTE      bFieldCode = 0;
    
    if (bReadFieldName==TRUE)
    {
        bFieldCode = DECODE_SHORTINT (poDecode->i_pbWspHeader
                                               [poDecode->i_nWspHdrPos++]);
    }

    /* Check the first byte */
    if (poDecode->i_nWspHdrPos > poDecode->i_nWspHdrLen)
    {   /* Error - Data overflow! */
        nRet = WPS_HC_ERROR_WSP_DATA_OVERFLOW;
    }
    if (PASSABLE(nRet))
    {
        BYTE bOctet = poDecode->i_pbWspHeader [poDecode->i_nWspHdrPos++];

        if (OCTET_IS_SHORTINT (bOctet))
        {   /* Short-integer (128-255) */
            if (bReadFieldName == TRUE)
                poDecode->i_nWspHdrActLen = 2;
            else
                poDecode->i_nWspHdrActLen = 1;

            nRet = WPS_HC_STATUS_OK;
        }
        else if (OCTET_IS_BINARY (bOctet))
        {   /* Long-integer - length of 0 is illegal! (0-31) */
            if (bOctet==WPS_CODEC_MAX_BINARY)
            {   /* (31) uintvar value */
                unsigned int nLength;
                unsigned int nPos = poDecode->i_nWspHdrPos;

                poDecode->i_nWspHdrPos--;
                nRet = HC_DecodeLength(poDecode, &nLength, FALSE);
                poDecode->i_nWspHdrActLen = nLength + poDecode->i_nWspHdrPos - poDecode->i_nWspHdrDecLen;

                poDecode->i_nWspHdrPos = nPos;
            }
            else
            {   /* (0-30) Length of the data octets */
                if ( (bOctet + poDecode->i_nWspHdrPos) > poDecode->i_nWspHdrLen )
                {
                    nRet = WPS_HC_ERROR_WSP_DATA_OVERFLOW;
                }
                else
                {
                    poDecode->i_nWspHdrActLen = bOctet + poDecode->i_nWspHdrPos - poDecode->i_nWspHdrDecLen;
                }
            }
        }
        else
        {   /* text value (32-127) */
            unsigned int nPos = poDecode->i_nWspHdrPos;

            while (poDecode->i_pbWspHeader[nPos] != '\0' && nPos < poDecode->i_nWspHdrLen-1 )
            {
                nPos++;
            }
            if (poDecode->i_pbWspHeader[nPos] != '\0')
            {
                nRet = WPS_HC_ERROR_WSP_DATA_OVERFLOW;
            }
            else
            {
                poDecode->i_nWspHdrActLen = nPos + 1 - poDecode->i_nWspHdrDecLen;
            }
        }
        poDecode->i_nWspHdrPos--;
    }/* Endif (PASSABLE(nRet)) */

    if (bReadFieldName==TRUE)
    {
        poDecode->i_nWspHdrPos--;
    }

    return (nRet);

}



/*
-----------------------------------------------------------------------------
    Decodes the actual value as a field name and appends 
    the decoded data to the HTTP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_DecodeWellknownFieldName(
    DecodeData* poDecode)   /* in, out: Decode data. */
{
    HC_STATUS nRet        = WPS_HC_STATUS_OK;
    BYTE      bFirstByte  = 0;

    nRet = HC_ReadOneByte(poDecode, &bFirstByte);

    if (PASSABLE(nRet))
    {
        if (bFirstByte==0x00)
        {
            nRet = HC_DecodeText (poDecode, FALSE, FALSE);
        }
        else if (OCTET_IS_TEXT (bFirstByte))
        {
            nRet = HC_DecodeText (poDecode, FALSE, FALSE);
        }
        else
        {
            const char* pcTmp   = HC_FieldCode2FieldName(WPS_HCP_DEFAULT, NULL, 
                                                         DECODE_SHORTINT(bFirstByte),
                                                         poDecode->i_version);

            if (pcTmp)
            {
                nRet = HC_AppendText (poDecode, pcTmp);
                poDecode->i_nWspHdrPos++;
            }
            else
            {   /* Error - invalid field code! */
                nRet = WPS_HC_ERROR_WSP_INVALID_FIELD_CODE;
            }
        }
    }
    return nRet;
} /* HC_DecodeWellknownFieldName */

/*
-----------------------------------------------------------------------------
    Decodes the actual value as whether integer or text and appends 
    the decoded data to the HTTP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_DecodeUnknown(
    DecodeData* poDecode)   /* in, out: Decode data. */
{
    HC_STATUS nRet    = WPS_HC_STATUS_OK;
    BYTE      bFirstByte = 0;

    nRet = HC_ReadOneByte(poDecode, &bFirstByte);

    if (PASSABLE(nRet))
    {
        /* If there is no value:*/
        if (bFirstByte == '\0')
        {
            poDecode->i_nWspHdrPos++;
            nRet = WPS_HC_STATUS_OK;
        }
        else
        if (OCTET_IS_TEXT (bFirstByte))
        {
            nRet = HC_DecodeText (poDecode, TRUE, FALSE);
        }
        else
        {
            nRet = HC_DecodeInteger (poDecode);
        }
    }
    return nRet;
} /* HC_DecodeUnknown */

/*
-----------------------------------------------------------------------------
    Decodes the actual value as whether a token or text and appends 
    the decoded data to the HTTP buffer. The value is compared to 
    the smallest short integer value; if it matches, it is translated 
    to the given token; otherwise it is decoded as text.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_DecodeSingleCode(
    DecodeData*     poDecode,   /* in, out: Decode data. */
    const char*     pcToken,    /* in:      Default token. */
    HC_STATUS       nErrCode)   /* in:      Error code to return if value is illegal. */
{
    HC_STATUS   nRet;
    BYTE        bOctet  = 0;
    const char* pcTmp;

    nRet = HC_ReadOneByte(poDecode, &bOctet);
    if (PASSABLE(nRet))
    {
        if (bOctet == 128)
        {   /* This means the well-known token */
            pcTmp = pcToken;
            poDecode->i_nWspHdrPos++;
            nRet = HC_AppendText (poDecode, pcTmp);
        }
        else if (OCTET_IS_TEXT (bOctet))
        { /* Token-text */
            nRet = HC_DecodeText(poDecode, FALSE, FALSE);
        }
        else
        {   /* Error - invalid value! */
            nRet = nErrCode;
        }
    }
    return nRet;
} /* HC_DecodeSingleCode */

/*
-----------------------------------------------------------------------------
    Decodes the actual value as whether a value code or text and appends 
    the decoded data to the HTTP buffer. The values are compared to the 
    values in the given table, and if there is a match, the corresponding 
    name is appended to the HTTP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_DecodeByTable(
    DecodeData*         poDecode,   /* in, out: Decode data. */
    const NameBytePair  aPairs[],   /* in:      Table of name-value pairs. */
    int                 nNumPairs,  /* in:      Number of elements in table. */
    HC_STATUS           nErrCode)   /* in:      Error code to return if value is illegal. */
{
    HC_STATUS   nRet;
    BYTE        bOctet  = 0;
    const char* pcTmp   = NULL;

    nRet = HC_ReadOneByte(poDecode, &bOctet);

    if (PASSABLE(nRet))
    {
        pcTmp = HC_Byte2Name_Unord (aPairs, nNumPairs, bOctet);
    
        if (pcTmp)
        {
            poDecode->i_nWspHdrPos++;
            nRet = HC_AppendText (poDecode, pcTmp);
        }
        else if (OCTET_IS_TEXT (bOctet))
        { /* Token-text */
            nRet = HC_DecodeText(poDecode, FALSE, FALSE);
        }
        else
        {   /* Error - invalid value! */
            nRet = nErrCode;
        }
    }
    return nRet;
} /* HC_DecodeByTable */

/*
-----------------------------------------------------------------------------
    PARAMETER DECODING
-----------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------
    Decodes the Q parameter value from the WSP header and appends 
    the decoded data to the HTTP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_DecodeQValue(
    DecodeData* poDecode)   /* in, out: Decode data. */
{
    UINT32     nQValue;
    HC_STATUS  nRet    = HC_DecodeUintvar (poDecode->i_pbWspHeader, 
                                            &poDecode->i_nWspHdrPos, &nQValue, 
                                            poDecode->i_nWspHdrActLen+poDecode->i_nWspHdrDecLen-poDecode->i_nWspHdrPos);

    if (PASSABLE(nRet))
    {
        char    acBuf[6]="";
        INT32   nLen = 0;

        if (nQValue == 0 || nQValue >= 1200)
        {   /* Error - invalid Q-value code! */
            nRet = WPS_HC_ERROR_WSP_INVALID_Q_CODE;
        }
        else if (nQValue == 1)
        {   /* Q-value is 0 */
            strcpy (acBuf, "0");
        }
        else if (nQValue <= 100)
        {   /* Q-value is 0.x or 0.xx */
            if (--nQValue % 10 == 0)
            {
                nQValue /= 10;
                strcpy(acBuf, "0.");

                nLen = HC_Strnlen(acBuf, 6);
                if (nLen==-1)
                {
                    nRet = WPS_HC_ERROR_WSP_DATA_OVERFLOW;
                }
                else
                {
                    nRet = UINT32toStr(nQValue, &acBuf[nLen], 6);
                }
            }
            else
            {
                strcpy(acBuf, "0.");
                if (nQValue<10)
                {
                    strcat(acBuf, "0");
                }

                nLen = HC_Strnlen(acBuf, 6);
                if (nLen==-1)
                {
                    nRet = WPS_HC_ERROR_WSP_DATA_OVERFLOW;
                }
                else
                {
                    nRet = UINT32toStr(nQValue, &acBuf[nLen], 6);
                }
            }
        }
        else if (nQValue < 1200)
        {   /* Q-value is 0.xxx */
            strcpy(acBuf, "0.");
            if ((nQValue-100)<10)
            {
                strcat(acBuf, "00");
            }else if ((nQValue-100) <100)
            {
                strcat(acBuf, "0");
            }

            nLen = HC_Strnlen(acBuf, 6);
            if (nLen==-1)
            {
                nRet = WPS_HC_ERROR_WSP_DATA_OVERFLOW;
            }
            else
            {
                nRet = UINT32toStr((nQValue-100), &acBuf[nLen], 6);
            }
        }
        if (PASSABLE(nRet))
        {
            nRet = HC_AppendText (poDecode, acBuf);
        }
    }
    return nRet;
} /* HC_DecodeQValue */

/*
-----------------------------------------------------------------------------
    Decodes the Charset parameter value from the WSP header and appends 
    the decoded data to the HTTP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_DecodeCharsetValue(
    DecodeData* poDecode)   /* in, out: Decode data. */
{
    HC_STATUS  nRet        = WPS_HC_STATUS_OK;
    BYTE       bFirstByte  = 0;

    nRet = HC_ReadOneByte(poDecode, &bFirstByte);
    
    if (PASSABLE(nRet))
    {
        if (OCTET_IS_TEXT (bFirstByte))
        {
            nRet = HC_DecodeText (poDecode, FALSE, FALSE);
        }
        else
        {
            UINT32 nCharsetCode;
    
            nRet = HC_ParseInteger (poDecode, &nCharsetCode);
            if (PASSABLE(nRet))
            {
                const char* pcTmp   = HC_CharsetCode2CharsetName((WORD)nCharsetCode,
                                                                 poDecode->i_version);
    
                if (pcTmp)
                {
                    nRet = HC_AppendText (poDecode, pcTmp);
                }
                else
                {   /* Error - unknown charset code! */
                    nRet = WPS_HC_ERROR_WSP_UNKNOWN_CHARSET_CODE;
                }
            }
        }
    }

    return nRet;
} /* HC_DecodeCharsetValue */

/*
-----------------------------------------------------------------------------
    Decodes the Version parameter value from the WSP header and appends 
    the decoded data to the HTTP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_DecodeVersionValue(
    DecodeData* poDecode)   /* in, out: Decode data. */
{
    HC_STATUS  nRet        = WPS_HC_STATUS_OK;
    BYTE       bFirstByte  = 0;
    
    nRet = HC_ReadOneByte(poDecode, &bFirstByte);

    if (PASSABLE(nRet))
    {
        if (OCTET_IS_TEXT (bFirstByte))
        {
            nRet = HC_DecodeText (poDecode, FALSE, FALSE);
        }
        else
        {
            int     nMajor  = (bFirstByte & 0x70) / 15, 
                    nMinor  = bFirstByte & 0x0F;
            char    acBuf[5]= "";
            INT32   nLen    = 0;
    
            if (nMinor == 15)
            {   /* Major version only */
                INT32toStr(nMajor, acBuf, 5);
            }
            else
            {
                INT32toStr(nMajor, acBuf, 5);
                strcat(acBuf, ".");
    
                nLen = HC_Strnlen(acBuf, 5);
                if (nLen==-1)
                {
                    nRet = WPS_HC_ERROR_WSP_DATA_OVERFLOW;
                }
                else
                {
                    nRet = INT32toStr(nMinor, &acBuf[nLen], 5);
                }           
            }

            if (PASSABLE(nRet))
            {
                nRet = HC_AppendText (poDecode, acBuf);
                poDecode->i_nWspHdrPos++;
            }
        }
    }
    return nRet;
} /* HC_DecodeVersionValue */

/*
-----------------------------------------------------------------------------
    Decodes a parameter from the WSP header and appends 
    the decoded data to the HTTP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_DecodeParameter(
    DecodeData* poDecode,       /* in, out: Decode data. */
    BOOL        bQValueOnly,    /* in:      Determines whether only Q-values or full 
                                            parameters are accepted. */
    BOOL        bPutSeparator)  /* in:      Determines whether separator is needed or not. */
{
    HC_STATUS   nRet;
    BYTE        bFirstByte  = 0;
    const char* pcTmp       = NULL;
    UINT32      OldHttpPosition = poDecode->i_nHttpBufPos;
    
    nRet = HC_ReadOneByte(poDecode, &bFirstByte);

    if (PASSABLE(nRet))
    {
        if (bPutSeparator)
        {   /* Param separator */
            nRet = HC_AppendText (poDecode, "; ");
        }
        if (bQValueOnly)
        {
            nRet = HC_AppendText (poDecode, HC_ParamCode2ParamName (WPS_HFC_PARAM_Q));
            if (PASSABLE(nRet))
            {
                nRet = HC_AppendText (poDecode, "=");
            }
            if (PASSABLE(nRet))
            {
                nRet = HC_DecodeQValue (poDecode);
            }
        }
        else if (OCTET_IS_SHORTINT (bFirstByte))
        {   /* Typed-parameter */
            bFirstByte = DECODE_SHORTINT (bFirstByte);
            pcTmp = HC_ParamCode2ParamName (bFirstByte);
            if (pcTmp)
            {
                nRet = HC_AppendText (poDecode, pcTmp);
                if (PASSABLE(nRet))
                {
                    nRet = HC_AppendText (poDecode, "=");
                }
                poDecode->i_nWspHdrPos++;
    
                switch (bFirstByte)
                {
                case WPS_HFC_PARAM_Q            : /* Q-value */
                    nRet = HC_DecodeQValue (poDecode);
                    break;
                case WPS_HFC_PARAM_CHARSET      : /* Well-known charset */
                    nRet = HC_DecodeCharsetValue (poDecode);
                    break;
                case WPS_HFC_PARAM_LEVEL        : /* Version-value */
                    nRet = HC_DecodeVersionValue (poDecode);
                    break;
                case WPS_HFC_PARAM_PADDING      : /* Short-integer */
                case WPS_HFC_PARAM_TYPE         : /* Integer-value */
                    nRet = HC_DecodeInteger (poDecode);
                    break;
                case WPS_HFC_PARAM_NAME         : /* Text-string */
                case WPS_HFC_PARAM_FILENAME     : /* Text-string */
                    nRet = HC_DecodeText (poDecode, FALSE, FALSE);
                    break;
                case WPS_HFC_PARAM_DIFFERENCES  : /* Field-name */
                    nRet = HC_DecodeWellknownFieldName (poDecode);
                    break;
                }
            }
            else
            {   /* Error - unknown parameter code! */
                poDecode->i_nHttpBufPos = OldHttpPosition;
                return SetError(poDecode,WPS_HC_ERROR_WSP_INVALID_PARAMETER);
            }
        }
        else if (OCTET_IS_TEXT (bFirstByte))
        { /* Untyped-parameter */
            BYTE next_byte=0;

            nRet = HC_DecodeText(poDecode, FALSE, FALSE);

            if (PASSABLE(nRet))
                nRet = HC_ReadOneByte(poDecode, &next_byte);
            
            /* If No-value present, no equation sign is required */
            if (PASSABLE(nRet) && next_byte != 0)
            {
                nRet = HC_AppendText (poDecode, "=");
            }
            if (PASSABLE(nRet))
            {
                nRet = HC_DecodeUnknown (poDecode);
            }
        }
    }
    return nRet;
} /* HC_DecodeParameter */

/*
-----------------------------------------------------------------------------
    FIELD VALUE DECODING
-----------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------
    Decodes a media value (with its parameters, if there are any) and appends 
    the decoded data to the HTTP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_DecodeMediaValue(
    DecodeData*     poDecode,       /* in, out: Decode data. */
    Word2NameFunc   pfLookupFunc,   /* in:      Lookup function for decoding the media value. */
    BOOL            bQValueOnly)    /* in:      Determines whether only Q-values or full 
                                                parameters are accepted. */
{
    HC_STATUS  nRet;
    UINT32     nValueLen   = 0, 
               nOrigPos    = poDecode->i_nWspHdrPos;
    BYTE       bFirstByte  = 0;

    nRet = HC_ReadOneByte(poDecode, &bFirstByte);

    if (PASSABLE(nRet))
    {

        if (bFirstByte!=0) /* If it isn't empty */
        {
            if (OCTET_IS_BINARY (bFirstByte))
            { /* General form: Value-length first */
                nRet = HC_DecodeLength (poDecode, &nValueLen, TRUE);
                if (PASSABLE(nRet))
                {
                    nRet = HC_ReadOneByte(poDecode, &bFirstByte);
                }
            }
            if (PASSABLE(nRet))
            {
                const char* pcTmp   = NULL;
        
                if (OCTET_IS_TEXT (bFirstByte))
                { /* Token-text */
                    nRet = HC_DecodeText(poDecode, FALSE, FALSE);
                }
                else
                {   /* By default we assume Constrained-encoding */
                    UINT32  nCode   = DECODE_SHORTINT (bFirstByte);
        
                    if (OCTET_IS_BINARY (bFirstByte))
                    {   /* Integer-value */
                        nRet = HC_ParseInteger (poDecode, &nCode);
                    }
                    else
                    {   /* Short-integer - move the pointer to the next byte */
                        poDecode->i_nWspHdrPos++;
                    }
                    if (PASSABLE(nRet))
                    {
                        pcTmp = pfLookupFunc((WORD)nCode, poDecode->i_version);
                        if (pcTmp)
                        {
                            nRet = HC_AppendText (poDecode, pcTmp);
                        }
                        else
                        {   /* Error - unknown content-type code! */
                            return SetError(poDecode,WPS_HC_ERROR_WSP_UNKNOWN_CNT_TYPE_CODE);
                        }
                    }
                }
                while ( PASSABLE(nRet) && 
                        poDecode->i_nWspHdrPos - nOrigPos <= nValueLen )
                {
                    nRet = HC_DecodeParameter (poDecode, bQValueOnly, TRUE);
                }
            }
        }
        else
        { 
            /*It is empty */
            poDecode->i_nWspHdrPos++;
        }
    }
    return nRet;
} /* HC_DecodeMediaValue */

/*
-----------------------------------------------------------------------------
    Decodes the actual data as a method name and appends 
    the decoded data to the HTTP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_DecodeMethodName(
    DecodeData* poDecode)   /* in, out: Decode data. */
{
    HC_STATUS   nRet;
    BYTE        bCode   = 0;
    const char* pcTmp;

    nRet = HC_ReadOneByte(poDecode, &bCode);

    if (PASSABLE(nRet))
    {
        if (OCTET_IS_TEXT (bCode))
        { /* Token-text */
            nRet = HC_DecodeText(poDecode, FALSE, FALSE);
        }
        else 
        { /* Well-known method */
            pcTmp = HC_MethodCode2MethodName (DECODE_SHORTINT (bCode));
            if (pcTmp)
            {
                nRet = HC_AppendText (poDecode, pcTmp);
                poDecode->i_nWspHdrPos++;
            }
            else
            {   /* Error - unknown method code! */
                nRet = WPS_HC_ERROR_WSP_UNKNOWN_METHOD_CODE;
            }
        }
    }
    return nRet;
} /* HC_DecodeMethodName */

/*
-----------------------------------------------------------------------------
    Decodes the Authorization field value and appends 
    the decoded data to the HTTP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_DecodeFieldValue_Authorization(
    DecodeData* poDecode)   /* in, out: Decode data. */
{
    HC_STATUS   nRet;
    UINT32      nValueLen   = 0;

    nRet = HC_DecodeLength (poDecode, &nValueLen, TRUE);
    if (PASSABLE(nRet))
    {
        const char* pcTmp;
        BYTE bFirstByte = 0;

        nRet = HC_ReadOneByte(poDecode, &bFirstByte);
        
        if (PASSABLE(nRet))
        {
            if (bFirstByte == 128)
            {   /* Basic */
                char    acUser      [WPS_CODEC_BUF_SIZE], 
                        acCookie    [WPS_CODEC_BUF_SIZE];
                INT32   nLen        = 0;
    
                poDecode->i_nWspHdrPos++;
                nRet = HC_AppendText (poDecode, WPS_HC_FV_AUTH_SCHEME_BASIC);
                if (PASSABLE(nRet))
                {
                    nRet = HC_AppendText (poDecode, " ");
                }
                if (PASSABLE(nRet))
                {
                    pcTmp = (char*)poDecode->i_pbWspHeader + poDecode->i_nWspHdrPos;
    
                    nLen = HC_Strnlen(pcTmp, poDecode->i_nWspHdrActLen + poDecode->i_nWspHdrDecLen - poDecode->i_nWspHdrPos);
                    if (nLen==-1)
                    {
                        nRet = WPS_HC_ERROR_WSP_DATA_OVERFLOW;
                    }
                    else
                    {
                        strcpy (acUser, pcTmp);
                        poDecode->i_nWspHdrPos += nLen + 1;
                        strcat (acUser, ":");
                        pcTmp = (char*)poDecode->i_pbWspHeader + poDecode->i_nWspHdrPos;
    
                        nLen = HC_Strnlen(pcTmp, poDecode->i_nWspHdrActLen + poDecode->i_nWspHdrDecLen - poDecode->i_nWspHdrPos);
                        if (nLen==-1)
                        {
                            nRet = WPS_HC_ERROR_WSP_DATA_OVERFLOW;
                        }
                        else
                        {
                            UINT32 acCookieLen = 0;
                            strcat (acUser, pcTmp);
                            poDecode->i_nWspHdrPos += nLen + 1;
                            acCookieLen = HC_EncodeBase64 (acUser, acCookie, HC_Strnlen(acUser, 0));
                            *(acCookie+acCookieLen)='\0';

                            nRet = HC_AppendText (poDecode, acCookie);
                        }
                    }
                }
            }
            else
            {   /* Authentication-scheme */
                nRet = HC_DecodeText (poDecode, FALSE, FALSE);
            }
        }
    }
    return nRet;
} /* HC_DecodeFieldValue_Authorization */

/*
-----------------------------------------------------------------------------
    Decodes the Cache-Control field value and appends 
    the decoded data to the HTTP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_DecodeFieldValue_CacheControl(
    DecodeData* poDecode)   /* in, out: Decode data. */
{
    HC_STATUS   nRet        = WPS_HC_STATUS_OK;
    UINT32      nValueLen   = 0, 
                nOrigPos    = poDecode->i_nWspHdrPos,
                nLengthLen  = 0;
    BYTE        bCode       = 0;
    
    nRet = HC_ReadOneByte(poDecode, &bCode);

    if (PASSABLE(nRet))
    {
        if (OCTET_IS_BINARY (bCode))
        { /* General form: Value-length first */
            nRet = HC_DecodeLength (poDecode, &nValueLen, TRUE);
            /* The length of the length */
            nLengthLen = poDecode->i_nWspHdrPos - nOrigPos;

            if (PASSABLE(nRet))
            {
                nRet = HC_ReadOneByte(poDecode, &bCode);
            }
        }
        if (PASSABLE(nRet))
        {
            const char* pcTmp;
    
            if( nValueLen == 0 && 
                ( bCode == WPS_HFC_CACHECTL_MAX_AGE || 
                bCode == WPS_HFC_CACHECTL_MIN_FRESH ) )
            {   /* Error - Value-length required! */
                nRet = WPS_HC_ERROR_WSP_LENGTH_EXPECTED;
            }
            if (PASSABLE(nRet))
            {
                if (OCTET_IS_TEXT (bCode))
                { /* Token-text */
                    nRet = HC_DecodeText(poDecode, FALSE, FALSE);
                }
                else 
                { /* Well-known Cache-control-value */
                    pcTmp = HC_CacheCtlCode2CacheCtlName (bCode);
                    if (pcTmp)
                    {
                        nRet = HC_AppendText (poDecode, pcTmp);
                        poDecode->i_nWspHdrPos++;
                    }
                    else
                    {   /* Error - unknown code! */
                        nRet = WPS_HC_ERROR_WSP_UNKNOWN_CACHE_CTL_CODE;
                    }
                }
                if ( PASSABLE(nRet) && 
                    poDecode->i_nWspHdrPos - nOrigPos <= nValueLen )
                {   /* There is a parameter after the value */
                    BOOL    bFirstParam = TRUE;
    
                    switch (bCode)
                    {
                    case WPS_HFC_CACHECTL_NO_CACHE  :
                    case WPS_HFC_CACHECTL_PRIVATE   : /* 1*(Field-name) */
                        nRet = HC_AppendText (poDecode, "=");
                        while ( PASSABLE(nRet) && 
                                poDecode->i_nWspHdrPos - nOrigPos <= nValueLen )
                        {
                            nRet = HC_AppendText (poDecode, (bFirstParam ? "\"" : ","));
                            nRet = HC_DecodeWellknownFieldName (poDecode);
                            bFirstParam = FALSE;
                        }
                        if (PASSABLE(nRet))
                        {
                            nRet = HC_AppendText (poDecode, "\"");
                        }
                        break;
                    case WPS_HFC_CACHECTL_MAX_AGE   :
                    case WPS_HFC_CACHECTL_MAX_STALE :
                    case WPS_HFC_CACHECTL_MIN_FRESH : /* Delta-second-value */
                        nRet = HC_AppendText (poDecode, "=");
                        nRet = HC_DecodeInteger (poDecode);
                        break;
                    default                         : /* The rest must be parameter */
                        while ( PASSABLE(nRet) && 
                            poDecode->i_nWspHdrPos - nOrigPos <= nValueLen )
                        {
                            nRet = HC_DecodeParameter (poDecode, FALSE, TRUE);
                        }
                        break;
                    }
                }
                if ( PASSABLE(nRet) && 
                    nValueLen != 0 && 
                    poDecode->i_nWspHdrPos - nOrigPos != nValueLen + nLengthLen )
                {   /* Error - incorrect length! */
                    nRet = WPS_HC_ERROR_WSP_INCORRECT_VALUE_LEN;
                }
            }
        }
    }
    return nRet;
} /* HC_DecodeFieldValue_CacheControl */

/*
-----------------------------------------------------------------------------
    Decodes the Content-Disposition field value and appends 
    the decoded data to the HTTP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_DecodeFieldValue_ContentDisposition(
    DecodeData* poDecode)   /* in, out: Decode data. */
{
    HC_STATUS   nRet        = WPS_HC_STATUS_OK;
    UINT32      nValueLen   = 0, 
                nOrigPos    = poDecode->i_nWspHdrPos;

    nRet = HC_DecodeLength (poDecode, &nValueLen, TRUE);
    if (PASSABLE(nRet))
    {
        BYTE        bCode   = 0 ;
        const char* pcTmp   = NULL; 

        nRet = HC_ReadOneByte(poDecode, &bCode);

        if (PASSABLE(nRet))
        {
            poDecode->i_nWspHdrPos++;
            pcTmp = HC_Byte2Name_Unord (g_aContentDispValues, DIM (g_aContentDispValues), bCode);

            if (pcTmp)
            {   /* Disposition *(Parameter) */
                nRet = HC_AppendText (poDecode, pcTmp);
                while ( PASSABLE(nRet) && 
                        poDecode->i_nWspHdrPos - nOrigPos <= nValueLen )
                {
                    nRet = HC_DecodeParameter (poDecode, FALSE, TRUE);
                }
            }
            else
            {   /* Error - unknown Content-Disposition value! */
                nRet = WPS_HC_ERROR_WSP_UNKNOWN_CONTENT_DISP;
            }
        }
    }
    return nRet;
} /* HC_DecodeFieldValue_ContentDisposition */

/*
-----------------------------------------------------------------------------
    Decodes the Content-Language field value and appends 
    the decoded data to the HTTP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_DecodeFieldValue_ContentLanguage(
    DecodeData* poDecode)   /* in, out: Decode data. */
{
    HC_STATUS  nRet    = WPS_HC_STATUS_OK;
    const char* pcTmp;
    BYTE bFirstByte = 0;

    nRet = HC_ReadOneByte(poDecode, &bFirstByte);

    if (PASSABLE(nRet))
    {
        if (OCTET_IS_TEXT (bFirstByte))
        { /* Token-text */
            nRet = HC_DecodeText(poDecode, FALSE, FALSE);
        }
        else 
        { /* Well-known language */
            UINT32  nLangCode;
    
            nRet = HC_ParseInteger (poDecode, &nLangCode);
            if (PASSABLE(nRet))
            {
                pcTmp = HC_LangCode2LangName((WORD)nLangCode, poDecode->i_version);
                if (pcTmp)
                {
                    nRet = HC_AppendText (poDecode, pcTmp);
                }
                else
                {   /* Error - unknown language code! */
                    nRet = WPS_HC_ERROR_WSP_UNKNOWN_LANG_CODE;
                }
            }
        }
    }
    return nRet;
} /* HC_DecodeFieldValue_ContentLanguage */

/*
-----------------------------------------------------------------------------
    Decodes the Content-MD5 field value and appends 
    the decoded data to the HTTP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_DecodeFieldValue_ContentMD5(
    DecodeData* poDecode)   /* in, out: Decode data. */
{
    HC_STATUS   nRet        = WPS_HC_STATUS_OK;
    UINT32      nValueLen   = 0;

    nRet = HC_DecodeLength (poDecode, &nValueLen, TRUE);
    if (PASSABLE(nRet))
    {
        if (nValueLen == 16)
        {   /* Content-MD5 value length is correct */
            char        acSrc   [WPS_CODEC_BUF_SIZE], 
                        acDest  [WPS_CODEC_BUF_SIZE];
            const char* pcTmp   = (char*)poDecode->i_pbWspHeader + poDecode->i_nWspHdrPos;
            UINT32 nLen         = 0;

            memcpy(acSrc, pcTmp, nValueLen);
            poDecode->i_nWspHdrPos += nValueLen;

            nLen = HC_EncodeBase64 (acSrc, acDest, 16);
            if (nLen!=24)
            {
                nRet = WPS_HC_ERROR_WSP_INCORRECT_MD5_LEN;
            }
            acDest[nLen] = '\0';
            nRet = HC_AppendText (poDecode, acDest);
        }
        else
        {   /* Error - Content-MD5 value length is incorrect! */
            nRet = WPS_HC_ERROR_WSP_INCORRECT_MD5_LEN;
        }
    }
    return nRet;
} /* HC_DecodeFieldValue_ContentMD5 */

/*
-----------------------------------------------------------------------------
    Decodes the Content-Range field value and appends 
    the decoded data to the HTTP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_DecodeFieldValue_ContentRange(
    DecodeData* poDecode)   /* in, out: Decode data. */
{
    HC_STATUS   nRet        = WPS_HC_STATUS_OK;
    UINT32      nValueLen   = 0;
    INT32       nLen        = 0;

    nRet = HC_DecodeLength (poDecode, &nValueLen, TRUE);
    if (PASSABLE(nRet))
    {
        UINT32  nOrigPos        = poDecode->i_nWspHdrPos, 
                nFirstBytePos, 
                nEntityLength;

        nRet = HC_DecodeUintvar (poDecode->i_pbWspHeader, 
                                 &poDecode->i_nWspHdrPos, &nFirstBytePos,
                                 poDecode->i_nWspHdrActLen+poDecode->i_nWspHdrDecLen-poDecode->i_nWspHdrPos);
        if (PASSABLE(nRet))
        {
            nRet = HC_DecodeUintvar (poDecode->i_pbWspHeader, 
                                     &poDecode->i_nWspHdrPos, &nEntityLength,
                                     poDecode->i_nWspHdrActLen+poDecode->i_nWspHdrDecLen-poDecode->i_nWspHdrPos);
            if (PASSABLE(nRet))
            {
                char    acBuf   [WPS_CODEC_BUF_SIZE]="";

                if (poDecode->i_nWspHdrPos > poDecode->i_nWspHdrLen)
                {   /* Error - Data overflow! */
                    nRet = WPS_HC_ERROR_WSP_DATA_OVERFLOW;
                }
                else if (poDecode->i_nWspHdrPos - nOrigPos != nValueLen)
                {   /* Error - Incorrect Value-length! */
                    nRet = WPS_HC_ERROR_WSP_INCORRECT_VALUE_LEN;
                }
                else
                {
                    strcpy(acBuf, "bytes ");
                    
                    nLen = HC_Strnlen(acBuf, WPS_CODEC_BUF_SIZE);
                    if (nLen==-1)
                    {
                        nRet = WPS_HC_ERROR_WSP_DATA_OVERFLOW;
                    }
                    else
                    {
                        UINT32toStr(nFirstBytePos, &acBuf[nLen], WPS_CODEC_BUF_SIZE);
                        strcat(acBuf, "-");
                        nLen = HC_Strnlen(acBuf, WPS_CODEC_BUF_SIZE);
                        if (nLen==-1)
                        {
                            nRet = WPS_HC_ERROR_WSP_DATA_OVERFLOW;
                        }
                        else
                        {
                            UINT32toStr((nFirstBytePos + poDecode->i_nContentLen - 1), &acBuf[nLen], WPS_CODEC_BUF_SIZE);
                            strcat(acBuf, "/");
                            nLen = HC_Strnlen(acBuf, WPS_CODEC_BUF_SIZE);
                            if (nLen==-1)
                            {
                                nRet = WPS_HC_ERROR_WSP_DATA_OVERFLOW;
                            }
                            else
                            {
                                UINT32toStr(nEntityLength, &acBuf[nLen], WPS_CODEC_BUF_SIZE);
                                nRet = HC_AppendText (poDecode, acBuf);
                            }
                        }
                    }
                }
            }
        }
    }
    return nRet;
} /* HC_DecodeFieldValue_ContentRange */

/*
-----------------------------------------------------------------------------
    Decodes the Date field value and appends 
    the decoded data to the HTTP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_DecodeFieldValue_Date(
    DecodeData* poDecode,   /* in, out: Decode data. */
    BOOL        bIsPrefix)  /* in:      Determines whether there is a prefix byte or not. */
{
    HC_STATUS   nRet        = WPS_HC_STATUS_OK;
    BOOL        bIsDate     = TRUE;

    if (bIsPrefix)
    {
        UINT32  nValueLen;

        nRet = HC_DecodeLength (poDecode, &nValueLen, TRUE);
        if (PASSABLE(nRet))
        {
            BYTE    bTimeCode   = poDecode->i_pbWspHeader [poDecode->i_nWspHdrPos++];

            if (bTimeCode == 128)
            {
                bIsDate = TRUE;
            }
            else if (bTimeCode == 129)
            {
                bIsDate = FALSE;
            }
            else
            {   /* Error - invalid Time prefix! */
                nRet = WPS_HC_ERROR_WSP_INVALID_TIME_CODE;
            }
        }
    }
    if (PASSABLE(nRet))
    {
        if (OCTET_IS_TEXT (poDecode->i_pbWspHeader [poDecode->i_nWspHdrPos]))
        { /* Text-string */
            nRet = HC_DecodeText(poDecode, FALSE, FALSE);
        }
        else if (bIsDate)
        { /* Date-value */
            UINT32      tSeconds;

            nRet = HC_ParseInteger (poDecode, &tSeconds);
            if (PASSABLE(nRet))
            {
                DateStruct      oTime;
                char            acBuf       [WPS_CODEC_BUF_SIZE]="";
                const char*     pcMonth;
                static const char* acDays[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
                const char*     pcDay;
                INT32           nLen        = 0;
                
                SectoDate(tSeconds, &oTime);

                pcDay = acDays [oTime.week_day];
                pcMonth = HC_Byte2Name_Unord (g_aMonths, DIM (g_aMonths), (BYTE)oTime.mon);


                strcpy(acBuf, pcDay);
                strcat(acBuf, ", ");
                if (oTime.day<10)
                {
                    strcat(acBuf, "0");
                }
                
                nLen = HC_Strnlen(acBuf, WPS_CODEC_BUF_SIZE);
                if (nLen==-1)
                {
                    nRet = WPS_HC_ERROR_WSP_DATA_OVERFLOW;
                }
                else
                {
                    INT32toStr(oTime.day, &acBuf[nLen], WPS_CODEC_BUF_SIZE);
                    strcat(acBuf, " ");
                    strcat(acBuf, pcMonth);
                    strcat(acBuf, " ");                 
                    nLen = HC_Strnlen(acBuf, WPS_CODEC_BUF_SIZE);
                    if (nLen==-1)
                    {
                        nRet = WPS_HC_ERROR_WSP_DATA_OVERFLOW;
                    }
                    else
                    {
                        INT32toStr((oTime.year + 1900), &acBuf[nLen], WPS_CODEC_BUF_SIZE);
                        strcat(acBuf, " ");
                        if (oTime.hour<10)
                        {
                            strcat(acBuf, "0");
                        }
                        nLen = HC_Strnlen(acBuf, WPS_CODEC_BUF_SIZE);
                        if (nLen==-1)
                        {
                            nRet = WPS_HC_ERROR_WSP_DATA_OVERFLOW;
                        }
                        else
                        {
                            INT32toStr(oTime.hour, &acBuf[nLen], WPS_CODEC_BUF_SIZE);
                            strcat(acBuf, ":");
                            if (oTime.min<10)
                            {
                                strcat(acBuf, "0");
                            }
                            nLen = HC_Strnlen(acBuf, WPS_CODEC_BUF_SIZE);
                            if (nLen==-1)
                            {
                                nRet = WPS_HC_ERROR_WSP_DATA_OVERFLOW;
                            }
                            else
                            {
                                INT32toStr(oTime.min, &acBuf[nLen], WPS_CODEC_BUF_SIZE);
                                strcat(acBuf, ":");
                                if (oTime.sec<10)
                                {
                                    strcat(acBuf, "0");
                                }
                                nLen = HC_Strnlen(acBuf, WPS_CODEC_BUF_SIZE);
                                if (nLen==-1)
                                {
                                    nRet = WPS_HC_ERROR_WSP_DATA_OVERFLOW;
                                }   
                                else
                                {
                                    INT32toStr(oTime.sec, &acBuf[nLen], WPS_CODEC_BUF_SIZE);
                                    strcat(acBuf, " GMT");

                                    HC_AppendText (poDecode, acBuf);
                                }
                            }
                        }
                    }
                }
            }
        }
        else 
        { /* Delta-seconds-value */
            nRet = HC_DecodeInteger (poDecode);
        }
    }
    return nRet;
} /* HC_DecodeFieldValue_Date */

/*
-----------------------------------------------------------------------------
    Decodes the Pragma field value and appends 
    the decoded data to the HTTP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_DecodeFieldValue_Pragma(
    DecodeData* poDecode)   /* in, out: Decode data. */
{
    HC_STATUS   nRet        = WPS_HC_STATUS_OK;
    UINT32      nValueLen   = 0;
    BYTE        bFirstByte  = 0;

    nRet = HC_ReadOneByte(poDecode, &bFirstByte);

    if (PASSABLE(nRet))
    {
        if (bFirstByte == 128)
        {   /* No-cache */
            poDecode->i_nWspHdrPos++;
            nRet = HC_AppendText (poDecode, WPS_HC_FV_PRAGMA_NO_CACHE);
        }
        else
        { /* Value-length Parameter */
            nRet = HC_DecodeLength (poDecode, &nValueLen, TRUE);
            if (PASSABLE(nRet))
            {
                nRet = HC_DecodeParameter (poDecode, FALSE, FALSE);
            }
        }
    }
    return nRet;
} /* HC_DecodeFieldValue_Pragma */

/*
-----------------------------------------------------------------------------
    Decodes the Proxy-Authenticate field value and appends 
    the decoded data to the HTTP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_DecodeFieldValue_ProxyAuthenticate(
    DecodeData* poDecode)   /* in, out: Decode data. */
{
    HC_STATUS   nRet        = WPS_HC_STATUS_OK;
    UINT32      nValueLen   = 0;
    UINT32      nOrigPos    = poDecode->i_nWspHdrPos;
    BYTE        bFirstByte  = 0;

    nRet = HC_DecodeLength (poDecode, &nValueLen, TRUE);
    if (PASSABLE(nRet))
    {
        nRet = HC_ReadOneByte(poDecode, &bFirstByte);
        
        if (PASSABLE(nRet))
        {
            if (poDecode->i_pbWspHeader [poDecode->i_nWspHdrPos] == 128)
            {   /* Basic */
                poDecode->i_nWspHdrPos++;
                nRet = HC_AppendText (poDecode, WPS_HC_FV_AUTH_SCHEME_BASIC);
            }
            else
            { /* Authentication-scheme */
                nRet = HC_DecodeText (poDecode, FALSE, FALSE);
            }
            if (PASSABLE(nRet))
            {
                nRet = HC_AppendText (poDecode, " realm=\"");
                if (PASSABLE(nRet))
                {
                    nRet = HC_DecodeText (poDecode, FALSE, FALSE);
                    if (PASSABLE(nRet))
                    {
                        nRet = HC_AppendText (poDecode, "\"");
                    }
    
                    while ( PASSABLE(nRet) && 
                        poDecode->i_nWspHdrPos - nOrigPos <= nValueLen )
                    {
                        nRet = HC_DecodeParameter (poDecode, FALSE, TRUE);
                    }
                }
            }
        }
    }
    return nRet;
} /* HC_DecodeFieldValue_ProxyAuthenticate */

/*
-----------------------------------------------------------------------------
    Decodes the Range field value and appends 
    the decoded data to the HTTP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_DecodeFieldValue_Range(
    DecodeData* poDecode)   /* in, out: Decode data. */
{
    HC_STATUS   nRet        = WPS_HC_STATUS_OK;
    UINT32      nValueLen   = 0, 
                nOrigPos    = poDecode->i_nWspHdrPos;
    INT32       nLen        = 0;

    nRet = HC_DecodeLength (poDecode, &nValueLen, TRUE);
    if (PASSABLE(nRet))
    {
        BYTE    bCode           = 0;
        UINT32  nFirstBytePos, 
                nLastBytePos, 
                nSuffixLength;
        char    acBuf           [WPS_CODEC_BUF_SIZE]="";

        nRet = HC_ReadOneByte(poDecode, &bCode);

        if (PASSABLE(nRet))
        {
            poDecode->i_nWspHdrPos++;
            if (bCode == 128)
            {   /* Byte-range */
                nRet = HC_DecodeUintvar (poDecode->i_pbWspHeader, 
                                        &poDecode->i_nWspHdrPos, &nFirstBytePos,
                                        poDecode->i_nWspHdrActLen+poDecode->i_nWspHdrDecLen-poDecode->i_nWspHdrPos);
                if (PASSABLE(nRet))
                {
/*                  if (poDecode->i_nLastFieldCode != WPS_HFC_DEF_RANGE) */
                    if (!HC_RepeatField(poDecode,WPS_HFC_DEF_RANGE,1))
                    {   /* Normal Range field */
                        nRet = HC_AppendText (poDecode, "bytes=");
                    }
                    if (PASSABLE(nRet))
                    {
                        UINT32toStr(nFirstBytePos, acBuf, WPS_CODEC_BUF_SIZE);
                        strcat(acBuf, "-");
    
                        nRet = HC_AppendText (poDecode, acBuf);
                        if( PASSABLE(nRet) && 
                            poDecode->i_nWspHdrPos - nOrigPos <= nValueLen )
                        {
                            nRet = HC_DecodeUintvar (poDecode->i_pbWspHeader, 
                                                    &poDecode->i_nWspHdrPos, 
                                                    &nLastBytePos,
                                                    poDecode->i_nWspHdrActLen+poDecode->i_nWspHdrDecLen-poDecode->i_nWspHdrPos);
                            if (PASSABLE(nRet))
                            {
                                UINT32toStr(nLastBytePos, acBuf, WPS_CODEC_BUF_SIZE);
                                nRet = HC_AppendText (poDecode, acBuf);
                            }
                        }
                    }
                }
            }
            else if (bCode == 129)
            {   /* Suffix-byte-range */
                nRet = HC_DecodeUintvar (poDecode->i_pbWspHeader, 
                                        &poDecode->i_nWspHdrPos, &nSuffixLength,
                                        poDecode->i_nWspHdrActLen+poDecode->i_nWspHdrDecLen-poDecode->i_nWspHdrPos);
                if (PASSABLE(nRet))
                {
/*                  if (poDecode->i_nLastFieldCode == WPS_HFC_DEF_RANGE) */
                    if (HC_RepeatField(poDecode,WPS_HFC_DEF_RANGE,1))
                    {   /* This is not the first Range field value */
                        strcpy(acBuf, "-");
                    }
                    else
                    {   /* This is the first Range field value */
                        strcpy(acBuf, "bytes=-");
                    }
                    nLen = HC_Strnlen(acBuf, WPS_CODEC_BUF_SIZE);
                    if (nLen==-1)
                    {
                        nRet = WPS_HC_ERROR_WSP_DATA_OVERFLOW;
                    }
                    else
                    {
                        UINT32toStr(nSuffixLength, &acBuf[nLen], WPS_CODEC_BUF_SIZE);
                        nRet = HC_AppendText (poDecode, acBuf);
                    }
                }
            }
            else
            {   /* Error - unknown Range specifier code! */
                nRet = WPS_HC_ERROR_WSP_UNKNOWN_RANGE_CODE;
            }
        }
    }
    return nRet;
} /* HC_DecodeFieldValue_Range */

/*
-----------------------------------------------------------------------------
    Decodes the Warning field value and appends 
    the decoded data to the HTTP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_DecodeFieldValue_Warning(
    DecodeData* poDecode)   /* in, out: Decode data. */
{
    HC_STATUS   nRet;
    UINT32      nValueLen   = 0;
    BYTE        bFirstByte  = 0;
    
    nRet = HC_ReadOneByte(poDecode, &bFirstByte);
    
    if (PASSABLE(nRet))
    {
        if (OCTET_IS_BINARY (bFirstByte))
        { /* General form: Value-length first */
            nRet = HC_DecodeLength (poDecode, &nValueLen, TRUE);
            if (PASSABLE(nRet))
            {   /* Warn-code */
                nRet = HC_DecodeInteger (poDecode);
                if (PASSABLE(nRet))
                {
                    nRet = HC_AppendText (poDecode, " ");
                    if (PASSABLE(nRet))
                    {   /* Warn-agent */
                        nRet = HC_DecodeText (poDecode, FALSE, FALSE);
                        if (PASSABLE(nRet))
                        {
                            nRet = HC_AppendText (poDecode, " ");
                            if (PASSABLE(nRet))
                            {   /* Warn-text */
                                nRet = HC_DecodeText (poDecode, TRUE, FALSE);
                            if (PASSABLE(nRet))
                                {
                                }
                        }   
                    }   
                    }
                }
            }
        }
        else
        {
            nRet = HC_DecodeInteger (poDecode);
        }
    }
    return nRet;
} /* HC_DecodeFieldValue_Warning */


/* New functions added for WAP Version 1.2 */

/*
-----------------------------------------------------------------------------
    Decodes the Application-id-value field value and appends 
    the decoded data to the HTTP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/

HC_STATUS HC_DecodeFieldValue_ApplicationIdValue(
    DecodeData* poDecode)  /* in,out : Decode data */
{
    BYTE first_byte;
    HC_STATUS  nRet    = WPS_HC_STATUS_OK;

    nRet = HC_ReadOneByte(poDecode, &first_byte);
    
    if (PASSABLE(nRet)) {
        if (OCTET_IS_TEXT(first_byte))
            nRet = HC_DecodeText(poDecode, FALSE, TRUE);
        else
            nRet = HC_DecodeInteger(poDecode);
    }

    return nRet;
}

/*
-----------------------------------------------------------------------------
    Decodes the Accept-application field value and appends 
    the decoded data to the HTTP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/


HC_STATUS HC_DecodeFieldValue_AcceptApplicationValue(
    DecodeData* poDecode)   /* in, out : Decode data */
{
    BYTE first_byte;
    HC_STATUS nRet = WPS_HC_STATUS_OK;

    nRet = HC_ReadOneByte(poDecode, &first_byte);
    if (PASSABLE(nRet)) {
        if (first_byte == 128) {
            poDecode->i_nWspHdrPos++;
            nRet = HC_AppendText(poDecode,"*");
        } else {
            nRet = HC_DecodeFieldValue_ApplicationIdValue(poDecode);
        }
    }

    return nRet;
}

/*
-----------------------------------------------------------------------------
    Decodes a Short-integer field value and appends 
    the decoded data to the HTTP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/


HC_STATUS HC_DecodeShortInteger(
    DecodeData* poDecode) /* in, out : Decode data structure */
{
    HC_STATUS   nRet    = WPS_HC_STATUS_OK;
    BYTE        bOctet  = 0;
    char        pStr[10];
                    
    nRet = HC_ReadOneByte(poDecode, &bOctet);
    if (PASSABLE(nRet))
    {
        poDecode->i_nWspHdrPos++;
        if (OCTET_IS_SHORTINT (bOctet)) 
        {
            nRet = UINT32toStr(DECODE_SHORTINT (bOctet), pStr, 10);                                
            if (PASSABLE(nRet))
                nRet = HC_AppendText (poDecode, pStr);                             
        }
    }

    return nRet;
}

/*
----------------------------------------------------------------------------
    This function is to return the textual representation of the given byte value,
    supposing it is a Profile-Warning field. (8.4.2.62).
    If it is out of the range of the valid value, it returns NULL
----------------------------------------------------------------------------
*/

const char* HC_Textualize_ProfileWarningCode(
    BYTE byte)              
{
    switch (byte) {
        case 0x90:
            return("100");
        case 0x91:
            return("101");
        case 0x92:
            return("102");
        case 0xA0:
            return("200");
        case 0xA1:
            return("201");
        case 0xA2:
            return("202");
        case 0xA3:
            return("203");
        default:
            return NULL;
    }
    
}

/*
----------------------------------------------------------------------------
    This function is to return the textual representation of the comment
    corresponding to a Profile-Warning field. (8.4.2.62, W3CNote: NOTE-CCPPExchange)
    If it is out of the range of the valid value, it returns NULL
----------------------------------------------------------------------------
*/

const char* HC_Textualize_ProfileWarningComment(
    BYTE byte)              
{
    switch (byte) {
        case 0x90:
            return("\"OK\"");
        case 0x91:
            return("\"Used stale profile\"");
        case 0x92:
            return("\"Not used profile\"");
        case 0xA0:
            return("\"Not applied\"");
        case 0xA1:
            return("\"Content selection applied\"");
        case 0xA2:
            return("\"Content generation applied\"");
        case 0xA3:
            return("\"Transformation applied\"");
        default:
            return NULL;
    }
    
}

/*
-----------------------------------------------------------------------------
    Decodes a Profile-warning field value and appends 
    the decoded data to the HTTP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
    Remark: Inconsistency in http and WAP spacification (is lone Warn-code 
            allowed and what to do with comments)
-----------------------------------------------------------------------------
*/


HC_STATUS HC_DecodeFieldValue_ProfileWarning(
    DecodeData* poDecode)   /* in, out : Decode data structure */
{
    HC_STATUS nRet = WPS_HC_STATUS_OK;
    UINT32 orig_pos    = poDecode->i_nWspHdrPos;    
    UINT32 value_len;
    BYTE first_byte;    
    const char *codetext;
    const char *warningtext = NULL;
    
    nRet = HC_ReadOneByte(poDecode, &first_byte);
    if (PASSABLE(nRet)) {
        switch (first_byte) {
        case 0x90:
        case 0x91:
        case 0x92:
        case 0xA0:
        case 0xA1:
        case 0xA2:
        case 0xA3:
            /* Warn-code */
            if ( (codetext = HC_Textualize_ProfileWarningCode(first_byte)) != NULL) {
                nRet = HC_AppendText(poDecode,codetext);
            } else {
                /* Well, this case is impossible unter normal circumstances, but I don't 
                   want to risk a crash */
                nRet = 1;
            }
            poDecode->i_nWspHdrPos++;
            break;
        default:
            /* (Value-length Warn-code Warn-target *Warn-date) */
            /* Value-length */
            nRet = HC_DecodeLength (poDecode, &value_len, TRUE);
            
            /* Warn-code (as above, but here is must fall into the right range */
            if (PASSABLE(nRet)) {
                nRet = HC_ReadOneByte(poDecode, &first_byte);
                poDecode->i_nWspHdrPos++;
                if ( (codetext = HC_Textualize_ProfileWarningCode(first_byte)) != NULL) {
                    nRet = HC_AppendText(poDecode,codetext);
                    if (PASSABLE(nRet))
                       HC_AppendText(poDecode," ");
                } else {
                    nRet = WPS_HC_ERROR_WSP_UNKNOWN_WARNING_CODE;
                }
            }
        
            /* Warn-target */
            if (PASSABLE(nRet)) 
                nRet = HC_DecodeText (poDecode, FALSE, TRUE);
            if (PASSABLE(nRet)) 
                nRet = HC_AppendText(poDecode," ");
            
            /* It is possible to have a empty string here, but don't put superfluous space 
                after it */
            if (nRet == WPS_HC_ERROR_WSP_EMPTY_TEXT)
                nRet = WPS_HC_STATUS_OK;

            /* Warn-comment (see W3C note) */
            if ((PASSABLE(nRet)) &&
                ( (warningtext = HC_Textualize_ProfileWarningComment(first_byte)) != NULL) )
            {
                nRet = HC_AppendText(poDecode,warningtext);
            }
            
            /* Warn-dates */
            while ( PASSABLE(nRet) && 
                poDecode->i_nWspHdrPos - orig_pos <= value_len )
            {
                if (PASSABLE(nRet)) 
                    nRet = HC_AppendText(poDecode," \"");
                if (PASSABLE(nRet)) 
                    nRet = HC_DecodeFieldValue_Date (poDecode, FALSE);
                if (PASSABLE(nRet)) 
                    nRet = HC_AppendText(poDecode,"\"");
            }
            break;
        } /* switch */
    
    } /* if NO_ERROR */

    return nRet;
}

/*
-----------------------------------------------------------------------------
    Decodes the next field and appends the decoded data 
    to the HTTP buffer.
    Returns WPS_HC_STATUS_OK if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_DecodeField(
    DecodeData* poDecode,   /* in, out: Decode data. */
    BYTE        bCodePage)  /* in:      The actual header code page number. */
{
    HC_STATUS   nRet        = WPS_HC_STATUS_OK;
    UINT16      iFieldCode  = DECODE_SHORTINT (poDecode->i_pbWspHeader
                                               [poDecode->i_nWspHdrPos++]);
    const char* pcTmp       = HC_FieldCode2FieldName(bCodePage,NULL,
                                    (BYTE)iFieldCode,poDecode->i_version);
    /* Save the current HTTP buffer position. If the decoding of the field value fails, 
       we will jump back to this saved position */

    UINT32 HttpBufPosBeforeEncoding = poDecode->i_nHttpBufPos; 

    /* Do we have the same header field as de previous one ? */
/*    BOOL   bIsMultiple = (poDecode->i_nLastFieldCode == bFieldCode); */
    BOOL   bIsMultiple = HC_RepeatField(poDecode,iFieldCode,bCodePage);

    if (pcTmp)
    {
        if (!bIsMultiple)
        {   
            /* Common case - append the field name and the separator */
            nRet = HC_AppendText (poDecode, pcTmp);
            if (PASSABLE(nRet))
            {   /* Append the field value separator */
                nRet = HC_AppendText (poDecode, ": ");
            }
        }
        else
        {   /* Special case - concatenate multiple field values of the same type into one */

            /* Seek WPS_HC_NEWLINE_SIZE position back */
            poDecode->i_nHttpBufPos = poDecode->i_nHttpBufPos - (WPS_HC_NEWLINE_SIZE);

            /* Save the current HTTP buffer position */
            HttpBufPosBeforeEncoding = poDecode->i_nHttpBufPos;

            nRet = HC_AppendText (poDecode, ", ");

        }

        if (PASSABLE(nRet))
        {
            switch (iFieldCode)
            {
            case WPS_HFC_DEF_ACCEPT                  :
                nRet = HC_DecodeMediaValue (poDecode, 
                                            HC_ContentTypeCode2ContentTypeName, 
                                            FALSE);
                break;
            case WPS_HFC_DEF_ACCEPT_CHARSET          :
                nRet = HC_DecodeMediaValue (poDecode, HC_CharsetCode2CharsetName, 
                                            TRUE);
                break;
            case WPS_HFC_DEF_ACCEPT_ENCODING         :
            case WPS_HFC_DEF_CONTENT_ENCODING        :
                nRet = HC_DecodeByTable (poDecode, g_aContentEncodingValues, 
                                         DIM (g_aContentEncodingValues), 
                                         WPS_HC_ERROR_WSP_INVALID_CONTENT_ENC);
                break;
            case WPS_HFC_DEF_ACCEPT_LANGUAGE         :
                nRet = HC_DecodeMediaValue (poDecode, HC_LangCode2LangName, TRUE);
                break;
            case WPS_HFC_DEF_ACCEPT_RANGES           :
                nRet = HC_DecodeByTable (poDecode, g_aAcceptRangesValues, 
                                         DIM (g_aAcceptRangesValues), 
                                         WPS_HC_ERROR_WSP_INVALID_ACCEPT_RANGE);
                break;
            case WPS_HFC_DEF_AGE                     : /* Delta-seconds-value */
            case WPS_HFC_DEF_CONTENT_LENGTH          : /* Integer-value */
            case WPS_HFC_DEF_MAX_FORWARDS            : /* Integer-value */
                nRet = HC_DecodeInteger (poDecode);
                break;
            case WPS_HFC_DEF_ALLOW                   :
                nRet = HC_DecodeMethodName (poDecode);
                break;
            case WPS_HFC_DEF_AUTHORIZATION           :
            case WPS_HFC_DEF_PROXY_AUTHORIZATION     : /* Value-length Credentials */
                nRet = HC_DecodeFieldValue_Authorization (poDecode);
                break;
            case WPS_HFC_DEF_CACHE_CONTROL           :
                nRet = HC_DecodeFieldValue_CacheControl (poDecode);
                break;
            case WPS_HFC_DEF_CONNECTION              :
                nRet = HC_DecodeSingleCode (poDecode, WPS_HC_FV_CONNECTION_CLOSE, 
                                            WPS_HC_ERROR_WSP_INVALID_CONNECTION);
                break;
            case WPS_HFC_DEF_CONTENT_DISPOSITION     :
                nRet = HC_DecodeFieldValue_ContentDisposition (poDecode);
                break;
            case WPS_HFC_DEF_CONTENT_LANGUAGE        :
                nRet = HC_DecodeFieldValue_ContentLanguage (poDecode);
                break;
            case WPS_HFC_DEF_CONTENT_MD5             :
                nRet = HC_DecodeFieldValue_ContentMD5 (poDecode);
                break;
            case WPS_HFC_DEF_CONTENT_RANGE           :
                nRet = HC_DecodeFieldValue_ContentRange (poDecode);
                break;
            case WPS_HFC_DEF_CONTENT_TYPE            :
                nRet = HC_DecodeMediaValue (poDecode, 
                                            HC_ContentTypeCode2ContentTypeName, 
                                            FALSE);
                break;
            case WPS_HFC_DEF_DATE                    :
            case WPS_HFC_DEF_EXPIRES                 :
            case WPS_HFC_DEF_IF_MODIFIED_SINCE       :
            case WPS_HFC_DEF_IF_RANGE                :
            case WPS_HFC_DEF_IF_UNMODIFIED_SINCE     :
            case WPS_HFC_DEF_LAST_MODIFIED           : /* Date-value */
                nRet = HC_DecodeFieldValue_Date (poDecode, FALSE);
                break;
            case WPS_HFC_DEF_PRAGMA                  :
                nRet = HC_DecodeFieldValue_Pragma (poDecode);
                break;
            case WPS_HFC_DEF_PROXY_AUTHENTICATE      :
            case WPS_HFC_DEF_WWW_AUTHENTICATE        : /* Value-length Challenge */
                nRet = HC_DecodeFieldValue_ProxyAuthenticate (poDecode);
                break;
            case WPS_HFC_DEF_PUBLIC                  :
                nRet = HC_DecodeMethodName (poDecode);
                break;
            case WPS_HFC_DEF_RANGE                   :
                nRet = HC_DecodeFieldValue_Range (poDecode);
                break;
            case WPS_HFC_DEF_RETRY_AFTER             :
                nRet = HC_DecodeFieldValue_Date (poDecode, TRUE);
                break;
            case WPS_HFC_DEF_TRANSFER_ENCODING       :
                nRet = HC_DecodeSingleCode (poDecode, WPS_HC_FV_TRANSFER_ENC_CHUNKED, 
                                            WPS_HC_ERROR_WSP_INVALID_TRANSFER_ENC);
                break;
            case WPS_HFC_DEF_VARY                    :
                nRet = HC_DecodeWellknownFieldName (poDecode);
                break;
            case WPS_HFC_DEF_WARNING                 :
                nRet = HC_DecodeFieldValue_Warning (poDecode);
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
            case WPS_HFC_DEF_VIA                     : /* Text-string */
                nRet = HC_DecodeText (poDecode, FALSE, TRUE);
                break;

            default :
                /* WAP 1.2 specific fields */
                if (poDecode->i_version >= HC_WapVersion1_2) {
                    switch (iFieldCode)
                    {
                    case WPS_HFC_DEF_X_WAP_APPLICATION_ID:
                        nRet = HC_DecodeFieldValue_ApplicationIdValue(poDecode);
                        break;
                    
                    case WPS_HFC_DEF_X_WAP_CONTENT_URI:
                    case WPS_HFC_DEF_X_WAP_INITIATOR_URI:
                    case WPS_HFC_DEF_PROFILE:
                        nRet = HC_DecodeText(poDecode, FALSE, TRUE);
                        break;

                    case WPS_HFC_DEF_ACCEPT_APPLICATION:
                        nRet = HC_DecodeFieldValue_AcceptApplicationValue(poDecode);
                        break;

                    case WPS_HFC_DEF_BEARER_INDICATION:
                        nRet = HC_DecodeInteger(poDecode);
                        break;

                    case WPS_HFC_DEF_PUSH_FLAG:
                        nRet = HC_DecodeShortInteger(poDecode);
                        break;

                    case WPS_HFC_DEF_PROFILE_DIFF:
                        /* Yet unimplemented */
                        iFieldCode = HC_MALFORMED_FIELD;
                        break;

                    case WPS_HFC_DEF_PROFILE_WARNING:
                        nRet = HC_DecodeFieldValue_ProfileWarning(poDecode);
                        break;

                    default :
                        if (poDecode->i_version >= HC_WapVersion1_3) {
                            switch (iFieldCode)
                            {
                            case WPS_HFC_DEF_EXPECT:
                            case WPS_HFC_DEF_TE:
                            case WPS_HFC_DEF_TRAILER:
                                /* Yet unimplemented */
                                iFieldCode = HC_MALFORMED_FIELD;
                                break;

                            case WPS_HFC_DEF_ACCEPT_CHARSET13:
                                nRet = HC_DecodeMediaValue(poDecode, 
                                       HC_CharsetCode2CharsetName, TRUE);
                                break;

                            case WPS_HFC_DEF_ACCEPT_ENCODING13:
                                nRet = HC_DecodeByTable (poDecode, 
                                            g_aContentEncodingValues, 
                                            DIM (g_aContentEncodingValues), 
                                            WPS_HC_ERROR_WSP_INVALID_CONTENT_ENC);
                                break;

                            case WPS_HFC_DEF_CACHE_CONTROL13:
                                nRet = HC_DecodeFieldValue_CacheControl(poDecode);
                                break;

                            case WPS_HFC_DEF_CONTENT_RANGE13:
                                nRet = HC_DecodeFieldValue_ContentRange (poDecode);
                                break;

                            case WPS_HFC_DEF_X_WAP_TOD:
                                nRet = HC_DecodeFieldValue_Date (poDecode, FALSE);
                                break;

                            case WPS_HFC_DEF_CONTENT_ID:
                                nRet = HC_DecodeText(poDecode, TRUE, FALSE);
                                break;

                            case WPS_HFC_DEF_SET_COOKIE:
                            case WPS_HFC_DEF_COOKIE:
                            case WPS_HFC_ENCODING_VERSION:
                                /* Yet unimplemented */
                                iFieldCode = HC_MALFORMED_FIELD;
                                break;

                            default:
                                nRet = SetError(poDecode,WPS_HC_ERROR_WSP_INVALID_FIELD_CODE);
                                break;
                            }
                        } else {
                            nRet = SetError(poDecode,WPS_HC_ERROR_WSP_INVALID_FIELD_CODE);
                        }
                    }
                } else {
                    nRet = SetError(poDecode,WPS_HC_ERROR_WSP_INVALID_FIELD_CODE);
                }
                break;
            }

            /* If something unusual has happened */
            if (nRet != WPS_HC_STATUS_OK) {
                if (WPS_HC_STATUS_IS_WARNING(nRet)) {
                    SetWarning(poDecode,nRet);
                } else {
                    SetError(poDecode,nRet);
                }
                if (!(WRITEABLE(nRet))) {
                    iFieldCode = HC_MALFORMED_FIELD;
                }
            }

            HC_SetLastFieldCode(poDecode,iFieldCode);

            if (PASSABLE(nRet) && iFieldCode != HC_MALFORMED_FIELD) {
                nRet = HC_AppendText (poDecode, WPS_HC_NEWLINE);
            }

            if (!WRITEABLE(nRet))
                nRet = WPS_HC_WARNING_MALFORMED_HEADER_SKIPPED;

            if (iFieldCode == HC_MALFORMED_FIELD) {
                poDecode->i_nHttpBufPos = HttpBufPosBeforeEncoding;
                poDecode->i_acHttpBuf[poDecode->i_nHttpBufPos] = '\0';
                SetWarning(poDecode,WPS_HC_WARNING_MALFORMED_HEADER_SKIPPED);
                if (bIsMultiple) {
                    HC_AppendText(poDecode,WPS_HC_NEWLINE);
                }
            }

            if (PASSABLE(nRet))
                nRet = WPS_HC_STATUS_OK;
        }

    }
    else
    {   /* Error - invalid field code! */
        poDecode->i_nHttpBufPos = HttpBufPosBeforeEncoding;
        poDecode->i_acHttpBuf[poDecode->i_nHttpBufPos] = '\0';
        SetError(poDecode,WPS_HC_ERROR_WSP_INVALID_FIELD_CODE);
        SetWarning(poDecode,WPS_HC_WARNING_UNKNOWN_HEADER_SKIPPED);
        nRet = WPS_HC_STATUS_OK;
    }

    return nRet;
} /* HC_DecodeField */

/*
-----------------------------------------------------------------------------
    API implementation
-----------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------
*/
HC_STATUS HC_DecodeHeader(
    const BYTE*         pbWspHeader, 
    UINT32              nWspHdrLen, 
    UINT32              nContentLen, 
    const NameBytePair* pCodePages, 
    UINT8               nNumCodePages, 
    BITF                bFlags, 
    HC_WapVersion       wapVersion,
    char*               pcHttpHeader, 
    UINT32*             pnHttpHdrLen)
{
    HC_STATUS   nRet        = WPS_HC_STATUS_OK;
    BYTE        bCodePage   = WPS_HCP_DEFAULT;
    DecodeData  oDecode;

    oDecode.i_pbWspHeader       = pbWspHeader;
    oDecode.i_nWspHdrPos        = 0;
    oDecode.i_nWspHdrLen        = nWspHdrLen;
    oDecode.i_nWspHdrActLen     = 0;
    oDecode.i_nWspHdrDecLen     = 0;
    oDecode.i_nContentLen       = nContentLen;
    oDecode.i_nHttpBufPos       = 0;
    oDecode.i_nWarnings         = WPS_HC_STATUS_OK;
    oDecode.i_nLastFieldCode    = 0xFFFF;
    oDecode.i_nLastValidFieldCode = 0xFFFF;
    oDecode.i_nFirstError       = WPS_HC_STATUS_OK;
    oDecode.i_version           = wapVersion;
    
    /* Sanity check */
    if (wapVersion < HC_WapVersionLowest || wapVersion > HC_WapVersionLatest) {
        return WPS_CODEC_ERROR_INVALID_PARAMETER;
    }
    if (pbWspHeader == NULL || pcHttpHeader == NULL || pnHttpHdrLen == NULL) {
        return WPS_CODEC_ERROR_INVALID_PARAMETER;
    }

    /* If the nNumCodePages is not 0, the pCodePages can not be NULL */
    if (nNumCodePages!=0 && pCodePages == NULL) {
        return WPS_CODEC_ERROR_INVALID_PARAMETER;
    }

    if ( !(bFlags & WPS_HC_OPT_DONT_SEPARATE_CONTENTTYPE) )
    {   /* Decode the Content-Type value first */

        const char* pcTmp   = NULL;
        BYTE bFirstByte;

        if (nWspHdrLen!=0)
        {
            nRet = HC_ReadOneByte(&oDecode, &bFirstByte);
            if (PASSABLE(nRet)) {
                if (bFirstByte == 0) {
                    oDecode.i_nWspHdrPos++;
                    oDecode.i_nWspHdrActLen++;
                    oDecode.i_nWspHdrDecLen++;
                } else {
                   
                    pcTmp = HC_FieldCode2FieldName(WPS_HCP_DEFAULT, NULL, 
                                                   WPS_HFC_DEF_CONTENT_TYPE,
                                                   wapVersion);
        
                    /* Append the field name and the separator */
                    nRet = HC_AppendText (&oDecode, pcTmp);
                    if (PASSABLE(nRet))
                    {
                        nRet = HC_AppendText (&oDecode, ": ");
                        if (PASSABLE(nRet))
                        {   /* Append the field value */
                            
                            if (PASSABLE(nRet))
                            {   /* check the header field name length if it is fit to wsp buffer */
                                nRet = HC_CheckLength(&oDecode, FALSE);
                            }
                            nRet = HC_DecodeMediaValue (&oDecode, 
                                                        HC_ContentTypeCode2ContentTypeName, 
                                                        FALSE);
                            if( PASSABLE(nRet) )
                            {
                                nRet = HC_AppendText (&oDecode, WPS_HC_NEWLINE);
                            }
        
                            oDecode.i_nWspHdrDecLen += oDecode.i_nWspHdrActLen;
                        }
                    }
                }
            }
        }
    }
    while ( PASSABLE(nRet) && 
            oDecode.i_nWspHdrPos < nWspHdrLen )
    {
        BYTE    bCodePageShift  = bCodePage;

        if (OCTET_IS_BINARY (pbWspHeader[oDecode.i_nWspHdrPos]))
        {   /* Short-cut code page shift */
            bCodePageShift = pbWspHeader[oDecode.i_nWspHdrPos];

            /* Increase the decoded buffer length */
            oDecode.i_nWspHdrPos ++;
            oDecode.i_nWspHdrDecLen ++;

        }
        else if (pbWspHeader[oDecode.i_nWspHdrPos] == WPS_HC_SHIFT_DELIMITER)
        {   /* Code page shift delimiter */
            oDecode.i_nWspHdrPos++;

            /* Short-cut code page shift */
            bCodePageShift = pbWspHeader[oDecode.i_nWspHdrPos++];
            
            /* Increase the decoded buffer length */
            oDecode.i_nWspHdrDecLen +=2;
        }
        else if (OCTET_IS_TEXT (pbWspHeader[oDecode.i_nWspHdrPos]))
        {   /* Textual field name */
            if (PASSABLE(nRet))
            {   /* check the header field name length if it is fit to wsp buffer */
                nRet = HC_CheckLength(&oDecode, TRUE);
            }
            if (PASSABLE(nRet))
            {
                nRet = HC_DecodeText (&oDecode, FALSE, FALSE);
            }
            if (PASSABLE(nRet))
            {
                nRet = HC_AppendText (&oDecode, ": ");
            }
            if (PASSABLE(nRet))
            {   /* check the header value length if it is fit to wsp buffer */
                nRet = HC_CheckLength(&oDecode, FALSE);
            }
            if (PASSABLE(nRet))
            {
                nRet = HC_DecodeUnknown (&oDecode);
            }
            if (PASSABLE(nRet))
            {   /* Append the field separator */
                nRet = HC_AppendText (&oDecode, WPS_HC_NEWLINE);
            }
            /* The last field was non-standard - avoid mixing different fields */
            oDecode.i_nLastFieldCode = 0xFFFF;
            oDecode.i_nLastValidFieldCode = 0xFFFF;           

            /* Increase the decoded buffer length */
            oDecode.i_nWspHdrDecLen += oDecode.i_nWspHdrActLen;
        }
        else
        {   /* Well-known field name */
            if (PASSABLE(nRet))
            {   /* check the header value length if it is fit to wsp buffer */
                nRet = HC_CheckLength(&oDecode, TRUE);
            }
            if (PASSABLE(nRet))
            {

                nRet = HC_DecodeField (&oDecode, bCodePage);
                /* We don't have to bother about the return value, since
                   HC-DecodeField handles the error cases - skips the invalid fields*/

                if (bFlags & WPS_HC_OPT_STOP_AT_ERROR)
                    nRet = oDecode.i_nFirstError;

                /* Increase the decoded buffer length */
                oDecode.i_nWspHdrDecLen += oDecode.i_nWspHdrActLen;
            }
        }
        
        if (bCodePageShift != bCodePage)
        {   /* A code page shift occurred - check if it is legal */
            BOOL bFound = ( bCodePageShift >= WPS_HCP_WAP_FIRST && 
                            bCodePageShift <= WPS_HCP_WAP_LAST );
            int i;

            for (i = 0; i < nNumCodePages; i++)
            {
                bFound = (bCodePageShift = pCodePages[i].i_bValue);
            }
            if (bFound)
            {   /* Code page shift is accepted */
                bCodePage = bCodePageShift;
                /* Changing code page - avoid mixing different fields */
                oDecode.i_nLastFieldCode = 0xFFFF;
                oDecode.i_nLastValidFieldCode = 0xFFFF;
            }
            else
            {   /* Shift to an unknown code page */

                if (bCodePageShift >= WPS_HCP_WAP_FIRST_RESERVED && bCodePageShift <= WPS_HCP_WAP_LAST_RESERVED) {
                    oDecode.i_nWarnings |= WPS_HC_WARNING_RESERVED_CODEPAGE_SKIPPED;
                } else {
                    oDecode.i_nWarnings |= WPS_HC_WARNING_UNKNOWN_CODEPAGE_SKIPPED;
                }

                /* Skip until we found the next code code page shift */
                while (PASSABLE(nRet) && !OCTET_IS_SHIFT_SEQUENCE (pbWspHeader[oDecode.i_nWspHdrPos])) {
                    /* Calculate the length of the actual field */
                    nRet = HC_CheckLength(&oDecode, TRUE);

                    /* Increase the decoded buffer length */
                    oDecode.i_nWspHdrPos+= oDecode.i_nWspHdrActLen;
                    oDecode.i_nWspHdrDecLen+= oDecode.i_nWspHdrActLen;
                }
            }
        }
        oDecode.i_nWspHdrPos = oDecode.i_nWspHdrDecLen;
    }
    if (oDecode.i_nHttpBufPos < *pnHttpHdrLen)
    {   /* Result fits in the destination buffer - copy it */
        if (oDecode.i_nHttpBufPos > 0)
        {
            strcpy (pcHttpHeader, oDecode.i_acHttpBuf);
        }
    }
    else
    {   /* Error - the given HTTP buffer is too small! */
        nRet = WPS_CODEC_ERROR_DEST_BUFFER_TOO_SMALL;
        /* Add one byte for the trailing \0 */
        oDecode.i_nHttpBufPos++;
    }
    *pnHttpHdrLen = oDecode.i_nHttpBufPos;


    /* If no error in the outside loop - look for the inner errors */
    if (nRet == WPS_HC_STATUS_OK) {
        if (bFlags & WPS_HC_OPT_PREFER_ERROR) {
            if (oDecode.i_nFirstError != WPS_HC_STATUS_OK)
                nRet = oDecode.i_nFirstError;
            else
                nRet = oDecode.i_nWarnings;
        } else {
            nRet = oDecode.i_nWarnings;
        }

        if ((bFlags & WPS_HC_OPT_STOP_AT_ERROR) && oDecode.i_nFirstError != WPS_HC_STATUS_OK) {
            nRet = oDecode.i_nFirstError;
        } else {
            nRet = oDecode.i_nWarnings;
        }

    }
    
    return nRet;
} /* HC_DecodeHeader */



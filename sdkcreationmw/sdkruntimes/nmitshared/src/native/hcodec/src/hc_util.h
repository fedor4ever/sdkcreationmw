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
/* File    : T:\wpsvob\wsp\work\codecs\headercodec\hc_util.h
   Version : \main\25
   Date    : 02-May-00.16:05:58
*/

/*
 * ========================================================================
 *  Name        : hc_util.h
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

    DESCRIPTION

    Contains utility functions for the WSP header codec.

-----------------------------------------------------------------------------
*/

#ifndef HC_Util_h
#define HC_Util_h

/*
-----------------------------------------------------------------------------
    Includes
-----------------------------------------------------------------------------
*/

#include "hcodec.h"

#include <stdlib.h>

#ifdef _DEBUG
#include <assert.h>
#endif /* _DEBUG */

/*
-----------------------------------------------------------------------------
    Defines
-----------------------------------------------------------------------------
*/

#define CHAR    HC_CHAR
#define UCHAR   HC_UCHAR
#define BYTE    HC_BYTE
#define WORD    HC_WORD
#define DWORD   HC_DWORD
#define UINT8   HC_UINT8
#define UINT16  HC_UINT16
#define UINT32  HC_UINT32
#define INT8    HC_INT8
#define INT16   HC_INT16
#define INT32   HC_INT32
#define BITS8   HC_BITS8
#define BITS16  HC_BITS16
#define BITS32  HC_BITS32
#define BITF    HC_BITF

#define NameBytePair HC_NameBytePair
#define NameWordPair HC_NameWordPair

#define BOOL    int
#define TRUE    1
#define FALSE   0

/* Useful macros */
#define DIM(array)                  sizeof (array) / sizeof (array[0])

#define min(a, b)                   (((a) < (b)) ? (a) : (b))

#define CODEC_NULLIFY(ptr)          if (ptr) { WSL_OS_Free (ptr); ptr = NULL; }

#ifdef _DEBUG
#define CODEC_ASSERT(x)             assert(x)
#else
#define CODEC_ASSERT(x)             ((void)0)
#endif /* _DEBUG */

/* Special octet values */
#define WPS_CODEC_MIN_BINARY        0x00
#define WPS_CODEC_MAX_BINARY        0x1F
#define WPS_CODEC_MIN_TEXT          0x20
#define WPS_CODEC_MAX_TEXT          0x7F
#define WPS_CODEC_MIN_SHORTINT      0x80
#define WPS_CODEC_MAX_SHORTINT      0xFF
#define WPS_CODEC_MAX_DEC_SHORTINT  0x7F
/* Some useful macros for range checking of octet values */
#define OCTET_IS_BINARY(x)          ((x) <= WPS_CODEC_MAX_BINARY)
#define OCTET_IS_TEXT(x)            ((x) >= WPS_CODEC_MIN_TEXT && \
                                     (x) <= WPS_CODEC_MAX_TEXT)
#define OCTET_IS_SHORTINT(x)        ((x) >= WPS_CODEC_MIN_SHORTINT)
#define DECODE_SHORTINT(x)          (BYTE)((x) & 0x7F)

#define WPS_CODEC_NON7BIT_QUOTE     0x7F

/* Predefined parameter codes for media types. */
#define WPS_HFC_PARAM_Q                     0x00
#define WPS_HFC_PARAM_CHARSET               0x01
#define WPS_HFC_PARAM_LEVEL                 0x02
#define WPS_HFC_PARAM_TYPE                  0x03
/* Removed after the Kyoto WSP Corrigendum
#define WPS_HFC_PARAM_UAPROF                0x04*/
#define WPS_HFC_PARAM_NAME                  0x05
#define WPS_HFC_PARAM_FILENAME              0x06
#define WPS_HFC_PARAM_DIFFERENCES           0x07
#define WPS_HFC_PARAM_PADDING               0x08

/* Cache-Control value codes. */
#define WPS_HFC_CACHECTL_NO_CACHE           128
#define WPS_HFC_CACHECTL_NO_STORE           129
#define WPS_HFC_CACHECTL_MAX_AGE            130
#define WPS_HFC_CACHECTL_MAX_STALE          131
#define WPS_HFC_CACHECTL_MIN_FRESH          132
#define WPS_HFC_CACHECTL_ONLY_IF_CACHED     133
#define WPS_HFC_CACHECTL_PUBLIC             134
#define WPS_HFC_CACHECTL_PRIVATE            135
#define WPS_HFC_CACHECTL_NO_TRANSFORM       136
#define WPS_HFC_CACHECTL_MUST_REVALIDATE    137
#define WPS_HFC_CACHECTL_PROXY_REVALIDATE   138

/* Common field values. */
#define WPS_HC_FV_AUTH_SCHEME_BASIC         "Basic"
#define WPS_HC_FV_AUTH_SCHEME_BASIC_LC      "basic"
#define WPS_HC_FV_PRAGMA_NO_CACHE           "no-cache"
#define WPS_HC_FV_CONNECTION_CLOSE          "close"
#define WPS_HC_FV_TRANSFER_ENC_CHUNKED      "chunked"

/* Special characters and character sets */
#define WPS_HC_FIELD_SEPARATOR  ':'
#define WPS_HC_VALUE_SEPARATOR  ','
#define WPS_HC_PARAM_SEPARATOR  ';'
#define WPS_HC_EQUAL_SIGN       '='
#define WPS_HC_WHITESPACES      " \t"
#define WPS_HC_NEWLINE          "\r\n"
#define WPS_HC_NEWLINE_SIZE     2
#define WPS_HC_TERMINATORS      "\r\n\0"

/* Special octet values */
#define WPS_HC_LAST_SHORT_CUT_SHIFT_DELIMITER   0x1F
#define WPS_HC_SHIFT_DELIMITER                  0x7F
#define WPS_HC_LENGTH_QUOTE                     0x1F

#define OCTET_IS_SHIFT_SEQUENCE(x)          (((x) <= WPS_HC_LAST_SHORT_CUT_SHIFT_DELIMITER) || ((x) == WPS_HC_SHIFT_DELIMITER))

/*--------------------------------------------------
        General macros for the header codec
---------------------------------------------------- */

/* NOT_ERROR means, that the return code doesn't indicate severe error. Included instead of 
   ret == WPS_NO_ERROR, because we would like to continue in case of warnings.
   As you can see in the definition, it simply means not-error */

#define HC_NOT_ERROR(x)   (!WPS_HC_STATUS_IS_ERROR(x))

/* Writeable means that the field value can be printed out, but something tructation, correction 
   or something like that happened to the value. Anyway, it is more or less correct, or at
   least some non-empty, meaningful output can be produced */

#define HC_WRITEABLE(x)      ((x) == WPS_NO_ERROR || \
                             (x) == WPS_HC_WARNING_NUMBER_TRUNCATED)


/*
-----------------------------------------------------------------------------
    Exported tables
-----------------------------------------------------------------------------
*/

/*  Well-known Content-Disposition values. */
extern const NameBytePair g_aContentDispValues[2];

/*  Well-known Content-Encoding values. */
extern const NameBytePair g_aContentEncodingValues[3];

/*  Well-known Accept-Ranges values. */
extern const NameBytePair g_aAcceptRangesValues[2];

/*  Well-known Month values. */
extern const NameBytePair g_aMonths[12];

/*
-----------------------------------------------------------------------------
    Function declarations
-----------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------
    Looks for the content type code associated with the given content type
    name. The content type code is returned in the <pwValue> parameter
    in case of success; otherwise <pwValue> remains unchanged.
    Returns TRUE if succeeded, otherwise FALSE.
-----------------------------------------------------------------------------
*/
BOOL HC_ContentTypeName2ContentTypeCode(
    const char*   pcName,   /* in:  Content type name. */
    HC_WapVersion version,  /* in:  WAP version to comply with */
    WORD*         pwValue); /* out: The corresponding content type code. */

/*
-----------------------------------------------------------------------------
    Looks for the content type name associated with the given 
    content type code.
    Returns the content type name if succeeded, otherwise NULL.
-----------------------------------------------------------------------------
*/
const char* HC_ContentTypeCode2ContentTypeName(
    WORD    wValue,         /* in:  Content type code. */
    HC_WapVersion version); /* in:  WAP version to comply with */

/*
-----------------------------------------------------------------------------
    Looks for the language code associated with the given language name. The 
    language code is returned in the <pwValue> parameter in case of success; 
    otherwise <pwValue> remains unchanged.
    Returns TRUE if succeeded, otherwise FALSE.
-----------------------------------------------------------------------------
*/
BOOL HC_LangName2LangCode(
    const char* pcName,     /* in:  Language name. */
    HC_WapVersion version,  /* in:  WAP version to comply with */
    WORD*       pwValue);   /* out: The corresponding language code. */

/*
-----------------------------------------------------------------------------
    Looks for the language name associated with the given language code.
    Returns the language name if succeeded, otherwise NULL.
-----------------------------------------------------------------------------
*/
const char* HC_LangCode2LangName(
    WORD    wValue,         /* in:  Language code. */
    HC_WapVersion version); /* in:  WAP version to comply with */

/*
-----------------------------------------------------------------------------
    Looks for the character set code associated with the given character set 
    name. The charset code is returned in the <pwValue> parameter in case of 
    success; otherwise <pwValue> remains unchanged.
    Returns TRUE if succeeded, otherwise FALSE.
-----------------------------------------------------------------------------
*/
BOOL HC_CharsetName2CharsetCode(
    const char* pcName,     /* in:  Character set name. */
    HC_WapVersion version,  /* in:  WAP version to comply with */
    WORD*       pwValue);   /* out: The corresponding character set code. */

/*
-----------------------------------------------------------------------------
    Looks for the character set name associated with the given character 
    set code.
    Returns the charset name if succeeded, otherwise NULL.
-----------------------------------------------------------------------------
*/
const char* HC_CharsetCode2CharsetName(
    WORD          wValue,   /* in:  Character set code. */
    HC_WapVersion version); /* in:  WAP version to comply with */

/*
-----------------------------------------------------------------------------
    Looks for the parameter code associated with the given parameter 
    name. The parameter code is returned in the <pbValue> parameter
    in case of success; otherwise <pbValue> remains unchanged.
    Returns TRUE if succeeded, otherwise FALSE.
-----------------------------------------------------------------------------
*/
BOOL HC_ParamName2ParamCode(
    const char* pcName,     /* in:  Parameter name. */
    BYTE*       pbValue);   /* out: The corresponding parameter code. */

/*
-----------------------------------------------------------------------------
    Looks for the parameter name associated with the given 
    parameter code.
    Returns the parameter name if succeeded, otherwise NULL.
-----------------------------------------------------------------------------
*/
const char* HC_ParamCode2ParamName(
    BYTE    bValue);    /* in:  Parameter code. */

/*
-----------------------------------------------------------------------------
    Looks for the method code associated with the given method 
    name. The method code is returned in the <pbValue> parameter 
    in case of success; otherwise <pbValue> remains unchanged.
    Returns TRUE if succeeded, otherwise FALSE.
-----------------------------------------------------------------------------
*/
BOOL HC_MethodName2MethodCode(
    const char* pcName,     /* in:  Method name. */
    BYTE*       pbValue);   /* out: The corresponding method code. */

/*
-----------------------------------------------------------------------------
    Looks for the method name associated with the given 
    method code.
    Returns the method name if succeeded, otherwise NULL.
-----------------------------------------------------------------------------
*/
const char* HC_MethodCode2MethodName(
    BYTE    bValue);    /* in:  Method code. */

/*
-----------------------------------------------------------------------------
    Looks for the Cache-Control code associated with the given Cache-Control 
    name. The Cache-Control code is returned in the <pbValue> parameter 
    in case of success; otherwise <pbValue> remains unchanged.
    Returns TRUE if succeeded, otherwise FALSE.
-----------------------------------------------------------------------------
*/
BOOL HC_CacheCtlName2CacheCtlCode(
    const char* pcName,     /* in:  Cache-Control name. */
    BYTE*       pbValue);   /* out: The corresponding Cache-Control code. */

/*
-----------------------------------------------------------------------------
    Looks for the Cache-Control name associated with the given
    Cache-Control code.
    Returns the Cache-Control name if succeeded, otherwise NULL.
-----------------------------------------------------------------------------
*/
const char* HC_CacheCtlCode2CacheCtlName(
    BYTE    bValue);    /* in:  Cache-Control code. */

/*
-----------------------------------------------------------------------------
    Encodes the given character string using Base64 encoding.
    The buffer <pcEncodedText> should be at least 4/3 times the length
    of the text to be encoded (e.g. 60 ASCII characters are encoded into 
    80 base64 characters).
    Returns the effective length of the encoded text.
-----------------------------------------------------------------------------
*/
int HC_EncodeBase64(
    const char* pcOrigText,     /* in:  The text to be encoded. */
    char*       pcEncodedText,  /* out: The base64-encoded text. */
	UINT32		nOrigTextLen);	/* in: the length of the Origtext */

/*
-----------------------------------------------------------------------------
    Decodes the given Base64-encoded character string.
    The buffer <pcDecodedText> should be at least 3/4 times the length 
    of the text to be decoded (e.g. 80 base64 characters are decoded into 
    60 ASCII characters).
    Returns the effective length of the decoded text.
-----------------------------------------------------------------------------
*/
int HC_DecodeBase64(
    const char* pcOrigText,			/* in:  The base64-encoded text. */
    char*       pcDecodedText,		/* out: The decoded text. */
	UINT32		nOrigTextLen);		/* in: The length of the encoded text */

/*
-----------------------------------------------------------------------------
    Compares two case-insensitive stings 
-----------------------------------------------------------------------------
*/
UINT32 HC_strnicmp (const char *string1, const char *string2, UINT32 count);

/*
-----------------------------------------------------------------------------
    Determines whether the given header field may have multiple 
    field values separated by comma, or comma is legal in its value.
    Returns TRUE if commas in the field value are to be interpreted
    as value separators, otherwise FALSE.
-----------------------------------------------------------------------------
*/

BOOL HC_MayHaveMultipleValues(BYTE    bCodePage, BYTE    bFieldCode);


/* ------------------------------------------------------------- */

/* Other common functions */


/*
-----------------------------------------------------------------------------
    Lookup function type which receives a name and searches for a
    corresponding value.
    Returns TRUE if succeeded, otherwise FALSE.
-----------------------------------------------------------------------------
*/
typedef BOOL (*Name2WordFunc)(
    const char*   pcName,   /* in:  Content type name. */
    HC_WapVersion version,  /* in:  Version to comply with */
    WORD*         pwValue); /* out: The corresponding content type code. */

/*
-----------------------------------------------------------------------------
    Lookup function type which receives a value and searches for a
    corresponding name.
    Returns a pointer to the name if succeeded, otherwise NULL.
-----------------------------------------------------------------------------
*/
typedef const char* (*Word2NameFunc)(
    WORD          wValue,   /* in:  Content type code. */
    HC_WapVersion version); /* in:  Version to comply with */

/*
-----------------------------------------------------------------------------
    Function declarations
-----------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------
    Looks for the given name in the given table. If it is found, the
    corresponding byte value is filled in pbByte, otherwise pbByte remains
    unchanged.
    The table must be ordered by names.
    Returns TRUE if the name is found, otherwise FALSE.
-----------------------------------------------------------------------------
*/
BOOL HC_Name2Byte_Ord(
    const NameBytePair  aPairs[],       /* in:  Table of name-value pairs. */
    int                 nNumPairs,      /* in:  Number of elements in table. */
    BOOL                bIsLowerCase,   /* in:  Tells whether the table contains
                                                only lowercase names or not. */
    const char*         pcName,         /* in:  Name to look for. */
    BYTE*               pbByte);        /* out: The corresponding byte value. */

/*
-----------------------------------------------------------------------------
    Looks for the given byte value in the given table.
    The table must be ordered by numeric values.
    Returns a pointer to the name if it is found, otherwise NULL.
-----------------------------------------------------------------------------
*/
const char* HC_Byte2Name_Ord(
    const NameBytePair  aPairs[],   /* in:  Table of name-value pairs. */
    int                 nNumPairs,  /* in:  Number of elements in table. */
    BYTE                bByte);     /* in:  Byte value to look for. */

/*
-----------------------------------------------------------------------------
    Looks for the given name in the given table using linear search.
    If it is found, the corresponding byte value is filled in pbByte,
    otherwise pbByte remains unchanged.
    Returns TRUE if the name is found, otherwise FALSE.
-----------------------------------------------------------------------------
*/
BOOL HC_Name2Byte_Unord(
    const NameBytePair  aPairs[],       /* in:  Table of name-value pairs. */
    int                 nNumPairs,      /* in:  Number of elements in table. */
    BOOL                bIsLowerCase,   /* in:  Tells whether the table contains
                                                only lowercase names or not. */
    const char*         pcName,         /* in:  Name to look for. */
    BYTE*               pbByte);        /* out: The corresponding byte value. */

/*
-----------------------------------------------------------------------------
    Looks for the given byte value in the given table using linear search.
    Returns a pointer to the name if it is found, otherwise NULL.
-----------------------------------------------------------------------------
*/
const char* HC_Byte2Name_Unord(
    const NameBytePair  aPairs[],   /* in:  Table of name-value pairs. */
    int                 nNumPairs,  /* in:  Number of elements in table. */
    BYTE                bByte);     /* in:  Byte value to look for. */

/*
-----------------------------------------------------------------------------
    Compares two strings, regardless of their case.
    The return value indicates the relation of <pcString1> to <pcString2>
    as follows.
    Return Value        Description
        < 0             <pcString1> less than <pcString2>
        0               <pcString1> identical to <pcString2>
        > 0             <pcString1> greater than <pcString2>
-----------------------------------------------------------------------------
*/
int HC_stricmp(
    const char* pcString1,  /* in:  Null-terminated string to compare. */
    const char* pcString2); /* in:  Null-terminated string to compare. */

/*
-----------------------------------------------------------------------------
    Compares a normal string with a lowercase string, regardless of
    their case.
    The return value indicates the relation of <pcString1> to <pcString2>
    as follows.
    Return Value        Description
        < 0             <pcString1> less than <pcString2>
        0               <pcString1> identical to <pcString2>
        > 0             <pcString1> greater than <pcString2>
-----------------------------------------------------------------------------
*/
int HC_stricmp2(
    const char* pcString1,  /* in:  Null-terminated string to compare. */
    const char* pcString2); /* in:  Null-terminated lowercase string to compare. */

/*
-----------------------------------------------------------------------------
    Appends the given octet to the given buffer.
    Returns WPS_NO_ERROR if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeOctet(
    BYTE    bValue,         /* in:  Octet value. */
    BYTE*   pbDestBuf,      /* out: The encoded data. */
    UINT32* pnBufLen);      /* in:  The actual length of the destination buffer.
                               out: The new length of the destination buffer. */

/*
-----------------------------------------------------------------------------
    Appends the given short integer to the WSP buffer, with its
    most significant bit set to 1.
    Returns WPS_NO_ERROR if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
#define HC_EncodeShortInt(bValue, pbDestBuf, pnBufLen) \
		bValue > WPS_CODEC_MAX_DEC_SHORTINT ? \
		WPS_CODEC_ERROR_VALUE_OUT_OF_RANGE : \
        HC_EncodeOctet ((BYTE)(bValue | 0x80), pbDestBuf, pnBufLen)

/*
-----------------------------------------------------------------------------
    Encodes the given value into an uintvar and appends it to the
    destination buffer if it fits in.
    Returns WPS_NO_ERROR if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeUintvar(
    UINT32  nValue,         /* in:  The value to be encoded. */
    BYTE*   pbDestBuf,      /* out: The encoded data. */
    UINT32* pnBufLen);      /* in:  The actual length of the destination buffer.
                               out: The new length of the destination buffer. */

/*
-----------------------------------------------------------------------------
    Appends the given text to the destination buffer as a
    null-terminated string.
    Returns WPS_NO_ERROR if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_EncodeText(
    const char*     pcToken,        /* in:  Pointer to the token text. */
    BYTE*           pbDestBuf,      /* out: The encoded data. */
    UINT32*         pnBufLen,       /* in:  The actual length of the destination buffer.
                                       out: The new length of the destination buffer. */
    BOOL            bCheckQuotes);  /* in:  Determines whether to take care of quotes or not. */

/*
-----------------------------------------------------------------------------
    Parses an uintvar value out of the source buffer.
    Returns WPS_NO_ERROR if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_STATUS HC_DecodeUintvar(
    const BYTE* pbSrcBuf,   /* in, out: The source buffer. */
    UINT32*     pnSrcPos,   /* in, out: The actual position in the source buffer. */
    UINT32*     pnValue,	/* out:     Parsed integer value. */
	UINT32		nMaxLen);   /* in: max length of the buffer */

/*
-----------------------------------------------------------------------------
	Returns the length of the given string, if it contains \0. Otherwise
	returns -1. If nMeaxlen is 0 this function works as strlen.
*/
INT32 HC_Strnlen(
    const char* pbBuf,
    INT32		nMaxLen);

/*
-----------------------------------------------------------------------------
*/
HC_STATUS N_HC_EncodeOctet (
    BYTE    bValue,
    BYTE*   pbDestBuf,
    UINT32* pnDestBufLen,
    UINT32  nMaxDestBufLen);

/*
-----------------------------------------------------------------------------
*/
HC_STATUS N_HC_EncodeText (
    const char*     pcToken,
    UINT32          nTokenLen,
    BYTE*           pbDestBuf,
    UINT32*         pnDestBufLen,
    UINT32          nMaxDestBufLen,
    BOOL            bCheckQuote);




#endif /* HC_Util_h */

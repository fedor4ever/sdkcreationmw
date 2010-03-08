/*
* Copyright (c) 2000 - 2005 Nokia Corporation and/or its subsidiary(-ies).
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




/*
-----------------------------------------------------------------------------

    DESCRIPTION

    Contains the interface declarations for the WSP 
    header codec facility.

-----------------------------------------------------------------------------
*/

#ifdef HC_API_h
#  error "you should not include hc_api.h"
#endif /* HC_API_h */

#ifndef _HCODEC_H_
#define _HCODEC_H_ 1

/*
-----------------------------------------------------------------------------
    Includes
-----------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------
    Type definitions
-----------------------------------------------------------------------------
*/

typedef signed   char   HC_CHAR;
typedef unsigned char   HC_UCHAR;
typedef unsigned char   HC_BYTE;
typedef unsigned short  HC_WORD;
typedef unsigned int    HC_DWORD;
typedef unsigned char   HC_UINT8;
typedef unsigned short  HC_UINT16;
typedef unsigned int    HC_UINT32;
typedef signed   char   HC_INT8;
typedef signed   short  HC_INT16;
typedef signed   int    HC_INT32;
typedef unsigned char   HC_BITS8;
typedef unsigned short  HC_BITS16;
typedef unsigned int    HC_BITS32;
typedef unsigned short  HC_BITF;

typedef unsigned int    HC_STATUS;

/* Assigns a string token to a one-byte numeric identifier or vice versa. */
typedef struct _HC_NameBytePair
{
    const HC_CHAR * i_pcName;   /* Textual name. */
    HC_BYTE         i_bValue;   /* Numeric identifier. */
} HC_NameBytePair;

/* Assigns a string token to a two-byte numeric identifier or vice versa. */
typedef struct _HC_NameWordPair
{
    const HC_CHAR * i_pcName;   /* Textual name. */
    HC_WORD         i_wValue;   /* Numeric identifier. */
} HC_NameWordPair;

/*
-----------------------------------------------------------------------------
    Defines
-----------------------------------------------------------------------------
*/

#ifndef HC_EXPORT
#  define HC_EXPORT
#endif /* HC_EXPORT */

#define WPS_CODEC_BUF_SIZE      4096

/*
-----------------------------------------------------------------------------
    Option flags for the header codecs (HC).
-----------------------------------------------------------------------------
*/
/*  No specific requirements. */
#define WPS_HC_OPTION_NONE                      0
/*  Specifies that the Content-Type field should be handled just as 
    the other fields (by default, the value of this field is first in the 
    header, without its header field code).
    See also WSP spec, sections 8.2.3.2, 8.2.3.3, 8.2.4.1 and 8.5. */
#define WPS_HC_OPT_DONT_SEPARATE_CONTENTTYPE    1

/*
 * WAP version. NOTE: most gateways don't handle post 1.2 encoding
 */
typedef enum _HC_WapVersion {
    HC_WapVersionLowest = 1,
    HC_WapVersion1_1 = HC_WapVersionLowest,  /* WAP 1.1 */
    HC_WapVersion1_2,                        /* WAP 1.2 */
    HC_WapVersion1_3,                        /* WAP 1.3 */
    HC_WapVersion1_4,                        /* WAP 1.4 */
    HC_WapVersion1_5,                        /* WAP 1.5 */
    HC_WapVersionLatest = HC_WapVersion1_5,  /* latest supported version */
    HC_WapVersionDefault = HC_WapVersion1_2, /* accepted by most gateways */
} HC_WapVersion;

/*
-----------------------------------------------------------------------------
    Error codes for the codecs in general.
-----------------------------------------------------------------------------
*/

#define WPS_CODEC_STATUS_OK                      0

#define WPS_CODEC_STATUS_BASE                   (0x1000)

/*  The codec's internal buffer is too small. */
#define WPS_CODEC_ERROR_INT_BUFFER_TOO_SMALL    (WPS_CODEC_STATUS_BASE +  1)
/*  The destination buffer passed to the header encoder/decoder is too small. */
#define WPS_CODEC_ERROR_DEST_BUFFER_TOO_SMALL   (WPS_CODEC_STATUS_BASE +  3)
/*  The given HTTP buffer contains invalid shortint (more than 127). */
#define WPS_CODEC_ERROR_VALUE_OUT_OF_RANGE      (WPS_CODEC_STATUS_BASE +  5)
/*  Invalid parameter (NULL pointer, etc). */
#define WPS_CODEC_ERROR_INVALID_PARAMETER       (WPS_CODEC_STATUS_BASE +  7)

/*
-----------------------------------------------------------------------------
    Status codes and useful macros for the header codecs (HC) in general.
    Note: all error codes are odd, while warning codes are even!
    Error codes indicate that the input was malformed, and the 
    encoding/decoding was stopped.
    Warning codes indicate that the input was incorrect, however 
    the encoding/decoding was finished successfully. 
    The HC may return only one error code, but multiple warnings.
-----------------------------------------------------------------------------
*/
#define WPS_HC_STATUS_OK            WPS_CODEC_STATUS_OK
#define WPS_HC_STATUS_IS_OK(x)      ((x) == WPS_HC_STATUS_OK)
#define WPS_HC_STATUS_IS_ERROR(x)   ((x) & 1)
#define WPS_HC_STATUS_IS_WARNING(x) (!WPS_HC_STATUS_IS_OK(x) && !WPS_HC_STATUS_IS_ERROR(x))

#define WPS_HC_STATUS_BASE          (WPS_CODEC_STATUS_BASE + 20)

/*
-----------------------------------------------------------------------------
    Warning codes for the header codec.
-----------------------------------------------------------------------------
*/

#define WPS_HC_WARNING_BASE                     (0)

/* At least one of the header fields was malformed and skipped during the codec */
#define WPS_HC_WARNING_MALFORMED_HEADER_SKIPPED (WPS_HC_WARNING_BASE | 2)
    
/* At least one of the header fields was unknown and skipped during the decode */
#define WPS_HC_WARNING_UNKNOWN_HEADER_SKIPPED   (WPS_HC_WARNING_BASE | 4)

/*  Too big number in the input; value has been truncated to the largest 
    32-bit unsigned value (ULONG_MAX). */
#define WPS_HC_WARNING_NUMBER_TRUNCATED         (WPS_HC_WARNING_BASE | 8)

/* At least one of reserved code pages (2-15) was skipped during the decode */
#define WPS_HC_WARNING_RESERVED_CODEPAGE_SKIPPED   (WPS_HC_WARNING_BASE | 16)

/* At least one of unknown code pages (16-255) was skipped during the decode */
#define WPS_HC_WARNING_UNKNOWN_CODEPAGE_SKIPPED   (WPS_HC_WARNING_BASE | 32)

/*
-----------------------------------------------------------------------------
    Error codes for the header encoder.
-----------------------------------------------------------------------------
*/

/*  HTTP syntax error: illegal combination of newline characters 
    (the HC accepts '\n' or '\r\n'). */
#define WPS_HC_ERROR_HTTP_ILLEGAL_NEWLINE       (WPS_HC_STATUS_BASE + 31)
/*  HTTP syntax error: header field without field separator (':'). */
#define WPS_HC_ERROR_HTTP_NO_FIELD_SEPARATOR    (WPS_HC_STATUS_BASE + 33)
/*  HTTP error: failed to parse Q-value. */
#define WPS_HC_ERROR_HTTP_BAD_Q_VALUE           (WPS_HC_STATUS_BASE + 35)
/*  HTTP error: failed to parse date value. */
#define WPS_HC_ERROR_HTTP_BAD_DATE              (WPS_HC_STATUS_BASE + 37)
/*  HTTP error: failed to parse month value. */
#define WPS_HC_ERROR_HTTP_BAD_MONTH             (WPS_HC_STATUS_BASE + 39)
/*  HTTP error: invalid date value. */
#define WPS_HC_ERROR_HTTP_INVALID_DATE          (WPS_HC_STATUS_BASE + 41)
/*  HTTP error: unknown method name. */
#define WPS_HC_ERROR_HTTP_UNKNOWN_METHOD        (WPS_HC_STATUS_BASE + 43)
/*  HTTP error: unknown Content-Disposition value. */
#define WPS_HC_ERROR_HTTP_UNKNOWN_CONTENT_DISP  (WPS_HC_STATUS_BASE + 45)
/*  HTTP error: failed to parse Content-Range value. */
#define WPS_HC_ERROR_HTTP_BAD_CONTENT_RANGE     (WPS_HC_STATUS_BASE + 47)
/*  HTTP error: failed to parse Range value. */
#define WPS_HC_ERROR_HTTP_BAD_RANGE             (WPS_HC_STATUS_BASE + 49)
/*  HTTP error: failed to parse Warning value. */
#define WPS_HC_ERROR_HTTP_BAD_WARNING           (WPS_HC_STATUS_BASE + 51)
/*  HTTP error: invalid base 64 encoded string */
#define WPS_HC_ERROR_HTTP_INVALID_B64ENC_STRING (WPS_HC_STATUS_BASE + 53)
/*  HTTP error: bad authentication field */
#define WPS_HC_ERROR_HTTP_BAD_AUTHENTICATE		(WPS_HC_STATUS_BASE + 55)
/*  HTTP error: bad Content-MD5 field */
#define WPS_HC_ERROR_HTTP_BAD_CONTENTMD5		(WPS_HC_STATUS_BASE + 57)

/*
-----------------------------------------------------------------------------
    Error codes for the header decoder.
-----------------------------------------------------------------------------
*/

/*  WSP error: invalid header field code. */
#define WPS_HC_ERROR_WSP_INVALID_FIELD_CODE     (WPS_HC_STATUS_BASE + 81)
/*  WSP error: attempt to shift to an unknown header code page. (obsolete) */
#define WPS_HC_ERROR_WSP_ILLEGAL_CODEPAGE_SHIFT (WPS_HC_STATUS_BASE + 83)
/*  WSP error: invalid Content-Encoding value. */
#define WPS_HC_ERROR_WSP_INVALID_CONTENT_ENC    (WPS_HC_STATUS_BASE + 85)
/*  WSP error: invalid Accept-Range value. */
#define WPS_HC_ERROR_WSP_INVALID_ACCEPT_RANGE   (WPS_HC_STATUS_BASE + 87)
/*  WSP error: invalid Connection value. */
#define WPS_HC_ERROR_WSP_INVALID_CONNECTION     (WPS_HC_STATUS_BASE + 89)
/*  WSP error: invalid Transfer-Encoding value. */
#define WPS_HC_ERROR_WSP_INVALID_TRANSFER_ENC   (WPS_HC_STATUS_BASE + 91)
/*  WSP error: invalid Q-value code. */
#define WPS_HC_ERROR_WSP_INVALID_Q_CODE         (WPS_HC_STATUS_BASE + 93)
/*  WSP error: invalid Time prefix in the Retry-After value. */
#define WPS_HC_ERROR_WSP_INVALID_TIME_CODE      (WPS_HC_STATUS_BASE + 95)
/*  WSP error: unknown language code. */
#define WPS_HC_ERROR_WSP_UNKNOWN_LANG_CODE      (WPS_HC_STATUS_BASE + 97)
/*  WSP error: unknown Content-Type code. */
#define WPS_HC_ERROR_WSP_UNKNOWN_CNT_TYPE_CODE  (WPS_HC_STATUS_BASE + 99)
/*  WSP error: unknown charset code. */
#define WPS_HC_ERROR_WSP_UNKNOWN_CHARSET_CODE   (WPS_HC_STATUS_BASE + 101)
/*  WSP error: unknown method code. */
#define WPS_HC_ERROR_WSP_UNKNOWN_METHOD_CODE    (WPS_HC_STATUS_BASE + 103)
/*  WSP error: unknown Cache-Control code. */
#define WPS_HC_ERROR_WSP_UNKNOWN_CACHE_CTL_CODE (WPS_HC_STATUS_BASE + 105)
/*  WSP error: unknown Content-Disposition value. */
#define WPS_HC_ERROR_WSP_UNKNOWN_CONTENT_DISP   (WPS_HC_STATUS_BASE + 107)
/*  WSP error: unknown Range specifier code. */
#define WPS_HC_ERROR_WSP_UNKNOWN_RANGE_CODE     (WPS_HC_STATUS_BASE + 109)
/*  WSP error: text found where integer value was expected. */
#define WPS_HC_ERROR_WSP_INTEGER_EXPECTED       (WPS_HC_STATUS_BASE + 111)
/*  WSP error: short-int or text found where length value was expected. */
#define WPS_HC_ERROR_WSP_LENGTH_EXPECTED        (WPS_HC_STATUS_BASE + 113)
/*  WSP error: Incorrect Value-length. */
#define WPS_HC_ERROR_WSP_INCORRECT_VALUE_LEN    (WPS_HC_STATUS_BASE + 115)
/*  WSP error: empty text string. */
#define WPS_HC_ERROR_WSP_EMPTY_TEXT             (WPS_HC_STATUS_BASE + 117)
/*  WSP error: data overflows the end of the WSP header. */
#define WPS_HC_ERROR_WSP_DATA_OVERFLOW          (WPS_HC_STATUS_BASE + 119)
/*  WSP error: Content-MD5 value has incorrect length (it must be exactly 16 octets). */
#define WPS_HC_ERROR_WSP_INCORRECT_MD5_LEN      (WPS_HC_STATUS_BASE + 121)

/*
-----------------------------------------------------------------------------
    Function declarations
-----------------------------------------------------------------------------
*/

#ifdef __cplusplus
extern "C" {
#endif

/*
-----------------------------------------------------------------------------
    Encodes the given HTTP header into a WSP header. The HTTP header may be 
    terminated by either a NULL character or two consecutive newlines (\n).
    The <pCodePages> array contains the assignments of application-specific 
    header code page names to numbers, which have been set during the 
    capability negotiations. It may be NULL if <nNumCodePages> is set to 0, 
    indicating that there are no application-specific header code pages.

    Returns WPS_NO_ERROR if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_EXPORT 
HC_STATUS HC_EncodeHeader(
    const char *        pcHttpHeader,   /* in:  The HTTP header to be encoded. */
    const HC_NameBytePair* pCodePages,  /* in:  Application-specific code page assignments. */
    HC_UINT8            nNumCodePages,  /* in:  Number of application-specific code pages. */
    HC_BITF             bFlags,         /* in:  Option flags. */
    HC_WapVersion       wapVersion,     /* in:  WAP version */
    HC_BYTE*            pbWspHeader,    /* in:  Buffer allocated by caller.
                                           out: The encoded WSP header. */
    HC_UINT32*          pnWspHdrLen);   /* in:  The maximum length of the WSP header.
                                           out: The length of the encoded WSP header. */

/*
-----------------------------------------------------------------------------
    Decodes the given WSP header into an HTTP header.
    The <pCodePages> array contains the assignments of application-specific 
    header code page names to numbers, which have been set during the 
    capability negotiations. It may be NULL if <nNumCodePages> is set to 0, 
    indicating that there are no application-specific header code pages.

    Returns WPS_NO_ERROR if succeeded, otherwise an error code.
-----------------------------------------------------------------------------
*/
HC_EXPORT 
HC_STATUS HC_DecodeHeader(
    const HC_BYTE*      pbWspHeader,    /* in:  The WSP header to be decoded. */
    HC_UINT32           nWspHdrLen,     /* in:  The actual length of the WSP header. */
    HC_UINT32           nContentLen,    /* in:  The content length (see WSP 8.4.2.23). */
    const HC_NameBytePair* pCodePages,  /* in:  Application-specific code page assignments. */
    HC_UINT8            nNumCodePages,  /* in:  Number of application-specific code pages. */
    HC_BITF             bFlags,         /* in:  Option flags. */
    HC_WapVersion       wapVersion,     /* in:  WAP version */
    char*               pcHttpHeader,   /* in:  Buffer allocated by caller.
                                           out: The decoded (null-terminated) HTTP header. */
    HC_UINT32*          pnHttpHdrLen);  /* in:  The maximum length of the HTTP header.
                                           out: The length of the decoded HTTP header. */

#ifdef __cplusplus
}
#endif

#endif /* _HCODEC_H_ */

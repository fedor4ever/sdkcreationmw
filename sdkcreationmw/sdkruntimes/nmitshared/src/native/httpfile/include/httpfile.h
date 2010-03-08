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


#ifndef _HTTPFILE_H_
#define _HTTPFILE_H_ 1

#ifdef _WIN32
#  define HTTP_API __cdecl
#else
#  define HTTP_API
#endif /* _WIN32 */

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* *************************************************************************
 *  (some) HTTP headers
 * *************************************************************************/

#define HTTP_HEADER_ACCEPT               "Accept"
#define HTTP_HEADER_USER_AGENT           "User-Agent"

#define HTTP_HEADER_AGE                  "Age"
#define HTTP_HEADER_CACHE_CONTROL        "Cache-Control"
#define HTTP_HEADER_CONTENT_DISPOSITION  "Content-Disposition"
#define HTTP_HEADER_CONTENT_ENCODING     "Content-Encoding"
#define HTTP_HEADER_CONTENT_LANGUAGE     "Content-Language"
#define HTTP_HEADER_CONTENT_LENGTH       "Content-Length"
#define HTTP_HEADER_CONTENT_LOCATION     "Content-Location"
#define HTTP_HEADER_CONTENT_MD5          "Content-MD5"
#define HTTP_HEADER_CONTENT_RANGE        "Content-Range"
#define HTTP_HEADER_CONTENT_TYPE         "Content-Type"
#define HTTP_HEADER_DATE                 "Date"
#define HTTP_HEADER_ETAG                 "Etag"
#define HTTP_HEADER_EXPIRES              "Expires"
#define HTTP_HEADER_FROM                 "From"
#define HTTP_HEADER_LAST_MOFIFIED        "Last-Modified"
#define HTTP_HEADER_TRANSFER_ENCODING    "Transfer-Encoding"
#define HTTP_HEADER_VARY                 "Vary"

#define HTTP_HEADER_X_WAP_APPLICATION_ID "X-Wap-Application-Id"
#define HTTP_HEADER_X_WAP_CONTENT_URI    "X-Wap-Content-URI"
#define HTTP_HEADER_X_WAP_INITIATOR_URI  "X-Wap-Initiator-URI"

/* *************************************************************************
 *  HTTP status codes
 * *************************************************************************/

#define HTTP_STATUS_CODE_CONTINUE            100
#define HTTP_STATUS_CODE_SWITCH_PROTOCOLS    101

#define HTTP_STATUS_CODE_OK                  200
#define HTTP_STATUS_CODE_CREATED             201
#define HTTP_STATUS_CODE_ACCEPTED            202
#define HTTP_STATUS_CODE_PARTIAL             203
#define HTTP_STATUS_CODE_NO_CONTENT          204
#define HTTP_STATUS_CODE_RESET_CONTENT       205
#define HTTP_STATUS_CODE_PARTIAL_CONTENT     206

#define HTTP_STATUS_CODE_AMBIGUOUS           300
#define HTTP_STATUS_CODE_MOVED               301
#define HTTP_STATUS_CODE_REDIRECT            302
#define HTTP_STATUS_CODE_REDIRECT_METHOD     303
#define HTTP_STATUS_CODE_NOT_MODIFIED        304
#define HTTP_STATUS_CODE_USE_PROXY           305
#define HTTP_STATUS_CODE_REDIRECT_KEEP_VERB  307

#define HTTP_STATUS_CODE_BAD_REQUEST         400
#define HTTP_STATUS_CODE_DENIED              401
#define HTTP_STATUS_CODE_PAYMENT_REQ         402
#define HTTP_STATUS_CODE_FORBIDDEN           403
#define HTTP_STATUS_CODE_NOT_FOUND           404
#define HTTP_STATUS_CODE_BAD_METHOD          405
#define HTTP_STATUS_CODE_NONE_ACCEPTABLE     406
#define HTTP_STATUS_CODE_PROXY_AUTH_REQ      407
#define HTTP_STATUS_CODE_REQUEST_TIMEOUT     408
#define HTTP_STATUS_CODE_CONFLICT            409
#define HTTP_STATUS_CODE_GONE                410
#define HTTP_STATUS_CODE_LENGTH_REQUIRED     411
#define HTTP_STATUS_CODE_PRECOND_FAILED      412
#define HTTP_STATUS_CODE_REQUEST_TOO_LARGE   413
#define HTTP_STATUS_CODE_URI_TOO_LONG        414
#define HTTP_STATUS_CODE_UNSUPPORTED_MEDIA   415

#define HTTP_STATUS_CODE_SERVER_ERROR        500
#define HTTP_STATUS_CODE_NOT_SUPPORTED       501
#define HTTP_STATUS_CODE_BAD_GATEWAY         502
#define HTTP_STATUS_CODE_SERVICE_UNAVAIL     503
#define HTTP_STATUS_CODE_GATEWAY_TIMEOUT     504
#define HTTP_STATUS_CODE_VERSION_NOT_SUP     505

/* *************************************************************************
 *  Types
 * *************************************************************************/

typedef enum _HttpBool {
    HttpFalse = 0,
    HttpTrue  = 1
} HttpBool;

typedef struct _HttpHeader {
    char * name;                        /* name */
    char * value;                       /* value (including parameters) */
} HttpHeader;

typedef struct _HttpParam {             /* parameter of the HTTP header */
    char * name;                        /* name */
    char * value;                       /* value */
} HttpParam;

/* Content descriptor */
typedef struct _HttpContent {
    HttpHeader * headers;               /* HTTP headers */
    int hcount;                         /* number of headers */
    void * data;                        /* content data */
    int size;                           /* data size, in bytes */
    char * type;                        /* NULL to determine from headers */
    char * charset;                     /* character set, if applicable */
} HttpContent;

/* Opaque data types */
typedef struct _ParseHttpResponse ParseHttpResponse;
typedef struct _ParseHttpHeader ParseHttpHeader;

/* *************************************************************************
 *  Utilities
 * *************************************************************************/

/* Headers */
HttpHeader *
HTTP_API
HEADER_Create(
    const char * name,
    const char * value
    );

HttpHeader * 
HTTP_API 
HEADER_Parse(
    const char * str
    );

HttpHeader * 
HTTP_API 
HEADER_Clone(
    const HttpHeader * header
    );

HttpHeader *
HTTP_API
HEADER_Find(
    HttpHeader * header, 
    int n,
    const char * name
    );

HttpBool
HTTP_API
HEADER_SetName(
    HttpHeader * header,
    const char * name
    );

HttpBool 
HTTP_API 
HEADER_SetValue(
    HttpHeader * header, 
    const char * value
    );

void 
HTTP_API 
HEADER_Delete(
    HttpHeader * header
    );

/* HTTP header + parameters */
ParseHttpHeader * 
HTTP_API 
HTTP_HeaderCreate(
    const char * name,
    const char * value
    );

ParseHttpHeader * 
HTTP_API 
HTTP_HeaderParse(
    const HttpHeader * header
    );

ParseHttpHeader * 
HTTP_API 
HTTP_HeaderParseString(
    const char * str
    );

const char *
HTTP_API 
HTTP_HeaderName(
    const ParseHttpHeader * header
    );

const char * 
HTTP_API 
HTTP_HeaderValue(
    const ParseHttpHeader * header
    );

const HttpParam * 
HTTP_API 
HTTP_HeaderParam(
    const ParseHttpHeader * header, 
    int i
    );

HttpHeader *
HTTP_API
HTTP_HeaderUnparse(
    ParseHttpHeader * header,
    HttpBool free
    );

HttpBool 
HTTP_API 
HTTP_HeaderSetValue(
    ParseHttpHeader * h,
    const char * value
    );

int
HTTP_API 
HTTP_HeaderParamCount(
    const ParseHttpHeader * header
    );

void
HTTP_API 
HTTP_HeaderDelete(
    ParseHttpHeader * header
    );

/* Manipulating header parameters */
const char * 
HTTP_API 
HTTP_HeaderParamValue(
    const ParseHttpHeader * header,
    const char * param
    );

HttpBool 
HTTP_API 
HTTP_HeaderRemoveParam(
    ParseHttpHeader * header, 
    const char * param
    );

HttpBool 
HTTP_API
HTTP_HeaderSetParam(
    ParseHttpHeader * header, 
    const char * name,
    const char * value
    );

/* Parsing HTTP response */
ParseHttpResponse * 
HTTP_API 
HTTP_ResponseParse(
    const void * data,
    int len
    );

ParseHttpResponse * 
HTTP_API 
HTTP_ParseFile(
    const char * fname,
    HttpBool strict
    );

const HttpContent * 
HTTP_API 
HTTP_ResponseContent(
    const ParseHttpResponse * resp
    );

const char * 
HTTP_API
HTTP_ResponseReason(
    const ParseHttpResponse * resp
    );

int  
HTTP_API 
HTTP_ResponseStatus(
    const ParseHttpResponse * resp
    );

void 
HTTP_API 
HTTP_ResponseDelete(
    ParseHttpResponse * resp
    );

#ifdef __cplusplus
} /* end of extern "C" */
#endif  /* __cplusplus */

#endif /* _HTTPFILE_H_ */

/*
 * Local Variables:
 * mode:C
 * c-basic-offset:4
 * indent-tabs-mode:nil
 * End:
 */

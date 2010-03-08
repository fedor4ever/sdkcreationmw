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


#include "httpfile.h"
#include "s_vector.h"
#include "s_file.h"
#include "s_util.h"
#include "s_mem.h"

/* *************************************************************************
 *  Parsing HTTP response
 * *************************************************************************/

struct _ParseHttpResponse {             /* parsed HTTP response */
    int status;                         /* HTTP response status */
    char * reason;                      /* the reason string */
    HttpContent content;                /* the content */
};

#define HTTP_DEFAULT_CHARSET "ISO-8859-1"

/**
 * Similar to isdigit() except that it's guaranteed to be a function rather
 * than a macro. We don't want any side effects caused by macro parameter
 * being referenced more than once.
 */
STATIC Bool HTTP_IsDigit(char c)
{
    return IsDigit(c);
}

/**
 * Returns a number represented by a single decimal digit
 */
STATIC int HTTP_Number(char c)
{
    switch (c) {
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    default:
    case '0': return 0;
    }
}

/**
 * VectorFree function for a Vector containing HttpHeaders
 */
STATIC void HTTP_HeaderVectorFreeProc(VElement e)
{
    HEADER_Delete((HttpHeader*)e);
}

/**
 * Internal helper for parsing HTTP response
 */
STATIC Bool HTTP_ParseInternal(ParseHttpResponse * r, File * in, StrBuf * sb)
{
    /* read the status line */
    Str s = FILE_ReadLine(in, sb);
    if (s) {
        int i = 0;
        char c1,c2,c3;
        int contentLength = -1;
        Vector headers;
        VECTOR_Init(&headers, 0, NULL, HTTP_HeaderVectorFreeProc);

        r->status = HTTP_STATUS_CODE_OK;
        if (s[i++] == 'H' && s[i++] == 'T' && 
            s[i++] == 'T' && s[i++] == 'P' && 
            s[i++] == '/' && 
            
            /* HTTP version */
            HTTP_IsDigit(s[i++]) &&
            s[i++] == '.' && 
            HTTP_IsDigit(s[i++]) &&

            /* 3 digit status code */
            s[i++] == ' ' && 
            HTTP_IsDigit(c1 = s[i++]) &&
            HTTP_IsDigit(c2 = s[i++]) && 
            HTTP_IsDigit(c3 = s[i++]) &&
            s[i++] == ' ') {

            /* it looks like a valid status line */
            r->status = HTTP_Number(c1) * 100 + 
                        HTTP_Number(c2) * 10 + 
                        HTTP_Number(c3);

            /* copy the reason string */
            r->reason = STRING_Dup(s + i);
            if (!r->reason) {
                VECTOR_Destroy(&headers);
                return False;
            }
        } else {
            /* else assume it's a header */
            HttpHeader * h = HEADER_Parse(sb->s);
            if (!h || !VECTOR_TryAdd(&headers, h)) {
                VECTOR_Destroy(&headers);
                return False;
            }
        }

        /* read the headers */
        while ((s = FILE_ReadLine(in, sb)) && s[0]) {
            HttpHeader * h = HEADER_Parse(sb->s);
            if (h && StrCaseCmp(h->name, HTTP_HEADER_CONTENT_LENGTH) == 0) {
                VERIFY(PARSE_Int(h->value, &contentLength, 10));
            }
            if (!h || !VECTOR_TryAdd(&headers, h)) {
                VECTOR_Destroy(&headers);
                MEM_Free(r->reason);
                r->reason = NULL;
                return False;
            }
        }

        if (s) {
            /* the rest of it is the body */
            int hcount = VECTOR_Size(&headers);
            Buffer buf;
            BUFFER_Init(&buf);
            FILE_ReadData(in, &buf, contentLength);
            ASSERT(contentLength < 0 || BUFFER_Size(&buf) == contentLength);
            if (hcount) {
                r->content.hcount = hcount;
                r->content.headers = MEM_NewArray(HttpHeader,hcount);
                if (!r->content.headers) {
                    VECTOR_Destroy(&headers);
                    MEM_Free(r->reason);
                    r->reason = NULL;
                    return False;
                }
                
                /* don't need to deallocate the header strings */
                headers.free = vectorFreeValueProc;
            }

            /* steal the data from the buffer */
            r->content.size = BUFFER_Size(&buf);
            r->content.data = buf.data;
            ASSERT(buf.start == 0);

            /* copy the headers */
            for (i=0; i<hcount; i++) {
                HttpHeader * h = (HttpHeader*)VECTOR_Get(&headers,i);
                r->content.headers[i] = *h;

                /* parse the content type header */
                if (StrCaseCmp(h->name, HTTP_HEADER_CONTENT_TYPE) == 0) {
                    ParseHttpHeader * type = HTTP_HeaderParse(h);
                    if (type) {
                        Str charset = HTTP_HeaderParamValue(type,"charset");
                        r->content.type = STRING_Dup(HTTP_HeaderValue(type));
                        if (charset) {
                            r->content.charset = STRING_Dup(charset);
                        }
                        HTTP_HeaderDelete(type);
                    }
                    if (!r->content.charset) {
                        r->content.charset = STRING_Dup(HTTP_DEFAULT_CHARSET);
                    }
                }
            }
            VECTOR_Destroy(&headers);
            return True;
        }
    }
    return False;
}

/**
 * Parses HTTP response, returns NULL if parsing fails
 */
ParseHttpResponse * HTTP_API HTTP_ResponseParse(const void * data, int len)
{
    ParseHttpResponse * resp = NULL;
    if (len != 0) {
        int size = ((len > 0) ? len : strlen(data));
        File * in = FILE_MemIn(data, size);
        if (in) {
            resp = MEM_New(ParseHttpResponse);
            if (resp) {
                StrBuf64 buf;
                STRBUF_InitBufXXX(&buf);
                memset(resp, 0, sizeof(*resp));
                if (!HTTP_ParseInternal(resp, in, &buf.sb)) {
                    MEM_Free(resp);
                    resp = NULL;
                }
                FILE_Close(in);
                STRBUF_Destroy(&buf.sb);
            }
        }
    }
    return resp;
}

/**
 * Parses HTTP response file. If the 'strict' parameter is True, it only
 * attempts to parse the file if the file name ends with the ".http" suffix
 * or the file begins with 'HTTP/1.x' signature. Otherwise, it attempts
 * to parse it no matter what.
 */
ParseHttpResponse * HTTP_API HTTP_ParseFile(const char * fname, HttpBool strict)
{
    File * in;
    ParseHttpResponse * resp = NULL;
    in = FILE_Open(fname, READ_BINARY_MODE, NULL);
    if (in) {
        Buffer buf;
        BUFFER_Init(&buf);
        FILE_ReadData(in, &buf, -1);
        if (BUFFER_Size(&buf) > 0) {
            char * data = (char*)BUFFER_Access(&buf);
            if (!strict ||
                STRING_EndsWith(fname, ".http") == 0 ||
               (BUFFER_Size(&buf) > 8 && 
                data[0] == 'H' && 
                data[1] == 'T' && 
                data[2] == 'T' && 
                data[3] == 'P' && 
                data[4] == '/' && 
                IsDigit(data[5]) && 
                data[6] == '.' && 
                IsDigit(data[7]) &&
                data[8] == ' ')) {

                /* parse it as an HTTP response */
                resp = HTTP_ResponseParse(data, BUFFER_Size(&buf));
            }
        }
        BUFFER_Destroy(&buf);
        FILE_Close(in);
    }
    return resp;
}

/**
 * Returns pointer to the content portion of the parsed HTTP response
 */
const HttpContent* HTTP_API HTTP_ResponseContent(const ParseHttpResponse* r)
{
    return &r->content;
}

/**
 * Returns the reason string from the HTTP status line
 */
const char * HTTP_API HTTP_ResponseReason(const ParseHttpResponse * resp)
{
    return resp->reason;
}

/**
 * Returns the HTTP status from the HTTP status line
 */
int HTTP_API HTTP_ResponseStatus(const ParseHttpResponse * resp)
{
    return resp->status;
}

/**
 * Deallocates parsed HTTP response allocated by HTTP_ResponseParse
 */
void HTTP_API HTTP_ResponseDelete(ParseHttpResponse * resp)
{
    if (resp) {
        int i;
        for (i=0; i<resp->content.hcount; i++) {
            MEM_Free(resp->content.headers[i].name);
            MEM_Free(resp->content.headers[i].value);
        }
        MEM_Free(resp->content.headers);
        MEM_Free(resp->content.charset);
        MEM_Free(resp->content.type);
        MEM_Free(resp->content.data);
        MEM_Free(resp->reason);
        MEM_Free(resp);
    }
}

/*
 * Local Variables:
 * c-basic-offset:4
 * indent-tabs-mode:nil
 * End:
 */

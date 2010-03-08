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
#include "s_util.h"
#include "s_mem.h"

/* *************************************************************************
 *  Parsing HTTP headers
 * *************************************************************************/

struct _ParseHttpHeader {
    char * name;                        /* header name */
    char * value;                       /* header value (no parameters) */
    Vector params;                      /* parameters */
};

/**
 * VectorFree routine for the vector of HttpParams
 */
STATIC void HTTP_HeaderParamFree(VElement e)
{
    HttpParam * param = (HttpParam*)e;
    if (param) {
        MEM_Free(param->name);
        MEM_Free(param->value);
        MEM_Free(param);
    }
}

/**
 * Allocates memory for ParseHttpHeader structure
 */
ParseHttpHeader * HTTP_API HTTP_HeaderCreate(const char * n, const char * v)
{
    ParseHttpHeader * header = MEM_New(ParseHttpHeader);
    if (header) {
        header->name = NULL;
        header->value = NULL;
        VECTOR_Init(&header->params, 0, NULL, HTTP_HeaderParamFree);
        if (!n || (header->name = STRING_Dup(n)) != NULL) {
            if (!v || (header->value = STRING_Dup(v)) != NULL) {
                return header;
            }
            MEM_Free(header->name);
        }
        MEM_Free(header);
    }
    return NULL;
}

/**
 * Parses HTTP header parameters. Fills the vector with pointers to 
 * HttpParam structures. Returns True if it has successfully parsed
 * the parameter or if there's no parameters. If this function returns
 * False, the params vector MAY contain some HttpHeader structures.
 * It's the responsibility of the caller to clean up the params vector.
 */
STATIC Bool HTTP_HeaderParseParams(const char * value, Vector * params)
{
    const char * sep = StrChr(value, ';');
    if (sep) {
        StrBuf32 buf;
        STRBUF_InitBufXXX(&buf);

        /* search for parameters */
        while (sep) {
            const char * p = sep + 1;
            STRBUF_Clear(&buf.sb);
            while (IsSpace(*p)) p++;
            while (*p && *p != '=') STRBUF_AppendChar(&buf.sb,*p++);
            STRBUF_Trim(&buf.sb);   /* remove trailing spaces */
            if (*p == '=') {
                char * name = STRING_Dup(STRBUF_Text(&buf.sb));
                if (name) {
                    char * value;
                    STRBUF_Clear(&buf.sb);
                    for (p++; IsSpace(*p); p++);
                    while (*p && *p != ';') STRBUF_AppendChar(&buf.sb,*p++);
                    value = STRING_Dup(STRBUF_Text(&buf.sb));
                    if (value) {
                        HttpParam * param = MEM_New(HttpParam);
                        if (param) {
                            param->name = name;
                            param->value = value;
                            if (VECTOR_Add(params, param)) {
                                sep = ((*p == ';') ? p : NULL);
                                continue;
                            }
                            MEM_Free(param);
                        }
                        MEM_Free(value);
                    }
                    MEM_Free(name);
                }
            }
            STRBUF_Destroy(&buf.sb);
            return False;
        }
        STRBUF_Destroy(&buf.sb);
        return True;
    }
    return True;
}

/**
 * Parses HTTP formatted header string
 */
ParseHttpHeader * HTTP_API HTTP_HeaderParseString(const char * str)
{
    HttpHeader * raw = HEADER_Parse(str);
    if (raw) {
        char * sep = StrChr(raw->value, ';');
        if (sep) {
            Vector params;
            VECTOR_Init(&params, 0, NULL, HTTP_HeaderParamFree);
            if (HTTP_HeaderParseParams(raw->value, &params)) {
                char * value;
                int i = sep - raw->value;
                while (i > 0 && IsSpace(raw->value[i])) i--;
                value = MEM_NewArray(char, i+2);
                if (value) {
                    ParseHttpHeader * header;
                    memcpy(value, raw->value, i+1);
                    value[i+1] = 0;
                    header = HTTP_HeaderCreate(NULL, NULL);
                    if (header) {
                        header->name = raw->name;
                        header->value = value;
                        header->params = params;
                        MEM_Free(raw->value);
                        MEM_Free(raw);
                        return header;
                    }
                    MEM_Free(value);
                }
            }
            VECTOR_Destroy(&params);
        } else {
            ParseHttpHeader * header = HTTP_HeaderCreate(NULL, NULL);
            if (header) {
                header->name = raw->name;
                header->value = raw->value;
                MEM_Free(raw);
                return header;
            }
        }
        HEADER_Delete(raw);
    }
    return NULL;
}

/**
 * Parses raw HTTP header
 */
ParseHttpHeader * HTTP_API HTTP_HeaderParse(const HttpHeader * raw)
{
    char * sep = StrChr(raw->value, ';');
    if (sep) {
        Vector params;
        VECTOR_Init(&params, 0, NULL, HTTP_HeaderParamFree);
        if (HTTP_HeaderParseParams(raw->value, &params)) {
            char * value;
            int i = sep - raw->value;
            while (i > 0 && IsSpace(raw->value[i])) i--;
            value = MEM_NewArray(char, i+2);
            if (value) {
                ParseHttpHeader * header = HTTP_HeaderCreate(raw->name, NULL);
                if (header) {
                    header->params = params;
                    header->value = value;
                    memcpy(value, raw->value, i+1);
                    value[i+1] = 0;
                    return header;
                }
                MEM_Free(value);
            }
        }
        VECTOR_Destroy(&params);
    } else {
        return HTTP_HeaderCreate(raw->name, raw->value);
    }
    return NULL;
}

/**
 * Returns the header name
 */
const char * HTTP_API HTTP_HeaderName(const ParseHttpHeader * header)
{
    return header->name;
}

/**
 * Returns the header value with parameters stripped off
 */
const char * HTTP_API HTTP_HeaderValue(const ParseHttpHeader * header)
{
    return header->value;
}

/**
 * Returns the i-th parameter
 */
const HttpParam * HTTP_API HTTP_HeaderParam(const ParseHttpHeader* h, int i)
{
    return (HttpParam*)VECTOR_Get(&h->params, i);
}

/**
 * Returns the number of parameters
 */
int HTTP_API HTTP_HeaderParamCount(const ParseHttpHeader * header)
{
    return VECTOR_Size(&header->params);
}

/**
 * Sets the HTTP header value 
 */
HttpBool HTTP_API HTTP_HeaderSetValue(ParseHttpHeader * h, const char * value)
{
    if (StrCmp(h->value,value)) {
        char * newValue = STRING_Dup(value);
        if (newValue) {
            MEM_Free(h->value);
            h->value = newValue;
            return True;
        }
        return False;
    }
    return True;
}

/**
 * Deallocates ParseHttpResponse and everything it contains
 */
void HTTP_API HTTP_HeaderDelete(ParseHttpHeader * header)
{
    if (header) {
        VECTOR_Destroy(&header->params);
        MEM_Free(header->name);
        MEM_Free(header->value);
        MEM_Free(header);
    }
}

/**
 * Returns header in a raw format, optionally deallocating ParseHttpHeader.
 * If the second parameter is True, the parsed header is deallocated even
 * if conversion to HttpHeader fails.
 */
HttpHeader * HTTP_API HTTP_HeaderUnparse(ParseHttpHeader * h, HttpBool free)
{
    HttpHeader * raw = MEM_New(HttpHeader);
    if (raw) {
        int i;
        int n = HTTP_HeaderParamCount(h);
        StrBuf64 buf;
        STRBUF_InitBufXXX(&buf);
        STRBUF_Copy(&buf.sb, h->value);
        for (i=0; i<n; i++) {
            const HttpParam * param = HTTP_HeaderParam(h, i);
            STRBUF_Append(&buf.sb, "; ");
            STRBUF_Append(&buf.sb, param->name);
            STRBUF_Append(&buf.sb, "=");
            STRBUF_Append(&buf.sb, param->value);
        }
        raw->value = STRING_Dup(STRBUF_Text(&buf.sb));
        STRBUF_Destroy(&buf.sb);

        if (raw->value) {
            if (free) {
                raw->name = h->name;
                h->name = NULL;
                HTTP_HeaderDelete(h);
                return raw;
            } else {
                raw->name = STRING_Dup(h->name);
                if (raw->name) {
                    return raw;
                }
            }
            MEM_Free(raw->value);
        }

        MEM_Free(raw);
    }
    if (free) HTTP_HeaderDelete(h);
    return NULL;
}

/**
 * Returns the value of the named parameter
 */
const char * 
HTTP_API 
HTTP_HeaderParamValue(
    const ParseHttpHeader * header,
    const char * name)
{
    int i,n = HTTP_HeaderParamCount(header);
    for (i=0; i<n; i++) {
        const HttpParam * param = HTTP_HeaderParam(header, i);
        if (StrCaseCmp(param->name, name) == 0) {
            return param->value;
        }
    }
    return NULL;
}

/**
 * Removes the named parameter
 */
HttpBool 
HTTP_API 
HTTP_HeaderRemoveParam(
    ParseHttpHeader * header, 
    const char * name)
{
    int i,n = HTTP_HeaderParamCount(header);
    for (i=0; i<n; i++) {
        const HttpParam * param = HTTP_HeaderParam(header, i);
        if (StrCaseCmp(param->name, name) == 0) {
            VECTOR_Remove(&header->params, i);
            return True;
        }
    }
    return False;
}

/**
 * Sets the value of the named parameter
 */
HttpBool 
HTTP_API 
HTTP_HeaderSetParam(
    ParseHttpHeader * header, 
    const char * name,
    const char * value)
{
    HttpParam * newParam;
    int i,n = HTTP_HeaderParamCount(header);
    for (i=0; i<n; i++) {
        HttpParam * param = (HttpParam*)VECTOR_Get(&header->params, i);
        if (StrCaseCmp(param->name, name) == 0) {
            
            /* update existing parameter */
            if (StrCmp(param->value, value) != 0) {
                char * newValue = STRING_Dup(value);
                if (newValue) {
                    MEM_Free(param->value);
                    param->value = newValue;
                    return True;
                }
                return False;
            }
            return True;
        }
    }

    /* create new parameter */
    newParam = MEM_New(HttpParam);
    if (newParam) {
        newParam->name = STRING_Dup(name);
        if (newParam->name) {
            newParam->value = STRING_Dup(value);
            if (newParam->value) {
                return VECTOR_TryAdd(&header->params, newParam);
            }
            MEM_Free(newParam->name);
        }
        MEM_Free(newParam);
    } 
    return False;
}

/*
 * Local Variables:
 * c-basic-offset:4
 * indent-tabs-mode:nil
 * End:
 */

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
#include "s_util.h"
#include "s_mem.h"

/* *************************************************************************
 *  Header utilities
 * *************************************************************************/

/**
 * Allocates HttpHeader structure
 */
HttpHeader * HTTP_API HEADER_Create(Str name, Str value)
{
    HttpHeader * header = MEM_New(HttpHeader);
    if (header) {
        header->name = STRING_Dup(name);
        if (header->name) {
            header->value = STRING_Dup(value);
            if (header->value) {
                return header;
            }
            MEM_Free(header->name);
        }
        MEM_Free(header);
    }
    return NULL;
}

/**
 * Parses "name: value" string into HttpHeader
 *
 * RFC 1945, section 4.2 "Message Headers":
 *
 * Each header field consists of a name followed immediately by a colon
 * (":"), a single space (SP) character, and the field value. Field names
 * are case-insensitive. Header fields can be extended over multiple lines
 * by preceding each extra line with at least one SP or HT, though this is
 * not recommended.
 */
HttpHeader * HTTP_API HEADER_Parse(const char * s)
{
    char * colon = StrChr(s, ':');
    if (colon && colon > s) {
        int icolon = colon - s;
        int ivalue = icolon+1;
        while (s[ivalue] && IsSpace(s[ivalue])) ivalue++;
        if (s[ivalue]) {
            HttpHeader * header = MEM_New(HttpHeader);
            if (header) {
                header->name = MEM_NewArray(char, icolon+1);
                if (header->name) {
                    memcpy(header->name, s, icolon);
                    header->name[icolon] = 0;
                    header->value = STRING_Dup(s + ivalue);
                    if (header->value) {
                        return header;
                    }
                    MEM_Free(header->name);
                }
                MEM_Free(header);
            }
        }
    }
    return NULL;
}

/**
 * Finds a header by name
 */
HttpHeader * HTTP_API HEADER_Find(HttpHeader * h, int n, const char * name)
{
    int i;
    for (i=0; i<n; i++) {
        if (StrCaseCmp(h[i].name, name) == 0) {
            return h + i;
        }
    }
    return NULL;
}

/**
 * Clones a Header structure
 */
HttpHeader * HTTP_API HEADER_Clone(const HttpHeader * header)
{
    if (header) {
        return HEADER_Create(header->name, header->value);
    }
    return NULL;
}

/**
 * Sets the header name
 */
HttpBool HTTP_API HEADER_SetName(HttpHeader * header, const char * name)
{
    if (StrCaseCmp(header->name, name)) {
        char * copy = STRING_Dup(name);
        if (copy) {
            MEM_Free(header->name);
            header->name = copy;
            return True;
        }
        return False;
    }
    return True;
}

/**
 * Sets the header value
 */
HttpBool HTTP_API HEADER_SetValue(HttpHeader * header, const char * value)
{
    if (StrCaseCmp(header->value, value)) {
        char * copy = STRING_Dup(value);
        if (copy) {
            MEM_Free(header->value);
            header->value = copy;
            return True;
        }
        return False;
    }
    return True;
}

/**
 * Deallocates header allocated by HEADER_Create
 */
void HTTP_API HEADER_Delete(HttpHeader * header) 
{
    if (header) {
        MEM_Free(header->name);
        MEM_Free(header->value);
        MEM_Free(header);
    }
}

/*
 * Local Variables:
 * c-basic-offset:4
 * indent-tabs-mode:nil
 * End:
 */

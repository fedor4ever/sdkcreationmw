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


#include "x_int.h"

/* *************************************************************************
//
//  Utility Functions
//
// *************************************************************************/

/**
 * Global initialization
 */
XRpcBool XRPC_Init()
{

#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(1,1),&wsaData);
#endif /* _WIN32 */

    MEM_InitModule();
    RANDOM_InitModule();
    THREAD_InitModule();
    HASH_InitModule();
    LOCK_InitModule();
    WKQ_InitModule();

    return XRpcTrue;
}

/**
 * Global deinitialization
 */
void XRPC_Deinit()
{
    WKQ_Shutdown();
    LOCK_Shutdown();
    HASH_Shutdown();
    THREAD_Shutdown();
    RANDOM_Shutdown();
    MEM_Shutdown();
}

/**
 * Returns the statically allocated tag name only if it's a valid tag.
 * Otherwise return NULL
 */
XRpcString XRPC_GetValidTagName(XRpcTag tag)
{
    static const char * names [] = {
        XRPC_TAG_ROOT,
        XRPC_TAG_DOUBLE,
        XRPC_TAG_LONG,
        XRPC_TAG_INT,
        XRPC_TAG_SHORT,
        XRPC_TAG_BYTE,
        XRPC_TAG_BOOLEAN,
        XRPC_TAG_BASE64,
        XRPC_TAG_STRING,
        XRPC_TAG_STRUCT,
        XRPC_TAG_ARRAY,
        XRPC_TAG_NULL,
        XRPC_TAG_VOID,
        XRPC_TAG_CALL,
        XRPC_TAG_PARAMS,
        XRPC_TAG_RESPONSE,
        XRPC_TAG_RESULT
    };
    int n = COUNT(names);
    int index = (tag - XRpcTagRoot);
    ASSERT(n == (XRpcTagUnknown-1));
    if (index >= 0 && index < n) {
        return names[index];
    }
    return NULL;
}

/**
 * Returns the statically allocated tag name
 */
XRpcString XRPC_GetTagName(XRpcTag tag)
{
    XRpcString name = XRPC_GetValidTagName(tag);
    if (name) {
        return name;
    } else {
        static char unknown[32];
        sprintf(unknown,"XRpcTagUnknown(%d)",tag);
        return unknown;
    }
}

/**
 * XRPC_IsValidChild returns True the child tag is a valid child f
 * or the specified parent
 */
#define XRPC_IsDataElementTag(_tag) \
    ((_tag) == XRpcTagDouble    ||  \
     (_tag) == XRpcTagLong      ||  \
     (_tag) == XRpcTagInt       ||  \
     (_tag) == XRpcTagShort     ||  \
     (_tag) == XRpcTagByte      ||  \
     (_tag) == XRpcTagBoolean   ||  \
     (_tag) == XRpcTagBase64    ||  \
     (_tag) == XRpcTagString    ||  \
     (_tag) == XRpcTagArray     ||  \
     (_tag) == XRpcTagStruct    ||  \
     (_tag) == XRpcTagNull)

XRpcBool XRPC_IsValidChild(XRpcTag parent, XRpcTag child)
{
    ASSERT(XRPC_GetValidTagName(child));
    switch (parent) {
    case XRpcTagNone:   
        return  child == XRpcTagRoot;

    case XRpcTagRoot:
        return  child == XRpcTagCall || 
                child == XRpcTagResponse;

    case XRpcTagResponse:
        return  child == XRpcTagResult;

    case XRpcTagCall:
        return  child == XRpcTagParams;

    case XRpcTagParams:
    case XRpcTagResult:
        return  XRPC_IsDataElementTag(child) ||
                child == XRpcTagVoid;

    case XRpcTagStruct:
    case XRpcTagArray:
        return  XRPC_IsDataElementTag(child);

    default:
        ASSERT(XRPC_GetValidTagName(parent));
    }
    return XRpcFalse;
}

/*
 * From rfc2044: encoding of the Unicode values on UTF-8:
 *
 * UCS-4 range (hex.)    UTF-8 octet sequence (binary)
 * 0000 0000-0000 007F   0xxxxxxx
 * 0000 0080-0000 07FF   110xxxxx 10xxxxxx
 * 0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx 
 */

/**
 * Returns number of bytes used by a UTF8 character (1...3), -1 if 'utf' 
 * parameter does not point to a valid UTF8 sequence or zero if size of the 
 * character appears to be larger than the amount of data available. 
 *
 * If 'maxsize' parameter is negative, the string is assumed to be null 
 * terminated. Note: if *utf points to a null character, this function 
 * returns 1.
 */
int XRPC_GetUTF8CharSize(XRpcStringUTF8 utf, int maxlen) 
{
    ASSERT(maxlen >= 0);
    ASSERT(utf);
    if (utf) {
        if (maxlen == 0) {
            return 0;
        } else {
            register char c = utf[0];
            if (c & 0x80) {
                if (maxlen == 1) {  

                    /* not enough data */
                    return 0;
                }
                if ((utf[1] & 0xc0) != 0x80) {

                    /* not a UTF8 */
                    return (-1);
                }
                if ((c & 0xe0) == 0xe0) {
                    if (maxlen == 2) {

                        /* not enough data */
                        return 0;
                    }
                    if ((utf[2] & 0xc0) != 0x80) {

                        /* not a UTF8 */
                        return (-1);
                    }
                    if ((c & 0xf0) == 0xf0) {

                        /* disallow 4 byte codes */
                        return (-1);

                    } else {

                        /* 3-byte code */
                        return 3;
                    }
                } else  {

                    /* 2-byte code */
                    return 2;
                }
            } else {

                /* 1-byte code */
                return 1;
            }
        }
    }
    return (-1);
}

/*
 * Reads one UTF8 character. 
 * On input, *len contains number of bytes available in the buffer
 *
 * If succeeds, returns numeber of bytes used, puts the wide character 
 * into *wc, and  updates *len subtracting the number of bytes used from 
 * the original value. Returns zero if there's not enough data in the input 
 * buffer to read the next UTF8 character. If there's an error in UTF8
 * sequence, returns -1
 *
 * If the UTF8 character cannot be converted into a single byte character 
 * in active code page, the *ch character is set to zero
 */
int
XRPC_GetUTF8Char(
    XRpcStringUTF8  utf,        /* input UTF8 string */
    XRpcUcs2 *      wc,         /* receives the wide character */
    char *          c,          /* receives character in active code page */
    int *           len)        /* available/remaining bytes */
{
    if (wc) *wc = 0;
    if (c) *c = 0;
    
    ASSERT(utf);
    ASSERT(wc || c);
    ASSERT(len);
    
    if (utf && (wc || c) && len && *len > 0) {
        register XRpcUcs2 ch = 0;
        int size  = XRPC_GetUTF8CharSize(utf, *len);
        switch (size) {
        case 3: 
            /* 3-byte code */
            ch  = (utf[0] & 0x0f) << 12;
            ch |= (utf[1] & 0x3f) << 6;
            ch |= (utf[2] & 0x3f);
            break;

        case 2:
            /* 2-byte code */
            ch  = (utf[0] & 0x1f) << 6;
            ch |= (utf[1] & 0x3f);
            break;

        case 1:
            /* 1-byte code */
            ch = utf[0];
            break;

        case 0:
            return 0;

        default:
            return (-1);
        }

        *len -= size;
        if (wc) *wc = ch;
        if (c) *c = XRPC_WideCharToASCII(ch);
        return size;
    }
    return (-1);
}

/**
 * Converts a wide char to UTF-8 chars
 */
int
XRPC_WideCharToUTF8(
    XRpcUcs2        wc,         /* input wide char*/
    char *          dest)       /* receives UTF chars, at least 4 bytes */
{
    int n = 0;
    if (wc <= 0x007F) {
        *dest++ = (char)(wc & 0x007F);
        n = 1; 
    } else if (wc <= 0x07FF) {
        *dest++ = (char)((wc >> 6) | 0x00c0);
        *dest++ = (char)((wc & 0x003F) | 0x0080);
        n = 2; 
    } else {
        *dest++ = (char)(((wc >> 12) & 0x000F) | 0x00e0);
        *dest++ = (char)(((wc >> 6) & 0x003F) | 0x0080);
        *dest++ = (char)((wc & 0x003F) | 0x0080);
        n = 3; 
    }
    *dest = 0;
    return n;
}

/**
 * Converts a wide char to a single byte character in active code page.
 * Returns zero if conversion is impossible
 */
char
XRPC_API
XRPC_WideCharToASCII(
    XRpcUcs2        wc)         /* input wide char*/
{
    if (wc < 0x0100) {
        return (char)wc;
    } else {
        return '\0';
    }
}

/**
 * Converts an 8-bit US-ASCII character into a wide character.
 */
XRpcUcs2
XRPC_API
XRPC_ASCIIToWideChar(char c)
{
    return (((XRpcUcs2)c) & 0xff); 
}

/**
 * Returns number of characters in a null terminated UTF8 string
 * or -1 if it's not a valid UTF8 string
 */
int
XRPC_API
XRPC_LengthUTF8(
    XRpcStringUTF8  s)          /* input UTF8 string */
{
    int len = 0;
    if (s) {
        while (*s) {
            int charsize = 0;
            size_t maxlen = 1;
            if (s[1]) {
                maxlen++;
                if (s[2]) {
                    maxlen++;
                }
            }
            charsize = XRPC_GetUTF8CharSize(s, maxlen);
            if (charsize < 0) return -1;
            len++;
            s += charsize;
        }
    }
    return len;
}

/**
 * Converts a UTF-8 encoded string into null terminated UCS-2 string.
 * If number of characters of -1, the input string is assumed to be null
 * terminated
 */
XRpcUcs2 *
XRPC_UTF8ToWideString(
    XRpcStringUTF8  utf,        /* input UTF8 string */
    int             size)       /* number of bytes */
{
    /* this may be more than necessary, but surely enough */    
    int len = (size < 0 ? strlen(utf) : size) + 1;
    XRpcUcs2 * ws = MEM_NewArray(XRpcUcs2,len);
    if (ws) {
        int n = 0;
        const char * src = utf;
        XRpcUcs2 * dest = ws;
        while (*src && (n = XRPC_GetUTF8Char(src, dest, NULL, &len)) > 0) {
            src += n;
            dest++;
        }
        if (n >= 0) {
            *dest = 0;
            return ws;
        }
        MEM_Free(ws);
    }
    return NULL;
}

/**
 * Converts a UTF-8 encoded string into null terminated ASCII string
 * If number of characters of -1, the input string is assumed to be null
 * terminated. The returned string must be deallocated with XRPC_Free
 */
char *
XRPC_API
XRPC_UTF8ToASCIIString(
    XRpcStringUTF8  utf,        /* input UTF8 string */
    int             size)       /* number of bytes */
{
    /* this may be more than necessary, but surely enough */    
    int len = (size < 0 ? strlen(utf) : size) + 1;
    char * s = MEM_NewArray(char,len);
    if (s) {
        int n = 0;
        const char * src = utf;
        char * dest = s;
        while (*src && (n = XRPC_GetUTF8Char(src, NULL, dest, &len)) > 0) {
            src += n;
            dest++;
        }
        if (n >= 0) {
            *dest = 0;
            return s;
        }
        MEM_Free(s);
    }
    return NULL;
}

/**
 * Converts a null terminated wide string to UTF-8
 */
char *
XRPC_WideStringToUTF8(
    const XRpcUcs2 * ws)        /* input wide string */
{
    char * buf;
    int size = 1;
    const XRpcUcs2 * p = ws;
    while (*p) {
        register XRpcUcs2 wc = *p++;
        if (wc <= 0x007F) {
            size++;
        } else if (wc <= 0x07FF) {
            size += 2;
        } else {
            size += 3;
        }
    }

    buf = MEM_NewArray(char,size);
    if (buf) {
        char * dest = buf;
        const XRpcUcs2 * src = ws;
        while (*src) {
            dest += XRPC_WideCharToUTF8(*src++, dest);
        }
        *dest = 0;
        ASSERT((int)strlen(buf) == (size-1));
    }

    return buf;
}

/**
 * Converts a null terminated wide string into a 8-bit ASCII string. 
 * If either memory allocation or convertion fails, returns NULL;
 */
char *
XRPC_WideStringToASCII(
    const XRpcUcs2 * ws)        /* input wide string */
{
    if (ws) {
        int len = UCS2_Len(ws);
        char * buf = MEM_NewArray(char, len + 1);
        if (buf) {
            int i;
            const XRpcUcs2 * src = ws;
            char * dst = buf;
            for (i=0; i<len; i++) {
                /* hmm... what am I supposed to do if I cannot convert? */
                char c = XRPC_WideCharToASCII(*src++);
                *dst++ = (c ? c : '.');
            }
            *dst = 0;
            return buf;
        }
    }
    return NULL;
}

/**
 * Converts a null terminated 8-bit ASCII string into a wide string.
 * If memory allocation fails, returns NULL;
 */
XRpcUcs2 *
XRPC_API
XRPC_ASCIIToWideString(
    const char * s)             /* input US-ASCII string */
{
    if (s) {
        size_t len = strlen(s);
        XRpcUcs2 * buf = MEM_NewArray(XRpcUcs2, len + 1);
        if (buf) {
            size_t i;
            const char * src = s;
            XRpcUcs2 * dst = buf;
            for (i=0; i<len; i++) {
                *dst++ = (((XRpcUcs2)(*src++)) & 0xff);
            }
            *dst = 0;
            return buf;
        }
    }
    return NULL;
}

/**
 * Translates a binary buffer into a BASE64 encoded string. The returned 
 * string is guaranteed to be a 7-bit ASCII string which also happens to 
 * be a valid UTF-8 string. If buffer is empty, returns an empty string. 
 * Only returns NULL if memory allocation fails. The caller will have to 
 * deallocate the returned string with XRPC_Free
 */
char *
XRPC_API
XRPC_BASE64Encode(
    const void *    data,       /* buffer to encode */
    size_t          size)       /* number of bytes to encode */
{
    return BASE64_Encode(data, size
#ifdef SLIB_VERSION
#  if SLIB_VERSION > SLIB_MAKE_VERSION(1,30)
        , BASE64_PAD
#  endif /* SLIB_VERSION > 1.30 */
#endif 
        );
}

/**
 * Decodes a BASE64 encoded data block. Returns XRpcTrue if string has been 
 * successfully decoded, or XRpcFalse if it cannot be decoded (or memory
 * allocation failes). The buffer returned by this function must be 
 * deallocated with XRPC_Free
 */
XRpcBool 
XRPC_API
XRPC_BASE64Decode(
    XRpcString      base64,     /* BASE64 encoded data */
    XRpcData *      result)     /* receives the decoded data */
{
    /* clear the output data */
    if (result) {
        result->ptr = NULL;
        result->size = 0;
    }

    /* allow NULL strings */
    if (base64) {
        if (result) {
            XRpcBool ok;
            Buffer buf;
            BUFFER_Init(&buf);
            ok = BASE64_Decode(base64, &buf);
            if (ok) {
                int size = BUFFER_Size(&buf);
                if (size > 0) {
                    void * data = BUFFER_Access(&buf);
                    ASSERT(buf.start == 0);
                    if (data == buf.data) {
                        /* "steal" data from the buffer */
                        result->ptr = (XRpcByte*)data;
                        result->size = size;
                        buf.flags &= ~BUFFER_OWN_DATA;
                    } else {
                        result->ptr = MEM_NewArray(XRpcByte,size);
                        if (result->ptr) {
                            memcpy(result->ptr, data, size);
                            result->size = size;
                        } else {
                            ok = XRpcFalse;
                        }
                    }
                }
            }
            BUFFER_Destroy(&buf);
            return ok;
        } else {
            return BASE64_Decode(base64, NULL);
        }
    }

    /* this includes the case when string is empty or NULL */
    return XRpcTrue;
}

/**
 * This macro returns True if the specified character is valid in 
 * XML documents. 
 */
#define IS_VALID_XML_CHAR(_c) \
    ((_c) < 0 || \
     (_c) >= 0x20 || \
     (_c) == 0x0A || \
     (_c) == 0x09 || \
     (_c) == 0x0D)

/**
 * This macro returns True if the specified UCS2 character is valid in
 * XML documents. 
 */
#define IS_VALID_XML_CHAR_UCS2(_c) \
	    (((_c) >= 0x0020 && (_c) <= 0xD7FF) \
		|| (_c) == 0x000A || (_c) == 0x0009 \
		|| (_c) == 0x000D \
		|| ((_c) >= 0xE000 && (_c) <= 0xFFFD) \
		|| ((_c) >= 0x10000 && (_c) <= 0x10ffff))

/**
 * Returns True if the specified character is valid in XML documents. 
 */
XRpcBool XRPC_API XRPC_IsValidXMLChar(char c)
{
    return IS_VALID_XML_CHAR(c);
}

/**
 * Returns True if the specified UCS2 character is valid in XML documents. 
 */
XRpcBool XRPC_API XRPC_IsValidXMLCharUCS2(XRpcUcs2 wc)
{
    return IS_VALID_XML_CHAR_UCS2(wc);
}

/**
 * Returns True if the string doesn't contains characters that are invalid 
 * in XML documents. If the 'len' parameter is negative, the string is
 * assumed to be NULL terminated.
 */
XRpcBool XRPC_API XRPC_IsValidXMLString(XRpcString s, int len)
{
    if (s) {
        if (len < 0) {
            register char c = *s++;
            while (c) {
                if (!IS_VALID_XML_CHAR(c)) {
                    return False;
                }
                c = *s++;
            }
        } else {
            int i;
            for (i=0; i<len; i++) {
                register char c = *s++;
                if (!IS_VALID_XML_CHAR(c)) {
                    return False;
                }
            }
        }
        return True;
    }
    return False;
}

/**
 * Returns True if the UCS2 string doesn't contains characters that are 
 * invalid in XML documents. If the 'len' parameter is negative, the 
 * string is assumed to be NULL terminated.
 */
XRpcBool XRPC_API XRPC_IsValidXMLStringUCS2(XRpcStringUCS2 ws, int len)
{
    if (ws) {
        if (len < 0) {
            register XRpcUcs2 c = *ws++;
            while (c) {
                if (!IS_VALID_XML_CHAR_UCS2(c)) {
                    return False;
                }
                c = *ws++;
            }
        } else {
            int i;
            for (i=0; i<len; i++) {
                register XRpcUcs2 c = *ws++;
                if (!IS_VALID_XML_CHAR_UCS2(c)) {
                    return False;
                }
            }
        }
        return True;
    }
    return False;
}

/**
 * Returns True if the UTF8 string doesn't contains characters that are 
 * invalid in XML documents. If the 'nbytes' parameter is negative, the 
 * string is assumed to be NULL terminated.
 */
XRpcBool XRPC_API XRPC_IsValidXMLStringUTF8(XRpcStringUTF8 utf, int nbytes)
{
    if (utf) {
        int n;
        XRpcUcs2 wc;
        if (nbytes < 0) nbytes = strlen(utf);
        while (nbytes > 0 && (n=XRPC_GetUTF8Char(utf,&wc,NULL,&nbytes)) > 0) {
            utf += n;
            if (!IS_VALID_XML_CHAR_UCS2(wc)) {
                return False;
            }
        }
        return (nbytes == 0);
    }
    return False;
}

/**
 * Normally, this functions should only be reference by debug build but 
 * we still have to keep it around in case if someone links debug code
 * with release version of XRPC library.
 */
int 
XRPC_API
XRPC_DebugTrace(
    const char * format,        /* format */
    ...)                        /* arguments */
{
    int n;
    va_list va;
    va_start(va, format);
    n = TRACE_Stdout(format, va);
    va_end(va);
    return n;
}

/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

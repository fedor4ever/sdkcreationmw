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
//  XRPC client
//
// *************************************************************************/

struct _XRpcClient {
    File * out;                 /* output stream */
    XRpcSession * session;      /* the session object */
    Lock   rwlock;              /* read-write lock */
};

#define CLIENT_WRITE_LOCK(_c)     LOCK_WriteLock(&(_c)->rwlock)
#define CLIENT_READ_LOCK(_c)      LOCK_ReadLock(&(_c)->rwlock)
#define CLIENT_UNLOCK(_c)         LOCK_Unlock(&(_c)->rwlock)

#define CLOSE_TAG(_client,_tag) \
    (FILE_Puts((_client)->out,"</")  > 0 && \
     FILE_Puts((_client)->out,_tag)  > 0 && \
     FILE_Puts((_client)->out,">\n"))

/* *************************************************************************
//
//  Create/delete the client
//
// *************************************************************************/

/**
 * Creates the client
 */
XRpcClient * XRPC_CreateClient(XRpcSession * session, File * out) 
{
    XRpcClient * client = MEM_New(XRpcClient);
    if (client) {
        if (LOCK_Init(&client->rwlock)) {
            client->session = session;
            client->out = out;
            return client;
        }
        MEM_Free(client);
    }
    return NULL;
}

/**
 * Destroys the client
 */
void XRPC_FreeClient(XRpcClient * client)
{
    if (client) {
        LOCK_Destroy(&client->rwlock);
        MEM_Free(client);
    }
}

/* *************************************************************************
//
//  Encoding of XRPC elements
//
// *************************************************************************/

/**
 * Converts a nibble to a hex character
 */
static XRpcUcs2 XRPC_ToHex(int nibble)
{
    /** A table of hex digits */
    static char hexDigit[] = {
        '0','1','2','3','4','5','6','7',
        '8','9','A','B','C','D','E','F'
    };
    return (XRpcUcs2)(hexDigit[(nibble & 0xF)]);
}

/**
 * Sends XMLized string to the other size
 */
static 
XRpcBool
XRPC_SendString(
    XRpcClient * client, 
    XRpcStringUCS2 ws )
{
    ASSERT(ws);
    if (ws) {
        XRpcStringUCS2 p = ws;
        while (*p) {
            char utf[8];
            XRpcUcs2 buf[10];
            XRpcUcs2 wc = *p++;
            XRpcStringUCS2 replace = NULL; /* replacement string */
	    static XRpcUcs2 xml_amp [] = {
	      _UCS2('&'),_UCS2('a'),_UCS2('m'),_UCS2('p'),_UCS2(';'),
              _UCS2(0)
	    };
	    static XRpcUcs2 xml_lt [] = {
	      _UCS2('&'),_UCS2('l'),_UCS2('t'),_UCS2(';'),_UCS2(0)
	    };
	    static XRpcUcs2 xml_gt [] = {
	      _UCS2('&'),_UCS2('g'),_UCS2('t'),_UCS2(';'),_UCS2(0)
	    };
	    static XRpcUcs2 xml_quot [] = {
	      _UCS2('&'),_UCS2('q'),_UCS2('u'),_UCS2('o'),_UCS2('t'),
              _UCS2(';'),_UCS2(0)
	    };
	    static XRpcUcs2 xml_apos [] = {
	      _UCS2('&'),_UCS2('a'),_UCS2('p'),_UCS2('o'),_UCS2('s'),
              _UCS2(';'),_UCS2(0)
	    };

            switch (wc) {
            case _UCS2('&'):            /* Ampersand */
                replace = xml_amp;
                break;
            case _UCS2('<'):            /* Less than */
	        replace = xml_lt;
                break;
            case _UCS2('>') :           /* Greater than */
                replace = xml_gt;
                break;
            case _UCS2('\"') :          /* Double quote */
                replace = xml_quot;
                break;
            case _UCS2('\'') :          /* Single quote */
                replace = xml_apos;
                break;
            default:
                if ( wc < 0x0080 && IsPrint((char)wc)) {
                    /* Normal 8-bit printable character */
                    break;
                } else if (wc < 0x0100) {
                    /* 8-bit Unicode (US-ASCII) */
                    int len = 0;
                    buf[len++] = _UCS2('&');
                    buf[len++] = _UCS2('#');
                    buf[len++] = _UCS2('x');
                    buf[len++] = XRPC_ToHex(wc >> 4);
                    buf[len++] = XRPC_ToHex(wc);
                    buf[len++] = _UCS2(';');
                    buf[len] = 0;

                    replace = buf;
                } else {

                    /* 16-bit Unicode */
                    int len = 0;
                    buf[len++] = _UCS2('&');
                    buf[len++] = _UCS2('#');
                    buf[len++] = _UCS2('x');
                    buf[len++] = XRPC_ToHex(wc >> 12);
                    buf[len++] = XRPC_ToHex(wc >>  8);
                    buf[len++] = XRPC_ToHex(wc >>  4);
                    buf[len++] = XRPC_ToHex(wc);
                    buf[len++] = _UCS2(';');
                    buf[len] = 0;

                    replace = buf;
                }
            }

            if (replace) {
                XRpcStringUCS2 ptr = replace;
                while (*ptr) {
                    int len = XRPC_WideCharToUTF8(*ptr++, utf);
                    if (!FILE_Write(client->out, utf, len)) {
                        return XRpcFalse;
                    }
                }
            } else {
                char utf[8];
                int len = XRPC_WideCharToUTF8(wc, utf);
                if (!FILE_Write(client->out, utf, len)) {
                    return XRpcFalse;
                }
            }
        }
    }
    return XRpcTrue;
}

/**
 * Encodes attributes.
 */
static
XRpcBool 
XRPC_SendAttributes(
    XRpcClient * client, 
    const XRpcElement * elem) 
{
    int i;
    int n = XRPC_AttributeCount(elem);
    for (i=0; i<n; i++) {
        const XRpcAttr* attr = XRPC_GetAttributeAt(elem, i);
        if (FILE_Puts(client->out, " ")         <= 0 ||
            FILE_Puts(client->out, attr->name)  <= 0 ||
            FILE_Puts(client->out, "=\"")       <= 0 ||
            FILE_Puts(client->out, attr->value) <= 0 ||
            FILE_Puts(client->out, "\"")        <= 0) {
            return XRpcFalse;
        }
    }
    return XRpcTrue;
}

/**
 * Sends indentation string
 */
STATIC XRpcBool XRPC_Indent(XRpcClient * client, int indent)
{
    int i;
    for (i=0; i<indent; i++) {
        if (!FILE_Puts(client->out, INDENT)) {
            return XRpcFalse;
        }
    }
    return XRpcTrue;
}

/**
 * Encodes container.
 */
XRpcBool 
XRPC_SendElement(
    XRpcClient * client, 
    const XRpcElement * e,
    int indent) 
{
    const XRpcContainer * c = XRPC_IsContainer(e);
    if (c) {
        return XRPC_SendContainer(client, c, indent);
    } else {
        XRpcString tag = XRPC_GetTagName(e->tag);
        const XRpcDataElement * de = XRPC_IsDataElement(e);

        if (!XRPC_Indent(client, indent) || 
            FILE_Puts(client->out, "<")  <= 0 ||
            FILE_Puts(client->out, tag)  <= 0 || 
            !XRPC_SendAttributes(client, e)) {
            return XRpcFalse;
        }

        if (de) {
            XRpcUcs2 * text = XRPC_GetAsText(de);
            if (text) {
                if (FILE_Puts(client->out, ">")  <= 0 ||
                    !XRPC_SendString(client, text)    ||

                    /* close the tag */
                    FILE_Puts(client->out, "</")  <= 0 ||
                    FILE_Puts(client->out, tag)   <= 0 ||
                    FILE_Puts(client->out, ">\n") <= 0) {

                    return XRpcFalse;
                }

                XRPC_Free(text);
                return XRpcTrue;
            }
        }
        
        if (FILE_Puts(client->out, "/>\n")  <= 0) {
            return XRpcFalse;
        }

        return XRpcTrue;
    }
}

/**
 * Encodes container.
 */
XRpcBool 
XRPC_SendContainer(
    XRpcClient * client, 
    const XRpcContainer * c,
    int indent) 
{
    int ne = XRPC_ElementCount(c);
    int na = XRPC_AttributeCount(&c->element);
    if (ne>0 || na>0) {
        int i;
        XRpcString tag = XRPC_GetTagName(c->element.tag);

        if (!XRPC_Indent(client, indent) || 
            FILE_Puts(client->out, "<")  <= 0 ||
            FILE_Puts(client->out, tag)  <= 0 || 
            !XRPC_SendAttributes(client, &c->element)) {
            return XRpcFalse;
        }

        if (ne > 0) {
            if (FILE_Puts(client->out, ">\n")  <= 0) {
                return XRpcFalse;
            }

            /* recursively send child elements */
            for (i=0; i<ne; i++) {
                const XRpcElement * e = XRPC_GetElementAt(c,i);
                if (!XRPC_SendElement(client, e, indent+1)) {
                    return XRpcFalse;
                }
            }

            /* close the tag */
            if (!XRPC_Indent(client, indent) || 
                !CLOSE_TAG(client, tag)) {
                return XRpcFalse;
            }
        } else {
            if (FILE_Puts(client->out, "/>\n")  <= 0) {
                return XRpcFalse;
            }
        }
    }
    return XRpcTrue;
}

/**
 * Sends </tag> string and terminates the line
 */
XRpcBool XRPC_CloseTag(XRpcClient * client, XRpcString tag)
{
    XRpcBool ok;
    CLIENT_WRITE_LOCK(client);
    ok = CLOSE_TAG(client,tag);
    CLIENT_UNLOCK(client);
    return ok;
}

/* *************************************************************************
//
//  Call
//
// *************************************************************************/

static
XRpcBool
XRPC_DoCall(
    XRpcClient * client,
    XRpcString tag,
    XRpcString id,
    XRpcString protocol,
    XRpcString method,
    const XRpcElement * args) 
{    
    ASSERT(client);
    ASSERT(protocol);
    ASSERT(method);

    if (FILE_Puts(client->out,"<")               <= 0 ||
        FILE_Puts(client->out,tag)               <= 0 || 
        FILE_Puts(client->out," ")               <= 0) {
        return XRpcFalse;
    }

    /* id may be null if response is not required */
    if (id) {
        if (FILE_Puts(client->out,XRPC_ATTR_ID)  <= 0 || 
            FILE_Puts(client->out,"=\"")         <= 0 || 
            FILE_Puts(client->out,id)            <= 0 || /* xmlize? */
            FILE_Puts(client->out,"\" ")         <= 0) {
            return XRpcFalse;
        }
    }
    if (FILE_Puts(client->out,XRPC_ATTR_PROTOCOL)<= 0 || 
        FILE_Puts(client->out,"=\"")             <= 0 || 
        FILE_Puts(client->out,protocol)          <= 0 ||
        FILE_Puts(client->out,"\" ")             <= 0 ||
        FILE_Puts(client->out,XRPC_ATTR_METHOD)  <= 0 || 
        FILE_Puts(client->out,"=\"")             <= 0 || 
        FILE_Puts(client->out,method)            <= 0 ||
        FILE_Puts(client->out,"\"")              <= 0) {
        return XRpcFalse;
    }

    if (!args) {

        XRpcBool ok = (FILE_Puts(client->out,"/>\n") > 0);
        if (ok) FILE_Flush(client->out);
        return ok;

    } else {

        XRpcBool ok;

        /* write the data */
        if (args->tag == XRpcTagParams) {

            /* it's a <params> container, write it as is */
            if (FILE_Puts(client->out,">\n") <= 0 ||
               !XRPC_SendElement(client, args, 1)) {
                return XRpcFalse;
            }
        } else {

            /* wrap it into a <params> block */
            if (FILE_Puts(client->out,">\n" INDENT "<") <= 0 ||
                FILE_Puts(client->out,XRPC_TAG_PARAMS)  <= 0 || 
                FILE_Puts(client->out,">\n")            <= 0 ||

                !XRPC_SendElement(client, args, 2)         ||

                FILE_Puts(client->out,INDENT "</")      <= 0 ||
                FILE_Puts(client->out,XRPC_TAG_PARAMS)  <= 0 || 
                FILE_Puts(client->out,">\n")            <= 0) {

                return XRpcFalse;
            }
        }


        ok = CLOSE_TAG(client,tag);
        if (ok) FILE_Flush(client->out);
        return ok;
    }
}

/**
 * Sends message to the remote side and returns waitable context.
 * The context must be eventually deallocated with XRPC_FreeCall
 */
XRpcCall * 
XRPC_API
XRPC_Call(
    XRpcClient * client,
    XRpcString protocol,
    XRpcString method,
    const XRpcElement * args)   /* element, <struct>, <array> or <params> */
{
    XRpcCall * call = XRPC_CreateCall(client->session);
    if (call) {
        XRpcString id = XRPC_GetID(call);
        CLIENT_WRITE_LOCK(client);    
        XRPC_DoCall(client, XRPC_TAG_CALL, id, protocol, method, args);
        CLIENT_UNLOCK(client);
    }
    return call;
}

/**
 * Sends a message to the remote side which does not require a confirmation.
 * Returns True if message has been successfully sent. There's no guarantee
 * that it has been (or will ever be) processed by the other side. This is
 * OK for many kinds of asynchronous notifications.
 */
XRpcBool
XRPC_Notify(
    XRpcClient * client,
    XRpcString protocol,
    XRpcString method,
    const XRpcElement * args) 
{
    XRpcBool ok;
    CLIENT_WRITE_LOCK(client);    
    ok = XRPC_DoCall(client, XRPC_TAG_CALL, NULL, protocol, method, args);
    CLIENT_UNLOCK(client);
    return ok;
}

/**
 * Adds formatted parameters into a container. The format is a sequence of 
 * format specifications each of which looks as follows
 * 
 *     %<name>!<type> 
 *     %%<type> 
 *
 * The latter means that the name of the argument preceeds the parameter
 * in the argument list. 
 *
 * The types are
 *
 *     s   - ASCII string
 *     u   - UCS2 string
 *     S   - wide string
 *     c   - XRpcByte
 *     h   - XRpcShort
 *     i   - XRpcInt
 *     l   - XRpcLong
 *     b   - XRpcBool
 *     d   - XRpcData (binary)
 *     x   - XRpcElement (will be deleted)
 *     n   - null
 *
 * other types are not supported at the moment
 */
XRpcBool
XRPC_API
XRPC_FormatArgs(
    XRpcContainer * params,
    XRpcString format,
    va_list va) 
{
    XRpcBool ok = XRpcFalse;
    XRpcString p = format;
    StrBuf32 buf;
    StrBuf * sb = &buf.sb;

    if (!params) {
        return XRpcFalse;
    }

    STRBUF_InitBufXXX(&buf);

    while (*p) {
        char type;
        XRpcElement * e = NULL;

        while (*p && isspace((int)(*p))) p++;
        if (*p == '%') {
            p++;
        } else {
            ok = (!*p);
            break;
        }

        /* read the name of the variable */
        STRBUF_Clear(sb);
        if (*p == '%') {
            XRpcString s = va_arg(va, XRpcString);
            if (!STRBUF_Append(sb, s)) {
                break;
            }
        } else {
            while (*p && *p != '!') {
                if (!STRBUF_AppendChar(sb,*p)) {
                    break;
                }
                p++;
            }
            if (*p != '!') {
                break;
            }
        }

        /* skip whatever chatacter we stopped at */
        p++;

        /* now we should be at the character that represents the type */
        type = *p++;
        switch (type) {
        case 's': 
            {
                XRpcStringElement * elem;
                elem = XRPC_CreateStringElement(va_arg(va,XRpcString),-1);
                if (elem) e = &elem->delement.element;
                break;
            }
        case 'u': 
            {
                XRpcStringElement * elem;
                elem = XRPC_CreateStringElementUCS2(va_arg(va,UStr),-1);
                if (elem) e = &elem->delement.element;
                break;
            }
        case 'S': 
            {
                XRpcStringElement * elem;
                elem = XRPC_CreateStringElementUCS2(va_arg(va,XRpcStringUCS2),-1);
                if (elem) e = &elem->delement.element;
                break;
            }
        case 'c': 
            {
                XRpcByteElement * elem;
                elem = XRPC_CreateByteElement((XRpcByte)va_arg(va,int));
                if (elem) e = &elem->delement.element;
                break;
            }
        case 'h': 
            {
                XRpcShortElement * elem;
                elem = XRPC_CreateShortElement((XRpcShort)va_arg(va,int));
                if (elem) e = &elem->delement.element;
                break;
            }
        case 'i': 
            {
                XRpcIntElement * elem;
                elem = XRPC_CreateIntElement(va_arg(va,XRpcInt));
                if (elem) e = &elem->delement.element;
                break;
            }
        case 'l': 
            {
                XRpcLongElement * elem;
                elem = XRPC_CreateLongElement(va_arg(va,XRpcLong));
                if (elem) e = &elem->delement.element;
                break;
            }
        case 'b':
            {
                XRpcBoolElement * elem;
                elem = XRPC_CreateBoolElement(va_arg(va,XRpcBool));
                if (elem) e = &elem->delement.element;
                break;
            }
        case 'd': 
            {
                XRpcBinaryElement * elem;
                XRpcData * data = va_arg(va,XRpcData*);
                elem = XRPC_CreateBinaryElement(data->ptr, data->size);
                if (elem) e = &elem->delement.element;
                break;
            }
        case 'x': 
            {
                e = va_arg(va,XRpcElement*);
                break;
            }
        case 'n': 
            {
                XRpcNullElement * elem = XRPC_CreateNullElement();
                if (elem) e = &elem->element;
                break;
            }
        default:
            ASSMSG1("Invalid format character '%c'",*p);
            break;
        }

        if (!e) {
            break;
        }

        /* set element name if it has one */
        if (STRBUF_Length(sb) > 0) {
            XRPC_SetElementName(e, STRBUF_GetString(sb));
        }

        /* add it to the container */
        if (!XRPC_AddElement(params, e)) {
            XRPC_FreeElement(e);
            break;
        }

        /* ok becomes True is we successfully got to the end of the format */
        ok = (!*p);
    }

    STRBUF_Destroy(sb);
    return ok;
}

/**
 * Same as XRPC_Notify but takes a format specification and variable number of
 * arguments. 
 */
XRpcBool
XRPC_API
XRPC_FormatNotify(
    XRpcClient * client,
    XRpcString protocol,
    XRpcString method,
    XRpcString format,
    ...) 
{
    XRpcBool ok = XRpcFalse;
    XRpcContainer * params = XRPC_CreateContainer(XRpcTagParams);

    va_list va;
    va_start(va, format);
    if (XRPC_FormatArgs(params, format, va)) {
        ok = XRPC_Notify(client, protocol, method, &params->element);
    }
    va_end(va);

    XRPC_FreeElement(&params->element);
    return ok;
}

/**
 * Same as XRPC_Call but takes a format specification and variable number of
 * arguments. 
 */
XRpcCall *
XRPC_API
XRPC_FormatCall(
    XRpcClient * client,
    XRpcString protocol,
    XRpcString method,
    XRpcString format,
    ...) 
{
    XRpcCall * call = NULL;
    XRpcContainer * params = XRPC_CreateContainer(XRpcTagParams);

    va_list va;
    va_start(va, format);
    if (XRPC_FormatArgs(params, format, va)) {
        call = XRPC_Call(client, protocol, method, &params->element);
    }
    va_end(va);

    XRPC_FreeElement(&params->element);
    return call;
}

/* *************************************************************************
//
//  Response
//
// *************************************************************************/

/**
 * Sends error response. The response looks like this:
 *
 *      <response id="A004" status="noproto"/>
 *
 * Returns True if response has been successfully sent, False if there was 
 * an I/O error (connection broken, etc.)
 */
XRpcBool 
XRPC_SendError(
    XRpcClient * client, 
    XRpcString id, 
    XRpcString status) 
{
    XRpcBool ok;
    ASSERT(id);
    CLIENT_WRITE_LOCK(client);
    
    ok = (FILE_Puts(client->out,"<")               > 0 && 
          FILE_Puts(client->out,XRPC_TAG_RESPONSE) > 0 && 
          FILE_Puts(client->out," ")               > 0 &&
          FILE_Puts(client->out,XRPC_ATTR_ID)      > 0 && 
          FILE_Puts(client->out,"=\"")             > 0 && 
          FILE_Puts(client->out,id)                > 0 && /* xmlize? */
          FILE_Puts(client->out,"\" ")             > 0 && 
          FILE_Puts(client->out,XRPC_ATTR_STATUS)  > 0 && 
          FILE_Puts(client->out,"=\"")             > 0 && 
          FILE_Puts(client->out,status)            > 0 &&
          FILE_Puts(client->out,"\"/>\n")          > 0);

    CLIENT_UNLOCK(client);
    return ok;
}

/**
 * Sends normal response. The response looks like this:
 *
 *      <response id="A004" status="ok">
 *          <results>
 *              data ...
 *          </results>
 *      </response>
 *
 * Returns True if response has been successfully sent, False if there was 
 * an I/O error (connection broken, etc.)
 */
STATIC
XRpcBool 
_XRPC_SendResponse(
    XRpcClient * client, 
    XRpcString id, 
    const XRpcElement * resp) 
{
    ASSERT(id);
    if (FILE_Puts(client->out,"<")               <= 0 ||
        FILE_Puts(client->out,XRPC_TAG_RESPONSE) <= 0 || 
        FILE_Puts(client->out," ")               <= 0 ||
        FILE_Puts(client->out,XRPC_ATTR_ID)      <= 0 || 
        FILE_Puts(client->out,"=\"")             <= 0 || 
        FILE_Puts(client->out,id)                <= 0 || /* xmlize? */
        FILE_Puts(client->out,"\" ")             <= 0 || 
        FILE_Puts(client->out,XRPC_ATTR_STATUS)  <= 0 || 
        FILE_Puts(client->out,"=\"")             <= 0 || 
        FILE_Puts(client->out,XRPC_STATUS_OK)    <= 0 ||
        FILE_Puts(client->out,"\"")              <= 0) {
        return XRpcFalse;
    }

    if (!resp) {
        return FILE_Puts(client->out,"/>\n") > 0;
    } else {
        if (FILE_Puts(client->out,">\n" INDENT "<") <= 0 ||
            FILE_Puts(client->out,XRPC_TAG_RESULT)  <= 0 || 
            FILE_Puts(client->out,">\n")            <= 0) {
            return XRpcFalse;
        }

        /* write the data */
        if (!XRPC_SendElement(client, resp, 2)) {
            return XRpcFalse;
        }

        /* close the tag */
        if (FILE_Puts(client->out,INDENT "</")      <= 0 ||
            FILE_Puts(client->out,XRPC_TAG_RESULT)  <= 0 || 
            FILE_Puts(client->out,">\n")            <= 0) {
            return XRpcFalse;
        }

        return CLOSE_TAG(client,XRPC_TAG_RESPONSE);
    }
}

/**
 * Synchronized wrapper for _XRPC_SendResponse
 */
XRpcBool 
XRPC_SendResponse(
    XRpcClient * client, 
    XRpcString id, 
    const XRpcElement * resp) 
{
    XRpcBool ok;
    CLIENT_WRITE_LOCK(client);    
    ok = _XRPC_SendResponse(client, id, resp);
    CLIENT_UNLOCK(client);
    return ok;
}

/* *************************************************************************
//
//  "xrpc.core"
//
// *************************************************************************/

STATIC Bool XRPC_ParseProtInfo(const XRpcElement* e, XRpcProtocolInfo* info)
{
    const XRpcContainer* c = XRPC_IsStruct(e, XCORE_STRUCT_PROTO_INFO);
    memset(info, 0, sizeof(*info));
    if (c) {
        /* we require at least protocol name */
        const XRpcStringElement * name = 
            XRPC_GetStringElementByName(c, 
            XCORE_STRUCT_PROTO_INFO_NAME);
        if (name) {
            info->name = XRPC_GetString(name);
            if (info->name) {
                /* everything else is optional */
                const XRpcContainer* version;
                const XRpcStringElement * s;
                s = XRPC_GetStringElementByName(c,XCORE_STRUCT_PROTO_INFO_DESCR);
                if (s) info->descr = XRPC_GetString(s);
                s = XRPC_GetStringElementByName(c,XCORE_STRUCT_PROTO_INFO_VENDOR);
                if (s) info->vendor = XRPC_GetString(s);

                /* version is optional too */
                version = XRPC_GetStructByName(c,
                    XCORE_STRUCT_PROTO_INFO_VER,
                    XCORE_STRUCT_PROTO_VER);
                if (version) {
                    const XRpcIntElement * v = 
                        XRPC_GetIntElementByName(version, 
                        XCORE_STRUCT_PROTO_VER_MINOR);
                    if (e) info->vminor = XRPC_GetInt(v);
                    v = XRPC_GetIntElementByName(version, 
                        XCORE_STRUCT_PROTO_VER_MAJOR);
                    if (e) info->vmajor = XRPC_GetInt(v);
                }

                return True;
            }
        }
    }
    return False;
}

/**
 * Call the specified callback once per each protocol registered on the 
 * other side of the connection, assuming that the other side implements
 * "xrpc.core" protocol (which will be one of the protocols). Returns 
 * True if the remote side supports "xrpc.core" protocol, False if the 
 * other side does not support it or if the remote call failed.
 */
XRpcBool
XRPC_API
XRPC_ListRemoteProtocols(
    XRpcClient * client,                /* XRPC client */
    XRpcProtInfoProc proc,              /* the callback to invoke */
    void * ctx)                         /* context passed to the callback */
{
    Bool ok = False;
    XRpcCall * call = XRPC_Call(client, 
        XCORE_PROTOCOL, 
        XCORE_METHOD_LIST_PROTOCOLS,
        NULL);
    if (call) {
        XRPC_Wait(call);
        ok = (XRPC_CallStatus(call) == XRpcStatusOK);
        if (ok) {
            const XRpcContainer * array = XRPC_IsArray(XRPC_CallResult(call));
            if (array) {
                int i, n = XRPC_ElementCount(array);
                for (i=0; i<n && ok; i++) {
                    const XRpcStringElement * s = 
                        XRPC_IsStringElement(
                        XRPC_GetDataElementAt(array, i));
                    if (s) {
                        XRpcString name = XRPC_GetString(s);
                        if (name) {

                            /* make the "getProtocolInfo" call */
                            XRpcCall * getInfo = XRPC_FormatCall(
                                client, 
                                XCORE_PROTOCOL, 
                                XCORE_METHOD_GET_PROTOCOL_INFO,
                                "%"XCORE_PARAM_PROTO_NAME"!s",
                                name);

                            if (getInfo) {
                                XRPC_Wait(getInfo);
                                if (XRPC_CallStatus(getInfo) == XRpcStatusOK) {
                                    XRpcProtocolInfo info;
                                    if (XRPC_ParseProtInfo(
                                        XRPC_CallResult(getInfo),
                                        &info)) {

                                        /* invoke the callback */
                                        if (!(*proc)(&info, ctx)) {
                                            break;
                                        }
                                    }
                                }
                                XRPC_FreeCall(getInfo);
                            }
                        }
                    }
                }
            }
        }
        XRPC_FreeCall(call);
    }
    return ok;
}

/**
 * Tests whether the connection is alive
 */
XRpcBool XRPC_API XRPC_IsAlive(XRpcClient * client)
{
    if (client) {
        XRpcCall * call;
        call = XRPC_Call(client,XCORE_PROTOCOL,XCORE_METHOD_PING,NULL);
        if (call) {
            XRpcStatus status;
            XRPC_Wait(call);
            status = XRPC_CallStatus(call);
            XRPC_FreeCall(call);
            switch (status) {
            case XRpcStatusNoTarget:
            case XRpcStatusNoMethod:
            case XRpcStatusOK:
                return XRpcTrue;
            default:
                ASSMSG1("Unexpected call status %d",status);
                /* fall through */
            case XRpcStatusSysErr:
                return XRpcFalse;
            }
        }
    }
    return XRpcFalse;
}

/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

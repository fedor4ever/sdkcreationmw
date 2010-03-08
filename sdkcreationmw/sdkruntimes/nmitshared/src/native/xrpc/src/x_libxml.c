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
#include "x_parse.h"

#ifdef USE_LIBXML

#include <libxml/parser.h>

/* *************************************************************************
//
//  Support for libxml
//  http://www.libxml.org/
//
//
//  NOTE: we don't want to use libxml for two reasons. 
// 
//  1. First is the technical reason. At the time of this writing, libxml 
//     was trying to fill the internal buffer before it actually starts 
//     parsing, even though it already has enough data to parse the current 
//     XML tag. It does not really matter if you are reading files because 
//     sooner or later you hit end of file and everything gets processed. 
//     This strategy if probably good for performance. But in our case it 
//     makes libxml pretty much unusable because everything that comes from 
//     the network we must be process as soon as possible. While something 
//     is sitting in libxml buffer unprocessed, the XRPC handler won't be 
//     invoked. At the same time the other side may not be sending any more
//     data until it gets a reply from this side.
//
//  2. Non-technical reason. libxml is distributed under LGPL license. This 
//     means that we cannot compile it as a static library - it must be be 
//     a standalone executable (dynamic library). It's better than GPL but
//     I would like to be able to compile all the code into a single exe.
//     Which I cannot do with LGPL code.
//
// *************************************************************************/

/* *************************************************************************
//
//  Data types
//
// *************************************************************************/

typedef struct _LibXmlContext {
    I32s signature;             /* LIBXML_CONTEXT_SIGNATURE */
    void * xrpcContext;         /* XRPC context */
    XParseHandlers handlers;    /* XRPC event handlers */
    wchar_t * buf;              /* buffer for converting UTF-8 to UCS-2 */
    int alloc;                  /* size of wide above buffer */
} LibXmlContext;

#define LIBXML_CONTEXT_SIGNATURE _SIGNATURE32('L','X','M','L')

/* *************************************************************************
//
//  Support for libxml
//  http://www.libxml.org/
//
// *************************************************************************/

/**
 * "startElement" callback being invoked by libxml
 */
static void 
LIBXML_StartElement(void *ctx, const xmlChar *name, const xmlChar **atts) {
    LibXmlContext * libxmlContext = (LibXmlContext *)ctx;
    ASSERT(libxmlContext->signature == LIBXML_CONTEXT_SIGNATURE);
    TRACE1("<%s>\n",name);
    libxmlContext->handlers.startElement(libxmlContext->xrpcContext,name,atts);
}

/**
 * "endElement" callback being invoked by libxml
 */
static void 
LIBXML_EndElement(void *ctx, const xmlChar *name) {
    LibXmlContext * libxmlContext = (LibXmlContext *)ctx;
    ASSERT(libxmlContext->signature == LIBXML_CONTEXT_SIGNATURE);
    TRACE1("</%s>\n",name);
    libxmlContext->handlers.endElement(libxmlContext->xrpcContext,name);
}

/**
 * "characterData" callback being invoked by libxml
 */
static void 
LIBXML_Characters(void * ctx, const xmlChar *ch, int len) {
    int rem, i, n;
    LibXmlContext * libxmlContext = (LibXmlContext *)ctx;
    ASSERT(libxmlContext->signature == LIBXML_CONTEXT_SIGNATURE);

    /*
     * len is a number of bytes which may be greater than (but at least 
     * equal to) number of characters. We may be allocating more space than 
     * we actually need, but calculating the exact length would add more
     * overhead in terms of CPU usage. So, allocating len wide characters 
     * is a good compromise I think.
     */
    if (libxmlContext->alloc < len) {
        wchar_t* newbuf = (wchar_t*)
            MEM_Realloc(libxmlContext->buf,len *sizeof(wchar_t));
        if (!newbuf) {
            if (libxmlContext->buf) {
                MEM_Free(libxmlContext->buf);
                libxmlContext->buf = NULL;
            }
            libxmlContext->alloc = 0;
            return;
        }

        libxmlContext->alloc = len;
        libxmlContext->buf = newbuf;
    }

    /* convert UTF-8 to UNICODE */
    i = 0;
    rem = len;
    while ((n = XRPC_GetUTF8Char(ch, libxmlContext->buf + i, &rem)) > 0) {
        ch += n;
        i++;
    }
    libxmlContext->buf[n] = 0;
    ASSERT(n <= len);
    ASSERT(rem == 0);

    /* invoke XRPC handler */
    libxmlContext->handlers.characterData(libxmlContext->xrpcContext, 
                                          libxmlContext->buf, n);
}

/**
 * This routine runs libxml parser
 */
XRpcBool 
LIBXML_Parse (
    XParseInput * input,        /* input stream descriptor */
    XParseHandlers * handlers,  /* event handlers */
    void * context)             /* XRPC context */
{
    xmlParserCtxtPtr parserContext;
    xmlSAXHandler saxHandlers;
    LibXmlContext libxmlContext;


    /* initialize libxmlContext */
    memset(&libxmlContext, 0, sizeof(libxmlContext));
    libxmlContext.signature = LIBXML_CONTEXT_SIGNATURE;
    libxmlContext.xrpcContext = context;
    libxmlContext.handlers = *handlers;

    /* initialize saxHandlers */
    memset(&saxHandlers, 0, sizeof(saxHandlers));
    saxHandlers.startElement = LIBXML_StartElement; 
    saxHandlers.endElement = LIBXML_EndElement; 
    saxHandlers.characters = LIBXML_Characters;

    /* create parser context */
    parserContext = xmlCreateIOParserCtxt(&saxHandlers, &libxmlContext,
                                          input->read, NULL,
                                          context, XML_CHAR_ENCODING_UTF8);
    /* run the parser */    
    if (parserContext) {
        xmlParseDocument(parserContext);
        return XRpcTrue;
    } else {
        return XRpcFalse;
    }
}

#endif /* USE_LIBXML */

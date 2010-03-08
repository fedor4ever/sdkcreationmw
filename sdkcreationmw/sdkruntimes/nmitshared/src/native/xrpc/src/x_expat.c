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

#ifdef USE_EXPAT
#define XML_UNICODE
#define _STATIC

#include <expat.h>

/*
 * the point of this ASSERT is to avoid incliding broken <expat.h>
 * which comes with some versions of Linux. That file always defines 
 * XML_Char as char regardles of whether XML_UNICODE is defined.
 */
COMPILE_ASSERT(sizeof(XML_Char) > sizeof(char))

/* *************************************************************************
//
// Support for James Clark's EXPAT parser 
// http://www.jclark.com/xml/expat.html
//
// *************************************************************************/

typedef struct _ExpatContext {
    I32s signature;             /* EXPAT_CONTEXT_SIGNATURE */
    void * xrpcContext;         /* XRPC context */
    XParseHandlers handlers;    /* XRPC event handlers */
} ExpatContext;

#define EXPAT_CONTEXT_SIGNATURE _SIGNATURE32('X','P','A','T')

/**
 * "startElement" callback being invoked by expat
 */
STATIC void 
EXPAT_StartElement(void *ctx, const XML_Char *tag, const XML_Char **atts)
{
    char * name;
    ExpatContext * expat = (ExpatContext *)ctx;

    ASSERT(expat->signature == EXPAT_CONTEXT_SIGNATURE);
    
    if ((name = XRPC_WideStringToASCII(tag)) != NULL) {
        char** a;
        int i,n=0;
        while (atts[n++]);

        a = MEM_NewArray(char*,n);
        if (a) {
            for (i=0; i<n; i++) {
                a[i] = XRPC_WideStringToASCII(atts[i]);
            }

            expat->handlers.startElement(expat->xrpcContext,name,a);

            for (i=0; i<n; i++) {
                if (a[i]) {
                    MEM_Free(a[i]);
                }
            }
            MEM_Free(a);
        }
        MEM_Free(name);
    }
}

/**
 * "endElement" callback being invoked by libxml
 */
STATIC void EXPAT_EndElement(void *ctx, const XML_Char * tag)
{
    char * name;
    ExpatContext * expat = (ExpatContext *)ctx;
    ASSERT(expat->signature == EXPAT_CONTEXT_SIGNATURE);
    if ((name = XRPC_WideStringToASCII(tag)) != NULL) {
        expat->handlers.endElement(expat->xrpcContext,name);
        MEM_Free(name);
    }
}

/**
 * "characterData" callback being invoked by libxml
 */
STATIC void EXPAT_Characters(void * ctx, const XML_Char *ch, int len)
{
    ExpatContext * expat = (ExpatContext *)ctx;
    ASSERT(expat->signature == EXPAT_CONTEXT_SIGNATURE);
    expat->handlers.characterData(expat->xrpcContext, ch, len);
}

/**
 * This routine runs expat parser
 */
XRpcBool
EXPAT_Parse (
    XParseInput * input,        /* input stream descriptor */
    XParseHandlers * handlers,  /* event handlers */
    void * context)             /* XRPC context */
{
    XML_Parser parser = XML_ParserCreate(NULL);

    ASSERT(parser);    
    if (parser) {
        char buf[1024];
        XRpcBool ok = XRpcTrue;
        XRpcBool done = XRpcFalse;
        ExpatContext expatContext;
    
        /* initialize expatContext */
        memset(&expatContext, 0, sizeof(expatContext));
        expatContext.signature = EXPAT_CONTEXT_SIGNATURE;
        expatContext.xrpcContext = context;
        expatContext.handlers = *handlers;

        /* initialize SAX handlers */
        XML_SetElementHandler(parser, EXPAT_StartElement, EXPAT_EndElement);
        XML_SetCharacterDataHandler(parser, EXPAT_Characters);
        XML_SetUserData(parser, &expatContext);

        do {
            int len = input->read(context, buf, sizeof(buf));
            if (len <= 0) {
                done = XRpcTrue;
                len = 0;
            }
            ok = XML_Parse(parser, buf, len, done);

        } while (ok && !done);

        XML_ParserFree(parser);
        return ok;
    }
    return XRpcFalse;
}

#endif /* USE_EXPAT */

/*
 * Local Variables:
 * c-basic-offset:4
 * indent-tabs-mode: nil
 * End:
 */

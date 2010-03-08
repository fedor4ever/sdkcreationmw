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

#if !defined(USE_EXPAT) && !defined(USE_LIBXML)
#  error "Please choose a parser."
#endif

/*
 * Internal parser context
 */
typedef void (*FreeParserProc) (
    XParser * parser            /* parser context */
);

typedef struct _XParserInternal {
    FreeParserProc free;        /* the free() function */
    XParser parser;             /* the public part */
} XParserInternal;

/* *************************************************************************
//
//  Support for "generic parser"
//
// *************************************************************************/

#define GENERIC_PARSER_SIGNATURE _SIGNATURE32('G','P','A','R')

typedef XRpcBool (*GenericParserProc) (
    XParseInput * input,        /* input stream descriptor */
    XParseHandlers * handlers,  /* event handlers */
    void * context              /* XRPC context */
);

typedef struct _GenericParser {
    int32_t sign;               /* GENERIC_PARSER_SIGNATURE */
    GenericParserProc proc;     /* the parser function */
    XParserInternal internal;   /* the internal parser definition */
} GenericParser;

/*
 * A generic parser routine. Invokes the actual handler
 */
static 
void 
XPARSE_API
_XPARSE_GenericParse (
    XParser * parser,           /* parser context */
    XParseInput * input,        /* input stream descriptor */
    XParseHandlers * handlers,  /* event handlers */
    void * context)             /* XRPC context */
{
    GenericParser* gen = CAST(parser,GenericParser,internal.parser);
    ASSERT(gen->sign == GENERIC_PARSER_SIGNATURE);
    if (gen->sign == GENERIC_PARSER_SIGNATURE) {
        gen->proc(input, handlers, context);
    }
}

/*
 * Deallocates GenericParser structure
 */
static 
void
XPARSE_API
_XPARSE_GenericFree (
    XParser * parser)           /* parser context */
{
    GenericParser* gen = CAST(parser,GenericParser,internal.parser);
    ASSERT(gen->sign = GENERIC_PARSER_SIGNATURE);
    if (gen->sign == GENERIC_PARSER_SIGNATURE) {
        gen->sign = 0;
        MEM_Free(gen);
    }
}

/*
 * Creates a generic parser.
 */
static 
XParser *
_XPARSE_CreateGenericParser(
    GenericParserProc proc,
    const char * name,
    const char * descr )
{
    GenericParser* gen = MEM_New(GenericParser);
    if (gen) {
        memset(gen, 0, sizeof(GenericParser));
        gen->sign = GENERIC_PARSER_SIGNATURE;
        gen->proc = proc;
        gen->internal.free = _XPARSE_GenericFree;
        gen->internal.parser.parse = _XPARSE_GenericParse;
        gen->internal.parser.name  = name;
        gen->internal.parser.descr = descr;
    }
    return &gen->internal.parser;
}

/* *************************************************************************
//
//  Interface between XRPC and XML parser
//
// *************************************************************************/

/*
 * This function returns parser by name. Calling it with NULL name will 
 * return the default parser which is probably the most common scenario.
 */
XParser *
XPARSE_API 
XPARSE_CreateParser(
    const char * name)          /* the parser name, NULL to get default */
{
#ifdef USE_EXPAT
    if (!name || strcmp(EXPAT_PARSER,name) == 0) {
        return _XPARSE_CreateGenericParser(EXPAT_Parse, 
                                           EXPAT_PARSER, 
                                           EXPAT_DESCR);
    }
#endif /* USE_EXPAT */
#ifdef USE_LIBXML
    if (!name || strcmp(LIBXML_PARSER,name) == 0) {
        return _XPARSE_CreateGenericParser(LIBXML_Parse, 
                                           LIBXML_PARSER, 
                                           LIBXML_DESCR);
    }
#endif /* USE_LIBXML */
#if !defined(USE_EXPAT) && !defined(USE_LIBXML)
#  error "No parser defined!"
#endif
    TRACE1("ERROR: Unknown parser: %s\n",name);
    return NULL;    
}

/*
 * Free the parser and deallocate all the memory associated with it.
 */
void
XPARSE_API 
XPARSE_FreeParser(
    XParser *  parser)          /* the parser to free. */
{
    if (parser) {
        XParserInternal * internal = CAST(parser,XParserInternal,parser);
        internal->free(parser);
    }
}

/*
 * Convenience routine. Opens the default parser, invokes it and frees 
 * the parser after its parse routine returns. Returns zero if anything
 * fails, non zero (probably, 1) if parser was invoked. 
 */
int 
XPARSE_API 
XPARSER_Parse(
    XParseInput * input,        /* input stream descriptor */
    XParseHandlers * handlers,  /* event handlers */
    void * context)             /* XRPC context */
{
    XParser * parser = XPARSE_CreateParser(NULL);
    ASSERT(parser);
    if (parser) {
        parser->parse(parser, input, handlers, context);
        XPARSE_FreeParser(parser);
        return True;
    }
    return False;
}

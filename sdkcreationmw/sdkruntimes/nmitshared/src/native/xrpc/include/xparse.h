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


#ifndef _XPARSE_H_
#define _XPARSE_H_

#include <xrpc.h>

/* *************************************************************************
//
//  Interface between XRPC and XML parser
//
// *************************************************************************/

#ifdef _WIN32
#  define XPARSE_API __cdecl
#else
#  define XPARSE_API 
#endif 

/* *************************************************************************
//
//  Known parsers
//
// *************************************************************************/

#define EXPAT_PARSER  "expat"
#define LIBXML_PARSER "libxml"

/* *************************************************************************
//
//  Input 
//
// *************************************************************************/

/* 
 * Provides input to the parser. Returns number of bytes read, -1 on error
 */
typedef int (XPARSE_API * XParseInputRead) (
    void * context,             /* XRPC defined context */
    char * buf,                 /* biffer to read the data into */
    int len                     /* size of the buffer */
);

/*
 * This structure contains everything that XML parser needs to know about 
 * the input stream
 */
typedef struct _XParseInput {
    XParseInputRead  read; 
} XParseInput;

/* *************************************************************************
//
//  Event handlers
//
//  Note: tag names and their attributes (including both names and values)
//  must be within US_ASCII character set. Character data are provided in 
//  UNICODE allowing for international characters
//
// *************************************************************************/

typedef char ** XRpcAtts;      /* attribute set */

/*
 * Receives SAX style "startElement" notification
 */
typedef void (XPARSE_API * XParseStartElement) (
    void * context,             /* XRPC defined context */
    const char * name,          /* the element name */
    const XRpcAtts atts         /* attributes */
);

/*
 * Receives SAX style "endElement" notification
 */
typedef void (XPARSE_API * XParseEndElement) (
    void * context,             /* XRPC defined context */
    const char * name           /* the element name */
);

/*
 * Receives SAX style "characterData" notification. The string does not 
 * have to be NULL terminated. In fact, it's almost certainly not NULL
 * terminated. The length of the data block (in characters) if given as 
 * the third parameter.
 */
typedef void (XPARSE_API * XParseCharacterData) (
    void * context,             /* XRPC defined context */
    const XRpcUcs2 * chars,     /* the characters */
    int len                     /* number of charactars */
);

/*
 * This structure defines event handlers for all the events XRPC is 
 * interested in. It does not need much.
 */
typedef struct _XParseHandlers {
    XParseStartElement  startElement; 
    XParseEndElement    endElement;
    XParseCharacterData characterData; 
} XParseHandlers;

/* *************************************************************************
//
//  API
//
// *************************************************************************/

typedef struct _XParser XParser;

/*
 * Parser's entry point. Starts parsing the input. Returns when done.
 * Does not return anything because XRPC does not care
 */
typedef void (XPARSE_API * XParserParse)(
    XParser * parser,           /* parser context */
    XParseInput * input,        /* input stream descriptor */
    XParseHandlers * handlers,  /* event handlers */
    void * context              /* XRPC context */
);

/*
 * A reusable parser object. "Reusable" means that you can parse 
 * multiple documents by invoking parse routine of this object many 
 * times. The strings (parser's name and description) may be allocated 
 * dynamically and only be valid until XPARSE_FreeParser() is invoked.
 */
struct _XParser {
    XParserParse parse;         /* parser's entry point */
    const char * name;          /* parser's short name */
    const char * descr;         /* parser's description (long name) */
};

/*
 * This function returns parser by name. Calling it with NULL name will 
 * return the default parser which is probably the most common scenario.
 */
XParser *
XPARSE_API 
XPARSE_CreateParser (
    const char * name           /* the parser name, NULL to get default */
);

/*
 * Free the parser and deallocate all the memory associated with it.
 */
void
XPARSE_API 
XPARSE_FreeParser (
    XParser *  parser           /* the parser to free. */
);

/*
 * Convenience routine. Opens the default parser, invokes it and frees 
 * the parser after its parse routine returns. Returns zero if anything
 * fails, non zero (probably, 1) if parser was invoked. 
 */
int 
XPARSE_API 
XPARSER_Parse (
    XParseInput * input,        /* input stream descriptor */
    XParseHandlers * handlers,  /* event handlers */
    void * context              /* XRPC context */
);

#endif /* _XPARSE_H_ */

/*
 * Local Variables:
 * mode:C
 * c-basic-offset:4
 * indent-tabs-mode: nil
 * End:
 */

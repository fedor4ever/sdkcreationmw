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


#ifndef _XRPC_PARSE_H_
#define _XRPC_PARSE_H_

/* *************************************************************************
//
//  XPARSE internal header file
//
// *************************************************************************/

/* 
 * Interface to James Clark's EXPAT parser 
 * See http://www.jclark.com/xml/expat.html
 */
#ifdef USE_EXPAT

#define EXPAT_DESCR "James Clark's EXPAT parser"

XRpcBool 
EXPAT_Parse (
    XParseInput * input,        /* input stream descriptor */
    XParseHandlers * handlers,  /* event handlers */
    void * context              /* XRPC context */
);

#endif /* USE_EXPAT */

/* 
 * Interface to libxml parser 
 * See http://www.libxml.org/
 */
#ifdef USE_LIBXML

#define LIBXML_DESCR "GNOME libxml parser"

XRpcBool 
LIBXML_Parse (
    XParseInput * input,        /* input stream descriptor */
    XParseHandlers * handlers,  /* event handlers */
    void * context              /* XRPC context */
);

#endif /* USE_LIBXML */

#endif /* _XRPC_PARSE_H_ */

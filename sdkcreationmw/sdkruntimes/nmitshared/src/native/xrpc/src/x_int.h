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


#ifndef _XRPC_INT_H_
#define _XRPC_INT_H_

/* *************************************************************************
//
//  XRPC internal header file
//
// *************************************************************************/

#include "xrpc.h"
#include "xcore.h"
#include "xparse.h"

#include <s_base64.h>
#include <s_buf.h>
#include <s_mem.h>
#include <s_hash.h>
#include <s_lock.h>
#include <s_event.h>
#include <s_thread.h>
#include <s_vector.h>
#include <s_queue.h>
#include <s_file.h>
#include <s_util.h>
#include <s_wkq.h>

#include <ucs2lib.h>

#include "x_parse.h"

/* convert single byte character to UCS2 */
#define _UCS2(_char) (((XRpcUcs2)(_char)) & 0xff)

/*
 * Values of the boolean type. Case sensitive
 */
extern const XRpcStringUCS2 XRPC_True;
extern const XRpcStringUCS2 XRPC_False;

/*
 * These have to do with generation of XML messages
 */
#define INDENT "  "
#define XML_HEADER  "<?xml version='1.0' encoding='utf-8' standalone='yes'?>\n"

/* *************************************************************************
//
//  Internal protocol descriptor
//
// *************************************************************************/

typedef struct _Protocol {
    char *          name;       /* protocol name */
    char *          descr;      /* protocol description */
    char *          vendor;     /* protocol vendor */
    int             vmajor;     /* major version of the protocol */
    int             vminor;     /* minor version of the protocol */
    XRpcProtocol    protocol;   /* protocol descriptor */
    void *          context;    /* protocol context */
} Protocol;

typedef XRpcBool (*ListProtocolCB)(Protocol * protocol, void * context);

/* XRPC Core protocol descriptor */

extern const XRpcProtocol XCORE_Protocol;

/* *************************************************************************
//
//  Elements types
//
// *************************************************************************/

typedef struct _ElementType ElementType;
typedef struct _DataElementType DataElementType;

struct _XRpcElement {
    uint32_t signature;     /* type-specific signature */
    ElementType * type;     /* points to data shared by all instances */
    XRpcContainer * parent; /* parent container */
    XRpcDataType dataType;  /* the data type */
    XRpcTag tag;            /* tag that represents the element */
    char *  name;           /* points to name, most commonly used attribute */
    Vector  attr;           /* all attributes*/
};

struct _XRpcContainer {
    XRpcElement   element;  /* the element data */
    Vector        kids;     /* ordered list of children elements */
    HashTable     map;      /* maps names to children */
};

struct _XRpcDataElement {
    XRpcElement   element;  /* base element */
};

struct _XRpcBoolElement {
    XRpcDataElement delement;
    XRpcBool b;
};

struct _XRpcByteElement {
    XRpcDataElement delement;
    XRpcByte n;
};

struct _XRpcShortElement {
    XRpcDataElement delement;
    XRpcShort n;
};

struct _XRpcIntElement {
    XRpcDataElement delement;
    XRpcInt n;
};

struct _XRpcLongElement {
    XRpcDataElement delement;
    XRpcLong n;
};

struct _XRpcDoubleElement {
    XRpcDataElement delement;
    XRpcDouble d;
};

struct _XRpcBinaryElement {
    XRpcDataElement delement;
    XRpcData data;          /* the data */
};

struct _XRpcStringElement {
    XRpcDataElement delement; 
    size_t len;             /* string length, in characters */
    XRpcUcs2 * ws;          /* wide string */
    char * utf;             /* UTF8 string */
    char * s;               /* sigle byte string */
};

/* *************************************************************************
//
//  Functions
//
// *************************************************************************/

/* data */

XRpcBool 
XRPC_IsValidChild(
    XRpcTag         parent, 
    XRpcTag         child
);

XRpcContainer *
_XRPC_IsContainer(
    const XRpcElement * e
);

XRpcDataElement *
_XRPC_IsDataElement(
    const XRpcElement * e
);

XRpcElement * 
XRPC_DetachElementAt(
    XRpcContainer * container, 
    int             index
);

XRpcBool 
XRPC_DetachElement(
    XRpcContainer * container,
    XRpcElement *   elem
);

/* client */

XRpcClient *
XRPC_CreateClient(
    XRpcSession *   session,
    File *          out
);

void
XRPC_FreeClient(
    XRpcClient *    client
);

XRpcBool 
XRPC_CloseTag(
    XRpcClient *    client, 
    XRpcString      tag
);

XRpcBool 
XRPC_SendError(
    XRpcClient *    client, 
    XRpcString      id, 
    XRpcString      status
);
 
XRpcBool 
XRPC_SendResponse(
    XRpcClient *    client, 
    XRpcString      id, 
    const XRpcElement * resp
);
 
XRpcBool 
XRPC_SendElement(
    XRpcClient *    client, 
    const XRpcElement * e,
    int             indent
);
 
XRpcBool 
XRPC_SendContainer(
    XRpcClient *    client, 
    const XRpcContainer * c,
    int             indent
);

/* server */

XRpcCall * 
XRPC_CreateCall(
    XRpcSession *   server
);

XRpcString
XRPC_GetID(
    XRpcCall *      call
);
 
Protocol * 
XRPC_GetProtocol(
    XRpcServer *    server,
    XRpcString      name
);

void 
XRPC_ListProtocols(
    XRpcServer *    server,
    ListProtocolCB  cb,
    void *          context
);

#endif /* _XRPC_INT_H_ */

/*
 * Local Variables:
 * mode:C
 * c-basic-offset:4
 * indent-tabs-mode: nil
 * End:
 */

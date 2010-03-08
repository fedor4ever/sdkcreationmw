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

/**
 * Handles "ping" call
 */
STATIC XRpcElement * XRPC_API XCORE_Ping(void * ctx, const XRpcContainer * p)
{
    return NULL;
}

/**
 * Protocol enumaration callback. Adds protocol name to the array.
 */
STATIC Bool XCORE_ListCB(Protocol * protocol, void * context)
{
    XRpcContainer * names = (XRpcContainer*)context;
    XRpcStringElement * name = XRPC_CreateStringElement(protocol->name, -1);
    if (name) XRPC_TryAddElement(names, &name->delement.element);
    return True;
}

/**
 * Handles "listProtocols" call
 */
STATIC XRpcElement * XRPC_API 
XCORE_ListProtocols(void * ctx, const XRpcContainer * p)
{
    XRpcContainer * names = XRPC_CreateArray();
    if (names) {
        XRpcServer * server = (XRpcServer*)ctx;
        XRPC_ListProtocols(server, XCORE_ListCB, names);
        return XRPC_ContainerToElement(names);
    }
    return NULL;
}

/**
 * Creates "xrpc.core.ProtocolInfo" structure and fills it with protocol
 * information
 */
STATIC XRpcContainer * XCORE_EncodeProtocolInfo(Protocol * p)
{
    XRpcContainer * info = XRPC_CreateStruct(XCORE_STRUCT_PROTO_INFO);
    if (info) {
        XRpcContainer * ver = XRPC_CreateStruct(XCORE_STRUCT_PROTO_VER);
        XRPC_AddStringField(info,XCORE_STRUCT_PROTO_INFO_NAME,p->name);
        XRPC_AddStringField(info,XCORE_STRUCT_PROTO_INFO_VENDOR,p->vendor);
        XRPC_AddStringField(info,XCORE_STRUCT_PROTO_INFO_DESCR,p->descr);
        if (ver) {
            XRPC_AddIntField(ver,XCORE_STRUCT_PROTO_VER_MAJOR,p->vmajor);
            XRPC_AddIntField(ver,XCORE_STRUCT_PROTO_VER_MINOR,p->vminor);
            XRPC_SetElementName(&ver->element, XCORE_STRUCT_PROTO_INFO_VER);
            XRPC_TryAddElement(info, &ver->element);
        }
    }
    return info;
}

/**
 * Handles "getProtocolInfo" call
 */
STATIC XRpcElement * XRPC_API 
XCORE_GetProtocolInfo(void * context, const XRpcContainer * params)
{
    if (params) {
        const XRpcStringElement * str = XRPC_IsStringElement(
                                        XRPC_IsDataElement(
                                        XRPC_GetElementByName(params,
                                        XCORE_PARAM_PROTO_NAME)));
        if (str) {
            XRpcString name = XRPC_GetString(str);
            XRpcServer * server = (XRpcServer*)context;
            Protocol * protocol = XRPC_GetProtocol(server, name);
            if (protocol) {
                XRpcContainer * info = XCORE_EncodeProtocolInfo(protocol);
                if (info) {
                    return &info->element;
                }
            }
        }
    }
    return NULL;
}

/**
 * The Core protocol callback. The context parameter points to the XRpcServer
 */
STATIC
XRpcBool 
XRPC_API 
XCORE_ProtocolCB(
    void *                  context,    /* protocol specific context */
    XRpcString              name,       /* method name */
    XRpcMethodInfo *        info)       /* receives handler and context */
{
    if (StrCmp(XCORE_METHOD_PING,name) == 0) {
        info->handler = XCORE_Ping;
    } else if (StrCmp(XCORE_METHOD_LIST_PROTOCOLS,name) == 0) {
        info->handler = XCORE_ListProtocols;
    } else if (StrCmp(XCORE_METHOD_GET_PROTOCOL_INFO,name) == 0) {
        info->handler = XCORE_GetProtocolInfo;
    } else {
        return XRpcFalse;
    }
    info->context = context;
    info->synchronous = True;
    return XRpcTrue;
}

/* XRPC Core protocol descriptor */

const XRpcProtocol XCORE_Protocol = {
    XCORE_ProtocolCB,                   /* getmethod */
    NULL,                               /* cancel */
    NULL,                               /* sopen */
    NULL                                /* sclose */
};

/*
 * Local Variables:
 * c-basic-offset:4
 * indent-tabs-mode: nil
 * End:
 */

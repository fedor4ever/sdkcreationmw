/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#include "EcmtGwP.h"

/* These must match the contants defined in PushURL.cpp */
#define URL_PARAM_TYPE      0
#define BROWSER_PARAM_TYPE  1

/* Notification types */
struct _GwNotify {
    Str method;
    int mask;
};
const GwNotify _GwNotifySend = {
    ECMTGW_LISTENER_SEND_METHOD,
    ECMTGW_LISTEN_MASK_SEND
};
const GwNotify _GwNotifyReceive = {
    ECMTGW_LISTENER_RECV_METHOD,
    ECMTGW_LISTEN_MASK_RECEIVE
};

/**
 * "toFront" method handler
 */
STATIC XRpcElement* GWENG_XRpcToFront(void* ctx,  const XRpcContainer* param)
{
    EcmtGateway* gw = ctx;
    TRACE("GW: XRpcToFront\n");
    if (gw->cb.toFront) {
        gw->cb.toFront(gw, gw->ctx);
    }
    return NULL;
}

/**
 * "exit" method handler
 */
STATIC XRpcElement* GWENG_XRpcExit(void* ctx, const XRpcContainer* params)
{
    EcmtGateway* gw = ctx;
    TRACE("GW: XRpcExit\n");
    if (gw->cb.exit) {
        gw->cb.exit(gw, gw->ctx);
    }
    return NULL;
}

/**
 * Packs data for Push URL ECMT plugin. Data packed in the format:
 * <param_type><param_length><param_data>, where
 * <param_type> - 1 byte (0 if param is URL, 1 - if browser type)
 * <param_length> - 4 bytes (length if param_data in bytes)
 * <param_data> - actual data to sent
 * Function allocate the memory and returns pointer to the allocated
 * buffer in packed_data arg.
 * Returns length of packed data.
 */
STATIC Bool PushURLDataPack(const XRpcStringElement* urlParam,
                            const XRpcIntElement* browserParam,
                            Buffer* buf)
{
    if (urlParam) {
        int url_len = XRPC_GetStringLength(urlParam);
        if (url_len > 0) {
            Str url_str = XRPC_GetString(urlParam);
            if (url_str) {
                if (!BUFFER_PutI8(buf, URL_PARAM_TYPE) ||
                    !BUFFER_PutI32(buf, url_len) ||
                    BUFFER_Put(buf, url_str, url_len, False) != url_len) {
                    return False;
                }
            }
        }
    }

    if (browserParam) {
        if (!BUFFER_PutI8(buf, BROWSER_PARAM_TYPE) ||
            !BUFFER_PutI32(buf, 4) ||
            !BUFFER_PutI32(buf, XRPC_GetInt(browserParam))) {
            return False;
        }
    }
    return True;
}

/**
 * "load" method handler
 */
STATIC XRpcElement* GWENG_XRpcLoadURL(void* ctx,  const XRpcContainer* params)
{
    EcmtGateway* gw = ctx;
    Bool connected = False;
    const XRpcElement* portElem = /* without type checking */
        XRPC_GetElementByName(params, ECMTGW_LOAD_PORT_PARAM);
    const XRpcIntElement* intPortElem = NULL;
    const XRpcStringElement* url =
        XRPC_GetStringElementByName(params, ECMTGW_LOAD_URL_PARAM);
    const XRpcIntElement* browser =
        XRPC_GetIntElementByName(params, ECMTGW_LOAD_BROWSER_PARAM);
    XRpcElement* result = NULL;

    TRACE("GW: XRpcLoadURL\n");

    /*
     * If "port" parameter is an <int> element, then it's the COM port number.
     * If it's a <string> element then it can be either full name of the COM
     * port or name of the Bluetooth device.
     */
    if (portElem) {
        EcmtGatewayPort* port = NULL;
        intPortElem = XRPC_IsIntElement(XRPC_IsDataElement(portElem));
        if (intPortElem) {
            if (gw->comPorts) {
                char comPort[COM_PORT_NAME_LEN];
                sprintf(comPort, "COM%d",XRPC_GetInt(intPortElem));
                port = GWENG_PortByName(gw->comPorts, comPort, True);
            }
        } else {
            const XRpcStringElement* strPortElem = XRPC_IsStringElement(
                XRPC_IsDataElement(portElem));
            if (strPortElem) {
                Str portName = XRPC_GetString(strPortElem);
                port = GWENG_PortByName(gw->btSockets, portName, True);
                if (!port) {
                    port = GWENG_PortByName(gw->comPorts, portName, True);
                }
            }
        }
        if (port) {
            connected = ECMTGW_Activate(port);
            GWENG_PortReleaseRef(port);
        }
        else {
            result = _XRPC_CreateStringElement("ERROR: no such COM-port");
        }

    } else {
        /* No port specified, must already be connected */
        connected = ECMTGW_IsActive(gw);
    }

    if (connected) {
        Buffer buf;
        BUFFER_Init(&buf);
        buf.order = BIG_ENDIAN;
        if (PushURLDataPack(url, browser, &buf)) {
            ECMTGW_SendMessage(gw, PUSH_URL_PLUGIN_UID, BUFFER_Access(&buf),
                BUFFER_Size(&buf), EcmtDirectionToHandset);
        }
        BUFFER_Destroy(&buf);
    }
    else if (!result) {
            result = _XRPC_CreateStringElement("ERROR: device not connected");
    }

    return result;
}

/**
 * Decodes the parameter of "registerListener" and "unregisterListener"
 * methods. If the "mask" XRPC parameter is missing, the mask remains
 * unchanged (the caller fill in the default value)
 */
STATIC Bool GWENG_XRpcIsEcmtGwListener(EcmtGateway* gw,
                                       const XRpcContainer* params,
                                       int* mask)
{
    const XRpcStringElement* protocolParam =
        XRPC_GetStringElementByName(params,
        ECMTGW_LISTENER_PROTOCOL_PARAM);

    if (protocolParam) {
        XRpcString protocol = XRPC_GetString(protocolParam);
        if (protocol) {
            if (!StrCmp(protocol,ECMTGW_LISTENER_PROTOCOL)) {
                const XRpcIntElement* maskParam =
                    XRPC_GetIntElementByName(params,
                    ECMTGW_LISTENER_MASK_PARAM);
                if (maskParam) {
                    *mask = XRPC_GetInt(maskParam);
                }
                return True;
            }
        }
    }
    return False;
}

/**
 * Updates gw->eventMask. The caller holds the mutex.
 */
STATIC void GWENG_XRpcUpdateEventMask(EcmtGateway* gw)
{
    Iterator* itr = HASH_Entries(&gw->listenerMap);
    if (itr) {
        int mask = 0;
        while (ITR_HasNext(itr)) {
            HashEntry* entry = ITR_Next(itr);
            mask |= (int)entry->value;
        }
        ITR_Delete(itr);
        gw->eventMask = mask;
    }
}

/**
 * "registerListener" method handler
 */
STATIC XRpcElement* GWENG_XRpcRegisterListener(void* ctx,
                                               const XRpcContainer* params)
{
    int mask = ECMTGW_LISTEN_DEFAULT_MASK;
    EcmtGateway* gw = ctx;
    if (GWENG_XRpcIsEcmtGwListener(gw, params, &mask)) {
        XRpcSession* session = XRPC_GetCurrentSession(gw->xrpc);
        MUTEX_Lock(&gw->mutex);
        mask |= (int)HASH_Get(&gw->listenerMap, session);
        if (mask) {
            HASH_Put(&gw->listenerMap, session, (HashValue)mask);
            GWENG_XRpcUpdateEventMask(gw);
            TRACE1("GW: EcmtGwListener registered, mask 0x%08x\n",mask);
        }
        MUTEX_Unlock(&gw->mutex);
    }
    return NULL;
}

/**
 * "unregisterListener" method handler
 */
STATIC XRpcElement* GWENG_XRpcUnregisterListener(void* ctx,
                                                 const XRpcContainer* params)
{
    int mask = ECMTGW_LISTEN_MASK_ALL;
    EcmtGateway* gw = ctx;
    if (GWENG_XRpcIsEcmtGwListener(gw, params, &mask)) {
        XRpcSession* session = XRPC_GetCurrentSession(gw->xrpc);
        MUTEX_Lock(&gw->mutex);
        if (HASH_Contains(&gw->listenerMap, session)) {
            int newMask = (((int)HASH_Get(&gw->listenerMap,session)) & ~mask);
            if (newMask) {
                HASH_Put(&gw->listenerMap, session, (HashValue)mask);
                TRACE1("GW: EcmtGwListener modified, mask 0x%08x\n",mask);
            } else {
                HASH_Remove(&gw->listenerMap, session);
                TRACE("GW: EcmtGwListener unregistered\n");
            }
            GWENG_XRpcUpdateEventMask(gw);
        }
        MUTEX_Unlock(&gw->mutex);
    }
    return NULL;
}

/**
 * "session finished" callback
 */
STATIC void GWENG_XRpcSessionClose(void * context, XRpcSession * session)
{
    EcmtGateway* gw = context;
    if (!HASH_IsEmpty(&gw->listenerMap)) {
        MUTEX_Lock(&gw->mutex);
        if (HASH_Remove(&gw->listenerMap, session)) {
            GWENG_XRpcUpdateEventMask(gw);
        }
        MUTEX_Unlock(&gw->mutex);
    }
}

/**
 * Method resolver
 */
STATIC XRpcBool GWENG_XRpcGetMethod (void* ctx,
                                     XRpcString method,
                                     XRpcMethodInfo* info)
{
    info->context = ctx;
    info->synchronous = XRpcTrue;
    if (!StrCmp(method, ECMTGW_LOAD_METHOD)) {
        info->handler = GWENG_XRpcLoadURL;
    } else if (!StrCmp(method, DSCLI_TO_FRONT_METHOD)) {
        info->handler = GWENG_XRpcToFront;
    } else if (!StrCmp(method, ECMTGW_EXIT_METHOD)) {
        info->handler = GWENG_XRpcExit;
    } else if (!StrCmp(method, ECMTGW_REGISTER_LISTENER_METHOD)) {
        info->handler = GWENG_XRpcRegisterListener;
    } else if (!StrCmp(method, ECMTGW_UNREGISTER_LISTENER_METHOD)) {
        info->handler = GWENG_XRpcUnregisterListener;
    } else {
        return XRpcFalse;
    }
    return XRpcTrue;
}

/**
 * Send XRPC notifications to registered listeners
 */
void GWENG_Notify(EcmtGateway* gw, const GwNotify* notify,
                  int uid, const void* data, int len)
{
    if (gw->eventMask & notify->mask) {
        XRpcContainer* c = XRPC_CreateContainer(XRpcTagParams);
        if (c) {
            Iterator* itr;
            XRPC_AddIntField(c, ECMTGW_LISTENER_UID_PARAM, uid);
            XRPC_AddBinaryField(c, ECMTGW_LISTENER_DATA_PARAM, data, len);
            MUTEX_Lock(&gw->mutex);
            itr = HASH_Entries(&gw->listenerMap);
            if (itr) {
                while (ITR_HasNext(itr)) {
                    HashEntry* entry = ITR_Next(itr);
                    XRpcSession* session = entry->key;
                    if (((int)entry->value) & notify->mask) {
                        XRPC_Notify(XRPC_GetClient(session),
                            ECMTGW_LISTENER_PROTOCOL, notify->method,
                            XRPC_ContainerToElement(c));
                    }
                }
                ITR_Delete(itr);
            }
            MUTEX_Unlock(&gw->mutex);
            XRPC_FreeContainer(c);
        }
    }
}

/**
 * Callback invoked by XREG_List. Fills the hashtable with the existing
 * instance ids.
 */
STATIC XRpcBool GWENG_XRpcListCB(const XRegServiceInfo * svc, void* ctx)
{
    HashTable * ids = ctx;
    XRpcString id = XREG_GetServiceProp(svc, DSCLI_INSTANCE_ID_PROP);
    if (id) {
        char* copy = STRING_Dup(id);
        if (copy) {
            if (!HASH_Put(ids, copy, copy)) {
                MEM_Free(copy);
            }
        }
    }
    return True;
}

/**
 * Initializes Gateway's XRPC interfaces.
 */
Bool GWENG_StartXRPC (EcmtGateway* gw)
{
    if (gw->xreg = XREG_CreateRegistry(0, XRegTypeAny)) {
        gw->xrpc = XRPC_CreateServer(NULL);
        if (gw->xrpc) {

            /* Register the protocol */
            XRpcProtocol protocol;
            XRpcString protName = ECMTGW_PROTOCOL;
            memset(&protocol, 0, sizeof (protocol));
            protocol.getmethod = GWENG_XRpcGetMethod;
            protocol.sclose = GWENG_XRpcSessionClose;
            if (XRPC_RegisterProtocol(gw->xrpc, protName,
                "EcmtGW protocol", "Nokia", 1, 0, &protocol, gw)) {

                int iid = 9000;
                char instID[16];
                XRpcPort port;
                HashTable ids;

                /* Register SEI proxy protocol */
                GWENG_MidpRegisterXRpcProtocol(gw);

                /* Prepare hash table to store existing instance ids */
                HASH_Init(&ids, 0, hashCompareStringKey, stringHashProc,
                    hashFreeKeyProc);

                /* Lock the database and get the list of used instance ids */
                XREG_Lock(gw->xreg, protName);
                XREG_List(gw->xreg, protName, GWENG_XRpcListCB, &ids);

                /* Select unused instance id */
                do {
                    sprintf(instID, "%07d", ++iid);
                } while (HASH_Contains(&ids, instID));

                TRACE1("GW: selected instance ID %s\n",instID);

                /* Spawn the server thread */
                port = XRPC_ServerSpawn(gw->xrpc, 0);
                if (port) {
                    char* id;
                    int n = 0;
                    char pid[16];
                    char* logonID = XRPC_GetLogonID ();
                    XRegProperty props[5];

                    sprintf(pid, "%u", getpid());

                    props[n].name = DSCLI_PRODUCT_ID_PROP;
                    props[n++].value = gw->productId;

                    props[n].name = DSCLI_PROCESS_ID_PROP;
                    props[n++].value = pid;

                    props[n].name = DSCLI_INSTANCE_ID_PROP;
                    props[n++].value = instID;

                    if (logonID) {
                        props[n].name = DSCLI_LOGON_ID_PROP;
                        props[n++].value = logonID;
                    }

                    ASSERT(n <= COUNT(props));

                    /* Register with XRPC Registry */
                    id = XREG_RegisterService(gw->xreg,
                        protName, NULL, port, props, n);

                    XRPC_Free(logonID);
                    if (id) {
                        HASH_Destroy(&ids);
                        XREG_Unlock(gw->xreg);
                        MEM_Free(id);
                        return True;
                    }
                }

                /* cleanup */
                HASH_Destroy(&ids);
                XREG_Unlock(gw->xreg);
                XRPC_UnregisterProtocol(gw->xrpc, protName);
            }
            XRPC_FreeServer(gw->xrpc);
            gw->xrpc = NULL;
        }
        XREG_FreeRegistry(gw->xreg);
        gw->xreg = NULL;
    }

    return False;
}



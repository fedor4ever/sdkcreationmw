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


#include "x_reg.h"

/* *************************************************************************
//
//  Client side of XRPC Registry protocol
//
// *************************************************************************/

/**
 * XRPC Registry client context.
 */
struct _XRegClient {
    Lock lock;                      /* synchronization */
    HashTable serviceMap;           /* services registered by this client */
    XRpcSession * session;          /* current client connection to XREG */
    XRpcServer * remoteListener;    /* listens for events from the server */
    XRegEventProc listener;         /* registry listener */
    void * listenerContext;         /* registry listener context */

    /* XRegServiceInfo for XREG_Query */
    XRegServiceInfo serviceInfo;    /* returned by XREG_Query */
    Vector serviceInfoData;         /* contains XRegProperty and strings */
};

/* XRPC Registry Listener protocol descriptor */

STATIC const XRpcProtocol XREG_ListenerProtocol;

/* *************************************************************************
//
//  Client utilities
//
// *************************************************************************/

/**
 * Encodes parameters for the "registerService" XRPC call.
 * Returns the <params> container filled with service information.
 * The caller NEVER deallocates the properties element. It either
 * becomes a part of the <params> tree or deallocated by this 
 * function. The caller is responsible for deallocating the 
 * returned <params> container.
 */
STATIC
XRpcContainer * 
XREG_EncodeRegisterServiceParams(
    XRpcString    id,           /* service id, NULL to generate new one */
    XRpcString    name,         /* service protocol name */
    XRpcString    host,         /* host name or (typically) NULL */
    XRpcPort      port,         /* listening port in host byte order */
    XRpcElement * props)        /* encoded properties */
{
    XRpcContainer * params = XRPC_CreateContainer(XRpcTagParams);
    if (params) {
        XRPC_AddStringField(params,XREG_PARAM_REGISTER_SERVICE_NAME,name);
        XRPC_AddIntField(params,XREG_PARAM_REGISTER_SERVICE_PORT,port&0xffff);
        if (id) {
            XRPC_AddStringField(params,XREG_PARAM_REGISTER_SERVICE_ID,id);
        }
        if (host) {
            XRPC_AddStringField(params,XREG_PARAM_REGISTER_SERVICE_HOST,host);
        }
        if (props) {
            if (!XRPC_SetElementName(props,XREG_PARAM_REGISTER_SERVICE_PROP)||
                !XRPC_TryAddElement(params,props)) {
                XRPC_FreeElement(props);
            }
        }
        return params;
    }
    return NULL;
}

/* *************************************************************************
//
//  Initialization and deinitialization
//
// *************************************************************************/

STATIC
XRegService * 
XREG_ClientCreateService(
    XRpcString      id,         /* service id */
    XRpcString      name,       /* service protocol name */
    XRpcString      host,       /* host name or (typically) NULL */
    XRpcPort        port,       /* listening port in host byte order */
    const XRegProperty * p,     /* service properties */
    int             np)         /* number of properties */
{
    XRegService * service = MEM_New(XRegService);
    if (service) {
        memset(service, 0, sizeof(*service));
        service->port = port;
        service->id = STRING_Dup(id);
        if (PROP_Init(&service->props)) {
            if (service->id) {
                service->name = STRING_Dup(name);
                if (service->name) {
                    if (host) service->host = STRING_Dup(host);
                    if (!host || service->host) {
                        int i;
                        for (i=0; i<np; i++) {
                            Str key = p[i].name;
                            Str value = p[i].value;
                            PROP_Set(&service->props, key, value);
                        }
                        return service;
                    }
                    MEM_Free(service->name);
                }
                MEM_Free(service->id);
            }
            PROP_Destroy(&service->props);
        }
        MEM_Free(service);
    }
    return NULL;
}

/**
 * Deallocates the service
 */
STATIC void XREG_ClientFreeService(XRegService * service)
{
    if (service) {
        MEM_Free(service->id);
        MEM_Free(service->host);
        MEM_Free(service->name);
        PROP_Destroy(&service->props);
        MEM_Free(service);
    }
}

/**
 * Deallocates entry in the id -> XRegService map
 */
STATIC void XREG_ClientServiceMapFree(HashKey key, HashValue value)
{
    XRegService * service = (XRegService *)value;
    ASSERT(key == service->id);
    XREG_ClientFreeService(service);
}

/**
 * Helper for XREG_CreateClient
 */
STATIC Bool XREG_InitClient(XRegClient * client)
{
    memset(client, 0, sizeof(*client));
    if (HASH_Init(&client->serviceMap,0,hashCompareStringKey,
            stringHashProc, XREG_ClientServiceMapFree)) {
        if (LOCK_Init(&client->lock)) {
            /* vector will be reinitialized by XREG_InitRemoteServiceInfo */
            VECTOR_Init(&client->serviceInfoData,0,NULL,NULL);
            client->remoteListener = XRPC_CreateServer(NULL);
            if (client->remoteListener) {
                if (XRPC_RegisterProtocol(client->remoteListener, 
                    XREG_LISTENER_PROTOCOL,
                   "XRPC Registry Listener", "Nokia",
                    XREG_LISTENER_PROTOCOL_VER_MAJOR,
                    XREG_LISTENER_PROTOCOL_VER_MINOR,
                   &XREG_ListenerProtocol, client)) {
                    return True;
                }
                XRPC_FreeServer(client->remoteListener);
            }
            LOCK_Destroy(&client->lock);
        }
        HASH_Destroy(&client->serviceMap);
    }
    return False;
}

/**
 * Allocates client context.
 */
XRegClient * XREG_CreateClient()
{
    XRegClient * client = MEM_New(XRegClient);
    if (client) {
        if (XREG_InitClient(client)) {
            return client;
        }
        MEM_Free(client);
    }
    return NULL;
}

/**
 * Re-reregisters services with the remote registry after reconnnect
 */
STATIC Bool XREG_ReregisterCB(HashKey key, HashValue value, void * ctx)
{
    XRpcContainer * params;
    XRpcElement * props = NULL;
    XRegService * service = (XRegService*)value;
    if (PROP_Size(&service->props)) props = XREG_EncodeProp(&service->props);
    params = XREG_EncodeRegisterServiceParams(service->id,service->name,
        service->host,service->port,props);

    if (params) {
        XRegClient * client = (XRegClient*)ctx;
        XRPC_Notify(XRPC_GetClient(client->session), 
                    XREG_REGISTRY_PROTOCOL, 
                    XREG_METHOD_REGISTER_SERVICE,
                    XRPC_ContainerToElement(params));
        XRPC_FreeContainer(params);
    }

    return True;
}

/**
 * Creates XRPC connection to the registry server. Returns the created
 * session. The socket will be (eventually) closed by the caller so we
 * don't keep it. NOTE: create/connect/disconnect/free operations are 
 * synchronized by an external lock. We can safely make the assumption
 * that only one of those operation is going on at any point of time.
 */
void XREG_Connect(XRegClient * client, XRpcSocket socket)
{
    XRPC_FreeSession(client->session);
    client->session = XRPC_CreateSocketSession(client->remoteListener,socket);
    if (client->session) {

        /* prevent XRPC library from automatically creating session thread */
        XRPC_UseExternalThread(client->session);
        LOCK_ReadLock(&client->lock);

        /* notify the listener */
        if (client->listener) {
            XRegEvent event;
            memset(&event, 0, sizeof(event));
            event.type = XRegEventRebind;
            client->listener(&event, client->listenerContext);
        }

        /* re-register the listener */
        if (client->listener) {
            XRPC_FormatNotify(XRPC_GetClient(client->session), 
                XREG_REGISTRY_PROTOCOL, 
                XREG_METHOD_REGISTER_LISTENER, "%"
                XREG_PARAM_REGISTER_LISTENER_PROTO"!s", 
                XREG_LISTENER_PROTOCOL);
        }

        /* and the services... */
        HASH_Examine(&client->serviceMap, XREG_ReregisterCB, client);
        LOCK_Unlock(&client->lock);
    }
}

/**
 * Disconnects client from the server
 */
void XREG_Disconnect(XRegClient * client)
{
    if (client->session) {
        XRPC_CloseSession(client->session);
    }
}

/**
 * Register a listener
 */
void XREG_ReceiveEvents(XRegClient * client, XRegEventProc cb, void * ctx)
{
    client->listener = cb;
    client->listenerContext = ctx;
}

/**
 * Runs current client session
 */
void XREG_RunClient(XRegClient * client)
{
    if (client->session) {
        XRPC_RunSession(client->session);
    }
}

/**
 * Deallocates the resources used by the client
 */
void XREG_FreeClient(XRegClient * client)
{
    if (client) {

        /* notify the listener */
        if (client->listener) {
            XRegEvent event;
            memset(&event, 0, sizeof(event));
            event.type = XRegEventShutdown;
            client->listener(&event, client->listenerContext);
        }

        XRPC_FreeSession(client->session);
        XRPC_FreeServer(client->remoteListener);
        VECTOR_Destroy(&client->serviceInfoData);
        LOCK_Destroy(&client->lock);
        HASH_Destroy(&client->serviceMap);
        MEM_Free(client);
    }
}

/* *************************************************************************
//
//  Remote calls
//
// *************************************************************************/

/**
 * Calls the remote "registerService" function. Returns the XRPC call
 * context, NULL if something goes terribly wrong. The caller will 
 * deallocate the call.
 */
XRpcCall * 
XREG_CallRegisterService(
    XRegClient *    client,     /* XRPC Registry client */
    XRpcString      svcid,      /* service id, NULL to generate new one */
    XRpcString      name,       /* service protocol name */
    XRpcString      host,       /* host name or (typically) NULL */
    XRpcPort        port,       /* listening port in host byte order */
    const XRegProperty * p,     /* service properties */
    int             np)         /* number of properties */
{
    XRpcCall * call = NULL;
    if (client->session) {
        XRpcContainer * params;
        XRpcElement * props = NULL;
        if (p && np) props = XREG_EncodeProperties(p, np);
        params = XREG_EncodeRegisterServiceParams(svcid,name,host,port,props);
        if (params) {
            call = XRPC_Call(XRPC_GetClient(client->session), 
                             XREG_REGISTRY_PROTOCOL, 
                             XREG_METHOD_REGISTER_SERVICE,
                             XRPC_ContainerToElement(params));
            XRPC_FreeContainer(params);
            if (call) {
                XRPC_Wait(call);
                if (XRPC_CallStatus(call) == XRpcStatusOK) {
                    const XRpcStringElement * str = XRPC_IsStringElement(
                                                    XRPC_IsDataElement(
                                                    XRPC_CallResult(call)));
                    if (str) {
                        Str id = XRPC_GetString(str);
                        ASSERT((!svcid && id)||(svcid && !strcmp(id,svcid))); 
                        if (id) {

                            /* 
                             * store the information about the service 
                             * we have just registered, so that we can 
                             * re-register it after re-connect.
                             */
                            XRegService * svc = XREG_ClientCreateService(
                                id, name, host, port, p, np);

                            if (svc) {
                                LOCK_WriteLock(&client->lock);
                                if (!HASH_Put(&client->serviceMap,svc->id,svc)) {
                                    XREG_ClientFreeService(svc);
                                }
                                LOCK_Unlock(&client->lock);
                            }
                        }
                    }
                }
            }
        }
    }
    return call;
}

/**
 * Calls a remote XRPC registry method that takes one string parameter.
 */
STATIC 
XRpcCall *
XREG_RemoteCallStr(
    XRegClient *    client,     /* XRPC Registry client */
    XRpcString      method,     /* method to call */
    XRpcString      pname,      /* name of the parameter */
    XRpcString      pvalue)     /* value of the parameter */
{
    XRpcCall * call = NULL;
    if (client->session) {
        XRpcContainer * param = NULL;
        if (pvalue) param = XRPC_CreateContainer(XRpcTagParams);
        if (!pvalue || param) {
            if (!param || XRPC_AddStringField(param, pname, pvalue)) {
                call = XRPC_Call(XRPC_GetClient(client->session), 
                    XREG_REGISTRY_PROTOCOL, method,
                    XRPC_ContainerToElement(param));
            }
            XRPC_FreeContainer(param);
        }
    }
    return call;
}

/**
 * Makes "unregisterService" call 
 */
XRpcCall *
XREG_CallUnregisterService(
    XRegClient *    client,     /* XRPC Registry client */
    XRpcString      id)         /* service id assigned by the registry */
{
    return XREG_RemoteCallStr(client, 
        XREG_METHOD_UNREGISTER_SERVICE, 
        XREG_PARAM_UNREGISTER_SERVICE_ID, 
        id); 
}

/**
 * Makes "registerListener" call 
 */
XRpcCall *
XREG_CallRegisterListener(
    XRegClient *    client,     /* XRPC Registry client */
    XRpcString      protocol)   /* listener protocol to register */
{
    return XREG_RemoteCallStr(client, 
        XREG_METHOD_REGISTER_LISTENER, 
        XREG_PARAM_REGISTER_LISTENER_PROTO, 
        protocol); 
}

/**
 * Makes "unregisterListener" call 
 */
XRpcCall *
XREG_CallUnregisterListener(
    XRegClient *    client,     /* XRPC Registry client */
    XRpcString      protocol)   /* listener protocol */
{
    return XREG_RemoteCallStr(client, 
        XREG_METHOD_UNREGISTER_LISTENER, 
        XREG_PARAM_UNREGISTER_LISTENER_PROTO, 
        protocol); 
}

/**
 * Makes "listServices" call 
 */
XRpcCall *
XREG_CallListServices(
    XRegClient *    client,     /* XRPC Registry client */
    XRpcString      name)       /* service name */
{
    return XREG_RemoteCallStr(client, 
        XREG_METHOD_LIST_SERVICES, 
        XREG_PARAM_LIST_SERVICES_NAME, 
        name); 
}

/**
 * Makes "queryService" call 
 */
XRpcCall *
XREG_CallQueryService(
    XRegClient *    client,     /* XRPC Registry client */
    XRpcString      id)         /* service id */
{
    return XREG_RemoteCallStr(client, 
        XREG_METHOD_QUERY_SERVICE, 
        XREG_PARAM_QUERY_SERVICE_ID, 
        id); 
}

/**
 * Processes the result of the queryService call. The caller must make sure
 * that the container is indeed a xrpc.reg.ServiceInfo structure. Returns
 * NULL if parsing fails.
 */
XRegServiceInfo * 
XREG_ParseQueryResult(
    XRegClient *    client,         /* XRPC Registry client */
    const XRpcContainer * svc)      /* XRPC-encoded xrpc.reg.ServiceInfo */
{
    XRegServiceInfo * info = &client->serviceInfo;
    Vector * data = &client->serviceInfoData;
    ASSERT(XRPC_IsStruct((XRpcElement*)svc,XREG_STRUCT_SERVICE_INFO));

    if (LOCK_WriteLock(&client->lock)) {
        /* XREG_InitRemoteServiceInfo will reinitialize the vector */
        VECTOR_Destroy(data);
        if (XREG_InitRemoteServiceInfo(svc, info, data)) {

            /* copy the strings */
            char * s = STRING_Dup(info->id);
            ASSERT(info->id);
            if (s && VECTOR_TryAdd(data, s)) {
                info->id = s;
                ASSERT(info->name);
                s = STRING_Dup(info->name);
                if (s && VECTOR_TryAdd(data, s)) {
                    int i;
                    Bool ok = True;
                    info->name = s;

                    /* copy properties */
                    for (i=0; i<info->nprops; i++) {
                        ASSERT(info->props[i].name);
                        s = STRING_Dup(info->props[i].name);
                        if (s && VECTOR_TryAdd(data, s)) {
                            info->props[i].name = s;
                            ASSERT(info->props[i].value);
                            s = STRING_Dup(info->props[i].value);
                            if (s && VECTOR_TryAdd(data, s)) {
                                info->props[i].value = s;
                                continue;
                            }
                        }
                        ok = False;
                        break;
                    }
                
                    if (ok) {
                        LOCK_Unlock(&client->lock);
                        return &client->serviceInfo;
                    }
                }
            }
            VECTOR_Destroy(data);
            memset(info, 0, sizeof(*info));
        }
        LOCK_Unlock(&client->lock);
    }
    return NULL;
}

/**
 * Makes "setProperty" call 
 */
XRpcCall * 
XREG_CallSetProp(
    XRegClient *    client,     /* XRPC Registry client */
    XRpcString      id,         /* service id */
    XRpcString      name,       /* property name */
    XRpcString      value)      /* property value */
{
    XRpcCall * call = NULL;
    ASSERT(name && value);
    if (client->session && name && value) {
        XRpcContainer * p = XRPC_CreateContainer(XRpcTagParams);
        if (p) {
            if (XRPC_AddStringField(p,XREG_PARAM_SET_PROPERTY_ID,id) &&
                XRPC_AddStringField(p,XREG_PARAM_SET_PROPERTY_NAME,name) &&
                XRPC_AddStringField(p,XREG_PARAM_SET_PROPERTY_VALUE,value)) {
                call = XRPC_Call(XRPC_GetClient(client->session), 
                                 XREG_REGISTRY_PROTOCOL,
                                 XREG_METHOD_SET_PROPERTY,
                                 XRPC_ContainerToElement(p));
                if (call) {
                    if (LOCK_WriteLock(&client->lock)) {
                        XRegService * service = (XRegService*)
                            HASH_Get(&client->serviceMap, id);
                        if (service) {
                            PROP_Set(&service->props, name, value);
                        }
                        LOCK_Unlock(&client->lock);
                    }
                }
            }
            XRPC_FreeContainer(p);
        }
    }
    return call;
}

/**
 * Makes "clearProperty" call 
 */
XRpcCall * 
XREG_CallClearProp(
    XRegClient *    client,     /* XRPC Registry client */
    XRpcString      id,         /* service id */
    XRpcString      name)       /* property name */
{
    XRpcCall * call = NULL;
    if (client->session) {
        XRpcContainer * p = XRPC_CreateContainer(XRpcTagParams);
        if (p) {
            if (XRPC_AddStringField(p,XREG_PARAM_CLEAR_PROPERTY_ID,id) &&
                XRPC_AddStringField(p,XREG_PARAM_CLEAR_PROPERTY_NAME,name)) {
                call = XRPC_Call(XRPC_GetClient(client->session), 
                                 XREG_REGISTRY_PROTOCOL,
                                 XREG_METHOD_CLEAR_PROPERTY,
                                 XRPC_ContainerToElement(p));
                if (call) {
                    if (LOCK_WriteLock(&client->lock)) {
                        XRegService * service = (XRegService*)
                            HASH_Get(&client->serviceMap, id);
                        if (service) {
                            PROP_Remove(&service->props, name);
                        }
                        LOCK_Unlock(&client->lock);
                    }
                }
            }
            XRPC_FreeContainer(p);
        }
    }
    return call;
}

/**
 * Makes "lock" call 
 */
XRpcCall * XREG_CallLock(XRegClient * client, XRpcString name)
{
    return XREG_RemoteCallStr(client, 
        XREG_METHOD_LOCK, 
        XREG_PARAM_LOCK_NAME, 
        name); 
}

/**
 * Makes "unlock" call 
 */
XRpcCall * XREG_CallUnlock(XRegClient * client)
{
    return XRPC_Call(XRPC_GetClient(client->session), 
        XREG_REGISTRY_PROTOCOL, XREG_METHOD_UNLOCK, NULL);
}

/* *************************************************************************
//
//  void serviceRegistered(xrpc.reg.ServiceInfo service)
//
//  Invoked when a new service is registered
//
// *************************************************************************/
STATIC XRpcElement * XRPC_API 
XREG_ListenerServiceRegistered(void * ctx, const XRpcContainer * params)
{
    const XRpcElement * elem;
    elem = XRPC_GetElementByName(params,XREG_LISTENER_PARAM_SERVICE_REG);
    if (elem) {
        const XRpcContainer * info;
        info = XRPC_IsStruct(elem,XREG_STRUCT_SERVICE_INFO);
        if (info) {
            XRegClient * client = (XRegClient*)ctx;
            LOCK_ReadLock(&client->lock);
            if (client->listener) {
                Vector v;
                XRegEvent event;
                memset(&event, 0, sizeof(event));
                event.type = XRegEventRegisterService;
                if (XREG_InitRemoteServiceInfo(info,
                    &event.registerService.service,&v)) {
                    client->listener(&event, client->listenerContext);
                    VECTOR_Destroy(&v);
                }
            }
            LOCK_Unlock(&client->lock);
        }
    }
    return NULL;
}

/* *************************************************************************
//
//  void serviceUnregistered(xrpc.reg.ServiceInfo service)
//
//  Invoked when existing service gets unregistered
//
// *************************************************************************/
STATIC XRpcElement * XRPC_API 
XREG_ListenerServiceUnregistered(void * ctx, const XRpcContainer * params)
{
    const XRpcElement * elem;
    elem = XRPC_GetElementByName(params,XREG_LISTENER_PARAM_SERVICE_UNREG);
    if (elem) {
        const XRpcContainer * info;
        info = XRPC_IsStruct(elem,XREG_STRUCT_SERVICE_INFO);
        if (info) {
            XRegClient * client = (XRegClient*)ctx;
            LOCK_ReadLock(&client->lock);
            if (client->listener) {
                Vector v;
                XRegEvent event;
                memset(&event, 0, sizeof(event));
                event.type = XRegEventUnregisterService;
                if (XREG_InitRemoteServiceInfo(info,
                    &event.unregisterService.service,&v)) {
                    client->listener(&event, client->listenerContext);
                    VECTOR_Destroy(&v);
                }
            }
            LOCK_Unlock(&client->lock);
        }
    }
    return NULL;
}

/* *************************************************************************
//
//  void propertyChanged(string id, string name, string value)
//
//  Invoked when property of an existing service gets changed
//  Currently, this notification is only being sent to the session
//  that has registered the service in question
//
// *************************************************************************/

STATIC XRpcElement * XRPC_API 
XREG_ListenerPropertyChanged(void * ctx, const XRpcContainer * params)
{
    const XRpcStringElement * idElem = XRPC_GetStringElementByName(params,
        XREG_LISTENER_PARAM_PROP_CHANGE_ID);
    const XRpcStringElement * nameElem = XRPC_GetStringElementByName(params,
        XREG_LISTENER_PARAM_PROP_CHANGE_NAME);
    const XRpcStringElement * valueElem = XRPC_GetStringElementByName(params,
        XREG_LISTENER_PARAM_PROP_CHANGE_VALUE);
    if (idElem && nameElem && valueElem) {
        XRpcString id = XRPC_GetString(idElem);
        XRpcString name = XRPC_GetString(nameElem);
        XRpcString value = XRPC_GetString(valueElem);
        if (id && name && value) {
            XRegClient * client = (XRegClient*)ctx;
            if (LOCK_WriteLock(&client->lock)) {
                XRegService * service = (XRegService*)
                    HASH_Get(&client->serviceMap, id);
                if (service) {
                    PROP_Set(&service->props, name, value);
                }
                LOCK_Unlock(&client->lock);
            }
        }
    }
    return NULL;
}

/* *************************************************************************
//
//  void propertyRemoved(string id, string name)
//
//  Invoked when property of an existing service gets removed (cleared)
//  Currently, this notification is only being sent to the session
//  that has registered the service in question
//
// *************************************************************************/

STATIC XRpcElement * XRPC_API 
XREG_ListenerPropertyRemoved(void * ctx, const XRpcContainer * params)
{
    const XRpcStringElement * idElem = XRPC_GetStringElementByName(params,
        XREG_LISTENER_PARAM_PROP_CHANGE_ID);
    const XRpcStringElement * nameElem = XRPC_GetStringElementByName(params,
        XREG_LISTENER_PARAM_PROP_CHANGE_NAME);
    if (idElem && nameElem) {
        XRpcString id = XRPC_GetString(idElem);
        XRpcString name = XRPC_GetString(nameElem);
        if (id && name) {
            XRegClient * client = (XRegClient*)ctx;
            if (LOCK_WriteLock(&client->lock)) {
                XRegService * service = (XRegService*)
                    HASH_Get(&client->serviceMap, id);
                if (service) {
                    PROP_Remove(&service->props, name);
                }
                LOCK_Unlock(&client->lock);
            }
        }
    }
    return NULL;
}

/**
 * XRPC Registry Listener protocol handler 
 */
STATIC
XRpcBool 
XRPC_API
XREG_ListenerProtocolCB(
    void *                  context,    /* pointer to XRpcRegistry */
    XRpcString              name,       /* method name */
    XRpcMethodInfo *        info)       /* receives handler and context */
{
    if (StrCmp(XREG_LISTENER_METHOD_SERVICE_REG,name) == 0) {
        info->handler = XREG_ListenerServiceRegistered;
    } else if (StrCmp(XREG_LISTENER_METHOD_SERVICE_UNREG,name) == 0) {
        info->handler = XREG_ListenerServiceUnregistered;
    } else if (StrCmp(XREG_LISTENER_METHOD_PROP_CHANGE,name) == 0) {
        info->handler = XREG_ListenerPropertyChanged;
    } else if (StrCmp(XREG_LISTENER_METHOD_PROP_REMOVE,name) == 0) {
        info->handler = XREG_ListenerPropertyRemoved;
    } else {
        return XRpcFalse;
    }
    info->context = context;
    info->synchronous = True;
    return True;
}

/* XRPC Registry Listener protocol descriptor */

STATIC const XRpcProtocol XREG_ListenerProtocol = {
    XREG_ListenerProtocolCB,            /* getmethod */
    NULL,                               /* cancel */
    NULL,                               /* sopen */
    NULL                                /* sclose */
};

/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

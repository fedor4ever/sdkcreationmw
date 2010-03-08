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

#define XREG_MAX_ATTEMPTS 10

/* 
 * XRPC registry context.
 */
struct _XRpcRegistry {
    XRpcPort port;                  /* XRPC Registry port */
    char * host;                    /* host to connect to (client only) */
    IPaddr addr;                    /* host address (client only) */
    XRegType type;                  /* type of this object */
    Lock bindLock;                  /* synchronization object */
    ThrID clientThread;             /* reconnection thread */
    XRpcSocket clientSocket;        /* client's socket */
    Event configureEvent;           /* set by XREG_ClientThread */
    int connectionIndex;            /* connection index */
    XRegClient * client;            /* client */
    XRegServer * server;            /* server */
    int clientRefs;                 /* additional references to the client */
    int flags;                      /* flags, see below */

#define XREG_DEAD           0x0001  /* registry is being deallocated */
};

/*
 * Remote call parameters
 */

typedef XRpcCall * (*XRegInvokeProc)(XRegClient * client, void * context);
typedef void (*XRegResultProc)(XRpcCall * call, void * context);
typedef struct _XRegRegisterServiceParams {
    XRpcString name;                /* service protocol name */
    XRpcString host;                /* host name or (typically) NULL */
    XRpcPort   port;                /* listening port in host byte order */
    const XRegProperty * p;         /* service properties */
    int np;                         /* number of properties */
    char * id;                      /* the ID assigned by the server */
} XRegRegisterServiceParams;

typedef struct _XRegBoolResult {
    XRpcBool ok;                    /* the return value from the API call */
} XRegBoolResult;

typedef struct _XRegListParams {
    XRpcString name;                /* service to find or NULL for any */
    XRegListProc cb;                /* the callback function */
    void * context;                 /* passed to the callback */
    XRpcBool ok;                    /* the return value from the API call */
} XRegListParams;

typedef struct _XRegQueryParams {
    XRegClient * client;            /* client */
    XRpcString id;                  /* id of the service to find */
    XRegServiceInfo * info;         /* the result of the call */
} XRegQueryParams;

typedef struct _XRegLockParams {
    XRegBoolResult result;          /* the result of the call */
    XRpcString name;                /* lock name */
} XRegLockParams;

typedef struct _XRegSetPropParams {
    XRegBoolResult result;          /* the result of the call */
    XRpcString id;                  /* service id */
    XRpcString name;                /* property name */
    XRpcString value;               /* property value */
} XRegSetPropParams;

typedef struct _XRegClearPropParams {
    XRegBoolResult result;          /* the result of the call */
    XRpcString name;                /* property name */
    XRpcString id;                  /* service id */
} XRegClearPropParams;

/**
 * Parameters passed to XRPC client thread.
 */
typedef struct _XRegClientParams {
    Event startEvent;               /* set when thread has started */
    XRpcRegistry * reg;             /* the registry that owns the thread */
} XRegClientParams;

/* *************************************************************************
//
//  Reconnection logic
//
// *************************************************************************/

/**
 * Client thread. Reconnects to the server when connection breaks.
 */
STATIC Bool XREG_Configure(XRpcRegistry * reg)
{
    int i;
    LOCK_WriteLock(&reg->bindLock);
    for (i=0; i<XREG_MAX_ATTEMPTS; i++) {
        
        /* try to register a server */
        if (reg->type == XRegTypeServer || reg->type == XRegTypeAny) {
            if (!reg->server) {
                reg->server = XREG_CreateServer(reg->port);
            }
            if (reg->server && reg->type == XRegTypeServer) {

                /* server-only configuration is done */
                EVENT_Set(&reg->configureEvent);
                LOCK_Unlock(&reg->bindLock);
                return True;
            }
        }

        /* next, connect to the server */
        if (reg->type != XRegTypeServer) {
            
            /* lazily create client context */
            if (!reg->client) {
                reg->client = XREG_CreateClient();
                if (reg->client) {
                    /* reg->client does not count as a reference */
                    reg->clientRefs = 0;
                } else {
                    break;
                }
            }
            
            /* establish TCP/IP connection */
            if (SOCKET_GetTcp(0, &reg->clientSocket)) {
                if (!reg->addr) {
                    /* by default will connect to the local host */
                    reg->addr = HOST_IPADDR(127,0,0,1);
                    if (reg->host) {
                        INET_ResolveAddr(reg->host,&reg->addr);
                    }
                }
                if (SOCKET_Connect(reg->clientSocket,reg->addr,reg->port)) {
                    TRACE("XREG: connected to the registry\n");
                    reg->connectionIndex++;
                    if (!reg->connectionIndex) reg->connectionIndex++;
                    XREG_Connect(reg->client, reg->clientSocket);
                    EVENT_Set(&reg->configureEvent);
                    LOCK_Unlock(&reg->bindLock);
                    return True;
                }
                SOCKET_Close(reg->clientSocket);
                reg->clientSocket = INVALID_SOCKET;
                if (reg->type == XRegTypeClient) {

                    /* client-only configuration fails quickly */
                    break;
                }
            }
        }
    }
    EVENT_Set(&reg->configureEvent);
    LOCK_Unlock(&reg->bindLock);
    return False;
}

/**
 * Addds reference to the client
 */
STATIC XRegClient * XREG_RefClient(XRpcRegistry * reg)
{
    XRegClient * client = NULL;
    LOCK_WriteLock(&reg->bindLock);
    if (reg->client) {
        reg->clientRefs++;
        client = reg->client;
    }
    LOCK_Unlock(&reg->bindLock);
    return client;
}

/**
 * Dereferences the client
 */
STATIC void XREG_DerefClient(XRpcRegistry * reg)
{
    XRegClient * client = NULL;
    LOCK_WriteLock(&reg->bindLock);
    ASSERT(reg->clientRefs > 0);
    ASSERT(reg->client);
    if (reg->client) {
        reg->clientRefs--;
        ASSERT(reg->clientRefs >= 0);
        if (reg->clientRefs < 1) {
            client = reg->client;
            reg->client = NULL;
            reg->clientRefs = -1;
        }
    }
    LOCK_Unlock(&reg->bindLock);
    XREG_FreeClient(client);
}

/**
 * XRPC registry client thread
 */
STATIC void XREG_ClientThread(void * arg)
{
    XRegClient * client = NULL;
    Bool firstLoop = True;
    XRegClientParams * params = (XRegClientParams*)arg;
    XRpcRegistry * reg = params->reg;
    TRACE("XREG: client thread started\n");

    LOCK_WriteLock(&reg->bindLock);

    /* signal XREG_CreateRegistry that we have started */
    EVENT_Set(&params->startEvent);
    while (!(reg->flags & XREG_DEAD)) {
        if (firstLoop) {
            firstLoop = False;
        } else {
            if (!XREG_Configure(reg)) {
                /* this is really bad if a server fails */
                ASSERT(!reg->server);
                break;
            }
        }

        if (reg->client) {
            ASSERT(reg->client == client || !client);
            if (!client) {
                client = reg->client;
                reg->clientRefs++;
            }
            LOCK_Unlock(&reg->bindLock);
            TRACE("XREG: starting client session\n");
            XREG_RunClient(client);
            TRACE("XREG: disconnected from the registry\n");                            
            LOCK_WriteLock(&reg->bindLock);
        } else {
            /* we switched to the server mode, terminate the client thread */
            ASSERT(reg->server);
            break;
        }
    }
    if (reg->clientSocket != INVALID_SOCKET) {
        shutdown(reg->clientSocket,SHUT_RDWR);
        SOCKET_Close(reg->clientSocket);
        reg->clientSocket = INVALID_SOCKET;
    }
    if (reg->client) {
        if (client) {
            ASSERT(reg->client == client);
            reg->clientRefs--;
        }
        ASSERT(reg->clientRefs >= 0);
        if (reg->clientRefs > 0) {
            /* somebody is holding reference to XRegClient */
            client = NULL;
        } else {
            client = reg->client;
            reg->client = NULL;
            reg->clientRefs = -1;
        }
    } else {
        ASSERT(!client);
        client = NULL;
    }
    LOCK_Unlock(&reg->bindLock);    
    XREG_FreeClient(client);
    TRACE("XRPC registry thread exiting\n");
}

/**
 * Makes a remote call, handles connection failures
 */
STATIC 
Bool 
XREG_Invoke(
    XRpcRegistry * reg,             /* XRPC Registry context */
    XRegInvokeProc callProc,        /* submits the call */
    XRegResultProc resultProc,      /* interprets the result (optional) */
    void * context)                 /* common context for both */
{
    Bool ok = False;
    int i;
    int lastConnectionIndex = 0;
    for (i=0; !ok && i<XREG_MAX_ATTEMPTS; i++) {
        XRegClient * client;

        /* wait for client thread to reconnect */
        if (lastConnectionIndex) {
            Bool wait = False;
            LOCK_WriteLock(&reg->bindLock);
            if (reg->connectionIndex != lastConnectionIndex) {
                EVENT_Reset(&reg->configureEvent);
                wait = True;
            }
            LOCK_Unlock(&reg->bindLock);
            if (wait) EVENT_TimeWait(&reg->configureEvent, 500);
        }

        /* submit the call */
        client = XREG_RefClient(reg);
        if (client) {
            XRpcCall * call;

            LOCK_ReadLock(&reg->bindLock);
            call = callProc(client, context);
            lastConnectionIndex = reg->connectionIndex;
            LOCK_Unlock(&reg->bindLock);

            /* lock has been released, wait for call to complete */
            if (call) {
                XRPC_Wait(call);
                ok = (XRPC_CallStatus(call) == XRpcStatusOK);
                if (ok && resultProc) resultProc(call, context);
                XRPC_FreeCall(call);
            }

            XREG_DerefClient(reg);
        }
    }
    return ok;
}

/* *************************************************************************
//
//  Public API
//
// *************************************************************************/

/**
 * Allocates an instance of XRpcRegistry and initializes (almost) all 
 * its fields, getting it into the state when it can be deallocated by
 * XREG_FreeRegistry. The only field it does NOT initialize is the 
 * 'thread' field, but XREG_FreeRegistry can handle it. Basically, this
 * function is doing a chunk of work for XREG_CreateRegistry, otherwise
 * XREG_CreateRegistry gets too big.
 */
STATIC
XRpcRegistry *
XREG_AllocRegistry(
    XRpcString      host,       /* XRPC Registry host */
    XRpcPort        port,       /* XRPC Registry port */
    XRegType        type)       /* type of object */
{
    XRpcRegistry * reg = MEM_New(XRpcRegistry);
    if (reg) {
        memset(reg, 0, sizeof(*reg));
        reg->clientSocket = INVALID_SOCKET;
        reg->port = (port ? (port & 0xffff) : XREG_DEFAULT_PORT);
        reg->type = type;
        if (host) reg->host = STRING_Dup(host);
        if (!host || reg->host) {
            if (LOCK_Init(&reg->bindLock)) {
                if (EVENT_Init(&reg->configureEvent)) {
                    return reg;
                }
                LOCK_Destroy(&reg->bindLock);
            }
            MEM_Free(reg->host);
        }
        MEM_Free(reg);
    }
    return NULL;
}

/**
 * Internal functions that creates an instance of XRpcRegistry listening 
 * on or connected to the specified port. Returns pointer to the created
 * instance of XRpcRegistry or NULL if something fails, for example if you
 * specify XRegTypeServer object type and a port that is currently in use.
 */
STATIC
XRpcRegistry *
XREG_API
XREG_CreateRegistryContext(
    XRpcString      host,       /* XRPC Registry host */
    XRpcPort        port,       /* XRPC Registry port */
    XRegType        type)       /* type of object */
{
    XRpcRegistry * reg = XREG_AllocRegistry(host, port, type);
    ASSERT(!host || type == XRegTypeClient);
    if (reg) {
        Bool locked = LOCK_WriteLock(&reg->bindLock);
        XRegClientParams param;
        param.reg = reg;
        if (EVENT_Init(&param.startEvent)) {
            if (THREAD_Create(&reg->clientThread,XREG_ClientThread,&param)) {
                if (XREG_Configure(reg)) {
                    LOCK_Unlock(&reg->bindLock);
                    EVENT_Wait(&param.startEvent);

                    /* make sure registry thread is out of EVENT_Set
                     * before destroying the event */
                    LOCK_WriteLock(&reg->bindLock);
                    LOCK_Unlock(&reg->bindLock);
                    EVENT_Destroy(&param.startEvent);
                    return reg;
                }

                /* must wait for XREG_Thread to terminate
                 * before we may deallocate param.startEvent */
                reg->flags |= XREG_DEAD;
                LOCK_Unlock(&reg->bindLock);
                THREAD_Join(reg->clientThread);
                reg->clientThread = (ThrID)0;
                locked = False;
            }
            EVENT_Destroy(&param.startEvent);
        }
        if (locked) LOCK_Unlock(&reg->bindLock);
        XREG_FreeRegistry(reg);
    }
    return NULL;
}

/**
 * Creates an instance of XRpcRegistry listening on or connected to the
 * specified port. Returns pointer to the created instance of XRpcRegistry
 * or NULL if something fails, for example if you specify XRegTypeServer
 * object type and a port that is currently in use.
 */
XRpcRegistry *
XREG_API
XREG_CreateRegistry(
    XRpcPort        port,       /* XRPC Registry port */
    XRegType        type)       /* type of object */
{
    return XREG_CreateRegistryContext(NULL, port, type);
}

/**
 * Creates an instance of XRpcRegistry client connected to the registry
 * running on the specified host and listening on the specified port.
 * NULL host is interpeted as localhost. Returns pointer to the created
 * instance of XRpcRegistry or NULL if something failed, for example if
 * could not connect to the registry server.
 */
XRpcRegistry *
XREG_API
XREG_ConnectRegistry(
    XRpcString      host,       /* XRPC Registry host */
    XRpcPort        port)       /* XRPC Registry port */
{
    return XREG_CreateRegistryContext(host, port, XRegTypeClient);
}

/*                                  
 * Deallocates XRpcRegistry
 */
void
XREG_API
XREG_FreeRegistry(
    XRpcRegistry *  reg)        /* instance of XRpcRegistry */
{
    if (reg) {
        XRegClient * client;
        XRegServer * server;

        /* shutdown the registry */
        LOCK_WriteLock(&reg->bindLock);
        if (reg->clientSocket != INVALID_SOCKET) {
            shutdown(reg->clientSocket,SHUT_RDWR);
            SOCKET_Close(reg->clientSocket);
            reg->clientSocket = INVALID_SOCKET;
        }
        reg->flags |= XREG_DEAD;
        server = reg->server;
        reg->server = NULL;
        client = reg->client;
        if (client) reg->clientRefs++;
        LOCK_Unlock(&reg->bindLock);

        /* these may block, do it outside of the critical section */
        if (client) XREG_Disconnect(client);
        if (server) XREG_FreeServer(server);

        /* wait until the client thread terminates */
        if (reg->clientThread) THREAD_Join(reg->clientThread);

        /* deallocate the rest of stuff */
        if (client) {
            ASSERT(reg->client == client);
            ASSERT(reg->clientRefs == 1);
            XREG_FreeClient(client);
        }
        MEM_Free(reg->host);
        EVENT_Destroy(&reg->configureEvent);
        LOCK_Destroy(&reg->bindLock);
        MEM_Free(reg);
    }
}

/*
 * Returns True if XRpcRegistry is currently being a server. 
 * If XRpcRegistry has XRegTypeClient, this call will always
 * return False, for XRegTypeServer is always returns True and
 * in case of XRegTypeAny the result depends on whether the
 * registry is running in client or server mode.
 */
XRpcBool XREG_API XREG_IsServer(XRpcRegistry *  reg)
{
    return (reg->server != NULL);
}

/*
 * Returns True if XRpcRegistry is currently being a client. 
 * If XRpcRegistry has XRegTypeServer, this call will always
 * return False, for XRegTypeClient is always returns True if
 * connected and in case of XRegTypeAny the result depends on 
 * whether the registry is running in client or server mode.
 */
XRpcBool XREG_API XREG_IsClient(XRpcRegistry *  reg)
{
    return (reg->server == NULL);
}

/**
 * Submits RegisterService call
 */
STATIC XRpcCall * XREG_RegisterServiceCall(XRegClient * client, void * ctx)
{
    XRegRegisterServiceParams * param = (XRegRegisterServiceParams*)ctx;
    return XREG_CallRegisterService(client, NULL, param->name, 
        param->host, param->port, param->p, param->np);
}

/**
 * Interpretes the result of the RegisterService call
 */
STATIC void XREG_RegisterServiceResult(XRpcCall * call, void * ctx)
{
    XRegRegisterServiceParams * param = (XRegRegisterServiceParams*)ctx;
    const XRpcStringElement * str = XRPC_IsStringElement(
                                    XRPC_IsDataElement(
                                    XRPC_CallResult(call)));
    if (str) param->id = STRING_Dup(XRPC_GetString(str));
}

/*
 * Registers an XRPC service with the registry. Returns the id
 * assigned by XRPC registry to the service, NULL in case of failure.
 * The id can be used to unregister the service.
 */
char *
XREG_API
XREG_RegisterService(
    XRpcRegistry *  reg,        /* instance of XRpcRegistry */
    XRpcString      name,       /* service protocol name */
    XRpcString      host,       /* host name or (typically) NULL */
    XRpcPort        port,       /* listening port in host byte order */
    const XRegProperty * p,     /* service properties */
    int             np)         /* number of properties */
{
    XRegRegisterServiceParams par;
    memset(&par, 0, sizeof(par));
    par.name = name;
    par.host = host;
    par.port = port;
    par.p    = p;
    par.np   = np;
    XREG_Invoke(reg,XREG_RegisterServiceCall,XREG_RegisterServiceResult,&par);
    return par.id;
}

/**
 * Submits UnregisterService call
 */
STATIC XRpcCall * XREG_UnregisterServiceCall(XRegClient * client, void * ctx)
{
    return XREG_CallUnregisterService(client, (XRpcString)ctx);
}

/*
 * Unregisters the XRPC service previously registered with 
 * XREG_RegisterService call.
 */
void
XREG_API
XREG_UnregisterService(
    XRpcRegistry *  reg,        /* instance of XRpcRegistry */
    XRpcString      id)         /* service id assigned by the registry */
{
    XREG_Invoke(reg,XREG_UnregisterServiceCall,NULL,(void*)id);
}

/**
 * Submits RegisterListener call
 */
STATIC XRpcCall * XREG_RegisterListenerCall(XRegClient * client, void * ctx)
{
    return XREG_CallRegisterListener(client, (XRpcString)ctx);
}

/**
 * Submits UnregisterListener call
 */
STATIC XRpcCall * XREG_UnregisterListenerCall(XRegClient * client, void * ctx)
{
    return XREG_CallUnregisterListener(client, (XRpcString)ctx);
}

/*
 * Registers an XRPC listener callback. The new callback replaces the 
 * previous one (no more than one callback can be registered at a time). 
 * NULL callback disables the notification
 */
void
XREG_API
XREG_SetListener(
    XRpcRegistry *  reg,        /* instance of XRpcRegistry */
    XRegEventProc   cb,         /* the callback receiving the events */
    void * context)             /* passed to the callback */
{
    if (reg->client) {
        XRegInvokeProc callProc = XREG_RegisterListenerCall;
        if (!cb) callProc = XREG_UnregisterListenerCall;
        XREG_ReceiveEvents(reg->client, cb, context);
        XREG_Invoke(reg, callProc, NULL, (void*)XREG_LISTENER_PROTOCOL);
    }
}

/**
 * Submits ListServices call
 */
STATIC XRpcCall * XREG_ListServicesCall(XRegClient * client, void * ctx)
{
    XRegListParams * params = (XRegListParams*)ctx;
    return XREG_CallListServices(client, params->name);
}

/**
 * Interpretes the result of the ListServices call
 */
STATIC void XREG_ListServicesResult(XRpcCall * call, void * ctx)
{
    XRegListParams * params = (XRegListParams*)ctx;
    const XRpcContainer * result = XRPC_IsArray(XRPC_CallResult(call));
    params->ok = XRpcTrue;
    if (result) {
        int i, n = XRPC_ElementCount(result);
        for (i=0; i<n && params->ok; i++) {
            const XRpcElement * e = XRPC_GetElementAt(result,i);
            const XRpcContainer*s = XRPC_IsStruct(e,XREG_STRUCT_SERVICE_INFO);
            if (s) {
                Vector v;
                XRegServiceInfo info;
                if (XREG_InitRemoteServiceInfo(s,&info,&v)) {
                    params->ok = params->cb(&info, params->context);
                    VECTOR_Destroy(&v);
                }
            }
        }
    }
}

/*
 * Lists all services of given type. If the service parameter is NULL,
 * then all services will be reported. The callback function is invoked
 * for each service  registered with the XRpcRegistry. The return value
 * if the last value returned by the callback, or True if there's no
 * registered services.
 */
XRpcBool
XREG_API
XREG_List(
    XRpcRegistry *  reg,        /* instance of XRpcRegistry */
    XRpcString      name,       /* service to find or NULL for any */
    XRegListProc    cb,         /* the callback function */
    void *          context)    /* passed to the callback */
{
    XRegListParams param;
    memset(&param, 0, sizeof(param));
    param.name = name;
    param.cb = cb;
    param.context = context;
    XREG_Invoke(reg,XREG_ListServicesCall,XREG_ListServicesResult,&param);
    return param.ok;
}

/**
 * Submits QueryService call
 */
STATIC XRpcCall * XREG_QueryServiceCall(XRegClient * client, void * ctx)
{
    XRegQueryParams * params = (XRegQueryParams*)ctx;
    ASSERT(params->client == client);
    return XREG_CallQueryService(client, params->id);
}

/**
 * Interpretes the result of the QueryService call
 */
STATIC void XREG_QueryServiceResult(XRpcCall * call, void * ctx)
{
    XRegQueryParams * params = (XRegQueryParams*)ctx;
    const XRpcContainer * result = XRPC_IsStruct(XRPC_CallResult(call),
        XREG_STRUCT_SERVICE_INFO);
    if (result) {
        params->info = XREG_ParseQueryResult(params->client, result);
    }
}

/*
 * Returns information about the service identified by its id.
 * The XRegServiceInfo pointer is valid until the next XREG_Query 
 * or (of course) XREG_FreeRegistry call. Do NOT deallocate the
 * returned pointer.
 */
const XRegServiceInfo * 
XREG_API
XREG_Query(
    XRpcRegistry *  reg,        /* instance of XRpcRegistry */
    XRpcString      id)         /* service to find */
{
    XRegQueryParams param;
    XRegClient * client = XREG_RefClient(reg);
    memset(&param, 0, sizeof(param));
    param.client = XREG_RefClient(reg);
    param.id = id;
    if (client) {
        XREG_Invoke(reg,XREG_QueryServiceCall,XREG_QueryServiceResult,&param);
        XREG_DerefClient(reg);
    }
    return param.info;
}

/**
 * Interpretes the result of a call that returns a boolean value
 */
STATIC void XREG_BoolResult(XRpcCall * call, void * ctx)
{
    XRegBoolResult * result = (XRegBoolResult*)ctx;
    const XRpcBoolElement * elem = XRPC_IsBoolElement(
                                   XRPC_IsDataElement(
                                   XRPC_CallResult(call)));
    if (elem) {
        result->ok = XRPC_GetBool(elem);
    }
}

/**
 * Submits SetProperty call
 */
STATIC XRpcCall * XREG_SetPropCall(XRegClient * client, void * ctx)
{
    XRegSetPropParams * params = (XRegSetPropParams*)ctx;
    return XREG_CallSetProp(client, params->id, params->name, params->value);
}

/*
 * Sets or updates a service property.
 * Returns True if the property has been created or updated, False
 * if the client is not allowed to set this property, or if a
 * communication error has occured.
 */
XRpcBool 
XREG_API
XREG_SetProperty(
    XRpcRegistry *  reg,        /* instance of XRpcRegistry */
    XRpcString      id,         /* service id */
    XRpcString      name,       /* the name of the property */
    XRpcString      value)      /* the value of the property */
{
    XRegSetPropParams param;
    memset(&param, 0, sizeof(param));
    param.id = id;
    param.name = name;
    param.value = value;
    XREG_Invoke(reg,XREG_SetPropCall,XREG_BoolResult,&param);
    return param.result.ok;
}

/**
 * Submits ClearProperty call
 */
STATIC XRpcCall * XREG_ClearPropCall(XRegClient * client, void * ctx)
{
    XRegClearPropParams * params = (XRegClearPropParams*)ctx;
    return XREG_CallClearProp(client, params->id, params->name);
}

/*
 * Clears (removes) a service property.
 * Returns True if the property has been removed, False if the property
 * did not exist or the client is not allowed to set this property, or if
 * a communication error has occured.
 */
XRpcBool 
XREG_API
XREG_ClearProperty(
    XRpcRegistry *  reg,        /* instance of XRpcRegistry */
    XRpcString      id,         /* service id */
    XRpcString      name)       /* the name of the property */
{
    XRegClearPropParams param;
    memset(&param, 0, sizeof(param));
    param.id = id;
    param.name = name;
    XREG_Invoke(reg,XREG_ClearPropCall,XREG_BoolResult,&param);
    return param.result.ok;
}

/**
 * Submits Lock call
 */
STATIC XRpcCall * XREG_LockCall(XRegClient * client, void * ctx)
{
    XRegLockParams * params = (XRegLockParams*)ctx;
    return XREG_CallLock(client, params->name);
}

/*
 * Acquires named mutex. If the name parameter is NULL or an empty
 * string, acquires a global XRPC registry mutex. It's guaranteed 
 * that no services will be registered or unregistered while client
 * is holding the global mutex. A client is not allowed to hold 
 * more than one mutex.
 */
XRpcBool XREG_API XREG_Lock(XRpcRegistry *  reg, XRpcString name)
{
    XRegLockParams param;
    memset(&param, 0, sizeof(param));
    param.name = name;
    XREG_Invoke(reg,XREG_LockCall,XREG_BoolResult,&param);
    return param.result.ok;
}

/**
 * Submits Unlock call
 */
STATIC XRpcCall * XREG_UnlockCall(XRegClient * client, void * ctx)
{
    return XREG_CallUnlock(client);
}

/*
 * Releases the mutex held by the client.
 */
void XREG_API XREG_Unlock(XRpcRegistry *  reg)
{
    XREG_Invoke(reg,XREG_UnlockCall,NULL,NULL);
}

/*
 * Deallocates memory allocated by XREG library. Ignores NULL pointers.
 */
void XREG_API XREG_Free(void * ptr)
{
    MEM_Free(ptr);
}

/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

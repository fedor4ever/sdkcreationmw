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
//  Server side of XRPC Registry protocol
//
// *************************************************************************/

typedef uint64_t XRegSessionID;
typedef struct _XRegSession  XRegSession;

/*
 * A registry lock.
 */
typedef struct _XRegLock {
    QEntry entry;                   /* list entry */
    Mutex mutex;                    /* synchronization mutex */
    Event event;                    /* set when lock is released */
    StrBuf name;                    /* name of the lock */
    XRegSession * owner;            /* remote session that owns the lock */
    int waiters;                    /* number of sessions waiting */
} XRegLock;

/* 
 * Service information
 */
typedef struct _XRegServerService {
    XRegServer * server;            /* server that registered this service */
    XRegSession * session;          /* session that owns this service */
    XRegService service;            /* service information */
    Prop clientProps;               /* properties assigned by the client */
    int ref;                        /* reference count */
} XRegServerService;

/**
 * XRPC Registry server context.
 */
struct _XRegServer {
    Lock lock;                      /* synchronization */
    XRpcServer * server;            /* XRPC server object */
    XRpcSocket socket;              /* server socket */
    XRegLock regLock;               /* global (unnamed) registry lock */
    ThrID thread;                   /* the server thread */
    Queue locks;                    /* owned XRegLocks */
    Queue lockPool;                 /* pool of XRegLock */
    HashTable serviceMap;           /* service id -> XRegServerService */
    HashTable sessionMap;           /* maps XRpcSession to XRegSession */
    int remoteListeners;            /* number of remote listeners */
    XRegSessionID lastSessionID;    /* ID of the last created session */
    XRegWorkQueue eventQueue;       /* notifications */
    Mutex openCloseMutex;           /* protects openList and closeList */
    Vector openList;                /* XRPC sessions not yet initialized */
    Vector closeList;               /* XRPC sessions not yet deinitialized */
    Event openCloseEvent;           /* open/close notification event */
    WorkQueue * openClose;          /* open/close processing thread */
};

/*
 * Session context.
 *
 * NOTE: session IDs are being used by the event notification code to ensure
 * that only those sessions created *before* the event are notified.
 * Also, session ID changes when remote listener is registered to avoid sending 
 * extra notifications to the same listener.
 */
struct _XRegSession {
    XRegServer * server;            /* server that owns this session */
    XRpcSession * session;          /* the XRPC session */
    XRegSessionID id;               /* non-zero session ID */
    XRegLock * lock;                /* lock owned by this client */
    int flags;                      /* flags, see below */

#define XREG_SESSION_REGISTRY_LISTENER 0x0001 /* has RegistryListener */

};

/*
 * XREG_RemoteListServicesCB context
 */
typedef struct _XRegSrvEnumCtx {
    XRpcString name;
    XRpcContainer * result;
} XRegSrvEnumCtx;

/*
 * Context for XREG_SessionOpenCB and XREG_SessionCloseCB
 */
typedef struct _XRegOpenClose {
    XRegServer * server;            /* XRPC Registry server */
    XRpcSession * xsession;         /* XRPC session, created or closed */
} XRegOpenClose;

/*
 * Context of a service registered/unregistered event
 */
typedef struct _XRegServerEventWorkItem {
    XRegWorkItem item;
    XRegSessionID lastSessionID;
    Vector calls;
    XRegEvent event;
    HashCB sessionCB;
    XRegServerService * svc;
    XRpcElement * params;
} XRegServerEventWorkItem;

/* Various descriptors */
STATIC const XRpcProtocol xregProtocol;
STATIC const XRegWorkItemCB xregServerWorkItemCB;

/* Forward prototypes */
STATIC void XREC_CallWaitCB(VElement elem);

/* *************************************************************************
//
//  Utilities
//
// *************************************************************************/

STATIC XRpcElement * XREG_EncodeService(XRegService * s, Prop * p)
{
    XRpcContainer * c = XRPC_CreateStruct(XREG_STRUCT_SERVICE_INFO);
    if (c) {
        if (XRPC_AddStringField(c,XREG_STRUCT_SERVICE_INFO_ID,s->id) &&
            XRPC_AddStringField(c,XREG_STRUCT_SERVICE_INFO_NAME,s->name) &&
            (!s->host ||
            XRPC_AddStringField(c,XREG_STRUCT_SERVICE_INFO_HOST,s->host)) &&
            XRPC_AddIntField(c, XREG_STRUCT_SERVICE_INFO_PORT, s->port)) {
            if (PROP_Size(&s->props) > 0) {
                XRpcElement * e= XREG_EncodeProps(&s->props, p);
                if (e) {
                    if (XRPC_SetElementName(e,XREG_STRUCT_SERVICE_INFO_PROP)&&
                        XRPC_AddElement(c, e)) {
                        return XRPC_ContainerToElement(c);
                    }
                    XRPC_FreeElement(e);
                }
            } else {
                return XRPC_ContainerToElement(c);
            }
        }
        XRPC_FreeContainer(c);
    }
    return NULL;
}

/**
 * Generates a unique service id. Caller must hold the lock.
 */
STATIC char * XREG_ServerUniqueID(XRegServer *  server)
{
#ifdef _WIN32
    LUID luid;
#endif /* _WIN32 */
    int len;
    char id[26];

    ASSERT(LOCK_CanWrite(&server->lock));

    /* 
     * On Windows we will use AllocateLocallyUniqueId 
     */
#ifdef _WIN32
    if (AllocateLocallyUniqueId(&luid)) {
        sprintf(id,"%08lX%08lX",luid.HighPart,luid.LowPart);
        return STRING_Dup(id);
    }
#endif /* _WIN32 */

    /*
     * On Unix (or in unlikely event if AllocateLocallyUniqueId fails)
     * use platform independent algorithm (PID+address+random string)
     */
    len = sprintf(id,"%08X%08lX",(int)getpid(),(long)server);
    do {
        FILE_TempName(id + len, 8);
    } while (HASH_Get(&server->serviceMap, id));
    return STRING_Dup(id);
}

/* *************************************************************************
//
//  XRPC Registry Lock
//
// *************************************************************************/

#if !defined(XLOCK_TIMEOUT) && !defined(XLOCK_NO_TIMEOUT)
#  define XLOCK_TIMEOUT 30000 /* ms */
#endif /* !XLOCK_TIMEOUT && !XLOCK_NO_TIMEOUT */

/**
 * Initializes pre-allocated registry lock
 */
STATIC Bool XREG_InitLock(XRegLock * lock)
{
    memset(lock, 0, sizeof(*lock));
    if (MUTEX_Init(&lock->mutex)) {
        if (EVENT_Init(&lock->event)) {
            EVENT_Set(&lock->event);
            STRBUF_Init(&lock->name);
            return True;
        }
        MUTEX_Destroy(&lock->mutex);
    }
    return False;
}

/**
 * Destroys the registry lock. Does not free the memory used by the lock.
 */
void XREG_DestroyLock(XRegLock * lock)
{
    ASSERT(!lock->owner);
    ASSERT(!lock->waiters);
    ASSERT(!lock->entry.queue);
    MUTEX_Destroy(&lock->mutex);
    EVENT_Destroy(&lock->event);
    STRBUF_Destroy(&lock->name);
}

/**
 * Allocates a new lock, with or without name
 */
XRegLock * XREG_CreateLock(Str name)
{
    XRegLock * lock = MEM_New(XRegLock);
    if (lock) {
        if (XREG_InitLock(lock)) {
            if (!name || STRBUF_Copy(&lock->name,name)) {
                return lock;
            }
            XREG_DestroyLock(lock);
        }
        MEM_Free(lock);
    }
    return NULL;
}

/**
 * Destroys the lock and deallocates the memory used by the lock.
 */
void XREG_DeleteLock(XRegLock * lock)
{
    if (lock) {
        XREG_DestroyLock(lock);
        MEM_Free(lock);
    }
}

/**
 * Finds lock given its name
 */
STATIC XRegLock * XREG_FindLock(Queue * list, XRpcString name)
{
    QEntry * e;
    ASSERT(name && name[0]);
    for (e = QUEUE_First(list); e; e = QUEUE_Next(e)) {
        XRegLock * lock = QCAST(e,XRegLock,entry);
        if (STRBUF_EqualsTo(&lock->name, name)) {
            return lock;
        }
    }
    return NULL;
}

/**
 * Acquires the lock. Waits if necessary. The caller must hold the exclusive
 * lock.
 */
STATIC Bool XREG_AcquireLock(XRegLock * lock, XRegSession * owner)
{
    Bool ok = False;
    ASSERT(owner);
    if (owner) {
        ASSERT(LOCK_CanWrite(&owner->server->lock));
        lock->waiters++;
        LOCK_Unlock(&owner->server->lock);
        if (MUTEX_Lock(&lock->mutex)) {
            Bool locked = True;

            /* ASSERT that we are not trying to acquire the lock recursively */
            ASSERT(lock->owner != owner);
            if (lock->owner != owner) {
#ifdef XLOCK_TIMEOUT
                if (lock->owner) {
                    Time now = TIME_Now();
                    Time end = now + XLOCK_TIMEOUT;
                    do {
                        MUTEX_Unlock(&lock->mutex);
                        EVENT_TimeWait(&lock->event, (long)(end - now) + 1);
                        locked = MUTEX_Lock(&lock->mutex);
                        now = TIME_Now();
                    } while (lock->owner && locked && now < end);

                    /*
                     * We can exit the above loop for 3 reasons:
                     * 1. the lock has been released
                     * 2. MUTEX_Lock failed
                     * 3. timeout, which most likely means a deadlock
                     */
                    ASSERT(!lock->owner);
                }
#else /* !XLOCK_TIMEOUT */
                if (lock->owner) {
                    MUTEX_Unlock(&lock->mutex);
                    EVENT_Wait(&lock->event);
                    locked = MUTEX_Lock(&lock->mutex);
                }
#endif /* !XLOCK_TIMEOUT */
            
                if (!lock->owner) {
                    lock->owner = owner;
                    ok = True;
                }
            }
            if (locked) MUTEX_Unlock(&lock->mutex);
        }
        LOCK_WriteLock(&owner->server->lock);
        lock->waiters--;
    }
    return ok;
}

/**
 * Releases the lock
 */
STATIC void XREG_ReleaseLock(XRegLock * lock)
{
    ASSERT(lock->owner);
    MUTEX_Lock(&lock->mutex);
    lock->owner = NULL;
    EVENT_Set(&lock->event);
    MUTEX_Unlock(&lock->mutex);
}

/* *************************************************************************
//
//  Notifications
//
// *************************************************************************/

/**
 * Sends "serviceRegistered" or "serviceUnregistered" notification to the 
 * remote listener. Common code for XREG_ServerRegisterNotifyCB and 
 * XREG_ServerUnregisterNotifyCB
 */
STATIC Bool XREG_ServerEventCB(Str method, HashValue value, void * ctx)
{
    XRegSession * session = (XRegSession*)value;
    XRegServerEventWorkItem * wki = (XRegServerEventWorkItem*)ctx;
    ASSERT(wki->lastSessionID);
    if ((session->flags & XREG_SESSION_REGISTRY_LISTENER) &&
        (session->id <= wki->lastSessionID)) {
        XRpcCall * call = XRPC_Call(XRPC_GetClient(session->session), 
            XREG_LISTENER_PROTOCOL, method, wki->params);
        if (call) {
            
            /* 
             * waiting here for completion of the call may cause a deadlock.
             * store the call context in the vector and wait later after we
             * have released the lock. that's done by XREC_CallWaitCB callback
             * invoked by VECTOR_Clear.
             */ 
            if (!VECTOR_Add(&wki->calls, call)) {
                XRPC_FreeCall(call);
            }
        }
    }
    return True;
}

/**
 * Sends "serviceRegistered" notification to the remote listener
 */
STATIC Bool XREG_ServerRegisterNotifyCB(HashKey k, HashValue v, void * ctx)
{
    return XREG_ServerEventCB(XREG_LISTENER_METHOD_SERVICE_REG,v,ctx);
}

/**
 * Sends "serviceUnregistered" notification to the remote listener
 */
STATIC Bool XREG_ServerUnregisterNotifyCB(HashKey k, HashValue v, void * ctx)
{
    return XREG_ServerEventCB(XREG_LISTENER_METHOD_SERVICE_UNREG,v,ctx);
}

/**
 * Fires "serviceRegistered" notification
 */
STATIC void XREG_ServerRegisterServiceNotification(XRegServerService * svc)
{
    ASSERT(LOCK_CanWrite(&svc->server->lock));
    if (svc->server->remoteListeners) {
        XRegServerEventWorkItem * wki = MEM_New(XRegServerEventWorkItem);
        if (wki) {
            XRegRegisterServiceEvent * evt = &wki->event.registerService;
            memset(wki, 0, sizeof(*wki));
            evt->type = XRegEventRegisterService;
            wki->lastSessionID = svc->server->lastSessionID;
            wki->item.cb = &xregServerWorkItemCB;
            wki->svc = svc;
            wki->svc->ref++;
            wki->sessionCB = XREG_ServerRegisterNotifyCB;
            wki->params = XREG_EncodeService(&svc->service,&svc->clientProps);
            if (wki->params) {
                XRPC_SetElementName(wki->params,
                    XREG_LISTENER_PARAM_SERVICE_REG);
            }
            VECTOR_Init(&wki->calls, 0, NULL, XREC_CallWaitCB); 
            XREG_PostWorkItem(&svc->server->eventQueue, &wki->item);
        }
    }
}

/**
 * Fires "serviceUnregistered" notification
 */
STATIC void XREG_ServerUnregisterServiceNotification(XRegServerService * svc)
{
    ASSERT(LOCK_CanWrite(&svc->server->lock));
    if (svc->server->remoteListeners) {
        XRegServerEventWorkItem * wki = MEM_New(XRegServerEventWorkItem);
        if (wki) {
            XRegRegisterServiceEvent * evt = &wki->event.registerService;
            memset(wki, 0, sizeof(*wki));
            evt->type = XRegEventRegisterService;
            wki->lastSessionID = svc->server->lastSessionID;
            wki->item.cb = &xregServerWorkItemCB;
            wki->svc = svc;
            wki->svc->ref++;
            wki->sessionCB = XREG_ServerUnregisterNotifyCB;
            wki->params = XREG_EncodeService(&svc->service,&svc->clientProps);
            if (wki->params) {
                XRPC_SetElementName(wki->params,
                    XREG_LISTENER_PARAM_SERVICE_UNREG);
            }
            VECTOR_Init(&wki->calls, 0, NULL, XREC_CallWaitCB); 
            XREG_PostWorkItem(&svc->server->eventQueue, &wki->item);
        }
    }
}

/* *************************************************************************
//
//  Operations on service database
//
// *************************************************************************/

/**
 * Adds either local or remote service to the local service database.
 */
STATIC
XRegServerService *
XREG_ServerRegisterService(
    XRegServer *  server,     /* instance of XRegServer */
    XRpcString    id,         /* service id, NULL to generate new one */
    XRpcString    name,       /* service protocol name */
    XRpcString    host,       /* host name or (typically) NULL */
    XRpcPort      port,       /* listening port in host byte order */
    const XRegProperty * p [],/* service properties */
    int           np,         /* number of properties */
    XRegSession * session)    /* associated session */
{
    XRegServerService * other = NULL;
    XRegServerService * svc = MEM_New(XRegServerService);
    if (svc) {
        memset(svc, 0, sizeof(*svc));
        svc->ref = 1;
        svc->server = server;
        svc->session = session;
        svc->service.port = port;
        svc->service.name = STRING_Dup(name);
        if (svc->service.name) {
            if (host) svc->service.name = STRING_Dup(host);
            if (!host || svc->service.name) {
                if (PROP_Init(&svc->service.props)) {
                    if (PROP_Init(&svc->clientProps)) {
                        Prop * props = &svc->service.props;
                        int i;
                        for (i=0; i<np; i++) {
                            PROP_Set(props, p[i]->name, p[i]->value);
                        }
                        LOCK_WriteLock(&server->lock);
                        if (id) {
                            HashValue val = HASH_Get(&server->serviceMap,id);
                            ASSERT(!val);
                            if (val) {
                                other = (XRegServerService*)val;
                                svc->service.id = (char*)id;
                                if (XREG_EqualService(&svc->service,
                                                      &other->service)) {
                                    /* keep existing service */
                                    svc->service.id = NULL;
                                } else {
                                    /* replace existing service */
                                    svc->service.id = XREG_ServerUniqueID(server);
                                    other = NULL;
                                }
                            } else {
                                svc->service.id = STRING_Dup(id);
                            }
                        } else {
                            svc->service.id = XREG_ServerUniqueID(server);
                        }
                        if (svc->service.id) {
                            HashTable * map = &server->serviceMap;
                            if (HASH_Put(map,svc->service.id,svc)) {
                                XREG_ServerRegisterServiceNotification(svc);
                                LOCK_Unlock(&server->lock);
                                return svc;
                            }
                            MEM_Free(svc->service.id);
                        }
                        LOCK_Unlock(&server->lock);
                        PROP_Destroy(&svc->clientProps);
                    }
                    PROP_Destroy(&svc->service.props);
                }
                MEM_Free(svc->service.host);
            }
            MEM_Free(svc->service.name);
        }
        MEM_Free(svc);
    }
    return other;
}

/**
 * Removes registered service from the database
 */
void
XREG_ServerUnregisterService(
    XRegServer *    server,     /* instance of XRegServer */
    XRpcString      id)         /* service id assigned by the registry */
{
    XRegServerService * svc;
    LOCK_WriteLock(&server->lock);
    svc = (XRegServerService*)HASH_Get(&server->serviceMap, id);
    if (svc) {
        XREG_ServerUnregisterServiceNotification(svc);
        HASH_Remove(&server->serviceMap, id);
    }
    LOCK_Unlock(&server->lock);
}

/* *************************************************************************
//
//  Session deallocation
//
// *************************************************************************/

/**
 * Dereferences the service, deallocates if it's no longer being used
 */
STATIC void XREG_ServerDereferenceService(XRegServerService * svc)
{
    XRegServer * server = svc->server;
    LOCK_WriteLock(&server->lock);
    svc->ref--;
    ASSERT(svc->ref >= 0);
    if (svc->ref == 0) {
        if (svc->service.host) MEM_Free(svc->service.host);
        MEM_Free(svc->service.id);
        MEM_Free(svc->service.name);
        PROP_Destroy(&svc->service.props);
        PROP_Destroy(&svc->clientProps);
        MEM_Free(svc);
    }
    LOCK_Unlock(&server->lock);
}

/**
 * Callback for XREG_FreeSession. Cleans up the service list. Context points
 * to XRegSession being deallocated.
 */
STATIC Bool XREG_FreeSessionCB(HashKey key, HashValue value, void * ctx)
{
    XRegSession * session = (XRegSession*)ctx;
    XRegServerService * svc = (XRegServerService*)value;
    if (svc->session == session) {
        TRACE1("XREG: ditching service %s\n",svc->service.id);
        svc->session = NULL;
        XREG_ServerUnregisterService(session->server, svc->service.id);
        if (session->lock) {
            XRegServer * server = session->server;
            XREG_ReleaseLock(session->lock);
            if (session->lock != &server->regLock && !session->lock->waiters) {
                QUEUE_InsertTail(&server->lockPool, &session->lock->entry);
            }
            session->lock = NULL;
        }
    }
    return True;
}

/**
 * Deallocates XRegSession and all related XRegServerService entries 
 */
void XREG_FreeSession(XRegSession * session)
{
    HASH_Examine(&session->server->serviceMap, XREG_FreeSessionCB, session);
    MEM_Free(session);
}

/* *************************************************************************
//
//  Initialization and deinitialization
//
// *************************************************************************/

/**
 * XRPC registry server thread
 */
STATIC void XREG_ServerThread(void * arg)
{
    XRegServer * server = (XRegServer*)arg;
    TRACE("XREG: starting registry\n");
    XRPC_ServerListenSocket(server->server, server->socket, False);
    TRACE("XREG: registry is shut down\n");
}

/**
 * Deallocates entry in the id -> XRegServerService map
 */
STATIC void XREG_ServerServiceMapFree(HashKey key, HashValue value)
{
    XRegServerService * svc = (XRegServerService *)value;
    ASSERT(key == svc->service.id);
    XREG_ServerDereferenceService(svc);
}

/**
 * Deallocates entry in the XRpcSession to XRegSession map
 */
STATIC void XREG_SessionMapFree(HashKey key, HashValue value)
{
    XRegSession * session = (XRegSession *)value;
    ASSERT(session->session == key);
    XREG_FreeSession(session);
}

/**
 * Helper for XREG_CreateServer
 */
STATIC Bool XREG_InitServer(XRegServer * server, XRpcSocket socket)
{
    memset(server, 0, sizeof(*server));
    QUEUE_Init(&server->locks);
    QUEUE_Init(&server->lockPool);
    VECTOR_Init(&server->openList,0,NULL,NULL);
    VECTOR_Init(&server->closeList,0,NULL,NULL);
    if (HASH_Init(&server->sessionMap,0,NULL,NULL,XREG_SessionMapFree)) {
        if (HASH_Init(&server->serviceMap,0,hashCompareStringKey,
                stringHashProc, XREG_ServerServiceMapFree)) {
            if (LOCK_Init(&server->lock)) {
                if (XREG_InitWorkQueue(&server->eventQueue, &server->lock)) {
                    if (XREG_InitLock(&server->regLock)) {
                        server->openClose = WKQ_Create();
                        if (server->openClose) {
                            if (MUTEX_Init(&server->openCloseMutex)) {
                                if (EVENT_Init(&server->openCloseEvent)) {
                                    server->server = XRPC_CreateServer(NULL);
                                    if (server->server) {
                                        if (XRPC_RegisterProtocol(
                                            server->server,
                                            XREG_REGISTRY_PROTOCOL,
                                            "XRPC Registry", "Nokia",
                                            XREG_REGISTRY_PROTOCOL_VER_MAJOR,
                                            XREG_REGISTRY_PROTOCOL_VER_MINOR,
                                            &xregProtocol, server)) {
                                            server->socket = socket;
                                            LOCK_WriteLock(&server->lock);
                                            if (THREAD_Create(&server->thread,
                                                XREG_ServerThread, server)) {
                                                LOCK_Unlock(&server->lock);
                                                return True;
                                            }
                                        }
                                        XRPC_FreeServer(server->server);
                                    }
                                    EVENT_Destroy(&server->openCloseEvent);
                                }
                                MUTEX_Destroy(&server->openCloseMutex);
                            }
                            WKQ_Delete(server->openClose);
                        }
                        XREG_DestroyLock(&server->regLock);
                    }
                    XREG_DestroyWorkQueue(&server->eventQueue);
                }
                LOCK_Destroy(&server->lock);
            }
            HASH_Destroy(&server->serviceMap);
        }
        HASH_Destroy(&server->sessionMap);
    }
    VECTOR_Destroy(&server->openList);
    VECTOR_Destroy(&server->closeList);
    return False;
}

/**
 * Attepts to bind to the specified port and if succeeds, creates the server
 * context and starts the server thread. Returns pointer to the server context
 * (opaque to the code outside of this file) or NULL on failure.
 */
XRegServer * XREG_CreateServer(XRpcPort port)
{
    Socket socket;
    if (SOCKET_GetTcp(port, &socket)) {
        XRegServer * server = MEM_New(XRegServer);
        Verbose("XREG: bound to port %hu\n",port);
        if (server) {
            if (XREG_InitServer(server, socket)) {
                return server;
            }
            MEM_Free(server);
        }
        shutdown(socket,SHUT_RDWR);
        SOCKET_Close(socket);
    }
    return NULL;
}

/**
 * Cleans up pool of registry locks
 */
STATIC Bool XREG_CleanupLockPoolCB(QEntry * e, void * ctx)
{
    QUEUE_RemoveEntry(e);
    XREG_DeleteLock(QCAST(e,XRegLock,entry));
    return True;
}

/**
 * Stops XRPC Registry server and deallocates the context
 */
void XREG_FreeServer(XRegServer * server)
{
    if (server) {
        shutdown(server->socket,SHUT_RDWR);
        SOCKET_Close(server->socket);   /* this will unblock the server thread */
        XRPC_FreeServer(server->server);
        THREAD_Join(server->thread);
        WKQ_Delete(server->openClose);
        ASSERT(VECTOR_IsEmpty(&server->openList));
        ASSERT(VECTOR_IsEmpty(&server->closeList));
        XREG_DestroyLock(&server->regLock);
        XREG_DestroyWorkQueue(&server->eventQueue);
        QUEUE_Examine(&server->lockPool, XREG_CleanupLockPoolCB, NULL);
        LOCK_Destroy(&server->lock);
        HASH_Destroy(&server->serviceMap);
        HASH_Destroy(&server->sessionMap);
        EVENT_Destroy(&server->openCloseEvent);
        MUTEX_Destroy(&server->openCloseMutex);
        VECTOR_Destroy(&server->openList);
        VECTOR_Destroy(&server->closeList);
        MEM_Free(server);
    }
}

/* *************************************************************************
//
//  Remote call handlers
//
// *************************************************************************/

/**
 * Returns the current XREG session and obtains the server lock. The caller 
 * must not own the server lock. Upon successful return, the calling thread
 * owns the server lock. If it returns NULL, then the lock is not owned.
 */
STATIC XRegSession * XREG_LockSession(XRegServer * server)
{
    XRegSession * session = NULL;
    XRpcSession * xs = XRPC_GetCurrentSession(server->server);
    ASSERT(xs);
    ASSERT(!LOCK_CanRead(&server->lock));
    if (xs) {
        if (LOCK_WriteLock(&server->lock)) {
            session = (XRegSession*)HASH_Get(&server->sessionMap, xs);
            if (!session) {
                /*
                 * There is a race condition between XREG_SessionOpen
                 * and XREG_SessionClose running on a separate thread
                 * and an incoming XRPC calls. The separate thread processing
                 * the open/close events is necessary to avoid deadlocks.
                 */
                Bool closed = False;
                LOCK_Unlock(&server->lock);

                if (MUTEX_Lock(&server->openCloseMutex)) {
                    Bool locked = True;
                    while (locked && VECTOR_Contains(&server->openList,xs)) {
                        if (VECTOR_Contains(&server->closeList,xs)) {
                            closed = True;
                            break;
                        }

                        /* wait for an event */
                        EVENT_Reset(&server->openCloseEvent);
                        MUTEX_Unlock(&server->openCloseMutex);
                        EVENT_Wait(&server->openCloseEvent);
                        locked = MUTEX_Lock(&server->openCloseMutex);
                    }
                    if (locked) MUTEX_Unlock(&server->openCloseMutex);
                }

                if (LOCK_WriteLock(&server->lock)) {
                    if (!closed) {
                        session = (XRegSession*)HASH_Get(&server->sessionMap,xs);
                        ASSERT(session);
                    }
                    if (!session)  {
                        LOCK_Unlock(&server->lock);
                    }
                }
            }
        }
    }
    return session;
}

/* *************************************************************************
//
//  string registerService(string name, string host, int port, prop [])
//
//  Registers service with the registry. Parameters:
//
//  name  - service (protocol name) being registered.
//  host  - host name where the service is located (optional)
//  port  - port number the service is listening on
//  prop  - the service properties (array of xrpc.reg.Property structures)
//
//  If host is not specified, local host "127.0.0.1" is assumed.
//  Returns the service ID assigned by the registry. 
//
// *************************************************************************/
STATIC XRpcElement * XRPC_API 
XREG_HandleRegisterService(void * ctx, const XRpcContainer * p)
{
    XRpcElement * result = NULL;
    XRegServer * server = (XRegServer*)ctx;

    /* these two elements are required */
    const XRpcStringElement * nameElem = XRPC_GetStringElementByName(p,
        XREG_PARAM_REGISTER_SERVICE_NAME);
    const XRpcIntElement * portElem = XRPC_GetIntElementByName(p,
        XREG_PARAM_REGISTER_SERVICE_PORT);

    if (nameElem && portElem && XRPC_GetInt(portElem) <= USHRT_MAX) {

        /* optional parameters */
        const XRpcStringElement * idElem = XRPC_GetStringElementByName(p,
            XREG_PARAM_REGISTER_SERVICE_ID);
        const XRpcStringElement * hostElem = XRPC_GetStringElementByName(p,
            XREG_PARAM_REGISTER_SERVICE_HOST);
        const XRpcContainer * propArray = XRPC_IsArray(
            XRPC_GetElementByName(p,XREG_PARAM_REGISTER_SERVICE_PROP));

        XRpcString id = (idElem ? XRPC_GetString(idElem) : NULL);
        XRpcString host = (hostElem ? XRPC_GetString(hostElem) : NULL);
        const XRegProperty ** props = NULL;
        int nprops = 0;
        Vector propVector;
        XRegSession * session;

        /* decode properties */
        VECTOR_Init(&propVector, 0, NULL, vectorFreeValueProc);
        if (propArray) {
            int i, n = XRPC_ElementCount(propArray);
            for (i=0; i<n; i++) {
                const XRpcContainer * propStruct = XRPC_IsStruct(
                    XRPC_GetElementAt(propArray,i), XREG_STRUCT_PROPERTY);

                if (propStruct) {
                    const XRpcStringElement * propNameElem = 
                        XRPC_GetStringElementByName(propStruct, 
                        XREG_STRUCT_PROPERTY_NAME);
                    const XRpcStringElement * propValueElem = 
                        XRPC_GetStringElementByName(propStruct, 
                        XREG_STRUCT_PROPERTY_VALUE);

                    if (propNameElem && propValueElem) {
                        XRpcString propName = XRPC_GetString(propNameElem);
                        XRpcString propValue = XRPC_GetString(propValueElem);

                        if (propName && propName[0] &&  propValue) {
                            XRegProperty * prop = MEM_New(XRegProperty);
                            if (prop) {
                                prop->name = propName;
                                prop->value = propValue;
                                if (!VECTOR_Add(&propVector,prop)) {
                                    MEM_Free(prop);
                                }
                            }
                        }
                    }
                }
            }

            nprops = VECTOR_Size(&propVector);
            if (nprops) {
                props = (const XRegProperty **)VECTOR_GetElements(&propVector);
            }
        }

        /* acquire global registry lock */
        session = XREG_LockSession(server);
        if (session) {
            Bool unlock = False;
            XRegServerService * svc;

            if (server->regLock.owner != session) {
                unlock = XREG_AcquireLock(&server->regLock, session);
            }
        
            /* update service database */
            svc = XREG_ServerRegisterService(server, id, XRPC_GetString(nameElem),
                host,(XRpcShort)XRPC_GetInt(portElem), props, nprops, session);

            if (svc) {
                result = _XRPC_CreateStringElement(svc->service.id);
                if (!result) {
                    XREG_ServerUnregisterService(server, svc->service.id);
                }
            }
            LOCK_Unlock(&server->lock);
            if (unlock) XREG_ReleaseLock(&server->regLock);
        }
        VECTOR_Destroy(&propVector);
    }

    return result;
}

/* *************************************************************************
//
//  void unregisterService(string id)
//
//  Unregisters service from the registry. Parameters:
//
//  id    - service id assigned by XRPC registry
//
// *************************************************************************/
STATIC XRpcElement * XRPC_API 
XREG_HandleUnregisterService(void * ctx, const XRpcContainer * p)
{
    XRegServer * server = (XRegServer*)ctx;
    const XRpcStringElement * idElem = XRPC_GetStringElementByName(p,
        XREG_PARAM_UNREGISTER_SERVICE_ID);

    if (idElem) {
        XRpcString id = XRPC_GetString(idElem);
        if (id) {

            /* acquire global registry lock */
            XRegSession * session = XREG_LockSession(server);
            if (session) {
                Bool unlock = False;
                if (server->regLock.owner != session) {
                    unlock = XREG_AcquireLock(&server->regLock, session);
                }
            
                /* update service database */
                XREG_ServerUnregisterService(server, id);
                LOCK_Unlock(&server->lock);
                if (unlock) XREG_ReleaseLock(&server->regLock);
            }
        }
    }

    return NULL;
}

/* *************************************************************************
//
//  void registerListener(string protocol)
//
//  registers named listener.
//
//  protocol  - the name of the listener protocol to register. Currently,
//              only "xrpc.reg.RegistryListener" protocol is supported
//
// *************************************************************************/
STATIC Bool XREG_InitNotifyCB(HashKey key,HashValue value,void * ctx)
{
    XRpcSession * session = (XRpcSession*)ctx;
    XRegServerService * svc = (XRegServerService*)value;
    XRpcElement * elem = XREG_EncodeService(&svc->service,&svc->clientProps);
    if (elem) {
        XRPC_SetElementName(elem, XREG_LISTENER_PARAM_SERVICE_REG);
        XRPC_Notify(XRPC_GetClient(session), 
                    XREG_LISTENER_PROTOCOL,
                    XREG_LISTENER_METHOD_SERVICE_REG,
                    elem);
        XRPC_FreeElement(elem);
    }
    return True;
}

STATIC XRpcElement * XRPC_API 
XREG_HandleRegisterListener(void * ctx, const XRpcContainer * p)
{
    XRegServer * server = (XRegServer*)ctx;
    const XRpcStringElement * protoElem = XRPC_GetStringElementByName(p,
        XREG_PARAM_REGISTER_LISTENER_PROTO);

    if (protoElem) {
        XRpcString protocol = XRPC_GetString(protoElem);
        if (StrCmp(XREG_LISTENER_PROTOCOL,protocol) == 0) {
            XRpcSession * xs = XRPC_GetCurrentSession(server->server);
            XRegSession * session = XREG_LockSession(server);
            if (session) {
                if (!(session->flags & XREG_SESSION_REGISTRY_LISTENER)) {
                    session->flags |= XREG_SESSION_REGISTRY_LISTENER;
                    server->remoteListeners++;

                    /*
                     * We update the session ID to prevent pending 
                     * notifications from being sent to this client.
                     */
                    session->id = server->lastSessionID++;
                    HASH_Examine(&server->serviceMap,XREG_InitNotifyCB,xs);
                }
            }
            LOCK_Unlock(&server->lock);
        }
    }

    return NULL;
}

/* *************************************************************************
//
//  void unregisterListener(string protocol)
//
//  unregisters named listener.
//
//  protocol  - the name of the listener protocol to unregister. Currently,
//              only "xrpc.reg.RegistryListener" protocol is supported
//
// *************************************************************************/
STATIC XRpcElement * XRPC_API 
XREG_HandleUnregisterListener(void * ctx, const XRpcContainer * p)
{
    XRegServer * server = (XRegServer*)ctx;
    const XRpcStringElement * protoElem = XRPC_GetStringElementByName(p,
        XREG_PARAM_UNREGISTER_LISTENER_PROTO);

    if (protoElem) {
        XRpcString protocol = XRPC_GetString(protoElem);
        if (StrCmp(XREG_LISTENER_PROTOCOL,protocol) == 0) {
            XRegSession * session = XREG_LockSession(server);
            if (session) {
                if (session->flags & XREG_SESSION_REGISTRY_LISTENER) {
                    session->flags &= ~XREG_SESSION_REGISTRY_LISTENER;
                    server->remoteListeners--;
                    ASSERT(server->remoteListeners >= 0);
                }
                LOCK_Unlock(&server->lock);
            }
        }
    }

    return NULL;
}

/* *************************************************************************
//
//  xrpc.reg.ServiceInfo [] listServices(string name)
//
//  Lists registered services. Parameters:
//
//  name  - the name of the service (optional)
//
//  If the service name is not specified, then it returns the list of all
//  registered services.
//
// *************************************************************************/

STATIC Bool XREG_RemoteListServicesCB(HashKey key,HashValue value,void * ctx)
{
    XRegSrvEnumCtx * enumCtx = (XRegSrvEnumCtx*)ctx;
    XRegServerService * svc = (XRegServerService*)value;
    if (enumCtx->name && strcmp(svc->service.name, enumCtx->name)) {
        return True;
    } else {
        XRpcElement * e = XREG_EncodeService(&svc->service,&svc->clientProps);
        if (e) {
            return XRPC_TryAddElement(enumCtx->result, e);
        }
        return False;
    }
}

STATIC XRpcElement * XRPC_API 
XREG_HandleListServices(void * ctx, const XRpcContainer * p)
{
    XRegServer * server = (XRegServer*)ctx;
    XRegSrvEnumCtx e;

    /* "name" parameter is optional */
    const XRpcStringElement * ne = NULL;
    if (p) ne = XRPC_GetStringElementByName(p,XREG_PARAM_LIST_SERVICES_NAME);

    /* initialize enumeration context */
    memset(&e, 0, sizeof(e));
    if (ne) e.name = XRPC_GetString(ne);

    /* walk the hashtable */
    LOCK_ReadLock(&server->lock);
    if (HASH_Size(&server->serviceMap) > 0) {
        e.result = XRPC_CreateArray();
        if (e.result) {
            if (!HASH_Examine(&server->serviceMap,XREG_RemoteListServicesCB,&e)) {
                XRPC_FreeContainer(e.result);
                e.result = NULL;
            }
        }
    }
    LOCK_Unlock(&server->lock);

    return (e.result ? XRPC_ContainerToElement(e.result) : NULL);
}

/* *************************************************************************
//
//  xrpc.reg.ServiceInfo queryService(string id)
//
//  Returns information about the service identified by its id. Parameters:
//
//  id  - id of the service to query information about
//
// *************************************************************************/
STATIC XRpcElement * XRPC_API 
XREG_HandleQueryService(void * ctx, const XRpcContainer * p)
{
    XRpcElement * result = NULL;
    XRegServer * server = (XRegServer*)ctx;
    const XRpcStringElement * idElem;
    idElem = XRPC_GetStringElementByName(p,XREG_PARAM_QUERY_SERVICE_ID);
    if (idElem) {
        XRpcString id = XRPC_GetString(idElem);
        if (id) {
            XRegServerService * svc;
            LOCK_ReadLock(&server->lock);
            svc = (XRegServerService*)HASH_Get(&server->serviceMap, id);
            if (svc) {
                result = XREG_EncodeService(&svc->service, &svc->clientProps);
            }
            LOCK_Unlock(&server->lock);
        }
    }

    return result;
}

/* *************************************************************************
//
//  boolean setProperty(string id, string name, string value)
//
//  Sets property for the specified service
//
//  id  - id of the service to modify
//  name - property name
//  value - property value
//
//  Returns true if the property has been created or updated, false
//  if the client is not allowed to set this property
//
// *************************************************************************/

STATIC XRpcElement * XRPC_API 
XREG_HandleSetProperty(void * ctx, const XRpcContainer * p)
{
    XRpcBool ok = XRpcFalse;
    const XRpcStringElement * idElem;
    const XRpcStringElement * nameElem;
    const XRpcStringElement * valueElem;
    XRegServer * server = (XRegServer*)ctx;
    idElem = XRPC_GetStringElementByName(p,XREG_PARAM_SET_PROPERTY_ID);
    nameElem = XRPC_GetStringElementByName(p,XREG_PARAM_SET_PROPERTY_NAME);
    valueElem = XRPC_GetStringElementByName(p,XREG_PARAM_SET_PROPERTY_VALUE);
    if (idElem && nameElem && valueElem && XRPC_GetStringLength(nameElem)) {
        XRpcString id = XRPC_GetString(idElem);
        if (id) {
            XRegServerService * svc;
            LOCK_WriteLock(&server->lock);
            svc = (XRegServerService*)HASH_Get(&server->serviceMap, id);
            if (svc) {
                XRpcString name = XRPC_GetString(nameElem);
                if (name) {

                    /* check if this is a readonly property */
                    if (!PROP_Get(&svc->service.props,name)) {
                        XRpcString value = XRPC_GetString(valueElem);
                        if (value) {
                            Bool update = True;
                            Str existValue = PROP_Get(&svc->clientProps,name);
                            if (existValue) {
                                if (StrCmp(existValue,value) == 0) {
                                    update = False;
                                    ok = True;
                                }
                            }

                            /* actually update the client property */
                            if (update && 
                                PROP_Set(&svc->clientProps,name,value)) {
                                ok = True;
                                if (XRPC_GetCurrentSession(
                                    svc->server->server) !=
                                    svc->session->session) {

                                    /* modified by different session */
                                    XRPC_FormatNotify(
                                        XRPC_GetClient(svc->session->session),
                                        XREG_LISTENER_PROTOCOL, 
                                        XREG_LISTENER_METHOD_PROP_REMOVE, "%"
                                        XREG_LISTENER_PARAM_PROP_CHANGE_ID"!s%"
                                        XREG_LISTENER_PARAM_PROP_REMOVE_NAME"!s%"
                                        XREG_LISTENER_PARAM_PROP_CHANGE_VALUE"!s",
                                        id, name, value);
                                }
                            }
                        }
                    }
                }
            }
            LOCK_Unlock(&server->lock);
        }
    }
    return _XRPC_CreateBoolElement(ok);
}

/* *************************************************************************
//
//  boolean clearProperty(string id, string name)
//
//  Clears (removes) a property for the specified service
//
//  id  - id of the service to modify
//  name - name of the property to remove
//
//  Returns true if the property has been removed, false if the property
//  did not exist or the client is not allowed to set this property
//
// *************************************************************************/

STATIC XRpcElement * XRPC_API 
XREG_HandleClearProperty(void * ctx, const XRpcContainer * p)
{
    XRpcBool ok = XRpcFalse;
    const XRpcStringElement * idElem;
    const XRpcStringElement * nameElem;
    XRegServer * server = (XRegServer*)ctx;
    idElem = XRPC_GetStringElementByName(p,XREG_PARAM_CLEAR_PROPERTY_ID);
    nameElem = XRPC_GetStringElementByName(p,XREG_PARAM_CLEAR_PROPERTY_NAME);
    if (idElem && nameElem && XRPC_GetStringLength(nameElem)) {
        XRpcString id = XRPC_GetString(idElem);
        if (id) {
            XRegServerService * svc;
            LOCK_WriteLock(&server->lock);
            svc = (XRegServerService*)HASH_Get(&server->serviceMap, id);
            if (svc) {
                XRpcString name = XRPC_GetString(nameElem);
                if (name) {

                    /* check if this is a readonly property */
                    if (!PROP_Get(&svc->service.props, name)) {
                        if (PROP_Remove(&svc->clientProps, name)) {
                            ok = True;
                            if (XRPC_GetCurrentSession(
                                svc->server->server) !=
                                svc->session->session) {

                                /* modified by different session */
                                XRPC_FormatNotify(
                                    XRPC_GetClient(svc->session->session),
                                    XREG_LISTENER_PROTOCOL, 
                                    XREG_LISTENER_METHOD_PROP_REMOVE, "%"
                                    XREG_LISTENER_PARAM_PROP_REMOVE_ID "!s%"
                                    XREG_LISTENER_PARAM_PROP_REMOVE_NAME "!s",
                                    id,name);
                            }
                        }
                    }
                }
            }
            LOCK_Unlock(&server->lock);
        }
    }
    return _XRPC_CreateBoolElement(ok);
}

/* *************************************************************************
//
//  boolean lock(string name)
//
//  Acquires a named lock.
//
//  name  - the name of the lock. If this parameter is missing or the name
//          is empty, a global unnamed lock is acquired. It's guaranteed 
//          that while unnamed lock is owned by a client, no modifications
//          will be made to the service database by any other client.
//          One client session is not allowed to hold more than one lock.
//
// *************************************************************************/
STATIC XRpcElement * XRPC_API 
XREG_HandleLock(void * ctx, const XRpcContainer * p)
{
    Bool ok = False;
    XRegServer * server = (XRegServer*)ctx;
    XRegSession * session = XREG_LockSession(server);
    if (session) {
        if (!session->lock) {
            Str name = NULL;
            XRegLock * lock;
            const XRpcStringElement * nameElem;
            nameElem = XRPC_GetStringElementByName(p,XREG_PARAM_LOCK_NAME);
            if (nameElem) name = XRPC_GetString(nameElem);

            /* find the lock */
            if (name && name[0]) {
                lock = XREG_FindLock(&server->locks,name);
                ASSERT(!lock || lock->owner);
                if (!lock) {
                    if (QUEUE_IsEmpty(&server->lockPool)) {
                        lock = XREG_CreateLock(name);
                    } else {
                        QEntry * e = QUEUE_RemoveHead(&server->lockPool);
                        lock = QCAST(e,XRegLock,entry);
                        ASSERT(!lock->waiters);
                        if (!STRBUF_Copy(&lock->name,name)) {
                            QUEUE_InsertTail(&server->lockPool, &lock->entry);
                            lock = NULL;
                        }
                    }
                }
            } else {
                lock = &server->regLock;
            }

            /* acquire the lock */
            if (lock) {
                ok = XREG_AcquireLock(lock, session);
                if (ok) {
                    if (session->lock) {
                        XREG_ReleaseLock(lock);
                        ok = False;
                    } else {
                        session->lock = lock;
                    }
                }
                if (lock != &server->regLock) {
                    if (ok) {
                        QUEUE_InsertTail(&server->locks, &lock->entry);
                    } else if (!lock->waiters) {
                        QUEUE_InsertTail(&server->lockPool, &lock->entry);
                    }
                }
            }
        }
        LOCK_Unlock(&server->lock);
    }
    return _XRPC_CreateBoolElement(ok);
}

/* *************************************************************************
//
//  void unlock()
//
//  Releases a lock held by the client.
//
// *************************************************************************/
STATIC XRpcElement * XRPC_API 
XREG_HandleUnlock(void * ctx, const XRpcContainer * p)
{
    XRegServer * server = (XRegServer*)ctx;
    XRegSession * session = XREG_LockSession(server);
    if (session) {
        if (session->lock) {
            ASSERT(session->lock->owner == session);
            ASSERT(session->lock == &server->regLock ||
                   session->lock->entry.queue == &server->locks);

            XREG_ReleaseLock(session->lock);
            QUEUE_RemoveEntry(&session->lock->entry);
            if (session->lock != &server->regLock && !session->lock->waiters) {
                QUEUE_InsertTail(&server->lockPool, &session->lock->entry);
            }
            session->lock = NULL;
        }
        LOCK_Unlock(&server->lock);
    }
    return NULL;
}

/**
 * XRPC Registry protocol handler 
 */
STATIC
XRpcBool 
XRPC_API
XREG_ProtocolCB(
    void *                  context,    /* pointer to XRegServer */
    XRpcString              name,       /* method name */
    XRpcMethodInfo *        info)       /* receives handler and context */
{
    if (StrCmp(XREG_METHOD_LIST_SERVICES,name) == 0) {
        info->handler = XREG_HandleListServices;
    } else if (StrCmp(XREG_METHOD_QUERY_SERVICE,name) == 0) {
        info->handler = XREG_HandleQueryService;
    } else if (StrCmp(XREG_METHOD_REGISTER_SERVICE,name) == 0) {
        info->handler = XREG_HandleRegisterService;
    } else if (StrCmp(XREG_METHOD_UNREGISTER_SERVICE,name) == 0) {
        info->handler = XREG_HandleUnregisterService;
    } else if (StrCmp(XREG_METHOD_REGISTER_LISTENER,name) == 0) {
        info->handler = XREG_HandleRegisterListener;
    } else if (StrCmp(XREG_METHOD_UNREGISTER_LISTENER,name) == 0) {
        info->handler = XREG_HandleUnregisterListener;
    } else if (StrCmp(XREG_METHOD_SET_PROPERTY,name) == 0) {
        info->handler = XREG_HandleSetProperty;
    } else if (StrCmp(XREG_METHOD_CLEAR_PROPERTY,name) == 0) {
        info->handler = XREG_HandleClearProperty;
    } else if (StrCmp(XREG_METHOD_LOCK,name) == 0) {
        info->handler = XREG_HandleLock;
    } else if (StrCmp(XREG_METHOD_UNLOCK,name) == 0) {
        info->handler = XREG_HandleUnlock;
    } else {
        return XRpcFalse;
    }
    info->context = context;
    info->synchronous = True;
    return True;
}

/**
 * Creates XRegSession and inserts into the map. The caller must own 
 * the lock.
 */
STATIC XRegSession * XREG_CreateSession(XRegServer * server, XRpcSession * s)
{
    XRegSession * session = MEM_New(XRegSession);
    ASSERT(LOCK_CanWrite(&server->lock));
    if (session) {
        memset(session, 0, sizeof(*session));
        session->id = (++(server->lastSessionID));
        session->server = server;
        session->session = s;
        if (HASH_Put(&server->sessionMap, s, session)) {
            return session;
        }
        MEM_Free(session);
    }
    return NULL;
}

/**
 * "session started" handler
 */
STATIC void XREG_SessionOpen(WorkItem * w, void * ctx)
{
    int i;
    XRegOpenClose * open = (XRegOpenClose*)ctx;
    
    LOCK_WriteLock(&open->server->lock);
    XREG_CreateSession(open->server, open->xsession);
    LOCK_Unlock(&open->server->lock);

    /* remove xsession from the pending list and notify XREG_LockSession */
    MUTEX_Lock(&open->server->openCloseMutex);
    i = VECTOR_IndexOf(&open->server->openList, open->xsession);
    VECTOR_Remove(&open->server->openList, i);
    EVENT_Set(&open->server->openCloseEvent);
    MUTEX_Unlock(&open->server->openCloseMutex);

    /* deallocate the context */
    MEM_Free(open);
}

/**
 * "session started" protocol callback 
 */
STATIC
void 
XRPC_API
XREG_SessionOpenCB(
    void *                  context,    /* protocol specific context */
    XRpcSession *           xsession)   /* new session */
{
    /* blocking in session open notification may cause deadlock... */
    XRegOpenClose * open = MEM_New(XRegOpenClose);
    if (open) {
        XRegServer * server = (XRegServer*)context;
        open->server = server;
        open->xsession = xsession;
        MUTEX_Lock(&server->openCloseMutex);
        VECTOR_Add(&server->openList, xsession);
        if (!WKQ_InvokeLater(server->openClose, XREG_SessionOpen, open)) {
            int i = VECTOR_IndexOf(&server->openList, xsession);
            VECTOR_Remove(&server->openList, i);
            MEM_Free(open);
        }
        MUTEX_Unlock(&server->openCloseMutex);
    }
}

/**
 * "session closed" handler
 */
STATIC void XREG_SessionClose(WorkItem * w, void * ctx)
{
    int i;
    XRegOpenClose * close = (XRegOpenClose*)ctx;
    XRegServer * server = close->server;
    XRegSession * session;
    LOCK_WriteLock(&close->server->lock);
    session = HASH_Get(&server->sessionMap, close->xsession);
    if (session && session->lock) {
        /* if session still holds the lock, release it */
        XREG_ReleaseLock(session->lock);
        QUEUE_RemoveEntry(&session->lock->entry);
        if (session->lock != &server->regLock && !session->lock->waiters) {
            QUEUE_InsertTail(&server->lockPool, &session->lock->entry);
        }
        session->lock = NULL;
    }
    VERIFY(HASH_Remove(&server->sessionMap, close->xsession));
    LOCK_Unlock(&server->lock);

    /* remove xsession from the closed list and notify XREG_LockSession */
    MUTEX_Lock(&server->openCloseMutex);
    i = VECTOR_IndexOf(&server->closeList, close->xsession);
    VECTOR_Remove(&server->closeList, i);
    EVENT_Set(&server->openCloseEvent);
    MUTEX_Unlock(&server->openCloseMutex);

    /* deallocate the context */
    MEM_Free(close);
}

/**
 * "session finished" protocol callback 
 */
STATIC
void 
XRPC_API
XREG_SessionCloseCB(
    void *                  context,    /* protocol specific context */
    XRpcSession *           xsession)   /* dead session */
{
    /* blocking in session close notification may cause deadlock... */
    XRegOpenClose * close = MEM_New(XRegOpenClose);
    if (close) {
        XRegServer * server = (XRegServer*)context;
        close->server = server;
        close->xsession = xsession;
        MUTEX_Lock(&server->openCloseMutex);
        VECTOR_Add(&server->closeList, xsession);
        if (!WKQ_InvokeLater(server->openClose, XREG_SessionClose, close)) {
            int i = VECTOR_IndexOf(&server->closeList, xsession);
            VECTOR_Remove(&server->closeList, i);
            MEM_Free(close);
        }
        MUTEX_Unlock(&server->openCloseMutex);
    }
}

/* XRPC Registry protocol descriptor */

STATIC const XRpcProtocol xregProtocol = {
    XREG_ProtocolCB,                    /* getmethod */
    NULL,                               /* cancel */
    XREG_SessionOpenCB,                 /* sopen */
    XREG_SessionCloseCB                 /* sclose */
};

/* *************************************************************************
//
//  Work items
//
// *************************************************************************/

STATIC void XREG_ServerWorkItemFree(XRegServerEventWorkItem * wki)
{
    XREG_ServerDereferenceService(wki->svc);
    XRPC_FreeElement(wki->params);
    VECTOR_Destroy(&wki->calls);
    MEM_Free(wki);
}

STATIC void XREG_ServerWorkItemCancel(XRegWorkItem * w)
{
    XREG_ServerWorkItemFree(CAST(w,XRegServerEventWorkItem,item));
}

STATIC void XREC_CallWaitCB(VElement elem)
{
    XRpcCall * call = (XRpcCall*)elem;
    XRPC_Wait(call);
    XRPC_FreeCall(call);
}

STATIC void XREG_ServerWorkItemRun(XRegWorkItem * w)
{
    XRegServerEventWorkItem * wki = CAST(w,XRegServerEventWorkItem,item);
        
    /* notify remote listeners */
    LOCK_ReadLock(&wki->svc->server->lock);
    HASH_Examine(&wki->svc->server->sessionMap, wki->sessionCB, wki);
    LOCK_Unlock(&wki->svc->server->lock);

    /* wait for all calls to complete */
    VECTOR_Clear(&wki->calls);
    XREG_ServerWorkItemFree(wki);
}

/*
 * work item callbacks
 */

STATIC const XRegWorkItemCB xregServerWorkItemCB = {
    XREG_ServerWorkItemRun,        /* runs and deallocates the work item */
    XREG_ServerWorkItemCancel      /* just deallocates the work item */
};

/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

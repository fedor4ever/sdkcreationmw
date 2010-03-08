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
* Description:  XRPC server object
 *
*/


#include "x_int.h"

/* *************************************************************************
//
//  Server support
//
// *************************************************************************/

typedef struct _XRpcElementContext XRpcElementContext;

struct _XRpcServer {
    uint32_t signature;             /* XRPC_SERVER_SIGNATURE */
    XParser * parser;               /* parser used by the server */
    HashTable protMap;              /* maps name to Protocol structure */
    HashTable tagMap;               /* maps tags to handlers */
    HashTable callMap;              /* maps call ids to the call context */
    ThrKey    sessionKey;           /* thread local key for XRpcSession */
    int       poolMax;              /* max number of threads to keep alive  */
    Vector    workPool;             /* pool of work threads */
    Vector    workActive;           /* active work threads */
    Vector    workDead;             /* dead work threads */
    Event     workEvent;            /* see XRPC_FreeServer */
    XRpcLong  lastId;               /* last call id */
    Lock      rwlock;               /* read-write lock */
    Event     sessionFreeEvent;     /* signaled when session is deallocated */
    Queue     sessions;             /* sessions */
    Queue     autoThreads;          /* auto-created threads */
    Bool      shuttingDown;         /* server is shutting down */
    void *    context;              /* context for callbacks below */
    XRpcThreadCreate createThread;  /* creates session thread */
    XRpcThreadJoin joinThread;      /* waits until thread is done */
    XRpcThreadDetach detachThread;  /* makes thread free-running */
    XRpcSessionOpen  sopen;         /* session started */
    XRpcSessionClose sclose;        /* session finished */
};

/* context for XRPC_ListProtocols */
typedef struct _ListProtocols {
    ListProtocolCB  cb;             /* callback to invoke */
    void *          context;        /* callback's context */
} ListProtocols;

/* Descriptor of an auto-created server thread */
typedef struct _XRpcServerThread {
    QEntry          entry;          /* list entry */
    XRpcSocket      socket;         /* listening socket */
    XRpcThread *    thread;         /* ID of auto-created thread */
} XRpcServerThread;

/* XRPC_ServerSpawn context */
typedef struct _XRpcServerSpawn {
    Event           event;          /* thread started event */
    XRpcServer *    server;         /* back pointer to XRpcServer */
    XRpcServerThread* spawn;        /* auto-created thread descriptor */
} XRpcServerSpawn;

/* *************************************************************************
//
//  Connection
//
// *************************************************************************/

typedef void (*XRpcCloseProc)(XRpcSession * session, void * context);

struct _XRpcSession {
    QEntry entry;                   /* entry in XRpcServer::sessions */
    Queue calls;                    /* active asynchronous calls */
    Event callEvent;                /* signaled when async call is complete */
    XRpcServer * server;            /* the server */
    XRpcClient * client;            /* client that sends data */
    File * out;                     /* output stream */
    File * in;                      /* input stream */
    Vector stack;                   /* element stack */
    XRpcThread * thread;            /* the parsing thread */
    XRpcThreadJoin joinThread;      /* waits until thread is done */
    XRpcThreadDetach detachThread;  /* makes thread free-running */
    int problem;                    /* problem counter */
    XRpcCloseProc closeProc;        /* XRPC_CloseSession helper */
    void * closeProcContext;        /* will be passed to closeProc */
    void * opaque;                  /* XRPC_Get/SetSessionContext */
    int flags;                      /* flags, see below */

#define XRPC_SESSION_DONE        0x01 /* root tag closed */
#define XRPC_SESSION_CLOSED      0x02 /* no more reads from input stream */
#define XRPC_SESSION_FREE_SERVER 0x04 /* free the server object when done */
#define XRPC_SESSION_OWN_INPUT   0x08 /* session owns the input stream */
#define XRPC_SESSION_OWN_OUTPUT  0x10 /* session owns the output stream */
#define XRPC_SESSION_THREAD      0x20 /* session thread was created */
#define XRPC_SESSION_THREAD_JOIN 0x40 /* session thread will be joined */
#define XRPC_SESSION_INTERNAL    0x80 /* deallocated on session thread */

};

/*
 * Network session information
 */

typedef struct _XRpcNetSessionData {
    Socket socket;                  /* open socket */
    IPaddr ipaddr;                  /* ip address of the other side */
    Port port;                      /* ip port on the other side */
} XRpcNetSessionData;

/*
 * Tag hadler
 */

typedef XRpcElementContext * (* XParseElementHandler) (
    XRpcSession * session,          /* session context */
    XRpcTag tag,                    /* the tag */
    const XRpcAtts atts             /* attributes */
);

typedef struct _ElementInfo {
    XParseElementHandler handler;   /* the handler */
    XRpcTag tag;                    /* the tag enum value */
} ElementInfo;

/*
 * context associated with an element
 */

typedef void (* XRpcElementClose) (
    XRpcElementContext * context    /* context of the element to close */
);

typedef void (* XRpcElementHandleData) (
    XRpcElementContext * context,   /* context */
    const XRpcUcs2 * chars,         /* the characters */
    size_t len                      /* number of charactars */
);

struct _XRpcElementContext {
    XRpcSession * session;          /* session context */
    XRpcElement * elem;             /* the element, if any */
    XRpcTag tag;                    /* tag */
    XRpcElementClose close;         /* close callback */
    XRpcElementHandleData handler;  /* character data handler */
};

/*
 * context associated with a data element
 */

typedef struct _XRpcDataElementContext {
    uint32_t signature;             /* DATA_CONTEXT_SIGNATURE */
    XRpcElementContext common;      /* common element context */
    XRpcUcs2 * buf;                 /* buffer for character data */
    size_t alloc;                   /* allocated size (in characters) */
    size_t len;                     /* data length (in characters) */
} XRpcDataElementContext;

/*
 * context associated with a call element
 */

typedef struct _XRpcCallContext {
    uint32_t signature;             /* CALL_CONTEXT_SIGNATURE */
    XRpcElementContext common;      /* common element context */
    XRpcMethodInfo method;          /* method that will be invoked */
} XRpcCallContext;

/*
 * call context. Note that we also handle response that comes over 
 * any session associated with this server, not just the one that
 * used to submit the call. It's possible to disallow that, but I 
 * don't see a good enough reason for that. However, when a session 
 * exits, all the calls submitted over this session get cancelled.
 */

struct _XRpcCall {
    char * id;                      /* id of the call to wait for */
    XRpcSession * session;          /* the session submitted the call */
    Event event;                    /* the caller is waiting on this event */
    XRpcBool done;                  /* true if the call has completed */
    XRpcStatus status;              /* status of the completed call */
    XRpcElement * result;           /* the result of the call */
};

/*
 * work item dispatching incoming XRPC calls
 */

typedef struct _XRpcCallWorkItem {
    QEntry entry;                   /* entry in XRpcSession::calls */
    XRpcMethodInfo method;          /* method that will be invoked */
    XRpcContainer * params;         /* call parameters */
    XRpcSession * session;          /* session that's using this work item */
    XRpcServer * server;            /* server context */
    WorkQueue * q;                  /* thread running this work item */
    char * id;                      /* call id (may be NULL) */
} XRpcCallWorkItem;

/*
 * session thread context
 */

typedef struct _XRpcSessionThread {
    XRpcSession * session;          /* the session */
    XRpcSessionProc proc;           /* the function to call */
    void * context;                 /* the context parameter */
} XRpcSessionThread;

#define SERVER_WRITE_LOCK(_srv)   LOCK_WriteLock(&(_srv)->rwlock)
#define SERVER_READ_LOCK(_srv)    LOCK_ReadLock(&(_srv)->rwlock)
#define SERVER_UNLOCK(_srv)       LOCK_Unlock(&(_srv)->rwlock)
#define SERVER_IS_LOCKED(_srv)    LOCK_CanWrite(&(_srv)->rwlock)
#define ASSERT_INPUT_THREAD(_session) \
        ASSERT(XRPC_GetCurrentSession((_session)->server) == (_session)) 

/* special thread context */
#define XRPC_CLIENT_THREAD ((XRpcThread*)(-1))

/* signatures */

#define XRPC_SERVER_SIGNATURE           _SIGNATURE32('X','S','R','V')
#define DATA_CONTEXT_SIGNATURE          _SIGNATURE32('D','A','T','A')
#define CALL_CONTEXT_SIGNATURE          _SIGNATURE32('C','A','L','L')
#define RESPONSE_CONTEXT_SIGNATURE      _SIGNATURE32('R','E','S','P')

/* *************************************************************************
//
//  Forward declarations
//
// *************************************************************************/

STATIC
void 
XPARSE_API 
XRPC_HandleStartElement(
    void * context,             /* XRPC defined context */
    const char * name,          /* the element name */
    const XRpcAtts atts         /* attributes */
);

STATIC
void 
XPARSE_API 
XRPC_HandleEndElement(
    void * context,             /* XRPC defined context */
    const char * name           /* the element name */
);

STATIC
void 
XPARSE_API 
XRPC_HandleCharacterData(
    void * context,             /* XRPC defined context */
    const XRpcUcs2 * chars,     /* the characters */
    int len                     /* number of charactars */
);

STATIC XRpcBool XRPC_InitTagHandlers(XRpcServer * srv);

/* *************************************************************************
//
//  Default thread handlers
//
//  NOTE: when we manipulate XRpcThread pointers, they don't necessarily 
//  point to the structure defined below, only if the default thread API
//  is used. If the client code changes the thread callbacks, we don't
//  know what XRpcThread pointer is pointing to.
//
// *************************************************************************/

struct _XRpcThread {
    ThrID tid;
};

STATIC void XRPC_ThreadJoin(XRpcThread * thread)
{
    THREAD_Join(thread->tid);
    MEM_Free(thread);
}

STATIC void XRPC_ThreadDetach(XRpcThread * thread)
{
    THREAD_Detach(thread->tid);
    MEM_Free(thread);
}

STATIC XRpcThread * XRPC_ThreadCreate(XRpcThreadProc proc, void * arg)
{
    XRpcThread * thread = MEM_New(XRpcThread);
    if (thread) {
        thread->tid = (ThrID)0;
        if (THREAD_Create(&thread->tid, proc, arg)) {
            return thread;
        }
        MEM_Free(thread);
    }
    return NULL;
}

/* *************************************************************************
//
//  Utilities
//
// *************************************************************************/

/**
 * Copies attributes into the element.
 */
STATIC XRpcBool XRPC_PutAttributes(XRpcElement * e, const XRpcAtts atts)
{
    XRpcAtts a = atts;
    while (a[0] && a[1]) {
        if (!XRPC_SetAttribute(e, a[0], a[1])) {
            return XRpcFalse;
        }
        a += 2;
    }
    return XRpcTrue;
}

/**
 * Copies attributes into the element.
 */
STATIC 
XRpcBool XRPC_CopyAttributes(XRpcElement * dest, const XRpcElement * src)
{
    int i, n = XRPC_AttributeCount(src);
    for (i=0; i<n; i++) {
        const XRpcAttr * attr = XRPC_GetAttributeAt(src, i);
        if (!XRPC_SetAttribute(dest, attr->name, attr->value)) {
            return XRpcFalse;
        }
    }
    return XRpcTrue;
}

/* *************************************************************************
//
//  Protocol
//
// *************************************************************************/

STATIC void PROTOCOL_Free(Protocol* p)
{
    if (p) {
        if (p->name) {
            MEM_Free(p->name);
        }
        if (p->descr) {
            MEM_Free(p->descr);
        }
        if (p->vendor) {
            MEM_Free(p->vendor);
        }
        MEM_Free(p);
    }
}

STATIC 
Protocol * 
PROTOCOL_Create(
    XRpcString            name,       /* protocol name */
    XRpcString            descr,      /* protocol description */
    XRpcString            vendor,     /* protocol vendor */
    int                   vmajor,     /* major version of the protocol */
    int                   vminor,     /* minor version of the protocol */
    const XRpcProtocol *  protocol,   /* protocol handler */
    void *                context)    /* protocol context */
{
    ASSERT(name);
    ASSERT(protocol);
    ASSERT(protocol->getmethod);
    if (name && protocol) {
        Protocol * p = MEM_New(Protocol);
        if (p) {
            memset(p, 0, sizeof(*p));
            if (((p->name = STRING_Dup(name)) != NULL) &&
                (!descr  || (p->descr  = STRING_Dup(descr))  != NULL) &&
                (!vendor || (p->vendor = STRING_Dup(vendor)) != NULL)) {
                p->vmajor = vmajor;
                p->vminor = vminor;
                p->protocol = *protocol;
                p->context = context;
                return p;
            }
            PROTOCOL_Free(p);
        }
    }
    return NULL;
}

XRpcBool 
XRPC_RegisterProtocol(
    XRpcServer *          server,     /* server instance */
    XRpcString            name,       /* protocol name */
    XRpcString            descr,      /* protocol description */
    XRpcString            vendor,     /* protocol vendor */
    int                   vmajor,     /* major version of the protocol */
    int                   vminor,     /* minor version of the protocol */
    const XRpcProtocol *  protocol,   /* protocol handler */
    void *                context)    /* protocol context */
{
    XRpcBool ok = XRpcFalse;
    Protocol * p = PROTOCOL_Create(name, descr, vendor, vmajor, vminor, 
                                   protocol, context);
    if (p) {
        SERVER_WRITE_LOCK(server);
        ok = HASH_Put(&server->protMap, p->name, p);
        SERVER_UNLOCK(server);
        if (!ok) {
            PROTOCOL_Free(p);
        }
    }
    return ok;
}

XRpcBool 
XRPC_UnregisterProtocol(
    XRpcServer *            server,     /* server instance */
    XRpcString              name)       /* protocol name */
{
    XRpcBool ok = XRpcFalse;
    Protocol * p;
    SERVER_WRITE_LOCK(server);
    p = (Protocol*)HASH_Get(&server->protMap, name);
    if (p) {
        HASH_Remove(&server->protMap, name); /* this deallocates the protocol */
        ok = True;
    }
    SERVER_UNLOCK(server);
    return  ok;
}

/**
 * Finds protocol by name
 */
Protocol *
XRPC_GetProtocol(
    XRpcServer *            server,     /* server instance */
    XRpcString              name)       /* protocol name */
{
    Protocol * p;
    SERVER_WRITE_LOCK(server);
    p = (Protocol*)HASH_Get(&server->protMap, name);
    SERVER_UNLOCK(server);
    return  p;
}

/**
 * XRPC_ListProtocols helper
 */
STATIC Bool XRPC_ListProtocolsCB(HashKey key, HashValue value, void * ctx)
{
    Protocol * protocol = (Protocol*)value;
    ListProtocols * list = (ListProtocols*)ctx;
    return list->cb(protocol, list->context);
}

/**
 * Invokes callback function on each registered protocol
 */
void 
XRPC_ListProtocols(
    XRpcServer *    server,
    ListProtocolCB  cb,
    void *          context)
{
    ListProtocols list;
    list.cb = cb;
    list.context = context;
    SERVER_WRITE_LOCK(server);
    HASH_Examine(&server->protMap, XRPC_ListProtocolsCB, &list);
    SERVER_UNLOCK(server);
}

/* *************************************************************************
//
//  Server 
//
// *************************************************************************/

STATIC void hashFreeProtocol(HashKey key, HashValue value)
{
    PROTOCOL_Free(value);
}

STATIC void scavengeProc(VElement e)
{
    WorkQueue * q = (WorkQueue*)e;
    WKQ_Delete(q);
}

/**
 * Allocates and initializes the server object
 */
XRpcServer *
XRPC_API
XRPC_CreateServer2(
    XRpcString              parser,     /* parser to use (NULL = default) */
    XRpcSessionOpen         sopen,      /* session started */
    XRpcSessionClose        sclose,     /* session finished */
    void *                  context)    /* context for callbacks */
{
    XRpcServer * server = MEM_New(XRpcServer);
    if (server) {
        memset(server, 0, sizeof(*server));
        server->signature = XRPC_SERVER_SIGNATURE;
        server->sopen = sopen;
        server->sclose = sclose;
        server->context = context;
        server->createThread = XRPC_ThreadCreate;
        server->joinThread = XRPC_ThreadJoin;
        server->detachThread = XRPC_ThreadDetach;
        QUEUE_Init(&server->sessions);
        QUEUE_Init(&server->autoThreads);
        HASH_Init(&server->protMap,0,
                  hashCompareStringKey, /* compare function for strings */
                  stringHashProc,       /* hash function for strings */
                  hashFreeProtocol);    /* deallocates Protocol value */

        HASH_Init(&server->tagMap,0,
                  hashCompareStringKey, /* compare function for strings */
                  stringHashProc,       /* hash function for strings */
                  hashFreeValueProc);   /* deallocate the value */

        HASH_Init(&server->callMap,0,
                  hashCompareStringKey, /* compare function for strings */
                  stringHashProc,       /* hash function for strings */
                  NULL);                /* do not deallocate the value */

        server->sessionKey = THREAD_CreateKey(NULL);
        if (server->sessionKey) {        
            /* initialize hashtable of tag handlers */
            if (XRPC_InitTagHandlers(server)) {

                /* get parser */
                server->parser = XPARSE_CreateParser(parser);
                if (server->parser) {
                    if (LOCK_Init(&server->rwlock)) {
                        server->poolMax = 5;
                        VECTOR_Init(&server->workPool,0,NULL,NULL);
                        VECTOR_Init(&server->workActive,0,NULL,NULL);
                        VECTOR_Init(&server->workDead,0,NULL,scavengeProc);
                        if (EVENT_Init(&server->workEvent)) {
                            if (EVENT_Init(&server->sessionFreeEvent)) {

                                /* register the internal "core" protocol */
                                XRPC_RegisterProtocol(server,XCORE_PROTOCOL,
                                                      "XRPC core protocol",
                                                      "Nokia",1,0,
                                                      &XCORE_Protocol,
                                                      server);

                                /* success */
                                return server;
                            }
                            EVENT_Destroy(&server->workEvent);
                        }
                        LOCK_Destroy(&server->rwlock);
                    }
                    XPARSE_FreeParser(server->parser);
                } else {
                    TRACE1("XRPC: unknown parser \"%s\"\n",parser);
                }
            }
            THREAD_DeleteKey(server->sessionKey);
        }
        HASH_Destroy(&server->callMap);
        HASH_Destroy(&server->protMap);
        HASH_Destroy(&server->tagMap);
    }
    return NULL;
}

/**
 * Deallocates dead work thread objects.
 * The caller must hold the lock
 */
STATIC void XRPC_Scavenge(XRpcServer * server)
{
    ASSERT(SERVER_IS_LOCKED(server));
    VECTOR_Clear(&server->workDead);
}

/**
 * Hash table callback for XRPC_CancelPendingCalls
 */
STATIC Bool XRPC_CancelCB(HashKey key, HashValue value, void * ctx)
{
    XRpcCall * call = (XRpcCall*)value;
    XRpcSession * session = (XRpcSession *)ctx;
    if (call->session == session) {
        HASH_Remove(&call->session->server->callMap, key);
        call->status = XRpcStatusSysErr;
        call->result = NULL;
        call->done = XRpcTrue;
        EVENT_Set(&call->event);
    }
    return True;
}

/**
 * Cancels all pending calls. The caller must hold the server lock
 */
STATIC void XRPC_CancelPendingCalls(XRpcSession * session)
{
    ASSERT(SERVER_IS_LOCKED(session->server));
    HASH_Examine(&session->server->callMap, XRPC_CancelCB, session);
}

/**
 * Used by XRPC_FreeServer
 */
STATIC Bool XRPC_KillSessionCB(QEntry * e, void * ctx)
{
    XRpcSession * session = CAST(e,XRpcSession,entry);
    ASSERT(session->server == ctx);
    XRPC_CloseSession(session);
    return True;
}

/**
 * Destroys the server and deallocates the memory. 
 */
void XRPC_API XRPC_FreeServer(XRpcServer * server)
{
    if (server) {
        QEntry* e;
        ASSERT(server->signature == XRPC_SERVER_SIGNATURE);

        SERVER_WRITE_LOCK(server);
        server->shuttingDown = True;
        QUEUE_Examine(&server->sessions, XRPC_KillSessionCB, server);

        /* wait for all calls to complete */
        while (!VECTOR_IsEmpty(&server->workActive)) {
            EVENT_Reset(&server->workEvent);
            SERVER_UNLOCK(server);
            EVENT_Wait(&server->workEvent);
            SERVER_WRITE_LOCK(server);
        }

        /* first free all external sessions */
        e = QUEUE_First(&server->sessions);
        while (e) {
            QEntry* next = QUEUE_Next(e);
            XRpcSession* session = QCAST(e,XRpcSession,entry);
            if (!(session->flags & XRPC_SESSION_INTERNAL)) {
                SERVER_UNLOCK(server);
                XRPC_FreeSession(session);
                SERVER_WRITE_LOCK(server);
            }
            e = next;
        }

        /* then wait until internal sessions deallocate themselves */
        while (!QUEUE_IsEmpty(&server->sessions)) {
            EVENT_Reset(&server->sessionFreeEvent);
            SERVER_UNLOCK(server);
            EVENT_Wait(&server->sessionFreeEvent);
            SERVER_WRITE_LOCK(server);
        }

        /* terminate spawned server threads */
        while (!QUEUE_IsEmpty(&server->autoThreads)) {
            QEntry* e = QUEUE_First(&server->autoThreads);
            XRpcServerThread* spawn = QCAST(e,XRpcServerThread,entry);
            XRpcThread* thread = spawn->thread;
            SOCKET_Close(spawn->socket);
            spawn->socket = INVALID_SOCKET;
            spawn->thread = NULL;
            SERVER_UNLOCK(server);
            server->joinThread(thread);
            SERVER_WRITE_LOCK(server);
        }

        /* drain the pool or work items */
        while (!VECTOR_IsEmpty(&server->workPool)) {
            int i = VECTOR_Size(&server->workPool)-1;
            WorkQueue * q = (WorkQueue*)VECTOR_Remove(&server->workPool,i);
            WKQ_Delete(q);
        }

        XRPC_Scavenge(server);
        SERVER_UNLOCK(server);

        LOCK_Destroy(&server->rwlock);
        EVENT_Destroy(&server->workEvent);
        EVENT_Destroy(&server->sessionFreeEvent);
        VECTOR_Destroy(&server->workPool);
        VECTOR_Destroy(&server->workActive);
        VECTOR_Destroy(&server->workDead);

        if (server->parser) {
            XPARSE_FreeParser(server->parser);
        }

        ASSERT(HASH_IsEmpty(&server->callMap));
        THREAD_DeleteKey(server->sessionKey);
        HASH_Destroy(&server->callMap);
        HASH_Destroy(&server->protMap);
        HASH_Destroy(&server->tagMap);
        MEM_Free(server);
    }
}

/* *************************************************************************
//
//  Dispatcher of incoming calls
//
// *************************************************************************/

/**
 * ListFree routine that deallocates an element
 */
STATIC void listFreeElementContext(VElement e)
{
    if (e) {
        XRpcElementContext * ec = (XRpcElementContext*)e;
        if (ec->close) {
            (*ec->close)(ec);
        } else {
            if (ec->elem) XRPC_FreeElement(ec->elem);
            MEM_Free(ec);
        }
    }
}

/**
 * Input routine for a parser
 */
int XPARSE_API XRPC_ServerRead(void * context, char * buf, int len)
{
    XRpcSession * session = (XRpcSession*)context;
    if (session->flags & XRPC_SESSION_CLOSED) {
        return -1;
    } else if (session->flags & XRPC_SESSION_DONE) {
        XRPC_CloseSession(session);     /* next time will return -1 */
        return 0;
    } else {
        int n = FILE_Read(session->in, buf, len);
#if DEBUG
        if (n > 0) {
            StrBuf64 tmp;
            STRBUF_InitBufXXX(&tmp);
            STRBUF_AppendN(&tmp.sb, buf, n);
            TRACE1("%s",STRBUF_Text(&tmp.sb));
            STRBUF_Destroy(&tmp.sb);
        }
#endif
        return n;
    }
}

STATIC void XRPC_CloseSocketSession(XRpcSession * session, void * context)
{
    XRpcNetSessionData * data = (XRpcNetSessionData*)context;
    SERVER_WRITE_LOCK(session->server);
    shutdown(data->socket,SHUT_RDWR);
    SOCKET_Close(data->socket);
    data->socket = INVALID_SOCKET;
    SERVER_UNLOCK(session->server);
}

STATIC void XRPC_SocketSession(XRpcSession* session, XRpcNetSessionData* data)
{
    /* 
     * Note that by the time we get here the session may have already been 
     * closed by XRPC_CloseSession close
     */
    SERVER_WRITE_LOCK(session->server);
    if (session->flags & XRPC_SESSION_CLOSED) {
        XRPC_CloseSocketSession(session, data);
        SERVER_UNLOCK(session->server);
    } else {
        /* 
         * XRPC_CloseSocketSession will make sure that XRPC_CloseSession
         * will cause XRPC_RunSession to return because of socket error 
         */
        session->closeProc = XRPC_CloseSocketSession;
        session->closeProcContext = data;
        SERVER_UNLOCK(session->server);
        XRPC_RunSession(session);
    }

    Verbose("XRPC: connection with "IPADDR_FORMAT":%hu has terminated\n",
            HOST_IPADDR_FORMAT_ARG(data->ipaddr),data->port);    

    ASSERT(data->socket == INVALID_SOCKET);
    ASSERT(session->flags & XRPC_SESSION_INTERNAL);
    XRPC_FreeSession(session);
}

STATIC void XRPC_ServerSessionThread(XRpcSession * session, void * param)
{
    /* copy the data because by the time when XRPC_SocketSession exits, the
     * slib memory management subsystem may already be deinitialized */
    XRpcNetSessionData data = *((XRpcNetSessionData*)param);
    MEM_Free(param);
    XRPC_SocketSession(session, &data);
}

/**
 * Listens on a socket and accepts one or any number of connections.
 */
XRpcBool
XRPC_API
XRPC_ServerListenSocket(
    XRpcServer *            server,     /* server instance */
    XRpcSocket              socket,     /* TCP socket (not listening) */
    XRpcBool                once)
{
    int err = listen(socket, SOMAXCONN);
    ASSERT(!err);
    if (!err) {
        return XRPC_ServerAccept(server, socket, once);
    }
    return False;
}

/**
 * Accept connections from a listening TCP socket
 */
XRpcBool
XRPC_API
XRPC_ServerAccept(
    XRpcServer *            server,     /* server instance */
    XRpcSocket              socket,     /* TCP socket */
    XRpcBool                once)
{
    int n;
    Bool ok = False;

    /* wait for incoming connection */
    while ((!server->shuttingDown) && 
           (n = SOCKET_WaitRead(socket,500)) >= 0) {

        Socket s;
        Port port;
        IPaddr ipaddr;
        XRpcSession * session;

        struct sockaddr_in addr;
        int addrlen = sizeof(addr);

        /* check for errors */
        if (n & SOCK_WAIT_EXCEPT) {
            Verbose("XRPC: cannot listen (socket error)...\n");
            break;
        }

        /* accept the connection */
        if (!(n & SOCK_WAIT_READ)) {
            continue;
        }

        memset(&addr, 0, addrlen);
        s = accept(socket, (struct sockaddr *)&addr, &addrlen);
        if (s == INVALID_SOCKET) {
            TRACE1("XRPC: failed to accept() connection, error %d\n",
                SOCKET_GetLastError());
            continue;
        }

        /* create client context and spawn new thread */
        ipaddr = ntohl(addr.sin_addr.s_addr);
        port = ntohs(addr.sin_port);
        Verbose("XRPC: accepting connection from "IPADDR_FORMAT":%hu\n",
                HOST_IPADDR_FORMAT_ARG(ipaddr), port);    

        session = XRPC_CreateSocketSession(server,s);
        if (session) {
            XRpcNetSessionData * data = MEM_New(XRpcNetSessionData);
            if (data) {
                data->socket = s;
                data->ipaddr = ipaddr;
                data->port = port;
                ok = True;
                session->flags |= XRPC_SESSION_INTERNAL;
                if (once) {
                    /* run session on the same thread */
                    XRPC_ServerSessionThread(session,data);
                    break;
                } else {
                    /* create a separate thread for this session */
                    if (XRPC_CreateSessionThread(session,
                        XRPC_ServerSessionThread,data)) {
                        continue;
                    }
                }
                MEM_Free(data);
            }
            XRPC_FreeSession(session);
        }
        SOCKET_Close(s);
    }
    return ok;
}

XRpcBool
XRPC_API
XRPC_ServerListen(
    XRpcServer *            server,     /* server instance */
    XRpcPort                port,       /* TCP port */
    XRpcBool                once)
{
    Bool ok = False;
    Socket socket;
    if (SOCKET_GetTcp(port, &socket)) {
        Verbose("XRPC: listening on port %hu\n",port);
        ok = XRPC_ServerListenSocket(server, socket, once);
        SOCKET_Close(socket);
    } else {
        TRACE2("XRPC: cannot listen on port %hu, error %d\n",
               port, SOCKET_GetLastError());
    }
    return ok;
}

/**
 * Runs on the thread created by XRPC_ServerSpawn
 */
STATIC void XRPC_ServerThread(void * param)
{
    /* copy parameters from the context and set the event */
    XRpcServerSpawn * context = param;
    XRpcServerThread* spawn = context->spawn;
    XRpcServer * server = context->server;
    ASSERT(spawn->entry.queue == &server->autoThreads);
    EVENT_Set(&context->event);

    /* run the accept loop */
    TRACE("XRPC: autocreated server thread started\n");
    XRPC_ServerAccept(server, spawn->socket, False);
    SERVER_WRITE_LOCK(server);
    if (spawn->socket != INVALID_SOCKET) {
        SOCKET_Close(spawn->socket);
        spawn->socket = INVALID_SOCKET;
    }
    if (spawn->thread) {
        server->detachThread(spawn->thread);
    }
    QUEUE_RemoveEntry(&spawn->entry);
    MEM_Free(spawn);
    SERVER_UNLOCK(server);
    TRACE("XRPC: autocreated server thread exiting\n");
}

/**
 * Spawns the server in a separate thread. Returns the actual listening port
 * (useful if the input parameter was zero), zero on failure
 */
XRpcPort
XRPC_API
XRPC_ServerSpawn(
    XRpcServer *            server,     /* server instance */
    XRpcPort                port)       /* listening TCP port (0 for any) */
{
    XRpcServerThread* spawn = MEM_New(XRpcServerThread);
    /* XRpcServerThread will be deallocated by XRPC_ServerThread */
    if (spawn) {
        XRpcServerSpawn context;
        memset(spawn, 0, sizeof(*spawn));
        memset(&context, 0, sizeof(context));
        if (EVENT_Init(&context.event)) {
            context.server = server;
            context.spawn = spawn;
            if (SOCKET_GetTcp(port, &spawn->socket)) {
                XRpcPort serverPort;
                struct sockaddr_in sa;
                int err, namelen = sizeof(sa);
                getsockname(spawn->socket,(struct sockaddr*)&sa, &namelen);
                serverPort = htons(sa.sin_port);
                err = listen(spawn->socket, SOMAXCONN);
                ASSERT(!err);
                if (!err) {
                    Verbose("XRPC: listening on port %hu\n",serverPort);
                    SERVER_WRITE_LOCK(server);
                    QUEUE_InsertTail(&server->autoThreads, &spawn->entry);
                    spawn->thread = server->createThread(XRPC_ServerThread,
                        &context);
                    if (spawn->thread) {
                        /* wait until the thread actually starts */
                        SERVER_UNLOCK(server);
                        EVENT_Wait(&context.event);
                        EVENT_Destroy(&context.event);
                        return serverPort;
                    }
                    QUEUE_RemoveEntry(&spawn->entry);
                    SERVER_UNLOCK(server);
                } else {
                    TRACE2("XRPC: cannot listen on port %hu, error %d\n",
                           serverPort, SOCKET_GetLastError());
                }
                SOCKET_Close(spawn->socket);
            }
            EVENT_Destroy(&context.event);
        }
        MEM_Free(spawn);
    }
    return 0;
}

/**
 * Changes the mechanism of creating session threads. The primary purpose 
 * of this is to support emulators which often require that the threads are
 * created using emulator-specific API.
 */
void
XRPC_API
XRPC_ServerSetThreadAPI(
    XRpcServer *            server,     /* server instance */
    XRpcThreadCreate        create,     /* thread creation routine */
    XRpcThreadJoin          join,       /* waits until thread is done */
    XRpcThreadDetach        detach)     /* makes thread free-running */
{
    ASSERT(create);
    ASSERT(join);
    ASSERT(detach);
    if (create && join && detach) {
        SERVER_WRITE_LOCK(server);
        server->createThread = create;
        server->joinThread = join;
        server->detachThread = detach;
        SERVER_UNLOCK(server);
    }
}

/**
 * Resets the server to use default thread model
 */
void
XRPC_API
XRPC_ServerResetThreadAPI(
    XRpcServer *            server)     /* server instance */
{
    XRPC_ServerSetThreadAPI(server, 
        XRPC_ThreadCreate, 
        XRPC_ThreadJoin, 
        XRPC_ThreadDetach);
}

/* *************************************************************************
//
//  Session object
//
// *************************************************************************/

XRpcSession *
XRPC_API
XRPC_CreateSession(
    XRpcServer *    server,     /* server instance */
    File *          in,         /* input stream */
    File *          out)        /* output stream */
{
    ASSERT(in);
    ASSERT(out);
    if (in && out) {
        XRpcServer * srv = NULL;
        if (!server) {
            server = srv = XRPC_CreateServer(NULL);
        }
        if (server && !server->shuttingDown) {
            XRpcSession * session = MEM_New(XRpcSession);
            if (session) {
                memset(session, 0, sizeof(*session));
                if (srv) session->flags |= XRPC_SESSION_FREE_SERVER;
                QUEUE_Init(&session->calls);
                VECTOR_Init(&session->stack, 0, NULL, listFreeElementContext);
                if (EVENT_Init(&session->callEvent)) {

                    /* test connection by writing the root tag */
                    if (FILE_Puts(out,XML_HEADER)    > 0 && 
                        FILE_Puts(out,"<")           > 0 &&
                        FILE_Puts(out,XRPC_TAG_ROOT) > 0 &&
                        FILE_Puts(out,">\n")         > 0) {

                        session->client = XRPC_CreateClient(session, out);
                        if (session->client) {
                            Bool ok = False;

                            session->in = in;
                            session->out = out;
                            session->server = server;

                            /* enforce separation of input and output */
                            FILE_ClrFlag(in, FILE_CAN_WRITE);
                            FILE_ClrFlag(out, FILE_CAN_READ);

                            SERVER_WRITE_LOCK(server);
                            if (!server->shuttingDown) {
                                ok = True;
                                QUEUE_InsertTail(&server->sessions,
                                    &session->entry);
                            }
                            SERVER_UNLOCK(server);

                            if (ok) {
                                return session;
                            }

                            XRPC_FreeClient(session->client);
                        }
                    } else {
                        TRACE("XRPC: cannot write XML header!\n");
                    }
                    EVENT_Destroy(&session->callEvent);
                }
                VECTOR_Destroy(&session->stack);
                MEM_Free(session);
            }
        }
        XRPC_FreeServer(srv);
    }

    return NULL;
}

/**
 * Default session proc. This function is used if second parameter of 
 * XRPC_CreateSessionThread is NULL. The context parameter is ignored.
 */
STATIC void XRPC_DefaultSessionProc(XRpcSession * session, void * context)
{
    TRACE("XRPC: session thread is running\n");
    XRPC_RunSession(session);
    TRACE("XRPC: session thread is exiting\n");
}

/**
 * Start function for the thread created by XRPC_CreateSessionThread 
 */
STATIC void XRPC_SessionThread(void * param)
{
    XRpcSessionThread * data = (XRpcSessionThread*)param;
    data->proc(data->session, data->context);
    MEM_Free(data);
}

/*
 * The right way for client to create a session thread is to call 
 * XRPC_CreateSessionThread. It's not a good idea to create session
 * threads in any other way, because it's impossible to synchronize 
 * this operation with XRPC, and you may end up having two session 
 * threads. The results are not very predictable.
 *
 * If handler is not specified, the default handler will be used
 * which simply calls XRPC_RunSession.
 *
 * This function uses default mechanism of creating and destroying threads.
 * Use XRPC_CreateSessionThread2 if the default mechanism does not work
 * for you.
 *
 * Returns True if session thrad has been created, False on failure
 */
XRpcBool 
XRPC_API 
XRPC_CreateSessionThread(
    XRpcSession *           session,    /* the session */
    XRpcSessionProc         proc,       /* session proc, may be NULL */
    void *                  context)    /* extra parameter */
{
    XRpcServer * server = session->server;
    return XRPC_CreateSessionThread2(session, proc, context, 
        server->createThread, server->joinThread, server->detachThread);
}

/*
 * The right way for client to create a session thread is to call 
 * XRPC_CreateSessionThread. It's not a good idea to create session
 * threads in any other way, because it's impossible to synchronize 
 * this operation with XRPC, and you may end up having two session 
 * threads. The results are not very predictable.
 *
 * If handler is not specified, the default handler will be used
 * which simply calls XRPC_RunSession.
 *
 * Returns True if session thrad has been created, False on failure
 */
XRpcBool 
XRPC_API 
XRPC_CreateSessionThread2(
    XRpcSession *           session,    /* the session */
    XRpcSessionProc         proc,       /* session proc, may be NULL */
    void *                  context,    /* extra parameter */
    XRpcThreadCreate        create,     /* thread creation routine */
    XRpcThreadJoin          join,       /* waits until thread is done */
    XRpcThreadDetach        detach)     /* makes thread free-running */
{
    XRpcBool ok = XRpcFalse;
    XRpcServer * server = session->server;
    ASSERT(create);
    ASSERT(join);
    ASSERT(detach);
    if (create && join && detach) {
        SERVER_WRITE_LOCK(server);
        if (!session->thread) {
            XRpcSessionThread * data = MEM_New(XRpcSessionThread);
            if (data) {
                memset(data, 0, sizeof(*data));
                data->session = session;
                if (proc) {
                    data->proc = proc;
                    data->context = context;
                } else {
                    data->proc = XRPC_DefaultSessionProc;
                }
                session->thread = create(XRPC_SessionThread, data);
                if (session->thread) {
                    session->joinThread = join;
                    session->detachThread = detach;
                    session->flags |= XRPC_SESSION_THREAD;
                    ok = XRpcTrue;
                } else {
                    MEM_Free(data);
                }
            }
        }
        SERVER_UNLOCK(server);
    }
    return ok;
}

/**
 * Indicates that an external (not created by XRPC runtime) thread will be 
 * used for running the session. That is, the the code that is using XRP
 * library will call XRPC_RunSession for this session on the its own thread
 * (not necessarily the current thread)
 */
void
XRPC_API 
XRPC_UseExternalThread(
    XRpcSession *           session)    /* the session */
{
    SERVER_WRITE_LOCK(session->server);
    ASSERT(!session->thread);
    if (!session->thread) {
        session->thread = XRPC_CLIENT_THREAD;
    }
    SERVER_UNLOCK(session->server);
}

/**
 * Creates a session thread. See comments for XRPC_SessionAutoThread
 */
XRpcClient * XRPC_API XRPC_GetClient(XRpcSession * session)
{
    return session->client;
}

XRpcServer * XRPC_API XRPC_GetServer(XRpcSession * session)
{
    return session->server;
}

STATIC Bool XRPC_SessionOpenCB(HashKey key, HashValue value, void * ctx)
{
    XRpcSession * session = (XRpcSession*)ctx;
    Protocol * protocol = (Protocol*)value;
    if (protocol->protocol.sopen) {
        protocol->protocol.sopen(protocol->context, session);
    }
    return True;
}

STATIC Bool XRPC_SessionCloseCB(HashKey key, HashValue value, void * ctx)
{
    XRpcSession * session = (XRpcSession*)ctx;
    Protocol * protocol = (Protocol*)value;
    if (protocol->protocol.sclose) {
        protocol->protocol.sclose(protocol->context, session);
    }
    return True;
}

void
XRPC_API
XRPC_RunSession(
    XRpcSession * session)
{
    XRpcServer * server = session->server;
    XParseInput input;
    XParseHandlers handlers;

    /* input methods */
    memset(&input, 0, sizeof(input));
    input.read = XRPC_ServerRead;

    /* handlers */
    memset(&handlers, 0, sizeof(handlers));
    handlers.startElement  = XRPC_HandleStartElement;
    handlers.endElement    = XRPC_HandleEndElement;
    handlers.characterData = XRPC_HandleCharacterData;

    /* register this session with the server */
    SERVER_WRITE_LOCK(server);
    ASSERT(THREAD_GetValue(server->sessionKey) == session->thread ||
           THREAD_GetValue(server->sessionKey) == NULL);
    THREAD_SetValue(server->sessionKey, session);
    if (server->sopen) server->sopen(server->context, session);
    HASH_Examine(&server->protMap, XRPC_SessionOpenCB, session);
    SERVER_UNLOCK(server);

    /* invoke the parser */
    server->parser->parse(server->parser, &input, &handlers, session);

    while (VECTOR_Size(&session->stack) > 0) {
        /* must remove elements in reverse order */
        VECTOR_Remove(&session->stack, VECTOR_Size(&session->stack) - 1);
    }

    /* cleanup */
    SERVER_WRITE_LOCK(server);
    XRPC_CloseSession(session);
    HASH_Examine(&server->protMap, XRPC_SessionCloseCB, session);
    if (server->sclose) server->sclose(server->context, session);
    THREAD_SetValue(server->sessionKey, NULL);
    XRPC_CancelPendingCalls(session);
    if ((session->flags & XRPC_SESSION_THREAD) && 
       !(session->flags & XRPC_SESSION_THREAD_JOIN)) {
        ASSERT(session->thread != XRPC_CLIENT_THREAD);
        if (session->thread != XRPC_CLIENT_THREAD) {
            session->detachThread(session->thread);
        }
        session->thread = NULL;
    }
    SERVER_UNLOCK(server);
}

/**
 * Creates a session that uses C runtime FILE objects for input/output
 */
XRpcSession *
XRPC_API
XRPC_CreateStreamSession(
    XRpcServer *    server,     /* server instance */
    FILE *          fin,        /* input stream */
    FILE *          fout)       /* output stream */
{
    File * in = FILE_AttachToFile(fin, "input");
    if (in) {
        File * out = FILE_AttachToFile(fout, "output");
        if (out) {
            XRpcSession * session = XRPC_CreateSession(server, in, out);
            if (session) {

                /* XRPC_FreeSession() will destroy the File objects */
                return session;
            }
            FILE_Detach(out);
            FILE_Close(out);
        }
        FILE_Detach(in);
        FILE_Close(in);
    }
    return NULL;
}

/**
 * Creates a session that uses a socket for input/output
 */
XRpcSession *
XRPC_API
XRPC_CreateSocketSession(
    XRpcServer *    server,     /* server instance */
    int             socket)     /* connected TCP socket */
{
    File * in = FILE_AttachToSocket(socket);
    if (in) {
        File * out = FILE_AttachToSocket(socket);
        if (out) {
            XRpcSession * session = XRPC_CreateSession(server, in, out);
            if (session) {

                /* XRPC_FreeSession() will destory the File objects */
                return session;
            }
            FILE_Detach(out);
            FILE_Close(out);
        }
        FILE_Detach(in);
        FILE_Close(in);
    }
    return NULL;
}

/**
 * Connects to XRPC server and establishes a session. Only port parameter
 * must be non-zero, all other parameters may be NULL.
 */
XRpcSession *
XRPC_API
XRPC_Connect(
    XRpcServer *            server,     /* server instance (may be NULL) */
    XRpcString              host,       /* host name (NULL for localhost) */
    XRpcPort                port)       /* TCP port (native byte order) */
{
    XRpcSession * session = NULL;
    IPaddr ipaddr = 0;
    if (host && host[0]) {
        if (!INET_ResolveAddr(host, &ipaddr)) {
            TRACE1("XRPC: cannot resolve host name %s\n",host);
        }
    } else {
        ipaddr = HOST_IPADDR(127,0,0,1);
    }
    if (ipaddr) {
        XRpcSocket socket;
        if (SOCKET_GetTcp(0,&socket)) {
            TRACE2("XRPC: connecting to %s:%hu\n",host?host:"localhost",port);
            if (SOCKET_Connect(socket,ipaddr,port)) {
                session = XRPC_CreateSocketSession(server, socket);
                if (session) {
                    /* session will close the socket when done */
                    SERVER_WRITE_LOCK(session->server);
                    session->flags |= XRPC_SESSION_OWN_INPUT;
                    SERVER_UNLOCK(session->server);
                    return session;
                }
                shutdown(socket,SHUT_RDWR);
            }
            SOCKET_Close(socket);
        }
    }
    return NULL;
}

/**
 * Returns the session associated with the current thread. 
 * Returns NULL if called from outside of XRPC call context
 */
XRpcSession * XRPC_API XRPC_GetCurrentSession(XRpcServer * server)
{
    return (XRpcSession*)THREAD_GetValue(server->sessionKey);
}

/**
 * Associates opaque context with the session. Returns the old context.
 * This call is not synchronized, and shouldn't be invoked on multiple
 * threads. Typical use - call it from the "session created" callback.
 */
void *
XRPC_API
XRPC_SetSessionContext(
    XRpcSession *           session,    /* the session */
    void *                  context)    /* associated context */
{
    void * old = session->opaque;
    session->opaque = context;
    return old;
}

/**
 * Returns the session context previously associated with this session
 * using XRPC_SetSessionContext call. 
 */
void * XRPC_API XRPC_GetSessionContext(XRpcSession * session)
{
    return session->opaque;
}

/**
 * Mark session as closed
 */
void XRPC_API XRPC_CloseSession(XRpcSession * session)
{
    SERVER_WRITE_LOCK(session->server);
    if (!(session->flags & XRPC_SESSION_CLOSED)) {
        session->flags |= XRPC_SESSION_CLOSED;
        XRPC_CloseTag(session->client, XRPC_TAG_ROOT);
        if (session->closeProc) {
            session->closeProc(session, session->closeProcContext);
        }
    }
    SERVER_UNLOCK(session->server);
}

/**
 * Deallocates the session. 
 */
void XRPC_API XRPC_FreeSession(XRpcSession * session)
{
    if (session) {
        XRpcThread * thread = NULL;
        XRpcServer* server = session->server;

        /* mark the session as closed */
        XRPC_CloseSession(session);

        /* wait for all calls to complete */
        SERVER_WRITE_LOCK(server);
        while (!QUEUE_IsEmpty(&session->calls)) {
            EVENT_Reset(&session->callEvent);
            SERVER_UNLOCK(server);
            EVENT_Wait(&session->callEvent);
            SERVER_WRITE_LOCK(server);
        }

        if (session->thread && (session->flags & XRPC_SESSION_THREAD))  {
            session->flags |= XRPC_SESSION_THREAD_JOIN;
            thread = session->thread;
        }
        SERVER_UNLOCK(server);

        if (thread) {
            ASSERT(thread != XRPC_CLIENT_THREAD);
            if (thread != XRPC_CLIENT_THREAD) {
                session->joinThread(thread);
            }
        }

        XRPC_FreeClient(session->client);
        if (session->flags & XRPC_SESSION_FREE_SERVER) {
            SERVER_WRITE_LOCK(server);
            QUEUE_RemoveEntry(&session->entry);
            SERVER_UNLOCK(server);
            XRPC_FreeServer(server);
            server = NULL;
        }

        if (session->out) {
            if (!(session->flags & XRPC_SESSION_OWN_OUTPUT)) {
                FILE_Detach(session->out);
            }
            FILE_Close(session->out);
        }
        if (session->in) {
            if (!(session->flags & XRPC_SESSION_OWN_INPUT)) {
                FILE_Detach(session->in);
            }
            FILE_Close(session->in);        
        }

        EVENT_Destroy(&session->callEvent);
        VECTOR_Destroy(&session->stack);

        if (server) {
            SERVER_WRITE_LOCK(server);
            QUEUE_RemoveEntry(&session->entry);
            MEM_Free(session);
            EVENT_Set(&server->sessionFreeEvent);
            SERVER_UNLOCK(server);
        } else {
            MEM_Free(session);
        }
    }
}

/* *************************************************************************
//
//  Call context
//
// *************************************************************************/

/**
 * Sets up a context for a call
 */
XRpcCall * XRPC_CreateCall(XRpcSession * session)
{
    XRpcCall * call = MEM_New(XRpcCall);
    if (call) {
        char id[32];
        XRpcServer * server = session->server;
        SERVER_WRITE_LOCK(server);
        if (!(session->flags & XRPC_SESSION_CLOSED)) {

            /* automatically create the parsing thread */
            if (!session->thread) {
                XRPC_CreateSessionThread(session, NULL, NULL);
            }

            if (session->thread) {
                /* initialize the call context */
                memset(call, 0, sizeof(*call));
                call->status = XRpcStatusInProgress;
                server->lastId++;
                sprintf(id,LONG_ULONG_FORMAT,server->lastId);
                call->id = STRING_Dup(id);
                if (call->id) {
                    if (EVENT_Init(&call->event)) {
                        if (HASH_Put(&server->callMap,call->id,call)) {
                            call->session = session;
                            SERVER_UNLOCK(server);
                            return call;
                        }
                        EVENT_Destroy(&call->event);
                    }
                    MEM_Free(call->id);
                }
            }
        }
        MEM_Free(call);
        SERVER_UNLOCK(server);
    }
    return NULL;
}

/**
 * Waits for the results of the pending XRPC call. Does not return until
 * the call is complete
 */
void XRPC_API XRPC_Wait(XRpcCall * call)
{
    ASSERT(call);
    if (call) {
        TRACE1("XRPC: waiting for call %s to complete\n", call->id);
        EVENT_Wait(&call->event);
        ASSERT(call->done);
        TRACE1("XRPC: call %s is done\n", call->id);
    }
}

/**
 * Waits for the results of the pending XRPC call. 
 * Timeout is given in milliseconds.
 * Returns True if call completes, False if the call is still pending
 */
XRpcBool
XRPC_API
XRPC_TimeWait(
    XRpcCall * call,
    XRpcInt timeout)
{
    ASSERT(call);
    if (call) {
        if (timeout > 0) {
            WaitState state;
            TRACE1("XRPC: waiting for call %s to complete\n", call->id);
            state = EVENT_TimeWait(&call->event, timeout);
            if (state == WAIT_STATE_ERROR) {
                call->status = XRpcStatusSysErr;
            }
        }
        TRACE2("XRPC: call %s is %s\n",call->id,call->done?"done":"pending");
        return call->done;
    } else {
        return XRpcFalse;
    }
}
 
/**
 * Returns call id
 */
XRpcString XRPC_GetID(XRpcCall * call) 
{
    return call->id;
}
 
/**
 * Returns the status of the completed call, or XRpcStatusInvalid if the
 * call is still in progress.
 */
XRpcStatus XRPC_API XRPC_CallStatus(XRpcCall * call)
{
    return call->status;
}

/**
 * Returns the result of the completed call, or NULL if the call is still 
 * in progress.
 */
const XRpcElement * XRPC_API XRPC_CallResult(XRpcCall * call)
{
    return call->result;
}

/**
 * Deallocates the call context
 */
void XRPC_API XRPC_FreeCall(XRpcCall * call)
{
    if (call) {

        /* the caller should not free pending calls */
        ASSERT(call->done);
        EVENT_Wait(&call->event);

        /* deallocate the call context */
        XRPC_FreeElement(call->result);
        EVENT_Destroy(&call->event);
        MEM_Free(call->id);
        MEM_Free(call);
    }
}

/* *************************************************************************
//
//  Event handlers
//
// *************************************************************************/

/**
 * Adds element to the parent container
 */
STATIC
void
XRPC_AddToParentContainer(
    XRpcSession * session,          /* session */
    XRpcElement * elem)             /* element */
{
    Vector * stack = &session->stack;
    int n = VECTOR_Size(stack);
    if (n > 0) {
        XRpcElementContext * c = (XRpcElementContext*)VECTOR_Get(stack,n-1);
        if (c && c->elem) {
            XRpcContainer * parent = _XRPC_IsContainer(c->elem);
            if (parent) {
                XRPC_AddElement(parent, elem);
            }
        }
    }
}

/* generic handlers */

STATIC void XRPC_DefaultElementClose(XRpcElementContext * context)
{
    if (context) {
        if (context->elem) {

            /* 
             * delete the element only if it has no parent. Those that 
             * have a parent will be deallocated by the container.
             */

            if (!XRPC_GetParent(context->elem)) {
                XRPC_FreeElement(context->elem);
            }
        }
        MEM_Free(context);
    }
}

/**
 * Close handler for a data element
 */
STATIC void XRPC_DataElementClose(XRpcElementContext * context)
{
    XRpcDataElementContext * de = CAST(context,XRpcDataElementContext,common);
    ASSERT(de->signature == DATA_CONTEXT_SIGNATURE);
    if (de->signature == DATA_CONTEXT_SIGNATURE) {
        XRpcStringUCS2 ws = de->buf;
        XRpcDataElement * elem = CAST(de->common.elem,XRpcDataElement,element);
        if (elem) {
            if (ws) {
                if (!XRPC_SetAsText(elem, de->buf)) {
                    TRACE2("XRPC: conversion to %s failed: %s",
                        XRPC_GetTypeName(de->common.elem->dataType),de->buf);
                }

            /* if we didn't get any text, replace this element with null */
            } else if (elem->element.parent) {
                XRpcContainer * parent = elem->element.parent;
                int index = XRPC_GetElementIndex(parent, &elem->element);
                XRpcNullElement * null = XRPC_CreateNullElement();
                XRPC_CopyAttributes(&null->element, &elem->element);
                XRPC_RemoveElementAt(parent, index);
                XRPC_InsertElement(parent, &null->element, index);
            }
        }

        /* deallocate element context */
        if (de->buf) MEM_Free(de->buf);
        MEM_Free(de);
    }
}

/**
 * Character data handler for XRpcDataElementContext. Appends data to the 
 * buffer
 */
STATIC void XRPC_DataElementHandleData(
    XRpcElementContext * context,   /* context */
    const XRpcUcs2 * wc,            /* the characters */
    size_t len)                     /* number of charactars */
{
    XRpcDataElementContext * de = CAST(context,XRpcDataElementContext,common);
    ASSERT(de->signature == DATA_CONTEXT_SIGNATURE);
    if (de->signature == DATA_CONTEXT_SIGNATURE && wc && len > 0) {
        if (de->len + len >= de->alloc) {
            size_t count = MAX(2*de->alloc, de->len + len + 1);
            XRpcUcs2 * buf = MEM_Realloc(de->buf, sizeof(wc[0]) * count);
            if (buf) {
                de->buf = buf;
                de->alloc = count;
            } else {
                return;
            }
        }
        memcpy(de->buf + de->len, wc, len * sizeof(wc[0]));
        de->len += len;
        de->buf[de->len] = 0;
        ASSERT(UCS2_Len(de->buf) == (int)de->len);
    }
}

/**
 * Creates data element context. If memory allocation fails, deallocates 
 * the element. This context accumulates character data.
 */
STATIC 
XRpcElementContext * 
XRPC_CreateDataElementContext(
    XRpcSession * session,
    XRpcElement * elem,
    const XRpcAtts atts)
{
    if (elem) {
        if (XRPC_PutAttributes(elem, atts)) {
            XRpcDataElementContext * data = MEM_New(XRpcDataElementContext);
            if (data) {
                XRPC_AddToParentContainer(session, elem);
                memset(data, 0, sizeof(*data));
                data->signature = DATA_CONTEXT_SIGNATURE;
                data->common.session = session;
                data->common.elem = elem;
                data->common.tag = elem->tag;
                data->common.close = XRPC_DataElementClose;
                data->common.handler = XRPC_DataElementHandleData;
                return &data->common;
            } 
        }
        XRPC_FreeElement(elem);
    }
    return NULL;
}

STATIC
XRpcElementContext *
XRPC_DefaultContainerHandler(
    XRpcSession * session,          /* session */
    XRpcTag tag,                    /* the tag */
    const XRpcAtts atts)            /* attributes */
{
    XRpcElementContext * ec = MEM_New(XRpcElementContext);
    if (ec) {
        XRpcContainer * container = XRPC_CreateContainer(tag);
        if (container) {
            XRPC_PutAttributes(&container->element, atts);
            XRPC_AddToParentContainer(session, &container->element);
            memset(ec, 0, sizeof(*ec));
            ec->session = session;
            ec->elem = &container->element;
            ec->tag = tag;
            ec->close = XRPC_DefaultElementClose;
            return ec;
        }
        MEM_Free(ec);
    }
    return NULL;
}

STATIC 
void 
XRPC_ProblemElementClose(
    XRpcElementContext * context) 
{
    ASSERT(!context->elem);

    /* decrement problem count */
    ASSERT(context->session->problem > 0);
    if (context->session->problem > 0) {
        context->session->problem--;
    }

    MEM_Free(context);
}

STATIC
XRpcElementContext *
XRPC_ProblemElementHandler(
    XRpcSession * session,          /* session */
    XRpcTag tag,                    /* the tag */
    const XRpcAtts atts)            /* attributes */
{
    XRpcElementContext * ec = MEM_New(XRpcElementContext);
    if (ec) {
        memset(ec, 0, sizeof(*ec));
        ec->tag = tag;
        ec->session = session;
        ec->close = XRPC_ProblemElementClose;

        /* increment problem count */
        session->problem++;
        return ec;
    }
    return NULL;
}

/* *************************************************************************
//
//  SAX event handlers
//
// *************************************************************************/

/**
 * SAX style "startElement" handler
 */
STATIC
void 
XPARSE_API 
XRPC_HandleStartElement(
    void * context,             /* XRPC defined context */
    const char * tag,           /* the element name */
    const XRpcAtts atts)        /* attributes */
{
    XRpcElementContext * ec = NULL;
    XRpcSession * session = (XRpcSession*)context;
    Vector * stack = &session->stack;
    ElementInfo * info = (ElementInfo*)HASH_Get(&session->server->tagMap,tag);

    ASSERT(session->server->signature == XRPC_SERVER_SIGNATURE);
    ASSERT_INPUT_THREAD(session);

    /* is there a problem ? */

    if (session->problem) {
        
        XRpcTag t = (info ? info->tag : XRpcTagUnknown);
        ec = XRPC_ProblemElementHandler(session, t, atts);

    } else {

        /* no problem, invoke regular handler */

        if (info) {
            int n = VECTOR_Size(stack);
            XRpcTag parent = XRpcTagNone;
            if (n > 0) {
                XRpcElementContext * c = (XRpcElementContext*)VECTOR_Get(stack,n-1);
                if (c) {
                    ASSERT(c->tag);
                    parent = c->tag;
                }
            }

            if (!XRPC_IsValidChild(parent, info->tag)) {
                TRACE2("XRPC: <%s> is not a valid child of <%s>\n",
                    XRPC_GetTagName(info->tag), XRPC_GetTagName(parent));

                /* oh, wait, there is a problem */

                ec = XRPC_ProblemElementHandler(session, info->tag, atts);

            } else {    
                ec = (*info->handler)(session, info->tag, atts);
            }
        } else {
            ec = XRPC_DefaultContainerHandler(session,XRpcTagUnknown,atts);
        }
    }

    /* context field must point to the session context */
    ASSERT(ec && ec->session == session);

    /* tag field must be set and must match the element if such exists */
    ASSERT(!ec || !info || ec->tag);
    ASSERT(!ec || !ec->elem || ec->elem->tag == ec->tag);

    /* just in case, fix these things if handler screwed up */
    ec->session = session;
    if (!VECTOR_Add(stack, ec)) {
        listFreeElementContext(ec);
    }
}

/**
 * SAX style "endElement" handler
 */
STATIC
void 
XPARSE_API 
XRPC_HandleEndElement(
    void * context,             /* XRPC defined context */
    const char * name)          /* the element name */
{
    int size;
    XRpcSession * session = (XRpcSession*)context;
    
    ASSERT(session->server->signature == XRPC_SERVER_SIGNATURE);
    ASSERT_INPUT_THREAD(session);

    size = VECTOR_Size(&session->stack);
    ASSERT(size > 0);
    if (size > 0) {
        XRpcElementContext * ec = NULL;

        ec = (XRpcElementContext*)VECTOR_Get(&session->stack, size-1);
        if (ec) {
            XRpcString elemTag = NULL;
            if (ec->elem) elemTag = XRPC_GetElementTagName(ec->elem);

            ASSERT(!elemTag || (elemTag && strcmp(name, elemTag) == 0));
            if (!elemTag || (elemTag && strcmp(name, elemTag) == 0)) {

                /*
                 * VECTOR_Remove() call will do the Right Thing (TM)
                 * Its VectorFree function (listFreeElementContext) will 
                 * call the end tag handler which is supposed to deallocate
                 * the element context
                 */
                VECTOR_Remove(&session->stack, size-1);
            }
        } else {
            VECTOR_Remove(&session->stack, size-1);
        }
    }
}

/**
 * SAX style "characterData" handler
 */
STATIC
void 
XPARSE_API 
XRPC_HandleCharacterData(
    void * context,             /* XRPC defined context */
    const XRpcUcs2 * chars,     /* the characters */
    int len)                    /* number of charactars */
{
    int size;
    XRpcSession * session = (XRpcSession*)context;
    
    ASSERT(session->server->signature == XRPC_SERVER_SIGNATURE);
    ASSERT_INPUT_THREAD(session);

    size = VECTOR_Size(&session->stack);
    ASSERT(size > 0);
    if (size > 0) {
        XRpcElementContext * ec = NULL;
        ec = (XRpcElementContext*)VECTOR_Get(&session->stack, size-1);
        if (ec) {
            if (ec->handler) {
                (*ec->handler)(ec, chars, len);
            }
        }
    }
}

/* *************************************************************************
//
//  Root element handler
//
// *************************************************************************/

STATIC void XRPC_RootElementClose(XRpcElementContext * context)
{
    TRACE("XRPC: root element closed\n");
    context->session->flags |= XRPC_SESSION_DONE;
    XRPC_DefaultElementClose(context);
}

STATIC
XRpcElementContext *
XRPC_HandleRoot(
    XRpcSession * session,          /* session */
    XRpcTag tag,                    /* the tag */
    const XRpcAtts atts)            /* attributes */
{
    XRpcElementContext * ec = MEM_New(XRpcElementContext);
    TRACE("XRPC: root element opened\n");
    if (ec) {
        memset(ec, 0, sizeof(*ec));
        ec->tag = tag;
        ec->session = session;
        ec->close = XRPC_RootElementClose;
        return ec;
    }
    return NULL;
}

/* *************************************************************************
//
//  <call> tag 
//
// *************************************************************************/

/**
 * Returns a WorkQueue to the pool or deallocates it if pool already
 * contains poolMax elements
 */
STATIC void XRPC_ReturnWorkQueue(XRpcServer * server, WorkQueue * q)
{
    if (q) {
        Bool pooled = False;
        int pos;
        SERVER_WRITE_LOCK(server);
        pos = VECTOR_IndexOf(&server->workActive,q);
        ASSERT(pos >= 0);
        VECTOR_Remove(&server->workActive,pos);
        if (VECTOR_Size(&server->workPool) < server->poolMax) {
            if (VECTOR_Size(&server->workPool) < server->poolMax) {
                pooled = VECTOR_Add(&server->workPool,q);
            }
        }
        if (!pooled) {
            WKQ_Stop(q, False);
            VECTOR_Add(&server->workDead, q);
        }
        EVENT_Set(&server->workEvent);
        SERVER_UNLOCK(server);
    }
}

/**
 * Allocates a new WorkQueue or gets it from the pool
 */
STATIC WorkQueue * XRPC_GetWorkQueue(XRpcServer * server)
{
    Bool locked = False;
    WorkQueue * q = NULL;
    if (!VECTOR_IsEmpty(&server->workPool)) {
        locked = SERVER_WRITE_LOCK(server);
        if (locked) {
            if (!VECTOR_IsEmpty(&server->workPool)) {
                int last = VECTOR_Size(&server->workPool)-1;
                q = (WorkQueue*)VECTOR_Remove(&server->workPool,last);
            }
        }
    }
    if (!q) {
        if (!locked) locked = SERVER_WRITE_LOCK(server);
        if (locked) {
            q = WKQ_Create();
        }
    }
    if (q) {
        if (!VECTOR_Add(&server->workActive, q)) {
            XRPC_ReturnWorkQueue(server, q);
            q = NULL;
        }
    }
    if (locked) {
        XRPC_Scavenge(server);
        SERVER_UNLOCK(server);
    }
    return q;
}

/**
 * This functions actually invokes the function handler. Runs on its own 
 * thread.
 */
STATIC void XRpcCallWorkItemCB(WorkItem * w, void * ctx)
{
    XRpcCallWorkItem * call = (XRpcCallWorkItem*)ctx;
    XRpcContainer * params = call->params;
    XRpcElement * resp;

    /* map this thread to the session that is making this call */
    ASSERT(!THREAD_GetValue(call->server->sessionKey));
    THREAD_SetValue(call->server->sessionKey, call->session);

    /* 
     * these try/catch statements should only be present in the final release
     * build to make the code more reliable "in the field". During the normal
     * development cycle they should be commented out, so that testers can 
     * catch more bugs.
     */
#if defined(_WIN32) && defined(XRPC_EXCEPTION_HANDLING) && !DEBUG
     __try {
#endif /* _WIN32 && !DEBUG*/

    resp = (*call->method.handler)(call->method.context,params);

    if (call->id) {
        XRPC_SendResponse(call->session->client, call->id, resp);
        MEM_Free(call->id);
    }
    if (resp) XRPC_FreeElement(resp);

#if defined(_WIN32) && defined(XRPC_EXCEPTION_HANDLING) && !DEBUG
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        ASSMSG1("EXCEPTION %08lX in RPC handler!",GetExceptionCode());
        if (call->id) {
            XRPC_SendError(call->session->client,call->id,XRPC_STATUS_SYSERR);
            MEM_Free(call->id);
        }
    }
#endif /* _WIN32 && !DEBUG*/

    if (call->params) XRPC_FreeElement(&call->params->element);

    SERVER_WRITE_LOCK(call->server);
    THREAD_SetValue(call->server->sessionKey, NULL);
    QUEUE_RemoveEntry(&call->entry);
    EVENT_Set(&call->session->callEvent);
    SERVER_UNLOCK(call->server);
    
    XRPC_ReturnWorkQueue(call->server, call->q);
    MEM_Free(call);
    WKI_Detach(w);
}

STATIC void XRPC_CallElementClose(XRpcElementContext * context) 
{
    XRpcBool ok = XRpcFalse;
    XRpcString id = NULL;
    XRpcElement * resp = NULL;
    XRpcCallContext * call = CAST(context,XRpcCallContext,common);
    ASSERT(call->signature == CALL_CONTEXT_SIGNATURE);
    ASSERT(call->method.handler);
    if (call->signature == CALL_CONTEXT_SIGNATURE) {

        /* must have no more than one <params> child */

        XRpcContainer * callElement = _XRPC_IsContainer(context->elem);
        id = XRPC_GetAttribute(context->elem, XRPC_ATTR_ID);

        ASSERT(callElement);
        if (callElement) {
            XRpcContainer * par = NULL;
            int n = XRPC_ElementCount(callElement);
            switch (n) {
            case 1:
                par = _XRPC_IsContainer(XRPC_GetElementAt(callElement,0));
                ASSERT(par && par->element.tag == XRpcTagParams);
                if (!par) break;
                XRPC_DetachElementAt(callElement,0);
                /* fall through and call the handler */

            case 0:
                if (call->method.synchronous) {
                    resp = (*call->method.handler)(call->method.context, par);
                    if (par) XRPC_FreeElement(&par->element);
                    ok = XRpcTrue;
                } else {
                    XRpcServer * server = context->session->server;
                    WorkQueue * q = XRPC_GetWorkQueue(server);
                    if (q) {
                        XRpcCallWorkItem * w = MEM_New(XRpcCallWorkItem);
                        if (w) {
                            WorkItem * wki = WKI_Create(q,XRpcCallWorkItemCB,w);
                            if (wki) {
                                w->server = server;
                                w->session = context->session;
                                w->method = call->method;
                                w->params = par;
                                w->id = (id ? STRING_Dup(id) : NULL);
                                w->q = q;
                                SERVER_WRITE_LOCK(server);
                                QUEUE_InsertTail(&w->session->calls,&w->entry);
                                SERVER_UNLOCK(server);
                                ok = WKI_Submit(wki);
                                if (!ok) {
                                    SERVER_WRITE_LOCK(server);
                                    QUEUE_RemoveEntry(&w->entry);
                                    EVENT_Set(&w->session->callEvent);
                                    SERVER_UNLOCK(server);
                                    WKI_Detach(wki);
                                    MEM_Free(w->id);
                                }
                            }
                            if (!ok) MEM_Free(w);
                        }
                        if (!ok) XRPC_ReturnWorkQueue(server,q);
                    }
                }
                break;

            default:
                TRACE1("XRPC: too many <params> elements (%d)\n",n);
                break;
            }
        }

        /* send response */
        if (ok) {
            if (call->method.synchronous) {
                if (id) XRPC_SendResponse(context->session->client, id, resp);
                if (resp) XRPC_FreeElement(resp);
            }
        } else if (id) {
            XRPC_SendError(context->session->client, id, XRPC_STATUS_SYSERR);
        }

        /* deallocate the element context */
        XRPC_FreeElement(context->elem);
        MEM_Free(call);
    }
}

STATIC 
XRpcElementContext *
XRPC_HandleCall(
    XRpcSession * session,          /* session */
    XRpcTag tag,                    /* the tag */
    const XRpcAtts atts)            /* attributes */
{
    XRpcClient * client = session->client;
    XRpcContainer * container = XRPC_CreateContainer(tag);
    if (container) {
        XRpcElement * elem = &container->element;
        XRpcString pname, id;

        /* copy attributes to the element */
        XRPC_PutAttributes(elem, atts);
        id = XRPC_GetAttribute(elem, XRPC_ATTR_ID);
        pname = XRPC_GetAttribute(elem, XRPC_ATTR_PROTOCOL);
        if (pname) {
            Protocol * p = XRPC_GetProtocol(session->server, pname);
            if (p) {
                XRpcString mname = XRPC_GetAttribute(elem, XRPC_ATTR_METHOD);
                if (mname) {
                    XRpcCallContext * call = MEM_New(XRpcCallContext);
                    if (call) {
                        memset(call, 0, sizeof(*call));
                        call->signature = CALL_CONTEXT_SIGNATURE;
                        call->common.session = session;
                        call->common.elem = elem;
                        call->common.tag = tag;
                        call->common.close = XRPC_CallElementClose;
                
                        /* get method handler from the protocol */

                        if ((*p->protocol.getmethod)(p->context,mname,&call->method)) {
                            return &call->common;
                        } else {
                            TRACE2("XRPC: unknown method : %s (protocol %s)\n",
                                mname, pname);
                            if (id) XRPC_SendError(client, id, XRPC_STATUS_NOMETHOD);
                        }
                        MEM_Free(call);
                    }
                } else {
                    TRACE1("XRPC: missing method name (protocol %s)\n",pname);
                    if (id) XRPC_SendError(client, id, XRPC_STATUS_NOMETHOD);
                }
            } else {
                TRACE1("XRPC: unknown protocol: %s\n", pname);
                if (id) XRPC_SendError(client, id, XRPC_STATUS_NOPROTO);
            }
        } else {
            TRACE("XRPC: missing protocol name\n");
            if (id) XRPC_SendError(client, id, XRPC_STATUS_NOPROTO);
        }
        XRPC_FreeElement(elem);
    }
    return XRPC_ProblemElementHandler(session, tag, atts);
}

/* *************************************************************************
//
//  <response> tag 
//
// *************************************************************************/

STATIC void XRPC_ResponseElementClose(XRpcElementContext * context) 
{
    if (context) {

        /* deallocate the context itself, keep the element */
        XRpcServer * server = context->session->server;
        XRpcElement * elem = context->elem;
        MEM_Free(context);

        if (elem) {
            XRpcString id = XRPC_GetAttribute(elem, XRPC_ATTR_ID);
            if (id) {
                XRpcCall * call;

                /* check if we have anyone waiting for this response */
                SERVER_WRITE_LOCK(server);
                call = (XRpcCall*)HASH_Get(&server->callMap, id);
                if (call) {
                    XRpcContainer * response = _XRPC_IsContainer(elem);
                    XRpcString status = XRPC_GetAttribute(elem,XRPC_ATTR_STATUS);

                    /* translate status string into status code */
                    if (status) {
                        call->status = XRpcStatusSysErr;
                        if (strcmp(status,XRPC_STATUS_OK) == 0) {
                            call->status = XRpcStatusOK;
                        } else if (strcmp(status,XRPC_STATUS_NOPROTO) == 0) {
                            call->status = XRpcStatusNoTarget;
                        } else if (strcmp(status,XRPC_STATUS_NOMETHOD) == 0) {
                            call->status = XRpcStatusNoMethod;
                        }
                    } else {
                        call->status = XRpcStatusOK;
                    }

                    /* extract result data from the element tree */
                    call->result = NULL;
                    ASSERT(response);
                    if (XRPC_ElementCount(response) > 0) {
                        XRpcContainer * r; /* <result> element */
                        r = _XRPC_IsContainer(XRPC_GetElementAt(response,0));
                        if (r && XRPC_ElementCount(r)) {
                            call->result = XRPC_DetachElementAt(r,0);
                        }
                    }

                    /* remove the call  from the map */
                    HASH_Remove(&server->callMap, id);

                    /* call is done */
                    ASSERT(!call->done);
                    call->done = XRpcTrue;

                    /* wake up the waiter */
                    EVENT_Set(&call->event);
                }
                SERVER_UNLOCK(server);
            }

            /* response element should not have a parent */
            ASSERT(!XRPC_GetParent(elem));
            XRPC_FreeElement(elem);
        }
    }
}

STATIC
XRpcElementContext *
XRPC_ResponseHandler(
    XRpcSession * session,          /* session */
    XRpcTag tag,                    /* the tag */
    const XRpcAtts atts)            /* attributes */
{
    XRpcElementContext * c = XRPC_DefaultContainerHandler(session,tag,atts);
    if (c) c->close = XRPC_ResponseElementClose;
    return c;
}

/* *************************************************************************
//
//  Data element handlers
//
// *************************************************************************/

/* <long> */
STATIC 
XRpcElementContext *
XRPC_HandleLong(XRpcSession * s, XRpcTag tag, const XRpcAtts atts)
{
    XRpcLongElement * e = XRPC_CreateLongElement(0);
    return (e?XRPC_CreateDataElementContext(s,&e->delement.element,atts):NULL);
}

/*  <int> */
STATIC 
XRpcElementContext *
XRPC_HandleInt(XRpcSession * s, XRpcTag tag, const XRpcAtts atts)
{
    XRpcIntElement * e = XRPC_CreateIntElement(0);
    return (e?XRPC_CreateDataElementContext(s,&e->delement.element,atts):NULL);
}

/* <short> */
STATIC 
XRpcElementContext *
XRPC_HandleShort(XRpcSession * s, XRpcTag tag, const XRpcAtts atts)
{
    XRpcShortElement * e = XRPC_CreateShortElement(0);
    return (e?XRPC_CreateDataElementContext(s,&e->delement.element,atts):NULL);
}

/*  <byte> */
STATIC 
XRpcElementContext *
XRPC_HandleByte(XRpcSession * s, XRpcTag tag, const XRpcAtts atts)
{
    XRpcByteElement * e = XRPC_CreateByteElement(0);
    return (e?XRPC_CreateDataElementContext(s,&e->delement.element,atts):NULL);
}

/* <boolean> */
STATIC 
XRpcElementContext *
XRPC_HandleBool(XRpcSession * s, XRpcTag tag, const XRpcAtts atts)
{
    XRpcBoolElement * e = XRPC_CreateBoolElement(XRpcFalse);
    return (e?XRPC_CreateDataElementContext(s,&e->delement.element,atts):NULL);
}

/* <string> */
STATIC 
XRpcElementContext *
XRPC_HandleString(XRpcSession * s, XRpcTag tag, const XRpcAtts atts)
{
    XRpcStringElement * e = XRPC_CreateStringElement(NULL,-1);
    return (e?XRPC_CreateDataElementContext(s,&e->delement.element,atts):NULL);
}

/* <base64> */
STATIC 
XRpcElementContext *
XRPC_HandleBase64(XRpcSession * s, XRpcTag tag, const XRpcAtts atts)
{
    XRpcBinaryElement * e = XRPC_CreateBinaryElement(NULL,0);
    return (e?XRPC_CreateDataElementContext(s,&e->delement.element,atts):NULL);
}

/*  <void> and <null> */
STATIC 
XRpcElementContext *
XRPC_HandleEmptyElement(
    XRpcSession * session,          /* session */
    XRpcDataElement * elem,         /* the data element */
    XRpcTag tag,                    /* the tag */
    const XRpcAtts atts)            /* attributes */
{
    if (elem) {
        ASSERT(elem->element.tag == tag);
        if (XRPC_PutAttributes(&elem->element, atts)) {
            XRpcElementContext * ec = MEM_New(XRpcElementContext);
            if (ec) {
                memset(ec, 0, sizeof(*ec));
                ec->tag = tag;
                ec->elem = &elem->element;
                ec->session = session;
                if (ec->elem) {
                    ec->close = XRPC_DefaultElementClose;
                    return ec;
                }
                MEM_Free(ec);
            }
        }
        XRPC_FreeElement(&elem->element);
    }
    return NULL;
}

STATIC 
XRpcElementContext *
XRPC_HandleVoid(XRpcSession * s, XRpcTag tag, const XRpcAtts atts)
{
    return XRPC_HandleEmptyElement(s, XRPC_CreateVoidElement(), tag, atts);
}

STATIC 
XRpcElementContext *
XRPC_HandleNull(XRpcSession * s, XRpcTag tag, const XRpcAtts atts)
{
    return XRPC_HandleEmptyElement(s, XRPC_CreateNullElement(), tag, atts);
}

/* *************************************************************************
//
//  Tag -> handler map
//
// *************************************************************************/

STATIC 
XRpcBool 
XRPC_InitTagHandler(
    XRpcServer * server, 
    XRpcTag tag, 
    XParseElementHandler handler) 
{
    ElementInfo * info = MEM_New(ElementInfo);
    if (info) {
        HashKey key = (HashKey)XRPC_GetTagName(tag);
        info->handler = handler;
        info->tag = tag;
        if (HASH_Put(&server->tagMap, key, info)) {
            return XRpcTrue;
        }
        MEM_Free(info);
    }
    return XRpcFalse;
}

STATIC XRpcBool XRPC_InitTagHandlers(XRpcServer * s) 
{
    return 

        /* root tag */
        XRPC_InitTagHandler(s,XRpcTagRoot,    XRPC_HandleRoot) &&

        /* call tags */
        XRPC_InitTagHandler(s,XRpcTagCall,    XRPC_HandleCall) &&
        XRPC_InitTagHandler(s,XRpcTagParams,  XRPC_DefaultContainerHandler) &&

        /* response tags */
        XRPC_InitTagHandler(s,XRpcTagResponse,XRPC_ResponseHandler) &&
        XRPC_InitTagHandler(s,XRpcTagResult,  XRPC_DefaultContainerHandler) &&

        /* aggregates */
        XRPC_InitTagHandler(s,XRpcTagStruct,  XRPC_DefaultContainerHandler) &&
        XRPC_InitTagHandler(s,XRpcTagArray,   XRPC_DefaultContainerHandler) &&

        /* primitive data types */
        XRPC_InitTagHandler(s,XRpcTagLong,    XRPC_HandleLong) &&
        XRPC_InitTagHandler(s,XRpcTagInt,     XRPC_HandleInt) &&
        XRPC_InitTagHandler(s,XRpcTagShort,   XRPC_HandleShort) &&
        XRPC_InitTagHandler(s,XRpcTagByte,    XRPC_HandleByte) &&
        XRPC_InitTagHandler(s,XRpcTagBoolean, XRPC_HandleBool) &&
        XRPC_InitTagHandler(s,XRpcTagBase64,  XRPC_HandleBase64) &&
        XRPC_InitTagHandler(s,XRpcTagString,  XRPC_HandleString) &&

        /* special values */
        XRPC_InitTagHandler(s,XRpcTagNull,    XRPC_HandleNull) &&
        XRPC_InitTagHandler(s,XRpcTagVoid,    XRPC_HandleVoid);
}

/*
 * Local Variables:
 * c-basic-offset:4
 * indent-tabs-mode: nil
 * End:
 */

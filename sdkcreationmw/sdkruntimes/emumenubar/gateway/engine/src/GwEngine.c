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

static int gwInitCount = 0;

/**
 * Static initialization
 */
static void ECMTGW_Init()
{
    if (gwInitCount++ == 0) {
        MEM_InitModule();
        THREAD_InitModule();
        XRPC_Init();
    }
}

/**
 * Static deinitialization
 */
static void ECMTGW_Deinit()
{
    gwInitCount--;
    ASSERT(gwInitCount >= 0);
    if (gwInitCount == 0) {
        XRPC_Deinit();
        THREAD_Shutdown();
        MEM_Shutdown();
    }
}

/**
 * Hashtable callbacks
 */
STATIC Bool GWENG_MidpCompare(HashKeyC key1, HashKeyC key2)
{
    const MidpSessionKey* k1 = key1;
    const MidpSessionKey* k2 = key2;
    return ((k1->xrpcSid == k2->xrpcSid) && 
            (k1->xrpcSession == k2->xrpcSession));
}

STATIC HashCode GWENG_MidpHash(HashKeyC key)
{
    const MidpSessionKey* k = key;
    return 31*((HashCode)k->xrpcSession) + k->xrpcSid;
}

/**
 * Completes gateway initialization.
 */
STATIC Bool ECMTGW_Init2(EcmtGateway* gw)
{
    gw->clientQueueIo = GWENG_CreateQueueConnection(gw,gw->clientQueue);
    if (gw->clientQueueIo) {
        gw->handsetQueueIo = GWENG_CreateQueueConnection(gw,gw->handsetQueue);
        if (gw->handsetQueueIo) {
            if (HASH_Init(&gw->listenerMap, 0, NULL, NULL, NULL)) {
                if (HASH_Init(&gw->ecmtSessionMap, 0, NULL, NULL, NULL)) {
                    if (HASH_Init(&gw->midpSessionMap, 0,
                        GWENG_MidpCompare, GWENG_MidpHash, NULL)) {
                        gw->comPorts = GWENG_CreateComTransport(gw);
                        gw->btSockets = GWENG_CreateBtSocketTransport(gw);
						gw->lanSockets = GWENG_CreateLanSocketTransport(gw);
                        if (GWENG_StartXRPC(gw)) {
                            return True;
                        }
                        HASH_Destroy(&gw->midpSessionMap);
                    }
                    HASH_Destroy(&gw->ecmtSessionMap);
                }
                HASH_Destroy(&gw->listenerMap);
            }
            GWENG_IoReleaseRef(gw->handsetQueueIo);
        }
        GWENG_IoReleaseRef(gw->clientQueueIo);
    }
    return False;
}

/**
 * Creates ECMT Gateway instance and starts listening on a TCP port for
 * incoming TCP connections. Returns NULL on failure. The EcmtGwCallbacks
 * pointer can be NULL, which means that the caller is not interested in
 * receiving any events. If it's not NULL, gateway must copy the contents
 * of this structure into internal storage, because it may be allocated 
 * on stack.
 */
EcmtGateway* ECMTGW_Create(const char* productId, 
                           const EcmtGwCallbacks* cb, 
                           void* ctx)
{
    EcmtGateway* gw = NULL;
    ECMTGW_Init();
    gw = MEM_New(EcmtGateway);
    if (gw) {
        memset(gw, 0, sizeof(*gw));
        if (cb) {
            gw->cb = *cb;
            gw->ctx = ctx;
        }
        QUEUE_Init(&gw->ecmtClients);
        if (MUTEX_Init(&gw->mutex)) {
            gw->productId = STRING_Dup(productId);
            if (gw->productId) {
                gw->multiplexor = GWENG_CreateMultiplexor(gw);
                if (gw->multiplexor) {
                    gw->clientQueue = GWENG_QueueCreate(NULL);
                    if (gw->clientQueue) {
                        gw->handsetQueue = GWENG_QueueCreate(NULL);
                        if (gw->handsetQueue) {
                            if (ECMTGW_Init2(gw)) {
                                return gw;
                            }
                            GWENG_QueueDelete(gw->handsetQueue);
                        }
                        GWENG_QueueDelete(gw->clientQueue);
                    }
                    GWENG_IoReleaseRef(gw->multiplexor);
                }
                MEM_Free(gw->productId);
            }
            MUTEX_Destroy(&gw->mutex);
        }
        MEM_Free(gw);
    }

    ECMTGW_Deinit();
    return NULL;
}

/**
 * Destroys ECMT Gateway instance. Ignores NULL pointer.
 */
void ECMTGW_Destroy(EcmtGateway* gw)
{
    if (gw) {
        /* disable all notifications */
        MUTEX_Lock(&gw->mutex);
        memset(&gw->cb, 0, sizeof (EcmtGwCallbacks));
        MUTEX_Unlock(&gw->mutex);

        /* deregister out XRPC interface */
        XREG_FreeRegistry(gw->xreg);
        XRPC_FreeServer(gw->xrpc);

        /* disconnect from the handset */
        ECMTGW_Deactivate(gw);

        /* shutdown the listening thread */
        if (gw->ecmtSocket != INVALID_SOCKET) {
            SOCKET_Close (gw->ecmtSocket);
            if (gw->listenThread) THREAD_Join (gw->listenThread);
        }

        /* disconnect the clients */
        MUTEX_Lock(&gw->mutex);
        while (!QUEUE_IsEmpty(&gw->ecmtClients)) {
            ThrID thread;
            QEntry* e = QUEUE_First(&gw->ecmtClients);
            EcmtClient* client = QCAST(e, EcmtClient, entry);
            GWENG_IoCancel(client->conn);
            thread = client->thread;
            client->thread = NULL;
            MUTEX_Unlock(&gw->mutex);
            THREAD_Join(thread);
            MUTEX_Lock(&gw->mutex);
        }
        MUTEX_Unlock(&gw->mutex);

        ASSERT(!gw->eventMask);
        ASSERT(HASH_IsEmpty(&gw->listenerMap));
        ASSERT(HASH_IsEmpty(&gw->midpSessionMap));
        ASSERT(HASH_IsEmpty(&gw->ecmtSessionMap));

        /* destroy ports */
        GWENG_TransportFree(gw->comPorts);
        GWENG_TransportFree(gw->btSockets);

        /* destroy everything else */
        GWENG_IoReleaseRef(gw->handsetQueueIo);
        GWENG_IoReleaseRef(gw->clientQueueIo);
        GWENG_QueueDelete(gw->handsetQueue);
        GWENG_QueueDelete(gw->clientQueue);
        GWENG_IoReleaseRef(gw->multiplexor);
        HASH_Destroy(&gw->listenerMap);
        HASH_Destroy(&gw->midpSessionMap);
        HASH_Destroy(&gw->ecmtSessionMap);
        MEM_Free(gw->productId);
        MUTEX_Destroy(&gw->mutex);
        MEM_Free(gw);

        ECMTGW_Deinit();
    }
}

/**
 * If the gateway is active, returns the active port. Otherwise, returns NULL.
 */
EcmtGatewayPort* ECMTGW_GetActivePort(EcmtGateway* gw)
{
    if (gw->activeCon) {
        ASSERT(gw->activeCon->port);
        return gw->activeCon->port;
    } else {
        return NULL;
    }
}

/**
 * Initiates the procedure of refreshing the port list. The portFound callback
 * will be invoked on each new port, portDisappeared on each port that has
 * disappeared. The callbacks are invoked in arbitrary thread context.
 * Some callbacks may be invoked on the current thread, before this
 * function returns.
 */
void ECMTGW_RefreshPorts(EcmtGateway* gw)
{
    /* 
     * Double GWENG_RefreshStarted/Finished calls allow the transports restart
     * the search (i.e. call GWENG_RefreshFinished/Started) without causing
     * a pair of refreshDone/Start notifications.
     */
    GWENG_RefreshStarted(gw);
    GWENG_RefreshStarted(gw);
    if (gw->comPorts) {
        GWENG_TransportRefresh(gw->comPorts);
    }
    if (gw->btSockets) {
        GWENG_TransportRefresh(gw->btSockets);
    }
	//TODO, Petchatnikov: Insert Lan ports refreshment
	//gw->lanSockets->transport->refresh(t)
    if (gw->lanSockets) {
        GWENG_TransportRefresh(gw->lanSockets);
    }
	//Why do we need to call the following twice?
    GWENG_RefreshFinished(gw);
    GWENG_RefreshFinished(gw);
}

/**
 * Invoked on the handset read thread when it's about to exit.
 */
STATIC void GWENG_HandsetReadDone(EcmtReader* reader, Bool protErr)
{
    EcmtGateway* gw = reader->gw;
    MUTEX_Lock(&gw->mutex);
    if (gw->activeCon == reader->in) {
        GWENG_QueueSetConnection(gw->handsetQueue, NULL);
        GWENG_IoReleaseRef(gw->activeCon);
        gw->activeCon = NULL;
        if (gw->cb.deactivate) {
            gw->cb.deactivate(gw, protErr ? 
                              EcmtGwDeactProtErr : 
                              EcmtGwDeactIOErr, 
                              gw->ctx);
        }
    } else if (gw->activeCon) {
        GWENG_IoReleaseRef(gw->activeCon);
        gw->activeCon = NULL;
    }
    MUTEX_Unlock(&gw->mutex);
}

/**
 * Creates GWENG_Reader thread.
 */
STATIC Bool GWENG_CreateReader(EcmtGateway* gw, 
                               ThrID* tid,
                               EcmtConnection* in,
                               EcmtConnection* out,
                               ReaderFilter filter,
                               ReaderDone done)
{
    /* EcmtReader is deallocated by GWENG_Reader thread */
    EcmtReader* reader = MEM_New(EcmtReader);
    if (reader) {
        memset(reader, 0, sizeof(*reader));
        reader->gw = gw;
        reader->in = in;
        reader->out = out;
        reader->filter = filter;
        reader->done = done;
        GWENG_IoAddRef(reader->in);
        GWENG_IoAddRef(reader->out);
        if (THREAD_Create(tid, GWENG_Reader, reader)) {
            return True;
        }
        GWENG_IoReleaseRef(reader->in);
        GWENG_IoReleaseRef(reader->out);
        MEM_Free(reader);
    }
    return False;
}

/**
 * Activates the gateway by connecting it to the specified port. If 
 * gateway is already connected to the same port, does nothing and
 * returns True. If gateway is already connected to another port, it
 * disconnects it first and then attempts to connect to the specified 
 * port. Returns True if activation was successful, False on failure.
 */
EcmtBool ECMTGW_Activate(EcmtGatewayPort* port)
{
    EcmtBool result = EcmtFalse;
    ASSERT(port);
    if (port) {
        EcmtGateway* gw = port->gw;
        if (gw->activeCon && port == gw->activeCon->port) {
            return EcmtTrue;
        } else {
            ECMTGW_Deactivate(gw);
            ASSERT(!gw->activeCon);
            gw->activeCon = GWENG_PortConnect(port);
            if (gw->activeCon) {
                ASSERT(gw->activeCon->port == port);
                if (GWENG_CreateReader(gw, &gw->readThread, 
                                       gw->activeCon, gw->clientQueueIo, 
                                       GWENG_MidpFilter, 
                                       GWENG_HandsetReadDone)) {
                    GWENG_QueueSetConnection(gw->handsetQueue, gw->activeCon);
                    if (gw->cb.activate) {
                        gw->cb.activate(gw, port, gw->ctx);
                    }

				return EcmtTrue;
                }
                gw->activeCon = NULL;
                GWENG_IoClose(gw->activeCon);
            }
        }
    }
    return EcmtFalse;
}

/**
 * Deactivates the gateway by closing the currently active port. Does nothing
 * if gateway is not connected.
 */
void ECMTGW_Deactivate(EcmtGateway* gw)
{
    if (gw->readThread) {
        EcmtConnection* con = NULL;
        
        MUTEX_Lock(&gw->mutex);
        con = gw->activeCon;
        gw->activeCon = NULL;
        MUTEX_Unlock(&gw->mutex);

        GWENG_QueueSetConnection(gw->handsetQueue, NULL);
        if (con) {
            GWENG_IoCancel(con);
            THREAD_Join(gw->readThread);
            GWENG_IoReleaseRef(con);
            if (gw->cb.deactivate) {
                gw->cb.deactivate(gw, EcmtGwDeactRequest, gw->ctx);
            }
        } else {
            THREAD_Join(gw->readThread);
        }
        gw->readThread = NULL;
    }
    ASSERT(!gw->activeCon);
}

/**
 * Invoked on the ECMT client read thread when it's about to exit.
 */
STATIC void GWENG_ClientReadDone(EcmtReader* reader, Bool protErr)
{
    QEntry* e;
    EcmtGateway* gw = reader->gw;
    MUTEX_Lock(&gw->mutex);
    for (e=QUEUE_First(&gw->ecmtClients); e; e=QUEUE_Next(e)) {
        EcmtClient* client = QCAST(e, EcmtClient, entry);
        if (client->conn == reader->in) {
            QUEUE_RemoveEntry(e);
            GWENG_IoReleaseRef(client->conn);
            if (client->thread) {
                THREAD_Detach(client->thread);
            }
            MEM_Free(client);
            if (gw->cb.disconnect) {
                gw->cb.disconnect(gw, gw->ctx);
            }
            break;
        }
    }
    ASSERT(e);
    if (QUEUE_IsEmpty(&gw->ecmtClients)) {
        GWENG_QueueSetConnection(gw->clientQueue, NULL);
    }
    MUTEX_Unlock(&gw->mutex);
}

/**
 * Accepts Ecmt connection. After connection is accepted it adds it 
 * to the connections list and starts receiving thread.
 */
STATIC void GWENG_Listen(void* ctx)
{
    EcmtGateway* gw = ctx; 
    while (SOCKET_Wait(gw->ecmtSocket,SOCK_WAIT_READ,0) == SOCK_WAIT_READ) {
        struct sockaddr_in in;
        int len = sizeof(in);
        Socket s = accept(gw->ecmtSocket,(struct sockaddr*)&in,&len);
        if (s != INVALID_SOCKET) {
            EcmtConnection* conn = GWENG_CreateSocketConnection(gw, s);
            if (conn) {
                EcmtClient* client = MEM_New(EcmtClient);
                if (client) {
                    memset(client, 0, sizeof(*client));
                    client->conn = conn;
                    MUTEX_Lock(&gw->mutex);
                    QUEUE_InsertTail(&gw->ecmtClients, &client->entry);
                    if (GWENG_CreateReader(gw, &client->thread, 
                                           client->conn,
                                           gw->handsetQueueIo, 
                                           NULL,
                                           GWENG_ClientReadDone)) {
                        if (gw->cb.connect) {
                            gw->cb.connect(gw, gw->ctx);
                        }
                        GWENG_QueueSetConnection(gw->clientQueue, gw->multiplexor);
                        MUTEX_Unlock(&gw->mutex);
                        continue;
                    }
                    QUEUE_RemoveEntry(&client->entry);
                    MUTEX_Unlock(&gw->mutex);
                }
                MEM_Free(client);
                GWENG_IoReleaseRef(conn);
            } else {
                SOCKET_Close(s);
            }
        }
    }
}

/**
 * Returns the TCP port on which the gateway is listening for incoming
 * Ecmt connections.
 */
int ECMTGW_GetEcmtPort (EcmtGateway* gw)
{
    MUTEX_Lock (&gw->mutex);
    if (gw->ecmtPort <= 0) {
        if (GWENG_CreateEcmtListeningSocket (gw)) {
            if (!gw->listenThread && 
                !THREAD_Create (&gw->listenThread, GWENG_Listen, gw)) {
                SOCKET_Close (gw->ecmtSocket);
                gw->ecmtPort = -1;
                gw->ecmtSocket = INVALID_SOCKET;
            }
        }
    }
    MUTEX_Unlock (&gw->mutex);
    return gw->ecmtPort;
}

/**
 * Returns the number of currently connected Ecmt clients.
 */
int ECMTGW_CountEcmtClients (EcmtGateway* gw)
{
    return QUEUE_Size(&gw->ecmtClients);
}

/**
 * Sends a ECMT message to either COM port or to all connected clients.
 * Returns True if message was successfully sent, False on failure or
 * if there's nowhere to send the message (gateway not active, or no
 * clients connected).
 */
EcmtBool
ECMTGW_SendMessage(
    EcmtGateway* gw,        /* gateway instance */
    int uid,                /* destination UID */
    const void* data,       /* the packet data */
    int datalen,            /* length of the packet data */
    EcmtDirection dir)      /* to COM port or to ECMT clients */
{
    TRACE3("GWENG: message to %s, uid=0x%08x, %d byte(s) \n",
            (dir == EcmtDirectionToHandset)?"handset":"clients", 
            uid, datalen);

    DEBUG_ONLY(PRINT_Dump(DEBUG_Trace,data,datalen,0));
    return GWENG_QueueAdd((dir == EcmtDirectionToHandset) ? 
                           gw->handsetQueue : gw->clientQueue, 
                           uid, data, datalen);
}

/* ECMT protocol level notifications */
void GWENG_IoPacketStart(EcmtConnection* c, int uid, const void* data, int len)
{
    if (GWENG_IoHandset(c)) {
        if (c->gw->cb.sendPacket) {
            c->gw->cb.sendPacket(c->gw, uid, data, len, c->gw->ctx);
        }
        GWENG_Notify(c->gw, GwNotifySend, uid, data, len);
    }
}

void GWENG_IoPacketSent(EcmtConnection* c)
{
    if (GWENG_IoHandset(c) && c->gw->cb.sendPacketComplete) {
        c->gw->cb.sendPacketComplete(c->gw, c->gw->ctx);
    }
}

void GWENG_IoHeaderReceived(EcmtConnection* c, int uid, int len)
{
    if (GWENG_IoHandset(c) && c->gw->cb.receivePacket) {
        c->gw->cb.receivePacket(c->gw, uid, len, c->gw->ctx);
    }
}

void GWENG_IoPacketReceived(EcmtConnection* c, int uid, const void* data, int len)
{
    if (GWENG_IoHandset(c)) {
        GWENG_Notify(c->gw, GwNotifyReceive, uid, data, len);
        if (c->gw->cb.receivePacketComplete) {
            c->gw->cb.receivePacketComplete(c->gw, uid, data, len, c->gw->ctx);
        }
    }
}

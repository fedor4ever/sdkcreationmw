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


#include <winsock2.h>
#include "EcmtGwP.h"
#include <objbase.h>
#include <ws2bth.h>

static const char EcmtAgentName [] = "EcmtAgent";
static const GUID SerialPortServiceClass_UUID = 
    {0x00001101, 0x0000, 0x1000, {0x80,0x00,0x00,0x80,0x5F,0x9B,0x34,0xFB}};

/**
 * Communication port using Bluetooth RFCOMM sockets. This functionality
 * requires Microsoft Bluetooth stack.
 */

typedef struct _EcmtGatewayBtSocketPort {
    EcmtGatewayPort common;     /* common part */
    BTH_ADDR btaddr;            /* Bluetooth device address */
    ULONG btport;               /* RFCOMM channel */
} EcmtGatewayBtSocketPort;

/**
 * Bluetooth sockets
 */

STATIC Socket GWENG_BtConnect(const BTH_ADDR* btaddr, ULONG port)
{
    Socket s = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
    if (s != INVALID_SOCKET) {
        SOCKADDR_BTH bta;
        SOCKADDR* sa = (SOCKADDR*)&bta;
        memset(&bta, 0, sizeof(bta));
        bta.addressFamily = AF_BTH;
        bta.btAddr = *btaddr;
        bta.port = port;
        TRACE2("GW: connecting to %012I64X:%d\n",bta.btAddr,port);
        if (connect(s, sa, sizeof(bta)) == 0)  {
            TRACE2("GW: connected to %012I64X:%u\n",bta.btAddr,port);
            return s;
        } else {
            TRACE1("GW: connect error %d\n",SOCKET_GetLastError());
            SOCKET_Close(s);
        }
    } else {
        TRACE1("GW: socket error %d\n",SOCKET_GetLastError());
    }
    return INVALID_SOCKET;
}

STATIC Bool GWENG_BtSockCanOpen(const EcmtGatewayPort* port)
{
    EcmtGatewayBtSocketPort* bp = CAST(port,EcmtGatewayBtSocketPort,common);
    Socket s = GWENG_BtConnect(&bp->btaddr, bp->btport);
    if (s != INVALID_SOCKET) {
        SOCKET_Close(s);
        return True;
    } else {
        return False;
    }
}

STATIC EcmtConnection* GWENG_BtSockConnect(EcmtGatewayPort* port)
{
    EcmtGatewayBtSocketPort* bp = CAST(port,EcmtGatewayBtSocketPort,common);
    Socket s = GWENG_BtConnect(&bp->btaddr, bp->btport);
    if (s != INVALID_SOCKET) {
        EcmtConnection* c = GWENG_CreateHandsetSocketConnection(port, s);
        if (c) {
            return c;
        }
        SOCKET_Close(s);
    }
    return NULL;
}

STATIC void GWENG_BtSockFree(EcmtGatewayPort* port)
{
    GWENG_PortDestroy(port);
    MEM_Free(CAST(port,EcmtGatewayBtSocketPort,common));
}

STATIC EcmtPort gwBtSocket = {
    "BtSocket",                     /* name */
    EcmtGwPortBtSocket,             /* type */
    GWENG_BtSockCanOpen,            /* canOpen */
    GWENG_BtSockConnect,            /* connect */
    GWENG_BtSockFree                /* free */
};

/**
 * Bluetooth socket transport
 */
typedef struct _EcmtBtSocketTransport {
    EcmtGatewayTransport common;
    WorkQueue* refresh;
    WorkItem* refreshItem;
    Bool refreshInProgress;
    Bool exiting;
} EcmtBtSocketTransport;

STATIC Bool GWENG_HashCompareBtAddr(HashKeyC key1, HashKeyC key2)
{
    const BTH_ADDR* a1 = key1;
    const BTH_ADDR* a2 = key2;
    return (memcmp(a1,a2,sizeof(*a1)) == 0);
}

STATIC HashCode GWENG_BtAddrHashProc(HashKeyC key)
{
    const BTH_ADDR* a = key;
    return (HashCode)((*a) ^ ((*a) >> 32));
}

STATIC void GWENG_BtSockPortHashCleanup(HashKey key, HashValue value)
{
    EcmtGatewayBtSocketPort* port = value;
    GWENG_PortReleaseRef(&port->common);
}

/**
 * Queries WSAPROTOCOL_INFO for Bluetooth protocol provider
 */
STATIC Bool GWENG_BtGetProtocolInfo(WSAPROTOCOL_INFO* pi)
{
    Socket s = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
    if (s != INVALID_SOCKET) {
        char* opt = (char*)pi;
        int len = sizeof(*pi);
        if (!getsockopt(s, SOL_SOCKET, SO_PROTOCOL_INFO, opt, &len)) {
            SOCKET_Close(s);
            return True;
        } else {
	        TRACE1("GWENG: BT getsockopt error %d\n", GetLastError());
        }
        SOCKET_Close(s);
    } else {
        TRACE1("GWENG: BT socket error %d\n",SOCKET_GetLastError());
    }
    return False;
}

/**
 * Checks if we found EcmtAgent service and puts it into the hash table.
 */
STATIC EcmtGatewayBtSocketPort* 
GWENG_BtEcmtAgentCheck(Str device, const WSAQUERYSET* qs, HashTable* ht)
{
    if (qs->lpcsaBuffer && 
        qs->lpcsaBuffer->RemoteAddr.lpSockaddr &&
        qs->lpcsaBuffer->RemoteAddr.lpSockaddr->sa_family == AF_BTH &&
        qs->lpszServiceInstanceName) {
        SOCKADDR_BTH* bta;
        bta = (SOCKADDR_BTH*)qs->lpcsaBuffer->RemoteAddr.lpSockaddr;
        TRACE1("GWENG: found [%s]\n", qs->lpszServiceInstanceName);
        if (!strcmp(qs->lpszServiceInstanceName, EcmtAgentName)) {
            EcmtGatewayBtSocketPort* bp = MEM_New(EcmtGatewayBtSocketPort);
            if (bp) {
                memset(bp, 0, sizeof(*bp));
                bp->common.comment = STRING_Dup(qs->lpszServiceInstanceName);
                if (bp->common.comment) {
                    bp->common.name = STRING_Dup(device);
                    if (bp->common.name) {
                        bp->common.ref = 1;
                        bp->common.port = &gwBtSocket;
                        memcpy(&bp->btaddr, &bta->btAddr, sizeof(BTH_ADDR));
                        bp->btport = bta->port;
                        if (HASH_Put(ht, &bp->btaddr, bp)) {
                            return bp;
                        }
                        MEM_Free(bp->common.name);
                    }
                    MEM_Free(bp->common.comment);
                }
                MEM_Free(bp);
            }
        }
    }
    return NULL;
}

/**
 * Turns off the "refreshInProgress" flag 
 */
STATIC void GWENG_BtSockFinishRefresh(EcmtBtSocketTransport* bt)
{
    Bool finished = False;
    MUTEX_Lock(&bt->common.mutex);
    if (bt->refreshInProgress) {
        bt->refreshInProgress = False;
        finished = True;
    }
    MUTEX_Unlock(&bt->common.mutex);
    if (finished) {
        GWENG_RefreshFinished(bt->common.gw);
    }
}

/**
 * Scans for Bluetooth devices running EcmtAgent
 */
STATIC void GWENG_BtSockScan(WorkItem * w, void* arg)
{
    EcmtBtSocketTransport* bt = arg;
    EcmtGatewayBtSocketPort* port;
    EcmtGateway* gw = bt->common.gw;

    /* WSALookupServiceBegin stuff */
    BYTE qsbuf[2000];
    WSAQUERYSET* qs = (WSAQUERYSET*)qsbuf;
    WSAPROTOCOL_INFO pi;
    HANDLE hDeviceLoop = NULL;
    GUID serviceClassId = SerialPortServiceClass_UUID;
    ULONG svcFlags = (LUP_FLUSHCACHE  | 
                      LUP_RETURN_NAME | 
                      LUP_RETURN_TYPE | 
                      LUP_RETURN_ADDR);
    ULONG devFlags = (LUP_CONTAINERS  | 
                      LUP_RES_SERVICE | 
                      svcFlags);

    QEntry* e;
    Iterator* entries;
    HashTable knownPorts;
    HashTable foundPorts;

    /* Query protocol info. If that fails, give up */
    if (!GWENG_BtGetProtocolInfo(&pi)) {
        GWENG_BtSockFinishRefresh(bt);
        return;
    }

    /* It looks like Bluetooth sockets are functioning */
    if (!HASH_Init(&knownPorts, 0, GWENG_HashCompareBtAddr, 
        GWENG_BtAddrHashProc, GWENG_BtSockPortHashCleanup)) {
        GWENG_BtSockFinishRefresh(bt);
        return;
    }
    if (!HASH_Init(&foundPorts, 0, GWENG_HashCompareBtAddr, 
        GWENG_BtAddrHashProc, GWENG_BtSockPortHashCleanup)) {
        GWENG_BtSockFinishRefresh(bt);
        HASH_Destroy(&knownPorts);
        return;
    }

    /* Collect the list of known ports into a hashtable */
    MUTEX_Lock(&bt->common.mutex);
    for (e = QUEUE_First(&bt->common.ports); e; e = QUEUE_Next(e)) {
        port = QCAST(e,EcmtGatewayBtSocketPort,common.entry);
        if (HASH_Put(&knownPorts, &port->btaddr, port)) {
            GWENG_PortAddRef(&port->common);
        }
    }
    MUTEX_Unlock(&bt->common.mutex);

    memset(qs, 0, sizeof(WSAQUERYSET));
    qs->dwSize = sizeof(WSAQUERYSET);
    qs->dwNameSpace = NS_BTH;

    /* Device loop */
    if (!WSALookupServiceBegin(qs, devFlags, &hDeviceLoop)) {
        int len = sizeof(qsbuf);
        while (!bt->exiting && 
               !WSALookupServiceNext(hDeviceLoop, devFlags, &len, qs)) {
            char addrBuf[100];
            char* remoteAddr = NULL;
            char* devName = NULL;
            if (qs->lpszServiceInstanceName) {
                devName = STRING_Dup(qs->lpszServiceInstanceName);
                TRACE1("GWENG: BT device %s\n", qs->lpszServiceInstanceName);
            }
            if (devName && qs->lpcsaBuffer) {
                SOCKET_ADDRESS* ra = &qs->lpcsaBuffer->RemoteAddr;
                if (ra->lpSockaddr && 
                    ra->lpSockaddr->sa_family == AF_BTH) {
                    len =  sizeof(addrBuf);
                    if (!WSAAddressToString(ra->lpSockaddr,
                        ra->iSockaddrLength, &pi, addrBuf, &len)) {  
                        remoteAddr = addrBuf;
                        TRACE1("GWENG: remote address: %s\n", remoteAddr);
                    } else {
                        TRACE1("GWENG: WSAAddressToString: %s\n",
                            GetLastError());
                    }
                }
            }

            /* Service loop, only look up serial port services */
            if (remoteAddr) {
                HANDLE hSvc = NULL;
                memset(qs, 0, sizeof(WSAQUERYSET));
                qs->dwSize = sizeof(WSAQUERYSET);
                qs->dwNameSpace = NS_BTH;
                qs->lpszContext = remoteAddr;
                qs->lpServiceClassId = &serviceClassId;
                if (!WSALookupServiceBegin(qs, svcFlags, &hSvc)) {
                    len = sizeof(qsbuf);
                    while (!bt->exiting && 
                           !WSALookupServiceNext(hSvc,svcFlags,&len,qs)) {
                        EcmtGatewayBtSocketPort* bp;
                        bp = GWENG_BtEcmtAgentCheck(devName, qs, &foundPorts);
                        if (bp) {
                            EcmtGatewayPort* p = &bp->common;
                            EcmtGatewayBtSocketPort* bp2;
                            bp->common.gw = gw;
                            bp2 = HASH_Get(&knownPorts, &bp->btaddr);
                            if (bp2) {
                                EcmtGatewayPort* p2 = &bp2->common;
                                if (StrCmp(p->name, p2->name) ||
                                    StrCmp(p->comment, p2->comment) ||
                                    bp->btport != bp2->btport) {

                                    char* tmp = p->name;
                                    p->name = p2->name;
                                    p2->name = tmp;

                                    tmp = p->comment;
                                    p->comment = p2->comment;
                                    p2->comment = tmp;

                                    bp2->btport = bp->btport; 

                                    if (gw->cb.portModified) {
                                        gw->cb.portModified(gw, p2, gw->ctx);
                                    }
                                }
                            } else {
                                /* new service */
                                GWENG_PortAddRef(p);
                                MUTEX_Lock(&bt->common.mutex);
                                QUEUE_InsertTail(&bt->common.ports,&p->entry);
                                MUTEX_Unlock(&bt->common.mutex);
                                if (gw->cb.portFound) {
                                    gw->cb.portFound(gw, p, gw->ctx);
                                }
                            }
                            break;
                        }
                    }
                    if (WSALookupServiceEnd(hSvc)) {
	                    TRACE1("GWENG: WSALookupServiceEnd error %d\n",
                            GetLastError());
                    }
                } else {
	                TRACE1("GWENG: WSALookupServiceBegin error %d\n",
                        GetLastError());
                }
            }
            MEM_Free(devName);
            len = sizeof(qsbuf);
        }
        if (WSALookupServiceEnd(hDeviceLoop)) {
	        TRACE1("GWENG: WSALookupServiceEnd error %d\n", GetLastError());
        }
    } else {
	    TRACE1("GWENG: WSALookupServiceBegin error %d\n", GetLastError());
    }

    /* Remove the dead ports from the list */
    entries = HASH_Entries(&knownPorts);
    if (entries) {
        while (ITR_HasNext(entries)) {
            HashEntry* entry = ITR_Next(entries);
            if (!HASH_Contains(&foundPorts, entry->key)) {
                EcmtGatewayPort* port = entry->value;
                if (gw->cb.portDisappeared) {
                    gw->cb.portDisappeared(gw, port, gw->ctx);
                }
                ITR_Remove(entries);
                MUTEX_Lock(&bt->common.mutex);
                QUEUE_RemoveEntry(&port->entry);
                MUTEX_Unlock(&bt->common.mutex);
                GWENG_PortReleaseRef(port);
            }
        }
        ITR_Delete(entries);
    }

    HASH_Destroy(&knownPorts);
    HASH_Destroy(&foundPorts);
    GWENG_BtSockFinishRefresh(bt);
}

/* Initiates port detection */
STATIC void GWENG_BtSockTransportRefresh(EcmtGatewayTransport* t)
{
    EcmtBtSocketTransport* bt = CAST(t,EcmtBtSocketTransport,common);
    
    /* Don't even create refresh thread if Bluetooth sockets don't work */
    if (!bt->refresh) {
        WSAPROTOCOL_INFO pi;
        if (GWENG_BtGetProtocolInfo(&pi)) {
            bt->refresh = WKQ_Create();
            if (bt->refresh) {
                bt->refreshItem = WKI_Create(bt->refresh,GWENG_BtSockScan,bt);
                if (!bt->refreshItem) {
                    WKQ_Delete(bt->refresh);
                    bt->refresh = NULL;
                }
            }
        }
    }

    if (bt->refresh) {
        Bool started = False;
        MUTEX_Lock(&bt->common.mutex);
        if (WKI_Submit(bt->refreshItem)) {
            bt->refreshInProgress = True;
            started = True;
        }
        MUTEX_Unlock(&bt->common.mutex);
        if (started) {
            GWENG_RefreshStarted(bt->common.gw);
        }
    }
}

STATIC void GWENG_BtSockTransportFree(EcmtGatewayTransport* t)
{
    EcmtBtSocketTransport* bt = CAST(t,EcmtBtSocketTransport,common);
    if (bt->refresh) {
        bt->exiting = True;
        WKI_Cancel(bt->refreshItem);
        WKI_Detach(bt->refreshItem);
        WKQ_Delete(bt->refresh);
    }
    GWENG_TransportDestroy(&bt->common);
    MEM_Free(bt);
}

STATIC EcmtTransport gwBtSocketTransport = {
    "BtSocketTransport",            /* name */
    GWENG_BtSockTransportRefresh,   /* refresh */
    GWENG_BtSockTransportFree       /* free */
};

/**
 * Creates Bluetooth socket family
 */
EcmtGatewayTransport* GWENG_CreateBtSocketTransport(EcmtGateway* gw)
{
    EcmtBtSocketTransport* bt = MEM_New(EcmtBtSocketTransport);
    if (bt) {
        memset(bt, 0, sizeof(*bt));
        if (MUTEX_Init(&bt->common.mutex)) {
            QUEUE_Init(&bt->common.ports);
            bt->common.transport = &gwBtSocketTransport;
            bt->common.gw = gw;

            /*
             * Note: bt->refresh and bt->refreshItem are allocated later
             * and only in case if Bluetooth sockets actually work
             */
            return &bt->common;
        }
        MEM_Free(bt);
    }
    return NULL;
}

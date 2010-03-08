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


#ifndef _ECMT_GW_ENGINE_PRIVATE_H_
#define _ECMT_GW_ENGINE_PRIVATE_H_

#include "..\..\..\..\NmitShared\src\native\slib\include\s_lib.h"
#include "GwEngine.h"
#include "..\..\..\..\NmitShared\src\native\xrpc\include\xrpc.h"
#include "..\..\..\..\NmitShared\src\native\xrpc\xreg\include\xregapi.h"
#include "..\..\..\..\NMIT\lib\include\xdscli.h"

/**
 * Low level I/O abstraction
 */
typedef struct _EcmtIo EcmtIo;
typedef struct _EcmtConnection {
    const EcmtIo* io;               /* pointer to the callbacks */
    long ref;                       /* reference count */
    EcmtGateway* gw;                /* back pointer to the gateway context */
    EcmtGatewayPort* port;          /* associated port object (if any) */
} EcmtConnection;

/* Low level I/O callbacks */
typedef int  (*EcmtIoRead) (EcmtConnection* con, void* buf, int size);
typedef Bool (*EcmtIoWrite) (EcmtConnection* con, const void* data, int size);
typedef void (*EcmtIoCancel) (EcmtConnection* con);
typedef Bool (*EcmtIoHandset) (EcmtConnection* con);
typedef void (*EcmtIoClose) (EcmtConnection* con);

struct _EcmtIo {
    Str name;
    EcmtIoRead read;                /* read data */
    EcmtIoWrite write;              /* write complete message */
    EcmtIoCancel cancel;            /* cancel I/O */
    EcmtIoHandset handset;          /* check for handset I/O */
    EcmtIoClose close;              /* close the connection */
};

/* I/O API */
#define GWENG_IoRead(c,p,n)         ((c)->io->read(c,p,n))
#define GWENG_IoWrite(c,p,n)        ((c)->io->write(c,p,n))
#define GWENG_IoCancel(c)           ((c)->io->cancel(c))
#define GWENG_IoHandset(c)          ((c)->io->handset(c))
#define GWENG_IoClose(c)            ((c)->io->close(c))
extern void GWENG_IoAddRef (EcmtConnection* c);
extern Bool GWENG_IoReleaseRef (EcmtConnection* c);

/**
 * Communication port abstraction
 */
typedef struct _EcmtPort EcmtPort;
struct _EcmtGatewayPort {
    QEntry entry;                   /* list entry */
    long ref;                       /* reference count */
    Char* name;                     /* port name */
    Char* comment;                  /* port comment */
    const EcmtPort* port;           /* port type descriptor and callbacks */
    EcmtGateway* gw;                /* back pointer to the gateway context */
};

/* Communication port callbacks */
typedef Bool (*EcmtPortCanOpen) (const EcmtGatewayPort* port);
typedef EcmtConnection* (*EcmtPortConnect) (EcmtGatewayPort* port);
typedef void (*EcmtPortFree) (EcmtGatewayPort* port);

typedef struct _EcmtPort {
    Str name;                       /* transport name, for debugging*/
    EcmtGwPortType type;            /* port type for display purposes */
    EcmtPortCanOpen canOpen;        /* tests if the port can be opened */
    EcmtPortConnect connect;        /* opens a new connection */
    EcmtPortFree free;              /* deallocates the port structure */
} EcmtPort;

/* Port API */
#define GWENG_PortType(p)           ((p)->port->type)
#define GWENG_PortName(p)           ((p)->name)
#define GWENG_PortComment(p)        ((p)->comment)
#define GWENG_PortCanOpen(p)        ((p)->port->canOpen(p))
#define GWENG_PortConnect(p)        ((p)->port->connect(p))
#define GWENG_PortFree(p)           ((p)->port->free(p))
extern void GWENG_PortAddRef (EcmtGatewayPort* port);
extern Bool GWENG_PortReleaseRef (EcmtGatewayPort* port);
extern void GWENG_PortDestroy (EcmtGatewayPort* port);

/**
 * Family of communication ports
 */
typedef struct _EcmtTransport EcmtTransport;
typedef struct _EcmtGatewayTransport {
    const EcmtTransport* transport;
    EcmtGateway* gw;                /* back pointer to the gateway context */
    Queue ports;                    /* list of ports */
    Mutex mutex;                    /* protects the port list */
} EcmtGatewayTransport;

typedef void (*EcmtTransportRefresh) (EcmtGatewayTransport* t);
typedef void (*EcmtTransportFree) (EcmtGatewayTransport* t);
struct _EcmtTransport {
    Str name;                       /* transport name, for debugging*/
    EcmtTransportRefresh refresh;   /* initiate port search */
    EcmtTransportFree free;         /* deallocate the transport */
};

/* Port Family API */
#define GWENG_TransportFree(t)     (t ? ((t)->transport->free(t)) : NOTHING)
#define GWENG_TransportRefresh(t)  ((t)->transport->refresh(t))
extern EcmtGatewayPort* GWENG_PortByName(EcmtGatewayTransport* t, Str name, Bool ref);
extern void GWENG_TransportDestroy(EcmtGatewayTransport* t);

/* ECMT protocol level notifications */
extern void GWENG_IoPacketStart(EcmtConnection* c, int uid, const void* data, int len);
extern void GWENG_IoPacketSent(EcmtConnection* c);
extern void GWENG_IoHeaderReceived(EcmtConnection* c, int uid, int len);
extern void GWENG_IoPacketReceived(EcmtConnection* c, int uid, const void* data, int len);

/**
 * Ecmt client connection context
 */
typedef struct _EcmtClient {
    QEntry entry;                   /* queue entry */
    EcmtConnection* conn;           /* client socket I/O */
    ThrID thread;                   /* thread reading from client socket */
} EcmtClient;

/**
 * Gateway context
 */
typedef struct _EcmtReader EcmtReader;
typedef struct _EcmtQueue EcmtQueue;

struct _EcmtGateway {
    EcmtGwCallbacks cb;             /* callbacks */
    void* ctx;                      /* context for the callbacks */
    char* productId;                /* how we identify ourselves */
    Socket ecmtSocket;              /* listening Ecmt socket */
    int ecmtPort;                   /* TCP port for Ecmt clients to connect */
    ThrID listenThread;             /* Thread on which Ecmt listens */
    ThrID outputThread;             /* Thread on which we write to COM port */
    Queue ecmtClients;              /* list of connected Ecmt clients */
    EcmtQueue* handsetQueue;        /* writes data to the handset */
    EcmtConnection* handsetQueueIo; /* sends data to handsetQueue */
    ThrID readThread;               /* thread reading data from the handset */
    EcmtConnection* activeCon;      /* active connection, NULL if none */
    EcmtConnection* multiplexor;    /* forwards data to clients */
    EcmtConnection* clientQueueIo;  /* sends data to clientQueue */
    EcmtQueue* clientQueue;         /* writes data to ECMT clients */
    EcmtGatewayTransport* comPorts; /* serial ports */ 
    EcmtGatewayTransport* btSockets;/* Bluetooth sockets */
		EcmtGatewayTransport* lanSockets;/* Bluetooth sockets */
    int refreshInProgress;          /* how many transports being refreshed */
    Mutex mutex;
    XRpcServer* xrpc;
    XRpcRegistry* xreg;
    int eventMask;
    HashTable listenerMap;          /* XRpcSessions -> mask */
    HashTable midpSessionMap;       /* MidpSessionKey -> MidpSession */
    HashTable ecmtSessionMap;       /* CLID -> MidpSession */
};

typedef struct _MidpSessionKey {
    int xrpcSid;                    /* session id assigned by client */
    XRpcSession* xrpcSession;       /* XRPC sessioin that owns it */
} MidpSessionKey;

typedef struct _MidpSession {
    MidpSessionKey key;             /* idenifies the session */
    I32u sid;                       /* ECMT session id */
    HashTable connMap;              /* maps connId -> MidpConnection */
    Mutex xrpcMutex;                /* mutex for XRPC queue */
    Queue xrpcQueue;                /* pending XRPC calls */
    WorkQueue* xrpcWorkThread;      /* work thread for outgoing XRPC calls */
    WorkItem* xrpcWorkItem;         /* main work item in the queue */
} MidpSession;

typedef struct _MidpConnection {
    I32u connId;
    I32u outCount;                  /* outgoing packet count */
    I32u inCount;                   /* incoming packet count */
} MidpConnection;

typedef Bool (*ReaderFilter) (EcmtReader* reader, int uid, const void* data, int datalen);
typedef void (*ReaderDone) (EcmtReader* reader, Bool protErr);
struct _EcmtReader {
    EcmtGateway* gw;
    EcmtConnection* in;
    EcmtConnection* out;
    ReaderFilter filter;
    ReaderDone done;
};

#define COM_PORT_NAME_LEN     20
#define COM_PORT_OPEN_MODE    "rwb"
/*
Message for disconnecting USB connection. Ecmt Server USB bearer plug-in sends similar 
message on situation where Ecmt Server is going down and closing USB socket. 

This definition must be EXACTLY same as defined in USB bearer plug-in (USBSocket.cpp)!!

History for future EcmtGw developers:
First attemp to notify EcmtGw about Ecmt Server shut down was to use serial communication 
lines DTR and DSR (by setting DTR->1 at Ecmt Server side (device), and monitoring DSR on
EcmtGw side). DSR did not change on windows side, so it was not possible to monitor it. 
Second attemp was to use break signal from Ecmt Server side. It was not possible to 
notice this signal at EcmtGw side. Monitoring of DSR and Break signal changes is done in
GwIoPort.cpp, in GWENG_IoPortWait. 
So the final and the third attemp/(hack) was to send following message to the EcmtGw when 
Ecmt Server is going down. This works. Downside of this is that the disconnect message 
must be continuously sought from incoming messages :-( 
*/
#define ECMT_USB_DISCONNECT_MSG "ECMT-DISCONNECT"
#define FORMATED_HEX_LEN      10
#define ECMT_MSG_HEADER_SIZE (2*FORMATED_HEX_LEN) 
#define PUSH_URL_PLUGIN_UID   0x1020D6CE
#define EPDT_CORE_UID         0x10206FEF

/* Notifications (ECMT level trace) */
typedef struct _GwNotify GwNotify;
extern const GwNotify _GwNotifySend;
extern const GwNotify _GwNotifyReceive;
#define GwNotifySend (&_GwNotifySend)
#define GwNotifyReceive (&_GwNotifyReceive)
extern void GWENG_Notify(EcmtGateway* gw, const GwNotify* notify,
                         int uid, const void* data, int len);

/* Communication port families */
extern EcmtGatewayTransport* GWENG_CreateComTransport(EcmtGateway* gw);
extern EcmtGatewayTransport* GWENG_CreateBtSocketTransport(EcmtGateway* gw);
extern EcmtGatewayTransport* GWENG_CreateLanSocketTransport(EcmtGateway* gw);

/* Connections */
extern EcmtConnection* GWENG_CreateComPortConnection(EcmtGatewayPort* port);
extern EcmtConnection* GWENG_CreateSocketConnection(EcmtGateway* gw, Socket s);
extern EcmtConnection* GWENG_CreateHandsetSocketConnection(EcmtGatewayPort* port, Socket s);
extern EcmtConnection* GWENG_CreateQueueConnection(EcmtGateway* gw, EcmtQueue* q);
extern EcmtConnection* GWENG_CreateMultiplexor(EcmtGateway* gw);

/* Queue */
extern EcmtQueue* GWENG_QueueCreate(EcmtConnection* out);
extern void GWENG_QueueDelete(EcmtQueue* q);
extern void GWENG_QueueSetConnection(EcmtQueue* q, EcmtConnection* out);
extern Bool GWENG_QueueAdd(EcmtQueue* q, int uid,
                           const void* data, int size);
extern Bool GWENG_QueueAdd2(EcmtQueue* q, int uid, 
                            const void* data1, int size1,
                            const void* data2, int size2);

/* Other internal functions */
extern Str GWENG_FormatComPortName(int port, Char* buf);
extern Bool GWENG_CreateEcmtListeningSocket(EcmtGateway* gw);
extern void GWENG_Reader(void* ctx);

extern void GWENG_RefreshStarted(EcmtGateway* gw);
extern void GWENG_RefreshFinished(EcmtGateway* gw);
extern Bool GWENG_ParseEcmtHeader(Str data, int* uid, int* len);
extern Bool GWENG_StartXRPC(EcmtGateway* gw);
extern Bool GWENG_MidpRegisterXRpcProtocol(EcmtGateway* gw);
extern Bool GWENG_MidpFilter(EcmtReader* reader, int uid, 
                             const void* data, int datalen);

#endif /* _ECMT_GW_ENGINE_PRIVATE_H_ */

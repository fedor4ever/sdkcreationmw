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


#ifndef _XRPC_REG_PRIVATE_H_
#define _XRPC_REG_PRIVATE_H_ 1

#include "s_lib.h"
#include "xreg.h"
#include "xregapi.h"

/* *************************************************************************
//
//  Private XREG header file
//
//  Basic idea: server and client parts of XRPC registry are ABSOLUTELY
//  independent on each other even if they exists in context of the same
//  process. This significantly simplifies the implementation (as opposed
//  to treating local and remote interactions differently) although it does 
//  have some impact on performance. It has been decided that simplicity 
//  and clarity (which translates into reliability, fewer bugs and easier 
//  maintanence) have higher priority than performance, because such 
//  actions as registering or unregistering a service don't occur very 
//  often, typically only at startup.
//
// *************************************************************************/

typedef struct _XRegWorkItem XRegWorkItem;
typedef struct _XRegServer XRegServer;
typedef struct _XRegClient XRegClient;

/*
 * Event queue.
 *
 * NOTE: cancel routine is invoked under synchronization, it cannot do
 * anything that would block the current thread.
 */
typedef struct _XRegWorkQueue {
    Lock * lock;                    /* synchronization lock */
    Queue items;                    /* list of XRegWorkItem structures */
    WorkQueue * q;                  /* worker thread */
    Event event;                    /* set after work item is complete */
    Bool posted;                    /* work item has been submitted */
} XRegWorkQueue;

typedef void (*XRegWorkProc)(XRegWorkItem * w); 
typedef struct _XRegWorkItemCB {    /* work items callbacks */
    XRegWorkProc run;               /* runs and deallocates the work item */
    XRegWorkProc cancel;            /* just deallocates the work item */
} XRegWorkItemCB;

struct _XRegWorkItem {
    QEntry entry;                   /* list entry */
    const XRegWorkItemCB * cb;      /* work item callbacks */
};

/* 
 * Service information
 */
typedef struct _XRegService {
    char *   id;                    /* service ID assigned by the registry */
    char *   name;                  /* service protocol name */
    char *   host;                  /* host name or NULL if local host */
    XRpcPort port;                  /* the listening port number */    
    Prop     props;                 /* properties */
} XRegService;

/* XRPC Registry Server API */
XRegServer * XREG_CreateServer(XRpcPort port);
void XREG_FreeServer(XRegServer * server);

/* XRPC Registry Client API */
XRegClient * XREG_CreateClient(void);
void XREG_Connect(XRegClient * client, XRpcSocket socket);
void XREG_ReceiveEvents(XRegClient * client, XRegEventProc cb, void * ctx);
void XREG_Disconnect(XRegClient * client);
void XREG_RunClient(XRegClient * client);
void XREG_FreeClient(XRegClient * client);

/* XRPC Client calls */
XRpcCall * XREG_CallLock(XRegClient * client, XRpcString name);
XRpcCall * XREG_CallUnlock(XRegClient * client);
XRpcCall * XREG_CallListServices(XRegClient * client, XRpcString name);
XRpcCall * XREG_CallQueryService(XRegClient * client, XRpcString id);
XRpcCall * XREG_CallUnregisterListener(XRegClient * client, XRpcString prot);
XRpcCall * XREG_CallUnregisterService(XRegClient * client, XRpcString id);
XRpcCall * XREG_CallRegisterListener(XRegClient * client, XRpcString prot);
XRpcCall * 
XREG_CallSetProp(
    XRegClient *    client,         /* XRPC Registry client */
    XRpcString      id,             /* service id */
    XRpcString      name,           /* property name */
    XRpcString      value           /* property value */
);
XRpcCall * 
XREG_CallClearProp(
    XRegClient *    client,         /* XRPC Registry client */
    XRpcString      id,             /* service id */
    XRpcString      name            /* property name */
);
XRpcCall * 
XREG_CallRegisterService(
    XRegClient *    client,         /* XRPC Registry client */
    XRpcString      id,             /* service id, NULL to generate one */
    XRpcString      name,           /* service protocol name */
    XRpcString      host,           /* host name or (typically) NULL */
    XRpcPort        port,           /* listening port in host byte order */
    const XRegProperty * p,         /* service properties */
    int             np              /* number of properties */
);
XRegServiceInfo * 
XREG_ParseQueryResult(
    XRegClient *    client,         /* XRPC Registry client */
    const XRpcContainer * svc       /* XRPC-encoded xrpc.reg.ServiceInfo */
);

/* Work items API */
Bool XREG_InitWorkQueue(XRegWorkQueue * q, Lock * lock);
void XREG_DestroyWorkQueue(XRegWorkQueue * q);
void XREG_PostWorkItem(XRegWorkQueue * q, XRegWorkItem * w);

/* Utilities */
XRpcElement * XREG_EncodeProperties(const XRegProperty * p, int np);
XRpcElement * XREG_EncodeProp(Prop * prop);
XRpcElement * XREG_EncodeProps(Prop * p1, Prop * p2);
Bool XREG_EqualService(const XRegService * s1, const XRegService * s2);
Bool 
XREG_InitRemoteServiceInfo(
    const XRpcContainer * svc,  /* XRPC-encoded xrpc.reg.ServiceInfo */
    XRegServiceInfo * info,     /* the structure to be initialized */
    Vector * propVector         /* vector to hold XRegProperty structures */
);

#endif /* _XRPC_REG_PRIVATE_H_ */

/*
 * Local Variables:
 * mode:C
 * c-basic-offset:4
 * indent-tabs-mode: nil
 * End:
 */

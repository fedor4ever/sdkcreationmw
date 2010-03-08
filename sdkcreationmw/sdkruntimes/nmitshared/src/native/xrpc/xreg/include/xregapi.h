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


#ifndef _XRPC_REG_API_H_
#define _XRPC_REG_API_H_ 1

/*
 * Check for _XRPC_H_ may be useful if the xrpc/include directory is not in
 * the include path and header files are being included with statements like
 *
 * #include "xrpc/include/xrpc.h"
 * #include "xrpc/xreg/include/xregapi.h"
 */
#ifndef _XRPC_H_
#include "xrpc.h"
#endif /* _XRPC_H_ */

/* using the same calling convention as XRPC library */
#define XREG_API XRPC_API

/* type of XRpcRegistry */
typedef enum _XRegType {
    XRegTypeServer,         /* server only */
    XRegTypeClient,         /* client only */
    XRegTypeAny,            /* server or client */
    XRegTypeDefault = XRegTypeAny
} XRegType;

/* opaque types representing internal XRPC registry objects */
typedef struct _XRpcRegistry XRpcRegistry;

/* property (name-value pair) */
typedef struct _XRegProperty {
    XRpcString      name;       /* property name */
    XRpcString      value;      /* property value */
} XRegProperty;

/* information about registered XRPC service */
typedef struct _XRegServiceInfo {
    XRpcString      id;         /* service ID assigned by the registry */
    XRpcString      name;       /* service protocol name */
    XRpcString      host;       /* host name or NULL if local host */
    XRpcPort        port;       /* the listening port number */
    XRegProperty *  props;      /* service properties */
    int             nprops;     /* number of properties */
} XRegServiceInfo;

/* XRPC registry events */

typedef enum _XRegEventType {
    XRegEventRegisterService,   /* new service has registered */
    XRegEventUnregisterService, /* service has been unregistered */
    XRegEventRebind,            /* reconnected to XRPC registry */
    XRegEventShutdown           /* XRPC registry is gone */
} XRegEventType;

typedef struct _XRegServiceEvent {
    XRegEventType type;         /* event type */
    XRegServiceInfo service;    /* service information */
} XRegRegisterServiceEvent, XRegUnregisterServiceEvent;
    
typedef struct _XRegSimpleEvent {
    XRegEventType type;         /* XRegEventShutdown, XRegEventRebind */
} XRegShutdownEvent, XRegRebindEvent;

typedef union _XRegEvent {
    XRegEventType type;
    XRegUnregisterServiceEvent unregisterService;
    XRegRegisterServiceEvent registerService;
    XRegRebindEvent rebind;
    XRegShutdownEvent shutdown;
} XRegEvent;

/* 
 * Standard XRPC registry port. This port number has been assigned to XRPC
 * by IANA. The complete list of assigned port numbers can be found at
 * http://www.iana.org/assignments/port-numbers
 */
#define XREG_DEFAULT_PORT   3651

/* callback functions */
typedef 
XRpcBool (
XREG_API * 
XRegListProc)(
    const XRegServiceInfo * info,
    void *              context
);

typedef 
void (
XREG_API * 
XRegEventProc)(
    const XRegEvent *   event,
    void *              context
);

/* *************************************************************************
//
//  API to the XRPC Registry library
//
// *************************************************************************/

/*
 * Creates an instance of XRpcRegistry listening on or connected to the
 * specified port. Returns pointer to the created instance of XRpcRegistry
 * or NULL if something fails, for example if you specify XRegTypeServer
 * object type and a port that is currently in use.
 */
XRpcRegistry *
XREG_API
XREG_CreateRegistry(
    XRpcPort        port,       /* XRPC Registry port in host byte order */
    XRegType        type        /* type of object */
);

/**
 * Creates an instance of XRpcRegistry client cinnected to the registry
 * running on the specified host and listening on the specified port.
 * NULL host is interpeted as localhost. Returns pointer to the created
 * instance of XRpcRegistry or NULL if something fails, for example if
 * connection to the registry server fails.
 */
XRpcRegistry *
XREG_API
XREG_ConnectRegistry(
    XRpcString      host,       /* XRPC Registry host */
    XRpcPort        port        /* XRPC Registry port */
);

/*                                  
 * Deallocates XRpcRegistry
 */
void
XREG_API
XREG_FreeRegistry(
    XRpcRegistry *  reg         /* instance of XRpcRegistry */
);

/*
 * Returns True if XRpcRegistry is currently being a server. 
 * If XRpcRegistry has XRegTypeClient, this call will always
 * return False, for XRegTypeServer is always returns True and
 * in case of XRegTypeAny the result depends on whether the
 * registry is running in client or server mode.
 */
XRpcBool
XREG_API
XREG_IsServer(
    XRpcRegistry *  reg         /* instance of XRpcRegistry */
);

/*
 * Returns True if XRpcRegistry is currently being a client. 
 * If XRpcRegistry has XRegTypeServer, this call will always
 * return False, for XRegTypeClient is always returns True if
 * connected and in case of XRegTypeAny the result depends on 
 * whether the registry is running in client or server mode.
 */
XRpcBool
XREG_API
XREG_IsClient(
    XRpcRegistry *  reg         /* instance of XRpcRegistry */
);

/*
 * Registers an XRPC service with the registry. Returns the id
 * assigned by XRPC registry to the service, NULL in case of failure.
 * The id can be used to unregister the service. When id is not needed
 * anymore, it has to be deallocated with XREG_Free
 */
char *
XREG_API
XREG_RegisterService(
    XRpcRegistry *  reg,        /* instance of XRpcRegistry */
    XRpcString      service,    /* service protocol name */
    XRpcString      host,       /* host name or (typically) NULL */
    XRpcPort        port,       /* listening port in host byte order */
    const XRegProperty * p,     /* service properties */
    int             np          /* number of properties */
);

/*
 * Unregisters the XRPC service previously registered with 
 * XREG_RegisterService call.
 */
void
XREG_API
XREG_UnregisterService(
    XRpcRegistry *  reg,        /* instance of XRpcRegistry */
    XRpcString      id          /* service id assigned by the registry */
);

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
    void *          context     /* passed to the callback */
);

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
    XRpcString      id          /* service to find */
);

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
    void *          context     /* passed to the callback */
);

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
    XRpcString      value       /* the value of the property */
);

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
    XRpcString      name        /* the name of the property */
);

/*
 * Acquires named mutex. If the name parameter is NULL or an empty
 * string, acquires a global XRPC registry mutex. It's guaranteed 
 * that no services will be registered or unregistered while client
 * is holding the global mutex. A client is not allowed to hold 
 * more than one mutex.
 */
XRpcBool 
XREG_API
XREG_Lock(
    XRpcRegistry *  reg,        /* instance of XRpcRegistry */
    XRpcString      name        /* name of the mutex */
);

/*
 * Releases the mutex held by the client.
 */
void
XREG_API
XREG_Unlock(
    XRpcRegistry *  reg         /* instance of XRpcRegistry */
);

/*
 * Deallocates memory allocated by XREG library. Ignores NULL pointers.
 */
void 
XREG_API
XREG_Free(
    void *          ptr         /* memory block to deallocate */
);

/* *************************************************************************
//
//  Utilities
//
// *************************************************************************/

/*
 * Finds the value of the property with the specified name. Returns NULL
 * if property is not found
 */
XRpcString
XREG_API
XREG_GetPropValue(
    const XRegProperty * props, /* properties to scan */
    int nprops,                 /* number of properties */
    XRpcString name             /* name of the property */
);

/*
 * Finds the value of the property with the specified name. Returns NULL
 * if property is not found
 */
XRpcString
XREG_API
XREG_GetServiceProp(
    const XRegServiceInfo* svc, /* service info */
    XRpcString name             /* name of the property */
);

#endif /* _XRPC_REG_API_H_ */

/*
 * Local Variables:
 * mode: C
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

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


#ifndef _ECMT_GW_ENGINE_H_
#define _ECMT_GW_ENGINE_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifdef _WIN32
#  define ECMTGW_API __cdecl
#else
#  define ECMTGW_API
#endif

/* Boolean type */
typedef enum _EcmtBool {
    EcmtFalse = 0,
    EcmtTrue  = 1,
} EcmtBool;

#define IP_ADDR_LEN 22

/**
 * Defines the direction in which ECMTGW_SendMessage sends the packet.
 */
typedef enum _EcmtDirection {
    EcmtDirectionToHandset, /* send packet to the handset */
    EcmtDirectionToClients, /* send message to all connected clients */
} EcmtDirection;

/**
 * Reason for deactivation.
 */
typedef enum _EcmtGwDeactReason {
    EcmtGwDeactRequest,     /* user request, such as ECMTGW_Activate call */
    EcmtGwDeactIOErr,       /* COM port I/O error */
    EcmtGwDeactProtErr,     /* Ecmt protocol error */
} EcmtGwDeactReason;

/**
 * Statistics
 */
typedef struct _EcmtGatewayStat {
    int connTime;           /* connection time, in seconds */
    int sentBytes;          /* bytes sent to the COM port */
    int sentPackets;        /* Ecmt packets sent to the COM port */
    int recvBytes;          /* bytes received from COM port */
    int recvPackets;        /* Ecmt packets received from COM port */
} EcmtGatewayStat;

/* Port types */
typedef enum _EcmtGwPortType {
    EcmtGwPortBtSocket,     /* Bluetooth RFCOMM socket */
    EcmtGwPortSerial,       /* COM port */
    EcmtGwPortSocket,       /* TCP/IP socket */
    EcmtGwPortOther         /* Something I couldn't imagine :) */
} EcmtGwPortType;

/**
 * Opaque handles
 */
typedef struct _EcmtGateway EcmtGateway;
typedef struct _EcmtGatewayPort EcmtGatewayPort;

/**
 * Callbacks for receiving event notifications from the gateway engine.
 * The callbacks can be invoked in arbitrary thread context and possibly
 * under a lock. They shouldn't do anything that may block the current
 * thread.
 */

/**
 * Most callbacks have this format
 */
typedef 
void (
ECMTGW_API*
EcmtGwEventCB)(
    EcmtGateway* gw,        /* the gateway sending the notification */
    void* ctx               /* the context passed to ECMTGW_Create */
    );

/**
 * Port related callback.
 */
typedef
void (
ECMTGW_API* 
EcmtGwPortCB)(
    EcmtGateway* gw,        /* the gateway sending the notification */
    EcmtGatewayPort* port,  /* port in question */
    void* ctx               /* the context passed to ECMTGW_Create */
    );

/**
 * Invoked when a new Ecmt client is connected. It's ok if this callback
 * if only invoked once after several clients have connected. The actual
 * number of currently connected clients can be retrieved by calling the
 * ECMTGW_CountEcmtClients function.
 */
typedef EcmtGwEventCB EcmtGwClientConnectCB;

/**
 * Invoked when one Ecmt client is disconnected. It's ok if this callback
 * if only invoked once after several clients have disconnected. The actual
 * number of currently connected clients can be retrieved by calling the
 * ECMTGW_CountEcmtClients function.
 */
typedef EcmtGwEventCB EcmtGwClientDisconnectCB;

/**
 * Invoked when gateway has been activated, i.e. the specified port has been
 * successfully connected.
 */
typedef EcmtGwPortCB EcmtGwActivateCB;

/**
 * Invoked when gateway has been deactivated, either as a result of calling
 * ECMTGW_Deactivate, I/O error or Ecmt protocol violation.
 */
typedef
void (
ECMTGW_API* 
EcmtGwDeactivateCB)(
    EcmtGateway* gw,        /* the gateway sending the notification */
    EcmtGwDeactReason why,  /* reason for deactivation */
    void* ctx               /* the context passed to ECMTGW_Create */
    );

/**
 * Invoked when gateway detects a new communication port in the system.
 */
typedef EcmtGwPortCB EcmtGwPortFoundCB;

/**
 * Invoked when gateway detects that a communication port parameters
 * (such as name and/or comment) have been modified.
 */
typedef EcmtGwPortCB EcmtGwPortModifiedCB;

/**
 * Invoked when gateway detects that a communication port is gone.
 */
typedef EcmtGwPortCB EcmtGwPortDisapearedCB;

/**
 * Port refresh start/stop notifications
 */
typedef EcmtGwEventCB EcmtGwRefreshStartCB;
typedef EcmtGwEventCB EcmtGwRefreshDoneCB;

/**
 * Invoked when gateway starts sending Ecmt packet to the COM port.
 */
typedef
void (
ECMTGW_API* 
EcmtGwSendPacketCB)(
    EcmtGateway* gw,        /* the gateway sending the notification */
    int uid,                /* destination UID */
    const void* data,       /* the packet data */
    int datalen,            /* length of the packet data */
    void* ctx               /* the context passed to ECMTGW_Create */
    );

/**
 * Invoked when gateway has finished sending Ecmt packet to the COM port.
 */
typedef EcmtGwEventCB EcmtGwSendPacketCompleteCB;

/**
 * Invoked when gateway has received Ecmt header from the COM port and starts
 * receiving the body of the Ecmt message.
 */
typedef
void (
ECMTGW_API* 
EcmtGwReceivePacketCB)(
    EcmtGateway* gw,        /* the gateway sending the notification */
    int uid,                /* destination UID */
    int datalen,            /* length of the packet data */
    void* ctx               /* the context passed to ECMTGW_Create */
    );

/**
 * Invoked when gateway has finished receiving Ecmt packet from the COM port.
 */
typedef
void (
ECMTGW_API* 
EcmtGwReceivePacketCompleteCB)(
    EcmtGateway* gw,        /* the gateway sending the notification */
    int uid,                /* destination UID */
    const void* data,       /* the packet data */
    int datalen,            /* length of the packet data */
    void* ctx               /* the context passed to ECMTGW_Create */
    );

/**
 * Invoked when the UI needs to attract user's attention. Usually, it's done
 * by bringing main window to front, flashing application icon on the task
 * bar on Windows or "jumping" application icon in the Dock on Mac.
 */
typedef EcmtGwEventCB EcmtGwToFrontCB;

/**
 * Request to close the port and exit.
 */
typedef EcmtGwEventCB EcmtGwExitCB;

/**
 * Event callbacks. Any callback can be NULL, which indicates that the caller
 * if not interested in that particular event.
 */
typedef struct _EcmtGwCallbacks {
    EcmtGwClientConnectCB           connect;
    EcmtGwClientDisconnectCB        disconnect;
    EcmtGwActivateCB                activate;
    EcmtGwDeactivateCB              deactivate;
    EcmtGwPortFoundCB               portFound;
    EcmtGwPortModifiedCB            portModified;
    EcmtGwPortDisapearedCB          portDisappeared;
    EcmtGwRefreshStartCB            refreshStart;
    EcmtGwRefreshDoneCB             refreshDone;
    EcmtGwSendPacketCB              sendPacket;
    EcmtGwSendPacketCompleteCB      sendPacketComplete;
    EcmtGwReceivePacketCB           receivePacket;
    EcmtGwReceivePacketCompleteCB   receivePacketComplete;
    EcmtGwToFrontCB                 toFront;
    EcmtGwExitCB                    exit;
} EcmtGwCallbacks;

/**
 * Creates ECMT Gateway instance and starts listening on a TCP port for
 * incoming TCP connections. Returns NULL on failure. The EcmtGwCallbacks
 * pointer can be NULL, which means that the caller is not interested in
 * receiving any events. If it's not NULL, gateway must copy the contents
 * of this structure into internal storage, because it may be allocated 
 * on stack.
 */
extern 
EcmtGateway* 
ECMTGW_API 
ECMTGW_Create(
    const char* productId,      /* id of the product that uses the gateway */
    const EcmtGwCallbacks* cb,  /* event callbacks, can be NULL */
    void* ctx                   /* context to pass to the event callbacks */
    );

/**
 * Destroys ECMT Gateway instance. Returns NULL pointer.
 */
extern 
void 
ECMTGW_API 
ECMTGW_Destroy(
    EcmtGateway* gw         /* gateway instance */
    );

/**
 * Returns the TCP port on which the gateway is listening for incoming
 * Ecmt connections.
 */
extern 
int 
ECMTGW_API 
ECMTGW_GetEcmtPort(
    EcmtGateway* gw         /* gateway instance */
    );

/**
 * Returns the number of currently connected Ecmt clients.
 */
extern 
int 
ECMTGW_API 
ECMTGW_CountEcmtClients(
    EcmtGateway* gw         /* gateway instance */
    );

/**
 * If the gateway is active, returns the active port. Otherwise, returns NULL.
 */
extern 
EcmtGatewayPort* 
ECMTGW_API 
ECMTGW_GetActivePort(
    EcmtGateway* gw         /* gateway instance */
    );

/**
 * Macro that tests whether gateway is active.
 */
#define ECMTGW_IsActive(gw) (ECMTGW_GetActivePort(gw) != NULL)

/**
 * Activates the gateway by connecting it to the specified port. If 
 * gateway is already connected to the same port, does nothing and
 * returns True. If gateway is already connected to another port, it
 * disconnects it first and then attempts to connect to the specified 
 * port. Returns True if activation was successful, False on failure.
 */
extern 
EcmtBool
ECMTGW_API 
ECMTGW_Activate(
    EcmtGatewayPort* port   /* the port to activate */
    );

/**
 * Deactivates the gateway by closing the currently active port. Does 
 * nothing if gateway is not connected.
 */
extern 
void
ECMTGW_API 
ECMTGW_Deactivate(
    EcmtGateway* gw         /* gateway instance */
    );

/**
 * Initiates the procedure of refreshing the port list. The portFound callback
 * will be invoked on each new port, portDisappeared on each port that has
 * disappeared. The callbacks are invoked in arbitrary thread context.
 * Some callbacks may be invoked on the current thread, before this
 * function returns.
 */
extern 
void
ECMTGW_API 
ECMTGW_RefreshPorts(
    EcmtGateway* gw         /* gateway instance */
    );

/**
 * Sends a ECMT message to either COM port or to all connected clients.
 * Returns True if message was successfully sent, False on failure or
 * if there's nowhere to send the message (gateway not active, or no
 * clients connected).
 */
extern 
EcmtBool
ECMTGW_API 
ECMTGW_SendMessage(
    EcmtGateway* gw,        /* gateway instance */
    int uid,                /* destination UID */
    const void* data,       /* the packet data */
    int datalen,            /* length of the packet data */
    EcmtDirection dir       /* to COM port or to ECMT clients */
    );

/**
 * Returns the port usage statistics. If the gateway isn't currently
 * connected to a port, returns the final statistics for the last 
 * connection. If the gateway is currently active, returns the
 * statistics for the currently connected port.
 */
extern
void
ECMTGW_API 
ECMTGW_Statistics(
    EcmtGateway* gw,        /* gateway instance */
    EcmtGatewayStat* stat   /* receives the statistics */
    );

/**
 * Returns the display name for the specified port. The return string
 * is never NULL.
 */
extern 
const char*
ECMTGW_API 
ECMTGW_GetPortName(
    const EcmtGatewayPort* port
    );

/**
 * Returns the comment for the specified port, NULL if none.
 */
extern 
const char*
ECMTGW_API 
ECMTGW_GetPortComment(
    const EcmtGatewayPort* port
    );

/**
 * Returns the port type
 */
extern 
EcmtGwPortType
ECMTGW_API 
ECMTGW_GetPortType(
    const EcmtGatewayPort* port
    );

/**
 * Tests whether the specified port can be opened.
 */
extern 
EcmtBool 
ECMTGW_API 
ECMTGW_CanOpenPort(
    EcmtGatewayPort* port
    );

/**
 * Tests whether the specified port is currently active
 */
extern 
EcmtBool 
ECMTGW_API 
ECMTGW_IsActivePort(
    const EcmtGatewayPort* port
    );

/**
 * Gets wlan IP & port read from setings file
 */
extern 
EcmtBool 
ECMTGW_API
ECMTGW_GetWlanIpPort(
    char* ip,
		int* port
    );

/**
 * Writes wlan IP & port into setings file
 */
extern 
EcmtBool
ECMTGW_API
ECMTGW_SetWlanIpPort(
    const char* ip,
		int port,
		EcmtGateway* gw
    );

#ifdef __cplusplus
} /* and of extern "C" */
#endif  /* __cplusplus */

#endif /* _ECMT_GW_ENGINE_H_ */

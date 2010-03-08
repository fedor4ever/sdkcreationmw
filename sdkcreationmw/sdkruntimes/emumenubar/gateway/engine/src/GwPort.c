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

/**
 * Returns the display name for the specified port. The return string
 * is never NULL.
 */
const char* ECMTGW_GetPortName(const EcmtGatewayPort* port)
{
    return GWENG_PortName(port);
}

/**
 * Returns the comment for the specified port, NULL if none.
 */
const char* ECMTGW_GetPortComment(const EcmtGatewayPort* port)
{
    return GWENG_PortComment(port);
}

/**
 * Returns the port type
 */
EcmtGwPortType ECMTGW_GetPortType(const EcmtGatewayPort* port)
{
    return GWENG_PortType(port);
}

/**
 * Tests whether the specified port is currently active
 */
EcmtBool ECMTGW_IsActivePort(const EcmtGatewayPort* port)
{
    return (port->gw->activeCon &&  port->gw->activeCon->port == port);
}

/**
 * Tests whether the specified port can be opened.
 */
EcmtBool ECMTGW_CanOpenPort(EcmtGatewayPort* port)
{
    return GWENG_PortCanOpen(port);
}

/**
 * Adds reference to the port object
 */
void GWENG_PortAddRef(EcmtGatewayPort* port)
{
    ASSERT(port->ref > 0);
    InterlockedIncrement(&port->ref);
}

/**
 * Releases the reference and deallocates the object if the reference count
 * becomes zero. Returns True if the object has been deallocated, False 
 * otherwise.
 */
Bool GWENG_PortReleaseRef(EcmtGatewayPort* port)
{
    ASSERT(port && port->ref > 0);
    if (port && InterlockedDecrement(&port->ref) == 0) {
        GWENG_PortFree(port);
        return True;
    } else {
        return False;
    }
}

/**
 * Destroys the contents of the EcmtGatewayPort structure but doesn't
 * deallocate the structure itself.
 */
void GWENG_PortDestroy(EcmtGatewayPort* port)
{
    ASSERT(port->ref < 2);
    QUEUE_RemoveEntry(&port->entry);
    MEM_Free(port->name);
    MEM_Free(port->comment);
}

/**
 * Finds port by name. Optionally, adds a reference to the returned port.
 * The search is case insensitive.
 */
EcmtGatewayPort* GWENG_PortByName(EcmtGatewayTransport* t, Str name, Bool ref)
{
    QEntry* e;
    EcmtGatewayPort* port = NULL;
    MUTEX_Lock(&t->mutex);
    for (e = QUEUE_First(&t->ports); e; e = QUEUE_Next(e)) {
        if (!StrCaseCmp(name, QCAST(e,EcmtGatewayPort,entry)->name)) {
            port = QCAST(e,EcmtGatewayPort,entry);
            if (ref) GWENG_PortAddRef(port);
            break;
        }
    }
    MUTEX_Unlock(&t->mutex);
    return port;
}

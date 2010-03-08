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
#include "..\..\..\..\NmitShared\src\native\wlib\w_lib.h"

#define SERIALCOMM_KEY "HARDWARE\\DEVICEMAP\\SERIALCOMM"

/**
 * COM port
 */

STATIC Bool GWENG_ComPortCanOpen(const EcmtGatewayPort* port)
{
    return FILE_CanOpen(port->name, COM_PORT_OPEN_MODE);
}

STATIC EcmtConnection* GWENG_ComPortConnect(EcmtGatewayPort* port)
{
    return GWENG_CreateComPortConnection(port);
}

STATIC void GWENG_ComPortFree(EcmtGatewayPort* port)
{
    GWENG_PortDestroy(port);
    MEM_Free(port);
}

static EcmtPort gwComPort = {
    "ComPort",                      /* name */
    EcmtGwPortSerial,               /* type */
    GWENG_ComPortCanOpen,           /* canOpen */
    GWENG_ComPortConnect,           /* connect */
    GWENG_ComPortFree               /* free */
};

/**
 * COM port family
 */

/* COM port detection */
STATIC void GWENG_ComTransportRefresh(EcmtGatewayTransport* t)
{
    EcmtGatewayPort* port;
    EcmtGateway* gw = t->gw;
    Iterator* entries;
    HKEY hCommKey;
    QEntry* e;
    StrBuf sb;
    HashTable knownPorts;
    HashTable realPorts;

    STRBUF_Init(&sb);
    HASH_Init(&knownPorts, 0, hashCompareStringKey, stringHashProc, NULL);
    HASH_Init(&realPorts, 0, hashCompareStringKey, stringHashProc, 
        hashFreeKeyValueProc);

    /* Collect the list of known ports */
    for (e = QUEUE_First(&t->ports); e; e = QUEUE_Next(e)) {
        port = QCAST(e,EcmtGatewayPort,entry);
        HASH_Put(&knownPorts, port->comment, port);
    }

    /* Collect the list of (hopefully) real ports */
    hCommKey = REG_OpenKey(HKLM, SERIALCOMM_KEY, KEY_READ);
    if (hCommKey) {
        int i = 0;
        while (REG_GetNextValue(hCommKey, i++, &sb)) {
            Char* deviceName = STRBUF_Dup(&sb);
            if (deviceName) {
                /* The port name must be in format COMxx */
                if (REG_QueryStr(hCommKey, deviceName, &sb) &&
                    STRBUF_StartsWithNoCase(&sb, "COM") &&
                    PARSE_Int(sb.s+3,NULL,10)) {
                    /* This looks like a COM port entry */
                    Char* portName = STRBUF_Dup(&sb);
                    if (portName) {
                        if (HASH_Put(&realPorts, deviceName, portName)) {
                            continue;
                        }
                        MEM_Free(portName);
                    }
                }
                MEM_Free(deviceName);
            }
        }
        REG_CloseKey(hCommKey);
    }

    /* Remove the dead ports from the list */
    entries = HASH_Entries(&knownPorts);
    if (entries) {
        while (ITR_HasNext(entries)) {
            HashEntry* entry = ITR_Next(entries);
            if (!HASH_Contains(&realPorts, entry->key)) {
                port = entry->value;
                if (gw->cb.portDisappeared) {
                    gw->cb.portDisappeared(gw, port, gw->ctx);
                }
                ITR_Remove(entries);
                QUEUE_RemoveEntry(&port->entry);
                GWENG_PortReleaseRef(port);
            }
        }
        ITR_Delete(entries);
    }

    /* Add new ports and detect updated ports */
    entries = HASH_Entries(&realPorts);
    if (entries) {
        while (ITR_HasNext(entries)) {
            HashEntry* entry = ITR_Next(entries);
            port = HASH_Get(&knownPorts, entry->key);
            if (port) {
                Bool modified = False;
                char* tmp;
                if (StrCmp(port->comment, entry->key)) {
                    tmp = STRING_Dup(entry->key);
                    if (tmp) {
                        MEM_Free(port->comment);
                        port->comment = tmp;
                        modified = True;
                    }
                }
                if (StrCmp(port->name, entry->value)) {
                    tmp = STRING_Dup(entry->value);
                    if (tmp) {
                        MEM_Free(port->name);
                        port->name = tmp;
                        modified = True;
                    }
                }

                if (gw->cb.portModified && modified) {
                    gw->cb.portModified(gw, port, gw->ctx);
                }
            } else {
                port = MEM_New(EcmtGatewayPort);
                if (port) {
                    memset(port, 0, sizeof(*port));
                    port->comment = STRING_Dup(entry->key);
                    if (port->comment) {
                        port->name = STRING_Dup(entry->value);
                        if (port->name) {
                            port->ref = 1;
                            port->gw = gw;
                            port->port = &gwComPort;
                            QUEUE_InsertTail(&t->ports, &port->entry);
                            if (gw->cb.portFound) {
                                gw->cb.portFound(gw, port, gw->ctx);
                            }
                            continue;
                        }
                        MEM_Free(port->comment);
                    }
                    MEM_Free(port);
                }
            }
        }
        ITR_Delete(entries);
    }

    STRBUF_Destroy(&sb);
    HASH_Destroy(&realPorts);
    HASH_Destroy(&knownPorts);
}

STATIC void GWENG_ComTransportFree(EcmtGatewayTransport* t)
{
    GWENG_TransportDestroy(t);
    MEM_Free(t);
}

STATIC EcmtTransport gwComPortTransport = {
    "ComPortTransport",                /* name */
    GWENG_ComTransportRefresh,         /* refresh */
    GWENG_ComTransportFree             /* free */
};

/**
 * Creates COM port family
 */
EcmtGatewayTransport* GWENG_CreateComTransport(EcmtGateway* gw)
{
    EcmtGatewayTransport* t = MEM_New(EcmtGatewayTransport);
    if (t) {
        memset(t, 0, sizeof(*t));
        if (MUTEX_Init(&t->mutex)) {
            t->transport = &gwComPortTransport;
            t->gw = gw;
            QUEUE_Init(&t->ports);
            return t;
        }
        MEM_Free(t);
    }
    return NULL;
}

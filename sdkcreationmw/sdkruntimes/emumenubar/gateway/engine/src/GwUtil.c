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
 * Adds reference to the connection object
 */
void GWENG_IoAddRef(EcmtConnection* c)
{
    ASSERT(c->ref > 0);
    InterlockedIncrement(&c->ref);
}

/**
 * Releases the reference and deallocates the object if the reference count
 * becomes zero. Returns True if the object has been deallocated, False 
 * otherwise.
 */
Bool GWENG_IoReleaseRef(EcmtConnection* c)
{
    ASSERT(c && c->ref > 0);
    if (c && InterlockedDecrement(&c->ref) == 0) {
        GWENG_IoClose(c);
        return True;
    } else {
        return False;
    }
}

/**
 * Formats the name which represents the specified COM port in the 
 * OS namespace. The buffer have at least COM_PORT_NAME_LEN characters
 * available. Returns pointer to the buffer.
 */
Str GWENG_FormatComPortName(int port, Char* buf)
{
    Sprintf(buf, TEXT("\\\\.\\COM%d"), port);
    return buf;
}

/**
 * Create listening socket to serve Ecmt connections 
 * with Epdt core. Returns True in case of success, 
 * otherwise False.
 */
Bool GWENG_CreateEcmtListeningSocket(EcmtGateway* gw)
{
    Socket sock;
    if (SOCKET_Create (SOCK_STREAM, 0, 0, &sock)) {
        struct sockaddr_in in;
        int namelen = sizeof(in);
        int err = getsockname(sock,(struct sockaddr*)&in,&namelen);
        ASSERT(!err);
        if (!err) {
            err = listen(sock, SOMAXCONN);
            ASSERT(!err);
            if (!err) {
                gw->ecmtPort = ntohs(in.sin_port);
                gw->ecmtSocket = sock;
                return True;
            }
        }
        SOCKET_Close (sock);
    }
    gw->ecmtPort = -1;
    gw->ecmtSocket = INVALID_SOCKET;
    return False;
}

/**
 * Parses Ecmt message header. Returns uid and message length.
 * The returned length is the length of the whole message: HDR + DATA
 */
Bool GWENG_ParseEcmtHeader(Str data, int* uid, int* len)
{
    /* quick test */
    if (data[0] == '0' && (data[1] == 'x' || data[1] == 'X') &&
        data[10] == '0' && (data[11] == 'x' || data[11] == 'X')) {
        char str_uid[9];
        char str_len[9];
        memcpy(str_len, data + 2, 8);
        memcpy(str_uid, data + 12, 8);
        str_uid[8] = 0;
        str_len[8] = 0;

        if (PARSE_U32(str_len, len, 16) && 
            PARSE_U32(str_uid, uid, 16) &&
            *len >= ECMT_MSG_HEADER_SIZE) {
            return True;
        }
    }
    return False;
}

/**
 * Common deinitialization for all transports. Does not delete the
 * transport structure itself.
 */
void GWENG_TransportDestroy(EcmtGatewayTransport* t)
{
    /* at this point no other threads should be using this object */
    QEntry* e;
    EcmtGateway* gw = t->gw;
    MUTEX_Destroy(&t->mutex);
    while ((e = QUEUE_RemoveHead(&t->ports)) != NULL) {
        EcmtGatewayPort* port = QCAST(e,EcmtGatewayPort,entry);
        if (gw->cb.portDisappeared) {
            gw->cb.portDisappeared(gw, port, gw->ctx);
        }
        VERIFY(GWENG_PortReleaseRef(port));
    }
}

/**
 * Invoked (usually) by the transport when refresh has started.
 * This function can be invoked in arbitrary thread context.
 */
void GWENG_RefreshStarted(EcmtGateway* gw)
{
    if (InterlockedIncrement(&gw->refreshInProgress) == 1) {
        if (gw->cb.refreshStart) {
            gw->cb.refreshStart(gw, gw->ctx);
        }
    }
}

/**
 * Invoked (usually) by the transport when refresh has finished.
 * This function can be invoked in arbitrary thread context.
 */
void GWENG_RefreshFinished(EcmtGateway* gw)
{
    if (InterlockedDecrement(&gw->refreshInProgress) == 0) {
        if (gw->cb.refreshDone) {
            gw->cb.refreshDone(gw, gw->ctx);
        }
    }
}

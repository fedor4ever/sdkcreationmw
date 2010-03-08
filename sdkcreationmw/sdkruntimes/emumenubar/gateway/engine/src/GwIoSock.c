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

/*
 * Internal data structures
 */

typedef struct _EcmtFileConnection {
    EcmtConnection connection;          /* common part */
    Bool handsetIo;
    File* file;
} EcmtFileConnection;

typedef struct _EcmtSocketConnection {
    EcmtFileConnection file;
    Socket socket;
} EcmtSocketConnection;

/**
 * Reads the requested number of bytes from a File.
 */
STATIC int GWENG_IoFileRead(EcmtConnection* con, void* buf, int size)
{
    EcmtFileConnection* fconn = CAST(con,EcmtFileConnection,connection);
    return FILE_Read(fconn->file, buf, size);
}

STATIC Bool GWENG_IoFileWrite(EcmtConnection* con, const void* data, int size)
{
    EcmtFileConnection* fc = CAST(con,EcmtFileConnection,connection);
    if (FILE_Write(fc->file, data, size) == size) {
        TRACE1("GWENG_IoFileWrite: wrote %d bytes\n", size);
        return True;
    } else {
        TRACE("GWENG_IoFileWrite: I/O error\n");
        return False;
    }
}

STATIC void GWENG_IoSocketCancel(EcmtConnection* con)
{
    EcmtSocketConnection* sc = CAST(con,EcmtSocketConnection,file.connection);
    if (sc->socket != INVALID_SOCKET) {
        shutdown(sc->socket, SHUT_RDWR);
        SOCKET_Close(sc->socket);
        sc->socket = INVALID_SOCKET;
    }
}

STATIC Bool GWENG_IoFileHandset(EcmtConnection* con)
{
    EcmtFileConnection* fc = CAST(con,EcmtFileConnection,connection);
    return fc->handsetIo;
}

STATIC void GWENG_IoSocketClose(EcmtConnection* con)
{
    EcmtSocketConnection* sc = CAST(con,EcmtSocketConnection,file.connection);
    GWENG_IoSocketCancel(con);
    FILE_Close(sc->file.file);
    MEM_Free(sc);
}

/**
 * Defines ECMT client socket transport.
 */
STATIC const EcmtIo gwIoSocket = {
    "ClientSocket",          /* name */
    GWENG_IoFileRead,       /* read */
    GWENG_IoFileWrite,      /* write */
    GWENG_IoSocketCancel,   /* cancel */
    GWENG_IoFileHandset,    /* handset */
    GWENG_IoSocketClose,    /* close */
};

/**
 * Defines socket connection to the emulator
 */
STATIC const EcmtIo gwIoEmulatorSocket = {
    "EmulatorSocket",       /* name */
    GWENG_IoFileRead,       /* read */
    GWENG_IoFileWrite,      /* write */
    GWENG_IoSocketCancel,   /* cancel */
    GWENG_IoFileHandset,    /* handset */
    GWENG_IoSocketClose     /* close */
};

/**
 * Creates the client socket transport
 */
STATIC EcmtConnection* GWENG_CreateSockConn(EcmtGateway* gw, 
                                            Socket s, 
                                            Bool handsetIo,
                                            const EcmtIo* io)
{
    EcmtSocketConnection* sc = MEM_New(EcmtSocketConnection);
    if (sc) {
        memset(sc, 0, sizeof(*sc));
        sc->socket = s;
        sc->file.file = FILE_AttachToSocket(s);
        if (sc->file.file) {
            sc->file.handsetIo = handsetIo;
            sc->file.connection.io = io;
            sc->file.connection.ref = 1;
            sc->file.connection.gw = gw;
            return &sc->file.connection;
        }
        MEM_Free(sc);
    }
    return NULL;
}

/**
 * Creates the client socket transport
 */
EcmtConnection* GWENG_CreateSocketConnection(EcmtGateway* gw, Socket s)
{
    return GWENG_CreateSockConn(gw, s, False, &gwIoSocket); 
}

/**
 * Creates the socket connection to the handset (or emulator)
 */
EcmtConnection* GWENG_CreateHandsetSocketConnection(EcmtGatewayPort* port, Socket s)
{
    EcmtConnection* c = GWENG_CreateSockConn(port->gw, s, True, 
        &gwIoEmulatorSocket);
    if (c) {
        c->port = port;
        return c;
    } else {
        return NULL;
    }
}

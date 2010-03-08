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
 * Internal data structure for "write-only" mutiplexor I/O
 */
typedef struct _EcmtMultiplexor {
    EcmtConnection connection;          /* common part */
    Vector connections;                 /* stores connections */
} EcmtMultiplexor;

/* Collect references to client's connections. Returns the count. */
STATIC int GWENG_MultiCollectRefs(EcmtMultiplexor* m)
{
    QEntry* e;
    VECTOR_Clear(&m->connections);
    MUTEX_Lock(&m->connection.gw->mutex);
    for (e=QUEUE_First(&m->connection.gw->ecmtClients); e; e=QUEUE_Next(e)) {
        EcmtClient* client = QCAST(e, EcmtClient, entry);
        if (VECTOR_Add(&m->connections, client->conn)) {
            GWENG_IoAddRef(client->conn);
        }
    }
    MUTEX_Unlock(&m->connection.gw->mutex);
    return VECTOR_Size(&m->connections);
}

STATIC int GWENG_MultiRead(EcmtConnection* c, void* buf, int size)
{
    /* should never be invoked */
    ASSMSG("GWENG_MultiRead");
    return -1;
}

STATIC Bool GWENG_MultiWrite(EcmtConnection* c, const void* data, int size)
{
    EcmtMultiplexor* m = CAST(c,EcmtMultiplexor,connection);
    int i, n = GWENG_MultiCollectRefs(m);
    for (i=0; i<n; i++) {
        EcmtConnection* out = VECTOR_Get(&m->connections, i);
        GWENG_IoWrite(out, data, size);
    }
    VECTOR_Clear(&m->connections);
    return True;
}

STATIC void GWENG_MultiCancel(EcmtConnection* c)
{
    EcmtMultiplexor* m = CAST(c,EcmtMultiplexor,connection);
    int i, n = GWENG_MultiCollectRefs(m);
    for (i=0; i<n; i++) {
        EcmtConnection* out = VECTOR_Get(&m->connections, i);
        GWENG_IoCancel(out);
    }
    VECTOR_Clear(&m->connections);
}

STATIC Bool GWENG_MultiHandset(EcmtConnection* c)
{
    QEntry* e;
    Bool handsetIo = False;
    EcmtMultiplexor* m = CAST(c,EcmtMultiplexor,connection);
    MUTEX_Lock(&m->connection.gw->mutex);
    for (e=QUEUE_First(&m->connection.gw->ecmtClients); e; e=QUEUE_Next(e)) {
        EcmtClient* client = QCAST(e, EcmtClient, entry);
        if (GWENG_IoHandset(client->conn)) {
            handsetIo = True;
            break;
        }
    }
    MUTEX_Unlock(&m->connection.gw->mutex);
    return handsetIo;
}

STATIC void GWENG_MultiClose(EcmtConnection* c)
{
    EcmtMultiplexor* m = CAST(c,EcmtMultiplexor,connection);
    VECTOR_Destroy(&m->connections);
    MEM_Free(m);
}

/* Dereferences the connection when it gets removed from the vector */
STATIC void GWENG_MultiVectorFree(VElement e)
{
    GWENG_IoReleaseRef(e);
}

/**
 * Defines multiplexor transport.
 */
STATIC const EcmtIo gwIoMultiplexor = {
    "Multiplexor",          /* name */
    GWENG_MultiRead,        /* read */
    GWENG_MultiWrite,       /* write */
    GWENG_MultiCancel,      /* cancel */
    GWENG_MultiHandset,     /* handset */
    GWENG_MultiClose        /* close */
};

/**
 * Creates the COM port transport
 */
EcmtConnection* GWENG_CreateMultiplexor(EcmtGateway* gw)
{
    EcmtMultiplexor* m = MEM_New(EcmtMultiplexor);
    if (m) {
        memset(m, 0, sizeof(*m));
        VERIFY(VECTOR_Init(&m->connections, 0, NULL, GWENG_MultiVectorFree));
        m->connection.io = &gwIoMultiplexor;
        m->connection.ref = 1;
        m->connection.gw = gw;
        return &m->connection;
    }
    return NULL;
}

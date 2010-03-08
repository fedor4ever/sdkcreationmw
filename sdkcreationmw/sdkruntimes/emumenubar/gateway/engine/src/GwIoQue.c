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
 * Internal data structure for the "write-only" queued I/O.
 */
typedef struct _EcmtQueueConnection {
    EcmtConnection connection;          /* common part */
    EcmtQueue* queue;                   /* packet queue (not owned) */
    Buffer buf;                         /* packet buffer */
    Bool header;                        /* True if we have header */
    int uid;                            /* UID from header */
    int datalen;                        /* expected data length */
} EcmtQueueConnection;

STATIC int GWENG_QueueRead(EcmtConnection* c, void* buf, int size)
{
    /* should never be invoked */
    ASSMSG("GWENG_QueueRead");
    return False;
}

STATIC Bool GWENG_QueueFlush(EcmtQueueConnection* qc)
{
    if (qc->header && BUFFER_Size(&qc->buf) >= ECMT_MSG_HEADER_SIZE) {
        char* hdr = BUFFER_Access(&qc->buf);
        if (GWENG_ParseEcmtHeader(hdr, &qc->uid, &qc->datalen)) {
            qc->header = False;
            qc->datalen -= ECMT_MSG_HEADER_SIZE;
            BUFFER_Skip(&qc->buf, ECMT_MSG_HEADER_SIZE); 
        } else {
            /* There shoudn't be any garbage here */
            ASSMSG("Garbage in GWENG_QueueFlush");
            return False;
        }
    }
    if (!qc->header && BUFFER_Size(&qc->buf) >= qc->datalen) {
        char* data = BUFFER_Access(&qc->buf);
        GWENG_QueueAdd(qc->queue, qc->uid, data, qc->datalen);
        BUFFER_Skip(&qc->buf, qc->datalen); 
        qc->header = True;
        qc->datalen = 0;
        qc->uid = 0;
        return True;
    }
    return False;
}

STATIC Bool GWENG_QueueWrite(EcmtConnection* c, const void* data, int size)
{
    EcmtQueueConnection* qc = CAST(c,EcmtQueueConnection,connection);

    /*
     * Try to avoid copying. Most of the time we can safely do that because
     * both header and data are written as single chunks. However, if they
     * are not so nicely packaged, we have plan B.
     */

    if (!BUFFER_Size(&qc->buf)) {
        if (qc->header && size>=ECMT_MSG_HEADER_SIZE) {
            if (GWENG_ParseEcmtHeader(data, &qc->uid, &qc->datalen)) {
                data = ((char*)data) + ECMT_MSG_HEADER_SIZE;
                size -= ECMT_MSG_HEADER_SIZE;
                qc->datalen -= ECMT_MSG_HEADER_SIZE;
                qc->header = False;
            } else {
                /* There shoudn't be any garbage here */
                ASSMSG("Garbage in GWENG_QueueWrite");
                return False;
            }
        }
        if (!qc->header && size >= qc->datalen) {
            GWENG_QueueAdd(qc->queue, qc->uid, data, qc->datalen);
            data = ((char*)data) + qc->datalen;
            size -= qc->datalen; 
            qc->uid = 0;
            qc->datalen = 0;
            qc->header = True;
        }
    }

    if (size > 0) {
        /* Plan B */
        if (BUFFER_Put(&qc->buf, data, size, False) == size) {
            while (GWENG_QueueFlush(qc));
        } else {
            return False;
        }
    }

    return True;
}

STATIC void GWENG_QueueCancel(EcmtConnection* c)
{
}

STATIC Bool GWENG_QueueHandset(EcmtConnection* c)
{
    /* Queue does own GWENG_IoPacketStart/GWENG_IoPacketSent calls */
    return False;
}

STATIC void GWENG_QueueClose(EcmtConnection* c)
{
    EcmtQueueConnection* qc = CAST(c,EcmtQueueConnection,connection);
    BUFFER_Destroy(&qc->buf);
    MEM_Free(qc);
}

/**
 * Defines queue transport.
 */
STATIC const EcmtIo gwIoQueue = {
    "Queue",                /* name */
    GWENG_QueueRead,        /* read */
    GWENG_QueueWrite,       /* write */
    GWENG_QueueCancel,      /* cancel */
    GWENG_QueueHandset,     /* handset */
    GWENG_QueueClose,       /* close */
};

/**
 * Creates the COM port transport
 */
EcmtConnection* GWENG_CreateQueueConnection(EcmtGateway* gw, EcmtQueue* q)
{
    EcmtQueueConnection* qc = MEM_New(EcmtQueueConnection);
    if (qc) {
        memset(qc, 0, sizeof(*qc));
        qc->queue = q;
        qc->header = True;
        qc->connection.io = &gwIoQueue;
        qc->connection.ref = 1;
        qc->connection.gw = gw;
        BUFFER_Init(&qc->buf);
        return &qc->connection;
    }
    return NULL;
}

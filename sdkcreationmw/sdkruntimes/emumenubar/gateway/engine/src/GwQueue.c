/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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

typedef struct _EcmtQueueEntry {
    QEntry entry;
    int uid;
    int len;
    void* data;
} EcmtQueueEntry;

struct _EcmtQueue {
    EcmtConnection* out;
    int maxBytes;
    int totalBytes;
    Queue queue;
    Event event;
    Mutex mutex;
    ThrID thread;
    Bool done;
};

/**
 * Must be used under synchronization
 */
#define GWENG_QueueSuspended(q) (!(q)->out)

/**
 * Writes queued messages to the associated connection
 */
STATIC void GWENG_Writer(void* ctx)
{
    Bool ok = True;
    EcmtQueue* q = ctx;
    TRACE("GW: writer starting\n");
    MUTEX_Lock(&q->mutex);
    while (ok && !q->done) {
        QEntry* e = NULL;
        ASSERT(q->totalBytes >0 || QUEUE_IsEmpty(&q->queue));
        while (!q->done && 
               (GWENG_QueueSuspended(q) || 
               (e = QUEUE_RemoveHead(&q->queue)) == NULL)) {
            EVENT_Reset(&q->event);
            MUTEX_Unlock(&q->mutex);
            EVENT_Wait(&q->event);
            MUTEX_Lock(&q->mutex);
        }
        if (e) {
            char h[ECMT_MSG_HEADER_SIZE+1];
            EcmtConnection* out = q->out;
            EcmtQueueEntry* msg = QCAST(e, EcmtQueueEntry, entry);
            q->totalBytes -= (msg->len + sizeof(EcmtQueueEntry));
            ASSERT(q->totalBytes > 0 || QUEUE_IsEmpty(&q->queue));
            ASSERT(out);
            GWENG_IoAddRef(out);
            MUTEX_Unlock(&q->mutex);

            sprintf(h,"0x%08X0x%08X",msg->len+ECMT_MSG_HEADER_SIZE,msg->uid);
            GWENG_IoPacketStart(out, msg->uid, msg->data, msg->len);
            ok = (GWENG_IoWrite(out, h, ECMT_MSG_HEADER_SIZE) &&
                  GWENG_IoWrite(out, msg->data, msg->len));
            GWENG_IoPacketSent(out);

            MEM_Free(msg);
            GWENG_IoReleaseRef(out);
            MUTEX_Lock(&q->mutex);
        }
    }
    MUTEX_Unlock(&q->mutex);
    TRACE("GW: writer exiting\n");
}

/**
 * Creates the output queue object
 */
EcmtQueue* GWENG_QueueCreate(EcmtConnection* out)
{
    EcmtQueue* q = MEM_New(EcmtQueue);
    if (q) {
        memset(q, 0, sizeof(*q));
        if (EVENT_Init(&q->event)) {
            if (MUTEX_Init(&q->mutex)) {
                QUEUE_Init(&q->queue);
                if (out) GWENG_IoAddRef(out);
                q->out = out;
                q->maxBytes = 0x100000; /* 1MB */
                if (THREAD_Create(&q->thread, GWENG_Writer, q)) {
                    return q;
                }
                if (out) GWENG_IoReleaseRef(out);
                MUTEX_Destroy(&q->mutex);
            }
            EVENT_Destroy(&q->event);
        }
        MEM_Free(q);
    }
    return NULL;
}

/**
 * Deletes the output queue object
 */
void GWENG_QueueDelete(EcmtQueue* q)
{
    if (q) {
        QEntry* e;
        ASSERT(!q->done);

        /* shutdown the writer thread */
        MUTEX_Lock(&q->mutex);
        q->done = True;
        EVENT_Set(&q->event);
        MUTEX_Unlock(&q->mutex);
        THREAD_Join(q->thread);

        /* release resources */
        while ((e = QUEUE_RemoveHead(&q->queue)) != NULL) {
            EcmtQueueEntry* msg = QCAST(e, EcmtQueueEntry, entry);
            q->totalBytes -= (msg->len + sizeof(EcmtQueueEntry));
            MEM_Free(msg);
        }
        ASSERT(q->totalBytes == 0);
        if (q->out) GWENG_IoReleaseRef(q->out);
        MUTEX_Destroy(&q->mutex);
        EVENT_Destroy(&q->event);
        MEM_Free(q);
    }
}

/**
 * Adds a ECMT message to the queue. Copies the data.
 */
void GWENG_QueueSubmit(EcmtQueue* q, EcmtQueueEntry* msg)
{
    int nbytes = sizeof(EcmtQueueEntry) + msg->len;
    MUTEX_Lock(&q->mutex);
    if (GWENG_QueueSuspended(q)) {
        while (!QUEUE_IsEmpty(&q->queue) && 
            (q->totalBytes+nbytes) > q->maxBytes) { 
            QEntry* e = QUEUE_RemoveHead(&q->queue);
            EcmtQueueEntry* msg = QCAST(e, EcmtQueueEntry, entry);
            q->totalBytes -= (msg->len + sizeof(EcmtQueueEntry));
            TRACE("ECMTGW: dropping message...\n");
            MEM_Free(msg);
        }
    }
    q->totalBytes += nbytes;
    QUEUE_InsertTail(&q->queue, &msg->entry);
    if (!GWENG_QueueSuspended(q)) EVENT_Set(&q->event);
    MUTEX_Unlock(&q->mutex);
}

/**
 * Adds a ECMT message to the queue. Copies the data.
 */
Bool GWENG_QueueAdd(EcmtQueue* q, int uid, const void* data, int size)
{
    int nbytes = sizeof(EcmtQueueEntry) + size;
    EcmtQueueEntry* msg = MEM_Alloc(nbytes);
    if (msg) {
        memset(msg, 0, sizeof(*msg));
        msg->uid = uid;
        msg->len = size;
        msg->data = (msg+1);
        if (size > 0) memcpy(msg->data, data, size);
        GWENG_QueueSubmit(q, msg);
        return True;
    } else {
        return False;
    }
}

/**
 * Adds a ECMT message to the queue. Copies the data.
 */
Bool GWENG_QueueAdd2(EcmtQueue* q, int uid,
                     const void* data1, int size1,
                     const void* data2, int size2)
{
    int nbytes = sizeof(EcmtQueueEntry) + size1 + size2;
    EcmtQueueEntry* msg = MEM_Alloc(nbytes);
    if (msg) {
        char* data = (char*)(msg+1);
        memset(msg, 0, sizeof(*msg));
        msg->uid = uid;
        msg->len = size1+size2;
        msg->data = data;
        if (size1 > 0) memcpy(data, data1, size1);
        if (size2 > 0) memcpy(data+size1, data2, size2);
        GWENG_QueueSubmit(q, msg);
        return True;
    } else {
        return False;
    }
}

void GWENG_QueueSetConnection(EcmtQueue* q, EcmtConnection* out)
{
    MUTEX_Lock(&q->mutex);
    if (q->out != out) {
        if (out) GWENG_IoAddRef(out);
        if (q->out) GWENG_IoReleaseRef(q->out);
        q->out = out;
        if (!GWENG_QueueSuspended(q) && !QUEUE_IsEmpty(&q->queue)) {
            /* wake up the GWENG_Writer thread */
            EVENT_Set(&q->event);
        }
    }
    MUTEX_Unlock(&q->mutex);
}

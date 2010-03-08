/*
 * $Id: s_wkq.c,v 1.17 2005/02/19 03:36:13 slava Exp $
 *
 * Copyright (C) 2001-2005 by Slava Monich
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 *
 *   1.Redistributions of source code must retain the above copyright 
 *     notice, this list of conditions and the following disclaimer. 
 *   2.Redistributions in binary form must reproduce the above copyright 
 *     notice, this list of conditions and the following disclaimer 
 *     in the documentation and/or other materials provided with the 
 *     distribution. 
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) ARISING 
 * IN ANY WAY OUT OF THE USE OR INABILITY TO USE THIS SOFTWARE, EVEN 
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 *
 * The views and conclusions contained in the software and documentation 
 * are those of the authors and should not be interpreted as representing 
 * any official policies, either expressed or implied.
 */

#include "s_wkq.h"
#include "s_queue.h"
#include "s_event.h"
#include "s_mutex.h"
#include "s_thread.h"
#include "s_libp.h"
#include "s_mem.h"

typedef struct _Waiter Waiter;
struct _Waiter {
    Event event;                /* event the waiter is waiting on */
    Waiter * next;              /* next waiter */
};

struct _WorkItem {
    QEntry itemsQ;              /* queue entry, WorkQueue->items list */
    QEntry submitQ;             /* queue entry, WorkQueue->submit queue */
    WorkProc proc;              /* function to call */
    void * param;               /* parameter to pass in */
    Waiter * waiters;           /* the waiters */
    int flags;                  /* flags, see below */

#define WKI_CALL      0x0001    /* callback is in progress */
#define WKI_DONE      0x0002    /* callback was invoked */
#define WKI_CANCELED  0x0004    /* work item has been canceled */
#define WKI_DETACHED  0x0008    /* work item is owned by the work queue */
};

struct _WorkQueue {
    Queue items;                /* all work items associated with this queue */
    Queue submit;               /* submitted work items */
    Mutex mutex;                /* synchronization mutex */
    Event event;                /* notification event */
    Event stopEvent;            /* final notification event */
    ThrID thread;               /* id of the worker thread */
    IdleProc idleProc;          /* idle callback */
    void * idleParam;           /* parameter of the idle callback */
    long idleTimeout;           /* idle timeout */
    int flags;                  /* flags, see below */

#define WKQ_ACTIVE    0x0001    /* worker thread is active */
#define WKQ_DEAD      0x0002    /* work item thread has died */
};

#define DEFAULT_MAX_WAIT_POOL 16
#define DEFAULT_MAX_ITEM_POOL 16

typedef struct _WorkQueueModule {
    int initcount;              /* how many times we were initialized */
    Mutex mutex;                /* synchronization object */
    Waiter * waitpool;          /* pool of waiters */
    int maxwait;                /* max number of waiters in the pool */
    int nwait;                  /* current number of waiters in the pool */
    Queue itempool;             /* pool of preallocated work items */
    int maxitems;               /* maximum size of the work item pool */
} WorkQueueModule;

/*==========================================================================*
 *              G L O B A L    C O N T E X T
 *==========================================================================*/

STATIC WorkQueueModule WKQ = {0};

void WKQ_InitModule()
{
    if ((WKQ.initcount++) == 0) {
        THREAD_InitModule();
        QUEUE_Init(&WKQ.itempool);
        if (MUTEX_Init(&WKQ.mutex)) {
            WKQ.maxwait = DEFAULT_MAX_WAIT_POOL;
            WKQ.maxitems = DEFAULT_MAX_ITEM_POOL;
            return;
        }
        SLIB_Abort(TEXT("WKQ"));
    }
}

void WKQ_Shutdown()
{
    if ((--WKQ.initcount) == 0) {
        while (WKQ.waitpool) {
            Waiter * next = WKQ.waitpool->next;
            EVENT_Destroy(&WKQ.waitpool->event);
            MEM_Free(WKQ.waitpool);
            WKQ.waitpool = next;
            WKQ.nwait--;
        }
        ASSERT(WKQ.nwait == 0);
        while (!QUEUE_IsEmpty(&WKQ.itempool)) {
            QEntry * e = QUEUE_RemoveHead(&WKQ.itempool);
            WorkItem * w = QCAST(e,WorkItem,itemsQ);
            MEM_Free(w);
        }
        MUTEX_Destroy(&WKQ.mutex);
        THREAD_Shutdown();
    }
}

/**
 * Returns a wait context from the pool, or allocates a new one
 */
STATIC Waiter * WKQ_GetWaiter(WorkQueueModule * module)
{
    Waiter * waiter = NULL;
    ASSERT(module->initcount > 0);
    
    if (module->waitpool) {
        MUTEX_Lock(&module->mutex);
        if (module->waitpool) {
            waiter = module->waitpool;
            module->waitpool = waiter->next;
            waiter->next = NULL;
            module->nwait--;
            ASSERT(module->nwait >= 0);
            ASSERT(module->nwait || !module->waitpool);
        }
        MUTEX_Unlock(&module->mutex);
    }
    
    if (!waiter) {
        waiter = MEM_New(Waiter);
        if (waiter) {
            if (!EVENT_Init(&waiter->event)) {
                MEM_Free(waiter);
                waiter = NULL;
            }
        }
    }

    if (waiter) {
        EVENT_Reset(&waiter->event);
    }

    return waiter;
}

/**
 * Internal routine. Puts waiter to the pool, assumes that caller provides
 * synchronization and has checked everything (that waiter isn't NULL, 
 * that pool size did not exceed the maximum, etc.)
 */
STATIC void WKQ_WaiterToPool(WorkQueueModule * module, Waiter * waiter)
{
    waiter->next = module->waitpool;
    module->waitpool = waiter;
    module->nwait++;
}

/**
 * Puts event back to the pool or deallocates it.
 */
STATIC void WKQ_ReleaseWaiter(WorkQueueModule * module, Waiter * waiter)
{
    ASSERT(module->initcount > 0);
    if (module->nwait < module->maxwait) {
        MUTEX_Lock(&module->mutex);
        if (module->nwait < module->maxwait) {
            WKQ_WaiterToPool(module, waiter);
            waiter = NULL;
        }
        MUTEX_Unlock(&module->mutex);
    }
    if (waiter) {
        EVENT_Destroy(&waiter->event);
        MEM_Free(waiter);
    }
}

/**
 * Returns work item from the pool, allocates a new one if needed.
 */
STATIC WorkItem *
WKQ_GetWorkItem(WorkQueueModule * mod, WorkQueue * q, WorkProc cb, void * par)
{
    WorkItem * w = NULL;

    ASSERT(mod->initcount > 0);
    if (!QUEUE_IsEmpty(&mod->itempool)) {
        MUTEX_Lock(&mod->mutex);
        if (!QUEUE_IsEmpty(&mod->itempool)) {
            w = QCAST(QUEUE_RemoveHead(&mod->itempool),WorkItem,itemsQ);
            w->flags = 0;
        }
        MUTEX_Unlock(&mod->mutex);
    }

    if (!w) {
        w = MEM_New(WorkItem);
        if (w) {
            memset(w, 0, sizeof(*w));
        }
    }

    if (w) {
        if (MUTEX_Lock(&q->mutex)) {
            w->proc = cb;
            w->param = par;
            QUEUE_InsertTail(&q->items,&w->itemsQ);
            MUTEX_Unlock(&q->mutex);
            return w;
        }
        MEM_Free(w);
    }

    return NULL;
}

/**
 * Puts work item to the global pool or deallocates it. Also deallocates 
 * the events associated with the work item. NOTE: this code is designed 
 * to be efficient, not compact
 */
STATIC void WKQ_ReleaseWorkItem(WorkQueueModule * module, WorkItem * w)
{
    Bool locked = False;

    ASSERT(module->initcount > 0);
    ASSERT(!w->submitQ.queue);
    ASSERT(!w->itemsQ.queue);

    /* deallocate waiters */
    while (w->waiters) {
        Waiter * waiter = w->waiters;
        Waiter * next = waiter->next;

        if (module->nwait < module->maxwait) {
            if (locked) {
                WKQ_WaiterToPool(module, waiter);
                waiter = NULL;
            } else {
                locked = MUTEX_Lock(&module->mutex);
                if (module->nwait < module->maxwait) {                    
                    WKQ_WaiterToPool(module, waiter);
                    waiter = NULL;
                }
            }
        }

        if (waiter) {
            EVENT_Destroy(&waiter->event);
            MEM_Free(waiter);
        }

        w->waiters = next;
    }

    if (QUEUE_Size(&module->itempool) < module->maxitems) {
        if (locked) {
            w->flags = WKI_DETACHED;
            QUEUE_InsertTail(&module->itempool, &w->itemsQ);
        } else {
            locked = MUTEX_Lock(&module->mutex);
            if (QUEUE_Size(&module->itempool) < module->maxitems) {
                w->flags = WKI_DETACHED;
                QUEUE_InsertTail(&module->itempool, &w->itemsQ);
            } else {
                MEM_Free(w);
            }
        }
    } else {
        MEM_Free(w);
    }

    if (locked) {
        MUTEX_Unlock(&module->mutex);
    }
}

/*==========================================================================*
 *              W O R K    I T E M
 *==========================================================================*/

/**
 * Return the work queue for the specified work item
 */
STATIC WorkQueue * WKI_GetQueue(WorkItem * w)
{
    if (w) {
        ASSERT(w->itemsQ.queue);
        if (w->itemsQ.queue) {
            return CAST_QUEUE(w->itemsQ.queue,WorkQueue,items);
        }
    }
    return NULL;
}

/**
 * Creates a work item. The returned work item is not associated with any
 * work queue.
 */
WorkItem * WKI_Create(WorkQueue * q, WorkProc cb, void * par)
{
    ASSERT(WKQ.initcount > 0);
    if (WKQ.initcount == 0) WKQ_InitModule();
    return WKQ_GetWorkItem(&WKQ, q, cb, par);
}

/**
 * Submits a work item to the specified work queue. Re-submitting the same
 * work before it has been executed just moves it to the tail of the work
 * queue. It does NOT schedule it to run twice.
 */
Bool WKI_Submit(WorkItem * w)
{
    WorkQueue * q = WKI_GetQueue(w);
    ASSERT(q);
    ASSERT(!(w->flags & WKI_DETACHED));
    if (q) {
        if (MUTEX_Lock(&q->mutex)) {
            if (q->flags & WKQ_ACTIVE) {
                w->flags &= ~(WKI_DONE | WKI_CANCELED);
                QUEUE_RemoveEntry(&w->submitQ);
                QUEUE_InsertTail(&q->submit, &w->submitQ);
                EVENT_Set(&q->event);
                MUTEX_Unlock(&q->mutex);
                return True;
            }
            MUTEX_Unlock(&q->mutex);
            /* fall through and return False */
        }
    }
    return False;
}

/**
 * These simple functions check various flags
 */
Bool WKI_IsCanceled(WorkItem * w)
{
    ASSERT(!(w->flags & WKI_DETACHED));
    return BoolValue(w->flags & WKI_CANCELED);
}

Bool WKI_IsDone(WorkItem * w)
{
    ASSERT(!(w->flags & WKI_DETACHED));
    return BoolValue(w->flags & WKI_DONE);
}

/**
 * Switches the work item into the "detached" state. A detached work
 * item is not waitable, the caller does not own the work item anymore.
 */
void WKI_Detach(WorkItem * w)
{
    WorkQueue * q = WKI_GetQueue(w);
    ASSERT(!(w->flags & WKI_DETACHED));
    MUTEX_Lock(&q->mutex);
    w->flags |= WKI_DETACHED;
    if (!w->submitQ.queue && !(w->flags & WKI_CALL)) {
        QUEUE_RemoveEntry(&w->itemsQ);
        WKQ_ReleaseWorkItem(&WKQ, w);
    }
    MUTEX_Unlock(&q->mutex);
}

/**
 * Attaches a waiter to the work item if the work item is waitable.
 * The caller is responsible for deallocating this waiter
 */
STATIC Waiter * WKI_AttachWaiter(WorkItem * w)
{
    Waiter * waiter = NULL;
    WorkQueue * q = WKI_GetQueue(w);
    ASSERT(!(w->flags & WKI_DETACHED));

    /* quick check without synchronization */
    if (w->submitQ.queue || (w->flags & WKI_CALL)) {

        /* We avoid calling WKQ_GetWaiter under mutex because in NT kernel
         * mode environment this results in KeInitializeEvent being called
         * at IRQL DISPATCH_LEVEL. According to the Windows DDK documentation,
         * the callers of KeInitializeEvent must be running at IRQL 
         * PASSIVE_LEVEL. I personally think it's a mistake in the
         * documentation, because KeInitializeEvent does not do anything
         * that would require the current thread to wait or to access
         * pageable memory, unless the event is allocated from paged pool
         * or the KeInitializeEvent code itself resides in a pageable code
         * segment (which does not seem to be the case). Anyway, I decided
         * to play it safe and follow the documentation.
         */
        waiter = WKQ_GetWaiter(&WKQ);
        if (waiter) {
            Bool waitable = False;
            MUTEX_Lock(&q->mutex);
            /* the same check, this time under synchronization */
            if (w->submitQ.queue || (w->flags & WKI_CALL)) {
                waiter->next = w->waiters;
                w->waiters = waiter;
                waitable = True;
            }
            MUTEX_Unlock(&q->mutex);
            if (!waitable) {

                /* Something must have changed while we were allocating the 
                 * waiter. Return it to the pool. In real life, this almost 
                 * never happens.
                 */
                WKQ_ReleaseWaiter(&WKQ, waiter);
                waiter = NULL;
            }
        }
    }
    return waiter;
}

/**
 * Waits for the work item to complete
 */
void WKI_Wait(WorkItem * w)
{
    Waiter * waiter = WKI_AttachWaiter(w);
    if (waiter) {
        EVENT_Wait(&waiter->event);
        WKQ_ReleaseWaiter(&WKQ, waiter);
    }
}

/**
 * Waits for the work item to complete
 */
WaitState WKI_TimeWait(WorkItem * w, long ms)
{
    WaitState result = WAIT_STATE_OK;
    Waiter * waiter = WKI_AttachWaiter(w);
    if (waiter) {
        result = EVENT_TimeWait(&waiter->event,ms);
        WKQ_ReleaseWaiter(&WKQ, waiter);
    }
    return result;
}

/**
 * Signals the events associated with the work item. The caller must 
 * hold the queue mutex. Note that the event associated with the work
 * item is deallocated by the thread that was waiting for this work 
 * item to complete.
 */
STATIC void WKI_Signal(WorkItem * w)
{
    if (w->waiters) {
        Waiter * waiter;
        for (waiter = w->waiters; waiter; waiter = waiter->next) {
            EVENT_Set(&waiter->event);
        }
        /* the waiters will release their context */
        w->waiters = NULL;
    }
}

/**
 * Cancels the work item. Returns True if work item has been removed from 
 * the queue before being called, False in any other case. Unblocks the
 * waiters.
 */
Bool WKI_Cancel(WorkItem * w)
{
    Bool canceled = False;
    WorkQueue * q = WKI_GetQueue(w);
    ASSERT(!(w->flags & WKI_DETACHED));
    if (MUTEX_Lock(&q->mutex)) {
        if (QUEUE_RemoveEntry(&w->submitQ)) {
            canceled = True;
            w->flags |= WKI_CANCELED;
            WKI_Signal(w);
        }
        MUTEX_Unlock(&q->mutex);
    }
    return canceled;
}

/*==========================================================================*
 *              W O R K    Q U E U E
 *==========================================================================*/

/**
 * The worker thread
 */
STATIC void WKQ_Thread(void * par) 
{
    WorkQueue * q = (WorkQueue *)par;
    TRACE("WKQ: starting\n");
    
    /* start the loop */
    MUTEX_Lock(&q->mutex);
    while ((q->flags & WKQ_ACTIVE) || !QUEUE_IsEmpty(&q->submit)) {
        QEntry * e;
        while ((e = QUEUE_RemoveHead(&q->submit)) != NULL) {
            WorkItem * w = QCAST(e,WorkItem,submitQ);
            ASSERT(!(w->flags & (WKI_DONE|WKI_CANCELED)));

            /*
             * NULL callback may be used by dummy work items whose purpose
             * is to wait until all pending work items have been processed
             */
            if (w->proc) {

                /* update flags */
                w->flags |= WKI_CALL;

                /* invoke the handler */
                MUTEX_Unlock(&q->mutex);
                w->proc(w, w->param);
                MUTEX_Lock(&q->mutex);

                if (w->flags & WKI_DETACHED) {
                
                    /* put the work item to the pool or deallocate it */
                    ASSERT(!w->waiters);
                    QUEUE_RemoveEntry(&w->itemsQ);
                    WKQ_ReleaseWorkItem(&WKQ, w);

                } else {

                    /* 
                     * update flags. Note that we released the mutex when 
                     * were invoking the callback. Therefore, this work 
                     * item could be re-submitted to the queue. Or it could
                     * be re-submitted and then canceled. In such cases we
                     * don't need to set the WKI_DONE flag.
                     */
                    w->flags &= ~WKI_CALL;
                    if (!(w->flags & WKI_CANCELED) && !w->submitQ.queue) {
                        w->flags |= WKI_DONE;
                    }

                    /* signal the events associated with the work item */
                    WKI_Signal(w);
                }
            } else {
                
                /* it's a dummy work item. Just release the waiters */
                WKI_Signal(w);
            }
        }

        /* wait for a signal */
        if (q->flags & WKQ_ACTIVE) {
            EVENT_Reset(&q->event);
            if (q->idleProc) {

                /* we have an idle timeout */
                IdleProc idle = q->idleProc;
                void * param = q->idleParam;
                long timeout = q->idleTimeout;
                MUTEX_Unlock(&q->mutex);
                switch (EVENT_TimeWait(&q->event, timeout)) {

                case WAIT_STATE_OK:
                    /* don't invoke idle callback */
                    MUTEX_Lock(&q->mutex);
                    break;

                case WAIT_STATE_TIMEOUT:
                    /* invoke idle callback */
                    idle(q, param);
                    MUTEX_Lock(&q->mutex);
                    break;

                default:
                case WAIT_STATE_ERROR:
                    /* terminate the thread on error */
                    MUTEX_Lock(&q->mutex);
                    q->flags |= WKQ_ACTIVE;
                    break;
                }

            } else {

                /* wait forever */
                MUTEX_Unlock(&q->mutex);
                EVENT_Wait(&q->event);
                MUTEX_Lock(&q->mutex);
            }
        }
    }

    /* cleanup */
    MUTEX_Unlock(&q->mutex);
    TRACE("WKQ: done\n");
}

/**
 * Creates and starts the worker thread
 */
WorkQueue * WKQ_Create()
{
    WorkQueue * q = MEM_New(WorkQueue);
    if (q) {
        
        ASSERT(WKQ.initcount > 0);
        if (WKQ.initcount == 0) WKQ_InitModule();

        memset(q, 0, sizeof(*q));
        if (MUTEX_Init(&q->mutex)) {
            if (EVENT_Init(&q->event)) {
                if (EVENT_Init(&q->stopEvent)) {
                    if (EVENT_Reset(&q->stopEvent)) {
                        q->flags = WKQ_ACTIVE;
                        QUEUE_Init(&q->items);
                        QUEUE_Init(&q->submit);
                        if (THREAD_Create(&q->thread,WKQ_Thread,q)) {
                            return q;
                        }
                    }
                    EVENT_Destroy(&q->stopEvent);
                }
                EVENT_Destroy(&q->event);
            }
            MUTEX_Destroy(&q->mutex);
        }
        MEM_Free(q);
    }
    return NULL;
}

/**
 * Stop the worker thread
 */
void WKQ_Stop(WorkQueue * q, Bool canWait)
{
    ThrID thread = (ThrID)0;
    if (MUTEX_Lock(&q->mutex)) {
        if (q->flags & WKQ_ACTIVE) {
            q->flags &= ~WKQ_ACTIVE;
            EVENT_Set(&q->event);
        }
        if (canWait && q->thread) {
            ASSERT(!(q->flags & WKQ_DEAD));
            thread = q->thread;
            q->thread = (ThrID)0;
        }
        MUTEX_Unlock(&q->mutex);
    }

    if (canWait) {
        if (thread) {
            THREAD_Join(thread);
            ASSERT(!(q->flags & WKQ_DEAD));
            MUTEX_Lock(&q->mutex);
            q->flags |= WKQ_DEAD;
            EVENT_Set(&q->stopEvent);
            MUTEX_Unlock(&q->mutex);
        } else {
            WKQ_Wait(q);
        }
    }
}

/**
 * Deallocates the work queue
 */
STATIC void WKQ_Free(WorkQueue * q)
{
    ASSERT(QUEUE_IsEmpty(&q->submit));
    ASSERT(QUEUE_IsEmpty(&q->items));
    ASSERT(q->flags & WKQ_DEAD);

    EVENT_Destroy(&q->stopEvent);
    EVENT_Destroy(&q->event);
    MUTEX_Destroy(&q->mutex);

    MEM_Free(q);
}

/**
 * Waits until all work items associated with this work queue are done ane 
 * deallocates the work queue.
 */
void WKQ_Delete(WorkQueue * q)
{
    if (q) {
        WKQ_Stop(q, True);
        ASSERT(QUEUE_IsEmpty(&q->items));
        WKQ_Free(q);
    }
}

/**
 * Submits internal workitem that will invoke the specified callback function
 * with the specified parameter
 */
Bool WKQ_InvokeLater(WorkQueue * q, WorkProc cb, void * par)
{
    Bool ok = False;
    WorkItem * w = WKI_Create(q, cb, par);
    if (w) {
        ok = WKI_Submit(w);
        WKI_Detach(w);
    }
    return ok;
}

/**
 * Submits internal workitem that will invoke the specified callback function
 * with the specified parameter, and waits for completion
 */
Bool WKQ_InvokeAndWait(WorkQueue * q, WorkProc cb, void * par)
{
    Bool ok = False;
    WorkItem * w = WKI_Create(q, cb, par);
    if (w) {
        ok = WKI_Submit(w);
        if (ok) {
            WKI_Wait(w);
        }
        WKI_Detach(w);
    }
    return ok;
}

/**
 * Sets the idle timeout function for the work queue. If the cb parameter 
 * is NULL, idle timeouts are disabled and the other parameters (ms, param)
 * are ignored. The timeout must be positive. 
 */
void WKQ_SetIdle(WorkQueue * q, long ms, IdleProc cb, void * param)
{
    if (MUTEX_Lock(&q->mutex)) {
        if (cb) {
            ASSERT(ms > 0);
            q->idleTimeout = MAX(ms,1);
            q->idleProc = cb;
            q->idleParam = param;
        } else {
            q->idleTimeout = 0;
            q->idleProc = NULL;
            q->idleParam = NULL;
        }
        /* notify the worker thread */
        EVENT_Set(&q->event); 
        MUTEX_Unlock(&q->mutex);
    }
}

/**
 * Cancels all pending work items in the work queue.
 */
void WKQ_Cancel(WorkQueue * q)
{
    if (MUTEX_Lock(&q->mutex)) {
        QEntry * e;
        while ((e = QUEUE_RemoveHead(&q->submit)) != NULL) {
            WorkItem * w = QCAST(e,WorkItem,submitQ);
            w->flags |= WKI_CANCELED;
            if (w->flags & WKI_DETACHED) {
                ASSERT(!w->waiters);
                QUEUE_RemoveEntry(&w->itemsQ);
                WKQ_ReleaseWorkItem(&WKQ, w);
            } else {
                WKI_Signal(w);
            }
        }
        MUTEX_Unlock(&q->mutex);
    }
}

/**
 * Waits for this wait queue to stop
 */
void WKQ_Wait(WorkQueue * q)
{
    EVENT_Wait(&q->stopEvent);
    ASSERT(q->flags & WKQ_DEAD);
}

/**
 * Waits for this wait queue to stop
 */
WaitState WKQ_TimeWait(WorkQueue * q, long ms)
{
    return EVENT_TimeWait(&q->stopEvent, ms);
}

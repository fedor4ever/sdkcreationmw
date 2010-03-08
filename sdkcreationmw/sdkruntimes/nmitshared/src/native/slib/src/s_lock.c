/*
 * $Id: s_lock.c,v 1.30 2005/02/20 20:31:29 slava Exp $
 *
 * Copyright (C) 2000-2005 by Slava Monich
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

#include "s_lock.h"
#include "s_util.h"
#include "s_mem.h"

/*
 * Flags
 */

#define LOCK_FLAG_EXCLISIVE_LOCK         0x0001  /* writer owns the lock */
#define LOCK_FLAG_EXCLISIVE_EVENT_IN_USE 0x0002  /* exclusiveEvent in use */

/*
 * Waiter context. In the waiter is shared, then its event field points
 * to Lock->shareEvent. The event field of the first exclusive waiter
 * points to Lock->exclusiveEvent. For subsequent exclusive waiters,
 * it points to a dynamically allocated event. The index is zero for
 * the first waiter. If waiter was not the first, the index if the 
 * highest active index + 1. The highest active index is determined
 * by checking the last entries in both exclusive and shared lists.
 */
typedef struct _LockWaiter {
    QEntry entry;               /* entry in the linked list */
    Event * event;              /* event this waiter is waiting on */
    int index;                  /* to determine the order of waiters */
} LockWaiter;

/*
 * Here is the problem. The resource is being used in readonly mode.
 * Then comes an exclusive waiter. It gets put into the list of waiters
 * and starts waiting. Then comes another readonly waiter. What do we do?
 * We can do one of the two things:
 * 
 * o immediately grant access to the reader because writer has to wait anyway
 * o put reader into the queue after the writer because writer got here first
 *
 * The first case should normally be more efficient unless reader are coming
 * in too fast, in which case the writer may never get a chance to run. The
 * second variant guarantees that writer will run sooner or later but forces
 * reader threads wait longer than necessary. What we do is a combination
 * of these two solutions. We allow the reader to get ahead of the writer
 * as long as this happens no more than LOCK_MAX_BYPASS_COUNT times. After 
 * that, everyone, including the readers has to wait for its turn. The
 * counter is reset every time a writer gets the lock.
 */
#define LOCK_MAX_BYPASS_COUNT 10

/*
 * Number of entries allocated statically as a part of Lock structure
 */
#define STATIC_ENTRIES ((int)(FIELDSIZE(Lock,staticEntries)/sizeof(LockEntry)))

/* 
 * The maximum number of waiters to cache.
 */
#define MAX_WAITER_CACHE_SIZE (STATIC_ENTRIES + 1)

/*
 * NOTE: Even if we allocate additional entries we keep using 
 * the static entries.
 *
 * EXCERSISE. Below is the original version of GET_ENTRY macro:
 *
 *   #define GET_ENTRY(_lock, _i) \
 *        (((_i) < STATIC_ENTRIES) ? \
 *        ((_lock)->staticEntries + (_i)) : \
 *        ((_lock)->moreEntries + ((_i) - STATIC_ENTRIES)))
 *
 * For some reason (I think it's a bug in Microsoft's compiler)
 * this macro results in a code that causes access violation
 * when compiled by Visual C++ 6.0 with optimization on (i.e. 
 * release build). Moving this code into a function solved 
 * the problem. Am I missing something? Do you see any problem
 * with this macro?
 */
       
#define GET_ENTRY(_lock, _i) LOCK_GetEntryAt(_lock, _i)

/*
 * LOCK_InitModule() increments this counter
 */
STATIC int LOCK_initcount = 0;

/**
 * Initialize the module
 */
void LOCK_InitModule() 
{
    if ((LOCK_initcount++) == 0) {
        THREAD_InitModule(); 
    }
}

/**
 * Deinitialize the module
 */
void LOCK_Shutdown() 
{
    if ((--LOCK_initcount) == 0) {
        THREAD_Shutdown(); 
    }
}

/**
 * Allocates a new lock and initializes it
 */
Lock * LOCK_Create() 
{
    Lock * lock = MEM_New(Lock);
    if (lock) {
        if (LOCK_Init(lock)) {
            return lock;
        }
        MEM_Free(lock);
    }
    return NULL;
}

/**
 * Deletes and deallocates the mutex previously allocated with 
 * LOCK_Create()
 */
void LOCK_Delete(Lock * lock) 
{
    if (lock) {
        LOCK_Destroy(lock);
        MEM_Free(lock);
    }
}

/**
 * Initialize the lock.
 */
Bool LOCK_Init(Lock * lock) 
{
    ASSERT(LOCK_initcount > 0);
    if (LOCK_initcount == 0) LOCK_InitModule();
    memset(lock, 0, sizeof(*lock));
    if (MUTEX_Init(&lock->mutex)) {
        if (EVENT_Init(&lock->shareEvent)) {
            if (EVENT_Init(&lock->exclusiveEvent)) {
                QUEUE_Init(&lock->shareWaiters);
                QUEUE_Init(&lock->exclusiveWaiters);
                QUEUE_Init(&lock->waiterCache);
                lock->numEntries = COUNT(lock->staticEntries);
                return True;
            }
            EVENT_Destroy(&lock->shareEvent);
        }
        MUTEX_Destroy(&lock->mutex);
    }
    return False;
}

/**
 * Deallocates the system resources used by the lock.
 */
void LOCK_Destroy(Lock * lock) 
{
    QEntry * e;
    ASSERT(!lock->locks);
    ASSERT(!lock->entriesInUse);
    ASSERT(!lock->entriesActive);
    ASSERT(!lock->shareWaiters.size);
    ASSERT(!lock->exclusiveWaiters.size);
    MUTEX_Destroy(&lock->mutex);
    EVENT_Destroy(&lock->shareEvent);
    EVENT_Destroy(&lock->exclusiveEvent);

    /* free dynamically allocated entries */
    if (lock->moreEntries) {
        MEM_Free(lock->moreEntries);
        lock->moreEntries = NULL;
    }

    /* free waiter cache */
    while ((e = QUEUE_RemoveHead(&lock->waiterCache)) != NULL) {
        LockWaiter * w = QCAST(e,LockWaiter,entry);
        MEM_Free(w);
    }

    /* free dynamically allocated events */
    while (lock->eventsInCache > 0) {
        lock->eventsInCache--;
        ASSERT(lock->eventCache[lock->eventsInCache]);
        EVENT_Delete(lock->eventCache[lock->eventsInCache]);
        lock->eventCache[lock->eventsInCache] = NULL;
    }

    lock->numEntries = 0;
    lock->locks = -1;  /* to cause ASSERT if destroyed twice */
}

/**
 * Gets event from cache or allocates a new one. Must be called 
 * under synchronization.
 */
STATIC Event * LOCK_GetExclusiveEvent(Lock * lock)
{
    if (lock->eventsInCache > 0) {
        Event * e = e = lock->eventCache[lock->eventsInCache-1];
        lock->eventCache[lock->eventsInCache-1] = NULL;
        lock->eventsInCache--;
        ASSERT(e);
        return e;
    } else {
        return EVENT_Create();
    }
}

/**
 * Returns event to the cache or deallocates it if the cache is full.
 * Must be called under synchronization
 */
STATIC void LOCK_ReleaseExclusiveEvent(Lock * lock, Event * e)
{
    ASSERT(e);
    ASSERT(e != &lock->exclusiveEvent);
    if (lock->eventsInCache < COUNT(lock->eventCache)) {
        lock->eventCache[lock->eventsInCache++] = e;
    } else {
        EVENT_Delete(e);
    }
}

/**
 * Gets the next waiter index. The index is zero for the first waiter.
 * If waiter was not the first, the index if the highest active index + 1.
 * The highest active index is determined by checking the last entries
 * in both exclusive and shared lists. Must be called under synchronization
 */
STATIC int LOCK_GetNextWaiterIndex(Lock * lock)
{
    int i = 0;
    LockWaiter * lastWaiter;
    QEntry * e = QUEUE_Last(&lock->shareWaiters);
    if (e) {
        lastWaiter = QCAST(e,LockWaiter,entry);
        i = lastWaiter->index + 1;
    }
    e = QUEUE_Last(&lock->exclusiveWaiters);
    if (e) {
        lastWaiter = QCAST(e,LockWaiter,entry);
        i = MAX(i,lastWaiter->index + 1);
    }
    return i;
}

/**
 * Puts LockWaiter structure into the cache or deallocates it. This is enough
 * for a shared waiter but not for exclusive waiter (see function below) 
 * Must be used under synchronization.
 */
STATIC void LOCK_ReleaseWaiter(Lock * lock, LockWaiter * w)
{
    QUEUE_RemoveEntry(&w->entry);
    if (QUEUE_Size(&lock->waiterCache) < MAX_WAITER_CACHE_SIZE) {
        w->index = 0;
        w->event = NULL;
        QUEUE_InsertTail(&lock->waiterCache, &w->entry);
    } else {
        MEM_Free(w);
    }
}

/**
 * Release a LockWaiter structure for an exclusive waiter.
 * Must be used under synchronization
 */
STATIC void LOCK_ReleaseExclusiveWaiter(Lock * lock, LockWaiter * w)
{
    /* release the event */
    if (w->event) {
        if (w->event == &lock->exclusiveEvent) {
            ASSERT(lock->flags & LOCK_FLAG_EXCLISIVE_EVENT_IN_USE);
            lock->flags &= ~LOCK_FLAG_EXCLISIVE_EVENT_IN_USE;
        } else {
            LOCK_ReleaseExclusiveEvent(lock, w->event);
        }
    }

    /* put waiter into the cache or deallocate it */
    LOCK_ReleaseWaiter(lock, w);
}

/**
 * Gets a LockWaiter structure from the cache or allocates a new one.
 * It's the responsibility of the caller to initialize the event 
 * pointer and insert the waiter into the right queue. It's done
 * by LOCK_GetExclusiveWaiter and LOCK_GetSharedWaiter functions.
 * Must be called under synchronization
 */
STATIC LockWaiter * LOCK_GetWaiter(Lock * lock)
{
    LockWaiter * w = NULL;
    QEntry * e = QUEUE_RemoveTail(&lock->waiterCache);
    if (e) {
        w = QCAST(e,LockWaiter,entry);
        w->index = LOCK_GetNextWaiterIndex(lock);
        return w;
    } else {
        w = MEM_New(LockWaiter);
        if (w) {
            memset(w,0,sizeof(*w));
            w->index = LOCK_GetNextWaiterIndex(lock);
            return w;
        }
    }
    return NULL;
}

/**
 * Gets a LockWaiter structure for an exclusive waiter from the cache or 
 * allocates a new one. Adds the waiter to the tail of the queue. 
 * Must be called under synchronization
 */
STATIC LockWaiter * LOCK_GetExclusiveWaiter(Lock * lock)
{
    LockWaiter * w = LOCK_GetWaiter(lock);
    if (w) {
        if (lock->flags & LOCK_FLAG_EXCLISIVE_EVENT_IN_USE) {
            ASSERT(w->index);
            w->event = LOCK_GetExclusiveEvent(lock);
            if (!w->event) {
                LOCK_ReleaseWaiter(lock, w);
                return NULL;
            }
        } else {
            lock->flags |= LOCK_FLAG_EXCLISIVE_EVENT_IN_USE;
            w->event = &lock->exclusiveEvent;
        }
        QUEUE_InsertTail(&lock->exclusiveWaiters, &w->entry);
        return w;
    }
    return NULL;
}

/**
 * Gets a LockWaiter structure for a shared waiter from the cache or 
 * allocates a new one. Must be called under synchronization
 */
STATIC LockWaiter * LOCK_GetShareWaiter(Lock * lock)
{
    LockWaiter * w = LOCK_GetWaiter(lock);
    if (w) {
        w->event = &lock->shareEvent;
        QUEUE_InsertTail(&lock->shareWaiters, &w->entry);
    }
    return w;
}

/**
 * Acquires the lock mutex. This function is being used instead of simply
 * calling MUTEX_Lock in case if Lock is a const pointer to avoid casts 
 * all over the place.
 */
STATIC void LOCK_GrabMutex(const Lock * lock)
{
    MUTEX_Lock((Mutex*)&lock->mutex);
}

/**
 * Releases the ownership of the lock mutex. This function is being used 
 * instead of simply calling MUTEX_Lock in case if Lock is a const pointer 
 * to avoid casts all over the place.
 */
STATIC void LOCK_ReleaseMutex(const Lock * lock) {
    MUTEX_Unlock((Mutex*)&lock->mutex);
}

/**
 * Returns lock entry at specified index. If index is less that 
 * STATIC_ENTRIES, looks up the "statis entry" (i.e. a part of 
 * the Lock structure), otherwise looks up an "extended" entry.
 * Initially this was a macro but Microsoft Visual C++ couldn't 
 * compile it right, so it became a function. Must be invoked 
 * under synchronization because it touches the extended entries
 * which may be reallocated by the other thread.
 */
STATIC LockEntry * LOCK_GetEntryAt(Lock * lock, int i) 
{
    if (i < STATIC_ENTRIES) {
        return (lock->staticEntries + i);
    } else {
        int pos = i - STATIC_ENTRIES;
        ASSERT(lock->moreEntries);
        return (lock->moreEntries + pos);
    }
}

/**
 * Finds existing static LockEntry for the calling thread (an entry that 
 * is a part of Lock structure). It does not have to be called under 
 * synchronization because:
 *
 * o other thread cannot modify the entry occupied by this thread; and
 * o these entries cannot be deallocated by the other thread (unlike 
 *   the "extended" entries)
 *
 * Returns NULL if entry for calling thread does not exist.
 */
STATIC const LockEntry * LOCK_FindStaticEntry(const Lock * lock) 
{
    int i;
    ThrID self = THREAD_Self();
    for (i=0; i<lock->entriesInUse && i<STATIC_ENTRIES; i++) {
        const LockEntry * entry = lock->staticEntries + i;
        if (entry->id == self) {
            return entry;
        }
    }    
    return NULL;
}

/**
 * Finds existing "extended" LockEntry for the calling thread. This 
 * function must be called under synchronization. Returns NULL if 
 * entry for calling thread does not exist.
 */
STATIC const LockEntry * LOCK_FindExtEntry(const Lock * lock) 
{
    int i;
    ThrID self = THREAD_Self();
    for (i=STATIC_ENTRIES; i<lock->entriesInUse; i++) {
        const LockEntry * entry = lock->moreEntries + i-STATIC_ENTRIES;
        if (entry->id == self) {
            return entry;
        }
    }    
    return NULL;
}

/**
 * Finds a LockEntry for the calling thread. This function must be called 
 * under synchronization. Returns NULL if entry for calling thread does not 
 * exist.
 */
STATIC LockEntry * LOCK_FindEntry(Lock * lock) 
{
    int i;
    ThrID self = THREAD_Self();
    for (i=0; i<lock->entriesInUse; i++) {
        LockEntry * entry = GET_ENTRY(lock,i);
        if (entry->id == self) {
            return entry;
        }
    }    
    return NULL;
}

/**
 * Returns available LockEntry for the calling thread. Reallocates array
 * of thread entries if necessary. Obviously, this function must be called 
 * under synchronization. Returns NULL only if we run out of thread entries 
 * and memory allocation fails.
 */
STATIC LockEntry * LOCK_GetEntry(Lock * lock) 
{
    int i;
    ThrID self = THREAD_Self();
    LockEntry * empty = NULL;  /* vacant empty entry */

    /* 
     * try to find existing entry for this thread. At the same time
     * pick up the first available empty slot.
     */
    for (i=0; i<lock->entriesInUse; i++) {
        LockEntry * entry = GET_ENTRY(lock,i);
        if (entry->id == self) {
            return entry;
        } else if (!empty && !entry->id) {
            empty = entry;
        }
    }

    /* 
     * we didn't find existing slot for the calling thread. 
     * We have to find an empty slot.
     */
    if (!empty) {
        for (; i<lock->numEntries; i++) {
            LockEntry * entry = GET_ENTRY(lock,i);
            if (!entry->id) {
                empty = entry;
                break;
            }
        }

        /*
         * looks like we are out of luck. We need to reallocate the array
         * of thread entries.
         */
        if (!empty) {
            int count = lock->numEntries + STATIC_ENTRIES;
            size_t size = count * sizeof(LockEntry);
            LockEntry * newEntries = (LockEntry*)
                MEM_Realloc(lock->moreEntries, size);
            if (!newEntries) {
                return NULL;
            }
            
            lock->moreEntries = newEntries;
            lock->numEntries = count;
            empty = lock->moreEntries + i - STATIC_ENTRIES;
            memset(empty, 0, sizeof(LockEntry) * STATIC_ENTRIES);
        }

        lock->entriesInUse = i + 1;
    }

    lock->entriesActive++;

    /*
     * initialize the empty entry.
     */
    empty->id = self;
    empty->read = empty->write = 0L;

#if defined(_WIN32) && !defined(_WIN32_WCE)
    /*
     * need to reference the current thread to prevent thread object from
     * being deallocated when the thread dies, and therefore thread id from
     * being reused by the system.
     *
     * On the other hand, in order for something bad to happen, the thread 
     * must exit without releasing a lock. Which is a programming error,
     * i.e. should be fixed. Under normal conditions, this never happens.
     *
     * NOTE: on Windows CE there's no DuplicateHandle() 
     */
    empty->handle = NULL;
    if (!DuplicateHandle(GetCurrentProcess(), GetCurrentThread(),
                         GetCurrentProcess(), &empty->handle, 0,
                         False, DUPLICATE_SAME_ACCESS)) {
        ASSMSG1("DuplicateHandle() failed, error %lu",GetLastError());
    }
#endif /* _WIN32 && !_WIN32_WCE*/

    return empty;
}

/**
 * Locks resource for exclusive use, waits if necessary. Returns True if lock 
 * has been successfully acquired, otherwise False.
 */
Bool LOCK_TimeWriteLock(Lock * lock, long ms) 
{
    Bool ok = True;
    Bool success = False;
    LockWaiter * waiter = NULL;
    Time deadline = 0;

    /*
     * this flag is False if we have found that current thread is NOT 
     * an owner of the resource, so that we don't scan the lock entries
     * more than once.
     */
    Bool maybeOwner = True;
    LockEntry * entry = NULL;

    /* calculate the deadline if it's a wait with timeout */
    if (ms > 0) {
        deadline = TIME_Now() + ms;
    }

    /*
     * we can acquire the resource immediately if
     * 1. resource is unowned and no one is waiting; or
     * 2. this thread is the only one that is using the resource, either 
     *    shared or exclusively 
     */
    MUTEX_Lock(&lock->mutex);
    while (ok) {

        Time now = 0;
        
        /*
         * if this thread already owns this resource exclusively, 
         * we are all set. All we need is to increment entry count.
         */
        if (lock->entriesActive == 1 && maybeOwner) {
            if (!entry) {
                entry = LOCK_FindEntry(lock);
            }
            if (entry) {
                success = True;
                lock->flags |= LOCK_FLAG_EXCLISIVE_LOCK;
                entry->write++; /* convert shared to exclusive */
                break;
            } else {
                maybeOwner = False;
            }
        }

        /* if resource is not owned and no one is waiting, we can have it */
        if (lock->locks <= 0) {
            Bool gotIt = False;
            if (waiter) {
                gotIt = BoolValue(lock->exclusiveWaiters.head.next == 
                    &waiter->entry);
            } else {
                gotIt = BoolValue(QUEUE_Size(&lock->shareWaiters) == 0 &&
                                  QUEUE_Size(&lock->exclusiveWaiters) == 0);
            }

            /*
             * note that it's quite possible that resource is not owned
             * but the wait queue is not empty. this can happen for example
             * if this thread just released the resource and waiters didn't
             * yet have the chance to run. in such case, this thread should
             * be place into the queue to avoid starving the waiters
             */
            if (gotIt) {
                if (!entry) {
                    entry = LOCK_GetEntry(lock);
                }
                if (entry) {
                    success = True;
                    lock->flags |= LOCK_FLAG_EXCLISIVE_LOCK;
                    entry->write++;
                }
                break;
            }
        }

        /*
         * resource cannot be acquired immediately for exclusive access. 
         * If we cannot wait (any longer), break the loop. 
         */
        if (ms == 0) {
            break;
        } else if (ms > 0) {
            /* check for timeout */
            now = TIME_Now();
            if (now >= deadline) {
                break;
            }
        }

        /* 
         * release the mutex and wait for event to be signaled, then 
         * start it all over again. 
         */
        lock->contentions++;
        if (!waiter) {
            waiter = LOCK_GetExclusiveWaiter(lock);
            if (!waiter) break;
        }

        EVENT_Reset(waiter->event);
        MUTEX_Unlock(&lock->mutex);

        /* wait */
        if (ms > 0) {
            long tmo = (long)(deadline - now);
            if (EVENT_TimeWait(waiter->event,tmo) == WAIT_STATE_ERROR) {
                ok = False;
            }
        } else {
            ok = BoolValue(EVENT_Wait(waiter->event) == WAIT_STATE_OK);
        }
        MUTEX_Lock(&lock->mutex);
    }
    if (success) lock->locks++;
    if (waiter) LOCK_ReleaseExclusiveWaiter(lock, waiter);
    lock->bypassCount = 0;
    MUTEX_Unlock(&lock->mutex);
    return success;
}

/**
 * Locks resource for non-exclusive use, waits if necessary. Returns True 
 * if lock has been successfully acquired, otherwise False.
 */
Bool LOCK_TimeReadLock(Lock * lock, long ms) 
{
    Bool ok = True;
    Bool success = False;
    LockWaiter * waiter = NULL;
    Time deadline = 0;

    /*
     * this flag is False if we have found that current thread is NOT 
     * an owner of the resource, so that we don't scan the lock entries
     * more than once.
     */
    Bool maybeOwner = True;
    LockEntry * entry = NULL;

    /* calculate the deadline if it's a wait with timeout */
    if (ms > 0) {
        deadline = TIME_Now() + ms;
    }

    MUTEX_Lock(&lock->mutex);
    while (ok) {
        
        Time now = 0;

        /*
         * if this thread already owns this resource either exclusively 
         * or shared, we are all set. All we need is to increment entry
         * count. NOTE that we don't touch the "exclusive" flag, meaning
         * that if resource has been acquired exclusively, it remains
         * this way.   
         */
        if (maybeOwner) {
            entry = LOCK_FindEntry(lock);
            if (entry) {
                success = True;
                if (lock->flags & LOCK_FLAG_EXCLISIVE_LOCK) {
                    ASSERT(entry->write > 0);
                    entry->write++;
                } else { 
                    ASSERT(entry->write == 0);
                    entry->read++;
                }
                break;
            } else {
                maybeOwner = False; /* don't scan entry table again */
            }
        }

        /* if resource is not owned and no one is waiting, we can have it */
        if (lock->locks <= 0 && 
            QUEUE_Size(&lock->shareWaiters) == 0 &&
            QUEUE_Size(&lock->exclusiveWaiters) == 0) {

            /*
             * note that it's quite possible that resource is not owned
             * but the wait queue is not empty. this can happen for example
             * if this thread just released the resource and waiters didn't
             * yet have the chance to run. in such case, this thread should
             * be place into the queue to avoid starving the waiters
             */
            entry = LOCK_GetEntry(lock);
            if (entry) {
                success = True;
                lock->flags &= ~LOCK_FLAG_EXCLISIVE_LOCK;
                entry->read++;
            }
            break;
        }

        /* 
         * if resource is owned in shared mode, there's a good chance that
         * we can have it immediately. Some restrictions apply (see below)
         */
        if (!(lock->flags & LOCK_FLAG_EXCLISIVE_LOCK)) {

            /*
             * normally we allow this thread to access the resource
             * in readonly mode even if there's an exclusive waiter.
             * However, if we always did that, the exclusive waiter
             * might end up waiting forever if new readonly waters
             * keep coming. To prevent this from happening, we count
             * the number of times an exclusive waiter has been bypassed
             * by a lucky late-coming reader. If this number exceeds
             * the limit, everyone has to stay in the line.
             */
            if (QUEUE_Size(&lock->exclusiveWaiters) == 0  ||
                lock->bypassCount < LOCK_MAX_BYPASS_COUNT) {
                entry = LOCK_GetEntry(lock);
                if (entry) {
                    if (QUEUE_Size(&lock->exclusiveWaiters) > 0) {
                        lock->bypassCount++;
                    }
                    ASSERT(entry->write == 0);
                    success = True;
                    entry->read++;
                }
                break; 
            }
        }

        /*
         * resource cannot be acquired immediately for exclusive access. 
         * If we cannot wait (any longer), break the loop. 
         */
        if (ms == 0) {
            break;
        } else if (ms > 0) {
            /* check for timeout */
            now = TIME_Now();
            if (now >= deadline) {
                break;
            }
        }

        /* 
         * release the mutex and wait for event to be signalled, then 
         * start it all over again. 
         */
        lock->contentions++;
        if (!waiter) {
            waiter = LOCK_GetShareWaiter(lock);
            if (!waiter) break;
        }

        EVENT_Reset(&lock->shareEvent);
        MUTEX_Unlock(&lock->mutex);

        /* wait */
        if (ms > 0) {
            long tmo = (long)(deadline - now);
            if (EVENT_TimeWait(waiter->event, tmo) == WAIT_STATE_ERROR) {
                ok = False;
            }
        } else {
            ok = BoolValue(EVENT_Wait(waiter->event) == WAIT_STATE_OK);
        }

        MUTEX_Lock(&lock->mutex);
    }

    if (success) lock->locks++;
    if (waiter) LOCK_ReleaseWaiter(lock, waiter);
    MUTEX_Unlock(&lock->mutex);
    return success;
}

/**
 * Attempts to acquire read lock immediately, without waiting. 
 * Returns True if the lock has been acquired, False otherwise
 * (which includes error conditions)
 */
Bool LOCK_TryReadLock(Lock * lock) 
{
    return LOCK_TimeReadLock(lock,0);
}

/**
 * Attempts to acquire exclusive lock immediately, without waiting. 
 * Returns True if the lock has been acquired, False otherwise
 * (which includes error conditions)
 */
Bool LOCK_TryWriteLock(Lock * lock)
{
    return LOCK_TimeWriteLock(lock,0);
}

/**
 * Locks resource for non-exclusive use. If the resource cannot be acquired 
 * immediately, waits until it becomes available. Returns True if lock has 
 * been successfully acquired, False if an error occured.
 */
Bool LOCK_ReadLock(Lock * lock)
{
    return LOCK_TimeReadLock(lock,-1);
}

/**
 * Locks resource for exclusive use. If the resource cannot be acquired 
 * immediately, waits until it becomes available. Returns True if lock has 
 * been successfully acquired, False if an error occured.
 */
Bool LOCK_WriteLock(Lock * lock)
{
    return LOCK_TimeWriteLock(lock,-1);
}

/**
 * Release n recursively acquired locks.
 */
void LOCK_UnlockMany(Lock * lock, int n) 
{
    if (n > 0) {
        LockEntry * entry;
        MUTEX_Lock(&lock->mutex);
        entry = LOCK_FindEntry(lock);

        /*
         * if we cannot find the entry, it means that current thread 
         * does not own the lock. It's a programming error. 
         */
        ASSERT(entry);
        if (entry) {
            lock->locks--;
            
            /* first release write locks */
            if (entry->write > 0) {
                if (entry->write >= n) {
                    entry->write -= n;
                    n = 0;
                } else {
                    n -= entry->write;
                    entry->write = 0;
                }
            }

            /* then read locks */
            if (n > 0) {
                entry->read -= n;
            }

            /* 
             * ASSERT that current thread does not release more locks than 
             * it has acquired
             */
            ASSERT(lock->locks >= 0);
            ASSERT(entry->read >= 0);
            ASSERT(entry->write >= 0);

            /*
             * no more work to do unless calling thread has released the
             * resource (i.e. usage count came down to zero)
             */
            if ((entry->read + entry->write) <= 0) {
                int i;
                int inUse;
                QEntry * e;
                LockWaiter * shareWaiter = NULL;
                LockWaiter * exclusiveWaiter = NULL;

                entry->id = 0;

#if defined(_WIN32) && !defined(_WIN32_WCE)
                VERIFY(CloseHandle(entry->handle));
                entry->handle = NULL;
#endif /* _WIN32 && !_WIN32_WCE*/

                lock->entriesActive--;
                ASSERT(lock->entriesActive >= 0);

                /* 
                 * update lock->entriesInUse
                 * NOTE that LOCK_FindStaticEntry() may access it without 
                 * synchronization.
                 */
                i = lock->entriesInUse - 1;
                inUse = 0;
                while (i >= 0) {
                    LockEntry * lockEntry = GET_ENTRY(lock,i);
                    if (lockEntry->id) {
                        inUse = i + 1;
                        break;
                    }
                    i--;
                }

                lock->entriesInUse = inUse;

                /*
                 * if resource was acquired exclusively, it must be free now
                 */
                if (lock->flags & LOCK_FLAG_EXCLISIVE_LOCK) {
                    ASSERT(!lock->locks);
                    lock->flags &= ~LOCK_FLAG_EXCLISIVE_LOCK;
                }

                /* 
                 * release the waiters in the order they have arrived
                 */
                e = QUEUE_First(&lock->shareWaiters);
                if (e) shareWaiter = QCAST(e,LockWaiter,entry);
                e = QUEUE_First(&lock->exclusiveWaiters);
                if (e) exclusiveWaiter = QCAST(e,LockWaiter,entry);

                if (exclusiveWaiter && (!shareWaiter ||
                    exclusiveWaiter->index < shareWaiter->index)) {
                    EVENT_Set(exclusiveWaiter->event);
                } else if (shareWaiter) {
                    /* this should unblock all shared waiters */
                    EVENT_Set(shareWaiter->event);
                }

            } else if (lock->flags & LOCK_FLAG_EXCLISIVE_LOCK) {

                /*
                 * if the owner of the lock has released all its WRITE locks 
                 * but still have some READ locks, switch to shared mode.
                 */
                if (!entry->write) {            
                    QEntry * e;
                    LockWaiter * shareWaiter = NULL;
                    LockWaiter * exclusiveWaiter = NULL;

                    ASSERT(entry->read > 0);
                    lock->flags &= ~LOCK_FLAG_EXCLISIVE_LOCK;

                    /*
                     * wake up shared waiters only unless the exclusive
                     * waiter is first in the line
                     */
                    e = QUEUE_First(&lock->shareWaiters);
                    if (e) shareWaiter = QCAST(e,LockWaiter,entry);
                    e = QUEUE_First(&lock->exclusiveWaiters);
                    if (e) exclusiveWaiter = QCAST(e,LockWaiter,entry);

                    if (shareWaiter && (!exclusiveWaiter ||
                        shareWaiter->index < exclusiveWaiter->index)) {
                        EVENT_Set(shareWaiter->event);
                    }
                }
            }
        }
        MUTEX_Unlock(&lock->mutex);
    }
}

/**
 * Release previously acquired lock.
 */
void LOCK_Unlock(Lock * lock) 
{
    LOCK_UnlockMany(lock, 1);
}
 
/**
 * Determines if the calling thread has READ access to the resource 
 * protected by the lock.
 */
Bool LOCK_CanRead(const Lock * lock) 
{
    if (lock->locks > 0) {
        const LockEntry * entry = LOCK_FindStaticEntry(lock);
        if (!entry && lock->entriesInUse > STATIC_ENTRIES) {
            /* must synchronize access to the "extended" entries */
            LOCK_GrabMutex(lock);
            entry = LOCK_FindExtEntry(lock);
            LOCK_ReleaseMutex(lock);
        }
        return BoolValue(entry != NULL);
    }
    return False;
}

/**
 * Determines if the calling thread has WRITE access to the resource 
 * protected by the lock.
 */
Bool LOCK_CanWrite(const Lock * lock) 
{
    if (lock->locks > 0 && (lock->flags & LOCK_FLAG_EXCLISIVE_LOCK)) {
        const LockEntry * entry = LOCK_FindStaticEntry(lock);
        if (!entry && lock->entriesInUse > STATIC_ENTRIES) {
            /* must synchronize access to the "extended" entries */
            LOCK_GrabMutex(lock);
            entry = LOCK_FindExtEntry(lock);
            LOCK_ReleaseMutex(lock);
        }
        return BoolValue(entry != NULL);
    }
    return False;
}

/**
 * Determines what kind of access the calling thread has.
 */
LockAccess LOCK_GetAccess(const Lock * lock) 
{
    if (lock->locks > 0) {
        const LockEntry * entry = LOCK_FindStaticEntry(lock);
        if (!entry && lock->entriesInUse > STATIC_ENTRIES) {
            /* must synchronize access to the "extended" entries */
            LOCK_GrabMutex(lock);
            entry = LOCK_FindExtEntry(lock);
            LOCK_ReleaseMutex(lock);
        }
        if (entry) {
            if (lock->flags & LOCK_FLAG_EXCLISIVE_LOCK) {
                return Lock_Write;
            } else {
                return Lock_Read;
            }
        }
    }
    return Lock_None;
}

/**
 * Determines how many times this lock has been locked by the current thread.
 * This includes both read and write locks. If current thread does not own 
 * the lock, the return value is zero as you might expect.
 */
int LOCK_GetLockCount(const Lock * lock) 
{
    if (lock->locks > 0) {
        const LockEntry * entry = LOCK_FindStaticEntry(lock);
        if (!entry && lock->entriesInUse > STATIC_ENTRIES) {
            /* must synchronize access to the "extended" entries */
            LOCK_GrabMutex(lock);
            entry = LOCK_FindExtEntry(lock);
            LOCK_ReleaseMutex(lock);
        }

        /* no need to synchronize access to our entry. 
         * Other threads won't touch it. */
        if (entry) {
            return (entry->read + entry->write);
        }
    }
    return 0;
}

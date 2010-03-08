/*
 * $Id: u_thread.c,v 1.4 2005/05/03 13:08:35 slava Exp $
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

#include "s_libp.h"
#include "s_thrp.h"
#include "s_mem.h"

/**
 * Thread API implementation for POSIX environment
 */

#ifndef _USE_PTHREADS
#  error "_USE_PTHREADS must be defined to compile this code"
#endif

/*
 * Thread start context
 */
typedef struct _ThrStart {
    ThrProc proc;
    void *  arg;
} ThrStart;

/*
 * Descriptor of a thread local value
 */
struct _ThrKey {
    pthread_key_t index;    /* the actual key */
};

/**
 * Unix-specific initialization
 */
void THREAD_PlatformInitialize()
{
    __pthread_initialize();
}

/**
 * Unix-specific deinitialization
 */
void THREAD_PlatformShutdown()
{
}

/**
 * Unix-specific thread proc
 */
STATIC void * PTHREAD_ThreadProc(void * arg) 
{
    ThrStart * thr = (ThrStart*)arg;
    ThrProc proc = thr->proc;
    void * param = thr->arg;
    MEM_Free(thr);
    (*proc)(param);
    return NULL;
}

/**
 * Create a new thread, returning True on success
 */
Bool THREAD_Create(ThrID* id, ThrProc proc, void * arg)
{
    ThrStart* start = NULL;
    Bool success = False;
    pthread_t thrid = (pthread_t)0;
    pthread_attr_t attr;
    int err;

    ASSERT(THREAD_IsInited());

    /* system-wide contention */
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

    if (id) *id = (ThrID)0;
    start = MEM_New(ThrStart);
    if (start) {
        start->proc = proc;
        start->arg = arg;

        /*
         * create POSIX thread
         */
        err = pthread_create(&thrid, &attr, PTHREAD_ThreadProc, start);
        ASSERT(!err);
        if (!err) {
            success = True;
            if (id) *id = thrid;
        } else {
            TRACE1("pthread_create() failed, error %d\n", err);
        }

        /*
         * if we have failed to start the thread, deallocate the context
         */
        if (!success) MEM_Free(start);
    }

    /*
     * destroy POSIX thread attributes
     */
    pthread_attr_destroy(&attr);
    return success;
}

/**
 * Returns the thread ID of the calling thread. 
 */
ThrID THREAD_Self()
{
    ASSERT(THREAD_IsInited());
    return pthread_self();
}

/**
 * This call essentially invalidates ThrID variable. 
 * Detached thread cannot be "joined" (POSIX semantics)
 */
Bool THREAD_Detach(ThrID id)
{
    int err;
    ASSERT(THREAD_IsInited());
    err = pthread_detach(id);
    if (!err) {
        return True;
    } else {
        TRACE2("pthread_detach(%d) failed, error %d\n",id,err);
        return False;
    }
}

/**
 * Waits until the specified thread terminates
 */
Bool THREAD_Join(ThrID id)
{
    int err;
    ASSERT(THREAD_IsInited());

    if (id == THREAD_Self()) {
        ASSMSG("You've got DEADLOCK, idiot!");
        return False;
    }

    err = pthread_join(id, NULL);
    if (!err) {
        return True;
    } else {
        TRACE2("pthread_join(%d) failed, error %d\n",id,err);
        return False;
    }
}

/**
 * Terminates the current thread
 */
void THREAD_Exit()
{
    pthread_exit(NULL);
}

/**
 * Relinguish the remainder of the time slice
 */
void THREAD_Yield()
{
    sched_yield();
}

/**
 * Sleep with millisecond precision
 */
void THREAD_Sleep(long ms)
{
    /* 
     * select with all three sets empty, n zero, and a non-null timeout 
     * is a fairly portable way to sleep with subsecond precision
     */
    struct timeval tv;
    tv.tv_sec = ms/1000;         /* seconds */
    tv.tv_usec = (ms%1000)*1000; /* microseconds */
    select(0, NULL, NULL, NULL, &tv);
}

/*
 * Creates a thread-local key
 */
ThrKey THREAD_CreateKey(ThrClean proc)
{
    ThrKey key = NULL;
    ASSERT(THREAD_IsInited());
    if (THREAD_IsInited()) {
        key = MEM_New(struct _ThrKey);
        if (key) {
            int err = pthread_key_create(&key->index, proc);
            if (!err) {
                return key;
            }
            TRACE1("pthread_key_create() failed, error %d\n",err);
            MEM_Free(key);
        }
    }
    return NULL;
}

/**
 * Deletes the thread local key created by THREAD_CreateKey function.
 * Does not run the destructors.
 */
void THREAD_DeleteKey(ThrKey key)
{
    ASSERT(key);
    ASSERT(THREAD_IsInited());
    if (THREAD_IsInited() && key) {
        DEBUG_ONLY(int err = )pthread_key_delete(key->index);
        ASSERT(!err);
        MEM_Free(key);
    }
}

/* 
 * Checks if thread local value can be set for this key.
 */
Bool THREAD_CanSetValue(ThrKey key)
{
    return (key != NULL);
}

/**
 * Sets a thread-local value
 */
Bool THREAD_SetValue(ThrKey key, void * value)
{
    ASSERT(key);
    if (key) {
        int err = pthread_setspecific(key->index, value);
        if (!err) return True;
        TRACE1("pthread_setspecific() failed, error %d\n",err);
    }
    return False;
}

/**
 * Returns a thread-local value
 */
void * THREAD_GetValue(ThrKey key)
{
    ASSERT(key);
    if (key) {
        return pthread_getspecific(key->index);
    }
    return NULL;
}

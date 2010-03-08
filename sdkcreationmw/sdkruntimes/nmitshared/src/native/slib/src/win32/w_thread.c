/*
 * $Id: w_thread.c,v 1.3 2005/02/25 02:53:40 slava Exp $
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
#include "s_vector.h"

/**
 * Thread API implementation for Win32 environment
 */

#ifndef _WIN32
#  error "_WIN32 must be defined to compile Win32 code"
#endif

#define MAX_DESTRUCTOR_ITERATIONS 10

/*
 * Thread local data
 */
typedef struct _ThrData {
    ThrProc proc;           /* thread function */
    void *  arg;            /* thread argument */
    DWORD thrid;            /* Windows thread id (not really being used) */
    Vector cleanupList;     /* contains sorted ThrKey pointers */
    HANDLE userHandle;      /* thread handle we have returned to the user */
    HANDLE originalHandle;  /* the thread handle returned by 
                             * CreateThread call, or NULL if current 
                             * thread wasn't created by THREAD_Create.
                             * The code outside this library never sees
                             * this handle. 
                             */
    Bool exiting;           /* true when invoking TLS destructors */
} ThrData;

/*
 * Descriptor of a thread local value. If cleanup function isn't NULL,
 * then this structure is a part of heavy-weight ThrCleanupKey defined
 * below.
 */
struct _ThrKey {
    DWORD index;            /* TLS index */
    ThrClean clean;         /* the destructor */
    long ref;               /* ref count */
};

/* 
 * Win32 thread local data.
 * 
 * WIN32_TLS_Data            points to ThrData structure for the thread
 *                           created by THREAD_Create. NULL for the threads
 *                           that were not created by SLIB
 * 
 * WIN32_TLS_ThreadHandle    contains the thread handle returned by
 *                           THREAD_Create and THREAD_Self functions.
 *                           It's a duplicate of the original handle
 *                           (see DuplicateHandle documentation)
 *
 * NOTE: it's the responsibility of the caller to close the handle
 * returned from THREAD_Create or THREAD_Self. It's done by calling
 * THREAD_Detach or THREAD_Join. 
 *
 * Most likely, THREAD_Self being called from a thread NOT created 
 * by THREAD_Create would result in a handle leak, and therefore 
 * should be avoided. That's because there's no way to find out 
 * whether the handle returned by THREAD_Self is the one returned
 * from THREAD_Create or it was auto-created by THREAD_Self function,
 * in which case no one would care to close it.
 */
STATIC DWORD WIN32_TLS_Data = TLS_OUT_OF_INDEXES;
STATIC DWORD WIN32_TLS_ThreadHandle = TLS_OUT_OF_INDEXES;

#define WIN32_FAILURE(what) \
    TRACE2("%s() failed, error %lu\n",TEXT(#what),GetLastError());

/**
 * This Win32 specific function checks if thread handle is valid
 */
STATIC Bool WIN32_IsThread(HANDLE handle) 
{
    DWORD status;
    if (GetExitCodeThread(handle,&status)) {
        return True;
    }
    WIN32_FAILURE(GetExitCodeThread);
    TRACE1("ERROR: not a thread handle: 0x%08lX\n",handle);
    return False;
}

/**
 * Win32-specific thread proc
 */
STATIC DWORD WINAPI WIN32_ThreadProc(LPVOID arg) 
{
    ThrData * thr = arg;
    int i, n = 1;

#ifdef _USE_EXCEPTION_HANDLING
    __try { 
#endif /* _USE_EXCEPTION_HANDLING */

    ASSERT(WIN32_IsThread(thr->userHandle));
    ASSERT(WIN32_IsThread(thr->originalHandle));
    ASSERT(TlsGetValue(WIN32_TLS_Data) == NULL);
    VERIFY(TlsSetValue(WIN32_TLS_Data, thr));
    if (WIN32_TLS_Data != TLS_OUT_OF_INDEXES) {
        ASSERT(TlsGetValue(WIN32_TLS_ThreadHandle) == NULL);
        VERIFY(TlsSetValue(WIN32_TLS_ThreadHandle, thr->userHandle));
    }
    
    (*thr->proc)(thr->arg);

#ifdef _USE_EXCEPTION_HANDLING
    } __except(EXCEPTION_EXECUTE_HANDLER) {
        ASSMSG1("EXCEPTION %08lX in thread proc!",GetExceptionCode());
    }
#endif /* _USE_EXCEPTION_HANDLING */

    /* invoke destructors for thread local variables */
    for (i=0; i<MAX_DESTRUCTOR_ITERATIONS && n > 0; i++) {
        int k;
        n = 0;
        for (k=0; k<VECTOR_Size(&thr->cleanupList); k++) {
            ThrKey key = VECTOR_Get(&thr->cleanupList, k);
            void * value = TlsGetValue(key->index);
            if (value) {
                TlsSetValue(key->index, NULL);
#ifdef _USE_EXCEPTION_HANDLING
                __try { 
#endif /* _USE_EXCEPTION_HANDLING */

                key->clean(value);
                ASSERT(!TlsGetValue(key->index));

#ifdef _USE_EXCEPTION_HANDLING
                } __except(EXCEPTION_EXECUTE_HANDLER) {
                    TlsSetValue(key->index,NULL);
                    ASSMSG1("EXCEPTION %08lX in cleanup proc!",
                            GetExceptionCode());
                }
#endif /* _USE_EXCEPTION_HANDLING */
                n++;
            }
        }
    }

    ASSERT(i<MAX_DESTRUCTOR_ITERATIONS);

    /* Dereference ThrKey structures */
    while ((n = VECTOR_Size(&thr->cleanupList)) > 0) {
        ThrKey key = VECTOR_Remove(&thr->cleanupList, n-1);
        InterlockedDecrement(&key->ref);
        ASSERT(key->ref >= 0);
        if (key->ref < 1) {
            ASSERT(key->index == TLS_OUT_OF_INDEXES);
            MEM_Free(key);
        }
    }

#ifndef _WIN32_WCE
    /* close the handle */
    ASSERT(WIN32_IsThread(thr->originalHandle));
    if (WIN32_IsThread(thr->originalHandle)) {
        if (!CloseHandle(thr->originalHandle)) {
            WIN32_FAILURE(CloseHandle);
        }
    }
#endif /* _WIN32_WCE */

    VECTOR_Destroy(&thr->cleanupList);
    MEM_Free(thr);
    return 0;
}

/**
 * Win32 specific initialization
 */
void THREAD_PlatformInitialize()
{
    WIN32_TLS_ThreadHandle = TlsAlloc();
    ASSERT(WIN32_TLS_ThreadHandle != TLS_OUT_OF_INDEXES);
    if (WIN32_TLS_ThreadHandle != TLS_OUT_OF_INDEXES) {
        /* Allow this to fail */
        WIN32_TLS_Data = TlsAlloc();
        ASSERT(WIN32_TLS_Data != TLS_OUT_OF_INDEXES);
        return;
    }

    /* unrecoverable error */
    SLIB_Abort(TEXT("THREAD"));
}

/**
 * Win32 specific deinitialization
 */
void THREAD_PlatformShutdown()
{
    TlsFree(WIN32_TLS_ThreadHandle);
    WIN32_TLS_ThreadHandle = TLS_OUT_OF_INDEXES;
    if (WIN32_TLS_Data != TLS_OUT_OF_INDEXES) {
        TlsFree(WIN32_TLS_Data);
        WIN32_TLS_Data = TLS_OUT_OF_INDEXES;
    }
}

/**
 * Create a new thread, returning True on success
 */
Bool THREAD_Create(ThrID* id, ThrProc proc, void * arg)
{
    ThrData * thr = NULL;
    ASSERT(THREAD_IsInited());

    if (id) *id = (ThrID)0;
    if (!THREAD_IsInited()) return False;

    /* allocate thread data */
    thr = MEM_New(ThrData);
    if (thr) {
        memset(thr, 0, sizeof(*thr));
        thr->proc = proc;
        thr->arg = arg;
        VERIFY(VECTOR_Init(&thr->cleanupList,0,NULL,NULL));

        /*
         * create Win32 thread. Thread is created in suspended state so that
         * we can pass thread handle in ThrStart structure
         */
        thr->originalHandle = CreateThread(NULL, 0, 
            WIN32_ThreadProc, thr, CREATE_SUSPENDED, 
            &thr->thrid);

        ASSERT(thr->originalHandle);
        if (thr->originalHandle) {
            if (id) {
#  ifndef _WIN32_WCE
                /* will return duplicate to the user */
                if (!DuplicateHandle(GetCurrentProcess(), thr->originalHandle,
                    GetCurrentProcess(), &thr->userHandle, 0, FALSE, 
                    DUPLICATE_SAME_ACCESS)) {
                    WIN32_FAILURE(DuplicateHandle);
                    VERIFY(TerminateThread(thr->originalHandle,0xffffffff));
                    VERIFY(CloseHandle(thr->originalHandle));
                    VECTOR_Destroy(&thr->cleanupList);
                    MEM_Free(thr);
                    return False;
                }
#  else /* _WIN32_WCE */
                thr->userHandle = thr->originalHandle;
#  endif /* _WIN32_WCE */

                /* copy the handle before resuming the thread */
                ASSERT(WIN32_IsThread(thr->userHandle));
                *id = thr->userHandle;
            }
            if (ResumeThread(thr->originalHandle) != (DWORD)-1) {
                return True;
            } else {
                if (id) *id = (ThrID)0;
                WIN32_FAILURE(ResumeThread);
                VERIFY(CloseHandle(thr->originalHandle));
#ifndef _WIN32_WCE
                if (thr->userHandle) {
                    VERIFY(CloseHandle(thr->userHandle));
                }
#endif /* _WIN32_WCE */
            }

        } else {
            WIN32_FAILURE(CreateThread);
        }

        VECTOR_Destroy(&thr->cleanupList);
        MEM_Free(thr);
    }

    return False;
}

/**
 * Returns the thread ID of the calling thread. 
 */
ThrID THREAD_Self()
{
    ASSERT(THREAD_IsInited());
    if (WIN32_TLS_ThreadHandle != TLS_OUT_OF_INDEXES) {
        HANDLE handle = (HANDLE)TlsGetValue(WIN32_TLS_ThreadHandle);
        if (!handle) {
            /*
             * NOTE: we only get here if this functions is invoked by 
             * a thread  that was not created by THREAD_Create()
             */
#ifdef _WIN32_WCE
            /*
             * Under WinCE we use current thread ID instead of thread 
             * handle because DuplicateHandle() does not exist under CE.
             * Without DuplicateHandle() it seems to be impossible to
             * get a handle to the current thread. Let's hope that the
             * probarility of a thread id being equal to a thread handle
             * is reasonably low. 
             */
            handle = (HANDLE)GetCurrentThreadId();
#else /* !_WIN32_WCE */
            /* 
             * This means that a thread that was not created by
             * THREAD_Create is calling THREAD_Self(). This is 
             * a potential source of handle leaks. 
             *
             * NOTE:
             * GetCurrentThread only returns a pseudo-handle
             * which is only valid in the current thread context.
             * Therefore, you should not pass the handle to
             * other threads for whatever purpose
             */
            if (DuplicateHandle(
                GetCurrentProcess(),
                GetCurrentThread(),
                GetCurrentProcess(),
                &handle, 0, FALSE,
                DUPLICATE_SAME_ACCESS)) {
                VERIFY(TlsSetValue(WIN32_TLS_ThreadHandle,handle));
            } else {
                WIN32_FAILURE(DuplicateHandle);
            }
#endif /* !_WIN32_WCE */
        }
        return handle;
    } else {
        return (HANDLE)NULL;
    }
}

/**
 * This call essentially invalidates ThrID variable. 
 * Detached thread cannot be "joined" (POSIX semantics)
 */
Bool THREAD_Detach(ThrID id)
{
    ASSERT(THREAD_IsInited());
    /* just close the handle */
    if (WIN32_IsThread(id)) {
        if (CloseHandle(id)) {
            return True;
        }
        WIN32_FAILURE(CloseHandle);
    }
    return False;
}

/**
 * Waits until the specified thread terminates
 */
Bool THREAD_Join(ThrID id)
{
    ASSERT(THREAD_IsInited());
    if (id == THREAD_Self()) {
        ASSMSG("You've got DEADLOCK, idiot!");
        return False;
    }

    ASSERT(WIN32_IsThread(id));
    if (WIN32_IsThread(id)) {
        WaitForSingleObject(id,INFINITE);
        CloseHandle(id);
        return True;
    }
    return False;
}

/**
 * Terminates the current thread
 */
void THREAD_Exit()
{
    ASSERT(THREAD_IsInited());
    ExitThread(0);
}

/**
 * Relinguish the remainder of the time slice
 */
void THREAD_Yield()
{
    Sleep(0);
}

/**
 * Sleep with millisecond precision
 */
void THREAD_Sleep(long ms)
{
    Sleep(ms);
}

/*
 * Creates a thread-local key
 */
ThrKey THREAD_CreateKey(ThrClean proc)
{
    ASSERT(THREAD_IsInited());
    if (THREAD_IsInited()) {
        DWORD index = TlsAlloc();
        ASSERT(index != TLS_OUT_OF_INDEXES);
        if (index != TLS_OUT_OF_INDEXES) {
            ThrKey key = MEM_New(struct _ThrKey);
            if (key) {
                key->index = index;
                key->clean = proc;
                key->ref = 1;
                return key;
            }
            TlsFree(index);
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
        VERIFY(TlsFree(key->index));
        key->index = TLS_OUT_OF_INDEXES;
        InterlockedDecrement(&key->ref);
        ASSERT(key->ref >= 0);
        if (key->ref < 1) {
            MEM_Free(key);
        }
    }
}

/* 
 * Checks if thread local value can be set for this key.
 *
 * NOTE: under Win32 we cannot run destructors for the thread that were 
 * not created by THREAD_Create(). Therefore, we refuse to set thread 
 * local value for such threads if key has a destructor.
 */
Bool THREAD_CanSetValue(ThrKey key)
{
    return (key && ((!key->clean) || (WIN32_TLS_Data != TLS_OUT_OF_INDEXES &&
        TlsGetValue(WIN32_TLS_Data))));
}

/**
 * Sets a thread-local value
 */
Bool THREAD_SetValue(ThrKey key, void * value)
{
    ASSERT(key);
    if (key) {

        /* 
         * NOTE: under Win32 we cannot run destructors for the thread that 
         * were not created by THREAD_Create(). Therefore, we refuse to set 
         * thread local value for such threads if key has a destructor.
         */
        if (THREAD_CanSetValue(key)) {
            if (key->clean) {
                int pos;
                ThrData* thr = TlsGetValue(WIN32_TLS_Data);
                void * prev = TlsGetValue(key->index);
                if (value && !prev) {
                    pos = VECTOR_Search(&thr->cleanupList,key,NULL);
                    if (pos < 0) {
                        InterlockedIncrement(&key->ref);
                        ASSERT(pos < 0);
                        if (VECTOR_Insert(&thr->cleanupList, -(pos+1), key)) {
                            if (TlsSetValue(key->index, value)) {
                                return True;
                            }
                            ASSMSG1("TlsSetValue err %u",GetLastError());
                            VECTOR_Remove(&thr->cleanupList,-(pos+1));
                        }
                        return False;
                    }
                }
            }

            /* set the thread local value */
            if (TlsSetValue(key->index, value)) {
                return True;
            }
            ASSMSG1("TlsSetValue err %u",GetLastError());
        } else {
            TRACE("SLIB: attempt to set TLS value for non-SLIB thread\n");
        }
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
        ASSERT(key->index != TLS_OUT_OF_INDEXES);
        ASSERT(key->index != 0xfdfdfdfd);
        ASSERT(key->index != 0xcdcdcdcd);
        return TlsGetValue(key->index);
    }
    return NULL;
}

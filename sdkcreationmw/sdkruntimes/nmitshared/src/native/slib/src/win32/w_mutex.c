/*
 * $Id: w_mutex.c,v 1.1 2005/02/19 00:37:41 slava Exp $
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

#include "s_mutex.h"

/**
 * Mutex API implementation for Win32 environment
 */

#ifndef _WIN32
#  error "_WIN32 must be defined to compile Win32 code"
#endif

/**
 * Initialize a mutex. Returns False if initialization fails.
 */
Bool MUTEX_Init(Mutex * m)
{
    m->handle = CreateMutex(NULL, False, NULL);
    if (!m) {
        ASSMSG1("CreateMutex() failed, error %lu", GetLastError());
        return False;
    }
    return True;
}

/**
 * Destroy the mutex 
 */
void MUTEX_Destroy(Mutex * m)
{
    VERIFY(CloseHandle(m->handle));
}

/**
 * Try to acquire the mutex. Returns True if mutex has been successfully
 * acquired, False otherwise.
 */
Bool MUTEX_TryLock(Mutex * m)
{
    DWORD status = WaitForSingleObject(m->handle,0);
    switch (status) {
        case WAIT_ABANDONED_0:
            ASSMSG("Mutex was abandoned!");
            /* fall through */
        case WAIT_OBJECT_0: return True;
        case WAIT_TIMEOUT:  return False;
    }
    TRACE1("WaitForSingleObject() status %08lX\n",status);
    ASSMSG1("WaitForSingleObject() failed, error %d",GetLastError());
    return False;
}

/**
 * Locks the mutex. If mutex is already locked, waits until it becomes
 * available. Returns True if mutex has been successfully acquired, 
 * False otherwise.
 */
Bool MUTEX_Lock(Mutex * m)
{
    DWORD status = WaitForSingleObject(m->handle,INFINITE);
    switch (status) {
        case WAIT_ABANDONED_0:
            ASSMSG("Mutex was abandoned!");
            /* fall through */
        case WAIT_OBJECT_0: return True;
        case WAIT_TIMEOUT:  return False;
    }
    TRACE1("WaitForSingleObject() status %08lX\n",status);
    ASSMSG1("WaitForSingleObject() failed, error %d",GetLastError());
    return False;
}

/**
 * Release the mutex.
 */
void MUTEX_Unlock(Mutex * m)
{
    if (!ReleaseMutex(m->handle)) {
        TRACE1("ERROR: thr %08lX can't release mutex\n",GetCurrentThreadId());
        ASSMSG1("ReleaseMutex() failed, error %lu", GetLastError());
    }
}

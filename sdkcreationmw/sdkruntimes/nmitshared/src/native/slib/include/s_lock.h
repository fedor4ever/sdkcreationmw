/*
 * $Id: s_lock.h,v 1.11 2003/02/01 05:37:54 slava Exp $
 *
 * Copyright (C) 2000-2003 by Slava Monich
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

#ifndef _SLAVA_LOCK_H_
#define _SLAVA_LOCK_H_

#include "s_def.h"
#include "s_queue.h"
#include "s_thread.h"
#include "s_event.h"
#include "s_mutex.h"

/*
 * PTHREAD implementation of read-write lock apparently assumes that 
 * locks cannot be acquired recursively (i.e. fail with EDEADLK if 
 * current thread already owns the read-write lock for writing or 
 * reading). This is why I have essentially re-implemented it. 
 * I believe that ability to acquire synchronization resources 
 * recursively significantly simplifies programming (although in 
 * same cases makes it easier to get into a deadlock situation).  
 * The other reason was that Win32 does not have anything like 
 * pthread's rwlock, i.e. I would pretty much have to re-implement 
 * it anyway if I want my code run under Windows (which I certainly do).
 */

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef struct _LockEntry {
    ThrID  id;                  /* zero if entry is vacant */
    int write;                  /* write entry count */
    int read;                   /* read entry count */

#if defined(_WIN32) && !defined(_WIN32_WCE)
    HANDLE handle;              /* Win32 thread handle */
#endif /* _WIN32 && !_WIN32_WCE*/

} LockEntry;

typedef struct _Lock {
    Mutex mutex;                /* protects this structure */

    Queue shareWaiters;         /* shared waiters */
    Queue exclusiveWaiters;     /* exclusive waiters */
    Queue waiterCache;          /* cached waiters */

    Event shareEvent;           /* event for all readers */
    Event exclusiveEvent;       /* event for the first writer */
    int   flags;                /* flags */
    int   locks;                /* total lock count */
    int   bypassCount;          /* n of times we let reader ahead of writer */
    long  contentions;          /* contention count, for debugging */

    int   numEntries;           /* total number of thread entries available */
    int   entriesInUse;         /* max entry index + 1 */
    int   entriesActive;        /* number of active entries */

    int   eventsInCache;        /* number of cached events */
    Event * eventCache[4];      /* cached writer events */

    LockEntry staticEntries[4]; /* statically allocated thread entries */
    LockEntry * moreEntries;    /* points to additional thread entries */
} Lock;

/* type of access */
typedef enum _LockAccess {
    Lock_None,
    Lock_Read,
    Lock_Write
} LockAccess;

/* prototypes */
extern void LOCK_InitModule    P_((void));
extern void LOCK_Shutdown      P_((void));
extern Lock * LOCK_Create      P_((void));
extern Bool LOCK_Init          P_((Lock * lock));
extern void LOCK_Delete        P_((Lock * lock));
extern void LOCK_Destroy       P_((Lock * lock));
extern Bool LOCK_ReadLock      P_((Lock * lock));
extern Bool LOCK_TryReadLock   P_((Lock * lock));
extern Bool LOCK_TimeReadLock  P_((Lock * lock, long ms));
extern Bool LOCK_WriteLock     P_((Lock * lock));
extern Bool LOCK_TryWriteLock  P_((Lock * lock));
extern Bool LOCK_TimeWriteLock P_((Lock * lock, long ms));
extern void LOCK_Unlock        P_((Lock * lock));
extern void LOCK_UnlockMany    P_((Lock * lock, int n));
extern Bool LOCK_CanRead       P_((const Lock * lock));
extern Bool LOCK_CanWrite      P_((const Lock * lock));
extern int  LOCK_GetLockCount  P_((const Lock * lock));
extern LockAccess LOCK_GetAccess P_((const Lock * lock));

#ifdef __cplusplus
} /* end of extern "C" */
#endif  /* __cplusplus */

#endif /* _SLAVA_LOCK_H_ */

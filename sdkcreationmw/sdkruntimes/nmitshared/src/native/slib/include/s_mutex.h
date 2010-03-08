/*
 * $Id: s_mutex.h,v 1.15 2005/02/20 20:31:29 slava Exp $
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

#ifndef _SLAVA_MUTEX_H_
#define _SLAVA_MUTEX_H_

#include "s_def.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef struct _Mutex Mutex;

/* platform specific mutex definition */
#if defined(_NT_KERNEL)

/* NOTE: this mutex can be used at DISPATCH_LEVEL as well as PASSIVE_LEVEL */
struct _Mutex {
    KSPIN_LOCK lock;    /* kernel spinlock */
    KIRQL irql;         /* IRQL at which spinlock was acquired */
    BOOLEAN dpc;        /* True if spinlock was acquired at DISPATCH_LEVEL */
    BOOLEAN acquired;   /* True if mutex is acquired (only for debugging) */
    BOOLEAN spare;      /* unused, always zero. Ensures proper alignment */
    PKTHREAD thread;    /* thread that owns the lock (only for debugging) */
};

#elif defined(_LINUX_KERNEL)

#include <linux/spinlock.h>
#include <asm/system.h>

struct _Mutex {
    spinlock_t lock;
    unsigned long flags;
    Bool acquired;
};

#elif defined(_USE_PTHREADS)

struct _Mutex {
    pthread_mutex_t mutex;  /* Posix mutex */
};

#elif defined(_WIN32)

struct _Mutex {
    HANDLE handle;          /* Win32 mutex handle */
};

#elif defined(__SYMBIAN32__)

struct _Mutex {
    Klass RMutex * mutex;  /* Symbian mutex object */
};

#else
#  error "One of the platform specific macros must be defined"
#endif

extern Mutex * MUTEX_Create P_((void));
extern void MUTEX_Delete P_((Mutex * e));
extern Bool MUTEX_Init P_((Mutex * m));
extern void MUTEX_Destroy P_((Mutex * m));
extern Bool MUTEX_TryLock P_((Mutex * m));
extern Bool MUTEX_Lock P_((Mutex * m));
extern void MUTEX_Unlock P_((Mutex * m));

#ifdef __cplusplus
} /* end of extern "C" */
#endif  /* __cplusplus */

#endif /* _SLAVA_MUTEX_H_ */

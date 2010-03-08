/*
 * $Id: s_event.h,v 1.15 2005/02/20 20:31:29 slava Exp $
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

#ifndef _SLAVA_EVENT_H_
#define _SLAVA_EVENT_H_

#include "s_def.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef enum _WaitState {
    WAIT_STATE_OK = True,
    WAIT_STATE_ERROR = False,
    WAIT_STATE_TIMEOUT = MayBe
} WaitState;

typedef enum _EventState {
    EVENT_NOT_SIGNALED = 0,
    EVENT_SIGNALED
} EventState;

typedef struct _Event Event;

/* platform specific mutex definition */
#if defined(_NT_KERNEL)

struct _Event {
    KEVENT kevent;          /* kernel event object */
    KPRIORITY pinc;         /* priority increment */
};

#elif defined(_LINUX_KERNEL)

struct _Event {
    EventState state;
    spinlock_t lock;
    unsigned long flags;
    wait_queue_head_t wq;
};

#elif defined(_USE_PTHREADS)

typedef struct _PosixEvent {
    pthread_mutex_t mut;
    pthread_cond_t cond;
} PosixEvent;

struct _Event {
    PosixEvent posix;
    EventState state;
};

#elif defined(_WIN32)

struct _Event {
    HANDLE handle;
};

#elif defined(__SYMBIAN32__)

#include "s_mutex.h"

struct _Event {
    Klass RSemaphore * semaphore;
    Mutex mutex;
    EventState state;
    int waiters;
};

#else
#  error "One of the platform specific macros must be defined"
#endif

extern Event * EVENT_Create P_((void));
extern void EVENT_Delete P_((Event * e));
extern Bool EVENT_Init P_((Event * e));
extern void EVENT_Destroy P_((Event * e));
extern Bool EVENT_Set P_((Event * e));
extern Bool EVENT_Pulse P_((Event * e));
extern Bool EVENT_Reset P_((Event * e));
extern EventState EVENT_State P_((Event * e));
extern WaitState EVENT_Wait P_((Event * e));
extern WaitState EVENT_TimeWait P_((Event * e, long ms));

#ifdef __cplusplus
} /* end of extern "C" */
#endif  /* __cplusplus */

#endif /* _SLAVA_EVENT_H_ */

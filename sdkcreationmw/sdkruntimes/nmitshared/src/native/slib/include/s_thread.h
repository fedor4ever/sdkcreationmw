/*
 * $Id: s_thread.h,v 1.18 2005/02/21 13:42:59 slava Exp $
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

#ifndef _SLAVA_THREAD_H_
#define _SLAVA_THREAD_H_

#include "s_def.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* 
 * NOTE: we are first checking for _USE_PTHREADS and then for _WIN32 so that
 * it's possible to compile against pthreads library on Win32. Not that it 
 * makes much sense...
 */
#if defined(_NT_KERNEL)
    typedef struct _ThrID* ThrID;
#elif defined(_USE_PTHREADS)
#  include <pthread.h>
   typedef pthread_t ThrID;
#elif defined(_WIN32)
   typedef HANDLE ThrID;
#elif defined(__SYMBIAN32__)
   typedef TUint ThrID;
#else
#  error "One of the platform specific macros must be defined"
#endif

typedef struct _ThrKey * ThrKey;
typedef void  (*ThrProc) P_((void * arg));
typedef void  (*ThrClean) P_((void * value));

extern void   THREAD_InitModule P_((void));
extern void   THREAD_Shutdown P_((void));

/* threads */
extern Bool   THREAD_Create P_((ThrID * tid, ThrProc proc, void * arg));
extern Bool   THREAD_Detach P_((ThrID tid));
extern Bool   THREAD_Join P_((ThrID tid));
extern ThrID  THREAD_Self P_((void));
extern void   THREAD_Exit P_((void));
extern void   THREAD_Yield P_((void));
extern void   THREAD_Sleep P_((long ms));

/* thread local storage */
extern ThrKey THREAD_CreateKey P_((ThrClean proc));
extern void   THREAD_DeleteKey P_((ThrKey key));
extern Bool   THREAD_CanSetValue P_((ThrKey key));
extern Bool   THREAD_SetValue P_((ThrKey key, void * value));
extern void * THREAD_GetValue P_((ThrKey key));

#ifdef __cplusplus
} /* end of extern "C" */
#endif  /* __cplusplus */

#endif /* _SLAVA_THREAD_H_ */

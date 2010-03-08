/*
 * $Id: s_thread.c,v 1.40 2005/02/25 02:53:39 slava Exp $
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

/*
 * Make sure that preprocessor definitions make sense
 */
#if defined(_USE_PTHREADS) 
#  undef _USE_WIN32_THREADS
   typedef pthread_key_t ThrKeyType;
#elif defined(_WIN32) 
#  define _USE_WIN32_THREADS
   typedef DWORD ThrKeyType;
#elif !defined(__SYMBIAN32__)
#  error "One of the platform specific macros must be defined"
#endif

/*
 * THREAD_InitModule() increments this count, THREAD_Shutdown decrements it
 */
STATIC int THREAD_initcount = 0;

/**
 * Used mostly for debug ASSERTs only
 */
Bool THREAD_IsInited()
{
    return BoolValue(THREAD_initcount > 0);
}

/**
 * Initialize the module
 */
void THREAD_InitModule() 
{
    if ((THREAD_initcount++) == 0) {
        MEM_InitModule();
        THREAD_PlatformInitialize();
    }
}

/**
 * Shutdown the module
 */
void THREAD_Shutdown()
{
    ASSERT(THREAD_initcount > 0);
    if (THREAD_initcount == 1) {
        THREAD_PlatformShutdown();
        MEM_Shutdown();
        THREAD_initcount = 0;
    } else {
        THREAD_initcount--;
    }
}

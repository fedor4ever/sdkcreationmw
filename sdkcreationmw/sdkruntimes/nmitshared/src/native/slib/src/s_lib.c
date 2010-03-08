/*
 * $Id: s_lib.c,v 1.14 2003/11/30 02:49:58 slava Exp $
 *
 * Copyright (C) 2001-2003 by Slava Monich
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

#include "s_lib.h"
#include "s_mem.h"

/**
 * Initializes all slib modules.
 */
void SLIB_InitModules()
{
    MEM_InitModule();
#ifndef __KERNEL__
    THREAD_InitModule();
    WKQ_InitModule();
    LOCK_InitModule();
#endif /* __KERNEL__ */
    HASH_InitModule();
    RANDOM_InitModule();
}

/**
 * Deinitializes all slib modules, prints memory usage statistics
 */
void SLIB_Shutdown()
{
    RANDOM_Shutdown();
    HASH_Shutdown();
#ifndef __KERNEL__
    LOCK_Shutdown();
    WKQ_Shutdown();
    THREAD_Shutdown();
#endif /* __KERNEL__ */
    MEM_Shutdown();
}

/**
 * This function is invoked when one of the modules has failed to initialize.
 * Never returns. Prints an error message ans calls abort()
 */
void SLIB_Abort(Str module)
{
    /* 
     * If this function is invoked, something must be badly broken. 
     * Allocate message on stack to reduce the chances that Error 
     * fail too.
     */
    Char msg[128];
    Sprintf(msg,TEXT("Cannot initialize %s module, exiting...\n"),module);
    Error(msg);
    ASSMSG1("Cannot initialize %s module",module);

    /* should panic in kernel mode? */
#ifndef __KERNEL__
#  ifdef _WIN32_WCE
    exit(-1);
#  else /* !_WIN32_WCE */
    abort();
#  endif /* !_WIN32_WCE */
#endif /* __KERNEL__ */
}

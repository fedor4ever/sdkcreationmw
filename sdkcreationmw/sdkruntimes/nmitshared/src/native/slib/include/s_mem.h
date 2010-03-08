/*
 * $Id: s_mem.h,v 1.10 2003/11/03 21:48:30 slava Exp $
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

#ifndef _SLAVA_MEM_H_
#define _SLAVA_MEM_H_

#include "s_def.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* memory allocation */

extern void   MEM_InitModule P_((void));
extern void   MEM_Shutdown P_((void));

extern void * MEM_Alloc P_((size_t size));
extern void * MEM_Calloc P_((size_t count, size_t size));
extern void * MEM_Realloc P_((void * ptr, size_t size));
extern void   MEM_Free P_((void * ptr));

/* memory allocation interface */

typedef struct _MemHook MemHook;
typedef struct _MemContext MemContext;

typedef Bool   (*MemInit) P_((MemContext * ctx, const MemHook * next));
typedef void * (*MemAlloc) P_((MemContext * ctx, size_t size));
typedef void * (*MemRealloc) P_((MemContext * ctx, void * ptr, size_t size));
typedef void   (*MemFree) P_((MemContext * ctx, void * ptr));

/*
 * Memory hooks stack on top of each other. Once registered, the hook remains 
 * there forever because there's no simple and efficient way to keep track of
 * which memory block allocated by which hook, nor there's a need for it. 
 * There's a default "hook", always the last one, that actually calls OS 
 * specific memory allocation functions. 
 *
 * Even though this mechanism allows for arbitrary number of hooks, it's not 
 * expected that there will be more than one (on top of the default one)
 *
 * Memory hooks must be registered at startup, before one piece of memory
 * is allocated by MEM_Alloc
 */
typedef struct _MemProc {
    MemInit    memInit; 
    MemAlloc   memAlloc;        /* malloc */
    MemRealloc memRealloc;      /* realloc */
    MemFree    memFree;         /* free */
} MemProc;

extern const MemHook * MEM_Hook P_((const MemProc * proc, MemContext * ctx));
extern const MemHook * MEM_NextHook P_((const MemHook * hook));

extern void * MEM_Alloc2 P_((const MemHook * hook, size_t size));
extern void * MEM_Realloc2 P_((const MemHook * hook, void * ptr, size_t size));
extern void   MEM_Free2 P_((const MemHook * hook, void * ptr));

/* debugging tricks */

#if DEBUG

extern void  MEM_DebugStat P_((int (*print) P_((const Char * format, ... ))));
extern void  MEM_DebugDump P_((int (*print) P_((const Char * format, ... ))));
extern Bool  MEM_IsValidPointer P_((void * ptr));

#ifdef _NT_KERNEL
#  define MEM_DumpStat()         MEM_DebugStat((PrintProc)DEBUG_Trace)
#  define MEM_Dump()             MEM_DebugDump((PrintProc)DEBUG_Trace)
#else  /* !_NT_KERNEL */
#  define MEM_DumpStat()         MEM_DebugStat(DEBUG_Trace)
#  define MEM_Dump()             MEM_DebugDump(DEBUG_Trace)
#endif /* !_NT_KERNEL */

#define ASSERT_VALID_POINTER(p) ASSERT(MEM_IsValidPointer(p))

#else /* !DEBUG */

#define MEM_DumpStat()           NOTHING
#define MEM_Dump()               NOTHING

#define ASSERT_VALID_POINTER(p)  NOTHING

#endif /* !DEBUG */

#define MEM_New(type)              ((type *)MEM_Alloc(sizeof(type)))
#define MEM_NewArray(type,n)       ((type *)MEM_Alloc((n)*sizeof(type)))
#define MEM_ReallocArray(p,type,n) ((type *)MEM_Realloc(p,sizeof(type)*(n)))

/*
 * NO_REALLOC means that MEM_Realloc should not be used, probably because 
 * memory reallocation is not supported by the platform.
 */
#if !defined(OS_MemRealloc) && !defined(NO_REALLOC)
#  define NO_REALLOC 1
#endif /* !OS_MemRealloc && !NO_REALLOC */

#ifdef __cplusplus
} /* end of extern "C" */
#endif  /* __cplusplus */

#endif /* _SLAVA_MEM_H_ */

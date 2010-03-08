/*
 * $Id: s_mem.c,v 1.29 2005/02/20 20:31:30 slava Exp $
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

#include "s_def.h"
#include "s_util.h"
#include "s_mutex.h"
#include "s_queue.h"
#include "s_libp.h"
#include "s_mem.h"

/*
 * This counter is incremented by MEM_Alloc and MEM_Realloc after each 
 * successful memory (re)allocation. NOTE: this counter may not be 100%
 * accurate in release build because its increment is not synchronized.
 * Which is OK since we don't need the count to be accurate in the
 * release build. It's being used by MEM_Hook which only cares whether
 * it's zero or not. In debug build the count is perfectly synchronized.
 */
STATIC I64s totalAllocations = 0;

/*==========================================================================*
 *              D E B U G
 *==========================================================================*/

/**
 * This part is only for debugging
 */

#if DEBUG

/**
 * Memory statistics.
 */
STATIC I64s bytesAllocated = 0;
STATIC I64s blocksAllocated = 0;

STATIC I64s maxBytesAllocated = 0;
STATIC I64s maxBlocksAllocated = 0;

STATIC Mutex memMutex;          /* protects data from being corrupted */
STATIC int  memInitCount = 0;   /* initialization count */

#ifdef DEBUG_MEM
STATIC Queue memQueue;                  /* all allocated memory blocks */ 
STATIC size_t memSizeBreak = (size_t)-1;  /* break on allocation size */
#endif /* DEBUG_MEM */

#define MEM_SIGNATURE  _SIGNATURE32('M','E','M',' ')
#define MEM_DEAD       _SIGNATURE32('D','E','A','D')
#define OVERHEAD       OFFSET(OSMemBlock,data)

typedef struct _OSMemHeader {
    I32u   sign;                /* signature */
    size_t size;                /* usable size */
#ifdef DEBUG_MEM
    I64s   num;                 /* unique block number */
    QEntry e;                   /* queue entry */ 
#endif /* DEBUG_MEM */
} OSMemHeader;

typedef struct _OSMemBlock {
    OSMemHeader head;           /* header */
    char data[1];               /* user data */
} OSMemBlock;

/**
 * Check if address is a valid pointer into a memory block allocated 
 * with one of our memory management routines. If so, returns pointer
 * to OSMemBlock, otherwise returns NULL.
 */
STATIC OSMemBlock * validateMemBlock(void * ptr) 
{
    if (ptr) {
        OSMemBlock * mb = CAST(ptr,OSMemBlock,data);
        if (mb->head.sign == MEM_SIGNATURE) {
            return mb;
        } else if (mb->head.sign == MEM_DEAD) {
            ASSMSG1("ERROR: referencing dead memory block at 0x%08lX",ptr); 
        } else {
            ASSMSG1("ERROR: referencing bad memory block at 0x%08lX",ptr); 
        }        
    }
    return NULL;
}

/**
 * Allocates requested amount of memory.
 */
STATIC void * MEM_DebugAlloc(size_t size) 
{
    size_t mbsize = size + OVERHEAD;
    OSMemBlock * mb = (OSMemBlock *)OS_MemAlloc(mbsize);
    if (memInitCount == 0) MEM_InitModule();
    if (mb) {
        Bool haveMutex;
        void * ptr = mb->data;
        mb->head.sign = MEM_SIGNATURE;
        mb->head.size = size;

        /* lock global data */
        haveMutex = MUTEX_Lock(&memMutex);
        ASSERT(haveMutex);

        /* update statistics */
        blocksAllocated++;
        bytesAllocated += size;
        if (maxBytesAllocated < bytesAllocated) {
            maxBytesAllocated = bytesAllocated;
        }
        if (maxBlocksAllocated < blocksAllocated) {
            maxBlocksAllocated = blocksAllocated;
        }

#ifdef DEBUG_MEM
        ASSERT(size != memSizeBreak);
        QUEUE_InsertTail(&memQueue, &mb->head.e);
        ASSERT(blocksAllocated == memQueue.size);
        mb->head.num = ++totalAllocations;
#endif /* DEBUG_MEM */

        /* unlock global data */
        if (haveMutex) MUTEX_Unlock(&memMutex);

        return ptr;
    }
    return NULL;
}

/**
 * Deallocate memory block
 */
STATIC void MEM_DebugFree(void * ptr) 
{
    ASSERT(memInitCount > 0);
    if (ptr) {
        OSMemBlock * mb = validateMemBlock(ptr);
        if (mb) {

            /* lock global data */
            Bool haveMutex = MUTEX_Lock(&memMutex);
            ASSERT(haveMutex);

            /* update statistics */
            blocksAllocated--;
            bytesAllocated -= mb->head.size;            
            ASSERT(blocksAllocated >= 0);
            ASSERT(bytesAllocated >= 0);

#ifdef DEBUG_MEM
            QUEUE_RemoveEntry(&mb->head.e);
            ASSERT(blocksAllocated == memQueue.size);
#endif /* DEBUG_MEM */

            /* unlock global data */
            if (haveMutex) MUTEX_Unlock(&memMutex);

            /* invalidate the signature and free the memory */
            mb->head.sign = MEM_DEAD;

            OS_MemFree(mb);
        }
    }
}

/**
 * Changes the size of the memory block pointed to by ptr to size bytes. 
 * The contents will be  unchanged to the minimum of the old and new sizes;
 * newly allocated memory will be uninitialized.  If ptr is NULL, the call
 * is equivalent  to MEM_Alloc(size); if size is equal to zero, the call 
 * is equivalent to MEM_Free(ptr). Unless ptr is NULL, it must have been 
 * returned by an earlier call to MEM_Alloc() or MEM_Realloc().
 */
STATIC void * MEM_DebugRealloc(void * ptr, size_t size) 
{
    if (memInitCount == 0) MEM_InitModule();
    if (!size) {
        MEM_DebugFree(ptr);
        return NULL;
    } else if (ptr) {
        OSMemBlock * mb = validateMemBlock(ptr);
        if (mb) {
            Bool haveMutex;
            OSMemBlock * newmb;
            size_t prevsize = mb->head.size;
            size_t mbsize = size + OVERHEAD;

            /* invalidate signature before reallocating the memory block */
            mb->head.sign = MEM_DEAD;

            /* lock global data */
            haveMutex = MUTEX_Lock(&memMutex);
            ASSERT(haveMutex);

#ifdef DEBUG_MEM
            ASSERT(size != memSizeBreak);
            ASSERT(blocksAllocated == memQueue.size);
            QUEUE_RemoveEntry(&mb->head.e);
            
            /* 
             * Temporary decrement blocksAllocated counter because we
             * are to release the mutex. Otherwise, another memory 
             * allocation may occur in between and we will get an
             * assertion failure because blocksAllocatedisn't equal 
             * memQueue.size
             */
            blocksAllocated--;
#endif /* DEBUG_MEM */

            /* unlock global data */
            if (haveMutex) MUTEX_Unlock(&memMutex);

#ifdef NO_REALLOC
            newmb = (OSMemBlock*)OS_MemAlloc(mbsize);
            if (newmb) {
                memcpy(newmb, mb, mb->head.size + OVERHEAD);
                OS_MemFree(mb);
            }
#else  /* NO_REALLOC */
            newmb = (OSMemBlock*)OS_MemRealloc(mb,mbsize);
#endif /* NO_REALLOC */
            if (newmb) {
                newmb->head.sign = MEM_SIGNATURE;
                newmb->head.size = size;

                /* lock global data */
                haveMutex = MUTEX_Lock(&memMutex);
                ASSERT(haveMutex);

                /* update statistics */
                if (prevsize > size) {
                    bytesAllocated -= (prevsize-size);
                } else {
                    bytesAllocated += (size-prevsize);
                    if (maxBytesAllocated < bytesAllocated) {
                        maxBytesAllocated = bytesAllocated;
                    }
                }

#ifdef DEBUG_MEM
                /* restore the blocksAllocated counter */
                blocksAllocated++;
                QUEUE_InsertTail(&memQueue, &newmb->head.e);
                ASSERT(blocksAllocated == memQueue.size);
                newmb->head.num = ++totalAllocations;
#endif /* DEBUG_MEM */

                /* unlock global data */
                if (haveMutex) MUTEX_Unlock(&memMutex);

                /* return new address to the caller */
                return newmb->data;
            } else {

                /* reallocation failed, restore signature */
                mb->head.sign = MEM_SIGNATURE;                

#ifdef DEBUG_MEM
                /* restore the blocksAllocated counter */
                haveMutex = MUTEX_Lock(&memMutex);
                blocksAllocated++;
                QUEUE_InsertTail(&memQueue, &newmb->head.e);
                ASSERT(blocksAllocated == memQueue.size);
                if (haveMutex) MUTEX_Unlock(&memMutex);
#endif /* DEBUG_MEM */
            }
            TRACE1("ERROR! failed to reallocate %lu bytes!\n",size);
            ASSMSG("realloc failed!");
        }
        return NULL;
    } else {
        return MEM_DebugAlloc(size);
    }
}

/**
 * Returns True if pointer appears to point to the beginning of a valid 
 * memory block.
 */
STATIC Bool _ValidPointer(void * ptr) 
{
    return BoolValue(validateMemBlock(ptr) != NULL);
}

/**
 * Dumps some memory statistics to standard output
 */
void MEM_DebugStat(PrintProc print) 
{
    Bool haveMutex = (memInitCount ? MUTEX_Lock(&memMutex) : False);
#ifdef DEBUG_MEM
    ASSERT(blocksAllocated == memQueue.size);
#endif /* DEBUG_MEM */

    print(TEXT("Total allocations:   ") 
          TEXT_(LONG_LONG_FORMAT) 
          TEXT_("\n"),totalAllocations);
    print(TEXT("Peak allocations:    ") 
          TEXT_(LONG_LONG_FORMAT) 
          TEXT_(" blocks\n"),maxBlocksAllocated);
    print(TEXT("                     ") 
          TEXT_(LONG_LONG_FORMAT) 
          TEXT_(" bytes\n"),maxBytesAllocated);
    print(TEXT("Currently allocated: ") 
          TEXT_(LONG_LONG_FORMAT) 
          TEXT_(" blocks\n"),blocksAllocated);
    print(TEXT("                     ") 
          TEXT_(LONG_LONG_FORMAT) 
          TEXT_(" bytes\n"),bytesAllocated);

    if (haveMutex) MUTEX_Unlock(&memMutex);
}

#ifdef DEBUG_MEM
typedef struct _MemDump {
    PrintProc print;
    int count;
} MemDump;

/**
 * Callback for _DebugMemDump().
 * Returns True to continue, False to stop examining the queue.
 */
STATIC Bool debugMemDumpCB(QEntry* e, void * ctx) 
{
    MemDump* dump = (MemDump*)ctx;
    OSMemBlock * block = QCAST(e, OSMemBlock, head.e);
    void * data = block->data;
    size_t size = block->head.size;
    I64s num = block->head.num;
    dump->count++;
    dump->print(TEXT("%d. Memory block #") 
                TEXT(LONG_LONG_FORMAT) 
                TEXT(" at 0x%08lX, %d bytes:\n"),
                dump->count, num, data, size);
    Dump(dump->print, block->data, size, 0);
    return True;
}
#endif /* DEBUG_MEM */

/**
 * Dumps all allocated memory to standard output. 
 */
void MEM_DebugDump(PrintProc print) 
{
#ifdef DEBUG_MEM
    if (memInitCount > 0) {
        MemDump dump;
        Bool haveMutex = MUTEX_Lock(&memMutex);
        ASSERT(haveMutex);
        dump.count=0;
        dump.print = print;
        QUEUE_Examine(&memQueue, debugMemDumpCB, &dump);
        if (haveMutex) MUTEX_Unlock(&memMutex);
    }
#else  /* !DEBUG_MEM */
    print(TEXT("Memory dump not available (try compiling with DEBUG_MEM)\n"));
#endif /* !DEBUG_MEM */
}

#endif /* DEBUG */

/*==========================================================================*
 *              M E M O R Y    H O O K
 *==========================================================================*/

struct _MemHook {
    MemProc proc;
    MemContext * ctx;
    const MemHook * next;
};

/**
 * Returns the next hook.
 */
const MemHook * MEM_NextHook(const MemHook * hook)
{
    return (hook ? hook->next : NULL);
}

/**
 * The following three functions call the appropriate hook functions
 */
void * MEM_Alloc2(const MemHook * hook, size_t size)
{
    while (!hook->proc.memAlloc) hook = hook->next;
    return (*(hook->proc.memAlloc))(hook->ctx, size);
}

void * MEM_Realloc2(const MemHook * hook, void * ptr, size_t size)
{
    while (!hook->proc.memRealloc) hook = hook->next;
    return (*(hook->proc.memRealloc))(hook->ctx, ptr, size);
}

void MEM_Free2(const MemHook * hook, void * ptr)
{
    while (!hook->proc.memFree) hook = hook->next;
    (*(hook->proc.memFree))(hook->ctx, ptr);
}

/*==========================================================================*
 *              D E F A U L T    H O O K
 *==========================================================================*/

STATIC void * MEM_DefaultAlloc(MemContext * ctx, size_t size) 
{
    UNREF(ctx);
#if DEBUG
    return MEM_DebugAlloc(size);
#else  /* DEBUG */
    return OS_MemAlloc(size);
#endif /* DEBUG */
}

STATIC void * MEM_DefaultRealloc(MemContext * ctx, void * ptr, size_t size) 
{
    UNREF(ctx);
#ifndef NO_REALLOC
#  if DEBUG
    return MEM_DebugRealloc(ptr, size);
#  else  /* DEBUG */
    return OS_MemRealloc(ptr, size);
#  endif /* DEBUG */
#else  /* NO_REALLOC */
    ASSMSG("MEM_DefaultRealloc");
    return OS_MemAlloc(size);
#endif /* NO_REALLOC */
}

STATIC void   MEM_DefaultMemFree(MemContext * ctx, void * ptr) 
{
    UNREF(ctx);
#if DEBUG
    MEM_DebugFree(ptr);
#else  /* DEBUG */
    OS_MemFree(ptr);
#endif /* DEBUG */
}

/*==========================================================================*
 *              I N T E R F A C E
 *==========================================================================*/

STATIC const MemHook memDefaultHook = {
    {                       /* proc */
        NULL,               /* memInit */
        MEM_DefaultAlloc,   /* memAlloc */
        MEM_DefaultRealloc, /* memRealloc */
        MEM_DefaultMemFree  /* memFree */
    },
    NULL,                   /* ctx */
    NULL                    /* next */
};

STATIC const MemHook * memHook = &memDefaultHook;

/**
 * Initialize the module. Has no effect in release build
 */
void MEM_InitModule() 
{
#if DEBUG
    if ((memInitCount++) == 0) {
#ifdef DEBUG_MEM
        /* 
         * Protection against MEM_InitModule() called after
         * MEM_Shutdown() with some memory still allocated
         */
        if (!memQueue.head.queue) {
            QUEUE_Init(&memQueue);
        }
#endif /* DEBUG_MEM */
        if (!MUTEX_Init(&memMutex)) {
            /* unrecoverable error */
            SLIB_Abort(TEXT("MEM"));
        }
    }
#endif /* DEBUG */
}

/**
 * Shutdown the module. Has no effect in release build
 */
void MEM_Shutdown() 
{
#if DEBUG
    if (memInitCount == 1) {
        MEM_DumpStat();
        MEM_Dump();
        memInitCount = 0;
        MUTEX_Destroy(&memMutex);
    } else {
        memInitCount--;
    }
#endif /* DEBUG */
}

/**
 * Allocates size bytes and returns a pointer to the allocated memory.  
 * The memory is not cleared.
 */
void * MEM_Alloc(size_t size)
{
    void * ptr = MEM_Alloc2(memHook, size);
    if (ptr) {

        /*
         * In debug build totalAllocations is being incremented by 
         * MEM_DebugAlloc and MEM_DebugRealloc under synchronization.
         * In release build, we have to do it here. Note that since
         * this operation is not synchronized in release build, this
         * counter may not be accurate. This is OK because it's not
         * being used in release build in any way, except by MEM_Hook
         * to check whether something has been allocated or not;
         * it does not need accurate count.
         */
#if !DEBUG
        totalAllocations++;
#endif /* !DEBUG */
        return ptr;
    }
    ASSMSG1("ERROR! failed to malloc %lu bytes!",size);
    return NULL;
}

/**
 * Allocates memory for the specified number of elements of the specified
 * size. The space is initialized to zeros.
 */
void * MEM_Calloc(size_t count, size_t size)
{
    size_t total = count*size;
    void * ptr = MEM_Alloc(total);
    if (ptr) memset(ptr, 0, total);
    return ptr;
}

/**
 * Changes the size of the memory block pointed to by ptr to size bytes. 
 * The contents will be  unchanged to the minimum of the old and new sizes;
 * newly allocated memory will be uninitialized.  If ptr is NULL, the call
 * is equivalent  to MEM_Alloc(size); if size is equal to zero, the call 
 * is equivalent to MEM_Free(ptr). Unless ptr is NULL, it must have been 
 * returned by an earlier call to MEM_Alloc() or MEM_Realloc().
 */
#ifndef NO_REALLOC
void * MEM_Realloc(void * ptr, size_t size) 
{
    void * newptr = MEM_Realloc2(memHook, ptr, size);
    if (newptr) {

        /*
         * In debug build totalAllocations is being incremented by 
         * MEM_DebugAlloc and MEM_DebugRealloc under synchronization.
         * In release build, we have to do it here. Note that since
         * this operation is not synchronized in release build, this
         * counter may not be accurate. This is OK because it's not
         * being used in release build in any way, except by MEM_Hook
         * to check whether something has been allocated or not;
         * it does not need accurate count.
         */
#if !DEBUG
        totalAllocations++;
#endif /* !DEBUG */

        return newptr;
    } 
    ASSERT(!size);
    return NULL;
}
#endif /* NO_REALLOC */

/**
 * Frees  the  memory  space pointed to by ptr, which must have been 
 * returned by a previous call to MEM_Alloc() or MEM_Realloc(). Otherwise, 
 * or if MEM_Free(ptr) has already been called before, undefined behaviour
 * occurs. If ptr is NULL, no operation is performed.
 */
void MEM_Free(void * ptr)
{
    MEM_Free2(memHook, ptr);
}

/**
 * Registers a hook. Memory hooks stack on top of each other. Once registered, 
 * the hook remains there forever because there's no simple and efficient way 
 * to keep track of which memory block allocated by which hook, nor there's a 
 * need for it. There's a default "hook", always the last one, that actually 
 * calls OS specific memory allocation functions. 
 *
 * Memory hooks must be registered at startup, before one piece of memory
 * is allocated by MEM_Alloc
*/
const MemHook * MEM_Hook(const MemProc * proc, MemContext * ctx) 
{
    if (!totalAllocations) {
        ASSERT(proc);
        if (proc) {
            MemHook * hook = (MemHook*)OS_MemAlloc(sizeof(MemHook));
            ASSERT(proc->memAlloc || proc->memRealloc || proc->memFree);
            if (hook) {
                memset(hook, 0, sizeof(MemHook));
                hook->proc = *proc;
                hook->ctx = ctx;
                hook->next = memHook;
                if (!proc->memInit) {
                    memHook = hook;
                    return hook;
                } else if ((*(proc->memInit))(ctx, hook->next)) {
                    memHook = hook;
                    return hook;
                }
                OS_MemFree(hook);
            }
        }
    } else {
        ASSMSG("It's too late to call MEM_Hook!");
    }
    return NULL;
}

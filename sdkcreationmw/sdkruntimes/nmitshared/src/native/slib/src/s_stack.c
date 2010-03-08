/*
 * $Id: s_stack.c,v 1.9 2003/05/21 00:11:04 slava Exp $
 *
 * Copyright (C) 2000-2003 by Oleg Levin and Slava Monich
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

#include "s_stack.h"
#include "s_mem.h"

/**
 * Allocates a new stack and sets it up.
 */
Stack * STACK_Create(int n)
{
    Stack * s = MEM_New(Stack);
    if (s) {
        if (STACK_Init(s, n)) {
            return s;
        }
        MEM_Free(s);
    }
    return NULL;
}

/**
 * Initialize the stack, allocate initial storage if necessary.
 * Returns False if memory allocation fails.
 */
Bool STACK_Init(Stack * s, int n)
{
    s->size = 0;
    s->alloc = MAX(n,0);
    if (s->alloc > 0) {
        s->stack = MEM_NewArray(SElement, s->alloc);
        if (!s->stack) {
            return False;
        }
        DEBUG_ONLY(memset(s->stack, 0, sizeof(s->stack[0])*s->alloc));
    } else {
        s->stack = NULL;
    }
    return True;
}

/**
 * Destroy the contents of the stack and deallocate the stack itself.
 */
void STACK_Delete(Stack * s)
{
    if (s) {
        STACK_Destroy(s);
        MEM_Free(s);
    }
}

/**
 * Clear the stack.
 */
void STACK_Clear(Stack * s)
{
    DEBUG_ONLY(memset(s->stack, 0, sizeof(s->stack[0])*s->size));
    s->size = 0;
}

/**
 * Clear the stack and deallocate internal buffer.
 */
void STACK_Destroy(Stack * s)
{
    STACK_Clear(s);
    s->alloc = 0;
    if (s->stack) {
        MEM_Free(s->stack);
        s->stack = NULL;
    }
}

/**
 * Removes element from the top of the stack
 * and returns this value.
 */
SElement STACK_Pop(Stack * s)
{
    if(s->size) {
        return s->stack[--s->size];
    }
    ASSMSG("STACK: underflow");
    return NULL;
}

/**
 * Looks at the element on the top of the stack
 * without removing it.
 */
SElement STACK_Peek(const Stack * s)
{
    if(s->size) {
        return s->stack[s->size-1];
    }
    ASSMSG("STACK: stack is empty");
    return NULL;
}

/**
 * Returns True if the stack contains the specified element
 */
Bool STACK_Contains(const Stack * s, SElementC e)
{
    int i;
    for (i=s->size-1; i>=0; i--) {
        if (s->stack[i] == e) {
            return True;
        }
    }
    return False;
}

/**
 * Increases the capacity of the stack, if necessary, to ensure
 * that it can hold at least the number of elements specified by
 * the minimum capacity argument. Returns False if memory allocation
 * fails.
 */
STATIC Bool STACK_EnsureCapacity(Stack * s, int min)
{
    int oldlen = s->alloc;
    if (min > oldlen) {
        SElement* newbuf;
        int newlen = (oldlen * 3)/2 + 1;
        if (newlen < min) newlen = min;
#ifdef NO_REALLOC
        newbuf = (SElement*)MEM_Alloc(sizeof(SElement) * newlen);
        if (newbuf && s->stack) {
            memcpy(newbuf, s->stack, s->alloc * sizeof(SElement));
            MEM_Free(s->stack);
        }
#else
        newbuf = (SElement*)MEM_Realloc(s->stack, sizeof(SElement) * newlen);
#endif /* NO_REALLOC */
        if (!newbuf) return False;

        /* switch to the new buffer */
        DEBUG_ONLY(memset(newbuf+s->size,0,sizeof(SElement)*(newlen-s->size)));
        s->stack = newbuf;
        s->alloc = newlen;
    }
    return True;
}

/**
 * Adds element on the top of the stack.
 * Returns False if memory allocation failed
 */
Bool STACK_Push(Stack * s, SElement e)
{
    if( STACK_EnsureCapacity(s, s->size + 1) ) {
        s->stack[s->size++] = e;
        return True;
    }
    return False;
}

/*
 * $Id: s_itra.c,v 1.1 2006/03/20 02:49:47 slava Exp $
 *
 * Copyright (C) 2000-2006 by Slava Monich
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

#include "s_mem.h"
#include "s_itrp.h"

STATIC IElement ITR_ArrayNext P_((Iterator * itr));
STATIC Bool ITR_ArrayHasNext P_((Iterator * itr));
STATIC void ITR_ArrayFree P_((Iterator * itr));

typedef struct _ArrayIterator {
    Iterator itr;           /* common part */
    const ConstPtr * array; /* the array */
    int size;               /* the size of the array */
    int nextPos;            /* the next position */
} ArrayIterator;

STATIC const Itr arrayIterator = {
    TEXT("Array"),          /* name     */
    ITR_ArrayHasNext,       /* hasNext  */
    ITR_ArrayNext,          /* next     */
    NULL,                   /* remove   */
    NULL,                   /* destroy  */
    ITR_ArrayFree           /* free     */
};

/*==========================================================================*
 *              A R R A Y    I T E R A T O R
 *==========================================================================*/

/**
 * Creates the iterator that represents the contents of the array.
 * Does not support removal of the elements (vector iterator does).
 */
Iterator * ITR_Array(const ConstPtr array[], int size)
{
    if (size > 0) {
        ArrayIterator * ai = MEM_New(ArrayIterator);
        if (ai) {
            ITR_Init(&ai->itr, &arrayIterator);
            ai->array = array;
            ai->nextPos = 0;
            ai->size = size;
            return &ai->itr;
        } else {
            return NULL;
        }
    } else {
        return ITR_Empty();
    }
}

STATIC Bool ITR_ArrayHasNext(Iterator * itr)
{
    ArrayIterator * ai = CAST(itr,ArrayIterator,itr);
    return BoolValue(ai->size > ai->nextPos);
}

STATIC IElement ITR_ArrayNext(Iterator * itr)
{
    ArrayIterator * ai = CAST(itr,ArrayIterator,itr);
    return (IElement)ai->array[ai->nextPos++];
}

STATIC void ITR_ArrayFree(Iterator * itr)
{
    ArrayIterator * ai = CAST(itr,ArrayIterator,itr);
    MEM_Free(ai);
}

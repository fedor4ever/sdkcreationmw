/*
 * $Id: s_itr.c,v 1.10 2006/03/20 02:49:47 slava Exp $
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

/* general purpose iterators */

STATIC Bool ITR_EmptyHasNext P_((Iterator * itr));
STATIC void ITR_EmptyFree P_((Iterator * itr));

STATIC const Itr emptyIterator = {
    TEXT("Empty"),          /* name     */
    ITR_EmptyHasNext,       /* hasNext  */
    NULL,                   /* next     */
    NULL,                   /* remove   */
    NULL,                   /* destroy  */
    ITR_EmptyFree           /* free     */
};

STATIC Iterator itrEmpty = {&emptyIterator, 0};

/** 
 * Initializes the common part
 */
void ITR_Init(Iterator * itr, const Itr * type)
{
    itr->type = type;
    itr->last = NULL;
    itr->flags = 0;
}

/**
 * Returns the next element from the collection
 */
IElement ITR_Next(Iterator * itr)
{
    if (itr->type->hasNext(itr)) {
        itr->flags |= ITR_ACTIVE;
        itr->flags &= ~ITR_REMOVED;
        return (itr->last = itr->type->next(itr));
    } else {
        ASSMSG("Unexpected ITR_Next call");
        return NULL;
    }
}

/**
 * Returns the last value returned by ITR_Next 
 */
IElement ITR_Last(Iterator * itr)
{
    ASSERT(itr->flags & ITR_ACTIVE);
    ASSERT(!(itr->flags & ITR_REMOVED));
    if ((itr->flags & ITR_ACTIVE) && !(itr->flags & ITR_REMOVED)) {
        return itr->last;
    } else {
        return NULL;
    }
}

/**
 * Returns True if the iterator has more elements to return.
 */
Bool ITR_HasNext(Iterator * itr)
{
    return itr->type->hasNext(itr);
}

/**
 * Removes the current value from the collection
 */
Bool ITR_Remove(Iterator * itr)
{
    ASSERT(itr->flags & ITR_ACTIVE);
    ASSERT(!(itr->flags & ITR_REMOVED));
    if ((itr->flags & ITR_ACTIVE) && !(itr->flags & ITR_REMOVED)) {
        itr->flags |= ITR_REMOVED;
        if (itr->type->remove) {
            return itr->type->remove(itr);
        }
    }
    return False;
}

/**
 * Deallocates the iterator
 */
void ITR_Delete(Iterator * itr)
{
    if (itr) {
        if (itr->type->destroy) itr->type->destroy(itr);
        itr->type->free(itr);
    }
}

/*==========================================================================*
 *              E M P T Y    I T E R A T O R
 *==========================================================================*/

/**
 * Creates the empty iterator. Never returns NULL. Does not allocate 
 * anything.
 */
Iterator * ITR_Empty()
{
    return &itrEmpty;
}

STATIC Bool ITR_EmptyHasNext(Iterator * itr)
{
    RELEASE_UNREF(itr);
    ASSERT(itr == &itrEmpty);
    return False;
}

STATIC void ITR_EmptyFree(Iterator * itr)
{
    RELEASE_UNREF(itr);
    ASSERT(itr == &itrEmpty);
}

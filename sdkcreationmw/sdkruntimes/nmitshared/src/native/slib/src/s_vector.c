/*
 * $Id: s_vector.c,v 1.45 2006/03/16 23:56:29 slava Exp $
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

#include "s_vector.h"
#include "s_itrp.h"
#include "s_mem.h"

/* Definition of the iterator */
STATIC IElement VECTOR_ItrNext P_((Iterator * itr));
STATIC Bool VECTOR_ItrHasNext P_((Iterator * itr));
STATIC Bool VECTOR_ItrRemove P_((Iterator * itr));
STATIC void VECTOR_ItrFree P_((Iterator * itr));

STATIC IElement VECTOR_ConstItrNext P_((Iterator * itr));
STATIC Bool VECTOR_ConstItrHasNext P_((Iterator * itr));
STATIC void VECTOR_ConstItrFree P_((Iterator * itr));

typedef struct _VectorIterator {
    Iterator itr;           /* common part */
    int nextPos;            /* next position */
    Vector * v;             /* the vector we are iterating through */
} VectorIterator;

typedef struct _VectorConstIterator {
    Iterator itr;           /* common part */
    int nextPos;            /* next position */
    const Vector * v;       /* the vector we are iterating through */
} VectorConstIterator;

STATIC const Itr vectorIterator = {
    TEXT("Vector"),         /* name     */
    VECTOR_ItrHasNext,      /* hasNext  */
    VECTOR_ItrNext,         /* next     */
    VECTOR_ItrRemove,       /* remove   */
    NULL,                   /* destroy  */
    VECTOR_ItrFree          /* free     */
};

STATIC const Itr vectorConstIterator = {
    TEXT("ConstVector"),    /* name     */
    VECTOR_ConstItrHasNext, /* hasNext  */
    VECTOR_ConstItrNext,    /* next     */
    NULL,                   /* remove   */
    NULL,                   /* destroy  */
    VECTOR_ConstItrFree     /* free     */
};

/**
 * Default VectorEquals function
 */
#define defaultVectorEqualsProc vectorEqualsDefault
Bool vectorEqualsDefault(VElementC e1, VElementC e2)
{
    return BoolValue(e1 == e2);
}

/**
 * VectorCompare function that compares two strings, case-sensitive
 */
int vectorCompareString(VElementC e1, VElementC e2)
{
    if (e1 != e2) {
        if (e1 && e2) {
            Str s1 = (Str)e1;
            Str s2 = (Str)e2;
            return StrCmp(s1,s2);
        } else if (e1) {
            return 1;
        } else {
            return -1;
        }
    } else {
        return 0;
    }
}

/**
 * VectorCompare function that compares two strings, case-insensitive
 */
#ifndef __KERNEL__
/* NT kernel does not provide stricmp function */
int vectorCompareStringNoCase(VElementC e1, VElementC e2)
{
    if (e1 != e2) {
        if (e1 && e2) {
            Str s1 = (Str)e1;
            Str s2 = (Str)e2;
            return StrCaseCmp(s1,s2);
        } else if (e1) {
            return 1;
        } else {
            return -1;
        }
    } else {
        return 0;
    }
}

/**
 * VectorEquals function that compares two strings, case-insensitive
 */
Bool vectorEqualsStringNoCase(VElementC e1, VElementC e2)
{
    return BoolValue(vectorCompareStringNoCase(e1, e2) == 0);
}
#endif /* __KERNEL__ */

/**
 * VectorEquals function that compares two strings, case-sensitive
 */
Bool vectorEqualsString(VElementC e1, VElementC e2)
{
    return BoolValue(vectorCompareString(e1, e2) == 0);
}

/**
 * Default VectorFree function. Does nothing.
 */
#define defaultVectorFreeProc vectorFreeNothingProc
void vectorFreeNothingProc(VElement e) 
{
    UNREF(e);
}

/**
 * VectorFree function that deallocates the value.
 */
void vectorFreeValueProc(VElement e)
{
    MEM_Free(e);
}

/**
 * Default VectorCompare function
 */
int vectorCompareDefault(VElementC e1, VElementC e2)
{
    PtrWord n1 = (PtrWord)e1;
    PtrWord n2 = (PtrWord)e2;
    if (n1 < n2) {
        return -1;
    } else if (n1 > n2) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Default VectorCompare2 function
 */
int vectorCompareDefault2(VElementC e1, VElementC e2, void * ctx)
{
    PtrWord n1 = (PtrWord)e1;
    PtrWord n2 = (PtrWord)e2;
    UNREF(ctx);
    if (n1 < n2) {
        return -1;
    } else if (n1 > n2) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * Allocates a new vector and sets it up.
 */
Vector * VECTOR_Create(int n, VectorEquals e, VectorFree f)
{
    Vector * v = MEM_New(Vector);
    if (v) {
        if (VECTOR_Init(v, n, e, f)) {
            return v;
        }
        MEM_Free(v);
    }    
    return NULL;
}

/**
 * Initializes the vector, allocates initial storage if necessary.
 * Returns False if memory allocation fails.
 */
Bool VECTOR_Init(Vector * v, int n, VectorEquals e, VectorFree f)
{
    v->size = 0;
    v->alloc = MAX(n,0);    
    if (v->alloc > 0) {
        v->data = MEM_NewArray(VElement, v->alloc);
        if (!v->data) {
            return False;
        }
        DEBUG_ONLY(memset(v->data, 0, sizeof(VElement)*v->alloc));
    } else {
        v->data = NULL;
    }
    v->equals = (e ? e : defaultVectorEqualsProc);
    v->free = (f ? f : defaultVectorFreeProc);
    return True;
}

/**
 * Destroys the contents of the vector and deallocates the vector itself.
 */
void VECTOR_Delete(Vector * v)
{
    if (v) {
        VECTOR_Destroy(v);
        MEM_Free(v);
    }
}

/**
 * Clear the vector.
 */
void VECTOR_Clear(Vector * v)
{
    /* if we have a dummy "free nothing" proc, don't go into this loop */
    if (v->free && v->free != vectorFreeNothingProc) {
        int i;
        for (i=0; i<v->size; i++) {
            v->free(v->data[i]);
        }
    }
    DEBUG_ONLY(memset(v->data, 0, sizeof(VElement)*v->size));
    v->size = 0;
}

/**
 * Clear the vector and deallocate internal buffer.
 */
void VECTOR_Destroy(Vector * v)
{
    VECTOR_Clear(v);
    v->alloc = 0;
    if (v->data) {
        MEM_Free(v->data);
        v->data = NULL;
    }
}

/**
 * Increases the capacity of the vector, if necessary, to ensure  
 * that it can hold at least the number of elements specified by 
 * the minimum capacity argument. Returns False if memory allocation
 * fails.
 */
Bool VECTOR_Alloc(Vector * v, int min)
{
    int oldlen = v->alloc;
    if (min > oldlen) {
        VElement* newbuf;
        int newlen = (oldlen * 3)/2 + 1;
        if (newlen < min) newlen = min;
#ifdef NO_REALLOC
        newbuf = (VElement*)MEM_Alloc(sizeof(VElement) * newlen);
        if (newbuf && v->data) {
            memcpy(newbuf, v->data, v->alloc * sizeof(VElement));
            MEM_Free(v->data);
        }
#else
        newbuf = (VElement*)MEM_Realloc(v->data, sizeof(VElement) * newlen);
#endif /* NO_REALLOC */
        if (!newbuf) return False;

        /* switch to the new buffer */
        DEBUG_ONLY(memset(newbuf+v->size,0,sizeof(VElement)*(newlen-v->size)));
        v->data = newbuf;
        v->alloc = newlen;
    }
    return True;
}

/**
 * Returns the size of the vector.
 */
int VECTOR_Size(const Vector * v)
{
    return v->size;
}

/**
 * Returns True if vector v contains all elements from vector v1.
 * If v1 is empty, return True (any set contains an empty set)
 */
Bool VECTOR_ContainsAll(const Vector * v, const Vector * v1)
{
    int i, n1 = v1->size;
    for (i=0; i<n1; i++) {
        if (!VECTOR_Contains(v,v1->data[i])) {
            return False;
        }
    }
    return True;
}

/**
 * Returns True if vector v contains any element from vector v1.
 * If both vectors are empty, returns True (empty set contains empty set)
 */
Bool VECTOR_ContainsAny(const Vector * v, const Vector * v1)
{
    int n1 = v1->size;
    if (v->size == 0) {
        return BoolValue(n1 == 0);
    } else {
        int i;
        for (i=0; i<n1; i++) {
            if (VECTOR_Contains(v,v1->data[i])) {
                return True;
            }
        }
        return False;
    }
}

/**
 * Returns True if two vectors are equal (contain equal elements in the 
 * same order)
 */
Bool VECTOR_Equals(const Vector * v1, const Vector * v2)
{
    if (v1->size == v2->size) {
        int i, n = v1->size;
        for (i=0; i<n; i++) {
            if (!v1->equals(v1->data[i], v2->data[i])) {

                /* equivalence relation must be symmetric */
                ASSERT(!v2->equals(v1->data[i], v2->data[i]));
                return False;
            }

            /* equivalence relation must be symmetric */
            ASSERT(v2->equals(v1->data[i], v2->data[i]));
        }
        return True;
    }
    return False;
}

/**
 * Internal variant of VECTOR_Swap(), does not check the indices.
 * It's a macro for efficiency reasons.
 */
#define VECTOR_DoSwap(_v,_i1,_i2) { \
    VElement _e = (_v)->data[_i1]; \
    (_v)->data[_i1] = (_v)->data[_i2]; \
    (_v)->data[_i2] = _e; \
}

/**
 * Randomly permutes the specified vector using the specified source of
 * randomness. All permutations occur with approximately equal likelihood.
 * If 'random' is NULL, the default source of randomness is used.
 */
void VECTOR_Shuffle(Vector * v, Random * r)
{
    int i;
    if (!r) r = RANDOM_GetRandom();
    for (i = v->size; i>1; i--) {
        int j = RANDOM_NextInt(r, i);
        VECTOR_DoSwap(v, i-1, j);
    }
}

/**
 * Sorts the vector according to the order induced by the specified 
 * comparator.
 *
 * This sort is guaranteed to be stable:  equal elements will
 * not be reordered as a result of the sort.
 *
 * Returns True if the list has changed as a result of the sort,
 * False if it was already sorted.
 */
Bool VECTOR_Sort(Vector * v, VectorCompare c)
{
    return VECTOR_SortPart(v, 0, v->size, c);
}

/**
 * Sorts the vector according to the order induced by the specified 
 * comparator.
 *
 * This sort is guaranteed to be stable:  equal elements will
 * not be reordered as a result of the sort.
 *
 * Returns True if the list has changed as a result of the sort,
 * False if it was already sorted.
 */
Bool VECTOR_Sort2(Vector * v, VectorCompare2 c, void * ctx)
{
    return VECTOR_SortPart2(v, 0, v->size, c, ctx);
}

/**
 * Sorts the specified range of the vector according to the order induced 
 * by the specified comparator.  The range to be sorted extends from index 
 * 'from', inclusive, to index 'to', exclusive.  (If from==to, the range 
 * to be sorted is empty.)
 *
 * This sort is guaranteed to be stable:  equal elements will
 * not be reordered as a result of the sort.
 *
 * Returns True if the list has changed as a result of the sort,
 * False if it was already sorted.
 */
Bool VECTOR_SortPart(Vector * v, int from, int to, VectorCompare c)
{
    int i;
    Bool changed = False;
    if (!c) c = vectorCompareDefault;

    /* 
     * just plain insertion sort for now. May come up with something more 
     * complicated in the future, for example modified mergesort from 
     * java.util.Arrays (why reinvent ther wheel?)
     */
    for (i=from+1; i<to; i++) {
        int j;
        for (j=i; j>from && (*c)(v->data[j-1], v->data[j])>0; j--) {
            VECTOR_DoSwap(v, j, j-1);
            changed = True;
        }
    }

    return changed;
}

/**
 * Sorts the specified range of the vector according to the order induced 
 * by the specified comparator.  The range to be sorted extends from index 
 * 'from', inclusive, to index 'to', exclusive.  (If from==to, the range 
 * to be sorted is empty.)
 *
 * This sort is guaranteed to be stable:  equal elements will
 * not be reordered as a result of the sort.
 *
 * Returns True if the list has changed as a result of the sort,
 * False if it was already sorted.
 */
Bool VECTOR_SortPart2(Vector * v,int from,int to,VectorCompare2 c,void * ctx)
{
    int i;
    Bool changed = False;
    if (!c) c = vectorCompareDefault2;

    /* 
     * just plain insertion sort for now. May come up with something more 
     * complicated in the future, for example modified mergesort from 
     * java.util.Arrays (why reinvent ther wheel?)
     */
    for (i=from+1; i<to; i++) {
        int j;
        for (j=i; j>from && (*c)(v->data[j-1], v->data[j], ctx)>0; j--) {
            VECTOR_DoSwap(v, j, j-1);
            changed = True;
        }
    }

    return changed;
}

/**
 * Searches the specified vector for the specified value using 
 * the binary search algorithm. The vector MUST be sorted (for example, 
 * by VECTOR_Sort() function above) prior to making this call.  If it is 
 * not sorted, the results are undefined.  If the vector contains multiple
 * elements equal to the specified key,  there is no guarantee which one 
 * will be found.
 *
 * Returns index of the search key, if it is contained in the vector;
 * otherwise, (-(INSERTION POINT) - 1). The INSERTION POINT is defined 
 * as the point at which the key would be inserted into the list: the index
 * of the first element greater than the key, or the size of the vector, if 
 * all elements in the list are less than the specified key. Note that this
 * guarantees that the return value will be >= 0 if and only if the key is 
 * found.
 */
int VECTOR_Search(const Vector * v, VElementC e, VectorCompare c)
{
    int low = 0;
    int high = v->size-1;
    if (!c) c = vectorCompareDefault;

    while (low <= high) {
        int mid = (low + high)/2;
        VElement val = v->data[mid];
        int cmp = (*c)(val, e);
        if (cmp < 0) {
            low = mid + 1;
        } else if (cmp > 0) {
            high = mid - 1;
        } else {
            /* key found */
            return mid; 
        }
    }

    /* key not found. */
    return -(low + 1);  
}

/**
 * Searches the specified vector for the specified value using 
 * the binary search algorithm. The vector MUST be sorted (for example, 
 * by VECTOR_Sort() function above) prior to making this call.  If it is 
 * not sorted, the results are undefined.  If the vector contains multiple
 * elements equal to the specified key,  there is no guarantee which one 
 * will be found.
 *
 * Returns index of the search key, if it is contained in the vector;
 * otherwise, (-(INSERTION POINT) - 1). The INSERTION POINT is defined 
 * as the point at which the key would be inserted into the list: the index
 * of the first element greater than the key, or the size of the vector, if 
 * all elements in the list are less than the specified key. Note that this
 * guarantees that the return value will be >= 0 if and only if the key is 
 * found.
 */
int VECTOR_Search2(const Vector* v, VElementC e, VectorCompare2 c, void * ctx)
{
    int low = 0;
    int high = v->size-1;
    if (!c) c = vectorCompareDefault2;

    while (low <= high) {
        int mid = (low + high)/2;
        VElement val = v->data[mid];
        int cmp = (*c)(val, e, ctx);
        if (cmp < 0) {
            low = mid + 1;
        } else if (cmp > 0) {
            high = mid - 1;
        } else {
            /* key found */
            return mid; 
        }
    }

    /* key not found. */
    return -(low + 1);  
}

/**
 * Inserts new element into the sorted vector keeping it (vector) sorted.
 * The insertion point is unpredictable if the vector is not sorted.
 * Returns the position of the new element in the vector, or -1 if the
 * insertion fails due to out of memory condition.
 */
int VECTOR_InsertInOrder(Vector* v, VElementC e, VectorCompare c)
{
    int pos = VECTOR_Search(v, e, c);
    if (pos < 0) {
        pos = -(pos+1);
    } else {

        /* 
         * Equal element already exists in this vector. Move the position
         * ahead at least by one, to reduce the amount of memory to shift
         */
        pos++;
        while (pos < v->size && (*c)(v->data[pos],e) == 0) pos++;

        /* Assert that we have either reached the end, or found an element
         * which is greater than the one we are adding. If the element that
         * we have found is less than the one being added, then the list is
         * not sorted. VECTOR_InsertInOrder should only be invoked on
         * sorted lists.
         */
        ASSERT(pos == v->size || (*c)(v->data[pos],e) > 0);
    }
    if (VECTOR_Insert(v, pos, e)) {
        return pos;
    } else {
        return -1;
    }
}

/**
 * Economical way to restore the order in a sorted list after one element
 * has been updated. Returns the new position of the updated element.
 */
int VECTOR_UpdateOrder(Vector* v, int pos, VectorCompare c)
{
    ASSERT(pos >= 0 && pos <= v->size);
    if (pos >= 0 && pos <= v->size) {
        VElement val = v->data[pos];
        int cmp, pos1;
        if (pos > 0) {
            pos1 = pos-1;
            cmp = (*c)(v->data[pos1],val);
            if (cmp > 0) {
                /* move the element down the list */
                while (pos1 > 0 && (*c)(v->data[pos1-1],val) > 0) {
                    pos1--;
                }
                VECTOR_Move(v,pos,pos1);
                return pos1;
            }
        }
        if ((pos+1) < v->size) {
            pos1 = pos+1;
            cmp = (*c)(v->data[pos1],val);
            if (cmp < 0) {
                /* move the element up towards the end of the list */
                while ((pos1+1) < v->size && (*c)(v->data[pos1+1],val) < 0) {
                    pos1++;
                }
                VECTOR_Move(v,pos,pos1);
                return pos1;
            }
        }
    }
    /* the element stays where it is */
    return pos;
}

/**
 * Adds new element to the end of the vector. Returns False if memory 
 * allocation fails.
 */
Bool VECTOR_Add(Vector * v, VElementC e)
{
    if (VECTOR_Alloc(v, v->size + 1)) {
        v->data[v->size++] = (VElement)e;
        return True;
    }
    return False;
}

/**
 * Adds new element to the end of the vector. Returns False and invokes 
 * the VectorFree routine if memory allocation fails. Note the difference
 * from VECTOR_Add which does not do anything with the element if it fails
 * to add it to the vector.
 */
Bool VECTOR_TryAdd(Vector * v, VElement e)
{
    if (VECTOR_Add(v, e)) {
        return True;
    } else {
        if (v->free) v->free(e);
        return False;
    }
}

/**
 * Adds all elements from vector v1 to the end of vector v. Returns False 
 * if memory allocation fails.
 */
Bool VECTOR_AddAll(Vector * v, const Vector * v1)
{
    if (v1->size == 0) return True;
    if (VECTOR_Alloc(v, v->size + v1->size)) {
        memmove(v->data + v->size, v1->data, sizeof(VElement)*v1->size);
        v->size += v1->size;
        return True;
    }
    return False;    
}

/**
 * Inserts new element at specified position. Returns False if memory 
 * allocation failed or index is invalid.
 */
Bool VECTOR_Insert(Vector * v, int pos, VElementC e)
{
    ASSERT(pos >= 0 && pos <= v->size);
    if (pos >= 0 && pos <= v->size && VECTOR_Alloc(v,v->size+1)) {
        if (pos < v->size) {
            size_t n = sizeof(VElement) * (v->size - pos);
            memmove(v->data + pos + 1, v->data + pos, n);
        }
        v->size++;
        v->data[pos] = (VElement)e;
        return True;
    }
    return False;
}

/**
 * Inserts new element at specified position. Returns False and invokes 
 * the VectorFree routine if memory allocation fails. Note the difference
 * from VECTOR_Insert which does not do anything with the element if it fails
 * to add new element to the vector.
 */
Bool VECTOR_TryInsert(Vector * v, int pos, VElement e)
{
    if (VECTOR_Insert(v, pos, e)) {
        return True;
    } else {
        if (v->free) v->free(e);
        return False;
    }
}

/**
 * Inserts all elements of v1 at specified position into vector v.
 */
Bool VECTOR_InsertAll(Vector * v, int pos, const Vector * v1)
{
    if (v1->size == 0) return True;

    ASSERT(pos >= 0 && pos <= v->size);
    if (pos < 0 || pos > v->size) return False;

    if (VECTOR_Alloc(v, v->size + v1->size)) {
        if (pos < v->size) {
            size_t n = sizeof(VElement) * (v->size - pos);
            memmove(v->data + pos + v1->size, v->data + pos, n);
        }
        memmove(v->data + pos, v1->data, sizeof(VElement) * v1->size);
        v->size += v1->size;
        return True;
    }
    return False;
}

/**
 * Returns element at specified position, NULL if index is out of bounds.
 */
VElement VECTOR_Get(const Vector * v, int pos)
{
    if (pos >= 0 && pos < v->size) {
        return v->data[pos];
    }
    ASSMSG2("VECTOR: Index %d is out of bounds 0..%d",pos,v->size-1);
    return NULL;
}

/**
 * Sets element at specified position. Does nothing if index is out of bounds.
 */
void VECTOR_Set(Vector * v, int pos, VElementC e)
{
    if (pos >= 0 && pos < v->size) {
        VElement elem = v->data[pos];
        if (elem != e) {
            if (v->free) v->free(elem);
            v->data[pos] = (VElement)e;
        }
        return;
    }
    ASSMSG2("VECTOR: Index %d is out of bounds 0..%d",pos,v->size-1);
}

/**
 * Makes sure that the vector does not contain more than maxsiz elements.
 * Returns True if the vector has been truncated, False if it already was
 * smaller than maxsiz
 */
Bool VECTOR_Truncate(Vector * v, int maxsiz)
{
    if (v->size > maxsiz) {
        if (v->free && v->free != vectorFreeNothingProc) {
            int i;
            for (i=maxsiz; i<v->size; i++) {
                v->free(v->data[i]);
            }
        }
        v->size = maxsiz;
        return True;
    }
    return False;
}

/**
 * Removes element at specified index from the vector. Does nothing if 
 * index is out of bounds. Returns the removed element or NULL if the
 * index is invalid. NOTE: the returned element may have been deallocated 
 * by the v->free() function, so it has to be used carefully.
 */
VElement VECTOR_Remove(Vector * v, int pos)
{
    if (pos >= 0 && pos < v->size) {
        VElement elem = v->data[pos];
        if (v->free) v->free(elem);
        if (pos < v->size-1) {
            size_t n = sizeof(VElement) * (v->size - pos - 1);
            memmove(v->data+pos, v->data + pos + 1, n);
        }
        v->size--;
        return elem;
    }
    ASSMSG2("VECTOR: Index %d is out of bounds 0..%d",pos,v->size-1);
    return NULL;
}

/**
 * Removes a collection of elements. Returns number of removed elements.
 */
int VECTOR_RemoveAll(Vector * v, const Vector * v1)
{
    int count = 0;
    if (v->size > 0) {
        int i;
        for (i=0; i<v1->size; i++) {
            int pos = VECTOR_IndexOf(v, v1->data[i]);
            if (pos >= 0) {
                VECTOR_Remove(v, pos);
                count++;
                if (v->size == 0) {
                    break;
                }
            }
        }
    }
    return count;
}

/**
 * Removes all elements from the vector except for those that belong to v1
 */
void VECTOR_RetainAll(Vector * v, const Vector * v1)
{
    if (v1->size > 0) {
        /* 
         * start from the end because removing the last element is 
         * less expensive
         */
        int i;
        for (i=v->size-1; i>=0; i++) {
            int pos = VECTOR_IndexOf(v1, v->data[i]);
            if (pos < 0) {
                VECTOR_Remove(v, i);
            }
        }
    }
}

/**
 * Fills the vector with the specified value
 */
void VECTOR_Fill(Vector * v, VElementC e)
{
    int i;
    for (i=0; i<v->size; i++) {
        v->data[i] = (VElement)e;
    }
}

/**
 * Swaps two elements
 */
void VECTOR_Swap(Vector * v, int i1, int i2)
{
    ASSERT(i1 >= 0 && i1 < v->size);
    ASSERT(i2 >= 0 && i2 < v->size);
    if (i1 != i2 && i1 >= 0 && i1 < v->size && i2 >= 0 && i2 < v->size) {
        VECTOR_DoSwap(v, i1, i2);
    }
}

/**
 * Moves element from position i1 to position i2.
 * Returns the moved element, NULL if indices are ouf of range.
 */
VElement VECTOR_Move(Vector * v, int i1, int i2)
{
    VElement elem = NULL;
    ASSERT(i1 >= 0 && i1 < v->size);
    ASSERT(i2 >= 0 && i2 < v->size);
    if (i1 >= 0 && i1 < v->size && i2 >= 0 && i2 < v->size) {
        elem = v->data[i1];
        if (i1 != i2) {
            if (i1 < i2) {
                memmove(v->data+i1, v->data+i1+1, sizeof(VElement) * (i2-i1));
            } else {
                memmove(v->data+i2+1, v->data+i2, sizeof(VElement) * (i1-i2));
            }
            v->data[i2] = elem;
        }
    }
    return elem;
}

/**
 * Deallocates unused memory.
 */
void VECTOR_Trim(Vector * v)
{
    if (v->size < v->alloc) {
        if (v->size > 0) {
            VElement* savebuf = v->data;
            size_t size = sizeof(VElement) * v->size;
#ifdef NO_REALLOC
            v->data = (VElement*)MEM_Alloc(size);
            if (v->data) {
                memcpy(v->data, savebuf, size);
                MEM_Free(v->data);
            }
#else
            v->data = (VElement*)MEM_Realloc(savebuf, size);
#endif /* NO_REALLOC */
            ASSERT(v->data);
            if (v->data) {
                v->alloc = v->size;
            } else {
                v->data = savebuf;
            }
        } else {
            MEM_Free(v->data);
            v->data = NULL;
            v->alloc = 0;
        }
    }
}

/**
 * Returns the first index of element which is "equal" to the specified 
 * element. Returns -1 if element is not found.
 */
int VECTOR_IndexOf(const Vector * v, VElementC e)
{
    int i;
    for (i=0; i<v->size; i++) {
        if (v->equals(v->data[i],e)) {
            return i;
        }
    }
    return -1;
}

/**
 * Returns the last index of element which is "equal" to the specified 
 * element. Returns -1 if element is not found.
 */
int VECTOR_LastIndexOf(const Vector * v, VElementC e)
{
    int i = v->size-1;
    do {
        if (v->equals(v->data[i],e)) {
            return i;
        }
    } while (i--);
    return -1;
}

/**
 * Creates a subvector containing the portion of this vector between the 
 * specified from index, inclusive, and to index, exclusive. Returns NULL 
 * if memory allocation fails or parameters are invalid
 */
Vector * VECTOR_SubVector(Vector * v, int from, int to)
{
    ASSERT(from < to);
    ASSERT(from >= 0 && from <= v->size);
    ASSERT(to >= 0 && to <= v->size);
    if (from<=to && from>=0 && from<=v->size && to>=0 && to<=v->size) {
        Vector * sublist = VECTOR_Create(to-from, v->equals, v->free);
        if (sublist) {
            int i;
            for (i=from; i<to; i++) {
                sublist->data[i-from] = v->data[i];
            }
            sublist->size = (to-from);
            return sublist;
        }
    }
    ASSMSG3("VECTOR: Range %d..%d is invalid (size=%s)",from,to-1,v->size);
    return NULL;
}

/**
 * Returns pointer to internal array of elements.
 */
VElement * VECTOR_GetElements(Vector * v)
{
    return v->data;
}

/**
 * Allocates an array containing all elements of the vector. Returns NULL
 * if memory allocation fails or if the vector is empty.
 */
VElement * VECTOR_ToArray(const Vector * v)
{
    if (v->size > 0) {
        VElement * array = MEM_NewArray(VElement,v->size);
        if (array) {
            memcpy(array, v->data, sizeof(VElement) * v->size);
            return array;
        }
    }
    return NULL;
}

/*==========================================================================*
 *              F U L L    I T E R A T O R
 *==========================================================================*/

/**
 * Creates the full iterator that allows removing elements from the vector
 */
Iterator * VECTOR_Iterator(Vector * v)
{
    if (VECTOR_IsEmpty(v)) {
        return ITR_Empty();
    } else {
        VectorIterator * vi = MEM_New(VectorIterator);
        if (vi) {
            ITR_Init(&vi->itr, &vectorIterator);
            vi->nextPos = 0;
            vi->v = v;
            return &vi->itr;
        } else {
            return NULL;
        }
    }
}

STATIC Bool VECTOR_ItrHasNext(Iterator * itr)
{
    VectorIterator * vi = CAST(itr,VectorIterator,itr);
    return BoolValue(VECTOR_Size(vi->v) > vi->nextPos);
}

STATIC IElement VECTOR_ItrNext(Iterator * itr)
{
    VectorIterator * vi = CAST(itr,VectorIterator,itr);
    return VECTOR_Get(vi->v, vi->nextPos++);
}

STATIC Bool VECTOR_ItrRemove(Iterator * itr)
{
    VectorIterator * vi = CAST(itr,VectorIterator,itr);
    VECTOR_Remove(vi->v, --vi->nextPos);
    return True;
}

STATIC void VECTOR_ItrFree(Iterator * itr)
{
    VectorIterator * vi = CAST(itr,VectorIterator,itr);
    MEM_Free(vi);
}

/*==========================================================================*
 *              C O N S T    I T E R A T O R
 *==========================================================================*/

/**
 * Creates the readonly iterator that does not allow removing elements from 
 * the vector
 */
Iterator * VECTOR_ConstIterator(const Vector * v)
{
    if (VECTOR_IsEmpty(v)) {
        return ITR_Empty();
    } else {
        VectorConstIterator * vi = MEM_New(VectorConstIterator);
        if (vi) {
            ITR_Init(&vi->itr, &vectorConstIterator);
            vi->nextPos = 0;
            vi->v = v;
            return &vi->itr;
        } else {
            return NULL;
        }
    }
}

STATIC Bool VECTOR_ConstItrHasNext(Iterator * itr)
{
    VectorConstIterator * vi = CAST(itr,VectorConstIterator,itr);
    return BoolValue(VECTOR_Size(vi->v) > vi->nextPos);
}

STATIC IElement VECTOR_ConstItrNext(Iterator * itr)
{
    VectorConstIterator * vi = CAST(itr,VectorConstIterator,itr);
    return VECTOR_Get(vi->v, vi->nextPos++);
}

STATIC void VECTOR_ConstItrFree(Iterator * itr)
{
    VectorConstIterator * vi = CAST(itr,VectorConstIterator,itr);
    MEM_Free(vi);
}

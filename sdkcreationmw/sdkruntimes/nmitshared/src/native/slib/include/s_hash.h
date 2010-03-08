/*
 * $Id: s_hash.h,v 1.14 2004/03/17 20:58:31 slava Exp $
 *
 * Copyright (C) 2000-2004 by Slava Monich
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

#ifndef _SLAVA_HASH_H_
#define _SLAVA_HASH_H_

#include "s_itr.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* data types */
typedef I32s  HashCode;
typedef struct _HashTable HashTable;
typedef void * HashKey;
typedef const void * HashKeyC;
typedef void * HashValue;
typedef const void * HashValueC;

/* 
 * default value returned by HASH_Get() if key is not mapped. If this 
 * happens to be a valid value, you should either do HASH_Contains() 
 * before doing each HASH_Get() or fix the nullValue field of the 
 * HashTable structure
 */
#define NULL_VALUE ((HashValue)0)

/* 
 * callback functions. 
 *
 * HashCompare - returns True if hash keys are equal, False if they are not
 * HashProc    - returns a hash code for the specified key
 * HashCB      - callback for use with HASH_Examine(). Returns True to 
 *               continue, False to stop. The value returned by HASH_Examine()
 *               is the last value returned by the callback, or True if 
 *               hashtable is empty. The callback routine is allowed to 
 *               remove the current element. Removing any other element 
 *               is not allowed and may cause a crash. 
 *
 * HashFree    - cleanup procedure invoked when a key/value pair gets 
 *               removed from the hashtable.
 */
typedef Bool (*HashCompare)P_((HashKeyC key1, HashKeyC key2));
typedef HashCode (*HashProc)P_((HashKeyC key));
typedef void (*HashFree)P_((HashKey key, HashValue value));
typedef Bool (*HashCB)P_((HashKey key, HashValue value, void * ctx));

/*
 * A few simple callback functions.
 *
 * hashFreeNothingProc - HashFree callback that doesn't do anything
 * hashFreeKeyProc - HashFree callback that deallocates the key
 * hashFreeValueProc - HashFree callback that deallocates the value
 * hashFreeKeyValueProc - HashFree callback that deallocates key and value
 *
 * hashCompareStringKey - HashCompare callback, assumes keys are ASCIIZ strings
 *
 * stringHashProc - returns hash code for a string key
 */
extern void hashFreeNothingProc P_((HashKey key, HashValue value));
extern void hashFreeKeyProc P_((HashKey key, HashValue value));
extern void hashFreeValueProc P_((HashKey key, HashValue value));
extern void hashFreeKeyValueProc P_((HashKey key, HashValue value));
extern Bool hashDefaultCompare P_((HashKeyC key1, HashKeyC key2));
extern Bool hashCompareStringKey P_((HashKeyC key1, HashKeyC key2));
extern Bool hashCaseCompareStringKey P_((HashKeyC key1, HashKeyC key2));
extern HashCode hashDefaultHashProc P_((HashKeyC key));
extern HashCode stringHashProc P_((HashKeyC key));
extern HashCode stringCaseHashProc P_((HashKeyC key));

/*
 * the hashtable itself. 
 */
typedef struct _HashBucket HashBucket;
struct _HashTable {
    long  count;                /* number of key-value pairs in the table */
    int   primeIndex;           /* current prime index */
    int   loadFactor;           /* the load factor * 100 */
    HashValue    nullValue;     /* the NULL value */
    HashProc     hasher;        /* hash function */
    HashCompare  equals;        /* compare function */
    HashFree     free;          /* cleanup function */
    HashBucket** buckets;       /* hashPrimes[primeIndex] slots */
};

typedef struct _HashEntry {
    HashKey   key;
    HashValue value;
} HashEntry;

/* initialize/deinitialize global data */
extern void HASH_InitModule P_((void));
extern void HASH_Shutdown P_((void));

/* operation on hash tables */
extern HashTable * HASH_Create P_((long size, HashCompare c, 
                                   HashProc h, HashFree f));

extern Bool HASH_Init P_((HashTable * ht, long size, HashCompare c, 
                          HashProc h, HashFree f));
extern void HASH_Reinit P_((HashTable * ht, HashCompare c, 
                           HashProc h, HashFree f));

extern void HASH_Destroy  P_((HashTable * ht));
extern void HASH_Delete   P_((HashTable * ht));
extern void HASH_Clear    P_((HashTable * ht));
extern long HASH_Size     P_((const HashTable * ht));
extern void HASH_Rehash   P_((HashTable * ht, long size));
extern Bool HASH_Put      P_((HashTable * ht, HashKey key, HashValue val));
extern HashValue HASH_Get P_((const HashTable * ht, HashKeyC key));
extern Bool HASH_Contains P_((const HashTable * ht, HashKeyC key));
extern Bool HASH_Remove   P_((HashTable * ht, HashKeyC key));
extern Bool HASH_Examine  P_((HashTable * ht, HashCB cb, void * ctx));

/* iterators */
extern Iterator * HASH_Keys    P_((HashTable * ht)); /* returns HashKey */
extern Iterator * HASH_Values  P_((HashTable * ht)); /* returns HashValue */
extern Iterator * HASH_Entries P_((HashTable * ht)); /* returns HashEntry */

/* macros */
#define HASH_IsEmpty(_ht) (HASH_Size(_ht) == 0)
#define HASH_ContainsKey(_ht,_key) HASH_Contains(_ht,_key)

#ifdef __cplusplus
} /* end of extern "C" */
#endif  /* __cplusplus */

#endif /* _SLAVA_HASH_H_ */

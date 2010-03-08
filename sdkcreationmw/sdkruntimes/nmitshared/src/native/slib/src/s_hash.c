/*
 * $Id: s_hash.c,v 1.38 2005/07/10 21:44:51 slava Exp $
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

#include "s_hash.h"
#include "s_util.h"
#include "s_mem.h"
#include "s_itrp.h"
#include "s_libp.h"

/* 
 * Linux kernel include files define 'cirrent' as a macro. Those guys 
 * never worried about namespace pollution...
 */

#undef current

/*
 * _HASH_POOL disables the use of the hash entry pool. If it's defined
 * to zero, all HashBucket structures will be dynamically allocated as
 * needed. By default, hash pool is on.
 */
#ifndef _HASH_POOL
#  define _HASH_POOL 1
#endif /* !_HASH_POOL */

/*
 * Disable use of thread local storage for NT kernel mode and Windows CE.
 * As far as I know, there's no support for thread local storage for NT 
 * kernel mode drivers. Windows CE does support thread local storage, 
 * but I had a weird TLS related problem with WinCE drivers (thread local
 * value suddenly appears even though I didn't put it there).
 */
#if _HASH_POOL
#  ifdef _HASH_THREAD_LOCAL_POOL
#    include "s_mutex.h"
#  else /* !defined(_HASH_THREAD_LOCAL_POOL) */
#    if defined(__KERNEL__) || defined(_WIN32_WCE)
#      include "s_mutex.h"
#      define _HASH_THREAD_LOCAL_POOL 0
#    else /* __KERNEL__ || _WIN32_WCE */
#      include "s_thread.h"
#      define _HASH_THREAD_LOCAL_POOL 1
#    endif /* __KERNEL__ || _WIN32_WCE */
#  endif /* _HASH_THREAD_LOCAL_POOL */
#endif /* _HASH_POOL */

/* (some) prime numbers */
STATIC int hashPrimes[] = { 
    11, 37, 71, 127, 179, 257, 359, 491, 661, 887, 1181, 1553,
    2053, 2683, 3517, 4591, 6007, 7817, 10193, 13291, 17291,
    22481, 29251, 38053, 49499, 64373, 83701, 108863, 141511,
    184003, 239231, 310997, 404321, 525649, 683377, 888397,
    1154947, 1501447, 1951949, 2537501, 3298807, 4288439,
    5575001, 7247533, 9421793, 12248389, 15922903, 20699753,
    26909713, 34982639, 45477503, 59120749, 76856959, 99914123,
    129888349, 168854831, 219511301, 285364721, 370974151,
    482266423, 626946367, 815030309, 1059539417, 1377401287,
    1790621681, 2147483647 
};

#define MAX_PRIME_INDEX (COUNT(hashPrimes)-1)
#define MAX_PRIME       (hashPrimes[MAX_PRIME_INDEX])

#define DEFAULT_LOAD_FACTOR 75

#define defaultHashFree hashFreeNothingProc

/* private data structures */
struct _HashBucket {
    HashKey   key;
    HashValue value;
    HashBucket *next;
};

/*
 * single-linked list of free hash buckets. It won't contain more than 
 * MAX_FREE_BUCKETS elements.
 */
#define MAX_FREE_BUCKETS 128
typedef struct _HashBuckets {
    int count;              /* number of elements in the list */
    HashBucket * buckets;   /* single-linked list of free hash buckets */
} HashBuckets;

typedef struct _HashModule {
    int initcount;          /* positive if module has been initialized */
#if _HASH_POOL
#  if !_HASH_THREAD_LOCAL_POOL
    Mutex lock;             /* spinlock */
    HashBuckets pool;       /* pool of free HashBacket structures */
#  else /* _HASH_THREAD_LOCAL_POOL */
    ThrKey key;             /* key in thread local storage */
#  endif /* _HASH_THREAD_LOCAL_POOL */
#endif /* _HASH_POOL */
} HashModule;

STATIC HashModule HASH = {0};

/* Iterators */
STATIC IElement HASH_ItrNextKey P_((Iterator * itr));
STATIC IElement HASH_ItrNextValue P_((Iterator * itr));
STATIC IElement HASH_ItrNextEntry P_((Iterator * itr));
STATIC Bool HASH_ItrHasNext P_((Iterator * itr));
STATIC Bool HASH_ItrRemove P_((Iterator * itr));
STATIC void HASH_ItrFree P_((Iterator * itr));
STATIC void HASH_ItrFreeEntry P_((Iterator * itr));

typedef struct _HashIterator {
    Iterator itr;           /* common part */
    HashTable * ht;         /* the hash table we are iterating through */
    HashBucket * current;   /* the current hash bucket */
    HashBucket * next;      /* the next hash bucket */
    int currentPos;         /* position of the current bucket in the array */
    int nextPos;            /* position of the next bucket in the array */
} HashIterator;

typedef struct _HashEntryIterator {
    HashIterator base;      /* common part for all hashs iterators */
    HashEntry entry;        /* the entry to return */
} HashEntryIterator;

STATIC const Itr hashKeyIterator = {
    TEXT("HashKey"),        /* name     */
    HASH_ItrHasNext,        /* hasNext  */
    HASH_ItrNextKey,        /* next     */
    HASH_ItrRemove,         /* remove   */
    NULL,                   /* destroy  */
    HASH_ItrFree            /* free     */
};

STATIC const Itr hashValueIterator = {
    TEXT("Hash"),           /* name     */
    HASH_ItrHasNext,        /* hasNext  */
    HASH_ItrNextValue,      /* next     */
    HASH_ItrRemove,         /* remove   */
    NULL,                   /* destroy  */
    HASH_ItrFree            /* free     */
};

STATIC const Itr hashEntryIterator = {
    TEXT("HashEntry"),      /* name     */
    HASH_ItrHasNext,        /* hasNext  */
    HASH_ItrNextEntry,      /* next     */
    HASH_ItrRemove,         /* remove   */
    NULL,                   /* destroy  */
    HASH_ItrFreeEntry       /* free     */
};

/* macros */
#define _NumBuckets(t) (hashPrimes[(t)->primeIndex])
#define _NeedRehash(t) ((t)->count >= (long)((t)->loadFactor*_NumBuckets(t)/100))

/**
 * Default compare function.
 */
Bool hashDefaultCompare(HashKeyC key1, HashKeyC key2)
{
    return BoolValue((long)key1 == (long)key2);
}

/**
 * Default hash function.
 */
HashCode hashDefaultHashProc(HashKeyC key)
{
    return ((HashCode)(PtrWord)key);
}

/**
 * HashFree callback that doesn't do anything
 */
void hashFreeNothingProc(HashKey key, HashValue value)
{
    UNREF(key);
    UNREF(value);
}

/**
 * HashFree callback that deallocates the key
 */
void hashFreeKeyProc(HashKey key, HashValue value)
{
    UNREF(value);
    MEM_Free(key);
}

/**
 * HashFree callback that deallocates the value
 */
void hashFreeValueProc(HashKey key, HashValue value)
{
    UNREF(key);
    MEM_Free(value);
}

/**
 * HashFree callback that deallocates both key and value
 */
void hashFreeKeyValueProc(HashKey key, HashValue value)
{
    MEM_Free(key);
    MEM_Free(value);
}

/**
 * HashCompare callback, assumes keys are ASCIIZ strings.
 * The comparison is case sensitive
 */
Bool hashCompareStringKey(HashKeyC key1, HashKeyC key2)
{
    if (key1 != key2) {
        Str s1 = (Str)key1;
        Str s2 = (Str)key2;
        return BoolValue(StrCmp(s1,s2) == 0);
    } else {
        return True;
    }
}

/**
 * HashCompare callback, assumes keys are ASCIIZ strings.
 * The comparison is case insensitive
 */
#ifndef _NT_KERNEL
/* NT kernel does not provide stricmp function */
Bool hashCaseCompareStringKey(HashKeyC key1, HashKeyC key2)
{
    if (key1 != key2) {
        Str s1 = (Str)key1;
        Str s2 = (Str)key2;
        return BoolValue(StrCaseCmp(s1,s2) == 0);
    } else {
        return True;
    }
}
#endif /* !_NT_KERNEL */

/**
 * case-sensitive hash function for a string key. 
 */
HashCode stringHashProc(HashKeyC key)
{
    return STRING_HashCode((Str)key);
}

/**
 * case-insensitive hash function for a string key. 
 */
HashCode stringCaseHashProc(HashKeyC key)
{
    return STRING_HashCodeNoCase((Str)key);
}

/**
 * Deallocates pooled hash buckets. Deos not deallocate pool itself
 */
STATIC void HASH_DrainPool(HashBuckets * pool)
{
    HashBucket * b = pool->buckets;
    while (b) {
        HashBucket * next = b->next;
        MEM_Free(b);
        pool->count--;
        b = next;
    }
    ASSERT(pool->count == 0);
}

/**
 * Stores hash bucket in the pool. Returns True if the hash bucket has been
 * stored, False if that would exceed the pool limit.
 */
STATIC Bool HASH_StoreInPool(HashBuckets * pool, HashBucket * b)
{
    if (pool->count < MAX_FREE_BUCKETS) {
        b->next = pool->buckets;
        pool->buckets = b;
        pool->count++;
        return True;
    }
    return False;
}

/**
 * Gets a hash bucket from the pool. Returns NULL if the pool is empty
 */
STATIC HashBucket * HASH_GetFromPool(HashBuckets * pool)
{
    if (pool->count > 0) {
        HashBucket * b = pool->buckets;
        if (b) {
            pool->count--;
            pool->buckets = b->next;
            b->next = NULL;
            return b;
        }
    }
    return NULL;
}

/**
 * Returns HashBuckets for the calling thread.
 */
#if _HASH_THREAD_LOCAL_POOL
STATIC HashBuckets * HASH_GetHashBucketPool(void) 
{
    ASSERT(HASH.initcount > 0);
    if (HASH.initcount > 0) {
        HashBuckets * pool = (HashBuckets*)THREAD_GetValue(HASH.key);
        if (pool) {
            return pool;
        } else if (THREAD_CanSetValue(HASH.key)) {
            pool = MEM_New(HashBuckets);
            if (pool) {
                memset(pool,0,sizeof(*pool));
                if (THREAD_SetValue(HASH.key,pool)) {
                    return pool;
                }
                MEM_Free(pool);
            }
        }
    }
    return NULL;
}
#endif /* _HASH_THREAD_LOCAL_POOL */

/**
 * Allocates a new hash bucket or gets one from the list of free
 * buckets. Returns NULL if memory allocation fails.
 */
STATIC HashBucket * HASH_GetHashBucket(void) 
{
#if _HASH_POOL
    HashBucket * b = NULL;
#  if !_HASH_THREAD_LOCAL_POOL
    ASSERT(HASH.initcount > 0);
    if (HASH.initcount > 0) {
        if (HASH.pool.count > 0) {
            if (MUTEX_Lock(&HASH.lock)) {
                b = HASH_GetFromPool(&HASH.pool);
                MUTEX_Unlock(&HASH.lock);
            }
        }
    }
#  else /* !_HASH_THREAD_LOCAL_POOL */
    HashBuckets * pool = HASH_GetHashBucketPool();
    if (pool) b = HASH_GetFromPool(pool);
#  endif /* !_HASH_THREAD_LOCAL_POOL */
    if (!b) {
        b = MEM_New(HashBucket);
    }
    return b;
#else /* !_HASH_POOL */
    ASSERT(HASH.initcount > 0);
    return MEM_New(HashBucket);
#endif /* !_HASH_POOL */
}

/**
 * Deallocates a hash bucket or puts it to the list of free hash buckets.
 */
STATIC void HASH_FreeHashBucket(HashBucket * b) 
{
    if (b) {
#if _HASH_POOL
#  if !_HASH_THREAD_LOCAL_POOL
        ASSERT(HASH.initcount > 0);
        if (HASH.initcount > 0) {
            if (HASH.pool.count < MAX_FREE_BUCKETS) {
                Bool stored = False;
                if (MUTEX_Lock(&HASH.lock)) {
                    stored = HASH_StoreInPool(&HASH.pool, b);
                    MUTEX_Unlock(&HASH.lock);
                }
                if (stored) {
                    return;
                }
            }
        }
#  else /* _HASH_THREAD_LOCAL_POOL */
        HashBuckets * pool = HASH_GetHashBucketPool();
        if (pool) {
            if (HASH_StoreInPool(pool, b)) {
                return;
            }
        }
#  endif /* _HASH_THREAD_LOCAL_POOL */
#endif /* _HASH_POOL */
        MEM_Free(b);
    }
}

/**
 * Selects prime index appropriate for the specified hash table size.
 */
STATIC int HASH_SelectPrimeIndex(long size, int loadFactor) 
{
    if (size < MAX_PRIME) {
        int i;
        for (i=0; i<COUNT(hashPrimes); i++) {
            if (((long)(loadFactor*hashPrimes[i]/100)) > size) {
                return i;
            }
        }
    }
    return MAX_PRIME_INDEX;
}

/**
 * Returns hash code for the specified key. Checks out-of-bounds  
 * conditions.
 */
STATIC HashCode HASH_GetHashCode(const HashTable * ht, HashKeyC key) 
{
    HashCode n = _NumBuckets(ht);
    HashCode hashCode = ht->hasher(key);
    if (hashCode < 0) hashCode = -hashCode;
    return (hashCode % n);
}

#if _HASH_THREAD_LOCAL_POOL
/* thread local storage destructor */
STATIC void HASH_Cleanup(void * param) 
{
    HashBuckets * pool = (HashBuckets*)param;
    if (pool) {
        HASH_DrainPool(pool);
        MEM_Free(pool);
    }
}
#endif /* _HASH_THREAD_LOCAL_POOL */

/**
 * Initialize the module
 */
void HASH_InitModule()
{
    if ((HASH.initcount++) == 0) {
#if _HASH_POOL
#  if !_HASH_THREAD_LOCAL_POOL
        if (MUTEX_Init(&HASH.lock)) {
            return;
        }
#  else  /* _HASH_THREAD_LOCAL_POOL */
        THREAD_InitModule();
        HASH.key = THREAD_CreateKey(HASH_Cleanup);
        if (HASH.key) {
            return;
        }
        THREAD_Shutdown();
#  endif /* _HASH_THREAD_LOCAL_POOL */

        /* unrecoverable error */
        SLIB_Abort(TEXT("HASH"));
#endif /* _HASH_POOL */
    }
}

/**
 * Deallocates all resources used internally by the module.
 */
void HASH_Shutdown() 
{
    ASSERT(HASH.initcount > 0);
    if ((--HASH.initcount) == 0) {
#if _HASH_POOL
#  if !_HASH_THREAD_LOCAL_POOL
        HASH_DrainPool(&HASH.pool);
        MUTEX_Destroy(&HASH.lock);
#  else  /* _HASH_THREAD_LOCAL_POOL */
        /* 
         * When HASH_Shutdown is called, only one user thread must be running
         * (i.e. the one calling this function. Therefore, we only need to 
         * worry about deallocating thread local data for the current thread.
         * That simplifies the cleanup procedure.
         */
        HASH_Cleanup(THREAD_GetValue(HASH.key));
        THREAD_DeleteKey(HASH.key);
        HASH.key = (ThrKey)NULL;
        THREAD_Shutdown();
#  endif /* _HASH_THREAD_LOCAL_POOL */
#endif /* _HASH_POOL */
    }
}

/**
 * Creates new hash table.
 * Returns NULL if memory allocation fails.
 */
HashTable * HASH_Create(long size, HashCompare c, HashProc h, HashFree f) 
{
    HashTable * ht = MEM_New(HashTable);
    if (ht) {
        if (!HASH_Init(ht, size, c, h, f)) {
            MEM_Free(ht);
            ht = NULL;
        }
    }
    return ht;
}

/**
 * Initialize hashtable. Returns False if it fails to allocate memory
 * or if hashtable pointer is NULL.
 */
Bool HASH_Init(HashTable * ht,long n,HashCompare c,HashProc h,HashFree f) 
{
    ASSERT(HASH.initcount > 0);
    if (HASH.initcount == 0) HASH_InitModule();
    if (ht) {
        size_t nb;
        ht->count = 0;
        ht->loadFactor = DEFAULT_LOAD_FACTOR;
        ht->primeIndex = HASH_SelectPrimeIndex(n, ht->loadFactor);
        ht->nullValue = NULL_VALUE;
        ht->hasher = (h ? h : hashDefaultHashProc);
        ht->equals = (c ? c : hashDefaultCompare);
        ht->free = (f ? f : defaultHashFree);

        /* allocate slots for hash buckets */
        nb = _NumBuckets(ht);
        ht->buckets = MEM_NewArray(HashBucket*,nb);
        if (ht->buckets) {
            memset(ht->buckets,0,sizeof(ht->buckets[0])*nb);
            return True;
        }
    }
    return False;
}

/**
 * Reinitializes the hashtable using the specified callbacks. The current
 * contents of the hashtable is destroyed. All other parameters of the 
 * hashtable (load factor, NULL value, current prime index) remain untouched.
 */
void HASH_Reinit(HashTable * ht, HashCompare c, HashProc h, HashFree f)
{
    HASH_Clear(ht);
    ht->hasher = (h ? h : hashDefaultHashProc);
    ht->equals = (c ? c : hashDefaultCompare);
    ht->free = (f ? f : defaultHashFree);
}

/**
 * Destroy contents of the hash table. After this, the has table becomes
 * unusable.
 */
void HASH_Destroy(HashTable * ht) 
{
    if (ht->buckets) {
        HASH_Clear(ht);
        MEM_Free(ht->buckets);
        ht->buckets = NULL;
    }
}

/**
 * Delete the hash table. 
 */
void HASH_Delete(HashTable * ht) 
{
    if (ht) {
        HASH_Destroy(ht);
        MEM_Free(ht);
    }
}

/**
 * Removes everything from the hash table. 
 */
void HASH_Clear(HashTable * ht) 
{
    int i;
    int n = _NumBuckets(ht);
    for (i=0; i<n && ht->count > 0; i++) {
        HashBucket * b = ht->buckets[i];
        if (b) {
            while (b) {
                HashBucket * next = b->next;
                ht->free(b->key, b->value);
                HASH_FreeHashBucket(b);
                b = next;
                ht->count--;
            }
            ht->buckets[i] = NULL;
        }
    }
    ASSERT(ht->count == 0);
}

/**
 * returns number of values stored in the hashtable.
 */
long HASH_Size(const HashTable * ht) 
{
    return ht->count;
}

/**
 * Rehash the hashtable so the it can hold at least the suggested 
 * number of entries
 */
void HASH_Rehash(HashTable * ht, long size) 
{
    int pi = HASH_SelectPrimeIndex(MAX(size,ht->count), ht->loadFactor);

    /* only rehash the hash table if we are going to either increase 
     * the number of hash buckets or significantly reduce it */
    if (pi > ht->primeIndex || (pi+1) < ht->primeIndex) {
        int n1 = hashPrimes[pi];  /* new size of the array */
        HashBucket ** buckets = MEM_NewArray(HashBucket*,n1);
        if (buckets) {
            int i;
            int n = _NumBuckets(ht);
            HashBucket ** old = ht->buckets;

            /* switch to the new array */
            memset(buckets,0,sizeof(buckets[0])*n1);
            ht->primeIndex = pi;
            ht->buckets = buckets;
            
            /* move hash buckets from the old array to the new one */
            for (i=0; i<n; i++) {
                HashBucket * b = old[i];
                if (b) {
                    while (b) {
                        HashBucket * next = b->next;
                        HashCode hashCode = HASH_GetHashCode(ht, b->key);
                        b->next = ht->buckets[hashCode];
                        ht->buckets[hashCode] = b;
                        b = next;
                    }
                    old[i] = NULL;
                }
            }
            
            /* deallocate the old one */
            MEM_Free(old);
        }
    }
}

/**
 * Puts a key/value pair into the hash table. The value replaces the 
 * previous value associated with the same hash key. Also, the new 
 * key replaces the old one if such hash entry already exists, i.e. 
 * this function can be used for replacing the key, the value or both.
 * If the new value if the same as the old one, the value is not 
 * deallocated. 
 *
 * Returns True on success, False if memory allocation fails.
 */
Bool HASH_Put(HashTable * ht, HashKey key, HashValue value) 
{    
    /* try to replace existing value */
    HashCode hashCode = HASH_GetHashCode(ht, key);
    HashBucket * b = ht->buckets[hashCode];
    while (b) {
        if (ht->equals(key,b->key)) {
            if (b->value != value) {
                ht->free(b->key, b->value);
            }
            b->key = key;
            b->value = value;
            return True;
        }
        b = b->next;
    }

    /* the value does not exist, create a new one */
    b = HASH_GetHashBucket();
    if (b) {
        b->key = key;
        b->value = value;
        b->next = ht->buckets[hashCode];
        ht->buckets[hashCode] = b;
        ht->count++;
        if (_NeedRehash(ht)) {
            HASH_Rehash(ht,ht->count);
        }
        return True;
    }

    /* failed to allocate new entry */
    return False;
}

/**
 * Finds hash bucket for the specified key. 
 */
STATIC const HashBucket * HASH_Lookup(const HashTable * ht, HashKeyC key) 
{
    if (ht->count > 0) {
        HashCode hashCode = HASH_GetHashCode(ht, key);
        HashBucket * b = ht->buckets[hashCode];
        while (b) {
            
            /* ASSERT that the keys never mutate. If they do, that breaks 
             * the integrity of the hash table. In most cases it happens
             * because the key has been deallocated. */
            ASSERT(HASH_GetHashCode(ht,b->key) == hashCode);

            if (ht->equals(key,b->key)) {
                return b;
            }
            b = b->next;
        }
    }
    return NULL;
}

/**
 * Gets a value from the hash table for the specified key. 
 * If such key does not exist in this hashtable, returns nullValue
 */
HashValue HASH_Get(const HashTable * ht, HashKeyC key) 
{
    const HashBucket * b = HASH_Lookup(ht, key);
    return (b ? b->value : ht->nullValue);
}

/**
 * Returns True if the hashtable contains the specified key
 */
Bool HASH_Contains(const HashTable * ht, HashKeyC key) 
{
    const HashBucket * b = HASH_Lookup(ht, key);
    return BoolValue(b != NULL);
}

/**
 * Removes key/value pair from the hashtable.
 * Returns True if key was found, and value was removed from hashtable.
 */
Bool HASH_Remove(HashTable * ht, HashKeyC key) 
{
    HashCode hashCode = HASH_GetHashCode(ht, key);
    HashBucket * b = ht->buckets[hashCode];
    HashBucket * prev = NULL;
    while (b) {
        if (ht->equals(key,b->key)) {
            ht->free(b->key, b->value);
            if (prev) {
                prev->next = b->next;
            } else {
                ht->buckets[hashCode] = b->next;
            }

            ht->count--;
            ASSERT(ht->count >= 0);
            HASH_FreeHashBucket(b);
            return True;
        }
        prev = b;
        b = b->next;
    }
    return False;
}

/*
 * examine the hash table, calling callback function on each entry. Stops
 * when either all entries get examined, or callback function returns
 * False. Returns the last value returned by the callback function, or
 * True if hashtable is empty. The callback routine is allowed to remove
 * the current element. Removing any other element from the callback routine
 * is not allowed and may cause a crash.
 */
Bool HASH_Examine(HashTable * ht, HashCB cb, void* ctx) 
{
    if (ht && ht->count > 0) {
        int i;
        int n = _NumBuckets(ht);
        for (i=0; i<n; i++) {
            HashBucket * b = ht->buckets[i];
            while (b) {
                /* save next pointer because current element may be removed */
                HashBucket * next = b->next;
                if (!(*cb)(b->key, b->value, ctx)) return False;
                b = next;
            }
        }
    }
    return True;
}

/* 
 * Initializes HashIterator
 */
STATIC void HASH_ItrInit(HashIterator * hi, HashTable * ht, const Itr * type)
{
    int n = _NumBuckets(ht);
    ITR_Init(&hi->itr, type);
    hi->ht = ht;
    hi->current = NULL;
    hi->next = NULL;
    hi->currentPos = -1;
    hi->nextPos = -1;
    while (++hi->nextPos < n) {
        hi->next = hi->ht->buckets[hi->nextPos];
        if (hi->next) {
            break;
        }
    }
}

/* 
 * Creates an iterator that returns HashKey 
 */
Iterator * HASH_Keys(HashTable * ht)
{
    if (HASH_IsEmpty(ht)) {
        return ITR_Empty();
    } else {
        HashIterator * hi = MEM_New(HashIterator);
        if (hi) {
            HASH_ItrInit(hi, ht, &hashKeyIterator);
            return &hi->itr;
        } else {
            return NULL;
        }
    }
}

/* 
 * Creates an iterator that returns HashValue 
 */
Iterator * HASH_Values(HashTable * ht)
{
    if (HASH_IsEmpty(ht)) {
        return ITR_Empty();
    } else {
        HashIterator * hi = MEM_New(HashIterator);
        if (hi) {
            HASH_ItrInit(hi, ht, &hashValueIterator);
            return &hi->itr;
        } else {
            return NULL;
        }
    }
}

/* 
 * Creates an iterator that returns pointers to HashEntry 
 */
Iterator * HASH_Entries(HashTable * ht)
{
    if (HASH_IsEmpty(ht)) {
        return ITR_Empty();
    } else {
        HashEntryIterator * hi = MEM_New(HashEntryIterator);
        if (hi) {
            HASH_ItrInit(&hi->base, ht, &hashEntryIterator);
            memset(&hi->entry, 0, sizeof(hi->entry));
            return &hi->base.itr;
        } else {
            return NULL;
        }
    }
}

/*==========================================================================*
 *              I T E R A T O R S
 *==========================================================================*/

/**
 * Advances the hash iterator to the next position
 */
STATIC void HASH_ItrAdvance(HashIterator * hi)
{
    hi->current = hi->next;
    hi->currentPos = hi->nextPos;
    if (hi->next) {
        if (hi->next->next) {
            hi->next = hi->next->next;
        } else {
            int n = _NumBuckets(hi->ht);
            hi->next = NULL;
            while (++hi->nextPos < n) {
                hi->next = hi->ht->buckets[hi->nextPos];
                if (hi->next) {
                    break;
                }
            }
        }
    }
}

STATIC IElement HASH_ItrNextKey(Iterator * itr)
{
    HashIterator * hi = CAST(itr,HashIterator,itr);
    HashKey key = hi->next->key;
    HASH_ItrAdvance(hi);
    return key;
}

STATIC IElement HASH_ItrNextValue(Iterator * itr)
{
    HashIterator * hi = CAST(itr,HashIterator,itr);
    HashValue value = hi->next->value;
    HASH_ItrAdvance(hi);
    return value;
}

STATIC IElement HASH_ItrNextEntry(Iterator * itr)
{
    HashEntryIterator * hi = CAST(itr,HashEntryIterator,base.itr);
    hi->entry.key = hi->base.next->key;
    hi->entry.value = hi->base.next->value;
    HASH_ItrAdvance(&hi->base);
    return &hi->entry;
}

STATIC Bool HASH_ItrHasNext(Iterator * itr)
{
    HashIterator * hi = CAST(itr,HashIterator,itr);
    return BoolValue(hi->next != NULL);
}

STATIC Bool HASH_ItrRemove(Iterator * itr)
{
    HashIterator * hi = CAST(itr,HashIterator,itr);
    if (hi->current == hi->ht->buckets[hi->currentPos]) {
        hi->ht->buckets[hi->currentPos] = hi->current->next;
    } else {
        HashBucket * prev = hi->ht->buckets[hi->currentPos];
        while (prev->next && prev->next != hi->current) prev = prev->next;
        if (prev->next) {
            prev->next = hi->current->next;
        } else {
            ASSMSG("Concurrent hash table modification?");
            return False;
        }
    }
    hi->ht->free(hi->current->key, hi->current->value);
    hi->ht->count--;
    ASSERT(hi->ht->count >= 0);
    HASH_FreeHashBucket(hi->current);
    hi->current = NULL;
    return True;
}

STATIC void HASH_ItrFree(Iterator * itr)
{
    HashIterator * hi = CAST(itr,HashIterator,itr);
    MEM_Free(hi);
}

STATIC void HASH_ItrFreeEntry(Iterator * itr)
{
    HashEntryIterator * hi = CAST(itr,HashEntryIterator,base.itr);
    MEM_Free(hi);
}

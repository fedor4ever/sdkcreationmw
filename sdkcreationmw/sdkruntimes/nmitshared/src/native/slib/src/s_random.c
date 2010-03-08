/*
 * $Id: s_random.c,v 1.31 2006/03/19 09:28:23 slava Exp $
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

#include "s_random.h"
#include "s_mutex.h"
#include "s_util.h"
#include "s_libp.h"
#include "s_mem.h"

#include <math.h>
#include <float.h>

#define DBL_MANT_DIG1 (DBL_MANT_DIG/2)
#define DBL_MANT_DIG2 (DBL_MANT_DIG-(DBL_MANT_DIG/2))

/* certain assumptions made about the size of mantissa */
COMPILE_ASSERT(DBL_MANT_DIG < 64);
COMPILE_ASSERT(FLT_MANT_DIG < 32);

/*
 * Random generator's context.
 */
struct _Random {
#ifndef __KERNEL__
    double nextGaussian;        /* next random gaussian value */
    Bool   haveNextGaussian;    /* True if we have the above */
#endif /* __KERNEL__ */

    Seed   seed;                /* see that inialized this sequence */

    RNG    rng;                 /* random number generation algorithm */
    void * ctx;                 /* algorithm context */

    Bool   syn;                 /* True to synchronize the generator */
    Mutex  mutex;               /* synchronization mutex */
};

#define NEXT_RANDOM(_r,_bits) ((_r)->rng.rng_next((_r)->ctx, _bits))

/**
 * Default RNG implementation is a linear congruential pseudorandom number 
 * generator, as defined by D. H. Lehmer and described by Donald E. Knuth in
 * "The Art of Computer Programming", Volume 2: "Seminumerical Algorithms", 
 * section 3.2.1.
 */
typedef struct _RNG_Lehmer_Data {
    I64s multiplier;
    I64s addend;
    I64s mask;
    I64s seed;
} RNG_Lehmer_Data;

STATIC void * RNG_Lehmer_Init(Random * rnd)
{
    RNG_Lehmer_Data * r;
    UNREF(rnd);
    
    r = MEM_New(RNG_Lehmer_Data);
    if (r) {
        r->multiplier = __INT64_C(0x5DEECE66D);
        r->addend = __INT64_C(0xB);
        r->mask = (__INT64_C(1) << 48) - 1;
        r->seed = RANDOM_GenSeed();
    }
    return r;
}

STATIC void RNG_Lehmer_Seed(void * rng, Seed s)
{
    RNG_Lehmer_Data * r =  (RNG_Lehmer_Data*)rng;
    r->seed = (s ^ r->multiplier) & r->mask;
}

STATIC I32s RNG_Lehmer_Next(void * rng, int bits)
{
    RNG_Lehmer_Data * r =  (RNG_Lehmer_Data*)rng;
    I64u nextseed = (r->seed * r->multiplier + r->addend) & r->mask;
    r->seed = nextseed;
    return (I32s)(nextseed >> (48 - bits));
}

STATIC Seed RNG_Lehmer_State(void * rng)
{
    RNG_Lehmer_Data * r =  (RNG_Lehmer_Data*)rng;
    return (r->seed ^ r->multiplier) & r->mask;
}

STATIC void RNG_Lehmer_Free(void * rng)
{
    MEM_Free(rng);
}

STATIC RNG RNG_Lehmer = {
    RNG_Lehmer_Init,
    RNG_Lehmer_Seed,
    RNG_Lehmer_Next,
    RNG_Lehmer_State,
    RNG_Lehmer_Free
};

/*
 * Global instance of Random
 */
STATIC Random RANDOM_random = {0};
STATIC int    RANDOM_initCount = 0;
STATIC Seed   RANDOM_lastSeed = 0; /* last auto-generated seed */

/**
 * Initialize random number generator context.
 */
STATIC Bool RANDOM_Init(Random * r, const RNG * algorithm)
{
    const RNG * rng = (algorithm ? algorithm : (&RNG_Lehmer));
    ASSERT(rng->rng_next);
    ASSERT(rng->rng_seed);
    if (MUTEX_Init(&r->mutex)) {
        void * ctx = NULL;
        if (!rng->rng_init || (ctx = (*(rng->rng_init))(r)) != NULL) {
#ifndef __KERNEL__
            r->nextGaussian = 0;
            r->haveNextGaussian = False;
#endif /* __KERNEL__ */
            r->rng = (*rng);
            r->ctx = ctx;
            r->syn = True;     /* synchronize by default */
            RANDOM_SetSeed(r, RANDOM_GenSeed());
            return True;
        }
        MUTEX_Destroy(&r->mutex);
    }
    return False;
}

/**
 * Deallocate resources used by the random generator context
 */
STATIC void RANDOM_Destroy(Random * r)
{
    if (r->rng.rng_free) (*(r->rng.rng_free))(r->ctx);
    MUTEX_Destroy(&r->mutex);
}

/**
 * Initialize the module.
 */
void RANDOM_InitModule()
{
    if ((RANDOM_initCount++) == 0) {
        MEM_InitModule();
        if (RANDOM_Init(&RANDOM_random, NULL)) {
            return;
        }

        /* unrecoverable error */
        SLIB_Abort(TEXT("RANDOM"));
    }
}

/**
 * Cleanup the module.
 */
void RANDOM_Shutdown()
{
    ASSERT(RANDOM_initCount > 0);
    if ((--RANDOM_initCount) == 0) {
        RANDOM_Destroy(&RANDOM_random);
        MEM_Shutdown();
    }
}

/**
 * Get single global instance of Random.
 */
Random * RANDOM_GetRandom()
{
    ASSERT(RANDOM_initCount > 0);
    return &RANDOM_random;
}

/**
 * Generates seed based on current time
 */
Seed RANDOM_GenSeed()
{

    /*
     * this is prone to race conditions, although very unlikely.
     * usually all initialization occurs on the same thread so
     * it's not a problem
     */
    Seed s = TIME_Now();
    if (RANDOM_lastSeed >= s) {
        s = ++RANDOM_lastSeed;
    } else {
        RANDOM_lastSeed = s;
    }

    return s;
}

/**
 * Allocate random number generator context.
 * Returns NULL is memory allocation fails.
 * The seed is initialized with the value returned by time()
 */
Random * RANDOM_CreateRNG(const RNG * rng)
{
    Random * r = MEM_New(Random);
    if (r) {
        if (!RANDOM_Init(r, rng)) {
            MEM_Free(r);
            r = NULL;
        }
    }
    return r;
}

/**
 * Deletes random number generator context.
 */
void RANDOM_Delete(Random * r)
{
    if (r) {
        RANDOM_Destroy(r);
        MEM_Free(r);
    }
}

/**
 * Returns True if operations on this Random object are being synchronized
 * internally. 
 *
 * NOTE: in multithread environment synchronization is required in order 
 * to produce statistically correct results. However, in cases when all 
 * operations on this instance of Random are being performed by the same 
 * thread OR if application provides external synchronization, you may 
 * want to turn off the internal Random synchronization and somewhat 
 * improve performance.
 */
Bool RANDOM_IsSync(const Random * r)
{
    return r->syn;
}

/**
 * Turns on/off the internal synchronization. By default it's on. See 
 * also comments for RANDOM_IsSync() above
 */
void RANDOM_SetSync(Random * r, Bool syn)
{
    r->syn = syn;
}

/**
 * Sets the seed of this random number generator. 
 */
Seed RANDOM_SetSeed(Random * r, Seed s)
{
    Bool unlock = BoolValue(r->syn && MUTEX_Lock(&r->mutex)); 
    r->seed = s;
    (*(r->rng.rng_seed))(r->ctx, s);
#ifndef __KERNEL__
    r->haveNextGaussian = False;
#endif /* __KERNEL__ */
    if (unlock) MUTEX_Unlock(&r->mutex);
    return s;
}

/**
 * Returns the seed that initiated the current pseudorandom number sequence
 */
Seed RANDOM_GetSeed(const Random * r)
{
    return r->seed;
}

/**
 * Returns the seed that can be used to restart the current pseudorandom 
 * number sequence from its current point.
 */
Seed RANDOM_GetState(const Random * r)
{
    Seed state = (Seed)0;
    if (r->rng.rng_state) {
        Bool unlock = BoolValue(r->syn && MUTEX_Lock((Mutex*)(&r->mutex))); 
        state = (*(r->rng.rng_state))(r->ctx);
        if (unlock) MUTEX_Unlock((Mutex*)(&r->mutex));
    }
    return state;
}

/**
 * Returns True if RNG supports the notion of state.
 */
Bool RANDOM_HasState(const Random * r)
{
    return BoolValue(r->rng.rng_state != NULL);
}

/**
 * Returns the next pseudorandom, uniformly distributed 32-bit integer
 * value from this random number generator's sequence. 
 */
I32s RANDOM_NextI32(Random * r)
{
    Bool unlock = BoolValue(r->syn && MUTEX_Lock(&r->mutex)); 
    I32s next =  NEXT_RANDOM(r,32); 
    if (unlock) MUTEX_Unlock(&r->mutex);
    return next;
}

/**
 * Returns a pseudorandom, uniformly distributed 32-bit integer value
 * between 0 (inclusive) and the specified value (exclusive), drawn 
 * from this random number generator's sequence.
 */
I32s RANDOM_NextInt32(Random * r, int n)
{
    Bool unlock = BoolValue(r->syn && MUTEX_Lock(&r->mutex)); 
    I32s bits, val;
    ASSERT(n>0);

    if ((n & -n) == n) { /* i.e., n is a power of 2 */
        I64s next = (I64s)NEXT_RANDOM(r,31);
        if (unlock) MUTEX_Unlock(&r->mutex);
        return (I32s)((n * next) >> 31);
    }

    do {
        bits = NEXT_RANDOM(r,31);
        val = bits % n;
    } while (bits - val + (n-1) < 0);
    
    if (unlock) MUTEX_Unlock(&r->mutex);
    return val;
}

/**
 * Returns the next pseudorandom, uniformly distributed 64-bit integer
 * value from this random number generator's sequence. 
 */
I64s RANDOM_NextI64(Random * r)
{
    Bool unlock = BoolValue(r->syn && MUTEX_Lock(&r->mutex)); 
    I64s next = ((I64s)(NEXT_RANDOM(r,32)) << 32) + NEXT_RANDOM(r,32);
    if (unlock) MUTEX_Unlock(&r->mutex);
    return next;
}

/**
 * Returns a pseudorandom, uniformly distributed 64-bit integer value
 * between 0 (inclusive) and the specified value (exclusive), drawn 
 * from this random number generator's sequence.
 */
I64s RANDOM_NextInt64(Random * r, I64s n)
{
    if (n <= INT_MAX) {
        return (I64s)RANDOM_NextInt32(r, (int)n);
    } else {
        /* this is essentially cut&pasted from RANDOM_NextInt32.
         * I haven't done any formal testing of whether it actually 
         * works as expected. */
        Bool unlock = BoolValue(r->syn && MUTEX_Lock(&r->mutex)); 
        I64s bits, val;
        ASSERT(n>0);

        do {
            bits = ((I64u)(NEXT_RANDOM(r,31)) << 32) + NEXT_RANDOM(r,32);
            val = bits % n;
        } while (bits - val + (n-1) < 0);
    
        if (unlock) MUTEX_Unlock(&r->mutex);
        return val;
    }
}

/**
 * Returns the next pseudorandom, uniformly distributed boolean 
 * value from this random number generator's sequence. 
 */
Bool RANDOM_NextBool(Random * r)
{
    Bool unlock = BoolValue(r->syn && MUTEX_Lock(&r->mutex)); 
    Bool next = BoolValue(NEXT_RANDOM(r,1));
    if (unlock) MUTEX_Unlock(&r->mutex);
    return next;
}

/*
 * The code that uses floating point numbers is not being compiled 
 * in NT kernel mode configuration.
 */
#ifndef __KERNEL__

/**
 * Returns the next pseudorandom, uniformly distributed float
 * value between 0.0 and 1.0 from this random number generator's 
 * sequence.
 *
 * This code is not being compiled in NT kernel configuration because
 * floating point arithmetics is not available there.
 */
float RANDOM_NextFloat(Random * r)
{
    Bool unlock = BoolValue(r->syn && MUTEX_Lock(&r->mutex)); 
    I32s i = NEXT_RANDOM(r,FLT_MANT_DIG);
    float f = i / ((float)(1 << FLT_MANT_DIG));
    if (unlock) MUTEX_Unlock(&r->mutex);
    return f;
}

/**
 * Returns the next pseudorandom, uniformly distributed double
 * value between 0.0 and 1.0 from this random number generator's 
 * sequence.
 */
STATIC double nextDouble(Random * r)
{
    I64s l = ((I64s)(NEXT_RANDOM(r,DBL_MANT_DIG1)) << DBL_MANT_DIG2) + 
        NEXT_RANDOM(r,DBL_MANT_DIG2);
    return (l / (double)(__INT64_C(1) << DBL_MANT_DIG));
}

/**
 * Returns the next pseudorandom, uniformly distributed double
 * value between 0.0 and 1.0 from this random number generator's 
 * sequence.
 */
double RANDOM_NextDouble(Random * r)
{
    Bool unlock = BoolValue(r->syn && MUTEX_Lock(&r->mutex)); 
    double d = nextDouble(r);
    if (unlock) MUTEX_Unlock(&r->mutex);
    return d;
}

/**
 * Returns the next pseudorandom, Gaussian ("normally") distributed
 * double value with mean 0.0 and standard deviation 1.0 from this 
 * random number generator's sequence.
 *
 * This uses the *polar method* of G. E. P. Box, M. E. Muller, 
 * and G. Marsaglia, as described by Donald E. Knuth in "The Art
 * of Computer Programming", Volume 2: Seminumerical Algorithms, 
 * section 3.4.1, subsection C, algorithm P. Note that algorithm 
 * generates two independent values at the cost of only one call 
 * to log() and one call to sqrt(). 
 *
 * To be honest, this code was stolen from Java class java.util.Random
 * if you haven't noticed yet.
 */
double RANDOM_NextGauss(Random * r)
{
    Bool unlock = BoolValue(r->syn && MUTEX_Lock(&r->mutex)); 
    double d;

    /* See Knuth, ACP, Section 3.4.1 Algorithm C. */
    if (r->haveNextGaussian) {
        r->haveNextGaussian = False;
        d = r->nextGaussian;
    } else {
        double v1, v2, s, x;
        do { 
            v1 = 2 * nextDouble(r) - 1; /* between -1 and 1 */
            v2 = 2 * nextDouble(r) - 1; /* between -1 and 1 */
            s = v1 * v1 + v2 * v2;
        } while (s >= 1);
        x = sqrt(-2 * log(s)/s);
        r->nextGaussian = v2 * x;
        r->haveNextGaussian = True;
        d = v1 * x;
    }

    if (unlock) MUTEX_Unlock(&r->mutex);
    return d;
}

#endif /* __KERNEL__ */

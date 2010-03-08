/*
 * $Id: s_random.h,v 1.21 2006/01/08 08:37:35 slava Exp $
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

#ifndef _SLAVA_RANDOM_H_
#define _SLAVA_RANDOM_H_

#include "s_def.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* data types */
typedef struct _Random Random;
typedef I64s Seed;

/* 
 * Random number generator algorithm.
 *
 * The next() function generates the next pseudorandom number for any of 
 * the RANDOM_NextXxx functions. The general contract is that it 
 * returns a 32 int value and if the argument bits is between 1 
 * and 32 (inclusive), then that many low-order bits of the returned 
 * value will be (approximately) independently chosen bit values, 
 * each of which is (approximately) equally likely to be 0 or 1.
 *
 * State of the generator is defined as a seed that can be used 
 * to restart the sequence from the current point. Such thing may 
 * not exist. If RNG does not support a notion of state, it must 
 * leave rng_state as NULL. RANDOM_HasState() can be used to test 
 * whether the algorithm has state. The default algorithm supports 
 * it of course.
 */
typedef void * (*RngInit)  P_((Random * r));
typedef void   (*RngSeed)  P_((void * rng, Seed s));
typedef I32s   (*RngNext)  P_((void * rng, int bits));
typedef Seed   (*RngState) P_((void * rng));
typedef void   (*RngFree)  P_((void * rng));

typedef struct _RNG {
    RngInit  rng_init;  /* creates an instance of RNG */
    RngSeed  rng_seed;  /* seeds the generator */
    RngNext  rng_next;  /* returns the next pseudorandom number */
    RngState rng_state; /* returns the state of the generator */
    RngFree  rng_free;  /* deallocates the instance */
} RNG;

/* global operations */
extern void     RANDOM_InitModule P_((void));
extern void     RANDOM_Shutdown   P_((void));
extern Random * RANDOM_GetRandom  P_((void));
extern Seed     RANDOM_GenSeed    P_((void));

/* operations on a particular instance of Random */
extern Random * RANDOM_CreateRNG  P_((const RNG * rng));
extern void     RANDOM_Delete     P_((Random * r));
extern Bool     RANDOM_IsSync     P_((const Random * r));
extern void     RANDOM_SetSync    P_((Random * r, Bool syn));
extern Bool     RANDOM_HasState   P_((const Random * r));
extern Seed     RANDOM_GetState   P_((const Random * r));
extern Seed     RANDOM_GetSeed    P_((const Random * r));
extern Seed     RANDOM_SetSeed    P_((Random * r, Seed seed));
extern I32s     RANDOM_NextI32    P_((Random * r));
extern I32s     RANDOM_NextInt32  P_((Random * r, int n));
extern I64s     RANDOM_NextI64    P_((Random * r));
extern I64s     RANDOM_NextInt64  P_((Random * r, I64s n));
extern Bool     RANDOM_NextBool   P_((Random * r));

#ifndef __KERNEL__
extern float    RANDOM_NextFloat  P_((Random * r));
extern double   RANDOM_NextDouble P_((Random * r));
extern double   RANDOM_NextGauss  P_((Random * r));
#endif /* __KERNEL__ */

#define RANDOM_Create()       RANDOM_CreateRNG(NULL)
#define RANDOM_NextInt(_r,_n) RANDOM_NextInt32(_r,_n)

/* operations on the shared instance of Random */
#define RAND_GetSeed()      RANDOM_GetSeed(RANDOM_GetRandom())
#define RAND_GetState()     RANDOM_GetState(RANDOM_GetRandom())
#define RAND_SetSeed(_seed) RANDOM_SetSeed(RANDOM_GetRandom(),_seed)
#define RAND_NextI32()      RANDOM_NextI32(RANDOM_GetRandom())
#define RAND_NextInt(_n)    RANDOM_NextInt(RANDOM_GetRandom(),_n)
#define RAND_NextInt32(_n)  RANDOM_NextInt32(RANDOM_GetRandom(),_n)
#define RAND_NextI64()      RANDOM_NextI64(RANDOM_GetRandom())
#define RAND_NextInt64(_n)  RANDOM_NextInt64(RANDOM_GetRandom(),_n)
#define RAND_NextBool()     RANDOM_NextBool(RANDOM_GetRandom())

#ifndef __KERNEL__
#define RAND_NextFloat()    RANDOM_NextFloat(RANDOM_GetRandom())
#define RAND_NextDouble()   RANDOM_NextDouble(RANDOM_GetRandom())
#define RAND_NextGauss()    RANDOM_NextGauss(RANDOM_GetRandom())
#endif /* __KERNEL__ */

#ifdef __cplusplus
} /* end of extern "C" */
#endif  /* __cplusplus */

#endif /* _SLAVA_RANDOM_H_ */

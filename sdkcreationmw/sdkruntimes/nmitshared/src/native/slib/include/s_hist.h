/*
 * $Id: s_hist.h,v 1.11 2005/02/20 20:31:29 slava Exp $
 *
 * Copyright (C) 2001-2005 by Slava Monich
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

#ifndef _SLAVA_HIST_H_
#define _SLAVA_HIST_H_

#include "s_def.h"
#include "s_strbuf.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* contents of one bin. */
#ifdef __SYMBIAN32__
   typedef I32u HBin;
#  define MAX_HBIN_VALUE  UINT_MAX
#  define HBIN_FORMAT "%u"
#else /* !__SYMBIAN32__ */
   typedef I64u HBin;
#  define MAX_HBIN_VALUE  UINT64_MAX
#  define HBIN_FORMAT LONG_ULONG_FORMAT
#endif /* !__SYMBIAN32__ */

/*
 * Flags:
 *
 * H_OVERFLOW   - this flag is set in Histogram->flags if an overflow is 
 *                detected in one of the bins of the histogram.
 *
 * H_AUTO_RANGE - this bit in HistAxis->flags marks the axes that have
 *                "auto range" feature enabled. If any axis has this flag,
 *                it's also set in Histogram->flags
 *
 * H_MAX_VALUE  - this flag is set in Histogram->flags if maxval field 
 *                contains a valid up-to-date value
 */
#define H_OVERFLOW   0x0001  /* overflow detected */
#define H_AUTO_RANGE 0x0002  /* range is detected automatically */
#define H_MAX_VALUE  0x0004  /* maxval field contains a valid value */

/* 
 * Histogram descriptor for one axis. Multi-dimensional histograms need 
 * more than one of those, one per each axis. 
 */
typedef struct _HistAxis {
    int nbins;          /* number of bins */
    StrBuf label;       /* name of the axis */
    double interval;    /* size of the interval */
    double min;         /* lower edge, inclusive */
    double max;         /* upper edge, exclusive */
    int flags;          /* flags */
} HistAxis;

/* 
 * N-dimensional histogram. Can be one-dimensional too. However, the
 * Hist1D histogram and functions that work with it are better optimized 
 * for one-dimensional case, use those whenever posisble. Note that the 
 * size of the Histogram memory block allocated for N-dimensional histogram,
 * where N >= 2, is greater than the size of this structure.
 */
typedef struct _Histogram {
    StrBuf title;       /* histogram title */
    HBin total;         /* total number of entries */
    HBin missed;        /* total attempts to put an out-of-bounds value */
    HBin maxval;        /* maximum value */
    HBin * bins;        /* histogram contents */
    int dim;            /* number of dimensions */
    int totalbins;      /* total number of bins */
    int usedbins;       /* number of used bins */
    int flags;          /* flags */
    HistAxis axis[1];   /* variable size array, must be the last */
} Histogram;

/* 
 * one-dimensional histogram. The size of this structure is fixed, so it 
 * may be allocated on stack, initialized by HIST1D_Init and deinitialized
 * by HIST1D_Destroy
 */
typedef union _Hist1D {
    Histogram hist;     /* histogram data */
} Hist1D;

/* content of a single bin (used by enumeration) */
typedef struct _HBinData {
    const Histogram * hist;     /* the histogram containing the data */
    HBin count;                 /* the contents of the bin */
    int bin[1];                 /* identifies the bin (variable size) */
} HBinData;

/**
 * HistCB - callback for HIST_Examine. It's called for each non-empty
 *          bin of the histogram. This is more efficient than requesting
 *          the content of every single bin with HIST_Get
 */

typedef Bool (*HistCB) P_((const HBinData * data, void * ctx));

/*
 * These are the functions for type-checking macro parameters of the macros
 * at compile time. We only need to do this in debug build.
 */
#if DEBUG
extern Hist1D * HIST1D_Cast P_((Hist1D * h));
extern Histogram * HIST_Cast P_((Histogram * h));
extern const Hist1D * HIST1D_CastC P_((const Hist1D * h));
extern const Histogram * HIST_CastC P_((const Histogram * h));
#else  /* !DEBUG */
#  define HIST1D_Cast(_h) (_h)
#  define HIST1D_CastC(_h) (_h)
#  define HIST_Cast(_h)  (_h)
#  define HIST_CastC(_h)  (_h)
#endif /* !DEBUG */

/* functions to work with one-dimensional histogram */
extern Hist1D * HIST1D_Create P_((Str s, int n, double min, double max));
extern Bool HIST1D_Init P_((Hist1D * h, Str s, int n, double min, double max));
extern void HIST1D_Destroy P_((Hist1D * h));
extern void HIST1D_Delete P_((Hist1D * h));

extern Bool HIST1D_IsIn P_((const Hist1D * h, double d));
extern Bool HIST1D_Put P_((Hist1D * h, double value));
extern Bool HIST1D_PutAll P_((Hist1D * h, const double * values, int count));
extern HBin HIST1D_Get P_((const Hist1D * h, int i));
extern void HIST1D_Set P_((Hist1D * h, int i, HBin value));

/*
 * The following functions are implemented as macros:
 *
 * extern void HIST1D_Reset P_((Hist1D * h));
 * extern HBin HIST1D_MaxValue P_((const Hist1D * h));
 * extern int  HIST1D_GetSize P_((const Hist1D * h));
 * extern Str  HIST1D_GetTitle P_((const Hist1D * h));
 * extern Bool HIST1D_SetTitle P_((Hist1D * h, Str title));
 * extern Str  HIST1D_GetLabel P_((const Hist1D * h));
 * extern Bool HIST1D_SetLabel P_((Hist1D * h, Str label));
 * extern Bool HIST1D_IsOver P_((const Hist1D * h));
 * extern HBin HIST1D_GetTotal P_((const Hist1D * h));
 * extern HBin HIST1D_GetMissed P_((const Hist1D * h));
 * extern int  HIST1D_GetUsedBins P_((const Hist1D * h));
 * extern double HIST1D_GetMin P_((const Hist1D * h));
 * extern double HIST1D_GetMax P_((const Hist1D * h));
 */
#define HIST1D_Reset(_h) HIST_Reset(&HIST1D_Cast(_h)->hist)
#define HIST1D_MaxValue(_h) HIST_MaxValue(&HIST1D_Cast(_h)->hist)
#define HIST1D_GetTitle(_h) STRBUF_GetString(&HIST1D_CastC(_h)->hist.title)
#define HIST1D_SetTitle(_h,_s) STRBUF_Copy(&HIST1D_Cast(_h)->hist.title),_s)
#define HIST1D_GetLabel(_h) HIST_GetLabel(&HIST1D_CastC(_h)->hist,0)
#define HIST1D_SetLabel(_h,_s) HIST_SetLabel(&HIST1D_Cast(_h)->hist,0,_s)
#define HIST1D_GetSize(_h) (HIST1D_CastC(_h)->hist.axis[0].nbins)
#define HIST1D_IsOver(_h) (((HIST1D_CastC(_h)->hist.flags) & H_OVERFLOW) != 0)
#define HIST1D_GetTotal(_h) (HIST1D_CastC(_h)->hist.total)
#define HIST1D_GetMissed(_h) (HIST1D_CastC(_h)->hist.missed)
#define HIST1D_GetUsedBins(_h) (HIST1D_CastC(_h)->hist.usedbins)
#define HIST1D_GetMin(_h) (HIST1D_CastC(_h)->hist.axis[0].min)
#define HIST1D_GetMax(_h) (HIST1D_CastC(_h)->hist.axis[0].max)

/* functions to work with multi-dimensional histogram */
extern Histogram * HIST_Create P_((Str s, int dim, const int n[],
    const double min[], const double max[]));
extern void HIST_Delete P_((Histogram * h));

extern double HIST_GetMin P_((const Histogram * h, int i));
extern double HIST_GetMax P_((const Histogram * h, int i));
extern int  HIST_GetSize P_((const Histogram * h, int i));
extern Str  HIST_GetLabel P_((const Histogram * h, int i));
extern Bool HIST_SetLabel P_((Histogram * h, int i, Str label));
extern void HIST_Reset P_((Histogram * h));
extern HBin HIST_MaxValue P_((const Histogram * h));
extern Bool HIST_IsIn P_((const Histogram * h, const double x[]));
extern Bool HIST_Put P_((Histogram * h, const double value[]));
extern void HIST_Set P_((Histogram * h, const int i[], HBin value));
extern HBin HIST_Get P_((const Histogram * h, const int i[]));
extern void HIST_Examine P_((const Histogram * h, HistCB cb, void * ctx));

/*
 * The following functions are implemented as macros:
 *
 * extern int  HIST_GetDim P_((const Histogram * h))
 * extern Str  HIST_GetTitle P_((const Histogram * h))
 * extern Bool HIST_SetTitle P_((Histogram * h, Str title))
 * extern Bool HIST_IsOver P_((const Histogram * h));
 * extern HBin HIST_GetTotal P_((const Histogram * h));
 * extern HBin HIST_GetMissed P_((const Histogram * h));
 * extern int  HIST_GetUsedBins P_((const Histogram * h));
 */
#define HIST_GetDim(_h) (HIST_CastC(_h)->dim)
#define HIST_GetTitle(_h) STRBUF_GetString(&HIST_CastC(_h)->title)
#define HIST_SetTitle(_h,_s) STRBUF_Copy(&HIST_Cast(_h)->title,_s)
#define HIST_IsOver(_h) (((HIST_CastC(_h)->flags) & H_OVERFLOW) != 0)
#define HIST_GetTotal(_h) (HIST_CastC(_h)->total)
#define HIST_GetMissed(_h) (HIST_CastC(_h)->missed)
#define HIST_GetUsedBins(_h) (HIST_CastC(_h)->usedbins)

#ifdef __cplusplus
} /* end of extern "C" */
#endif  /* __cplusplus */

#endif /* _SLAVA_HIST_H_ */

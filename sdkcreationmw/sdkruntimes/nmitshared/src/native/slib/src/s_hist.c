/*
 * $Id: s_hist.c,v 1.16 2005/02/19 03:36:13 slava Exp $
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

#include "s_hist.h"
#include "s_mem.h"

/* Flags that survive the reset */
#define H_PERMANENT_FLAGS H_AUTO_RANGE

/* values from returned by HIST_ComputePos */
#define HPOS_MISSED      -1
#define HPOS_CAN_STRETCH -2

/*
 * These are the functions for compile time type-checking macro 
 * parameters. We only need to do this in debug build.
 */
#if DEBUG
Hist1D * HIST1D_Cast(Hist1D * h) { return h; }
Histogram * HIST_Cast(Histogram * h) { return h; }
const Hist1D * HIST1D_CastC(const Hist1D * h) { return h; }
const Histogram * HIST_CastC(const Histogram * h) { return h; }
#endif /* !DEBUG */

/*
 * Enumaration context
 */
typedef struct _HistEnum {
    int nbins;                 /* number of bins found so far */
    HistCB cb;                 /* the callback to call */
    void * ctx;                /* callback's context */
    HBinData data;             /* the rest of the data (variable size) */
} HistEnum;

/*==========================================================================*
 *               MULTI DIMENSIONAL HISTOGRAM
 *==========================================================================*/

/**
 * Initializes a histogram
 */
STATIC Bool HIST_Init(Histogram * h, Str s, int dim, const int n[], 
                      const double min[], const double max[])
{
    int i;
    size_t size = OFFSET(Histogram,axis) + sizeof(HistAxis)*dim;

    /* initialize the Histogram structure */
    memset(h, 0, size);
    STRBUF_Init(&h->title);

    /* calculate total number of bins */
    h->totalbins = 1;
    for (i=0; i<dim && h->totalbins; i++) {
        ASSERT(n[i] >= 0);
        h->totalbins *= n[i];
    }

    /* allocate memory for bins */
    if (h->totalbins) h->bins = MEM_NewArray(HBin,h->totalbins);
    if (!h->totalbins || h->bins) {

        /* copy the title if we have one */
        if (!s || STRBUF_Copy(&h->title, s)) {

            h->dim = dim;
            for (i=0; i<dim; i++) {
                HistAxis * a = h->axis + i;
                ASSERT(min[i] < max[i]);
                STRBUF_Init(&a->label);
                a->nbins = n[i];
                a->min = min[i];
                a->max = max[i];
                if (a->nbins <= 0) {
                    a->nbins = 0;
                    a->flags |= H_AUTO_RANGE;
                    h->flags |= H_AUTO_RANGE;
                    a->interval = a->max - a->min;
                } else {
                    a->interval = (a->max - a->min)/a->nbins;
                }
            }

            /* this will clear the array of bins */
            HIST_Reset(h);
            return True;

        } else {
            MEM_Free(h->bins);
        }
    }
    return False;
}

/**
 * Allocates a new histogram
 */
Histogram * HIST_Create(Str s, int dim, const int n[], const double min[], 
                        const double max[])
{
    ASSERT(dim > 0);
    ASSERT(n);
    ASSERT(min);
    ASSERT(max);
    if (dim > 0 && n && min && max) {
        size_t size = OFFSET(Histogram,axis) + sizeof(HistAxis)*dim;
        Histogram * h = (Histogram*)MEM_Alloc(size);
        if (h) {
            if (HIST_Init(h, s, dim, n, min, max)) {
                return h;
            }
            MEM_Free(h);
        }
    }
    return NULL;
}

/**
 * Destroys the contents of the histogram
 */
STATIC void HIST_Destroy(Histogram * h)
{
    int i;
    STRBUF_Destroy(&h->title);
    for (i=0; i<h->dim; i++) {
        STRBUF_Destroy(&(h->axis[i].label));
    }
    if (h->bins) {
        MEM_Free(h->bins);
        h->bins = NULL;
    }
}

/**
 * Delete the histogram
 */
void HIST_Delete(Histogram * h)
{
    if (h) {
        HIST_Destroy(h);
        MEM_Free(h);
    }
}

/**
 * Sets the label for the i-th axis. If label is NULL, it gets reset
 * to its default value
 */
Bool HIST_SetLabel(Histogram * h, int i, Str label)
{
    ASSERT(i < h->dim);
    if (i < h->dim) {
        HistAxis * a = h->axis + i;
        if (label) {
            return STRBUF_Copy(&a->label, label);
        } else {
            STRBUF_Clear(&a->label);
            return True;
        }
    }
    return False;
}

/**
 * Returns the label for the i-th axis. Unless set externally, the axis
 * are labeled X, Y and Z for dimensions <= 3, or X1, X2, X3, X4, ... for
 * dimensions >= 4 
 */
Str HIST_GetLabel(const Histogram * h, int i)
{
    ASSERT(i < h->dim);
    if (i < h->dim) {
        const HistAxis * a = h->axis + i;
        if (STRBUF_Length(&a->label) == 0) {
            if (h->dim <= 3) {
                static Str labels [] = {TEXT("X"), TEXT("Y"), TEXT("Z")};
                return labels[i];
            } else {
                HistAxis * axis = (HistAxis*)a; /* cheating... */
                if (!STRBUF_Format(&axis->label, TEXT("X%d"), (i+1))) {
                    return NULL;
                }
            }
        }
        return STRBUF_Text(&a->label);
    }
    return NULL;
}

/**
 * Resets one axis of the histogram.
 */
STATIC void HIST_ResetAxis(HistAxis * a)
{
    a->flags &= H_PERMANENT_FLAGS;
    if (a->flags & H_AUTO_RANGE) {
        a->max = a->min;
        a->nbins = 0;
    }
}

/**
 * Resets the histogram.
 */
void HIST_Reset(Histogram * h)
{
    int i;
    h->missed  = 
    h->total = 0;
    h->usedbins = 0;
    h->maxval = 0;
    h->flags &= H_PERMANENT_FLAGS;
    h->flags |= H_MAX_VALUE;
    for (i=0; i<h->dim; i++) {
        HIST_ResetAxis(h->axis + i);
    }
    if (h->flags & H_AUTO_RANGE) {
        MEM_Free(h->bins);
        h->bins = NULL;
        h->totalbins = 0;
    } else {
        memset(h->bins, 0, sizeof(HBin) * h->totalbins);
    }
}

/**
 * Returns the minimum value.
 */
double HIST_GetMin(const Histogram * h, int i)
{
    ASSERT(h);
    ASSERT(i >= 0);
    ASSERT(i < h->dim);
    if (h && i >= 0 && i < h->dim) {
        return h->axis[i].min;
    }
    return 0.0;
}

/**
 * Returns the maximum value.
 */
double HIST_GetMax(const Histogram * h, int i)
{
    ASSERT(h);
    ASSERT(i >= 0);
    ASSERT(i < h->dim);
    if (h && i >= 0 && i < h->dim) {
        return h->axis[i].max;
    }
    return 0.0;
}

/**
 * Returns number of bins along ith coordinate.
 */
int HIST_GetSize(const Histogram * h, int i)
{
    ASSERT(h);
    ASSERT(i >= 0);
    ASSERT(i < h->dim);
    if (h && i >= 0 && i < h->dim) {
        return h->axis[i].nbins;
    }
    return 0;
}

/**
 * Tests if the specified value falls into the histogram range
 */
Bool HIST_IsIn(const Histogram * h, const double value [])
{
    ASSERT(h);
    if (h) {
        int i;
        for (i=0; i<h->dim; i++) {
            /* If the axis has H_AUTO_RANGE flag set, everything is in */
            const HistAxis * a = h->axis + i;
            if (!(a->flags & H_AUTO_RANGE)) {
                if (value[i] < a->min || value[i] >= a->max) {
                    return False;
                }
            }
        }
        return True;
    }
    return False;
}

/**
 * Recursive helper for HIST_Examine
 */
STATIC void HIST_Examine2(HistEnum * e, int k)
{
    const Histogram * h = e->data.hist;
    const HistAxis * a = h->axis + k;
    int * bin = e->data.bin;
    if ((k+1) == e->data.hist->dim) {

        /* calculate start index */
        int i;
        int pos = 0;
        int step = h->totalbins;
        for (i=0; i<(h->dim-1); i++) {
            step /= h->axis[i].nbins;
            pos += bin[i]*step;
        }

        /* scan the bins */
        for (bin[k]=0; e->nbins < h->usedbins && bin[k]<a->nbins; bin[k]++) {
            e->data.count = h->bins[pos + bin[k]];
            if (e->data.count) {
                e->nbins++;
                if (!e->cb(&e->data, e->ctx)) {
                    /* stop the enumeration */
                    e->nbins = h->usedbins;
                }
            }
        }

    } else {
        for (bin[k]=0; e->nbins < h->usedbins && bin[k]<a->nbins; bin[k]++) {
            /* scan the next dimension */
            HIST_Examine2(e, k+1);
        }
    }
}

/**
 * Calls the specified callback function for each non-empty bin of the 
 * histogram. This is more efficient than requesting the content of every 
 * single bin with HIST_Get
 */
void HIST_Examine(const Histogram * h, HistCB cb, void * ctx)
{
    if (h->total > 0) {
        int size = sizeof(HistEnum) + (h->dim-1)*sizeof(int);
        HistEnum * e = (HistEnum*)MEM_Alloc(size);
        if (e) {
            memset(e, 0, size);
            e->cb = cb;
            e->ctx = ctx;
            e->data.hist = h;
            HIST_Examine2(e, 0);
            MEM_Free(e);
        }
    }
}

/**
 * Increments the specified bin value and sets "overflow" flag if it's 
 * already reached MAX_BIN_VALUE value. ASSERTs when histogram is overflown 
 * for the first time
 */
STATIC HBin HIST_Inc(Histogram * h, HBin x)
{
    if (x == MAX_HBIN_VALUE) {
        /* 
         * Histograms should not overflow. They are pretty much useless when
         * they do. This ASSERT only fires once when overflow occurs for the 
         * first time.
         */
        ASSMSG("Histogram overflow!");   
        h->flags |= H_OVERFLOW;
    } else {
        x++;
    }
    return x;
}

/**
 * Calculates the index of the bin where we put the specified value.
 * If the value is out of bounds, returns negative value
 *
 * Should this be a macro?
 */
STATIC int HIST_BinIndex(const HistAxis * a, double x)
{
    if (x >= a->min && x < a->max && a->nbins > 0) {
        return (int)((x - a->min)/a->interval);
    } else {
        return -1;
    }
}

/**
 * Returns the position of the bin that matches the specified value.
 * If the value if out of bounds of the histogram, one of the following
 * (negative) values is returned:
 *
 * HPOS_MISSED (-1)       this is a miss
 * HPOS_CAN_STRETCH (-2)  we missed but the histogram is configured such that
 *                        we may dynamically stretch it and count this value 
 */
STATIC int HIST_ComputePos(const Histogram * h, const double value[])
{
    int i;
    int pos = 0;
    int step = h->totalbins;
    Bool missed = False;
    for (i=0; i<h->dim; i++) {
        const HistAxis * a = h->axis + i;
        int n = 0;
        if (!a->nbins || (n = HIST_BinIndex(a, value[i])) < 0) {
            if (a->flags & H_AUTO_RANGE) {
                missed = True;
            } else {
                /* it's out, cannot stretch */
                return HPOS_MISSED;
            }
        }
        if (step) {
            step /= a->nbins;
            pos += n*step;
        }
    }
    return (missed ? HPOS_CAN_STRETCH : pos);
}

/**
 * Recursive helper for resampling the histogram.
 */
STATIC void HIST_Stretch2(
    const Histogram * h,    /* the histogram beig stretched */
    HBin * newbuf,          /* new histogram buffer */
    int i,                  /* axis index */
    const int k[],          /* bins added, + up, - down */
    int start1,             /* start of the region in the old buffer */
    int start2,             /* start of the region in the new buffer */
    int size1,              /* size of the region in the old buffer */
    int size2)              /* size of the region in the new buffer */
{
    int n = ABS(k[i]);
    const HistAxis * a = h->axis + i;
    int nbins = a->nbins + n;
    int nextSize1 = size1/a->nbins;
    int nextSize2 = size2/nbins;
    int skip = nextSize2 * n;
    int offset = 0;

    ASSERT(!(size1%a->nbins));
    ASSERT(!(size2%nbins));

    /* clear unused part of the buffer */
    if (k[i] < 0) {
        memset(newbuf + start2, 0, sizeof(HBin) * skip);
        offset = skip;
    } else if (k[i] > 0) {
        memset(newbuf + start2 + size2 - skip, 0, sizeof(HBin) * skip);
    }
    
    if (i == (h->dim-1)) {
        /* copy data from the old buffer */
        int copyBytes = sizeof(HBin) * a->nbins;
        memcpy(newbuf + start2 + offset, h->bins + start1, copyBytes);
    } else {
        /* recurse */
        int j;
        for (j=0; j<a->nbins; j++) {
            HIST_Stretch2(h, newbuf, i+1, k, 
                          start1 + j * nextSize1,
                          start2 + offset + j * nextSize2, 
                          nextSize1, nextSize2);
        }
    }
}

/**
 * Resamples the histogram so that the specified value would fit in.
 * Does not actually updates the statistics. Returns False if it fails 
 * to reallocate the buffer.
 */
STATIC Bool HIST_Stretch(Histogram * h, const double value[])
{
    int k0;
    int * k = ((h->dim == 1) ? &k0 : MEM_NewArray(int,h->dim));
    if (k) {
        HBin * newbuf;
        
        /* determine the amount of memory we need */
        int i, n = 1;
        memset(k, 0, sizeof(*k) * h->dim);
        for (i=0; i<h->dim; i++) {
            const HistAxis * a = h->axis + i;
            if (HIST_BinIndex(a, value[i]) >= 0) {
                n *= a->nbins;
            } else {
                ASSERT(a->flags & H_AUTO_RANGE);
                if (a->nbins) {
                    double x = value[i];
                    if (x >= a->max) {
                        k[i] = (int)((x - a->max)/a->interval)+1;
                    } else if (x < a->min) {
                        k[i] = (int)((x - a->min)/a->interval);
                        if (x < (a->min+k[i]*a->interval)) k[i]--;
                    }
                    n *= a->nbins + ABS(k[i]);
                } else {
                    k[i] = 1;
                }
            }
        }

        /* allocate new buffer and resample the histogram */
        ASSERT(n > h->totalbins);
        newbuf = MEM_NewArray(HBin,n);
        if (newbuf) {
            if (!h->totalbins) {
                /* the first hit */
                memset(newbuf, 0, sizeof(HBin) * n);
            } else {
                HIST_Stretch2(h, newbuf, 0, k, 0, 0, h->totalbins, n);
            }
                
            /* update the axis */
            for (i=0; i<h->dim; i++) {
                HistAxis * a = h->axis + i;
                if (!a->nbins) {
                    ASSERT(a->min == a->max);
                    if (value[i] >= a->min) {
                        int add = (int)((value[i] - a->min)/a->interval);
                        a->max = a->min = a->min + a->interval * add;
                    } else {
                        int add = (int)((a->min - value[i])/a->interval) + 1;
                        a->max = a->min = a->min - a->interval * add;
                    }
                }
                if (k[i] > 0) {
                    a->nbins += k[i];
                    a->max += k[i] * a->interval;
                } else if (k[i] < 0) {
                    a->nbins -= k[i];
                    a->min += k[i] * a->interval;
                }
            }

            /* switch to the new buffer */
            if (h->dim > 1) MEM_Free(k);
            MEM_Free(h->bins);
            h->bins = newbuf;
            h->totalbins = n;
            return True;
        }
        if (h->dim > 1) MEM_Free(k);
    }
    return False;
}

/**
 * Adds another value to the statistics. If the value is outside of the
 * histogram's range, incremented the "missed" count, otherwise increments
 * the "total" count and the value of the appropriate bin. Returns True is
 * the value was counted, False if overflow has occured.
 */
Bool HIST_Put(Histogram * h, const double value[])
{
    if (!(h->flags & H_OVERFLOW)) {
        int pos = HIST_ComputePos(h, value);

        /* stretch the histogram if necessary */
        if (pos == HPOS_CAN_STRETCH) {

            /* resample the histogram */
            if (HIST_Stretch(h, value)) {

                /* recalculate the position. This time it must succeed */
                pos = HIST_ComputePos(h, value);
                ASSERT(pos >= 0);
            } else {
                /* count this as a miss */
                pos = HPOS_MISSED;
            }
        }

        if (pos == HPOS_MISSED) {

            /* 
             * do not set the "overflow" flag if "miss" count gets 
             * overflown. This counter usually does not affect the 
             * statistics we are collecting except for some rare cases. 
             * If we will ever need this counter for anything rather 
             * than strictly informational purposes, this behavior can 
             * be made configurable.
             */
            if (h->missed != MAX_HBIN_VALUE) {
                h->missed++;
            }
        } else {
            HBin binValue = HIST_Inc(h, h->bins[pos]);
            h->total = HIST_Inc(h, h->total);
            if (!h->bins[pos]) h->usedbins++;
            if ((h->flags & H_MAX_VALUE) && (binValue > h->maxval)) {
                h->maxval = binValue;
            }
            h->bins[pos] = binValue;
            if (!(h->flags & H_OVERFLOW)) {
                return True;
            } else {
                return False;
            }
        }
    }
    return False;
}

/**
 * Returns the index of the specified bin.
 */
STATIC int HIST_FlatBinIndex(const Histogram * h, const int n [])
{
    ASSERT(h);
    if (h) {
        int i, pos = 0;
        int step = h->totalbins;
        for (i=0; i<h->dim; i++) {
            ASSERT(n[i] >= 0 && n[i] < h->axis[i].nbins);
            if (n[i] >= 0 && n[i] < h->axis[i].nbins) {
                step /= h->axis[i].nbins;
                pos += n[i]*step;
            } else {
                return -1;
            }
        }
        ASSERT(pos < h->totalbins);
        return pos;
    }
    return -1;
}

/**
 * Returns the contents of the specified bin.
 */
HBin HIST_Get(const Histogram * h, const int i [])
{
    int pos = HIST_FlatBinIndex(h, i);
    return ((pos >= 0) ? h->bins[pos] : 0);
}

/**
 * Sets the contents of the specified bin
 */
void HIST_Set(Histogram * h, const int i [], HBin value)
{
    int pos = HIST_FlatBinIndex(h, i);
    if (pos >= 0) {
        HBin oldval = h->bins[pos];
        if (oldval != value) {
            h->bins[pos] = value;
            if (value == 0) {
                h->usedbins--;
            } else if (h->flags & H_MAX_VALUE) {
                if (value > h->maxval) {
                    /* update maximum value */
                    h->maxval = value;
                } else if (oldval == h->maxval) {
                    /* maximum value is unknown */
                    h->flags &= ~H_MAX_VALUE;
                }
            }
        }
    }
}

/**
 * Returns the maximum bin value. Note that in some cases this function
 * has to calculate this value, i.e. h->maxval does not always contains
 * the right value.
 */
HBin HIST_MaxValue(const Histogram * h)
{
    if (!(h->flags & H_MAX_VALUE)) {
        int count = 0;
        const HBin * p;
        HBin maxval = 0;
        for (p = h->bins; count<h->usedbins; p++) {
            if (*p) {
                if (*p > maxval) maxval = *p;
                count++;
            }
        }
        ((Histogram*)h)->maxval = maxval;
        ((Histogram*)h)->flags |= H_MAX_VALUE;
    }
    return h->maxval;
}

/*==========================================================================*
 *               ONE DIMENSIONAL HISTOGRAM
 *==========================================================================*/

/**
 * Allocates and initializes one-dimensional histogram
 */
Hist1D * HIST1D_Create(Str s, int n, double min, double max)
{
    Hist1D * h = MEM_New(Hist1D);
    if (h) {
        if (HIST1D_Init(h, s, n, min, max)) {
            return h;
        }
        MEM_Free(h);
    }
    return  NULL;
}

/**
 * Initializes one-dimensional histogram
 */
Bool HIST1D_Init(Hist1D * h, Str s, int n, double min, double max)
{
    return HIST_Init(&h->hist, s, 1, &n, &min, &max);
}

/**
 * Deallocates all the memory referenced by the histogram.
 */
void HIST1D_Destroy(Hist1D * h)
{
    HIST_Destroy(&h->hist);
}

/**
 * Deallocates all the memory referenced by the histogram and deallocates 
 * the histogram itself. The histogram is assumed to have been created 
 * with HIST1D_Create() call.
 */
void HIST1D_Delete(Hist1D * h)
{
    if (h) {
        HIST1D_Destroy(h);
        MEM_Free(h);
    }
}

/**
 * Tests if the specified value falls into the histogram range
 */
Bool HIST1D_IsIn(const Hist1D * h, double value)
{
    if (value >= h->hist.axis[0].min && value < h->hist.axis[0].max) {
        return True;
    } else {
        return False;
    }
}

/**
 * Adds another value to the statistics. If the value is outside of the
 * histogram's range, incremented the "missed" count, otherwise increments
 * the "total" count and the value of the appropriate bin. Returns True is
 * the value was counted, False if overflow has occured.
 */
Bool HIST1D_Put(Hist1D * h, double value)
{
    return HIST_Put(&h->hist, &value);
}

/**
 * Puts multiple values into the histogram. Returns True on success, False if
 * an overflow occurs.
 */
Bool HIST1D_PutAll(Hist1D * h, const double * values, int count)
{
    int i;
    for (i=0; i<count; i++) {
        if (!HIST1D_Put(h, values[i])) {
            return False;
        }
    }
    return True;
}

/**
 * Returns the contents of the ith bin.
 */
HBin HIST1D_Get(const Hist1D * h, int i)
{
    ASSERT(i >= 0);
    ASSERT(i < h->hist.axis[0].nbins);
    if (i >= 0 && i < h->hist.axis[0].nbins) {
        return h->hist.bins[i];
    }
    ASSMSG2("HIST1D: Index %d out of bounds 0..%d",i,h->hist.axis[0].nbins-1);
    return 0;
}

/**
 * Sets the value of the i-th bin
 */
void HIST1D_Set(Hist1D * h, int i, HBin value)
{
    HIST_Set(&h->hist, &i, value);    
}

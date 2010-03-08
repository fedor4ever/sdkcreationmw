/*
 * $Id: s_fzip.c,v 1.20 2005/03/11 22:31:37 slava Exp $
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

#include "s_util.h"
#include "s_fio.h"
#include "s_mem.h"

#include <zlib.h>

/* OS code part of GZ file header */
#ifdef _WIN32
#  define OS_CODE 0x0b
#elif defined(_UNIX)
#  define OS_CODE 0x03
#else
#  define OS_CODE 0x00
#endif

/* default input/output buffer size */
#define Z_BUFSIZE 16384

/*==========================================================================*
 *              G Z I P P E D     F I L E    I O
 *==========================================================================*/

typedef struct _Zip {
    File file;              /* shared File structure */
    File * f;               /* the file performing actual IO */
    int bufsize;            /* input/output buffer size */
    z_stream * in;          /* zlib input context */
    z_stream * out;         /* zlib output context */
    I8u * inbuf;            /* input buffer */
    I8u * outbuf;           /* output buffer */
    I32u  incrc;            /* crc32 of uncompressed data */
    I32u  outcrc;           /* crc32 of compressed data */
    int   zflags;           /* flags, see below */

#define ZIP_IN      0x0001  /* compress input stream */
#define ZIP_OUT     0x0002  /* compress output stream */
#define ZIP_ZHDR    0x0008  /* read and write ZLIB header */
#define ZIP_GZIP    0x0004  /* read and write GZIP header */
#define ZIP_FINISH  0x0010  /* we have finished writing */
#define ZIP_EOF     0x0020  /* end of file reported to the caller */
#define ZIP_IN_EOF  0x0040  /* low level end of file */
#define ZIP_IN_END  0x0080  /* end of decompression */
#define ZIP_IN_ERR  0x0100  /* input error */
#define ZIP_OUT_ERR 0x0200  /* output error */

} Zip;

/* gzip flag byte */
#define GZ_ASCII_FLAG  0x01 /* bit 0 set: file probably ascii text */
#define GZ_HEAD_CRC    0x02 /* bit 1 set: header CRC present */
#define GZ_EXTRA_FIELD 0x04 /* bit 2 set: extra field present */
#define GZ_ORIG_NAME   0x08 /* bit 3 set: original file name present */
#define GZ_COMMENT     0x10 /* bit 4 set: file comment present */
#define GZ_RESERVED    0xE0 /* bits 5..7: reserved */

STATIC int GzMagic[2] = {0x1f, 0x8b};   /* gzip magic header */

/**
 * Memory allocation/deallocation functions for use by zlib
 */
STATIC voidpf ZipMemAlloc(voidpf opaque, uInt items, uInt size)
{
    UNREF(opaque);
    return MEM_Alloc(items * size);
}

STATIC void ZipMemFree(voidpf opaque, voidpf address)
{
    UNREF(opaque);
    MEM_Free(address);
}

/**
 * Converts File * pointer into Zip * pointer
 */
STATIC Zip * ZipCast(File * f)
{
    ASSERT(f);
    if (f) {
        ASSERT(f->io == &ZipIO);
        if (f->io == &ZipIO) {
            return CAST(f,Zip,file);
        }
    }
    return NULL;
}

/**
 * Reads a byte from a gz_stream; update next_in and avail_in. 
 * Return zipEOF for end of file. Assumes that the stream zf has been 
 * sucessfully opened for reading.
 */
STATIC int ZipGetByte(Zip * zf)
{
    if (!(zf->zflags & (ZIP_IN_ERR | ZIP_IN_EOF))) {
        if (zf->in->avail_in == 0) {
            int nbytes = FILE_Read(zf->f, zf->inbuf, zf->bufsize);
            if (nbytes == 0) {
                zf->zflags |= ZIP_IN_EOF;
                return EOF;
            } else if (nbytes < 0) {
                zf->zflags |= ZIP_IN_ERR;
                return EOF;
            }
            zf->in->avail_in = nbytes;
            zf->in->next_in = zf->inbuf;
        }
        zf->in->avail_in--;
        return *((zf->in->next_in)++);
    }
    return EOF;
}

/**
 * Push the most recently read byte back into ZLIB input buffer
 */
STATIC Bool ZipPushBack(Zip * zf)
{
    ASSERT(zf->in->next_in > zf->inbuf);
    if (zf->in->next_in > zf->inbuf) {
        zf->in->next_in--;
        zf->in->avail_in++;
        return True;
    }
    return False;
}

/**
 * Checks the gzip header of a zf stream opened for reading.  Assumes that 
 * the stream zf has already been created sucessfully; zf->in->avail_in is 
 * zero for the first time, but may be non-zero for concatenated .gz files.
 */
STATIC Bool ZipSkipHeader(Zip * zf)
{
    int method; /* method byte */
    int flags;  /* flags byte */
    int len;    /* buyes read so far */
    int c;

    /* check the gzip magic header */
    if ((c = ZipGetByte(zf)) != GzMagic[0]) {
        ZipPushBack(zf);
        TRACE("ZIP: no magic[0]\n");
        return False;
    } else if ((c = ZipGetByte(zf)) != GzMagic[1]) {
        ZipPushBack(zf);
        ZipPushBack(zf);
        TRACE("ZIP: no magic[1]\n");
        return False;
    }

    method = ZipGetByte(zf);
    flags = ZipGetByte(zf);
    if (method != Z_DEFLATED || (flags & GZ_RESERVED) != 0) {
        TRACE("ZIP: unsupported method/flags\n");
        return False;
    }

    /* discard time, xflags and OS code: */
    for (len = 0; len<6; len++) (void)ZipGetByte(zf);

    /* skip the extra field */
    if ((flags & GZ_EXTRA_FIELD) != 0) { 
        len  =  ZipGetByte(zf);
        len +=  ZipGetByte(zf)<<8;

        /* len is garbage if EOF but the loop below will quit anyway */
        while (len-- != 0 && ZipGetByte(zf) != EOF) NOTHING;
    }

    /* skip the original file name */
    if ((flags & GZ_ORIG_NAME) != 0) { 
        while ((c = ZipGetByte(zf)) != 0  && c != EOF) NOTHING;
    }

    /* skip the .gz file comment */
    if ((flags & GZ_COMMENT) != 0) {   
        while ((c = ZipGetByte(zf)) != 0  && c != EOF) NOTHING;
    }

    /* skip the header crc */
    if ((flags & GZ_HEAD_CRC) != 0) {  
        for (len=0; len<2; len++) (void)ZipGetByte(zf);
    }

    return BoolValue((zf->zflags & (ZIP_IN_ERR | ZIP_IN_EOF)) == 0);
}

/**
 * Initializes the input zlib context 
 */
STATIC Bool ZipInitIn(Zip * zf)
{

    /* allocate buffer */
    zf->inbuf = (I8u*)MEM_Alloc(zf->bufsize);
    if (zf->inbuf) {

        /* allocate zlib context */
        zf->in = MEM_New(z_stream);
        if (zf->in) {
            int bits = ((zf->zflags & ZIP_ZHDR) ? (-MAX_WBITS) : MAX_WBITS);

            /* tell zlib to use our memory allocation functions */
            memset(zf->in, 0, sizeof(*zf->in));
            zf->in->zalloc = ZipMemAlloc;
            zf->in->zfree = ZipMemFree;
            zf->in->next_in = zf->inbuf;
            if (inflateInit2(zf->in, bits) == Z_OK) {

                /* skip .gz header */
                if (!(zf->zflags & ZIP_GZIP) || ZipSkipHeader(zf)) {
                    return True;
                }
                inflateEnd(zf->in);
            }
            MEM_Free(zf->in);
            zf->in = NULL;
        }
        MEM_Free(zf->inbuf);
        zf->inbuf = NULL;
    }
    zf->zflags |= ZIP_IN_ERR;
    return False;
}

/**
 * Reads and uncompresses up to len bytes from the compressed stream.
 * Returns -1 in case of error, 0 if end of file has been reached (-1 on
 * subequent calls)
 */
STATIC int ZipRead(File * f, void * buf, int len)
{
    Zip * zf = ZipCast(f);

    /* check if input is transparent */
    if (!(zf->zflags & ZIP_IN)) {
        int nbytes = FILE_Read(zf->f, buf, len);
        if (nbytes == 0) {
            zf->zflags |= (ZIP_IN_EOF | ZIP_EOF);
        } else if (nbytes < 0) {
            zf->zflags |= ZIP_IN_ERR;
        }
        return nbytes;
    }

    /* check for error or end-of-file condition */
    if (zf->zflags & (ZIP_IN_ERR | ZIP_EOF)) {
        return -1;
    }

    /* report end-of-file condition to the caller */
    if (zf->zflags & ZIP_IN_END) {
        zf->zflags |= ZIP_EOF;
        return 0;
    }

    /* check if we can read from the low level file */
    if (!FILE_AllowsReads(zf->f)) {
        zf->zflags |= ZIP_IN_ERR;
        return -1;
    }

    /* "lazy" allocation of the input context */
    if (!zf->in) {
        if (!ZipInitIn(zf)) {
            return -1;
        }
    }

    /* read and inflate the data */
    zf->in->next_out = (Bytef*)buf;
    zf->in->avail_out = len;
    while (zf->in->avail_out > 0 && !(zf->zflags & (ZIP_IN_ERR|ZIP_IN_END))) {
        int zerr;
        if (zf->in->avail_in == 0 && !(zf->zflags & ZIP_IN_EOF)) {
            int nbytes = FILE_Read(zf->f, zf->inbuf, zf->bufsize);
            if (nbytes == 0) {
                zf->zflags |= ZIP_IN_EOF;
            } else if (nbytes < 0) {
                zf->zflags |= ZIP_IN_ERR;
                break;
            }
            zf->in->avail_in = nbytes;
            zf->in->next_in = zf->inbuf;
        }
        zerr = inflate(zf->in, Z_NO_FLUSH);
        if (zerr == Z_STREAM_END) {
            inflateReset(zf->in);
            zf->zflags |= ZIP_IN_END;
        } else  if (zerr != Z_OK) {
            zf->zflags |= ZIP_IN_ERR;
            break;
        }
    }

    /* 
     * return error or end-of-file condition if nothing has been 
     * decompressed, otherwise return the amount we have successfully
     * inflated.
     */
    if (len == (int)zf->in->avail_out) {
        if (zf->zflags & ZIP_IN_EOF) {
            zf->zflags |= ZIP_EOF;
            return 0;
        }
        if (zf->zflags & ZIP_IN_ERR) {
            return -1;
        }
        /* what would this mean? there's no data and yet it's not an error */
        zf->zflags |= ZIP_EOF;
        return 0;
    } else {
        zf->incrc = crc32(zf->incrc, (Bytef*)buf, len - zf->in->avail_out);
        return (len - zf->in->avail_out);
    }
}

/**
 * Initializes the output zlib context 
 */
STATIC Bool ZipInitOut(Zip * zf)
{

    /* allocate buffer */
    zf->outbuf = (I8u*)MEM_Alloc(zf->bufsize);
    if (zf->outbuf) {

        /* allocate zlib context */
        zf->out = MEM_New(z_stream);
        if (zf->out) {
            int zerr;
            int bits = ((zf->zflags & ZIP_ZHDR) ? (-MAX_WBITS) : MAX_WBITS);

            /* tell zlib to use our memory allocation functions */
            memset(zf->out, 0, sizeof(*zf->out));
            zf->out->zalloc = ZipMemAlloc;
            zf->out->zfree = ZipMemFree;

            /* windowBits is passed < 0 to suppress zlib header */
            zerr = deflateInit2(zf->out, Z_BEST_COMPRESSION,
                                         Z_DEFLATED, bits, 8,
                                         Z_DEFAULT_STRATEGY);

            if (zerr == Z_OK) {
                if (zf->zflags & ZIP_GZIP) {
                    /* write a very simple .gz header */
                    I8u hdr[10];
                    memset(hdr, 0, sizeof(hdr));
                    hdr[0] = (I8u)GzMagic[0];
                    hdr[1] = (I8u)GzMagic[1];
                    hdr[2] = Z_DEFLATED;
                    hdr[9] = OS_CODE;
                    if (FILE_Write(zf->f,hdr,sizeof(hdr)) == sizeof(hdr)) {
                        FILE_Flush(zf->f);
                        zf->out->next_out = zf->outbuf;
                        zf->out->avail_out = zf->bufsize;
                        return True;
                    }
                } else {
                    /* not writing the header */
                    zf->out->next_out = zf->outbuf;
                    zf->out->avail_out = zf->bufsize;
                    return True;
                }
                deflateEnd(zf->out);
            }
            MEM_Free(zf->out);
            zf->out = NULL;
        }
        MEM_Free(zf->outbuf);
        zf->outbuf = NULL;
    }
    zf->zflags |= ZIP_OUT_ERR;
    return False;
}

/**
 * Compresses and writes len bytes
 */
STATIC int ZipWrite(File * f, const void * buf, int len)
{
    Zip * zf = ZipCast(f);

    /* check if output is transparent */
    if (!(zf->zflags & ZIP_OUT)) {
        int nbytes = FILE_Write(zf->f, buf, len);
        if (nbytes < 0) zf->zflags |= ZIP_OUT_ERR;
        return nbytes;
    }

    /* check for errors (ZIP_OUT_ERR) and caller's stupidity (ZIP_FINISH) */
    ASSERT(!(zf->zflags & ZIP_FINISH));
    if (zf->zflags & (ZIP_OUT_ERR | ZIP_FINISH)) {
        return -1;
    }

    /* check if we can write to the low level file */
    if (!FILE_AllowsWrites(zf->f)) {
        zf->zflags |= ZIP_OUT_ERR;
        return -1;
    }

    /* "lazy" allocation of the input context */
    if (!zf->out) {
        if (!ZipInitOut(zf)) {
            return -1;
        }
    }

    /* deflate and write data */
    zf->out->next_in = (Bytef*)buf;
    zf->out->avail_in = len;
    while (zf->out->avail_in != 0) {
        if (zf->out->avail_out == 0) {
            zf->out->next_out = zf->outbuf;
            if (FILE_Write(zf->f, zf->outbuf, zf->bufsize) != zf->bufsize) {
                zf->zflags |= ZIP_OUT_ERR;
                return -1;
            }
            zf->out->avail_out = zf->bufsize;
        }
        if (deflate(zf->out, Z_NO_FLUSH) != Z_OK) break;
    }
    zf->outcrc = crc32(zf->outcrc, (const Bytef *)buf, len);
    return (len - zf->out->avail_in);
}

/**
 * Sends all the data compressed so far to the underlying stream.
 */
STATIC Bool ZipFlush2(Zip * zf, int flush)
{
    if (zf->out) {
        int zerr = Z_OK;
        Bool done = False;

        ASSERT(!zf->out->avail_in);
        zf->out->avail_in = 0; /* should be zero already anyway */

        for (;;) {
            int len = zf->bufsize - zf->out->avail_out;
            if (len != 0) {
                if (FILE_Write(zf->f, zf->outbuf, len) != len) {
                    zf->zflags |= ZIP_OUT_ERR;
                    return False;
                }

                zf->out->next_out = zf->outbuf;
                zf->out->avail_out = zf->bufsize;
            }

            if (done) break;
            zerr = deflate(zf->out, flush);

            /* ignore the second of two consecutive flushes */
            if (len == 0 && zerr == Z_BUF_ERROR) zerr = Z_OK;

            /* deflate has finished flushing only when it hasn't used 
             * up all the available space in the output buffer
             */
            if (zf->out->avail_out || zerr == Z_STREAM_END) {
                done = True;
            }
            if (zerr != Z_OK && zerr != Z_STREAM_END) break;
        }
        return BoolValue(FILE_Flush(zf->f) && (zerr == Z_STREAM_END));
    }
    return True;
}

STATIC Bool ZipFlush(File * f)
{
    Zip * zf = ZipCast(f);
    return (zf ? ZipFlush2(zf, Z_SYNC_FLUSH) : False);
}

STATIC Bool ZipEof(File * f)
{
    Zip * zf = ZipCast(f);
    return (zf ? FILE_Eof(zf->f) : False);
}

STATIC File * ZipTarget(File * f)
{
    Zip * zf = ZipCast(f);
    return (zf ? zf->f : NULL);
}

STATIC void ZipDetach2(Zip * zf)
{
    if (zf->f) {
        if (zf->out && !(zf->zflags & ZIP_FINISH)) {
            ZipFlush2(zf, Z_FINISH);
        }
        zf->f = NULL;
    }
    if (zf->in) {
        inflateEnd(zf->in);
        MEM_Free(zf->in);
        zf->in = NULL;
    }
    if (zf->out) {
        deflateEnd(zf->out);
        MEM_Free(zf->out);
        zf->out = NULL;
    }
    if (zf->inbuf) {
        MEM_Free(zf->inbuf);
        zf->inbuf = NULL;
    }
    if (zf->outbuf) {
        MEM_Free(zf->outbuf);
        zf->outbuf = NULL;
    }
}

STATIC void ZipDetach(File * f)
{
    Zip * zf = ZipCast(f);
    if (zf) ZipDetach2(zf);
}

/**
 * Flushes the deflator and the underlying stream. 
 * Closes the low level stream. No I/O is possible 
 * after this function has been invoked.
 */
STATIC void ZipClose(File * f)
{
    Zip * zf = ZipCast(f);
    if (zf) {
        if (zf->f) {
            if (zf->out && !(zf->zflags & ZIP_FINISH)) {
                ZipFlush2(zf, Z_FINISH);
            }
            FILE_Close(zf->f);
            zf->f = NULL;
        }
        ZipDetach2(zf); /* deallocates buffers */
    }
}

STATIC void ZipFree(File * f)
{
    Zip * zf = ZipCast(f);
    if (zf) {
        ASSERT(!(f->flags & FILE_IS_OPEN));
        MEM_Free(zf);
    }
}

/**
 * Creates a new Zip IO context.
 * Compresses output data and decompresses input data.
 */
File * FILE_Zip(File * f, int flags)
{
    ASSERT(f);
    ASSERT(!(flags & (~0x000f)));
    if (f) {
        Zip * zf = MEM_New(Zip);
        if (zf) {
            memset(zf, 0, sizeof(*zf));
            if (FILE_Init(&zf->file, TEXT("zip"), True, &ZipIO)) {
                zf->f = f;
                zf->bufsize = Z_BUFSIZE;
                if (flags & FILE_ZIP_IN) zf->zflags |= ZIP_IN;
                if (flags & FILE_ZIP_OUT) zf->zflags |= ZIP_OUT;
                if (flags & FILE_ZIP_ZHDR) zf->zflags |= ZIP_ZHDR;
                if (flags & FILE_ZIP_GZIP) zf->zflags |= ZIP_GZIP;
                return &zf->file;
            }
            MEM_Free(zf);
        }
    }
    return NULL;
}

/**
 * Finishes deflation by calling deflate(out,Z_FINISH)
 * No more deflation is possible after this call.
 */
Bool FILE_ZipFinish(File * f)
{
    Zip * zf = ZipCast(f);
    if (zf) {
        ASSERT(!(zf->zflags & ZIP_FINISH));
        if (!(zf->zflags & ZIP_FINISH)) {
            ZipFlush2(zf, Z_FINISH);
            zf->zflags |= ZIP_FINISH;
            return True;
        }
    }
    return False;
}

/*
 * A set of handlers that perform file I/O
 */
const FileIO ZipIO = {
    False               /* fileIO   */,
    NULL                /* open     */,
    NULL                /* reopen   */,
    NULL                /* setparam */,
    ZipRead             /* read     */,
    ZipWrite            /* write    */,
    ZipFlush            /* flush    */,
    ZipEof              /* eof      */,
    ZipTarget           /* target   */,
    ZipDetach           /* detach   */,
    ZipClose            /* close    */,
    ZipFree             /* free     */
};

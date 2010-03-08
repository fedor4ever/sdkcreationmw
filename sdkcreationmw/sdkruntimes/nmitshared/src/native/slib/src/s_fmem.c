/*
 * $Id: s_fmem.c,v 1.10 2004/12/26 18:22:19 slava Exp $
 *
 * Copyright (C) 2001-2004 by Slava Monich
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

#include "s_fio.h"
#include "s_mem.h"

/*==========================================================================*
 *              I N - M E M O R Y    I O
 *==========================================================================*/

typedef struct _MemFile {
    File file;          /* shared File structure */
    Buffer * buf;       /* input/output buffer */
    int mflags;         /* flags, see below */

#define MEM_EOF  0x0001 /* end of file on input stream */
#define MEM_ERR  0x0002 /* output buffer full */
#define MEM_IN   0x0004 /* can read from the buffer */
#define MEM_OUT  0x0008 /* can write to the buffer */
#define MEM_BUF  0x0010 /* we deallocate the buffer */

} MemFile;

STATIC int  MemFileRead P_((File * f, void * buf, int len));
STATIC int  MemFileWrite P_((File * f, const void * buf, int len));
STATIC Bool MemFileFlush P_((File * f));
STATIC Bool MemFileEof P_((File * f));
STATIC void MemFileClose P_((File * f));
STATIC void MemFileFree P_((File * f));

/*
 * I/O type definition
 */
STATIC const FileIO MemFileIO = {
    False               /* fileIO   */,
    NULL                /* open     */,
    NULL                /* reopen   */,
    NULL                /* setparam */,
    MemFileRead         /* read     */,
    MemFileWrite        /* write    */,
    MemFileFlush        /* flush    */,
    MemFileEof          /* eof      */,
    NULL                /* target   */,
    NULL                /* detach   */,
    MemFileClose        /* close    */,
    MemFileFree         /* free     */
};

/*
 * Implementation
 */
STATIC MemFile * MemFileCast(File * f) 
{
    ASSERT(f);
    if (f) {
        ASSERT(f->io == &MemFileIO);
        if (f->io == &MemFileIO) {
            return CAST(f,MemFile,file);
        }
    }
    return NULL;
}

STATIC const MemFile * MemFileCastC(const File * f) 
{
    ASSERT(f);
    if (f) {
        ASSERT(f->io == &MemFileIO);
        if (f->io == &MemFileIO) {
            return CAST(f,MemFile,file);
        }
    }
    return NULL;
}

STATIC int MemFileRead(File * f, void * buf, int len)
{
    int nbytes = -1;
    MemFile * m = MemFileCast(f);
    if (m && (m->mflags & MEM_IN)) {
        if (!(m->mflags & MEM_EOF)) {
            nbytes = BUFFER_Get(m->buf, buf, len);
            if (!nbytes && BUFFER_Size(m->buf) == 0) {
                m->mflags |= MEM_EOF;
            }
        }
    }
    return nbytes;
}

STATIC int MemFileWrite(File * f, const void * buf, int len) 
{
    int nbytes = -1;
    MemFile * m = MemFileCast(f);
    if (m && (m->mflags & MEM_OUT)) {
        if (!(m->mflags & MEM_ERR)) {
            nbytes = BUFFER_Put(m->buf, buf, len, True);
            if (!nbytes && len > 0) {
                nbytes = -1;
                m->mflags |= MEM_ERR;
            } else {
                m->mflags &= ~MEM_EOF;
            }
        }
    }
    return nbytes;
}

STATIC Bool MemFileFlush(File * f) 
{
    MemFile * m = MemFileCast(f);
    return BoolValue(m && (m->mflags & MEM_OUT));
}

STATIC Bool MemFileEof(File * f) 
{
    MemFile * m = MemFileCast(f);
    return BoolValue(m && (!(m->mflags & MEM_IN) || (m->mflags & MEM_EOF)));
}

STATIC void MemFileClose(File * f) 
{
    RELEASE_UNREF(f);
    ASSERT(MemFileCast(f));
}

STATIC void MemFileFree(File * f) 
{
    MemFile * m = MemFileCast(f);
    if (m) {
        ASSERT(!(f->flags & FILE_IS_OPEN));
        if (m->mflags & MEM_BUF) BUFFER_Delete(m->buf);
        MEM_Free(m);
    }
}

/**
 * Creates the default in-memory I/O context. Allows writing to and 
 * reading from the internal buffer. Has no limit on the amount of
 * allocated data.
 */
File * FILE_Mem()
{
    MemFile * m = MEM_New(MemFile);
    if (m) {
        memset(m, 0, sizeof(*m));
        m->buf = BUFFER_Create();
        if (m->buf) {
            m->mflags = MEM_IN | MEM_OUT | MEM_BUF;
            if (FILE_Init(&m->file, TEXT("memio"), False, &MemFileIO)) {
                return &m->file;
            }
            BUFFER_Delete(m->buf);
        }
        MEM_Free(m);
    }
    return NULL;
}

/**
 * Creates a context for reading from the specified buffer.
 */
File * FILE_MemIn(const void * data, int size) 
{
    MemFile * m = MEM_New(MemFile);
    if (m) {
        memset(m, 0, sizeof(*m));
        m->buf = BUFFER_Create2((void*)data, size);
        if (m->buf) {
            m->mflags = MEM_IN | MEM_BUF;
            m->buf->flags &= ~BUFFER_OWN_DATA;
            if (FILE_Init(&m->file, TEXT("memin"), False, &MemFileIO)) {
                return &m->file;
            }
            BUFFER_Delete(m->buf);
        }
        MEM_Free(m);
    }
    return NULL;
}

/**
 * Creates a context for writing to the internal buffer.
 * If maxsize is negative, there's no limit.
 */
File * FILE_MemOut(int maxsize)
{
    MemFile * m = MEM_New(MemFile);
    if (m) {
        memset(m, 0, sizeof(*m));
        m->buf = BUFFER_Create();
        if (m->buf) {
            m->mflags = MEM_OUT | MEM_BUF;
            m->buf->maxsiz = maxsize;
            if (FILE_Init(&m->file, TEXT("memout"), False, &MemFileIO)) {
                return &m->file;
            }
            BUFFER_Delete(m->buf);
        }
        MEM_Free(m);
    }
    return NULL;
}

/**
 * Creates a context for writing to the specified buffer.
 * Will not resize the buffer.
 */
File * FILE_MemOut2(void * data, int size) 
{
    MemFile * m = MEM_New(MemFile);
    if (m) {
        memset(m, 0, sizeof(*m));
        m->buf = BUFFER_Create2(data, size);
        if (m->buf) {
            m->mflags = MEM_OUT | MEM_BUF;
            m->buf->maxsiz = size;  /* never resize */
            m->buf->flags &= ~BUFFER_OWN_DATA;
            if (FILE_Init(&m->file, TEXT("memout"), False, &MemFileIO)) {
                return &m->file;
            }
            BUFFER_Delete(m->buf);
        }
        MEM_Free(m);
    }
    return NULL;
}

/**
 * Creates an in-memory I/O context associated with an external buffer.
 * Does not deallocate the buffer if freeBuf is False. Allows both reads 
 * and writes.
 */
File * FILE_MemBuf(Buffer * buf, Bool freeBuf)
{
    MemFile * m = MEM_New(MemFile);
    ASSERT(buf);
    if (m) {
        memset(m, 0, sizeof(*m));
        m->buf = buf;
        m->mflags = MEM_IN | MEM_OUT;
        if (freeBuf) m->mflags |= MEM_BUF;
        if (FILE_Init(&m->file, TEXT("membuf"), False, &MemFileIO)) {
            return &m->file;
        }
        MEM_Free(m);
    }
    return NULL;
}

/**
 * Returns pointer to the in-memory data, NULL if this is not a MemFile
 */
void * FILE_MemData(File * f)
{
    MemFile * m = MemFileCast(f);
    if (m) {
        return BUFFER_Access(m->buf);
    } else {
        return NULL;
    }
}

/**
 * Returns the size of the data stored in the MemFile's buffer, -1 if this is
 * not a MemFile
 */
int FILE_MemSize(const File * f)
{
    const MemFile * m = MemFileCastC(f);
    if (m) {
        return BUFFER_Size(m->buf);
    } else {
        return -1;
    }
}

/**
 * Clears the internal buffer
 */
void FILE_MemClear(File * f)
{
    MemFile * m = MemFileCast(f);
    if (m) {
        BUFFER_Clear(m->buf);
        m->mflags &= ~(MEM_ERR|MEM_EOF);
    }
}

/**
 * Checks if this file is a memory I/O file
 */
Bool FILE_IsMem(const File * f)
{
    return BoolValue(f && f->io == &MemFileIO);
}

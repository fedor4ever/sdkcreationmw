/*
 * $Id: s_fio.h,v 1.8 2005/02/20 20:31:29 slava Exp $
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

#ifndef _SLAVA_FILE_IO_H_
#define _SLAVA_FILE_IO_H_ 1

#include "s_file.h"
#include "s_strbuf.h"

/*
 * "private" flags
 */
#define FILE_IS_OPEN      0x010000  /* set until closed or detached */
#define FILE_IS_ATTACHED  0x020000  /* attached to existing low-level file */

/* The value returned by FILE_Getc if it reaches the end of file */
#ifndef EOF
#  define EOF (-1)
#endif /* EOF */

/* assert that these are not public */
COMPILE_ASSERT((FILE_IS_OPEN & FILE_PUBLIC_FLAGS) == 0)
COMPILE_ASSERT((FILE_IS_ATTACHED & FILE_PUBLIC_FLAGS) == 0)

/* Max length of the open mode string (even 7 is too much...) */
#define MAX_MODE_LEN 7

/*
 * File functions. Some notes:
 *
 * 1. each FileOpen allocates File structure (with some additional fields) 
 *    FileClose closes the file (but does not deallocate the context) and 
 *    FileFree finally deallocates the context. The Close functionality
 *    is separated from Free in order to implement Finish semantics.
 *
 * 2. FileOpen only initializes its private portion of the file context.
 *    The shared portion (i.e. File structure) is initialized by FILE_Open.
 *    FileOpen may, however, initialize the whole thing to zero.
 *
 * 3. FileWrite and FileRead return number of bytes written or read. 
 *    In case of error they return -1
 *
 * 4. it's guaranteed that FileFree is the the only call that may occur 
 *    after FileClose or FileDetach
 *
 * 5. it's guaranteed that FileClose or FileDetach will always be invoked 
 *    before FileFree and that it happens no more than once per context
 */
typedef File * (*FileOpen)     P_((Str path, const char * mode));
typedef Bool   (*FileReopen)   P_((File * f, Str path, const char * mode));
typedef Bool   (*FileSetParam) P_((File * f, Str name, void * value));
typedef int    (*FileRead)     P_((File * f, void * buf, int len));
typedef int    (*FileWrite)    P_((File * f, const void * buf, int len));
typedef Bool   (*FileFlush)    P_((File * f));
typedef Bool   (*FileEof)      P_((File * f));
typedef File * (*FileTarget)   P_((File * f));
typedef void   (*FileDetach)   P_((File * f));
typedef void   (*FileClose)    P_((File * f));
typedef void   (*FileFree)     P_((File * f));

/*
 * A set of handlers that perform I/O
 */
struct _FileIO {
    Bool         fileIO;        /* True if this is a file based I/O */
    FileOpen     open;          /* open the stream */
    FileReopen   reopen;        /* reopen the stream */
    FileSetParam setparam;      /* set stream parameters */
    FileRead     read;          /* read bytes */
    FileWrite    write;         /* write bytes */
    FileFlush    flush;         /* flush output buffer */
    FileEof      eof;           /* test for end-of-file condition */
    FileTarget   target;        /* returns the File where I/O is redirected */
    FileDetach   detach;        /* detach from the lower level stream */
    FileClose    close;         /* close the stream */
    FileFree     free;          /* deallocate the stream context */
};

/*
 * A common context associated with an open file.
 */
struct _File {
    IODesc io;                  /* I/O functions */
    StrBuf name;                /* the file name */
    StrBuf buf;                 /* temporary buffer for printf */
    I8u    pushBack[4];         /* minimal pushback buffer */
    int    pushed;              /* number of bytes pushed back */
    I32u   flags;               /* flags */
};

#define CAN_READ(_f) \
    ((_f) && \
    ((_f)->flags & FILE_IS_OPEN) && \
    ((_f)->flags & FILE_CAN_READ) && \
     (_f)->io->read) 

#define CAN_WRITE(_f) \
    ((_f) && \
    ((_f)->flags & FILE_IS_OPEN) && \
    ((_f)->flags & FILE_CAN_WRITE) && \
     (_f)->io->write) 

/* Private I/O descriptors */
extern const FileIO ZipIO;
extern const FileIO WrapIO;

/* 
 * Internal functions 
 */
extern Bool FILE_Init P_((File * f, Str path, Bool attach, IODesc io));

#endif /* _SLAVA_FILE_IO_H_ */

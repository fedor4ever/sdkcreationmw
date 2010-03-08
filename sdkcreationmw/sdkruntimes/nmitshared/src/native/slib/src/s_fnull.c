/*
 * $Id: s_fnull.c,v 1.1 2005/08/23 23:10:52 slava Exp $
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

#include "s_fio.h"
#include "s_mem.h"

/*==========================================================================*
 *              D U M M Y    I O
 *==========================================================================*/

typedef struct _NullFile {
    File file;          /* shared File structure */
    Bool eof;           /* True if we have returned EOF */
} NullFile;

STATIC Bool NullFileReopen P_((File * f, Str path, const char * mode));
STATIC int  NullFileRead P_((File * f, void * buf, int len));
STATIC int  NullFileWrite P_((File * f, const void * buf, int len));
STATIC Bool NullFileFlush P_((File * f));
STATIC Bool NullFileEof P_((File * f));
STATIC void NullFileClose P_((File * f));
STATIC void NullFileFree P_((File * f));

/*
 * I/O type definition
 */
STATIC const FileIO NullFileIO = {
    False               /* fileIO   */,
    NULL                /* open     */,
    NullFileReopen      /* reopen   */,
    NULL                /* setparam */,
    NullFileRead        /* read     */,
    NullFileWrite       /* write    */,
    NullFileFlush       /* flush    */,
    NullFileEof         /* eof      */,
    NULL                /* target   */,
    NULL                /* detach   */,
    NullFileClose       /* close    */,
    NullFileFree        /* free     */
};

/*
 * Implementation
 */
STATIC Bool NullFileReopen(File * f, Str path, const char * mode)
{
    UNREF(f);
    UNREF(path);
    UNREF(mode);
    return True;
}

STATIC int NullFileRead(File * f, void * buf, int len)
{
    NullFile* nf = CAST(f,NullFile,file);
    UNREF(buf);
    UNREF(len);
    if (nf->eof) {
        return -1;
    } else {
        nf->eof = True;
        return 0;
    }
}

STATIC int NullFileWrite(File * f, const void * buf, int len) 
{
    UNREF(f);
    UNREF(buf);
    UNREF(len);
    return len;
}

STATIC Bool NullFileFlush(File * f) 
{
    UNREF(f);
    return True;
}

STATIC Bool NullFileEof(File * f) 
{
    UNREF(f);
    return True;
}

STATIC void NullFileClose(File * f) 
{
    UNREF(f);
}

STATIC void NullFileFree(File * f) 
{
    MEM_Free(CAST(f,NullFile,file));
}

/**
 * Creates a dummy file I/O structure. Consumes all data without errors,
 * does not produce any data on input.
 */
File * FILE_Null()
{
    NullFile * n = MEM_New(NullFile);
    if (n) {
        memset(n, 0, sizeof(*n));
        if (FILE_Init(&n->file, TEXT("null"), False, &NullFileIO)) {
            return &n->file;
        }
        MEM_Free(n);
    }
    return NULL;
}

/**
 * Checks if this file is a NULL file
 */
Bool FILE_IsNull(const File * f)
{
    return BoolValue(f && f->io == &NullFileIO);
}

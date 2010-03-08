/*
 * $Id: s_fio.c,v 1.8 2005/02/20 20:31:29 slava Exp $
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

#include "s_buf.h"
#include "s_fio.h"
#include "s_mem.h"

/*==========================================================================*
 *              P L A I N     F I L E    I O
 *==========================================================================*/

#undef PlainFile
typedef struct _PlainFile {
    File file;  /* shared File structure */
    FILE * f;   /* a C runtime file structure */
} PlainFile;

STATIC PlainFile * PlainFileCast(File * f)
{
    ASSERT(f);
    if (f) {
        ASSERT(f->io == &PlainFileIO);
        if (f->io == &PlainFileIO) {
            return CAST(f,PlainFile,file);
        }
    }
    return NULL;
}

STATIC File * PlainFileOpen(Str path, const char * mode)
{
    FILE * ff;
    PlainFile * pf = NULL; /* assume failure */

#undef _mode
#ifdef UNICODE
    const char * c = mode;
    int i = 0;
    wchar_t _m[MAX_MODE_LEN+1];
    while (*c && i<MAX_MODE_LEN) _m[i++] = *c++;
    ASSERT(i<MAX_MODE_LEN);
    _m[i] = 0;
#  define _mode _m
#else /* UNICODE */
#  define _mode mode
#endif /* UNICODE */

    ff = Fopen(path, _mode);
    if (ff) {
        pf = MEM_New(PlainFile);
        if (pf) {
            memset(pf, 0, sizeof(*pf));
            pf->f = ff;
        } else {
            fclose(ff);
        }
    }
    return (pf ? &pf->file : NULL);
}

#ifdef _WIN32_WCE
#  define PlainFileReopen NULL  /* not implemented on CE */
#else /* !_WIN32_WCE */
STATIC Bool PlainFileReopen(File * f, Str path, const char * mode)
{
    PlainFile * pf = PlainFileCast(f);
    if (pf) {

#undef _mode
#ifdef UNICODE
        const char * c = mode;
        int i = 0;
        wchar_t _m[MAX_MODE_LEN+1];
        while (*c && i<MAX_MODE_LEN) _m[i++] = *c++;
        ASSERT(i<MAX_MODE_LEN);
        _m[i] = 0;
#  define _mode _m
#else /* UNICODE */
#  define _mode mode
#endif /* UNICODE */

        pf->f = Freopen(path, _mode, pf->f);
        return BoolValue(pf->f != NULL);
    }
    return False;
}
#endif /* !_WIN32_WCE */

STATIC int PlainFileRead(File * f, void * buf, int len)
{
    PlainFile * pf = PlainFileCast(f);
    if (pf) {
        size_t nbytes = fread(buf, 1, len, pf->f);
        return (ferror(pf->f) ? (-1) : (int)nbytes);
    }
    return (-1);
}

STATIC int PlainFileWrite(File * f, const void * buf, int len)
{
    PlainFile * pf = PlainFileCast(f);
    if (pf) {
        size_t nbytes = fwrite(buf, 1, len, pf->f);
        return (ferror(pf->f) ? (-1) : (int)nbytes);
    }
    return (-1);
}

STATIC Bool PlainFileFlush(File * f)
{
    PlainFile * pf = PlainFileCast(f);
    return BoolValue(pf && (fflush(pf->f) != EOF));
}

STATIC Bool PlainFileEof(File * f)
{
    PlainFile * pf = PlainFileCast(f);
    return BoolValue(pf && feof(pf->f));
}

STATIC void PlainFileDetach(File * f)
{
    PlainFile * pf = PlainFileCast(f);
    if (pf) pf->f = NULL;
}

STATIC void PlainFileClose(File * f)
{
    PlainFile * pf = PlainFileCast(f);
    if (pf) {
        if (pf->f) {
            fclose(pf->f);
            pf->f = NULL;
        }
    }
}

STATIC void PlainFileFree(File * f)
{
    PlainFile * pf = PlainFileCast(f);
    if (pf) {
        ASSERT(!(f->flags & FILE_IS_OPEN));
        MEM_Free(pf);
    }
}

/**
 * Creates a File object for the given C runtime FILE structure.
 */
File * FILE_AttachToFile(FILE * file, Str name)
{
    if (file) {
        PlainFile * pf = MEM_New(PlainFile);
        if (pf) {
            memset(pf, 0, sizeof(*pf));
            pf->f = file;
            if (FILE_Init(&pf->file, name, True, &PlainFileIO)) {
                return &pf->file;
            }
            MEM_Free(pf);
        }
    }
    return NULL;
}

/*
 * A set of handlers that perform file I/O
 */
const FileIO PlainFileIO = {
    True                /* fileIO   */,
    PlainFileOpen       /* open     */,
    PlainFileReopen     /* reopen   */,
    NULL                /* setparam */,
    PlainFileRead       /* read     */,
    PlainFileWrite      /* write    */,
    PlainFileFlush      /* flush    */,
    PlainFileEof        /* eof      */,
    NULL                /* target   */,
    PlainFileDetach     /* detach   */,
    PlainFileClose      /* close    */,
    PlainFileFree       /* free     */
};

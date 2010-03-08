/*
 * $Id: s_fzio.c,v 1.9 2005/05/14 06:38:53 slava Exp $
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

/*==========================================================================*
 *              G Z I P P E D     F I L E    I O
 *==========================================================================*/

#undef GZipFile
typedef struct _GZipFile {
    File file;      /* shared File structure */
    gzFile f;       /* a zlib file context */
} GZipFile;

STATIC GZipFile * GZipFileCast(File * f)
{
    ASSERT(f);
    if (f) {
        ASSERT(f->io == &GZipFileIO);
        if (f->io == &GZipFileIO) {
            return CAST(f,GZipFile,file);
        }
    }
    return NULL;
}

STATIC File * GZipFileOpen(Str path, const char * mode)
{
    gzFile ff = (gzFile)0;
    GZipFile * zf = NULL; /* assume failure */

#ifdef UNICODE
    char * m = NULL;
#endif /* UNICODE */

    StrBuf16 buf;
    STRBUF_InitBufXXX(&buf);

#ifdef _WIN32
    /* replace "t" with "b" under Win32 */
    if (mode && (strchr(mode,'t') || !strchr(mode,'b'))) {
        Bool failed = False;
        Bool b = False;
        const char * s = mode;
        Char ch;
        while ((ch = *s++) != 0) {
            if (ch == 't') ch = 'b';
            if (ch != 'b' || !b) {
                if (!STRBUF_AppendChar(&buf.sb, ch)) {
                    failed = True;
                    break;
                }
                if (ch == 'b') b = True;
            }
        }
        if (!failed && !b && !STRBUF_AppendChar(&buf.sb, 'b')) {
            failed = True;
        }
        if (failed) {
            mode = (strchr(mode,'w') ? WRITE_BINARY_MODE : READ_BINARY_MODE);
        } else {
#  ifdef UNICODE
            mode = m = STRING_ToMultiByte(buf.sb.s);
#  else /* UNICODE */
            mode = buf.sb.s;
#  endif /* UNICODE */
        }
    }
#else  /* !_WIN32 */
    /* make sure "b" mode is specified */
    if (mode && !strchr(mode,'b')) {
        if (STRBUF_Copy(&buf.sb, mode) && 
            STRBUF_AppendChar(&buf.sb, 'b')) {
#  ifdef UNICODE
            mode = m = STRING_ToMultiByte(buf.sb.s);
#  else /* UNICODE */
            mode = buf.sb.s;
#  endif /* UNICODE */
        } else {
            mode = (strchr(mode,'w') ? WRITE_BINARY_MODE : READ_BINARY_MODE);
        }
    }
#endif /* _WIN32 */

    if (mode) {
        /* cannot use "append" mode on gzipped files */
        ASSERT(!strchr(mode,'a'));

        /* open the file */
#ifdef UNICODE
        {
            char * p = STRING_ToMultiByte(path);
            if (p) {
                ff = gzopen(p, m);
                MEM_Free(p);
            }
        }
#else /* UNICODE */
        ff = gzopen(path, mode);
#endif /* UNICODE */

        if (ff) {
            zf = MEM_New(GZipFile);
            if (zf) {
                memset(zf, 0, sizeof(*zf));
                zf->f = ff;
            } else {
                gzclose(ff);
            }
        }
    }

#ifdef UNICODE
    MEM_Free(m);
#endif /* UNICODE */
    STRBUF_Destroy(&buf.sb);
    return (zf ? &zf->file : NULL);
}

STATIC Bool GZipFileReopen(File * f, Str path, const char * mode)
{
    GZipFile * zf = GZipFileCast(f);
    if (zf) {
        File * f2;
        gzclose(zf->f);
        f2 = GZipFileOpen(path, mode);
        if (f2) {
            File save = *f;
            GZipFile * zf2 = GZipFileCast(f2);
            *zf = *zf2;
            *f = save;
            MEM_Free(zf2);
            return True;
        }
    }
    return False;
}

STATIC int GZipFileRead(File * f, void * buf, int len)
{
    int nbytes = 0;
    if (len  > 0) {
        GZipFile * zf = GZipFileCast(f);
        ASSERT(buf);        
        nbytes = gzread(zf->f, buf, len);
    }
    return nbytes;
}

STATIC int GZipFileWrite(File * f, const void * buf, int len)
{
    int nbytes = 0;
    if (len > 0) {
        GZipFile * zf = GZipFileCast(f);
        ASSERT(buf);

        /* 
         * I need the cast below because apparently "const voidp" and
         * "const void*" are different things. It's very much possible 
         * that zlib authors actually meant "const void*". 
         */
        nbytes = gzwrite(zf->f, (const voidp) buf, len);
    }
    return nbytes;
}

STATIC Bool GZipFileFlush(File * f)
{
    GZipFile * zf = GZipFileCast(f);
    if (zf) {
        return BoolValue(gzflush(zf->f, Z_SYNC_FLUSH) == Z_OK);
    } else {
        return False;
    }
}

STATIC Bool GZipFileEof(File * f)
{
    GZipFile * zf = GZipFileCast(f);
    if (zf) {
        return BoolValue(gzeof(zf->f));
    } else {
        return False;
    }
}

STATIC void GZipFileDetach(File * f)
{
    GZipFile * zf = GZipFileCast(f);
    if (zf) zf->f = NULL;
}

STATIC void GZipFileClose(File * f)
{
    GZipFile * zf = GZipFileCast(f);
    if (zf) {
        if (zf->f) {
            gzclose(zf->f);
            zf->f = NULL;
        }
    }
}

STATIC void GZipFileFree(File * f)
{
    GZipFile * zf = GZipFileCast(f);
    if (zf) {
        ASSERT(!(f->flags & FILE_IS_OPEN));
        MEM_Free(zf);
    }
}

/*
 * A set of handlers that perform file I/O
 */
const FileIO GZipFileIO = {
    True                /* fileIO   */,
    GZipFileOpen        /* open     */,
    GZipFileReopen      /* reopen   */,
    NULL                /* setparam */,
    GZipFileRead        /* read     */,
    GZipFileWrite       /* write    */,
    GZipFileFlush       /* flush    */,
    GZipFileEof         /* eof      */,
    NULL                /* target   */,
    GZipFileDetach      /* detach   */,
    GZipFileClose       /* close    */,
    GZipFileFree        /* free     */
};

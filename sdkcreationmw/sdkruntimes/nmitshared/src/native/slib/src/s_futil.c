/*
 * $Id: s_futil.c,v 1.13 2006/04/13 12:15:39 slava Exp $
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

#include "s_util.h"
#include "s_random.h"
#include "s_iop.h"
#include "s_fio.h"
#include "s_mem.h"

/**
 * Returns pointer to buffer containing directory part of the given path.
 * The returned string is either empty or ends with file separator character. 
 * Under Win32, if may also look like "C:", but in either case file name 
 * can be simply appended to it. The return buffer has 'extra' additional 
 * characters allocated past the end of the string. The function only 
 * returns NULL if input parameter is NULL or memory allocation fails. 
 */
Char * FILE_DirName(Str path, int extra) 
{
    Char * dir = NULL;
    ASSERT(path);
    ASSERT(extra >= 0);
    if (path) {
        int len = FILE_FilePart(path) - path;
        size_t size = len + 1;
        if (extra >= 0) size += extra;
        dir = MEM_NewArray(Char,size);

        /* copy portion of the source path */
        if (dir) {
            if (len > 0) StrnCpy(dir, path, len);
            dir[len] = 0;
        }
    }
    return dir;
}

/**
 * Generates random file name consisting of n random characters.
 * Returns pointer to the provided buffer. The buffer must be at 
 * least (n+1) characters big.
 */
Char * FILE_TempName(Char * buf, int n) 
{
    ASSERT(buf);
    if (buf) {
        int i;
        int nc = 'Z'-'A'+1;
        int nc2 = 2 * nc;
        for (i=0; i<n; i++) {
            int r = RAND_NextInt(nc2);
            buf[i] = (Char)(((r >= nc) ? ('A'+r-nc) : ('a'+r)));
        }
        buf[i] = 0;
    }
    return buf;
}

/**
 * Attempts to open a regular file in specified mode. 
 * PORTABILITY NOTE: on Unix it's possible to open a directory file,
 * while on Win32 fopen() cannot read directories. Therefore, this
 * function cannot be used as a check for existence or non-existence 
 * of a file. Use FILE_Exist() and FILE_NonExist() for that purpose
 */
Bool FILE_CanOpen(Str fname, const char * mode) 
{
    FILE * f;
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

    f = Fopen(fname, _mode);
    if (f) {
        fclose(f);
        return True;
    }
    return False;
}

/**
 * FILE_RmDir callback. Recursively destroys the directory contents
 */
STATIC Bool RmDirCB(Str dir, Str fname, void * ctx) 
{
    Bool ok = False;
    StrBuf64 path;
    UNREF(ctx);
    STRBUF_InitBufXXX(&path);
    if (STRBUF_Copy(&path.sb, dir) && 
        STRBUF_AppendChar(&path.sb, FILE_SEPARATOR_CHAR) &&
        STRBUF_Append(&path.sb, fname)) {
        ok = True;
        if (FILE_IsDir(path.sb.s)) {
            FILE_RmDir(path.sb.s, True);
        } else {
            FILE_Delete(path.sb.s);
        }
    }
    STRBUF_Destroy(&path.sb);
    return ok;
}

/**
 * Creates a directory hierarhy.
 */
Bool FILE_MkDir(Str dir) 
{
    Bool ok = False;
    StrBuf64 entry;
    StrBuf* sb = &entry.sb;
    STRBUF_InitBufXXX(&entry);

    if (STRBUF_Copy(sb, dir)) {
        while (sb->len > 0 && FILE_IsFileSeparator(STRBUF_LastChar(sb))) {
            STRBUF_SetLength(sb, sb->len-1);
        }

        /* check if the directory already exists */
        if (FILE_IsDir(STRBUF_Text(sb)) ||
            FILE_CreateDir(STRBUF_Text(sb))) {
            ok = True;

        } else {

            /* directory does not exists, walk the hierarhy */
            int pos = 0;
            int next = 0;
            while ((next = FILE_FindSeparator(dir+pos)) >= 0) {
                STRBUF_Clear(sb);
                if (next == 0) {
                    pos++;
                    continue;
                } else {
                    if (!STRBUF_CopyN(sb,dir,pos+next) || 
                        !FILE_CreateDir(STRBUF_Text(sb))) {
                        break;
                    }
                    pos += next + 1;
                }
            }
                
            /* final test */
            if (STRBUF_Copy(sb, dir)) {
                while (sb->len && FILE_IsFileSeparator(STRBUF_LastChar(sb))) {
                    STRBUF_SetLength(sb, sb->len-1);
                }
                if (FILE_IsDir(STRBUF_Text(sb)) ||
                    FILE_CreateDir(STRBUF_Text(sb))) {
                    ok = True;
                }
            }
        }
    }

    STRBUF_Destroy(sb);
    return ok;
}

/**
 * Removes the directory. If recurse parameter is True, also removes 
 * all files and subdirectories, otherwise it fails if directory is not
 * empty.
 */
Bool FILE_RmDir(Str dir, Bool recurse) 
{
    if (FILE_IsDir(dir)) {
        if (recurse) {
            FILE_List(dir, RmDirCB, NULL);
        }
#ifdef _WIN32
        return RemoveDirectory(dir);
#else
        return BoolValue(Rmdir(dir) == 0);
#endif /* WIN32 */
    }
    return False;
}

/**
 * Generates unique file name. Makes sure such file does not exist.
 * There's still non-zero probability that two processes would step 
 * on each other, but hell with it :)
 */
void FILE_MakeUnique(Char * name, int fixedPart, int randomPart) 
{
    Bool exists;
    do {
        FILE_TempName(name + fixedPart, randomPart);
        exists = BoolValue(!FILE_NonExist(name));
    } while (exists);
}

/**
 * Save something into the file. This function does it carefully, saving 
 * data into temporary file X, then renaming X into the specified file.
 * This way we never lose *both* old and new versions of the file.
 *
 * NOTE: Win32 implementation of rename() fails if target file exists,
 * while on Linux for example, it does not not. Since this code is 
 * supposed to be portable, we have to assume the worst (i.e. Win32) 
 * and do WRITE(tmp1)-RENAME(file,tmp2)-RENAME(tmp1,file)-DELETE(tmp2)
 * instead of just WRITE(tmp)-RENAME(tmp,file)
 *
 * This function opens file in a text mode ("t" mode on Win32).
 */
Bool FILE_Save(Str fname, FileSaveCB cb, void * ctx, IODesc io)
{
    return FILE_Save2(fname, cb, ctx, True, io);
}

/**
 * Same as the above, only allows you to specify file mode (text vs binary)
 * on those platforms where it matters (e.g. Windows).
 */
Bool FILE_Save2(Str fname, FileSaveCB cb, void * ctx, Bool txt, IODesc io)
{
    Bool success = False;
    ASSERT(fname && fname[0]);
    if (fname && fname[0]) {
        Char * tmp1 = FILE_DirName(fname, TEMP_FILE_NAME_LEN);
        if (tmp1 && (!tmp1[0] || FILE_MkDir(tmp1))) {
            const char * mode = txt ? WRITE_TEXT_MODE : WRITE_BINARY_MODE;
            int dirlen = StrLen(tmp1);
            File * f;

            /* 
             * write the temp file 
             */
            FILE_MakeUnique(tmp1, dirlen, TEMP_FILE_NAME_LEN);
            f = FILE_Open(tmp1, mode, io);
            if (f) {

                Bool saved = (*cb)(f,fname,ctx);
                FILE_Close(f);
                if (saved) {
                    
                    if (FILE_CanRead(fname)) {

                        /* 
                         * generate another unique file name
                         */
                        size_t nchars = dirlen + 1 + TEMP_FILE_NAME_LEN;
                        Char * tmp2 = MEM_NewArray(Char,nchars);
                        if (tmp2) {
                            StrCpy(tmp2, tmp1);
                            FILE_MakeUnique(tmp2,dirlen,TEMP_FILE_NAME_LEN);

                            /* 
                             * rename target -> tmp2
                             */
                
                            Verbose(TEXT("Renaming %s into %s\n"),fname,tmp2);
                            if (FILE_Rename(fname, tmp2)) {

                                /* 
                                 * rename tmp1 -> target
                                 */
                
                                Verbose(TEXT("Renaming %s into %s\n"),tmp1,fname);
                                if (FILE_Rename(tmp1, fname)) {
        
                                    /*
                                     * finally, remove tmp2 (old target)
                                     * perhaps, we should ignore the error
                                     * returned by FILE_Delete?
                                     */
                                    Verbose(TEXT("Deleting %s\n"),tmp2);
                                    success = FILE_Delete(tmp2);
                                }
                            }

                            MEM_Free(tmp2);
                        }

                    /*
                     * there's no target, just rename tmp1 -> target
                     */
                    } else {
                        Verbose(TEXT("Renaming %s into %s\n"),tmp1,fname);
                        success = FILE_Rename(tmp1, fname);
                    }

                } else {

                    /*
                     * just quietly delete the temporary file. An error
                     * message, if any, should have been provided by the
                     * callback.
                     */
                    FILE_Delete(tmp1);
                }
            }
        }
        MEM_Free(tmp1);
    }
    return success;
}

/**
 * Initializer for the platform independent part of the directory iterator.
 * Assumes that the memory has been zeroed.
 */
void DIR_ItrInit(DirIterator * di, const Itr * type)
{
    ITR_Init(&di->itr, type);
    STRBUF_Init(&di->dirName);
    STRBUF_Init(&di->fileName);
}

Bool DIR_ItrHasNext(Iterator * itr)
{
    DirIterator * di = CAST(itr,DirIterator,itr);
    return di->hasNext;
}

Bool DIR_ItrRemove(Iterator * itr)
{
    Bool ok = False;
    DirIterator * di = CAST(itr,DirIterator,itr);
    DirType type = di->entry.type;
    int dirlen = STRBUF_Length(&di->dirName);
    if (STRBUF_Alloc(&di->dirName, dirlen+STRBUF_Length(&di->fileName)+1)) {
        Str fullName;
        Bool isDir;
        STRBUF_AppendChar(&di->dirName, FILE_SEPARATOR_CHAR);
        STRBUF_AppendBuf(&di->dirName, &di->fileName);
        fullName = STRBUF_Text(&di->dirName);
        if (type == DTypeUnknown) {
            isDir = FILE_IsDir(fullName);
        } else {
            isDir = BoolValue(type == DTypeDir);
        }
        if (isDir) {
            ok = FILE_RmDir(fullName, True);
        } else {
            ok = FILE_Delete(fullName);
        }
        STRBUF_SetLength(&di->dirName, dirlen);
        di->entry.dir = STRBUF_Text(&di->dirName);
    }
    return ok;
}

void DIR_ItrDestroy(DirIterator * di)
{
    STRBUF_Destroy(&di->dirName);
    STRBUF_Destroy(&di->fileName);
}

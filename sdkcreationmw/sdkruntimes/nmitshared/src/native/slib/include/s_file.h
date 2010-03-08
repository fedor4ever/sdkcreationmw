/*
 * $Id: s_file.h,v 1.31 2006/03/26 05:50:50 slava Exp $
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

#ifndef _SLAVA_FILE_H_
#define _SLAVA_FILE_H_

#include "s_def.h"
#include "s_buf.h"
#include "s_strbuf.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/*
 * File flags
 */
#define FILE_CAN_READ       0x0001  /* caller can read from the file */
#define FILE_CAN_WRITE      0x0002  /* caller can write to the file */
#define FILE_PUBLIC_FLAGS   0xffff  /* flags caller is allowed to set/read */

/*
 * Flags for FILE_Zip
 */
#define FILE_ZIP_IN         0x0001  /* compress input stream */
#define FILE_ZIP_OUT        0x0002  /* compress output stream */
#define FILE_ZIP_ZHDR       0x0004  /* read and write zlib header */
#define FILE_ZIP_GZIP       0x0008  /* read and write gzip header */

#define FILE_ZIP_NONE       0       /* no compression */
#define FILE_ZIP_ALL        (FILE_ZIP_IN | FILE_ZIP_OUT)

/*
 * File is a context associted with an open file.
 * FileIO defines the set of functions that actualy do the I/O
 */
typedef struct _File File;
typedef struct _FileIO FileIO;
typedef const FileIO* IODesc;

/*
 * Set of file functions for plain file I/O
 */
extern const FileIO PlainFileIO;
#define PlainFile (&PlainFileIO)

/*
 * Set of file functions for socket I/O
 */
extern const FileIO SocketIO;
#define SocketFile (&SocketIO)

/*
 * A set of handlers for doing gzipped file I/O. Must have zlib in order 
 * to have this functionality
 */
#define GZIP_EXTENSION ".gz"
extern const FileIO GZipFileIO;
#define GZipFile (&GZipFileIO)

/* 
 * Function prototypes. The type of file I/O (i.e. gzipped or not) is 
 * defined by the set of handlers passed into FILE_Open. If FileIO 
 * parameter is NULL, the default will be used which is plain (non
 * compressed) file I/O. FILE_Connect creates a File that uses TCP
 * connection as an I/O channel.
 *
 * NOTE: these functions are not thread safe. They could be made thread 
 * safe buf that would have negative impact on performance, so I decided 
 * against it. Writing the same file by multiple threads is a relatively 
 * rare operation, and it would likely to require some synchronization 
 * anyway, which can be used to synchronize access to the file functions
 * as well.
 *
 * Another note:  FILE_Connect() takes IP address and port in host byte order.
 */
extern File * FILE_Connect  P_((IPaddr addr, Port port));
extern File * FILE_Open     P_((Str path, const char * mode, IODesc io));
extern Bool   FILE_Reopen   P_((File * f, Str path, const char * mode));
extern Bool   FILE_SetParam P_((File * f, Str name, void * value));
extern Str    FILE_Name     P_((File * f));
extern int    FILE_Flags    P_((File * f));
extern void   FILE_SetFlag  P_((File * f, int flag)); /* flags |= flag */
extern void   FILE_ClrFlag  P_((File * f, int flag)); /* flags &= ~flag */
extern int    FILE_Read     P_((File * f, void * buf, int len));
extern Bool   FILE_ReadAll  P_((File * f, void * buf, int len));
extern int    FILE_Write    P_((File * f, const void * buf, int len));
extern Bool   FILE_WriteAll P_((File * f, const void * buf, int len));
extern int    FILE_Skip     P_((File * f, int skip));
extern Bool   FILE_SkipAll  P_((File * f, int skip));
extern int    FILE_PushBack P_((File * f, const void * buf, int len));
extern int    FILE_Printf   P_((File * f, Str format, ...));
extern int    FILE_VaPrintf P_((File * f, Str format, va_list va));
extern Bool   FILE_Puts     P_((File * f, Str s));
extern Bool   FILE_Gets     P_((File * f, Char * buf, int len));
extern Bool   FILE_Putc     P_((File * f, int c));
extern int    FILE_Getc     P_((File * f));
extern Bool   FILE_Ungetc   P_((File * f, Char c));
extern Bool   FILE_Flush    P_((File * f));
extern Bool   FILE_Eof      P_((File * f));
extern File * FILE_Target   P_((File * f));
extern void   FILE_Detach   P_((File * f));
extern void   FILE_Finish   P_((File * f));
extern void   FILE_Close    P_((File * f));

/* compress/decompress the stream. */
extern File * FILE_Zip P_((File * f, int flags));
extern Bool   FILE_ZipFinish P_((File * f));

/* backward compatibility */
#define FILE_Compress(_f,_fl) FILE_Zip(_f, (_fl) | FILE_ZIP_GZIP)
#define FILE_COMPRESS_IN    FILE_ZIP_IN
#define FILE_COMPRESS_OUT   FILE_ZIP_OUT
#define FILE_COMPRESS_NONE  0
#define FILE_COMPRESS_ALL  (FILE_ZIP_IN | FILE_ZIP_OUT)

/* attach to existing file/socket */
extern File * FILE_AttachToSocket P_((Socket s));

#ifndef __KERNEL__
extern File * FILE_AttachToFile   P_((FILE * file, Str name));
#endif /* __KERNEL__ */

/* dummy file - consumes all output, provides no input */
extern File * FILE_Null     P_((void));
extern Bool   FILE_IsNull   P_((const File * f));

/* in-memory I/O */
extern File * FILE_Mem      P_((void));
extern File * FILE_MemBuf   P_((Buffer * buf, Bool freeBuf));
extern File * FILE_MemIn    P_((const void * data, int size));
extern File * FILE_MemOut   P_((int maxsize));
extern File * FILE_MemOut2  P_((void * data, int size));
extern Bool   FILE_IsMem    P_((const File * f));
extern int    FILE_MemSize  P_((const File * f));
extern void * FILE_MemData  P_((File * f));
extern void   FILE_MemClear P_((File * f));

/* splitting a stream */
extern File * FILE_Split    P_((Str name, File * f[], int n));
extern File * FILE_Split2   P_((Str name, File * f1, File * f2));

/* wrapping a text stream */
extern File * FILE_Wrap      P_((File * f, int step, int margin));
extern Bool   FILE_IsWrap    P_((const File * f));
extern Bool   WRAP_IsEnabled P_((const File * wrap));
extern Bool   WRAP_Enable    P_((File * wrap, Bool enable));
extern Bool   WRAP_Indent    P_((File * wrap, int change));
extern Bool   WRAP_SetIndent P_((File * wrap, int level));
extern int    WRAP_GetIndent P_((File * wrap));

/* utilities */
extern Bool   FILE_IsFileIO P_((File * f));
extern Str    FILE_ReadLine P_((File * in, StrBuf * sb));
extern int    FILE_ReadData P_((File * in, Buffer * out, int max));
extern int    FILE_Copy     P_((File * in, File * out));
extern int    FILE_CopyN    P_((File * in, File * out, int max));

/* utilities for reading binary data with byte order convertion */
extern Bool   FILE_ReadI16  P_((File * f, I16u * data, int endianess));
extern Bool   FILE_ReadI32  P_((File * f, I32u * data, int endianess));
extern Bool   FILE_ReadI64  P_((File * f, I64u * data, int endianess));
extern Bool   FILE_ReadI16B P_((File * f, I16u * data)); /* big endian */
extern Bool   FILE_ReadI32B P_((File * f, I32u * data)); /* big endian */
extern Bool   FILE_ReadI64B P_((File * f, I64u * data)); /* big endian */
extern Bool   FILE_ReadI16L P_((File * f, I16u * data)); /* little endian */
extern Bool   FILE_ReadI32L P_((File * f, I32u * data)); /* little endian */
extern Bool   FILE_ReadI64L P_((File * f, I64u * data)); /* little endian */
extern Bool   FILE_ReadF32  P_((File * f, float * data, int endianess));
extern Bool   FILE_ReadF64  P_((File * f, double * data, int endianess));
extern Bool   FILE_ReadF32B P_((File * f, float * data));  /* big endian */
extern Bool   FILE_ReadF64B P_((File * f, double * data)); /* big endian */
extern Bool   FILE_ReadF32L P_((File * f, float * data));  /* little endian */
extern Bool   FILE_ReadF64L P_((File * f, double * data)); /* little endian */

/* utilities for writing binary data with byte order convertion */
extern Bool   FILE_WriteI16  P_((File * f, I16u data, int endianess));
extern Bool   FILE_WriteI32  P_((File * f, I32u data, int endianess));
extern Bool   FILE_WriteI64  P_((File * f, I64u data, int endianess));
extern Bool   FILE_WriteI16B P_((File * f, I16u data)); /* big endian */
extern Bool   FILE_WriteI32B P_((File * f, I32u data)); /* big endian */
extern Bool   FILE_WriteI64B P_((File * f, I64u data)); /* big endian */
extern Bool   FILE_WriteI16L P_((File * f, I16u data)); /* little endian */
extern Bool   FILE_WriteI32L P_((File * f, I32u data)); /* little endian */
extern Bool   FILE_WriteI64L P_((File * f, I64u data)); /* little endian */
extern Bool   FILE_WriteF32  P_((File * f, float data, int endianess));
extern Bool   FILE_WriteF64  P_((File * f, double data, int endianess));
extern Bool   FILE_WriteF32B P_((File * f, float data));  /* big endian */
extern Bool   FILE_WriteF64B P_((File * f, double data)); /* big endian */
extern Bool   FILE_WriteF32L P_((File * f, float data));  /* little endian */
extern Bool   FILE_WriteF64L P_((File * f, double data)); /* little endian */

/* handy macros */
#define FILE_AllowsReads(_f) (((FILE_Flags(_f)) & FILE_CAN_READ) != 0)
#define FILE_AllowsWrites(_f) (((FILE_Flags(_f)) & FILE_CAN_WRITE) != 0)

#ifdef __cplusplus
} /* end of extern "C" */
#endif  /* __cplusplus */

#endif /* _SLAVA_FILE_H_ */

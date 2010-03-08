/*
 * $Id: s_util.h,v 1.56 2006/03/30 15:35:50 slava Exp $
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

#ifndef _SLAVA_UTIL_H_
#define _SLAVA_UTIL_H_

#include "s_itr.h"
#include "s_buf.h"
#include "s_file.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* output */
#define PRINT_ERROR     0x0001
#define PRINT_WARNING   0x0002
#define PRINT_NORMAL    0x0004
#define PRINT_VERBOSE   0x0008
#define PRINT_DEBUG     0x0010

/* Note: PRINT_DEBUG type of trace is only available in debug build */
#if DEBUG

#  define PRINT_ALL  (PRINT_ERROR   |\
                      PRINT_WARNING |\
                      PRINT_NORMAL  |\
                      PRINT_VERBOSE |\
                      PRINT_DEBUG)

#  define DEFAULT_PRINT_MASK  PRINT_ALL

#else

#  define PRINT_ALL  (PRINT_ERROR   |\
                      PRINT_WARNING |\
                      PRINT_NORMAL  |\
                      PRINT_VERBOSE)

#  define DEFAULT_PRINT_MASK  (PRINT_ERROR | PRINT_WARNING | PRINT_NORMAL)

#endif

/* Backward compatibility macros */
#define DEFAULT_TRACE_MASK DEFAULT_PRINT_MASK
#define TRACE_ALL          PRINT_ALL
#define TRACE_ERROR        PRINT_ERROR
#define TRACE_WARNING      PRINT_WARNING
#define TRACE_NORMAL       PRINT_NORMAL
#define TRACE_VERBOSE      PRINT_VERBOSE
#define TRACE_DEBUG        PRINT_DEBUG
#define TRACE_Error        PRINT_Error
#define TRACE_ErrorVa      PRINT_ErrorVa
#define TRACE_Warning      PRINT_Warning
#define TRACE_WarningVa    PRINT_WarningVa
#define TRACE_Output       PRINT_Output
#define TRACE_OutputVa     PRINT_OutputVa
#define TRACE_Verbose      PRINT_Verbose
#define TRACE_VerboseVa    PRINT_VerboseVa
#define TRACE_SetMask      PRINT_SetMask
#define TRACE_GetMask      PRINT_GetMask
#define TRACE_UseConsole   PRINT_UseConsole
#define TRACE_Dump         PRINT_Dump
#ifndef TRACE_MACROS_DEFINED
#  define TRACE_MACROS_DEFINED 1
#  define Error              PRINT_Error
#  define Warning            PRINT_Warning
#  define Output             PRINT_Output
#  define Verbose            PRINT_Verbose
#  define Dump               PRINT_Dump
#endif /* TRACE_MACROS_DEFINED */

extern void PRINT_SetMask P_((int mask));
extern int  PRINT_GetMask P_((void));
extern void PRINT_UseConsole P_((Bool enable));

/* these two functions DO NOT check the PRINT_xxx flags */
extern int PRINT_Stdout P_((Str format, va_list va));
extern int PRINT_Stderr P_((Str format, va_list va));

typedef int (*PrintProc) P_((Str format, ... ));
typedef int (*PrintVaProc) P_((Str format, va_list va));

extern int PRINT_Error P_((Str format, ... ));           /* PRINT_ERROR */
extern int PRINT_ErrorVa P_((Str format, va_list va));
extern int PRINT_Warning P_((Str format, ... ));         /* PRINT_WARNING */
extern int PRINT_WarningVa P_((Str format, va_list va));
extern int PRINT_Output P_((Str format, ... ));          /* PRINT_NORMAL */
extern int PRINT_OutputVa P_((Str format, va_list va));
extern int PRINT_Verbose P_((Str format, ... ));         /* PRINT_VERBOSE */
extern int PRINT_VerboseVa P_((Str format, va_list va));

/* helpers for generated parsers */
extern void yyerror P_((Str s)); 

/* system information */
extern int  SYSTEM_CountCPU P_((void));
extern Bool IO_TermSize P_((int * rows, int * cols));

/* date/time utilities */
extern Time   TIME_Now P_((void));
extern time_t TIME_ToUnix P_((Time t));
extern Str    TIME_ToString P_((Time t));

/* socket manupulations (port and IP address are in host byte order) */
extern Bool SOCKET_Create P_((int type, IPaddr addr, Port p, Socket * s));
extern Bool SOCKET_Connect P_((Socket s, IPaddr addr, Port p));
extern Bool SOCKET_Block P_((Socket s, Bool block));
extern Bool SOCKET_Close P_((Socket s));
extern int  SOCKET_GetLastError P_((void));
extern int  SOCKET_Wait P_((Socket s, int mask, Time timeout));

/* Internet related utilities */
extern Bool INET_ResolveAddr P_((Str host, IPaddr * addr));

/* useful wrappers for SOCKET_Create() */
#define SOCKET_GetTcp(_port,_sock) SOCKET_Create(SOCK_STREAM,0,_port,_sock)
#define SOCKET_GetUdp(_port,_sock) SOCKET_Create(SOCK_DGRAM,0,_port,_sock)

/* constants used by SOCKET_Wait */
#define SOCK_WAIT_READ 0x001
#define SOCK_WAIT_WRITE 0x002
#define SOCK_WAIT_EXCEPT 0x004
#define SOCK_WAIT_ALL (SOCK_WAIT_READ | SOCK_WAIT_WRITE | SOCK_WAIT_EXCEPT)

/* 
 * useful wrappers for SOCKET_Wait()
 * Note that SOCKET_WaitRead() and SOCKET_WaitWrite() set SOCK_WAIT_EXCEPT 
 * flag, might be a little bit counter-inituitive...
 */
#define SOCKET_WaitRead(s,t)  SOCKET_Wait(s,SOCK_WAIT_EXCEPT|SOCK_WAIT_READ,t)
#define SOCKET_WaitWrite(s,t) SOCKET_Wait(s,SOCK_WAIT_EXCEPT|SOCK_WAIT_WRITE,t)
#define SOCKET_WaitAll(s,t)   SOCKET_Wait(s,SOCK_WAIT_ALL,t)

/* file manupulations */
#define TEMP_FILE_NAME_LEN  8

typedef Bool (*FileSaveCB)P_((File* out, Str fname, void * ctx));
typedef Bool (*FileListCB)P_((Str dir, Str fname, void * ctx));

/*
 * Directory entry type enumeration. On Windows, you may expect DTypeFile
 * or DTypeDir, on other systems - anything else, including DTypeUnknown.
 * A pointer to the DirEntry structure is returned by the ITR_Next function
 * of the iterator created by FILE_ListDir.
 */
typedef enum _DirType {
    DTypeUnknown,       /* unknown entry type */
    DTypeFile,          /* a regular file */
    DTypeDir,           /* a directory */
    DTypePipe,          /* a named pipe */
    DTypeLink,          /* a symbolic link */
    DTypeSocket,        /* a socket */
    DTypeChar,          /* a character device */
    DTypeBlock          /* a block device */
} DirType;
typedef struct _DirEntry {
    Str dir;            /* the name of the directory containing the entry */
    Str name;           /* the name of the entry within the directory */
    DirType type;       /* the entry type, e.g. DTypeFile or DTypeDir */
} DirEntry;

extern Char * FILE_DirName P_((Str path, int extra));
extern Char * FILE_TempName P_((Char * buf, int n));
extern Bool FILE_IsFileSeparator P_((Char c));
extern Bool FILE_Save P_((Str fname, FileSaveCB cb, void * ctx, IODesc io));
extern Bool FILE_Save2 P_((Str fn,FileSaveCB cb,void* ctx,Bool txt,IODesc io));
extern Str  FILE_FilePart P_((Str path));
extern int  FILE_FindSeparator P_((Str s));
extern Bool FILE_CanOpen P_((Str fname, const char * flags));
extern Bool FILE_Exist P_((Str fname));
extern Bool FILE_NonExist P_((Str fname));
extern Bool FILE_IsFile P_((Str fname));
extern Bool FILE_IsDir P_((Str fname));
extern Bool FILE_IsAbs P_((Str fname));
extern void FILE_MakeUnique P_((Char * name, int fixedPart, int randomPart));
extern Bool FILE_MkDir P_((Str dir));
extern Bool FILE_RmDir P_((Str dir, Bool recurse));
extern Bool FILE_Delete P_((Str fname));
extern Bool FILE_Rename P_((Str oldn, Str newn));
extern int  FILE_List P_((Str dir, FileListCB cb, void * ctx));
extern Iterator * FILE_ListDir P_((Str dir));

#define FILE_CanRead(_fname)  FILE_CanOpen(_fname, "r")
#define FILE_CanWrite(_fname) FILE_CanOpen(_fname, "w")

/* URL encoding */
extern Str URL_Decode P_((StrBuf * dest, Str src));
extern Str URL_Encode P_((StrBuf * dest, Str src));
extern Str URL_EncodeChars P_((StrBuf * dest, Str src, Str esc));

/* string manipulations */
extern char * STRING_Dup8 P_((const char * s));
extern wchar_t * STRING_DupU P_((const wchar_t * ws));
extern Char * STRING_Format P_((Char * buf, int bufsize, Str format, ...));
extern Bool STRING_StartsWith P_((Str s1, Str s2));
extern Bool STRING_StartsWithNoCase P_((Str s1, Str s2));
extern Bool STRING_EndsWith P_((Str s1, Str s2));
extern Bool STRING_EndsWithNoCase P_((Str s1, Str s2));
extern int STRING_IndexOf P_((Str s, Char c));
extern int STRING_LastIndexOf P_((Str s, Char c));
extern int STRING_HashCode P_((Str s));
extern int STRING_HashCodeNoCase P_((Str s));
extern char * STRING_ToMultiByte P_((const wchar_t * ws));
extern char * STRING_ToMultiByteN P_((const wchar_t * ws, size_t count));
extern wchar_t * STRING_ToUnicode P_((const char * mbs));
extern Str STRING_FormatDouble P_((StrBuf * sb, double d));

#ifdef UNICODE
#  define STRING_Dup(_s)    STRING_DupU(_s)
#else  /* !UNICODE */
#  define STRING_Dup(_s)    STRING_Dup8(_s)
#endif /* !UNICODE */

/* dump data into the stream */
extern void PRINT_Dump P_((PrintProc p,const void* buf,size_t cb,size_t off));

/* parsing */
extern Bool PARSE_Bool P_((Str s, Bool * b));
extern Bool PARSE_Byte P_((Str s, char * n, int base));
extern Bool PARSE_UByte P_((Str s, unsigned char * n, int base));
extern Bool PARSE_Short P_((Str s, short * n, int base));
extern Bool PARSE_UShort P_((Str s, unsigned short * n, int base));
extern Bool PARSE_Int P_((Str s, int * n, int base));
extern Bool PARSE_UInt P_((Str s, unsigned int * n, int base));
extern Bool PARSE_Long P_((Str s, long * n, int base));
extern Bool PARSE_ULong P_((Str s, unsigned long * n, int base));
extern Bool PARSE_Double P_((Str s, double * d));

#define PARSE_I8(_x,_y,_z)  PARSE_Byte(_x,_y,_z)
#define PARSE_U8(_x,_y,_z)  PARSE_UByte(_x,_y,_z)
#define PARSE_I16(_x,_y,_z) PARSE_Short(_x,_y,_z)
#define PARSE_U16(_x,_y,_z) PARSE_UShort(_x,_y,_z)
#define PARSE_I32(_x,_y,_z) PARSE_Int(_x,_y,_z)
#define PARSE_U32(_x,_y,_z) PARSE_UInt(_x,_y,_z)
#define PARSE_I64(_x,_y,_z) PARSE_Long64(_x,_y,_z)
#define PARSE_U64(_x,_y,_z) PARSE_ULong64(_x,_y,_z)

#ifdef __LONG_64__
#  define PARSE_Long64 PARSE_Long
#  define PARSE_ULong64 PARSE_ULong
#else  /* !__LONG_64__ */
extern Bool PARSE_Long64 P_((Str s, int64_t * n, int base));
extern Bool PARSE_ULong64 P_((Str s, uint64_t * n, int base));
#endif /* !__LONG_64__ */

#ifdef __cplusplus
} /* end of extern "C" */
#endif  /* __cplusplus */

#endif /* _SLAVA_UTIL_H_ */

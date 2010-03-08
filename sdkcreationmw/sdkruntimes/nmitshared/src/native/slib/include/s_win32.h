/*
 * $Id: s_win32.h,v 1.35 2006/02/23 01:35:03 slava Exp $
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

#ifndef _SLAVA_WIN32_H_
#define _SLAVA_WIN32_H_ 1

#ifndef _WIN32
#  error "Do not include s_win32.h directly. Include s_os.h"
#endif

#define _TLS_SUPPORT 1

/* make sure that UNICODE and _UNICODE are both defined, or both undefined */
#ifdef _UNICODE
#  ifndef UNICODE
#    define UNICODE     /* _UNICODE was defined but UNICODE was not */
#  endif /* UNICODE */
#else
#  ifdef UNICODE
#    define _UNICODE    /* UNICODE was defined but _UNICODE was not */
#  endif /* UNICODE */
#endif /* _UNICODE */

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

/* these includes are present on all known platforms except Windiws CE */
#ifndef _WIN32_WCE
#include <stddef.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#endif /* _WIN32_WCE */

/* this is necessary to eliminate some conflicts */
#define Random Win32Random
#define __RPCASYNC_H__

/* ensure that windows.h is sucked in */
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN 1
#endif /* WIN32_LEAN_AND_MEAN */

#ifndef NOCRYPT
#  define NOCRYPT
#endif /* NOCRYPT */

#include <windows.h>
#include <winsock.h>

/* undo the damage done to slib namespace */
#undef ASSERT
#undef VERIFY
#undef Random

/* value returned by GetFileAttributes() that indicates failure */
#define FILE_ATTRIBUTE_FAILURE 0xffffffff

/* _stat() definitions */
#ifndef _WIN32_WCE
#  include <sys/types.h>
#  include <sys/stat.h>
#endif /* _WIN32_WCE */

/* _mkdir() is defined there */
#ifndef _WIN32_WCE
#  include <direct.h>
#endif /* _WIN32_WCE */

/* TLS_OUT_OF_INDEXES may not be defined  */
#ifndef TLS_OUT_OF_INDEXES
#  define TLS_OUT_OF_INDEXES (DWORD)0xFFFFFFFF
#endif /* TLS_OUT_OF_INDEXES */

/* 
 * __CW32__ in defined when the code is being build with CodeWarrior.
 * CodeWarrior for Symbian comes with its own set of Win32 header
 * files which are slightly different from those distributed by 
 * Microsoft. Also, the Metrowerks compiler does not support some
 * of the Microsoft's keywords.
 */
#ifdef __CW32__
#  define _WCTYPE_T_DEFINED
#  define _I64_MAX      __INT64_C(9223372036854775807)
#  define _I64_MIN      (_I64_MAX - 1)
#  define _UI64_MAX     __UINT64_C(0xffffffffffffffff)
#  ifndef _CRTIMP
#    define _CRTIMP
#  endif /* _CRTIMP */
_CRTIMP wchar_t * __cdecl _wctime(const time_t *);
_CRTIMP FILE * __cdecl _wfopen(const wchar_t *, const wchar_t *);
_CRTIMP FILE * __cdecl _wfreopen(const wchar_t *, const wchar_t *, FILE *);
extern int _cw_swprintf(wchar_t *, const wchar_t *, ...);
#endif /* __CW32__ */

#include <tchar.h>

#undef  far
#undef  near
#undef  FAR
#undef  NEAR
#define FAR
#define NEAR

/* character type */
#ifdef _UNICODE
typedef WCHAR Char;
#else  /* !_UNICODE */
typedef char Char;
#endif /* !_UNICODE */

#define TEXT_(quote) TEXT(quote)

/* string manipulation functions */
#define StrLen      _tcslen
#define StrCmp      _tcscmp
#define StrnCmp     _tcsncmp
#define StrCpy      _tcscpy
#define StrCat      _tcscat
#define StrChr      _tcschr
#define StrStr      _tcsstr
#define StrrChr     _tcsrchr
#define StrnCpy     _tcsncpy
#define StrpBrk     _tcspbrk
#define StrTod      _tcstod
#define StrTol      _tcstol
#define StrToul     _tcstoul
#define StrCaseCmp  _tcsicmp
#define StrnCaseCmp _tcsnicmp
#define Vsnprintf   _vsntprintf
#define Vfprintf    _vftprintf

/* CodeWarrior defines swprintf incorrectly */
#if defined(__CW32__) && defined(UNICODE)
#define Sprintf     _cw_swprintf
#else  /* !(__CW32__ && UNICODE) */
#define Sprintf     _stprintf
#endif /* !(__CW32__ && UNICODE) */

/* character classification functions */
#define IsUpper     _istupper
#define IsLower     _istlower
#define IsPrint     _istprint
#define IsDigit     _istdigit
#define IsXdigit    _istxdigit
#define IsAlpha     _istalpha
#define IsSpace     _istspace
#define ToUpper     _totupper
#define ToLower     _totlower

/* other functions */
#define Ctime       _tctime
#define Fopen       _tfopen
#define Freopen     _tfreopen

/* redefine Winsock-style shutdown() constants in Unix style */
#define SHUT_RD   SD_RECEIVE
#define SHUT_WR   SD_SEND
#define SHUT_RDWR SD_BOTH  

#ifndef SD_BOTH
#  define SD_RECEIVE 0x00
#  define SD_SEND    0x01
#  define SD_BOTH    0x02
#endif /* SD_BOTH */

/* define these types the way they are defined on Unix platforms */
typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
typedef int pid_t;

/* 
 * and these are their Win32-style equivalents. Only Windows CE defines 
 * all of them, have to fix that in non-WinCE build
 */
#ifndef _WIN32_WCE
#  ifndef MISSING_WIN32_TYPES_DEFINED
#    define MISSING_WIN32_TYPES_DEFINED
typedef int8_t       INT8;
typedef uint8_t      UINT8;
typedef int16_t      INT16;
typedef uint16_t     UINT16;
#    ifndef __CW32__
typedef uint32_t     UINT32;
#    endif /* !__CW32__ */
typedef const void * PCVOID;
#  endif /* !MISSING_WIN32_TYPES_DEFINED */
#endif /* !_WIN32_WCE */

#define INT8_MIN   _I8_MIN
#define INT8_MAX   _I8_MAX
#define UINT8_MAX  _UI8_MAX

#define INT16_MIN   _I16_MIN
#define INT16_MAX   _I16_MAX
#define UINT16_MAX  _UI16_MAX

#define INT32_MIN   _I32_MIN
#define INT32_MAX   _I32_MAX
#define UINT32_MAX  _UI32_MAX

#ifdef __CW32__
#  define __INT64_C(c)  ((I64s)(c))
#  define __UINT64_C(c) ((I64u)(c))
#else  /* !__CW32__ */
#  define __INT64_C(c)  c ## i64 
#  define __UINT64_C(c) c ## ui64
#endif /* !__CW32__ */

#define INT64_MIN  _I64_MIN
#define INT64_MAX  _I64_MAX
#define UINT64_MAX _UI64_MAX

#define LONG_HEX_FORMAT "%I64X"
#define LONG_LONG_FORMAT "%I64d"
#define LONG_ULONG_FORMAT "%I64u"

#define I64X_FORMAT LONG_HEX_FORMAT
#define I64S_FORMAT LONG_LONG_FORMAT
#define I64U_FORMAT LONG_ULONG_FORMAT

#define stat              _stat
#define fstat             _fstat
#define getcwd            _getcwd
#define chdir             _chdir
#define rmdir             _rmdir
#define unlink            _unlink
#define mkdir(_dir,_perm) _mkdir(_dir)
#define tzset             _tzset
#define tzname            _tzname
#define sleep(_n)         Sleep(1000*(_n))
#define strdup            _strdup
#define strupr            _strupr
#define strrev            _strrev
#define stricmp           _stricmp
#define strnicmp          _strnicmp
#define snprintf          _snprintf
#define vsnprintf         _vsnprintf
#define strcasecmp        _stricmp
#define strncasecmp       _strnicmp
#define getpid()          ((pid_t)GetCurrentProcessId())

#ifndef __CW32__
#  define S_IFMT   _S_IFMT
#  define S_IFDIR  _S_IFDIR
#  define S_IFCHR  _S_IFCHR
#  define S_IFREG  _S_IFREG
#  define S_IREAD  _S_IREAD
#  define S_IWRITE _S_IWRITE
#  define S_IEXEC  _S_IEXEC
#endif /* !__CW32__ */

#define READ_TEXT_MODE "rt"
#define WRITE_TEXT_MODE "wt"
#define APPEND_TEXT_MODE "a+t"
#define READ_BINARY_MODE "rb"
#define WRITE_BINARY_MODE "wb"
#define APPEND_BINARY_MODE "a+b"

#define FILE_SEPARATOR "\\"
#define FILE_SEPARATOR_CHAR '\\'
#define PATH_SEPARATOR ";"
#define PATH_SEPARATOR_CHAR ';'

typedef SOCKET Socket;

/* 
 * Definitions for byte order, according to significance of bytes, from low
 * addresses to high addresses.  The value is what you get by putting '4'
 * in the most significant byte, '3' in the second most significant byte,
 * '2' in the second least significant byte, and '1' in the least
 * significant byte.  
 */

#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN    4321
#define __PDP_ENDIAN    3412

#define LITTLE_ENDIAN   __LITTLE_ENDIAN
#define BIG_ENDIAN      __BIG_ENDIAN
#define PDP_ENDIAN      __PDP_ENDIAN
#define BYTE_ORDER      __BYTE_ORDER

/* if byte order is not defined, default to little endian, like x86 */
#if defined(_X86_) || !defined (__BYTE_ORDER)
#  define __BYTE_ORDER __LITTLE_ENDIAN
#endif 

/* the word that has same size as a pointer. */
#ifdef _WIN64
typedef int64_t PtrWord;
#else  /* ! _WIN64 */
typedef int32_t PtrWord;
#endif /* ! _WIN64 */

/* memory allocation primitives provided by the system */
#define OS_MemAlloc(_size) malloc(_size)
#define OS_MemRealloc(_ptr,_size) realloc((_ptr),(_size))
#define OS_MemFree(_ptr) free(_ptr)

#endif /* _SLAVA_WIN32_H_ */

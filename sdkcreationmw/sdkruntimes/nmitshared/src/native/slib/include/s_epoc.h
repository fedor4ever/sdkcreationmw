/*
 * $Id: s_epoc.h,v 1.2 2006/03/07 16:41:59 slava Exp $
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

#ifndef _SLAVA_EPOC_H_
#define _SLAVA_EPOC_H_ 1

#ifndef _SLAVA_OS_H_
#  error "Do not include s_epoc.h directly. Include s_os.h"
#endif

#ifdef __VC32__
#  pragma warning(disable: 4291)
#endif /* __VC32__ */

#undef _WIN32
#include <stddef.h>
#include <stdarg.h>
#undef NULL
#include <e32def.h>
#define VA_LIST va_list
#define Time EPOC_Time
#include <e32std.h>
#undef Time

/*
 * The problem with wchar_t is that GCC thinks that it's different from
 * TText16, causing a latge number of compilation warnings and errors.
 */ 
#define wchar_t TText16

#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <signal.h>
#include <limits.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>

#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#undef remove

#ifdef __cplusplus
#  define Klass class
#else  /* !__cplusplus */
#  define Klass struct 
#endif /* __cplusplus */

/* 
 * ieeefp.h detects the byte order. It also defines finite(), isnan() and
 * isinf() functions that conflicts with their definitions in <math.h>
 * The ones defined in <math.h> look better to me, so I ignore those
 * defined in <ieeefp.h>
 */
#define finite  ieeefp_finite
#define isnan   ieeefp_isnan
#define isinf   ieeefp_isinf
#include <ieeefp.h>
#undef finite
#undef isnan
#undef isinf

/* make sure BYTE_ORDER is properly defined */
#ifndef BYTE_ORDER
#  if defined(__IEEE_LITTLE_ENDIAN) && defined(__IEEE_BIG_ENDIAN)
#    error "Both __IEEE_LITTLE_ENDIAN and __IEEE_BIG_ENDIAN are defined"
#  endif
#  define __LITTLE_ENDIAN 1234
#  define __BIG_ENDIAN    4321
#  define __PDP_ENDIAN    3412
#  define LITTLE_ENDIAN   __LITTLE_ENDIAN
#  define BIG_ENDIAN      __BIG_ENDIAN
#  define PDP_ENDIAN      __PDP_ENDIAN
#  define BYTE_ORDER      __BYTE_ORDER
#  ifdef __IEEE_LITTLE_ENDIAN
#    define __BYTE_ORDER __LITTLE_ENDIAN
#  elif defined(__IEEE_BIG_ENDIAN)
#    define __BYTE_ORDER __BIG_ENDIAN
#  else /* !__IEEE_LITTLE_ENDIAN && !__IEEE_BIG_ENDIAN */
#    error "Neigher __IEEE_LITTLE_ENDIAN nor __IEEE_BIG_ENDIAN is defined";
#  endif /* !__IEEE_LITTLE_ENDIAN && !__IEEE_BIG_ENDIAN */
#endif /* BYTE_ORDER */

#ifdef _MSC_VER
#  define __INT64_C(c)  c ## i64 
#  define __UINT64_C(c) c ## ui64
#else
#  ifndef __INT64_C
#    define __INT64_C(c) c ## LL
#  endif /* __INT64_C */
#  ifndef __UINT64_C
#    define __UINT64_C(c) c ## ULL
#  endif /* __UINT64_C */
#endif

#ifndef HUGE_VAL
#  define HUGE_VAL 1.7976931348623157E+308 /* Wasn't defined in Series60 1.x */
#endif /* HUGE_VAL */

typedef TInt8 int8_t;
typedef TUint8 uint8_t;
typedef TInt16 int16_t;
typedef TUint16 uint16_t;
typedef TInt32 int32_t;
typedef TUint32 uint32_t;

#ifdef __WINS__
/* Does CodeWarrior compiler support this? */
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#elif defined(__GCC32__)
typedef long long int64_t;
typedef unsigned long long uint64_t;
#else
#  error "Unknown compiler!"
#endif

typedef int pid_t;

#define LONG_HEX_FORMAT "%LX"
#define LONG_LONG_FORMAT "%Ld"
#define LONG_ULONG_FORMAT "%Lu"
#define I64X_FORMAT LONG_HEX_FORMAT
#define I64S_FORMAT LONG_LONG_FORMAT
#define I64U_FORMAT LONG_ULONG_FORMAT

#define READ_TEXT_MODE "r"
#define WRITE_TEXT_MODE "w"
#define APPEND_TEXT_MODE "a+"
#define READ_BINARY_MODE "r"
#define WRITE_BINARY_MODE "w"
#define APPEND_BINARY_MODE "a+"

#define FILE_SEPARATOR "\\"
#define FILE_SEPARATOR_CHAR '\\'
#define PATH_SEPARATOR ":"
#define PATH_SEPARATOR_CHAR ':'

typedef int Socket;
#define INVALID_SOCKET (-1)
#define closesocket(_s) close(_s) 
#define ioctlsocket(_s,_cmd,_argp)  ioctl(_s,_cmd,(char*)(_argp))

/* the word that has same size as a pointer */
typedef unsigned long PtrWord;

/* string manipulation routines */
#ifdef _UNICODE

typedef wchar_t Char, UChar;
#define __TEXT(quote)   L##quote

#  define StrLen        wcslen
#  define StrCmp        wcscmp
#  define StrCpy        wcscpy
#  define StrCat        wcscat
#  define StrChr        wcschr
#  define StrStr        wcsstr
#  define StrnCpy       wcsncpy
#  define StrrChr       wcsrchr
#  define Sprintf       swprintf
#  define StrTod        wcstod
#  define StrTol        wcstol
#  define StrToul       wcstoul
#  define StrCaseCmp    wcsicmp
#  define StrnCaseCmp   wcsnicmp

#  define Fopen         wfopen
#  define Freopen       wfreopen
#  define Rmdir         wrmdir
#  define Mkdir         wmkdir
#  define Unlink        wunlink
#  define Rename        wrename
#  define Stat          wstat
#  define Opendir       wopendir
#  define Readdir       wreaddir
#  define Closedir      wclosedir
#  define Dir           WDIR
#  define Dirent        wdirent

#else  /* !_UNICODE */

typedef char Char;
typedef unsigned char UChar;
#define __TEXT(quote)   quote

#  define StrLen        strlen
#  define StrCmp        strcmp
#  define StrCpy        strcpy
#  define StrCat        strcat
#  define StrnCpy       strncpy
#  define StrChr        strchr
#  define StrStr        strstr
#  define StrrChr       strrchr
#  define StrnCpy       strncpy
#  define StrpBrk       strpbrk
#  define StrTod        strtod
#  define StrTol        strtol
#  define StrToul       strtoul
#  define StrCaseCmp    strcasecmp
#  define StrnCaseCmp   strncasecmp
#  define Vfprintf      vfprintf
#  define Sprintf       sprintf

#  define Fopen         fopen
#  define Freopen       freopen
#  define Rmdir         rmdir
#  define Mkdir         mkdir
#  define Unlink        unlink
#  define Rename        rename
#  define Stat          stat
#  define Opendir       opendir
#  define Readdir       readdir
#  define Closedir      closedir
#  define Dir           DIR
#  define Dirent        dirent

#endif /* !_UNICODE */

#define Vsnprintf       vsnprintf
#define TEXT(quote)     (Str) __TEXT(quote)
#define TEXT_(quote)    __TEXT(quote)

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* string formatting functions missing from EPOC */
extern int vsnprintf(Char * buf, TInt n, const Char * format, va_list va);
extern int swprintf(wchar_t * buf, const wchar_t * format, ...);
extern const wchar_t * wcschr(const wchar_t * str, wchar_t ch);
extern wchar_t * wcsstr(const wchar_t * s1, const wchar_t * s2);
extern const wchar_t * wcsrchr(const wchar_t * str, wchar_t ch);
extern wchar_t * wcsncpy(wchar_t * dest, const wchar_t * src, TInt n);
extern int wcsicmp(const wchar_t * left, const wchar_t * right);
extern int wcsnicmp(const wchar_t * left, const wchar_t * right, TInt n);
extern unsigned long wcstoul(const wchar_t * s, wchar_t **ptr, int base);
extern long wcstol(const wchar_t * str, wchar_t** tail, int base);
extern double wcstod(const wchar_t * str, wchar_t** tail);

/* character classification functions */
extern int IsUpper P_((int c));
extern int IsLower P_((int c));
extern int IsPrint P_((int c));
extern int IsDigit P_((int c));
extern int IsXdigit P_((int c));
extern int IsAlpha P_((int c));
extern int IsSpace P_((int c));
extern int ToUpper P_((int c));
extern int ToLower P_((int c));

#ifdef __cplusplus
} /* end of extern "C" */
#endif  /* __cplusplus */

/* shutdown() flags */
#define SHUT_RD     0x00
#define SHUT_WR     0x01
#define SHUT_RDWR   0x02  

/* other functions */
#define Ctime       ctime

#define OS_MemAlloc(_size) malloc(_size)
#define OS_MemRealloc(_ptr,_size) realloc((_ptr),(_size))
#define OS_MemFree(_ptr) free(_ptr)

#endif /* _SLAVA_EPOC_H_ */

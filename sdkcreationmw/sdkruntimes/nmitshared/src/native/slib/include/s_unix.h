/*
 * $Id: s_unix.h,v 1.38 2006/04/13 12:50:14 slava Exp $
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

#ifndef _SLAVA_UNIX_H_
#define _SLAVA_UNIX_H_ 1

#ifdef _WIN32
#  error "Do not include s_unix.h directly. Include s_os.h"
#endif

#ifdef _LINUX_KERNEL

#  ifndef KERNELRELEASE
#    define KERNELRELEASE 2.4
#  endif /* KERNELRELEASE */

#  include <linux/kernel.h>
#  include <linux/vmalloc.h>
#  include <linux/string.h>
#  include <linux/ctype.h>
#  include <linux/time.h>
#  include <linux/slab.h>
#  include <linux/nls.h>
#  include <asm/string.h>

   /* signals that should never be blocked by kernel */
#  define SHUTDOWN_SIGS  (sigmask(SIGKILL)|sigmask(SIGINT)|sigmask(SIGQUIT))

   /* converts milliseconds to kernel jiffies */
#  define MILLIS_TO_JIFFIES(_millis)  (((_millis) * HZ)/1000)

extern int strcasecmp(const char *, const char *);

#else /* _LINUX_KERNEL */

#define __STDC_LIMIT_MACROS
#define _TLS_SUPPORT 1
#define _USE_PTHREADS 1

#ifdef __APPLE__
/* actually, MacOS X 10.3 and later does have <wchar.h> */
#  define _NO_WCHAR_H 1
#else
#  include <wchar.h>
#  undef _NO_WCHAR_H
#endif /* __APPLE__ */

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stddef.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#ifndef __CYGWIN__

#  include <inttypes.h>
#  ifndef __APPLE__
#    include <wctype.h>
#  endif /* __APPLE__ */

#else /* __CYGWIN__ */

typedef u_int8_t uint8_t;
typedef u_int16_t uint16_t;
typedef u_int32_t uint32_t;
typedef u_int64_t uint64_t;

#  define INT32_MIN  INT_MIN
#  define INT32_MAX  INT_MAX
#  define UINT32_MAX UINT_MAX
#  define INT64_MIN  LONG_LONG_MIN
#  define INT64_MAX  LONG_LONG_MAX
#  define UINT64_MAX ULONG_LONG_MAX

#endif /* __CYGWIN__ */

#endif /* !_LINUX_KERNEL */

#ifndef _UNIX
#  define _UNIX
#endif /* _UNIX */

#ifndef __hpux__
#  define HAVE_STRTOLL
#  define HAVE_STRTOULL
#  ifdef _LINUX_KERNEL
#    define strtol(_cp,_endp,_base) simple_strtol(_cp,_endp,_base) 
#    define strtoul(_cp,_endp,_base) simple_strtoul(_cp,_endp,_base) 
#    define strtoll(_cp,_endp,_base) simple_strtoll(_cp,_endp,_base) 
#    define strtoull(_cp,_endp,_base) simple_strtoull(_cp,_endp,_base) 
#  endif /* _LINUX_KERNEL */
#endif /* __hpux__ */

/* make sure BYTE_ORDER is properly defined */
#ifndef BYTE_ORDER
#  if (BSD >= 199103)
#    include <machine/endian.h>
#  else
#    ifdef __linux__
#      include <endian.h>
#    else
#      define LITTLE_ENDIAN 1234 /* least-significant byte first (vax, pc) */
#      define BIG_ENDIAN    4321 /* most-significant byte first (IBM, net) */
#      define PDP_ENDIAN    3412 /* LSB first in word, MSW first in long */
       /* these platforms are known to be little endian */
#      if defined(vax) || defined(ns32000) || defined(sun386) || \
          defined(i386) || defined(__ia64) || defined(MIPSEL) || \
          defined(_MIPSEL) || defined(BIT_ZERO_ON_RIGHT) || \
          defined(__alpha__) || defined(__alpha)
#        define BYTE_ORDER LITTLE_ENDIAN
#      endif
       /* these platforms are known to be big endian */
#      if defined(sel) || defined(pyr) || defined(mc68000) || \
          defined(sparc) || defined(is68k) || defined(tahoe) || \
          defined(ibm032) || defined(ibm370) || defined(MIPSEB) || \
          defined(_MIPSEB) || defined(_IBMR2) || defined(DGUX) || \
          defined(apollo) || defined(__convex__) || defined(_CRAY) || \
          defined(__hppa) || defined(__hp9000) || defined(__hp9000s300) || \
          defined(__hp9000s700) || defined(BIT_ZERO_ON_LEFT) || defined(m68k)
#        define BYTE_ORDER BIG_ENDIAN
#      endif
#    endif /* linux */
#  endif /* BSD */
#endif /* BYTE_ORDER */

/* final sanity check */
#if !defined(BYTE_ORDER) || \
     (BYTE_ORDER != BIG_ENDIAN && \
      BYTE_ORDER != LITTLE_ENDIAN && \
      BYTE_ORDER != PDP_ENDIAN)

  /*
   * you must determine what the correct byte order is for
   * your compiler - the next line is an intentional error
   * which will force your compiles to bomb until you fix
   * the above macros.
   */
#  error "Undefined or invalid BYTE_ORDER";
#endif

/* Linux defines this but other platforms may not */
#ifndef __INT64_C 
#  define __INT64_C(c) c##LL 
#endif /* __INT64_C  */

/* 
 * __pthread_initialize() used to be in some Linux header files, but now 
 * it's gone, at least from RedHat's headers.
 */
#ifndef HAVE_PTHREAD_INITIALIZE
#  define __pthread_initialize() NOTHING
#endif /* HAVE_PTHREAD_INITIALIZE */

/* this needs some investigation - what are the most portable formats? */

#if defined(__linux__)
#  define LONG_HEX_FORMAT "%LX"
#  define LONG_LONG_FORMAT "%Ld"
#  define LONG_ULONG_FORMAT "%Lu"
#else
#  define LONG_HEX_FORMAT "%llX"
#  define LONG_LONG_FORMAT "%lld"
#  define LONG_ULONG_FORMAT "%llu"
#endif

/* define __LONG_64__ if long is 64 bits */ 

#ifdef __alpha__
#  define __LONG_64__
#elif defined(__sun) && defined(_LP64)
#  define __LONG_64__
#endif

#ifdef __LONG_64__
#  define I64X_FORMAT "%lX"
#  define I64S_FORMAT "%ld"
#  define I64U_FORMAT "%lu"
#  ifndef __INT64_C
#    define __INT64_C(c) c ## L
#  endif /* __INT64_C */
#  ifndef __UINT64_C
#    define __UINT64_C(c) c ## UL
#  endif /* __UINT64_C */
#else /* !__LONG_64__ */
#  define I64X_FORMAT LONG_HEX_FORMAT
#  define I64S_FORMAT LONG_LONG_FORMAT
#  define I64U_FORMAT LONG_ULONG_FORMAT
#  ifndef __INT64_C
#    define __INT64_C(c) c ## LL
#  endif /* __INT64_C */
#  ifndef __UINT64_C
#    define __UINT64_C(c) c ## ULL
#  endif /* __UINT64_C */
#endif /* !__LONG_64__ */

#ifdef _LINUX_KERNEL
#  define SCHAR_MIN  (-128)
#  define SCHAR_MAX  127
#  define UCHAR_MAX  255
#  ifdef __CHAR_UNSIGNED__
#    define CHAR_MIN 0
#    define CHAR_MAX UCHAR_MAX
#  else
#    define CHAR_MIN SCHAR_MIN
#    define CHAR_MAX SCHAR_MAX
#  endif
#  define SHRT_MIN   (-32768)
#  define SHRT_MAX   32767
#  define USHRT_MAX  65535
#  define UINT32_MAX UINT_MAX
#  define INT32_MAX  INT_MAX
#  define INT32_MIN  INT_MIN
#  define UINT64_MAX (~(__UINT64_C(0)))
#  define INT64_MAX  ((int64_t)(~(UINT64_MAX)>>1))
#  define INT64_MIN  (-INT64_MAX - 1)
#endif /* _LINUX_KERNEL */

#define READ_TEXT_MODE "r"
#define WRITE_TEXT_MODE "w"
#define APPEND_TEXT_MODE "a+"
#define READ_BINARY_MODE "r"
#define WRITE_BINARY_MODE "w"
#define APPEND_BINARY_MODE "a+"

#define FILE_SEPARATOR "/"
#define FILE_SEPARATOR_CHAR '/'
#define PATH_SEPARATOR ":"
#define PATH_SEPARATOR_CHAR ':'

typedef int Socket;
#define INVALID_SOCKET (-1)
#define closesocket(_s) close(_s) 
#define ioctlsocket(_s,_cmd,_argp)  ioctl(_s,_cmd,(char*)(_argp))

/* the word that has same size as a pointer */
typedef unsigned long PtrWord;

/* character type */
typedef char Char;

/* string manipulation routines */
#define StrLen      strlen
#define StrCmp      strcmp
#define StrnCmp     strncmp
#define StrCpy      strcpy
#define StrCat      strcat
#define StrChr      strchr
#define StrStr      strstr
#define StrrChr     strrchr
#define StrnCpy     strncpy
#define StrpBrk     strpbrk
#define StrTod      strtod
#define StrTol      strtol
#define StrToul     strtoul
#define StrCaseCmp  strcasecmp
#define StrnCaseCmp strncasecmp
#define Vsnprintf   vsnprintf
#define Vfprintf    vfprintf
#define Sprintf     sprintf

/* character classification functions */
#define IsUpper(x)  isupper((int)(x))
#define IsLower(x)  islower((int)(x))
#define IsPrint(x)  isprint((int)(x))
#define IsDigit(x)  isdigit((int)(x))
#define IsXdigit(x) isxdigit((int)(x))
#define IsAlpha(x)  isalpha((int)(x))
#define IsSpace(x)  isspace((int)(x))
#define ToUpper(x)  toupper((int)(x))
#define ToLower(x)  tolower((int)(x))

/* other functions */
#define Ctime       ctime
#define Fopen       fopen
#define Freopen     freopen
#define Rmdir       rmdir

/* don't need UNICODE on UNIX... at least not yet */
#define TEXT(x) x
#define TEXT_(x) TEXT(x)

/* memory allocation primitives provided by the system */
#ifdef _LINUX_KERNEL
#  define OS_MemAlloc(_size) kmalloc(_size,GFP_KERNEL)
#  define OS_MemFree(_ptr) kfree(_ptr)
#  define NO_REALLOC
#else /* _LINUX_KERNEL */
#  define OS_MemAlloc(_size) malloc(_size)
#  define OS_MemRealloc(_ptr,_size) realloc((_ptr),(_size))
#  define OS_MemFree(_ptr) free(_ptr)
#endif /* _LINUX_KERNEL */

#endif /* _SLAVA_UNIX_H_ */

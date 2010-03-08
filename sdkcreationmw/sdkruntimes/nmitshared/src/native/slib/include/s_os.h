/*
 * $Id: s_os.h,v 1.46 2006/03/07 16:42:57 slava Exp $
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

#ifndef _SLAVA_OS_H_
#define _SLAVA_OS_H_

#ifdef _NT_KERNEL
#define _CRTIMP
#endif /* _NT_KERNEL */

#if defined __SLIB_SYMBIAN32__
#  ifndef __SYMBIAN32__
#    error "__SYMBIAN32__ is expected to be defined"
#  endif /* __SYMBIAN32__ */
#  undef _WIN32
#else  /* !__SLIB_SYMBIAN32__ */
#  if !defined(_WIN32) && defined(__WINS__)
#    define _WIN32 /* CodeWarrior for Symbian */
#  endif /* !_WIN32 && __WINS__ */
#endif /* !__SLIB_SYMBIAN32__ */

#if defined(_WIN32) && !defined(__STDC__)
#  define __STDC__ 1
#endif /* _WIN32 && !__STDC__ */

#if defined(__linux__) && defined(__KERNEL__)
#  define _LINUX_KERNEL 1
#endif

/* compile-time assertion */
#define COMPILE_ASSERT(x) extern int _compile_time_assert[2*(x)-1];

/* anything still does not support arguments in function prototypes? */
#define P_(prot) prot

#if defined(_NT_KERNEL)
#  include "s_ntk.h"
#elif defined(_WIN32)
#  include "s_win32.h"
#elif defined(__SYMBIAN32__)
#  include "s_epoc.h"
#else /* !_WIN32 */
#  include "s_unix.h"
#endif /* !WIN32 */

/*
 * ALLPERMS - 777 mode
 */
#ifndef ALLPERMS
#  define ALLPERMS (S_IRWXU | S_IRWXG | S_IRWXO)
#endif /* ALLPERMS */

/* IP address and port */
typedef uint32_t IPaddr;
typedef unsigned short Port; 

/*
 * define INADDR_NONE
 * some platforms don't define it
 */
#ifndef INADDR_NONE
#  define INADDR_NONE ((IPaddr)-1)
#endif /* INADDR_NONE */

/*
 * Network byte order
 */
#define NET_BYTE_ORDER  BIG_ENDIAN

/* 
 * representing numbers in network byte order.
 * b0 is the least significant byte.
 */
#if BYTE_ORDER == LITTLE_ENDIAN
#  define NET_INT32(b3,b2,b1,b0) (\
    ((((uint32_t)(b3)))       & ((uint32_t)0x000000ff)) | \
    ((((uint32_t)(b2)) << 8)  & ((uint32_t)0x0000ff00)) | \
    ((((uint32_t)(b1)) << 16) & ((uint32_t)0x00ff0000)) | \
    ((((uint32_t)(b0)) << 24) & ((uint32_t)0xff000000)))
#  define NET_INT16(b1,b0) (\
    ((((uint16_t)(b1)))       & ((uint16_t)0x00ff)) | \
    ((((uint16_t)(b0)) << 8)  & ((uint16_t)0xff00))
#elif BYTE_ORDER == BIG_ENDIAN
#  define NET_INT32(b3,b2,b1,b0) (\
    ((((uint32_t)(b0)))       & ((uint32_t)0x000000ff)) | \
    ((((uint32_t)(b1)) << 8)  & ((uint32_t)0x0000ff00)) | \
    ((((uint32_t)(b2)) << 16) & ((uint32_t)0x00ff0000)) | \
    ((((uint32_t)(b3)) << 24) & ((uint32_t)0xff000000)))
#  define NET_INT16(b1,b0) (\
    ((((uint16_t)(b0)))       & ((uint16_t)0x00ff)) | \
    ((((uint16_t)(b1)) << 8)  & ((uint16_t)0xff00))
#else
#  error "Please fix BYTE_ORDER"
#endif /* BYTE_ORDER */

/*
 * the same, but using hex numbers. For example, 
 * NET_HEX32(63,82,53,63) gets translated into 
 * NET_INT32(0x63,0x82,0x53,0x63)
 */
#define NET_HEX32(b3,b2,b1,b0) NET_INT32(0x##b3,0x##b2,0x##b1,0x##b0)
#define NET_HEX16(b1,b0)       NET_INT16(0x##b1,0x##b0)

/* 
 * useful macros for IPv4 address formatting.
 */
#define BYTE_0(_i32) ((uint8_t)((_i32)  & 0x000000ff))
#define BYTE_1(_i32) ((uint8_t)(((_i32) & 0x0000ff00) >> 8))
#define BYTE_2(_i32) ((uint8_t)(((_i32) & 0x00ff0000) >> 16))
#define BYTE_3(_i32) ((uint8_t)(((_i32) & 0xff000000) >> 24))

/* 
 * IPv4 address in host byte order.
 */
#define HOST_IPADDR(b3,b2,b1,b0) (\
    ((((IPaddr)(b0)))       & 0x000000ff) | \
    ((((IPaddr)(b1)) << 8)  & 0x0000ff00) | \
    ((((IPaddr)(b2)) << 16) & 0x00ff0000) | \
    ((((IPaddr)(b3)) << 24) & 0xff000000))

/*
 * IPv4 address formatting
 */ 
#define IPADDR_FORMAT "%u.%u.%u.%u"

/* IPv4 address in the host byte order */
#define HOST_IPADDR_FORMAT_ARG(_i32) \
    (unsigned int)BYTE_3(_i32), \
    (unsigned int)BYTE_2(_i32), \
    (unsigned int)BYTE_1(_i32), \
    (unsigned int)BYTE_0(_i32)

/* IPv4 address in network byte order */
#define NET_IPADDR(b3,b2,b1,b0) NET_INT32(b3,b2,b1,b0)
#if BYTE_ORDER == LITTLE_ENDIAN
#  define NET_IPADDR_FORMAT_ARG(_i32) \
    (unsigned int)BYTE_0(_i32), \
    (unsigned int)BYTE_1(_i32), \
    (unsigned int)BYTE_2(_i32), \
    (unsigned int)BYTE_3(_i32)
#elif BYTE_ORDER == BIG_ENDIAN
#  define NET_IPADDR_FORMAT_ARG(_i32) HOST_IPADDR_FORMAT_ARG(_i32)
#endif /* BYTE_ORDER */

/* alias to NET_IPADDR_FORMAT_ARG for backward compatibility */
#define IPADDR_FORMAT_ARG(_i32) NET_IPADDR_FORMAT_ARG(_i32)

/* compile-time assertion that PtrWord actually has the size of a pointer */
COMPILE_ASSERT(sizeof(PtrWord) == sizeof(void*))

#endif /* _SLAVA_OS_H_ */

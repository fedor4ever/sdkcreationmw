/*
 * $Id: s_buf.h,v 1.27 2005/10/19 21:11:09 slava Exp $
 *
 * Copyright (C) 2000-2005 by Slava Monich
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

#ifndef _SLAVA_BUFFER_H_
#define _SLAVA_BUFFER_H_

#include "s_def.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef short ByteOrder;

/*
 * Buffer of binary data. Function are provided to store the data in
 * or retrieve from the buffer in FIFO (first in - first out) manner.
 * The buffer can have internal storage of fixed or variable (possibly,
 * limited) size. Buffer will automatically resize itself if necessary,
 * and it's allowed to do so. If the buffer is not allowed to resize,
 * a put (store) operation will fail if the buffer is full.
 *
 * NOTE that start offset is INCLUSIVE, while end offset is EXCLUSIVE. 
 * If start offset equals the end offset, it means that buffer is either
 * empty or full. To distinguish between these two cases, we use the 
 * BUFFER_FULL flag. There's one case though, when buffer is full but 
 * start and end offsets MAY NOT be equal; that's when start offset is 
 * zero. In that case, end offset MAY be equal the buffer's allocated 
 * size, and BUFFER_FULL flag may or may not be set.
 */
typedef struct _Buffer {
    I8u * data;                 /* data storage */
    int alloc;                  /* allocated buffer space */
    int maxsiz;                 /* max space to allocate, -1 if no limit */
    int start;                  /* start offset (inclusive) */
    int end;                    /* end offset (exclusive) */
    ByteOrder order;            /* byte order */
    short flags;                /* flags, see below: */

#define BUFFER_OWN_DATA 0x0001  /* BUFFER_Destroy() deallocates the data */
#define BUFFER_READONLY 0x0002  /* cannot write into the buffer */
#define BUFFER_FULL     0x0004  /* buffer is full */

} Buffer;

/* operations on buffer */
extern Buffer * BUFFER_Create P_((void));
extern Buffer * BUFFER_Create2 P_((void * buf, int size));
extern Buffer * BUFFER_CreateRead P_((const void * buf, int size));
extern Buffer * BUFFER_CreateWrite P_((void * buf, int size, Bool ownBuf));
extern void BUFFER_Delete P_((Buffer * b));

extern void BUFFER_Init P_((Buffer * b));
extern void BUFFER_Init2 P_((Buffer * b, void * buf, int size));
extern void BUFFER_InitRead P_((Buffer * b, const void * buf, int size));
extern void BUFFER_InitWrite P_((Buffer * b, void * buf, int size, Bool own));
extern void BUFFER_Destroy P_((Buffer * b));
extern int  BUFFER_Size P_((const Buffer * b));
extern void BUFFER_Clear P_((Buffer * b));
extern Bool BUFFER_EnsureCapacity P_((Buffer * b, int minsize, Bool partOK));
extern Bool BUFFER_IsReadOnly P_((const Buffer * b));

/* prepares buffer for direct access */
extern void * BUFFER_Access P_((Buffer * b));

/* opaque data, no conversion */
extern int  BUFFER_Get P_((Buffer * b, void * data, int size));
extern int  BUFFER_Put P_((Buffer * b, const void * data, int n, Bool part));
extern int  BUFFER_Unput P_((Buffer * b, int n));
extern Bool BUFFER_PushBack P_((Buffer * b, const void * data, int n));
extern int  BUFFER_Move P_((Buffer * b, Buffer * dest, int size));

/* moving start position back and forth */
#define BUFFER_Skip(_b,_size)  BUFFER_Get(_b,NULL,_size)
#define BUFFER_Unget(_b,_size) BUFFER_PushBack(_b,NULL,_size)

/* byte order conversion */
extern Bool BUFFER_PutI8 P_((Buffer * b, I8s data));
extern Bool BUFFER_PutI16 P_((Buffer * b, I16s data));
extern Bool BUFFER_PutI32 P_((Buffer * b, I32s data));
extern Bool BUFFER_PutI64 P_((Buffer * b, I64s data));
extern Bool BUFFER_GetI8 P_((Buffer * b, I8s * data));
extern Bool BUFFER_GetU8 P_((Buffer * b, I8u * data));
extern Bool BUFFER_GetI16 P_((Buffer * b, I16s * data));
extern Bool BUFFER_GetU16 P_((Buffer * b, I16u * data));
extern Bool BUFFER_GetI32 P_((Buffer * b, I32s * data));
extern Bool BUFFER_GetU32 P_((Buffer * b, I32u * data));
extern Bool BUFFER_GetI64 P_((Buffer * b, I64s * data));
extern Bool BUFFER_GetU64 P_((Buffer * b, I64u * data));

/* aliases for native types */
#define BUFFER_PutByte(_b,_d)   BUFFER_PutI8(_b,_d)
#define BUFFER_PutChar(_b,_d)   BUFFER_PutI8(_b,_d)
#define BUFFER_PutShort(_b,_d)  BUFFER_PutI16(_b,_d)
#define BUFFER_PutInt(_b,_d)    BUFFER_PutI32(_b,_d)

#define BUFFER_GetByte(_b,_d)   BUFFER_GetI8(_b,_d)
#define BUFFER_GetChar(_b,_d)   BUFFER_GetI8(_b,_d)
#define BUFFER_GetUChar(_b,_d)  BUFFER_GetU8(_b,_d)
#define BUFFER_GetShort(_b,_d)  BUFFER_GetI16(_b,_d)
#define BUFFER_GetUShort(_b,_d) BUFFER_GetU16(_b,_d)
#define BUFFER_GetInt(_b,_d)    BUFFER_GetI32(_b,_d)
#define BUFFER_GetUInt(_b,_d)   BUFFER_GetU32(_b,_d)

#ifdef __LONG_64__
#  define BUFFER_PutLong(_b,_d) BUFFER_PutI64(_b,_d)
#  define BUFFER_GetLong(_b,_d) BUFFER_GetI64(_b,_d)
#  define BUFFER_GetULong(_b,d) BUFFER_GetU64(_b,d)
#else  /* !__LONG_64__ */
#  define BUFFER_PutLong(_b,_d) BUFFER_PutInt(_b,_d)
#  define BUFFER_GetLong(_b,_d) BUFFER_GetInt(_b,_d)
#  define BUFFER_GetULong(_b,d) BUFFER_GetUInt(_b,d)
#endif /* !__LONG_64__ */

/* byte order converters */
I16u DATA_Conv16 P_((I16u data, int from, int to));
I32u DATA_Conv32 P_((I32u data, int from, int to));
I64u DATA_Conv64 P_((I64u data, int from, int to));

/* redefine ntohs and ntohl as macros */

#undef ntohs
#undef ntohl
#undef htons
#undef htonl

#if BYTE_ORDER == BIG_ENDIAN
#  define ntohs(_x)  (_x)
#  define ntohl(_x)  (_x)
#  define htons(_x)  (_x)
#  define htonl(_x)  (_x)
#else
#  define ntohs(_x)  DATA_Conv16(_x,BIG_ENDIAN,BYTE_ORDER)
#  define ntohl(_x)  DATA_Conv32(_x,BIG_ENDIAN,BYTE_ORDER)
#  define htons(_x)  DATA_Conv16(_x,BYTE_ORDER,BIG_ENDIAN)
#  define htonl(_x)  DATA_Conv32(_x,BYTE_ORDER,BIG_ENDIAN)
#endif
 
#ifdef __cplusplus
} /* end of extern "C" */
#endif  /* __cplusplus */

#endif /* _SLAVA_BUFFER_H_ */

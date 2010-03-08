/*
 * $Id: s_strbuf.h,v 1.28 2005/10/19 22:35:16 slava Exp $
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

#ifndef _SLAVA_STRBUF_H_
#define _SLAVA_STRBUF_H_

#include "s_def.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/*
 * string buffer of variable length.
 */
typedef struct _StrBuf {
    size_t len;                 /* current length (no need for strlen()) */
    size_t alloc;               /* available size */
    Char * s;                   /* current buffer */
    Char * ext;                 /* external buffer (typically, on stack) */
} StrBuf;

extern StrBuf * STRBUF_Create P_((void));
extern void STRBUF_Delete P_((StrBuf * sb));
extern void STRBUF_Init P_((StrBuf * sb));
extern void STRBUF_InitBuf P_((StrBuf * sb, Char* buf, size_t bufsize));
extern void STRBUF_InitBuf2 P_((StrBuf* sb, Char* buf, size_t len, size_t n));
extern void STRBUF_Destroy P_((StrBuf * sb));
extern size_t STRBUF_Length P_((const StrBuf * sb));
extern void STRBUF_SetLength P_((StrBuf * sb, size_t maxlen));
extern Bool STRBUF_Equals P_((const StrBuf * sb1, const StrBuf * sb2));
extern Bool STRBUF_EqualsTo P_((const StrBuf * sb, Str s));
extern Char STRBUF_CharAt P_((const StrBuf * sb, size_t pos));
extern Char STRBUF_FirstChar P_((const StrBuf * sb));
extern Char STRBUF_LastChar P_((const StrBuf * sb));
extern int  STRBUF_IndexOf P_((const StrBuf * sb, Char ch));
extern int  STRBUF_LastIndexOf P_((const StrBuf * sb, Char ch));
extern int  STRBUF_Find P_((const StrBuf * sb, Str s));
extern int  STRBUF_FindNoCase P_((const StrBuf * sb, Str s));
extern int  STRBUF_FindLast P_((const StrBuf * sb, Str s));
extern int  STRBUF_FindLastNoCase P_((const StrBuf * sb, Str s));
extern int  STRBUF_FindFrom P_((const StrBuf * sb, size_t pos, Str s));
extern int  STRBUF_FindFromNoCase P_((const StrBuf * sb, size_t pos, Str s));
extern int  STRBUF_Replace P_((StrBuf * sb, Char c1, Char c2));
extern int  STRBUF_ReplaceStr P_((StrBuf * sb, Str from, Str to));
extern Str  STRBUF_GetString P_((const StrBuf * sb));
extern Bool STRBUF_StartsWith P_((const StrBuf * sb, Str s));
extern Bool STRBUF_EndsWith P_((const StrBuf * sb, Str s));
extern Bool STRBUF_StartsWithNoCase P_((const StrBuf * sb, Str s));
extern Bool STRBUF_EndsWithNoCase P_((const StrBuf * sb, Str s));
extern void STRBUF_ToUpperCase P_((StrBuf * sb));
extern void STRBUF_ToLowerCase P_((StrBuf * sb));
extern Bool STRBUF_Alloc P_((StrBuf * sb, size_t minlen));
extern void STRBUF_Clear P_((StrBuf * sb));
extern void STRBUF_Erase P_((StrBuf * sb, size_t from, size_t to));
extern Bool STRBUF_Trim P_((StrBuf * sb));
extern Bool STRBUF_Copy P_((StrBuf * sb, Str s));
extern Bool STRBUF_CopyN P_((StrBuf * sb, Str s, size_t size));
extern Bool STRBUF_Append P_((StrBuf * sb, Str s));
extern Bool STRBUF_AppendN P_((StrBuf * sb, Str s, size_t n));
extern Bool STRBUF_AppendBuf P_((StrBuf * sb1, const StrBuf * sb2));
extern Bool STRBUF_AppendInt P_((StrBuf * sb, int i));
extern Bool STRBUF_AppendChar P_((StrBuf * sb, Char c));
extern Bool STRBUF_AppendBool P_((StrBuf * sb, Bool b));
extern Bool STRBUF_AppendDouble P_((StrBuf * sb, double d));
extern Bool STRBUF_AppendFormat P_((StrBuf * sb, Str format, ...));
extern Bool STRBUF_AppendFormatVa P_((StrBuf * sb, Str format, va_list va));
extern Bool STRBUF_Inflate P_((StrBuf * sb, size_t len, Char fill));
extern Bool STRBUF_Insert P_((StrBuf * sb, Str s, size_t pos));
extern Bool STRBUF_InsertN P_((StrBuf * sb, size_t pos, Str s, size_t len));
extern Bool STRBUF_InsertChar P_((StrBuf * sb, Char c, size_t pos));
extern Bool STRBUF_Format P_((StrBuf * sb, Str format, ...));
extern Bool STRBUF_FormatVa P_((StrBuf * sb, Str format, va_list va));
extern Char * STRBUF_Dup P_((const StrBuf * sb));

#ifndef _WIN32_WCE
extern Bool STRBUF_AppendTime P_((StrBuf * sb, Time t));
extern Bool STRBUF_FormatTime P_((StrBuf * sb, Time t));
#endif /* _WIN32_WCE */

/* 
 * STRBUF_Text is somewhat like STRBUF_GetString but it's a macro in release 
 * build and therefore is more efficient. Another difference of STRBUF_Text 
 * macro from STRBUF_GetString function is that it does not check if StrBuf 
 * pointer is NULL. 
 */
#if DEBUG
#   define STRBUF_Text(_sb) STRBUF_GetString(_sb)
#else  /* DEBUG */
#   define STRBUF_Text(_sb) ((_sb)->len ? (_sb)->s : TEXT(""))
#endif /* DEBUG */

/*
 * these variants include a fixed size buffer as well
 */
typedef struct _StrBuf16 {
    Char buf[16];
    StrBuf sb;
} StrBuf16;

typedef struct _StrBuf32 {
    Char buf[32];
    StrBuf sb;
} StrBuf32;

typedef struct _StrBuf64 {
    Char buf[64];
    StrBuf sb;
} StrBuf64;

typedef struct _StrBuf128 {
    Char buf[128];
    StrBuf sb;
} StrBuf128;

typedef struct _StrBuf256 {
    Char buf[256];
    StrBuf sb;
} StrBuf256;

#define STRBUF_InitBufXXX(b) STRBUF_InitBuf(&(b)->sb,(b)->buf,COUNT((b)->buf))

/* Macro to initialize StrBuf structure to point to a string constant. */
#define STRBUF_INIT(_string) { \
    COUNT(_string)-1,   /* current length */    \
    COUNT(_string),     /* available size */    \
    _string,            /* current buffer */    \
    _string             /* external buffer */   \
}
 
#ifdef __cplusplus
} /* end of extern "C" */
#endif  /* __cplusplus */

#endif /* _SLAVA_STRBUF_H_ */

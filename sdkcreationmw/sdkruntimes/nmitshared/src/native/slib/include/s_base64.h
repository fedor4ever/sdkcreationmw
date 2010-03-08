/*
 * $Id: s_base64.h,v 1.3 2004/08/18 02:43:20 slava Exp $
 *
 * Copyright (C) 2000-2004 by Slava Monich
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

#ifndef _SLAVA_BASE64_H_
#define _SLAVA_BASE64_H_

#include "s_buf.h"
#include "s_file.h"
#include "s_strbuf.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/*
 * Flags for the encoding functions
 */
#define BASE64_PAD      0x01    /* write the trailing pad characters */
#define BASE64_URLSAFE  0x02    /* use "URL and  filename safe" encoding */

/*
 * BASE64 encoding functions. The last argument to both functions is the
 * flags parameter. The individual flags are defined above.
 */
extern Char * BASE64_Encode P_((const void * data, int n, int flags));
extern Str    BASE64_EncodeStr P_((const void * data,int n,StrBuf * sb,int f));

/*
 * BASE64 decoding functions. The "Safe" functions use the "URL and 
 * filename safe" BASE64 encoding defined (or rather proposed) in RFC3548.
 */
extern Bool   BASE64_Decode P_((Str base64, Buffer * out));
extern Bool   BASE64_StdDecode P_((Str base64, Buffer * out));
extern Bool   BASE64_SafeDecode P_((Str base64, Buffer * out));
extern Bool   BASE64_DecodeFile P_((File * in, Buffer * out));
extern Bool   BASE64_StdDecodeFile P_((File * in, Buffer * out));
extern Bool   BASE64_SafeDecodeFile P_((File * in, Buffer * out));

#ifdef __cplusplus
} /* end of extern "C" */
#endif  /* __cplusplus */

#endif /* _SLAVA_BASE64_H_ */

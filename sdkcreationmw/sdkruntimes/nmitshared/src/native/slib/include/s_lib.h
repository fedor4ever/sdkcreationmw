/*
 * $Id: s_lib.h,v 1.23 2004/08/18 02:52:01 slava Exp $
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

#ifndef _SLAVA_LIB_H_
#define _SLAVA_LIB_H_

#include "s_ver.h"
#include "s_def.h"
#include "s_mem.h"
#include "s_buf.h"
#include "s_math.h"
#include "s_base32.h"
#include "s_base64.h"
#include "s_bitset.h"
#include "s_random.h"
#include "s_util.h"
#include "s_strbuf.h"
#include "s_hash.h"
#include "s_vector.h"
#include "s_queue.h"
#include "s_stack.h"
#include "s_prop.h"
#include "s_file.h"
#include "s_hist.h"
#include "s_mutex.h"
#include "s_itr.h"
#include "s_md.h"

#ifndef _LINUX_KERNEL
#include "s_thread.h"
#include "s_event.h"
#include "s_lock.h"
#include "s_opt.h"
#include "s_wkq.h"
#include "s_xml.h"
#include "s_dom.h"
#endif /* _LINUX_KERNEL */

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/*
 * SLIB_InitModules() and SLIB_Init() do the same thing, the only 
 * difference is that Win32 version of SLIB_Init() initializes 
 * Winsock, while SLIB_InitModules() does not. On any other platform
 * these two functions are absolutely equivalent.
 */
extern void SLIB_Init P_((void));
extern void SLIB_InitModules P_((void));
extern void SLIB_Shutdown P_((void));

#ifdef __cplusplus
} /* end of extern "C" */
#endif  /* __cplusplus */

#endif /* _SLAVA_LIB_H_ */

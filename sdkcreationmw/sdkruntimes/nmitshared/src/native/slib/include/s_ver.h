/*
 * $Id: s_ver.h,v 1.27 2006/04/13 12:22:53 slava Exp $
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

#ifndef _SLAVA_VERSION_H_
#define _SLAVA_VERSION_H_

/* NOTE: we have room for micro version, but not using it yet */
#define SLIB_MAKE_VERSION(major,minor) (((major)<<16)|(((minor)&0xff)<<8))

/* 
 * SLIB_VERSION macro defines the current version of SLIB.
 *
 * NOTE: stable (released) builds have even minor versions, e.g. 1.18, 1.20,
 * etc. Development builds (such as the ones that you get directly from CVS)
 * have odd minor versions, e.g. 1.19, 1.21 and so on. The SLIB_VERSION
 * macro first appeared in SLIB release 1.18
 */
#define SLIB_VERSION_MAJOR  1
#define SLIB_VERSION_MINOR  44
#define SLIB_VERSION SLIB_MAKE_VERSION(SLIB_VERSION_MAJOR,SLIB_VERSION_MINOR)

#endif /* _SLAVA_VERSION_H_ */

/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/
/* File    : U:\wpsvob\wmlenc\src\include\xmlhost.h
   Version : \main\8
   Date    : 17-Nov-00.11:05:33
*/

/*
 * ========================================================================
 *  Name        : xmlhost.h
 *  Part of     : 
 *  Description : 
 *  Version     : 
 *
 *  Copyright (c) 2000 - 2005 Nokia Corporation.
 *  This material, including documentation and any related
 *  computer programs, is protected by copyright controlled by
 *  Nokia Corporation. All rights are reserved. Copying,
 *  including reproducing, storing, adapting or translating, any
 *  or all of this material requires the prior written consent of
 *  Nokia Corporation. This material also contains confidential
 *  information which may not be disclosed to others without the
 *  prior written consent of Nokia Corporation.
 * ========================================================================
 */

/*
-----------------------------------------------------------------------------

    DESCRIPTION

    This header file conteins the following main components:

-----------------------------------------------------------------------------
*/

#ifndef XMLHOST_H
#define XMLHOST_H

#include <assert.h>     /* NULL assert (include after the NDEBUG define) */
#include <string.h>     /* strstr strchr strcat memcpy memset */
#include <stdio.h>      /* printf sprintf sscanf */
#include <stdlib.h>     /* malloc free */
#include <ctype.h>      /* toupper */

#ifdef WML_DEBUG
#include <stdarg.h>
#endif


#include "wpss_type.h"
#include "wmlapi.h"

/*  CONSTANTS  */

/*  MACROS  */



#ifndef WML_DEBUG

#define DBG_ASSERT(ignore)   {((void)0);}
#define DBG_LOG(a)           {((void)0);}
#define DBG_LOG2(ignore)     {((void)0);}

#else /* WML_DEBUG */

#define DBG_ASSERT(a)        {assert(a);}
#define DBG_LOG(a)           {dPrint a ;}
#define DBG_LOG2(a)          {dPrint2 a ;}

#define DBG_RECV     0x0001   /* URLLoad: rcv() */
#define DBG_XTOK     0x0002   /* TokenHandler() */
#define DBG_WBX      0x0004   /* WBXML */
#define DBG_ENC      0x0008   /* Encoding */
#define DBG_CACHE    0x0010   /* BufferCache */
#define DBG_WML      0x0020   /* wmlvalid */
#define DBG_SYM      0x0040   /* XML symbol tables */
#define DBG_XML      0x0080   /* XML general */
#define DBG_PP       0x0100   /* XML Push/Pop */
#define DBG_POOL     0x0200   /* XML Pool */
#define DBG_STR      0x0400   /* XML string */
#define DBG_PAR      0x0800   /* XML parser */
#define DBG_MEM      0x1000   /* MEM  */

#endif  /* WML_DEBUG */


#define XML_ALLOC(s)    xmlMalloc(s)
#define XML_FREE(p)     xmlFree(p)

#ifdef _DBGMEM
#define MEM_DBG_INIT   {initMemDebug();}
#define MEM_DBG_TERM   {termMemDebug();}
#else   /* _DBGMEM */
#define MEM_DBG_INIT   {((void)0);}
#define MEM_DBG_TERM   {((void)0);}
#endif  /* _DBGMEM */

/*  DATA TYPES  */

typedef UINT16 WCHAR;

typedef struct String_s * _PString;

/*  EXTERNAL DATA STRUCTURES  */

/*  FUNCTION PROTOTYPES  */

#ifdef _DEBUG

void        wmlLOGs
                (int          level,
                 const char * s1);
                

void        wmlLOGss
                (int          level,
                 const char * s1,
                 const char * s2);
                
void        wmlLOGsss
                (int          level,
                 const char * s1,
                 const char * s2,
                 const char * s3);
                
void        wmlLOGssss
                (int          level,
                 const char * s1,
                 const char * s2,
                 const char * s3,
                 const char * s4);
                
#else  /* _DEBUG */

/* no need for trace in release build */
#  define wmlLOGs(level,s1)                 ((void)0)
#  define wmlLOGss(level,s1,s2)             ((void)0)
#  define wmlLOGsss(level,s1,s2,s3)         ((void)0)
#  define wmlLOGssss(level,s1,s2,s3,s4)     ((void)0)

#endif /* _DEBUG */

void *      xmlMalloc
                (UINT32 siz);
                
void *      xmlCalloc
                (UINT32 nelem,
                 UINT32 elemsiz);
                
void        xmlFree
                (void *ptr);

#ifdef _DBGMEM
void  initMemDebug();
void  termMemDebug();
#endif  /* _DBGMEM */


#ifdef WML_DEBUG
void dPrint(int flg, char *fmt, ...);
void dPrint2(int flg, _PString str, char *fmt, ...);
#endif

#endif  /* XMLHOST_H */

/*  End of File: xmlhost.h  */


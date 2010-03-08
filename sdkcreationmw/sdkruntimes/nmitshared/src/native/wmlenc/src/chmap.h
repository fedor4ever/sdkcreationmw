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
/* File    : U:\wpsvob\wmlenc\src\include\chmap.h
   Version : \main\7
   Date    : 17-Oct-00.12:50:25
*/

/*
 * ========================================================================
 *  Name        : chmap.h
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

    This header file is used internally by the WML encoder.
-----------------------------------------------------------------------------
*/
#ifndef CHMAP_H
#define CHMAP_H

#include "xmlsymt.h"

/*  CONSTANTS  */

/*  MACROS  */

#define CHCONVERT(tbl,ch,ret) {\
    SCharNode *_n = tbl[ch&0xff].m_root;\
    ret = (_n && _n->m_from==ch) ? _n->m_to : getChFromList(_n,ch);\
}

/*  DATA TYPES  */

typedef enum {
    CF_BYTE,
    CF_UTF8,
    CF_UTF16,
    CF_UCS4,
    eInvalidCharFormat
}ECharFormat;

typedef struct CharNode_s
{
  WCHAR              m_from;
  WCHAR              m_to;
  struct CharNode_s *m_next;
}SCharNode;

DECLARE_SLIST(SCharNode);

typedef struct SCharMap_s
{
  struct SCharMap_s *m_next;         /* List of maps */
  int                m_nByte;
  SLIST(SCharNode)   m_fromBC[256];
  SLIST(SCharNode)   m_fromUC[256];
}SCharMap;

typedef struct SCharEncoding_s
{
  struct SCharEncoding_s * m_next;
  const SCharMap         * m_map;
  ECharFormat              m_format;
  unsigned long            m_MIBenum;
  char                     m_name[1];
}SCharEncoding;



/*  EXTERNAL DATA STRUCTURES  */

/*  FUNCTION PROTOTYPES  */


void            initChMap(void);

void            termChMap(void);

SCharEncoding * getCharEncoding
                        (const char *name);

WML_STATUS      addCharacterSet
                        (const char *name,
                         const UINT16 *map);
WML_STATUS      addAliasName
                        (const char *name,
                         const char *alias);

WCHAR           getChFromList
                        (SCharNode *n, WCHAR ch);

int             addISOTables(void);

/* mibenum.c */

void            initMIBenum(void);
void            termMIBenum(void);
unsigned long   getMIBenum
                            (const char *name);
#endif  /* CHMAP_H */

/*  End of File  */

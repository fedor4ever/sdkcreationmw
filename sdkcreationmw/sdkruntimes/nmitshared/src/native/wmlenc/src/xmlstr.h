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
/* File    : U:\wpsvob\wmlenc\src\include\xmlstr.h
   Version : \main\8
   Date    : 17-Oct-00.12:51:59
*/

/*
 * ========================================================================
 *  Name        : xmlstr.h
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

    This header file contains the XML string tool macros, declarations.

-----------------------------------------------------------------------------
*/

#ifndef XMLSTR_H
#define XMLSTR_H

#include "xmlpool.h"

/*  CONSTANTS  */

#define STR_INITIAL_SIZE    16

/*  MACROS  */

#define   STR_LEN(s)     (s->m_nChar)
#define   STR_DATA(s)    (s->m_data)
#define   STR_CLEAR(s)   ((s)->m_nChar=0)
#define   STR_SAME(s1,s2) (STR_LEN(s1)==STR_LEN(s2) ? strSame(s1,s2) : 0)

/*  DATA TYPES  */

typedef struct String_s
{
    UINT16	m_nChar;
    UINT16	m_nAllocatedChar;
    WCHAR * m_data;
    WCHAR   m_smallBuf[STR_INITIAL_SIZE];
}*PString;

/*  EXTERNAL DATA STRUCTURES  */

/*  FUNCTION PROTOTYPES  */


    /* Allocation / Deallocation */
PString   newString
                        (SPool *pool);

void	  deleteString
                        (PString s,
                         SPool *pool);

void      constrString
                        (PString s);
void      destrString
                        (PString s);

PString   strXDup
                        (PString s,
                         SPool *pool);

PString   newSubString  (const PString    src,
                         UINT16           fromPos,  /* included */
                         UINT16           toPos,    /* excluded */
                         SPool          * pool);

    /* String manipulations */
PString   strCpy
                        (PString dest,
                         const PString src);

PString   strCat
                        (PString dest,
                         const PString src);

PString   strCatAscii
                        (PString dest,
                         const char * src);

void      strAppendChar
                        (PString dest,
                         WCHAR ch);

    /* Comparisons */
int       strSame
                        (const PString v1,
                         const PString v2);

int       strSameAscii
                        (const PString v1,
                         const char *ascii);



    /* Formatting */
int       strStrip
                        (PString s);

void      strStripLeading
                        (PString s);

void      strStripTrailing
                        (PString s);

void      strOneSpace
                        (PString s);

void      strNormalize
                        (PString s);

    /* Logging      */
char *    wPrint
                        (UINT32     nWChar,
                         const WCHAR     *wBuf,
                         UINT32    nChar,
                         char      *buf);


#ifdef INVARIANTS
void      invPString(PString str);
#endif


#endif  /*  XMLSTR_H   */

/*  End of File  */

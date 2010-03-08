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
/* File    : U:\wpsvob\wmlenc\src\include\xmlctyp.h
   Version : \main\6
   Date    : 17-Oct-00.12:51:45
*/

/*
 * ========================================================================
 *  Name        : xmlctyp.h
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

    This header file contains the 'ctype' like macros.

-----------------------------------------------------------------------------
*/

#ifndef XMLCTYP_H
#define XMLCTYP_H

#include "xmlhost.h"

/*  CONSTANTS  */

#define CBIT_BASE           1
#define CBIT_IDEOGRAPHIC    2
#define CBIT_SPACE          4
#define CBIT_COMBINING      8
#define CBIT_DIGIT          16
#define CBIT_EXTENDER       32
#define CBIT_NAME           64
#define CBIT_PUBID          128
#define CBIT_XMLCHAR		256


/*  MACROS  */

#define IS_BASE(ch)     (g_xTypeTable[ch]&CBIT_BASE)
#define IS_SPACE(ch)        (g_xTypeTable[ch]&CBIT_SPACE)
#define IS_IDEOGRAPHIC(ch)  (g_xTypeTable[ch]&CBIT_IDEOGRAPHIC)
#define IS_COMBINING(ch)    (g_xTypeTable[ch]&CBIT_COMBINING)
#define IS_DIGIT(ch)        (g_xTypeTable[ch]&CBIT_DIGIT)
#define IS_EXTENDER(ch)     (g_xTypeTable[ch]&CBIT_EXTENDER)
#define IS_NAMECHAR(ch)     (g_xTypeTable[ch]&CBIT_NAME)
#define IS_PUBIDCHAR(ch)    (g_xTypeTable[ch]&CBIT_PUBID)
#define IS_LETTER(ch)       (g_xTypeTable[ch]&(CBIT_BASE|CBIT_IDEOGRAPHIC))
#define IS_XMLCHAR(ch)	   (g_xTypeTable[ch]!=0)

/*  DATA TYPES  */

/*  EXTERNAL DATA STRUCTURES  */

extern WCHAR g_xTypeTable[0x10000];


/*  FUNCTION PROTOTYPES  */

void    initXCType
                (void);

#endif  /* XMLCTYP_H */

/*  End of File  */

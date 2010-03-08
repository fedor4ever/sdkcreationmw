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
/* File    : U:\wpsvob\wmlscript\work\ported\pragpool.h
   Version : \main\4
   Date    : 01-Sep-99.10:35:43
*/

/*
 * ========================================================================
 *  Name        : pragpool.h
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

      Pragma pool definitions and interface function declarations.

-----------------------------------------------------------------------------
*/

#if !defined(PRAGPOOL_H)
#define PRAGPOOL_H

#include "bytecode.h"


#define MEMORY_ERROR -99


typedef enum
{
    PRAGMA_ACCESS_DOMAIN,
    PRAGMA_ACCESS_PATH,
    PRAGMA_USER_AGENT_PROPERTY,
    PRAGMA_USER_AGENT_PROPERTY_AND_SCHEME,
} PragmaType;

struct Pragma
{
    struct Pragma*  pNext;

    PragmaType      type;
    INT16           iIndex1;
    INT16           iIndex2;
    INT16           iIndex3;

};



/* global protos */
void    PragmaTableCleanUp(struct Pragma* pPragmas);
INT16   PragmaTableAdd(struct Pragma** pPragmas, PragmaType type, INT16 iIndex1, INT16 iIndex2, INT16 iIndex3);
INT16   PragmaTableGenerateBytecode(struct Pragma* pPragmas, struct StorageStruct* pStorage);

#endif

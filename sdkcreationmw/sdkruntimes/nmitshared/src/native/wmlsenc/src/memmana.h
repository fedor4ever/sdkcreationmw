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
/* File    : U:\wpsvob\wmlscript\work\ported\memmana.h
   Version : \main\6
   Date    : 13-Sep-99.12:29:16
*/

/*
 * ========================================================================
 *  Name        : memmana.h
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

      Memory Management definitions and interface function declarations.

-----------------------------------------------------------------------------
*/

#ifndef MEMMANA_H
#define MEMMANA_H


#include "common.h"

#define DEFAULT_MEMORY_NODE_SIZE  2000



struct MemoryNode
{
    BYTE*               pbBuffer;   /* pointer to allocated buffer */
    struct MemoryNode*  pNext;      /* pointer to next node or NULL */

	INT16               iUsed;      /* how many bytes alredy used */
    INT16               iTotalSize; /* what is the total size of this buffer */
};






/* global prototypes */
struct MemoryNode*  MemoryAllocNewNode(INT16 iSize);
BYTE*               MemoryAlloc(struct MemoryNode* pNodes, INT16 iSize);
void                MemoryDealloc(struct MemoryNode* pNodes);
void                MemoryDelete(struct MemoryNode* pNodes);





#endif

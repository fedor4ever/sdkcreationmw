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
/* File    : U:\wpsvob\wmlenc\src\include\xmlpool.h
   Version : \main\7
   Date    : 17-Oct-00.12:51:54
*/

/*
 * ========================================================================
 *  Name        : xmlpool.h
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

    This header file contains the pool utility for the XML parser.

-----------------------------------------------------------------------------
*/

#ifndef XMLPOOL_H
#define XMLPOOL_H

#include "xmlhost.h"

/*  CONSTANTS  */

/*  MACROS  */

#if 1
#define POOL_ALLOC(pool,ptr) { if((pool)->m_head==0)\
                                   PoolGrow((pool));\
                               ptr = (void*)((pool)->m_head);\
                               (pool)->m_head = (pool)->m_head->m_next;\
                             }

#define POOL_FREE(pool,ptr) {((SPoolLink*)((void*)ptr))->m_next=(pool)->m_head;\
                             (pool)->m_head = (SPoolLink*)((void*)ptr);\
                            }

#else
#define POOL_ALLOC(pool,ptr) { ptr = xmlMalloc(64); }

#define POOL_FREE(pool,ptr) { xmlFree(ptr); }
#endif

/*  DATA TYPES  */

typedef struct SPoolLink_s
{
  struct SPoolLink_s  * m_next;
}SPoolLink;

typedef struct SBlockNode_s
{
  struct SBlockNode_s * m_next;
  UINT8 *               m_buf;
}SBlockNode;

typedef struct
{
  SPoolLink  *  m_head;
  SBlockNode *  m_bnode;
  UINT16        m_esize;
}SPool;


/*  FUNCTION PROTOTYPES  */

void  constrPool
            (SPool *pool, UINT16 elem_size);

void  destrPool
            (SPool *pool);

void  PoolGrow
            (SPool *pool);


#endif  /* XMLPOOL_H */

/*  End of File  */

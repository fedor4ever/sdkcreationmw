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
/* File    : U:\wpsvob\wmlenc\src\common\xmlpool.c
   Version : \main\7
   Date    : 17-Oct-00.12:49:24
*/

/*
 * ========================================================================
 *  Name        : xmlpool.c
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

    1  ABSTRACT
        
    1.1 Module Type

    type  : subroutines

    1.2 Functional Description

        Memory management for small objects.

        The 'pool' utility provides fast allocation/deallocation for
        relatively small objects with same sizes.
        The 'pool' tool was designed according to the description in
        B.Stroustrup's C++ book ( Chapter13 ).

    1.3 Specification/Design Reference

        doc/wmlarch.doc

    1.4 Module Test Specification Reference

        none

    1.5 Compilation Information

        See: building.txt

    1.6 Notes

        - the pool never hands memory back to the system until itself is
          is destroyed (calling the destrPool()).

*/



/*  2  CONTENTS

        1  ABSTRACT

        2  CONTENTS

        3  GLOSSARY

        4  EXTERNAL RESOURCES
        4.1  Include Files
        4.2  External Data Structures
        4.3  External Function Prototypes

        5  LOCAL CONSTANTS AND MACROS

        6  MODULE DATA STRUCTURES
        6.1  Local Data Structures
        6.2  Local Function Prototypes

        7  MODULE CODE

        7.1   constrPool()
        7.2   destrPool()
        7.3   PoolGrow()

*/



/*  3  GLOSSARY

        None

*/


/*  4  EXTERNAL RESOURCES  */

    /* None */

/*  4.1  Include Files  */

#include "xmlpool.h"

/*  4.2 External Data Structures  */

    /* None */


/*  4.3 External Function Prototypes  */

    /* None */


/*  5  LOCAL CONSTANTS AND MACROS  */

#define S_OVERHEAD   24              /* possible less */
#define S_CHUNKSIZE  (4096-S_OVERHEAD)



/*  6  MODULE DATA STRUCTURES  */


/*  6.1 Local Data Structures  */

    /* None */


/*  6.2 Local Function Prototypes  */

    /* None */


/*  7 MODULE CODE  */

/*
 *===========================================================================
 */
/*  7.1  */
void constrPool(SPool *pool,        /* In: SPool to initialize  */
                UINT16 elem_size)  /* In: elem size            */
{
/*  Functional Description
 *
 *  Initializes 'pool' for objects with size 'elem_size'.
 *
 *  Return Value:  -
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/*  Code  */
  pool->m_esize = elem_size<sizeof(SPool *) ?
                        sizeof(SPoolLink*) : 
                        (elem_size&1 ? ++elem_size : elem_size);

  pool->m_head  = 0;
  pool->m_bnode = 0;
}

/*
 *===========================================================================
 */
/*  7.2  */
void destrPool(SPool *pool)     /* In: SPool to terminate   */
{
/*  Functional Description
 *
 *  Hands memory blocks back to general-purpose allocator.
 *
 *  Return Value:  -
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/*  Code  */

  while(pool->m_bnode)
   {
      SBlockNode *tmp = pool->m_bnode->m_next;
      XML_FREE(pool->m_bnode->m_buf);
      XML_FREE(pool->m_bnode);
      pool->m_bnode = tmp;
   }
}

/*
 *===========================================================================
 */
/*  7.3  */
void PoolGrow(SPool *pool)      /* In: SPool to grow   */
{
/*  
 *  Allocates and initializes a new memory block.
 *
 *  Return Value:  -
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  UINT8 *_last;
  UINT8 *_p;
  unsigned _nelem     = (S_CHUNKSIZE/pool->m_esize);
  SBlockNode *_b = (SBlockNode*)XML_ALLOC(sizeof(SBlockNode));

/*  Code  */

  _b->m_next    = pool->m_bnode;
  _b->m_buf     = (UINT8*)XML_ALLOC(S_CHUNKSIZE);
  pool->m_bnode = _b;
  _last         = _b->m_buf + ((_nelem-1)*pool->m_esize);

  for(_p=_b->m_buf; _p<_last; _p+=pool->m_esize) {
      ((SPoolLink*)((void*)_p))->m_next =
          (SPoolLink*)((void*)(_p+pool->m_esize));
  }

  ((SPoolLink*)((void*)_last))->m_next = 0;
  pool->m_head = (SPoolLink*)((void*)(_b->m_buf));
}

/*
=============================================================================
*/


/*  End of File  */

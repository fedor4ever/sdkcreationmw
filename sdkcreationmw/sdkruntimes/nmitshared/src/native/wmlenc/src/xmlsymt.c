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
/* File    : U:\wpsvob\wmlenc\src\common\xmlsymt.c
   Version : \main\7
   Date    : 17-Oct-00.12:50:07
*/

/*
 * ========================================================================
 *  Name        : xmlsymt.c
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

        This file contains the implementation of symbol tables.

    1.3 Specification/Design Reference

        doc/wmlarch.doc

    1.4 Module Test Specification Reference

        none

    1.5 Compilation Information

        See: building.txt

    1.6 Notes

        None

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

        7.1   constrSymTbl
        7.2   destrSymTbl
        7.3   symTblNewItem
        7.4   symTblLookUp
        7.5   invSymbolTable
        7.6   dbgSymbolTable

*/



/*  3  GLOSSARY

        None

*/


/*  4  EXTERNAL RESOURCES  */

    /* None */

/*  4.1  Include Files  */

#include "xmlsymt.h"
#include "xmllex.h"
#include "xmlpool.h"

/*  4.2 External Data Structures  */

    /* None */


/*  4.3 External Function Prototypes  */

    /* None */


/*  5  LOCAL CONSTANTS AND MACROS  */

#define STR_HASH(s) (STR_LEN(key) ?\
                     (*STR_DATA(key)^(*(STR_DATA(key)+(STR_LEN(key)>>1)))) : 0)\
                    & (TBLSZ-1);



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
void  constrSymTbl(SymbolTable *that,       /* In: an instance          */
                   SPool       *strPool,    /* In: pool for xml strings */
                   void        *param,      /* In: pass to constr/destr */
                   FConstructor  c,          /* In: func constructor     */
                   FDestructor   d,          /* In: func destructor      */
                   FInvariant    i,          /* In: func invariant       */
                   FDebugFunc    dbg,        /* In: func for debug       */
                   UINT16          elem_size)	/* In: must be small        */
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  int j;

/*  Code  */

  that->m_strPool = strPool;
  that->m_param   = param;

  for(j=0; j<TBLSZ; j++) {
      that->m_listArray[j].m_root= 0;
  }
  that->m_constr     = c;
  that->m_destr      = d;
  that->m_inv        = i;
  that->m_debug      = dbg;

  constrPool(&that->m_pool,(UINT16)(elem_size+sizeof(PString)+sizeof(StrMapNode*)));
}

/*
 *===========================================================================
 */
/*  7.2  */
void  destrSymTbl(SymbolTable *that)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  int i;

/*  Code  */

  for(i=0; i<TBLSZ; i++) {
      StrMapNode *_node;

      while(SLL_GET(that->m_listArray[i],_node)) { 
          deleteString(_node->m_key, that->m_strPool);
          if(that->m_destr)
              that->m_destr(&_node->m_value, that->m_param);
          POOL_FREE(&that->m_pool,_node);
      }
  }
  destrPool(&that->m_pool);
}

/*
 *===========================================================================
 */
/*  7.3  */
void * symTblNewItem(SymbolTable *that, PString key)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  StrMapNode *_node;
  int i;

/*  Code  */

  assert(key);
  
  i = STR_HASH(key);

  POOL_ALLOC(&that->m_pool,_node);
  SLL_INSERT(that->m_listArray[i],_node);
  _node->m_key  = strXDup(key, that->m_strPool);
  if(that->m_constr)
      that->m_constr(&_node->m_value, that->m_param);
  return &_node->m_value;
}

/*
 *===========================================================================
 */
/*  7.4  */
void * symTblLookUp(SymbolTable *that, PString key)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  int i;

/*  Code  */

  StrMapNode *_node;

  if(!key)
      return 0;

  i = STR_HASH(key);

  _node = that->m_listArray[i].m_root;

  while(_node && !STR_SAME(_node->m_key,key))
      _node = _node->m_next;

  return _node ? &_node->m_value : 0;
}

#ifdef INVARIANTS
/*
 *===========================================================================
 */
/*  7.5  */
void  invSymbolTable(SymbolTable *that)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  int i;
  StrMapNode *_node;

/*  Code  */

  if(that->m_inv) {
      for(i=0; i<TBLSZ; i++) {
          for(_node=that->m_listArray[i].m_root; _node ; _node=_node->m_next)
              that->m_inv(&_node->m_value);
      }
  }
}
#endif  /* INVARIANTS */

#ifdef WML_DEBUG
/*
 *===========================================================================
 */
/*  7.6  */
void  dbgSymbolTable(SymbolTable *that)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  int i;
  StrMapNode *_node;

/*  Code  */

  if(that->m_debug) {
      for(i=0; i<TBLSZ; i++) {
          for(_node=that->m_listArray[i].m_root; _node ; _node=_node->m_next) {
              DBG_LOG2( (DBG_SYM, _node->m_key,"\nKey:") );
              that->m_debug(&_node->m_value);
          }
      }
  }
}
#endif  /* WML_DEBUG */

/*
=============================================================================
*/


/*  End of File  */

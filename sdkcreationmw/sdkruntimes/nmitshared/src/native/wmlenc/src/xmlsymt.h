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
/* File    : U:\wpsvob\wmlenc\src\include\xmlsymt.h
   Version : \main\6
   Date    : 17-Oct-00.12:52:03
*/

/*
 * ========================================================================
 *  Name        : xmlsymt.h
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

    This header file contains the symbol table related stuff
    used by the XML parser.

-----------------------------------------------------------------------------
*/

#ifndef XMLSYMT_H
#define XMLSYMT_H

#include "xmlpool.h"
#include "xmlstr.h"

/*  CONSTANTS  */

#define TBLSZ   32      /* power of 2 */

/*  MACROS  */

#define name2(a,b)  _namE2(a,b)     /* same as in 'generic.h'   */
#define _namE2(a,b) a##b

/*
 *  Macros for intrusive single linked list
 *  the lack of inline functions painful
 */

#define SLIST(type)          struct name2(type,List)
#define DECLARE_SLIST(type)  SLIST(type) { type   *m_root; }

#define SLL_INSERT(list,ptr) { (ptr)->m_next=list.m_root;\
                                list.m_root=(ptr);}
#define SLL_GET(list,retv) (list.m_root ?\
                      (retv=list.m_root,list.m_root=list.m_root->m_next, retv)\
                    : (retv=0))

#define SLL_EMPTY(list) (list.m_root==0)

/*  DATA TYPES  */

typedef void (*FConstructor) (void *item, void *commonData);
typedef void (*FDestructor)  (void *item, void *commonData);
typedef void (*FInvariant)   (void *);
typedef void (*FDebugFunc)   (void *);

typedef struct StrMapNode_s
{
  struct StrMapNode_s *m_next;
  PString            m_key;
  int                m_value;
}StrMapNode;

DECLARE_SLIST(StrMapNode);

typedef struct
{
  SLIST(StrMapNode)  m_listArray[TBLSZ];
  SPool              m_pool;
  SPool           *  m_strPool;
  void            *  m_param;
  FConstructor       m_constr;   /* for items */
  FDestructor        m_destr;    /* for items */
  FInvariant         m_inv;      /* for items */
  FDebugFunc         m_debug;    /* for items */
}SymbolTable;

/*  EXTERNAL DATA STRUCTURES  */

/*  FUNCTION PROTOTYPES  */

void  constrSymTbl
                 (SymbolTable * that,
                  SPool       * strPool,
                  void        * param,
                  FConstructor  c,
                  FDestructor   d,
                  FInvariant    i,
                  FDebugFunc    dbg,
                  UINT16        elem_size);

void  destrSymTbl
                (SymbolTable *that);

void *symTblNewItem
                (SymbolTable *  that,
                 PString        key);
                
void *symTblLookUp
                (SymbolTable *  that,
                 PString        key);

#ifdef INVARIANTS
void  invSymbolTable(SymbolTable *that);
#endif

#ifdef WML_DEBUG
void  dbgSymbolTable(SymbolTable *that);
#endif

#endif  /* XMLSYMT_H */

/*  End of File  */

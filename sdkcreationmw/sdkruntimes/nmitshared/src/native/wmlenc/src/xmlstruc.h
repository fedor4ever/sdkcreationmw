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
/* File    : U:\wpsvob\wmlenc\src\include\xmlstruc.h
   Version : \main\7
   Date    : 17-Oct-00.12:52:00
*/

/*
 * ========================================================================
 *  Name        : xmlstruc.h
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

    This header file contains the declarations for data structures
    used by the XML parser. 

-----------------------------------------------------------------------------
*/

#ifndef XMLAUX_H
#define XMLAUX_H

#include "xmlpool.h"
#include "xmlapi.h"
#include "xmlsymt.h"

/*  CONSTANTS  */

#define STRING_STACK_SIZE       128
#define N_USED_ATTR             32 

/*  MACROS  */

#define STR_POOL(d)        ( &(d->m_commonData.m_strPool) )
#define ATTR_POOL(d)       ( &(d->m_commonData.m_attrPool) )
#define SNODE_POOL(d)      ( &(d->m_commonData.m_snodePool) )

#define STRPOOL(cd)        ( &(cd->m_strPool) )
#define ATTRPOOL(cd)       ( &(cd->m_attrPool) )
#define SNODEPOOL(cd)      ( &(cd->m_snodePool) )

/*  DATA TYPES  */

typedef enum {
    ET_INTERNAL,
    ET_EXTERNAL_PARSED,
    ET_EXTERNAL_UNPARSED
}EntityType;

typedef enum {
    AT_STRING,
    AT_TOKENIZED,
    AT_NOTATION,
    AT_ENUM
}EAttrType;

typedef enum {
    AD_REQUIRED,
    AD_IMPLIED,
    AD_DEFAULT,
    AD_FIXED
}EAttrDefault;

typedef enum
{
  RC_CDATA,            /* added for Bypassed/Not recognized and real CDATA */
  RC_IN_CONTENT,
  RC_IN_ATTR_VALUE,
  RC_IN_ENTITY_VALUE,
  RC_IN_CONTENT_MODEL
}ERefContext;

typedef struct
{
  SExtId  m_extId;
}SNotation;

typedef union
{
  PString m_value;
  SExtId  m_extId;
}UValue;

typedef struct
{
  EntityType     m_type;
  UValue  u;        /* unnamed union more elegant but MS specific */
  PString m_notation;
}SGEntity;

typedef struct
{
  EntityType     m_type;
  UValue  u;
}SPEntity;


typedef struct StrNode_s
{
  struct StrNode_s  *m_next;
  PString            m_str;
}StrNode;


DECLARE_SLIST(StrNode);

typedef struct Attribute_s
{
  struct Attribute_s *  m_next;
  PString               m_name;
  EAttrType             m_type;
  int                   m_token;
  SLIST(StrNode)       m_typeList;
  EAttrDefault          m_default;
  PString               m_defValue;
}SAttribute;

DECLARE_SLIST(SAttribute);


typedef struct
{
  int                   m_notDeclared;      /* XML rule 52, pg.18 */
  PString               m_contentModel;
  SLIST(SAttribute)     m_attrList;
}SElement;

typedef struct
{
  const WCHAR  * m_gptr;
  const WCHAR  * m_eptr;
  int      m_stackPos;
}SEntityBuffer;


typedef struct SIdNode_s
{
  struct SIdNode_s * m_next;
  UINT16             m_id;
  WCHAR            * m_buffer;
}SIdNode;

typedef struct SCP
{
  const WCHAR *m_cur;
  const WCHAR *m_beg;
  const WCHAR *m_end;
}SCP;

typedef struct SContentParser_s
{
  SCP                        m_scp;
  struct SContentParser_s *  m_parent;
  struct SContentParser_s ** m_current;
}SContentParser;

typedef struct
{
  SymbolTable       m_GEntities;
  SymbolTable       m_Notations;
  SymbolTable       m_PEntities;
  SymbolTable       m_Elements;
  SymbolTable       m_IDs;

  SPool             m_strPool;
  SPool             m_attrPool;
  SPool             m_snodePool;
}SCommonData;
typedef SCommonData * PCommon;


DECLARE_SLIST(SIdNode);

struct SParserData
{
  SEntityBuffer     m_eb;           /* must be the first ! */
  
  SEntityBuffer   * m_bufStack;
  int               m_bufStackSize;

  PString           m_valStack[STRING_STACK_SIZE];
  PString         * m_valSP;
  PString           m_tmpStack[STRING_STACK_SIZE];
  PString         * m_tmpSP;
  int               m_popFrom;
  const WCHAR     * m_popPtr;

  int               m_ruleStack[STRING_STACK_SIZE];
  int               m_ruleSP;


  FXMLCallBack      m_tokenCB;
  FXMLErrorCB       m_errorCB;
  void           *  m_usrData;

  PString           m_chardata;
  SAttribute     *  m_usedAttribute[N_USED_ATTR];
  int               m_nUsedAttr;

  PString           m_nameOfRootElement;
  int               m_elementNestingLevel;
  SContentParser  * m_cParserOfRoot;
  int               m_deepnessOfPERepl;
  int               m_standalone;
  
  SAttribute      * m_attribute;
  SElement        * m_element;
  SExtId            m_extId;
  SExtId            m_dtdId;
  PString           m_pubIdAltDTD;
  PString           m_notation;
  int               m_idOnlyFlag;
  SContentParser  * m_contentParser;
  int               m_unParsedFlag;
  int               m_literalFlag;
  SLIST(SIdNode)    m_bufferIdList;
  const WCHAR     * m_rootBuffer;
  const WCHAR     * m_rootBufferEnd;

  const BYTE      * m_location;

  SCommonData       m_commonData;
  int               m_error;

#ifdef WML_DEBUG

  char            * m_appStack;
  int               m_st;
  char            * m_spaces;
  char            * m_dbgbuf;

#endif  /* WML_DEBUG */
};

/*  EXTERNAL DATA STRUCTURES  */

/*  FUNCTION PROTOTYPES  */

/*
 * Attributes have their own pool, we can allocate/deallocate them
 * with newAttr/deleteAttr only.
 */
SAttribute *newAttr
                    (PCommon cd);

void        deleteAttr
                    (SAttribute *attr,
                     PCommon cd);

/*
 * Some structure is being created/deleted when the exact type have not
 * known yet (SymbolTable), so we need to call constructor/destructor.
 */


void constrGEntity
                (void *     pGE,
                 PCommon    cd);
                
void destrGEntity
                (void *     pGE,
                 PCommon    cd);
                
void constrNotation
                (void *     pGE,
                 PCommon    cd);

void destrNotation
                (void *     pGE,
                 PCommon    cd);

void constrPEntity
                (void *     pPE,
                 PCommon    cd);

void destrPEntity
                (void *     pPE,
                 PCommon    cd);

void constrElement
                (void *     pElement,
                 PCommon    cd);

void destrElement
                (void *     pElement,
                 PCommon    cd);

void constrCommonData
                (PCommon cd);

void destrCommonData
                (PCommon cd);

void constrContentParser
                (PARDATA         d,
                 SContentParser *pContentParser,
                 SElement       *pElement);

void destrContentParser
                (PARDATA d,
                 SContentParser *pContentParser);

int checkCP(const PString elem,
            SCP *scp);
int leaveCP(SCP *scp);

#ifdef INVARIANTS

void invSGEntity    (SGEntity *);
void invSNotation   (SNotation *);
void invSPEntity    (SPEntity *);
void invSAttribute  (SAttribute *);
void invSElement    (SElement *);
void invSEntityBuffer(SEntityBuffer *);
void invSParserData (PARDATA p);
void invSExtId      (SExtId *);
void invSContentParser (SContentParser *);

#endif  /* INVARIANTS */

#ifdef WML_DEBUG

void dbgSGEntity    (SGEntity *);
void dbgSNotation   (SNotation *);
void dbgSPEntity    (SPEntity *);
void dbgSAttribute  (SAttribute *);
void dbgSElement    (SElement *);
void dbgSEntityBuffer(SEntityBuffer *);
void dbgSParserData (PARDATA );
void dbgSExtId      (SExtId *);

#endif  /* WML_DEBUG */

#endif  /* XMLAUX_h */

/*  End of File  */

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
/* File    : U:\wpsvob\wmlenc\src\common\xmlstruc.c
   Version : \main\7
   Date    : 17-Oct-00.12:49:57
*/

/*
 * ========================================================================
 *  Name        : xmlstruc.c
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

        This file contains functions to manipulate data structures
        belong to XML Parser modul.


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

        7.1    newAttr()
        7.2    deleteAttr()
        7.3    constrGEntity()
        7.4    destrGEntity()
        7.5    constrNotation()
        7.6    destrNotation()
        7.7    constrPEntity()
        7.8    destrPEntity()
        7.9    constrElement()
        7.10   destrElement()
        7.11   constrContentParser()
        7.12   destrContentParser()
        7.13   invSGEntity()
        7.14   invSNotation()
        7.15   invSPEntity()
        7.16   invSElement()
        7.17   invSAttribute()
        7.18   invSExtId()
        7.19   invSContentParser()
        7.20   addAttr()
        7.21   lookUpAttr()
        7.22   addTypeValue()
        7.23   typeValueExists()
        7.24   checkContent()
        7.25   checkChild()
        7.26   dbgSGEntity()
        7.27   dbgSNotation()
        7.28   dbgSPEntity()
        7.29   dbgSElement()
        7.30   dbgSAttribute()
        7.31   dbgSExtId(SExtId *p)

*/



/*  3  GLOSSARY

        None

*/


/*  4  EXTERNAL RESOURCES  */

    /* None */

/*  4.1  Include Files  */

#include "xmlpars.h"
#include "xmllex.h"
#include "xmlmsg.h"

/*  4.2 External Data Structures  */

    /* None */


/*  4.3 External Function Prototypes  */

    /* None */


/*  5  LOCAL CONSTANTS AND MACROS  */

    /* None */



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
SAttribute * newAttr(PCommon pc)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  SAttribute *_retv;

/* Code */

  POOL_ALLOC(ATTRPOOL(pc),_retv);
  memset(_retv,0,sizeof(SAttribute));
  _retv->m_name     = newString(STRPOOL(pc));
  _retv->m_defValue = newString(STRPOOL(pc));
  return _retv;
}


/*
 *===========================================================================
 */
/*  7.2  */
void deleteAttr(SAttribute *attr, PCommon d)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/* Code */

  if(attr) {
      StrNode *_node;

      while(SLL_GET(attr->m_typeList,_node)) {
          deleteString(_node->m_str, STRPOOL(d));
          POOL_FREE(SNODEPOOL(d),_node);
      }

      deleteString(attr->m_name,      STRPOOL(d));
      deleteString(attr->m_defValue,  STRPOOL(d));

      POOL_FREE(ATTRPOOL(d),attr);
  }
}


/*
 *===========================================================================
 */
/*  7.3  */
void constrGEntity(void *pGE, PCommon pc)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/* Code */

  memset(pGE,0,sizeof(SGEntity));
}


/*
 *===========================================================================
 */
/*  7.4  */
void destrGEntity(void *pGE, PCommon pc)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/* Code */

  deleteString(((SGEntity*)pGE)->u.m_extId.m_pubId, STRPOOL(pc));
  deleteString(((SGEntity*)pGE)->u.m_extId.m_sysId, STRPOOL(pc));
  deleteString(((SGEntity*)pGE)->m_notation, STRPOOL(pc));
}


/*
 *===========================================================================
 */
/*  7.5  */
void constrNotation(void *pN, PCommon pc)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/* Code */

  memset(pN,0,sizeof(SNotation));
}


/*
 *===========================================================================
 */
/*  7.6  */
void destrNotation(void *pN, PCommon pc)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/* Code */

  deleteString(((SNotation*)pN)->m_extId.m_pubId, STRPOOL(pc));
  deleteString(((SNotation*)pN)->m_extId.m_sysId, STRPOOL(pc));
}


/*
 *===========================================================================
 */
/*  7.7  */
void constrPEntity(void *pPE, PCommon pc)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/* Code */

  memset(pPE,0,sizeof(SPEntity));
}


/*
 *===========================================================================
 */
/*  7.8  */
void destrPEntity(void *pPE, PCommon pc)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/* Code */

  deleteString(((SPEntity*)pPE)->u.m_extId.m_pubId, STRPOOL(pc));
  deleteString(((SPEntity*)pPE)->u.m_extId.m_sysId, STRPOOL(pc));
}


/*
 *===========================================================================
 */
/*  7.9  */
void constrElement  (void *pElement, PCommon pc)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/* Code */

  ((SElement*)pElement)->m_notDeclared     = 0;
  ((SElement*)pElement)->m_contentModel    = newString(STRPOOL(pc));
  ((SElement*)pElement)->m_attrList.m_root = 0;
}


/*
 *===========================================================================
 */
/*  7.10  */
void destrElement   (void *pElement, PCommon pc)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  SAttribute *_attr;

/* Code */

  while( SLL_GET(((SElement*)pElement)->m_attrList,_attr) )
      deleteAttr(_attr, pc);
  deleteString(((SElement*)pElement)->m_contentModel, STRPOOL(pc));
}


/*
 *===========================================================================
 */
/*  7.11  */
void constrContentParser(PARDATA d,
                          SContentParser *parser,
                          SElement       *pElement)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */
  SCP *_scp = &(parser->m_scp);

/* Code */

  _scp->m_cur = 0;
  _scp->m_beg = 0;
  _scp->m_end = 0;
  parser->m_parent      = d->m_contentParser;
  parser->m_current     = &(d->m_contentParser);

  /* vcCharData() wants to know if in root */
  if(d->m_contentParser && (!d->m_contentParser->m_parent))
      d->m_cParserOfRoot = parser;

  d->m_contentParser    = parser;

  if(pElement && pElement->m_contentModel) {
      _scp->m_beg = STR_DATA(pElement->m_contentModel);
      _scp->m_cur = _scp->m_beg;
      _scp->m_end = _scp->m_beg+STR_LEN(pElement->m_contentModel);
  }
}


/*
 *===========================================================================
 */
/*  7.12  */
void destrContentParser(PARDATA d,
                         SContentParser *p)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/* Code */

  if(p->m_parent)
      *p->m_current = p->m_parent;
}


#ifdef INVARIANTS

/*
 *===========================================================================
 */
/*  7.13  */
void invSGEntity(SGEntity *p)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */


/* Code */

  if( p->m_type == ET_INTERNAL ) {
      invPString(p->u.m_value);
      assert(p->u.m_extId.m_sysId ==0);
  }
  else if( p->m_type == ET_EXTERNAL_PARSED ) {
      invSExtId(&p->u.m_extId);
  }
  else if( p->m_type == ET_EXTERNAL_UNPARSED ) {
      invSExtId(&p->u.m_extId);
  }
  else {
      assert(0);
  }
}


/*
 *===========================================================================
 */
/*  7.14  */
void invSNotation(SNotation *p)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/* Code */

  invSExtId(&p->m_extId);
}


/*
 *===========================================================================
 */
/*  7.15  */
void invSPEntity(SPEntity *pPE)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/* Code */

  switch(pPE->m_type) {
     case ET_INTERNAL :
                 invPString(pPE->u.m_value);
                 break;
     case ET_EXTERNAL_PARSED :
                 invPString(pPE->u.m_extId.m_pubId);
                 invPString(pPE->u.m_extId.m_sysId);
                 break;
     default :   DBG_ASSERT(0);
                 break;
  }
}


/*
 *===========================================================================
 */
/*  7.16  */
void invSElement(SElement *pElement)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  SAttribute *_node;

/* Code */

  assert( (pElement->m_notDeclared==0)
        ||(pElement->m_notDeclared==1));
  invPString(pElement->m_contentModel);
  _node = pElement->m_attrList.m_root;
  while(_node) {
      invSAttribute(_node);
      _node = _node->m_next;
  }
}


/*
 *===========================================================================
 */
/*  7.17  */
void invSAttribute(SAttribute *attr)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  StrNode *_node = attr->m_typeList.m_root;

/* Code */

  while(_node) {
      invPString(_node->m_str);
      _node=_node->m_next;
  }
  invPString(attr->m_defValue);
}


/*
 *===========================================================================
 */
/*  7.18  */
void invSExtId(SExtId *p)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/* Code */

  invPString(p->m_pubId);
  invPString(p->m_sysId);
}


/*
 *===========================================================================
 */
/*  7.19  */
void invSContentParser(SContentParser *parser)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/* Code */

  assert(parser);
}


#endif  /* INVARIANTS */

/*
 *===========================================================================
 */
/*  7.20  */
void  addAttr(SElement *element,
              SAttribute *attr)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/* Code */

  SLL_INSERT(element->m_attrList,attr);
}


/*
 *===========================================================================
 */
/*  7.21  */
SAttribute *lookUpAttr(SElement *element,
                       PString name)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  SAttribute *_retv;

/* Code */

  for(_retv=element->m_attrList.m_root;
      _retv && !STR_SAME(_retv->m_name,name);
      _retv=_retv->m_next)
      ;
  return _retv;
}


/*
 *===========================================================================
 */
/*  7.22  */
void addTypeValue(SAttribute *attr,
                  PString     val,
                  PCommon     pc)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  StrNode *_node;
  
/* Code */

  POOL_ALLOC(SNODEPOOL(pc),_node);
  _node->m_str = strXDup(val,STRPOOL(pc));
  SLL_INSERT(attr->m_typeList,_node);
}


/*
 *===========================================================================
 */
/*  7.23  */
int  typeValueExists(SAttribute *attr,
                     PString val)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  StrNode *_retv;

/* Code */

  for(_retv=attr->m_typeList.m_root;
      _retv && !STR_SAME(_retv->m_str,val);
      _retv=_retv->m_next)
      ;
  return _retv!=0;
}


/*
 *===========================================================================
 */
/*  7.24  */
void checkContent(PARDATA         d,
                  SContentParser *p,
                  PString         where)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/* Code */
  DBG_ASSERT(p);

  if(!leaveCP(&(p->m_scp)))
      callError(d,XML_ERROR_NO_CHILD,where);

}


/*
 *===========================================================================
 */
/*  7.25  */
int checkChild(SContentParser *parser,
               PString child)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */
/* Code */
#ifdef INVARIANTS
  DBG_ASSERT(parser);
  invSContentParser(parser);
#endif

  return checkCP(child,&(parser->m_scp));
}



#ifdef WML_DEBUG

/*
 *===========================================================================
 */
/*  7.26  */
void dbgSGEntity(SGEntity *p)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/* Code */

  DBG_LOG( (DBG_SYM,"- General Entity - : ") );
  if(p->m_type==ET_INTERNAL) {
      DBG_LOG2( (DBG_SYM,p->u.m_value,"Internal:") );
  }
  else if(p->m_type==ET_EXTERNAL_PARSED) {
      DBG_LOG( (DBG_SYM,"ExternalParsed") );
      dbgSExtId(&p->u.m_extId);
  }
  else if(p->m_type==ET_EXTERNAL_UNPARSED) {
      DBG_LOG( (DBG_SYM,"External unparsed") );
      dbgSExtId(&p->u.m_extId);
      if(p->m_notation) {
          DBG_LOG2( (DBG_SYM,p->m_notation,"Notation:") );
      }
      else {
          DBG_LOG( (DBG_SYM,"No declared notation!") );
      }
  }
  else {
      DBG_LOG( (DBG_SYM,"???????? ") );
  }
}


/*
 *===========================================================================
 */
/*  7.27  */
void dbgSNotation(SNotation *p)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/* Code */

  DBG_LOG( (DBG_SYM,"- Notation -") );
  dbgSExtId(&p->m_extId);
}



/*
 *===========================================================================
 */
/*  7.28  */
void dbgSPEntity(SPEntity *pPE)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/* Code */

#ifndef WML_DEBUG
  assert(0);
#endif
  switch(pPE->m_type) {
     case ET_INTERNAL :
                 DBG_LOG2((DBG_SYM,pPE->u.m_value,"Internal:"));
                 break;
     case ET_EXTERNAL_PARSED :
                 DBG_LOG2((DBG_SYM,pPE->u.m_extId.m_pubId,"pubID:"));
                 DBG_LOG2((DBG_SYM,pPE->u.m_extId.m_sysId,"sysID:"));
                 break;
     default :   DBG_ASSERT(0);
                 break;
  }
}



/*
 *===========================================================================
 */
/*  7.29  */
void dbgSElement(SElement *pElement)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  SAttribute * _node = pElement->m_attrList.m_root;

/* Code */

  DBG_LOG( (DBG_SYM,"-  ELEMENT -") );
  DBG_LOG2( (DBG_SYM, pElement->m_contentModel,"Content model : ") );
  while(_node) {
      dbgSAttribute(_node);
      _node = _node->m_next;
  }
}


/*
 *===========================================================================
 */
/*  7.30  */
void dbgSAttribute(SAttribute *attr)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  const char *_cp;

/* Code */

  DBG_LOG2( (DBG_SYM,  attr->m_name,"Attribute: ") );
  switch(attr->m_type)
   {
     case AT_STRING    : _cp = "String";       break;
     case AT_TOKENIZED : _cp = "Tokenized";    break;
     case AT_NOTATION  : _cp = "Notation";     break;
     case AT_ENUM      : _cp = "Enumeration";  break;
     default           : _cp = "???????";      break;
   }
  DBG_LOG( (DBG_SYM,"\ttype: %s",_cp) );
  if(attr->m_token)
      DBG_LOG( (DBG_SYM,"\ttoken: %s", g_LexTable[attr->m_token]) );
  if(attr->m_typeList.m_root) {
      StrNode *_node = attr->m_typeList.m_root;
      while(_node) {
          DBG_LOG2( (DBG_SYM, _node->m_str,"\titem : "));
          _node=_node->m_next;
      }
  }
  switch(attr->m_default)
   {
     case AD_REQUIRED : _cp = "Required";  break;
     case AD_IMPLIED  : _cp = "Implied";   break;
     case AD_DEFAULT  : _cp = "Default";   break;
     case AD_FIXED    : _cp = "Fixed";     break;
     default          : _cp = "???";       break;
   }
  DBG_LOG( (DBG_SYM,"\tdefault   : %s", _cp) );
  DBG_LOG2( (DBG_SYM, attr->m_defValue,"\tdefValue :") );
}


/*
 *===========================================================================
 */
/*  7.31  */
void dbgSExtId(SExtId *p)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/* Code */

  DBG_LOG( (DBG_SYM,"External id:") );
  DBG_LOG2( (DBG_SYM,p->m_pubId,"\tpubid:") );
  DBG_LOG2( (DBG_SYM,p->m_sysId,"\tsysid:") );
}

#endif  /* WML_DEBUG    */

/*
=============================================================================
*/


/*  End of File  */

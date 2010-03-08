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
/* File    : U:\wpsvob\wmlenc\src\common\xmlvc.c
   Version : \main\8
   Date    : 17-Oct-00.12:50:10
*/

/*
 * ========================================================================
 *  Name        : xmlvc.c
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

        Implementation of the XML validity constraints.

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

        7.1   vcAuxElement
        7.2   vcUniqueElement
        7.3   vcElementForAttList
        7.4   vcCharData
        7.5   NameOk
        7.6   NmtokenOk
        7.7   VCNotify
        7.8   VCAttrTokenized
        7.9   VCAttrNotation
        7.10  vcAttribute
        7.11  vcUsedAttributes

*/



/*  3  GLOSSARY

        None

*/


/*  4  EXTERNAL RESOURCES  */

    /* None */

/*  4.1  Include Files  */

#include "xmlvc.h"
#include "xmlctyp.h"
#include "xmllex.h"
#include "xmlconst.h"

/*  4.2 External Data Structures  */

    /* None */


/*  4.3 External Function Prototypes  */

    /* None */


/*  5  LOCAL CONSTANTS AND MACROS  */

    /* None */



/*  6  MODULE DATA STRUCTURES  */


/*  6.1 Local Data Structures  */

static int st_emit_defaults = 0;


/*  6.2 Local Function Prototypes  */

    /* None */


/*  7 MODULE CODE  */


/*
 *===========================================================================
 */
/*  7.1  */
SElement * vcAuxElement(PARDATA d,      /* In: Parser data structure    */
                        PString name)   /* In: Name of the root element */
{
/*  Functional Description
 *
 *  Create and initializes the element, called the 'root of the root'.
 *  This auxiliary element is initialized to have the original root
 *  element as the only child. That is, its content model contains
 *  the name of the root element.
 *
 *  This function is called only once, from RuleDocument().
 *
 *  Return Value:  the parent of the root element.
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/*  Code  */

  SElement *_element = NEW_ELEMENT(d, g_strAuxName);

  if(name && !LOOKUP_ELEMENT(d,name))   /* If root declared */
      callError(d,XML_ERROR_NOT_DECLARED_ROOT,name);
  else
      strCpy(_element->m_contentModel, name);

  return _element;
}

/*
 *===========================================================================
 */
/*  7.2  */
SElement * vcUniqueElement(PARDATA d,       /* In: Parser data structure    */
                           PString name)    /* In: a new unique name        */
{
/*  Functional Description
 *
 *  Check if 'name' was unique name for an element type.
 *
 *  This function is called from RuleElementdecl().
 *
 *  Return Value:  the new element - if its name was unique
 *                 0               - otherwise
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  SElement *_element;
  
/*  Code  */

  _element = LOOKUP_ELEMENT(d,name);

  if(!_element)                                 /* name is not used     */
      _element = NEW_ELEMENT(d,name);
  else if(_element->m_notDeclared)              /* used but not declared */
      _element->m_notDeclared = 0;
  else {                                        /* it was declared      */
      callError(d,XML_ERROR_UNIQUE_ELEMENT_TYPE, name);   /* XR043 */
      _element = 0;
  }
  return _element;
}

/*
 *===========================================================================
 */
/*  7.3  */
SElement * vcElementForAttList(PARDATA d,   /* In: Parser data structure    */
                         PString name)      /* In: name of element          */
{
/*  Functional Description
 *
 *  Returns the element called 'name'.
 *
 *  This function is called from RuleAttlistDecl().
 *
 *  Return Value:  the element
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  SElement *_element = LOOKUP_ELEMENT(d,name);

/*  Code  */

  if( !_element ) {                             /* not found    */
      _element = NEW_ELEMENT(d,name);           /* create       */
      _element->m_notDeclared = 1;
      callWarning(d,XML_WARNING_ATT_LIST_BUT_ELEMENT,name);   /* XR052 */
  }
  else if(!SLL_EMPTY(_element->m_attrList)) {
      callWarning(d,XML_WARNING_MORE_ATTLIST,name);         /* XR053 */
  }
  return _element;
}


/*
 *===========================================================================
 */
/*  7.4  */
void vcCharData(PARDATA d,      /* In: Parser data structure    */
                PString text)   /* In: character data           */
{
/*  Functional Description
 *
 *  Checks if character data was a valid content for current element.
 *  If element may not have character data as content but 'text'
 *  is white spaces only, that is not error however.
 *
 *  This function is called from RuleContent() only.
 *
 *  Return Value:  -
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/*  Code  */

  if(!checkChild(d->m_contentParser,g_strPCDATA)) {
          if(!strStrip(text))                       /* XR044 */
              return;
      callError(d,XML_ERROR_NO_CHARDATA,text);
  }
}


/*
 *===========================================================================
 */
/*  7.5  */
static int NameOk(const PString name)   /* In: name     */
{
/*  Functional Description
 *
 *  Checks if 'name' matches the 'Name' production.
 *
 *  Return Values:  0         - does not match
 *                  otherwise - does
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  WCHAR *_p = STR_DATA(name);
  WCHAR *_e = _p+STR_LEN(name);

/*  Code  */

  if(_p>=_e)
      return 0;

  if( (IS_LETTER(*_p) || *_p=='_' || *_p==':') )
      while( (++_p < _e) && IS_NAMECHAR(*_p))
          ;
  return _p==_e;
}

/*
 *===========================================================================
 */
/*  7.6  */
static int NmtokenOk(const PString name,     /* In: nmtoken  */
                     int   more)
{
/*  Functional Description
 *
 *  Checks if 'name' matches the 'Nmtoken' production.
 *
 *  Return Values:  0         - does not match
 *                  otherwise - does
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  WCHAR *_p = STR_DATA(name);
  WCHAR *_e = _p+STR_LEN(name);

/*  Code  */

  if(_p>=_e)
      return 0;

  if( IS_NAMECHAR(*_p) ) {
      if(!more) {
          while( (++_p < _e) && IS_NAMECHAR(*_p))
              ;
      }
      else {
          while( (++_p < _e) && (IS_NAMECHAR(*_p) || (*_p==' ')) )
              ;
      }
  }

  return _p==_e;
}

/*
 *===========================================================================
 */
/*  7.7  */
static void VCNotify(PARDATA   d,
                     PString   name,
                     SGEntity *entity)
{
/*
 *
 *  Checks NOTATIONS.
 *
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  SNotation *_note;
  DBG_LOG2( (DBG_XML, name, "VCNotify name: ") );
  DBG_LOG2( (DBG_XML, entity->m_notation, "VCNotify notation: ") );

/*  Code  */

  if( (!entity->m_notation)
      || (!(_note=LOOKUP_NOTATION(d,entity->m_notation))) ) {
      callError(d,XML_ERROR_NOTATION_DECALRED,name);
  }
  else {
      callToken(d, XTOK_NOTE_UNPARSED_ENTITY, name);
      callToken(d, XTOK_NOTE_ENTITY_PUBID, entity->u.m_extId.m_pubId);
      callToken(d, XTOK_NOTE_ENTITY_SYSID, entity->u.m_extId.m_sysId);

      callToken(d, XTOK_NOTE_NOTIFICATION, entity->m_notation);
      callToken(d, XTOK_NOTE_NOTIFICATION_PUBID, _note->m_extId.m_pubId);
      callToken(d, XTOK_NOTE_NOTIFICATION_SYSID, _note->m_extId.m_sysId);
  }
}

/*
 *===========================================================================
 */
/*  7.8  */
static void VCAttrTokenized(PARDATA d,      /* In: Parser data structure    */
                            SAttribute *attr,
                            PString attrname,
                            PString value)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  SGEntity *_entity;

/*  Code  */

  /* TODO: One ID per Element Type */
  switch(attr->m_token) {
     case TOK_ID        : if(!NameOk(value))
                              callError(d,XML_ERROR_ID_NAME,value);      /* XR123 */
                          else if(LOOKUP_ID(d,value))
                              callError(d,XML_ERROR_ID_EXISTS,value);    /* XR056 */
                          else
                              NEW_ID(d,value);
                          break;
     case TOK_IDREF     : if(!NameOk(value))
                              callError(d,XML_ERROR_IDREF_NAME,value);
                          else if(!LOOKUP_ID(d,value))               /* XR059 */
                              callError(d,XML_ERROR_ID_REF,value);
                          break;
     case TOK_IDREFS    :   /* TODO: */
                          break;
     case TOK_ENTITY    : if( (_entity=LOOKUP_GE(d,value))    /* XR061 */
                                 && (_entity->m_type==ET_EXTERNAL_UNPARSED) ) {
                              VCNotify(d,value,_entity);
                          }
                          else
                              callError(d,XML_ERROR_ENTITY_NAME,value);
                          break;
     case TOK_ENTITIES  :  /* TODO: */
                          break;
     case TOK_NMTOKEN   : if(!NmtokenOk(value,0))
                              callError(d,XML_ERROR_NMTOKEN_NAME,value); /* XR063 */
                          break;
     case TOK_NMTOKENS  : if(!NmtokenOk(value,1))
                              callError(d,XML_ERROR_NMTOKEN_NAME,value); /* XR064 */
                          break;
     default            : assert(0);
                          break;
  }
}


/*
 *===========================================================================
 */
/*  7.9  */
static void VCAttrNotation(PARDATA d,
                           PString name,
                           PString value)
{
/*  
 *
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

}



/*
 *===========================================================================
 */
/*  7.10  */
void vcAttribute(PARDATA d,         /* In: Parser data structure    */
                 PString name,      /* In: attribute name           */
                 PString value)     /* In: attribute value          */
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Value:  -
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  SAttribute *_attr;
  int         _i;

/*  Code  */

  if(!d->m_element) {
      callError(d,XML_ERROR_GENERIC,0);
      return;
  }

  _attr = lookUpAttr(d->m_element,name);
  if(!_attr) {
      callError(d,XML_ERROR_ATTR_VALUE_TYPE, name);  /* XR034 */
      return;
  }

  for(_i=0; _i<d->m_nUsedAttr; _i++) {
      if(_attr==d->m_usedAttribute[_i]) {
          callError(d,XML_ERROR_UNIQUE_ATT_SPEC,name);   /* XR038 */
          return;
      }
  }

  if(_attr->m_default==AD_FIXED) {
      if(!strSame(_attr->m_defValue,value)) {
          callError(d,XML_ERROR_FIXED_ATT_DEF,name);   /* XR073 */
          return;                                 /* XR071 */
      }
  }

  if(d->m_nUsedAttr<N_USED_ATTR)
      d->m_usedAttribute[d->m_nUsedAttr++] = _attr;

  strNormalize(value);
  if(_attr->m_type!=AT_STRING) {
      /* Further normalization */
      if(strStrip(value))
          strOneSpace(value);
  }

  switch(_attr->m_type) {
     case AT_STRING    : break;    /* nothing to do: CDATA */
     case AT_TOKENIZED : VCAttrTokenized(d,_attr,name,value);
                         break;
     case AT_NOTATION  : VCAttrNotation(d,name,value);
                         break;
     case AT_ENUM      : if(!typeValueExists(_attr,value))  /* XR066 */
                             {
                                callError(d,XML_ERROR_ENUMERATION, value);
                             }
                         break;
     default           : assert(0); break;
  }
}

/*
 *===========================================================================
 */
/*  7.11  */
void  xmlEmitDefaults(void)
{
/*
 *---------------------------------------------------------------------------
 */
  st_emit_defaults = 1;
}

/*
 *===========================================================================
 */
/*  7.11  */
void vcUsedAttributes(PARDATA d)    /* In: Parser data structure    */
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Value:  -
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */
  SAttribute *_attr;
  int         _i;

/*  Code  */

  if(!d->m_element) {
      callError(d,XML_ERROR_GENERIC,0);
      return;
  }

  /* Check if all required attribute was used */
  for(_attr=d->m_element->m_attrList.m_root;  _attr;  _attr=_attr->m_next) {
      if(_attr->m_default==AD_REQUIRED) {
          for(_i=0; _i<d->m_nUsedAttr && (_attr!=d->m_usedAttribute[_i]) ; _i++)
              ;
          if(_i==d->m_nUsedAttr)
              callError(d,XML_ERROR_REQUIRED_ATT,_attr->m_name);   /* XR070 */
      }
  }

  if(st_emit_defaults) {
      /* Emit the implicit default values */
      for(_attr=d->m_element->m_attrList.m_root;  _attr;  _attr=_attr->m_next) {
          if(_attr->m_default==AD_DEFAULT) {
              for(_i=0;
                  _i<d->m_nUsedAttr && (_attr!=d->m_usedAttribute[_i]) ; _i++)
                  ;
              if(_i==d->m_nUsedAttr
                 && _attr->m_name
                 && _attr->m_defValue) {
                  callToken(d,XTOK_ATTR_NAME,_attr->m_name);
                  callToken(d,XTOK_ATTR_VALUE,_attr->m_defValue);
              }
          }
      }
  }
}

/*
=============================================================================
*/


/*  End of File  */

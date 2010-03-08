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
/* File    : U:\wpsvob\wmlenc\src\common\xmlrules.c
   Version : \main\11
   Date    : 17-Oct-00.12:49:31
*/

/*
 * ========================================================================
 *  Name        : xmlrules.c
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

/*  4  EXTERNAL RESOURCES  */

/*  4.1  Include Files  */

#include "xmlrules.h"
#include "xmlpars.h"
#include "xmlctyp.h"
#include "xmllex.h"
#include "xmlvc.h"

/*  4.2 External Data Structures  */

    /* None */

/*  4.3 External Function Prototypes  */

    /* None */

/*  5  LOCAL CONSTANTS AND MACROS  */

#define LOOKAHEAD_TRUE      1
#define LOOKAHEAD_FALSE     0
#define NRECURSION          20

#define DEEPNESS    d->m_eb.m_stackPos
#define GPTR    d->m_eb.m_gptr
#define EPTR    (d->m_eb.m_eptr)
#define CH      (*GPTR)
#define IS(ch)  (CH==ch)
#define GOOD    (GPTR<EPTR || popBuffer(d))
#define INC     ((++GPTR<EPTR || popBuffer(d)) ? ((IS('%')) ? PERepl(d) : 1):0)
#define INCR    {if(++GPTR<EPTR || popBuffer(d)) if((IS('%'))) PERepl(d); }

#define LETTER  ((CH>='a' && CH<='z')||(CH>='A' && CH<='Z'))
#define DIGIT   (CH>='0' && CH<='9')
#define VERNUMCH ( LETTER || DIGIT || IS('_') || IS('.') || IS(':') || IS('-'))

#define Expected(d,s) ((GOOD && *GPTR==*s) ? FuncExpected(d,s) : 0)
#define STR(tok)      (g_LexTable[(tok)])
#define EXPECTED(tok) Expected(d, g_LexTable[(tok)])

#define PUSH        (STR_CLEAR(*++d->m_valSP),1)
#define POP         (--d->m_valSP)
#define CUR_VALUE   (*d->m_valSP)
#define APPEND(ch)  strAppendChar(CUR_VALUE,ch)
#define APPEND_TOK(tok)  { APPEND(' '); strCatFromASCII(CUR_VALUE, STR(tok), strlen(STR(tok)));}

#define COPY_TO(s)  (strCpy(s,CUR_VALUE),STR_CLEAR(CUR_VALUE),1)
#define TMP         (*d->m_tmpSP)
#define PUBID       (d->m_extId.m_pubId)
#define SYSID       (d->m_extId.m_sysId)

#define ELEMENT     (d->m_element)
#define ATTR        (d->m_attribute)

#define XTRDUP(str)   strXDup(str,STR_POOL(d))
#define STRDEL(str)   deleteString(str,STR_POOL(d))

#define COMM        &d->m_commonData

/*  6  MODULE DATA STRUCTURES  */

    /* None */

/*  6.1 Local Data Structures  */

    /* None */

/*  6.2 Local Function Prototypes  */

int     PERepl
                (PARDATA d);

UINT8   XmlIsSpecial
                (PString s);

void    DeReference
                (PARDATA d,
                 ERefContext context);

int     FuncExpected
                (PARDATA d,
                 const char *s);

int     NextXmlToken
                (PARDATA d,
                 const int *tokens,
                 int lookahead);

int     NeedS
                (PARDATA d);

int     SkipS
                (PARDATA d);


/*  7 MODULE CODE  */

/*
 *===========================================================================
 */
/*  7.1  */
int RuleDocument(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Document production
 *  This is the 'main' for document
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */
  SContentParser _cParser;          /* Content parser for root */
  int _retv = 0;

/*  Code  */
  if( parse(d,RULE_PROLOG,' ')) {
      constrContentParser(d,
                           &_cParser,
                           vcAuxElement(d,d->m_nameOfRootElement));

      if( parse(d,RULE_ELEMENT,' ') /* Parsing root element */
          && parse(d,RULE_MISC,'*'))
          _retv++;

      destrContentParser(d,&_cParser);
  }

  if(!_retv) {                      /* Any kind of error  */
      callError(d,XML_ERROR_GENERIC,0);    /* XR001 */
  }
  else {
      if(GOOD && IS_SPACE(CH))      /* The rest of the doc is space only */
          while(INC && IS_SPACE(CH))
              ;
      if(GOOD) {
          callError(d,XML_ERROR_TEXT_AFTER_ROOT,0);
      }
  }

  return _retv;
}

/*
=============================================================================
 */
/*  7.2  */
int RuleS(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "S"
 *  It drains all of the insignificant white-spaces.
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */
  const WCHAR *_orig = GPTR;

/*  Code  */
  if(GOOD && IS_SPACE(CH))
      while(INC && IS_SPACE(CH))
          ;

  return _orig!=GPTR;
}

/*
 *===========================================================================
 */
/*  7.3  */
int RuleName(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "Name"
 *
 *  Append: The name
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  const WCHAR *_gptr = GPTR;

/*  Code  */
  if( GOOD && (IS_LETTER(CH) || IS('_') || IS(':')) ) {
      APPEND(*GPTR);
      while(INC && IS_NAMECHAR(CH)) {
          APPEND(*GPTR);
      }
  }
  return _gptr!=GPTR;
}

/*
 *===========================================================================
 */
/*  7.4  */
int RuleNmtoken(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "Nmtoken"
 *
 *  Append: The Nmtoken
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  const WCHAR *_orig = GPTR;

/*  Code  */
  if(GOOD && IS_NAMECHAR(CH)) {
      APPEND(*GPTR);
      while(INC && IS_NAMECHAR(CH))
          APPEND(*GPTR);
  }

  return GPTR!=_orig;
}

/*
 *===========================================================================
 */
/*  7.5  */
int RuleEntityValue(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "EntityValue"
 *
 *  Append: The literal entity value
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  WCHAR _delim;
  int   _deepness;

/*  Code  */
  if(GOOD && (IS('\"') || IS('\''))) {
      _delim=CH;
      _deepness = DEEPNESS;             /* pg.29 4.4.5 */
      INCR;
      while(GOOD && _deepness<=DEEPNESS) {
          switch(CH) {
             case '%' : callError(d,XML_ERROR_GENERIC,CUR_VALUE);
                        INCR;
                        break;
             case '&' : DeReference(d,RC_IN_ENTITY_VALUE);
                        break;
             default  : if(IS(_delim) && _deepness==DEEPNESS) {
                            INCR;
                            return 1;
                        }
                        APPEND(CH);
                        INCR;
                        break;
          }
      }
  }
  return 0;
}

/*
 *===========================================================================
 */
/*  7.6  */
int RuleAttValue(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "Attvalue"
 *
 *  Append: The literal atribute value
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  WCHAR _delim;
  int   _deepness;

/*  Code  */
  if(GOOD && (IS('\"') || IS('\''))) {
      _delim=CH;
      _deepness = DEEPNESS;             /* pg.29 4.4.5 */
      INCR;
      while(GOOD && _deepness<=DEEPNESS) {
          switch(CH) {
             case '<' : callError(d,XML_ERROR_NO_LT_IN_ATTVAL,CUR_VALUE);
                        INCR;
                        break;                      /* XR037 */
             case '&' : DeReference(d,RC_IN_ATTR_VALUE);
                        break;
             default  : if(IS(_delim) && _deepness==DEEPNESS) {
                            INCR;
                            return 1;
                        }
                        APPEND(CH);
                        INCR;
                        break;
          }
      }
  }
  return 0;
}

/*
 *===========================================================================
 */
/*  7.7  */
int RuleSystemLiteral(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "SystemLiteral"
 *
 *  Append: The literal
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  WCHAR _delim;

/*  Code  */
  if( GOOD && ((_delim=CH)=='\"' || _delim=='\'') ) {
      while(INC && (CH!=_delim)) {
         APPEND(CH) ;
      }
      if(GOOD && IS(_delim)) {
          INCR;
          d->m_literalFlag++;
          return 1;
      }
  }
  return 0;
}

/*
 *===========================================================================
 */
/*  7.8  */
int RulePubidLiteral(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "PubidLiteral"
 *
 *  Append: The literal
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  WCHAR _delim;

/*  Code  */
  if( GOOD && ((_delim=CH)=='\"' || _delim=='\'') ) {
      while(INC && IS_PUBIDCHAR(CH) && CH!=_delim)
          APPEND(CH);
      if(GOOD && IS(_delim)) {
          INCR;
          d->m_literalFlag++;
          return 1;
      }
  }
  return 0;
}

/*
 *===========================================================================
 */
/*  7.9  */
int RuleCharData(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "CharData"
 *
 *  Append: The character data
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  const WCHAR *_orig = GPTR;

/*  Code  */
  while( GOOD && CH!='<' && CH!='&' ) {
      /* XML spec: 2.4 : "]]>" in CDATA-section */
      APPEND(CH);
      INCR;
  }
  return _orig!=GPTR;
}

/*
 *===========================================================================
 */
/*  7.10  */
int RuleComment(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "Comment"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
  /* XR005 */
  /* XR006 */

/*  Code  */
  if(EXPECTED(TOK_COMMENT_START)) {
      int _seenEnd;
      int _dblHyphen=0;
      while(   !(_seenEnd  = EXPECTED(TOK_COMMENT_END))
            && !(_dblHyphen= EXPECTED(TOK_DOUBLE_HYPHEN))
            && INC)
          ;
      if(_seenEnd)
          return 1;
      if(_dblHyphen)
          callError(d,XML_ERROR_COMMENT,CUR_VALUE);
      else
          callError(d,XML_ERROR_GENERIC,CUR_VALUE);
  }
  return 0;
}

/*
 *===========================================================================
 */
/*  7.11  */
int RulePI(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "PI"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  int _retv = 0;

/*  Code  */
  if(EXPECTED(TOK_PI_START)) {
      PUSH;
      if(parse(d,RULE_PITARGET,' ') && COPY_TO(TMP)) {
          if(GOOD && IS_SPACE(CH)) {
              APPEND(CH);
              while(GOOD && (!EXPECTED(TOK_XML_PI_END))) {
                  APPEND(CH);
                  INCR;
              }
          }
          if(GOOD) {
              callToken(d,XTOK_PI_TARGET,TMP);
              callToken(d,XTOK_PI_VALUE,CUR_VALUE);
              _retv++;
          }
      }
      POP;
  }
  return _retv;
}

/*
 *===========================================================================
 */
/*  7.12  */
int RulePITarget(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "PITarget"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  PString _val = 0;

/*  Code  */
  PUSH;
  if(parse(d,RULE_NAME,' ')) {
      WCHAR *_data = STR_DATA(CUR_VALUE);
      if( (STR_LEN(CUR_VALUE)!=3)
          || (_data[0]!='x' && _data[0]!='X')
          || (_data[1]!='m' && _data[1]!='M')
          || (_data[2]!='l' && _data[2]!='L'))
          _val = CUR_VALUE;
  }
  POP;
  if(_val)
      strCat(CUR_VALUE,_val);
  return _val!=0;
}

/*
 *===========================================================================
 */
/*  7.13  */
int RuleCDSect(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "CDSect"
 *
 *  Append: The 'CData'
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
  /* XR004 */

/*  Code  */
  if( EXPECTED(TOK_CD_START) ) {
      int _seenEnd;
      while(!(_seenEnd=EXPECTED(TOK_CD_SECT_END))) {
          if(GOOD)
              APPEND(CH);
          if(!INC)
              break;
      }
      if(!_seenEnd)
          callError(d,XML_ERROR_GENERIC,CUR_VALUE);
      return _seenEnd;
  }
  return 0;
}

/*
 *===========================================================================
 */
/*  7.14  */
int RuleProlog(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "Prolog"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Code  */
  if(parse(d,RULE_XMLDECL,'?')
     && parse(d,RULE_MISC,'*')) {
      if(parse(d,RULE_DOCTYPEDECL,' ')) {
          parse(d,RULE_MISC,'*');
      }
      return 1;
  }
  callError(d,XML_ERROR_GENERIC,0);
  return 0;
}

/*
 *===========================================================================
 */
/*  7.15  */
int RuleXMLDecl(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "XMLDecl"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Code  */
  if(    EXPECTED(TOK_XML_START)
      && parse(d,RULE_VERSIONINFO,' ')
      && parse(d,RULE_ENCODINGDECL,'?')
      && parse(d,RULE_SDDECL,'?')
      && SkipS(d)
      && EXPECTED(TOK_XML_PI_END)) {
      return 1;
  }
  return 0;
}

/*
 *===========================================================================
 */
/*  7.16  */
int RuleVersionInfo(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "VersionInfo"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
  /* XR008 */

/*  Data Structures  */
  WCHAR _delim;

/*  Code  */
  if( NeedS(d)
      && EXPECTED(TOK_VERSION)
      && parse(d,RULE_EQ,' ')
      && GOOD ) {
      _delim = CH;
      if( (_delim=='\"' || _delim=='\'') && INC ) {
          PUSH;
          if( parse(d,RULE_VERSIONNUM,' ')
              && GOOD
              && IS(_delim)) {
              INCR;
              if(strSame(g_strXmlVer,CUR_VALUE)) {
                  POP;
                  return 1;
              }
              else {
                  callError(d,XML_ERROR_VERSION, CUR_VALUE);
              };

          }
          POP;
      }
  }
  return 0;
}

/*
 *===========================================================================
 */
/*  7.17  */
int RuleEq(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "Eq"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Code  */
  return SkipS(d)
      && GOOD
      && IS('=') && INC
      && SkipS(d);
}

/*
 *===========================================================================
 */
/*  7.18  */
int RuleVersionNum(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "VersionNum"
 *
 *  Append: The version number
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Code  */
  while(GOOD) {
      if( VERNUMCH ) {
          APPEND(CH);
          while(INC && VERNUMCH)
              APPEND(CH);
          return 1;
      }
  }
  return 0;
}

/*
 *===========================================================================
 */
/*  7.19  */
int RuleMisc(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "Misc"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Code  */
  return parse(d,RULE_COMMENT,' ')
      || parse(d,RULE_PI,' ')
      || NeedS(d);
}

/*
 *===========================================================================
 */
/*  7.20  */
int RuleDoctypedecl(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "Doctypedecl"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  int     _retv  = 0;

/*  Code  */
  STR_CLEAR(CUR_VALUE);

  if(EXPECTED(TOK_DOCTYPE)
     && NeedS(d)
     && parse(d,RULE_NAME,' ')) {
      strCpy(d->m_nameOfRootElement,CUR_VALUE);
      if(NeedS(d) && parse(d,RULE_EXTERNALID,' ')) {
          strCpy(d->m_dtdId.m_pubId, PUBID);
          strCpy(d->m_dtdId.m_sysId, SYSID);
      }
      SkipS(d);
      if(GOOD) {
          if(IS('>')) {
              INCR;
              _retv = 1;
          }
          else if(IS('[')) {
              INCR;
              while(    parse(d,RULE_MARKUPDECL,' ')
                        || parse(d,RULE_PEREFERENCE,' ')
                        || NeedS(d))
                  ;
              if(GOOD && IS(']') && INC && SkipS(d) && GOOD && IS('>')) {
                  INCR;
                  _retv = 1;
              }
          }
      }
  }

  if(_retv) {

      if(STR_LEN(d->m_dtdId.m_pubId) || STR_LEN(d->m_dtdId.m_sysId) ) {
          if(!d->m_idOnlyFlag && !parseExternal(d,RULE_EXTSUBSET,&d->m_dtdId))
              _retv = 0;
      }

  }
  ELEMENT = 0;  /* initial 0 for outside DTD */

  return _retv;
}

/*
 *===========================================================================
 */
/*  7.21  */
int RuleMarkupdecl(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "Markupdecl"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  const int s_tokens[]={TOK_PI_START,
                        TOK_NOTATION_START,
                        TOK_ENTITY_START,
                        TOK_ELEMENT_START,
                        TOK_ATTLIST_START,
                        TOK_COMMENT_START,
                        0};
  int   _retv=0;
  int   _nextToken;

/*  Code  */
  _nextToken = NextXmlToken(d, s_tokens, LOOKAHEAD_TRUE);
  switch(_nextToken) {
     case TOK_ELEMENT_START : _retv = parse(d,RULE_ELEMENTDECL,' ') ;
                              break;
     case TOK_ATTLIST_START : _retv = parse(d,RULE_ATTLISTDECL,' ') ;
                              break;
     case TOK_ENTITY_START  : _retv = parse(d,RULE_ENTITYDECL,' ')  ;
                              break;
     case TOK_NOTATION_START: _retv = parse(d,RULE_NOTATIONDECL,' ');
                              break;
     case TOK_PI_START      : _retv = parse(d,RULE_PI,' ')          ;
                              break;
     case TOK_COMMENT_START : _retv = parse(d,RULE_COMMENT,' ');    ;
                              break;
     default                : _retv = 0;
                              break;
  }
  return _retv;
}

/*
 *===========================================================================
 */
/*  7.22  */
int RuleExtSubset(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "ExtSubset"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Code  */
  return parse(d,RULE_TEXTDECL,'?')
      && parse(d,RULE_EXTSUBSETDECL,' ');
}

/*
 *===========================================================================
 */
/*  7.23  */
int RuleExtSubsetDecl(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "ExtSubsetDecl"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  int _retv = 0;

/*  Code  */
  while(1) {
      if(parse(d,RULE_MARKUPDECL,' ')) {
         _retv++;
      }
      else if(parse(d,RULE_CONDITIONALSECT,' ')) {
         _retv++;
      }
      else if(parse(d,RULE_PEREFERENCE,' ')) {
         _retv++;
      }
      else if(NeedS(d)) {
         _retv++;
      }
      else
         break;
  }
  return _retv;
}

/*
 *===========================================================================
 */
/* 7.24  */
int RuleSDDecl(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "SDDecl"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  int _retv = 0;
  WCHAR _delim;
/*  Code  */
  if(NeedS(d)
     && EXPECTED(TOK_STANDALONE)
     && parse(d,RULE_EQ,' ')
     && GOOD ) {
      if((IS('\"') || IS('\''))) {
          _delim=CH;
          INCR;
          if( (EXPECTED(TOK_YES) && ++d->m_standalone) || EXPECTED(TOK_NO)) {
              if(GOOD && IS(_delim)) {
                  INCR;
                  _retv++;
              }
          }
      }
      if(!_retv)
          callError(d,XML_ERROR_SA_YES_OR_NO,0);  /* XR015    */
  }
  return _retv;
}

/*
 *===========================================================================
 */
/*  7.25  */
int RuleElement(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "Element"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  int _retv = 0;
  SContentParser _cParser;

/*  Code  */
  PUSH;
  if(GOOD && IS('<') && INC
     && parse(d,RULE_NAME,' ')) {

      COPY_TO(TMP);
      if(   (++d->m_elementNestingLevel != 1)
         && STR_SAME(TMP,d->m_nameOfRootElement))
              callError(d,XML_ERROR_ONE_ROOT,TMP);  /* XR002    */

      ELEMENT = LOOKUP_ELEMENT(d,TMP);      /* Attributes use 'ELEMENT' */
      if(!ELEMENT)
          callError(d,XML_ERROR_NOT_DECL_ELEMENT,TMP);
      else if(!checkChild(d->m_contentParser,TMP)) {
          if(d->m_contentParser->m_parent)
              callError(d,XML_ERROR_NOT_CHILD,TMP);
          else
              callError(d,XML_ERROR_ROOT_ELEMENT_TYPE,TMP);   /* XR009    */
      }

      constrContentParser(d,&_cParser,ELEMENT);    /* TODO: redundant */

      callToken(d,XTOK_START_ELEMENT,TMP);

      d->m_nUsedAttr = 0;

      while(NeedS(d) && parse(d,RULE_ATTRIBUTE,' '))
          ;

      vcUsedAttributes(d);

      SkipS(d);
      if(GOOD && IS('>')) {                                 /* STag */
          INCR;
          if( parse(d,RULE_CONTENT,' ')) {                  /* Content */
              STR_CLEAR(CUR_VALUE);
              if( GOOD && IS('<') && INC && IS('/') && INC
                  && parse(d,RULE_NAME,' ')
                  && SkipS(d)
                  && GOOD && IS('>')) {                     /* ETag */
                  INCR;
                  if(!strSame(CUR_VALUE,TMP))               /* XR027 */
                      callError(d,XML_ERROR_START_END,CUR_VALUE);
                  _retv = 1;
              }
          }
      }
      else if(GOOD && IS('/') && INC && IS('>')) {      /* EmptyElemTag */
          INCR;
          _retv = 1;
      }
      callToken(d,XTOK_END_ELEMENT,TMP);

      checkContent(d,&_cParser,TMP);
      destrContentParser(d,&_cParser);       /* Here we leave the element */
      d->m_elementNestingLevel--;
  }
  POP;
  return _retv;
}

/*
 *===========================================================================
 */
/*  7.26  */
int RuleAttribute(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "Attribute"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Code  */
  PUSH;
  if( parse(d,RULE_NAME,' ') && COPY_TO(TMP)
      && parse(d,RULE_EQ,' ')
      && parse(d,RULE_ATTVALUE,' ')) {

      vcAttribute(d, TMP, CUR_VALUE);
      callToken(d,XTOK_ATTR_NAME,TMP);
      callToken(d,XTOK_ATTR_VALUE,CUR_VALUE);

      POP;
      return 1;
  }
  POP;
  return 0;
}

/*
 *===========================================================================
 */
/*  7.27  */
int RuleContent(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "content"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  int _tok;
  const int s_tokens[]={TOK_PI_START,
                        TOK_CD_START,
                        TOK_COMMENT_START,
                        0};

/*  Code  */
  while(1) {
      _tok = NextXmlToken(d, s_tokens, LOOKAHEAD_TRUE);
      PUSH;
      if( _tok==TOK_CD_START ) {                /* CDATA    */
          if(!parse(d,RULE_CDSECT,' '))         /* XR007    */
              break;
          callToken(d,XTOK_UNPARSED_CHARDATA,CUR_VALUE);
      }
      else if( _tok==TOK_PI_START ) {                /* PI       */
          if(!parse(d,RULE_PI,' '))
              break;
        /*  callToken(d,XTOK_PI_TARGET,CUR_VALUE); */
      }
      else if( _tok==TOK_COMMENT_START) {            /* Comment  */
          if(!parse(d,RULE_COMMENT,' '))
              break;
          /* XR005
           * Drain comment in content
           */
      }
      else if(GOOD) {
          if(IS('<')) {
              /* Here is the recursion for nested elements. */
              if(!parse(d,RULE_ELEMENT,' ')) {  /* Element  */
                  POP;
                  return 1;    /*  Exit here!
                                *  if error occured at RuleElement, error msg
                                *  was sent. Otherwise it is not an error.
                                */
              }
          }
          else if(IS('&')) {                    /* Reference    */
              DeReference(d,RC_IN_CONTENT);
              /* TODO: ? */
                  if(STR_LEN(CUR_VALUE))
                      callToken(d,XTOK_PARSED_CHARDATA,CUR_VALUE);

          }
          else {
              if(parse(d,RULE_CHARDATA,' ')) {  /* CharData     */
                  vcCharData(d,CUR_VALUE);      /* White space
                                                 * is ignored in root.
                                                 */
                  if(STR_LEN(CUR_VALUE))
                      callToken(d,XTOK_PARSED_CHARDATA,CUR_VALUE);
              }
              else {
                  callError(d,XML_ERROR_GENERIC,CUR_VALUE);
                  break;
              }
          }
      }
      else  /* GOOD */
          break;        /* unexpected EOF, return FAILURE
                           however the contents possible valid
                         */
      POP;  /* in while loop */
  }
  POP;
  return 0;
}

/*
 *===========================================================================
 */
/*  7.28  */
int RuleElementdecl(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "Elementdecl"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  SElement *_element;

/*  Code  */
  PUSH;
  if(EXPECTED(TOK_ELEMENT_START)
     && NeedS(d)
     && parse(d,RULE_NAME,' ') && COPY_TO(TMP)
     && NeedS(d)
     && parse(d,RULE_CONTENTSPEC,' ')
     && SkipS(d)
     && GOOD && IS('>')) {
      INCR;

      if( (_element=vcUniqueElement(d,TMP)) )
          strCpy(_element->m_contentModel,CUR_VALUE);

      POP;
      return 1;
  }
  POP;
  return 0;
}
/*
 *===========================================================================
 */
/*  7.29  */
int RuleContentspec(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "Contentspec"
 *
 *  Append: "EMPTY" or "ANY" or nothing
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */
  int _retv = 0;

/*  Code  */

  if(EXPECTED(TOK_EMPTY)) {
      strCat(CUR_VALUE,g_strEmpty);
      _retv++;
  }
  else if(EXPECTED(TOK_ANY)) {
      strCat(CUR_VALUE,g_strAny);
      _retv++;
  }
  else if(parse(d,RULE_MIXED,' ')) {
      _retv++;
  }
  else if(parse(d,RULE_CHILDREN,' ') ) {
      _retv++;
  }
  return _retv;
}

/*
 *===========================================================================
 */
/*  7.30  */
int RuleChildren(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "Children"
 *
 *  Append: The option if any
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */

/*  Code  */
  if(parse(d,RULE_CHOICE,' ') ) {
     /* || parse(d,RULE_SEQ,' ')) { */  /* choice handles seq */
      if(GOOD) {
          if(IS('?') || IS('*') || IS('+')) {
              APPEND(CH);
              INCR;
          }
          return 1;
      }
  }
  return 0;
}

/*
 *===========================================================================
 */
/*  7.31  */
int RuleCp(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "cp"
 *
 *  Append: The option if any
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Code  */
  if(parse(d,RULE_NAME,' ')
    || parse(d,RULE_CHOICE,' ')) {
/*  || parse(d,RULE_SEQ,' ')) { */  /* choice handles seq */
      if(GOOD) {
          if(IS('?') || IS('*') || IS('+')) {
              APPEND(*GPTR);
              INCR;
          }
          return 1;
      }
  }
  return 0;
}

/*
 *===========================================================================
 */
/*  7.32  */
int RuleChoice(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "choice"
 *
 *  Append: The choice/seq string, including the characters: (),|
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  const WCHAR *_eptr;

/*  Code  */
  if(GOOD && IS('(') && (_eptr=EPTR) && INC && SkipS(d)) {
      APPEND('(');
      if(parse(d,RULE_CP,' ') ) {
          SkipS(d);
          if(GOOD) {
              WCHAR _cs = CH;
              if(_cs=='|' || _cs==',') {
                  while(SkipS(d) && GOOD && IS(_cs)) {
                      APPEND(*GPTR);
                      INCR;
                      SkipS(d);
                      if(parse(d,RULE_CP,' ')) {
                          /* TODO */
                      }
                      else
                          break;
                  }
                  if(SkipS(d) && GOOD && IS(')')) {
                      if(EPTR!=_eptr)
                          callError(d,XML_ERROR_PROPER_GROUP,0);
                      APPEND(*GPTR);
                      INCR;
                      return 1;
                  }
              }
              else if(_cs==')') {
                  if(EPTR!=_eptr)
                      callError(d,XML_ERROR_PROPER_GROUP,0);
                  APPEND(*GPTR);
                  INCR;
                  return 1;
              }
          }
      }
  }
  return 0;
}

/*
 *===========================================================================
 */
/*  7.33  */
int RuleMixed(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "Mixed"
 *
 *  Append: The model string, including the initial and terminating
 *          characters.
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Code  */
  if(GOOD && IS('(')
     && INC
     && SkipS(d)
     && EXPECTED(TOK_PCDATA)
     && SkipS(d)) {
      APPEND('(');
      strCat(CUR_VALUE,g_strPCDATA);
      if(GOOD && IS(')')) {
          APPEND(*GPTR);
          INCR;
          return 1;
      }
      while(GOOD && IS('|')) {
          APPEND(*GPTR);
          INCR;
          SkipS(d);
          if(parse(d,RULE_NAME,' ')) {
              /* TO DO */
          }
          else {
              return 0;
          }
          SkipS(d);
      }
      SkipS(d);
      if(GOOD && IS(')') && INC && IS('*')) {
          APPEND(')');
          APPEND(*GPTR);
          INCR;
          return 1;
      }
  }
  return 0;
}

/*
 *===========================================================================
 */
/*  7.34  */
int RuleAttlistDecl(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "AttlistDecl"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Code  */
  PUSH;
  if(EXPECTED(TOK_ATTLIST_START)) {
      if(NeedS(d)
         && parse(d,RULE_NAME,' ')) {

          ELEMENT = vcElementForAttList(d, CUR_VALUE);

          if(parse(d,RULE_ATTDEF,'*')
             && SkipS(d)
             && SkipS(d)
             && GOOD && IS('>')) {
              INCR;
              POP;
              return 1;
          }
      }
      callError(d,XML_ERROR_GENERIC,CUR_VALUE);
  }
  POP;
  return 0;
}

/*
 *===========================================================================
 */
/*  7.35  */
int RuleAttDef(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal AttDef
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  SAttribute *_attr;

/*  Code  */
  PUSH;
  if(NeedS(d)
     && parse(d,RULE_NAME,' ') && COPY_TO(TMP)
     && NeedS(d)
     && parse(d,RULE_ATTTYPE,' ')
     && NeedS(d)
     && parse(d,RULE_DEFAULTDECL,' ') ) {

      _attr = lookUpAttr(ELEMENT,TMP);
      if(_attr) {
          deleteAttr(ATTR,COMM);
          callWarning(d,XML_WARNING_ATTDEF_IGNORED,TMP);            /* XR054 */
      }
      else {
          strCpy(ATTR->m_name,TMP);
          if(   strSame(TMP,g_strID)
             && (ATTR->m_default != AD_REQUIRED)
             && (ATTR->m_default != AD_IMPLIED) )
              callError(d,XML_ERROR_ID_ATT_DEFAULT,TMP);    /* XR058 */
          addAttr(ELEMENT,ATTR);                        /* XR053 */
      }
      ATTR = newAttr(COMM);
      POP;
      return 1;
  }
  POP;
  return 0;
}

/*
 *===========================================================================
 */
/*  7.36  */
int RuleAttType(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "AttType"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Code  */
  if( parse(d,RULE_STRINGTYPE,' ')
      || parse(d,RULE_TOKENIZEDTYPE,' ')
      || parse(d,RULE_ENUMERATEDTYPE,' ')) {

      return 1;
  }
  return 0;
}

/*
 *===========================================================================
 */
/*  7.37  */
int RuleStringType(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "StringType"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Code  */
  if( EXPECTED(TOK_CDATA) ) {
      ATTR->m_type = AT_STRING;
      return 1;
  }
  return 0;
}

/*
 *===========================================================================
 */
/*  7.38  */
int RuleTokenizedType(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "TokenizedType"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  const int s_tokens[]={TOK_NMTOKENS,
                        TOK_NMTOKEN,
                        TOK_IDREFS,
                        TOK_IDREF,
                        TOK_ID,
                        TOK_ENTITY,
                        TOK_ENTITIES,
                        0};

/*  Code  */
  switch(NextXmlToken(d, s_tokens, LOOKAHEAD_FALSE)) {
     case TOK_ID        : ATTR->m_type = AT_TOKENIZED;
                          ATTR->m_token= TOK_ID;
                          return 1;
     case TOK_IDREF     : ATTR->m_type = AT_TOKENIZED;
                          ATTR->m_token= TOK_IDREF;
                          return 1;
     case TOK_IDREFS    : ATTR->m_type = AT_TOKENIZED;
                          ATTR->m_token= TOK_IDREFS;
                          return 1;
     case TOK_ENTITY    : ATTR->m_type = AT_TOKENIZED;
                          ATTR->m_token= TOK_ENTITY;
                          return 1;
     case TOK_ENTITIES  : ATTR->m_type = AT_TOKENIZED;
                          ATTR->m_token= TOK_ENTITIES;
                          return 1;
     case TOK_NMTOKEN   : ATTR->m_type = AT_TOKENIZED;
                          ATTR->m_token= TOK_NMTOKEN;
                          return 1;
     case TOK_NMTOKENS  : ATTR->m_type = AT_TOKENIZED;
                          ATTR->m_token= TOK_NMTOKENS;
                          return 1;
  }
  return 0;
}

/*
 *===========================================================================
 */
/*  7.39  */
int RuleEnumeratedType(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "EnumeratedType"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
  return parse(d,RULE_NOTATIONTYPE,' ')
      || parse(d,RULE_ENUMERATION,' ');
}

/*
 *===========================================================================
 */
/*  7.40  */
int RuleNotationType(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "NotationType"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Code  */
  PUSH;
  if(EXPECTED(TOK_NOTATION)
     && NeedS(d)
     && GOOD && IS('(') && INC
     && SkipS(d)
     && parse(d,RULE_NAME,' ')) {

      ATTR->m_type = AT_NOTATION;
      addTypeValue(ATTR,CUR_VALUE,COMM);

      if(!LOOKUP_NOTATION(d,CUR_VALUE))     /* XR065 */
          callError(d,XML_ERROR_NOTATION_DECALRED,CUR_VALUE);

      while(SkipS(d) && GOOD && IS('|')) {
          INCR;
          SkipS(d);
          STR_CLEAR(CUR_VALUE);
          if(parse(d,RULE_NAME,' ')) {
              addTypeValue(ATTR,CUR_VALUE,COMM);
              if(!LOOKUP_NOTATION(d,CUR_VALUE))     /* XR065 */
                  callError(d,XML_ERROR_NOTATION_DECALRED,CUR_VALUE);
          }
      }
      if(SkipS(d) && GOOD && IS(')')) {
          INCR;
          POP;
          return 1;
      }
  }
  POP;
  return 0;
}

/*
 *===========================================================================
 */
/*  7.41  */
int RuleEnumeration(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "Enumeration"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Code  */
  PUSH;
  if(GOOD && IS('(') && INC
     && SkipS(d)
     && parse(d,RULE_NMTOKEN,' ') ) {

      ATTR->m_type = AT_ENUM;
      addTypeValue(ATTR,CUR_VALUE,COMM);

      while(SkipS(d) && GOOD && IS('|')) {
          INCR;
          SkipS(d);
          STR_CLEAR(CUR_VALUE);
          if(parse(d,RULE_NMTOKEN,' ')) {
#if 0
              /* FOR INTEROPERABILITY */
              if(typeValueExists(ATTR,CUR_VALUE)) /* XR067 */
                  callWarning(d,...);
#endif
              addTypeValue(ATTR,CUR_VALUE, COMM);
          }
      }
      SkipS(d);
      if(IS(')')) {
          INCR;
          POP;
          return 1;
      }
  }
  POP;
  return 0;
}

/*
 *===========================================================================
 */
/*  7.42  */
int RuleDefaultDecl(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "DefaultDecl"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Code  */
  if(EXPECTED(TOK_REQUIRED)) {
      ATTR->m_default = AD_REQUIRED;
      return 1;
  }
  else if(EXPECTED(TOK_IMPLIED)) {
      ATTR->m_default = AD_IMPLIED;
      return 1;
  }
  else {
      ATTR->m_default = AD_DEFAULT;
      if(EXPECTED(TOK_FIXED)) {
          if(!NeedS(d)) {
              return 0;
          }
          ATTR->m_default = AD_FIXED;
      }
      PUSH;
      if(parse(d,RULE_ATTVALUE,' ')) {
          strCpy(ATTR->m_defValue,CUR_VALUE);
          POP;
          return 1;
      }
      POP;
      return 1;
  }
  return 0;
}

/*
 *===========================================================================
 */
/*  7.43  */
int RuleConditionalSect(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "ConditionalSect"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Code  */
  return parse(d,RULE_INCLUDESECT,' ')      /* XR078 */
      || parse(d,RULE_IGNORESECT,' ');
}

/*
 *===========================================================================
 */
/*  7.44  */
int RuleIncludeSect(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "IncludeSect"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Code  */

  if(EXPECTED(TOK_SECTION_START)        /* XR078 */
     && SkipS(d)
     && EXPECTED(TOK_INCLUDE)
     && SkipS(d)
     && GOOD && IS('[') && INC
     && parse(d,RULE_EXTSUBSETDECL,' ')
     && EXPECTED(TOK_CD_SECT_END) ) {
      return 1;
  }
  return 0;
}

/*
 *===========================================================================
 */
/*  7.45  */
int RuleIgnoreSect(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "IgnoreSect"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Code  */
  if(EXPECTED(TOK_SECTION_START)        /* XR078 */
     && SkipS(d)
     && EXPECTED(TOK_IGNORE)
     && SkipS(d)
     && GOOD && IS('[') && INC) {

      parse(d,RULE_IGNORESECTCONTENTS,' '); /* note the ' ' */
      return 1;
  }
  return 0;
}

/*
 *===========================================================================
 */
/*  7.46  */
int RuleIgnoreSectContents(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "IgnoreSectContents"
 *
 *  Return Values:  Success !
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  UINT16    _deepness = 1;

/*  Code  */
    /* XR078 */
  do {
      if(EXPECTED(TOK_SECTION_START))
          _deepness++;
      if(EXPECTED(TOK_CD_SECT_END) && (!--_deepness))
          break;
  }while(INC);

  return 1;
}

/*
 *===========================================================================
 */
/*  7.47  */
static int ChRefHex(PARDATA d)
{
/*  Functional Description
 *
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  UINT32   _ch = 0;

/*  Code  */
  while(GOOD) {
      if(IS(';')) {
          INCR;
          if(IS_XMLCHAR(_ch)) {
              APPEND((WCHAR)_ch);
              return 1;
          }
          break;
      }
      else if( DIGIT ) {
          _ch*=16;
          _ch+=(CH-'0');
      }
      else if(CH>='a' && CH<='f') {
          _ch*=16;
          _ch+=(CH-'a'+10);
      }
      else if(CH>='A' && CH<='F') {
          _ch*=16;
          _ch+=(CH-'A'+10);
      }
      else {
          break;
      }
      INCR;
      if(_ch>=0xffffU && (sizeof(WCHAR)<4U)) {
          break;
      }
  }
  callError(d,XML_ERROR_LEGAL_CHAR,0);     /* XR082 */
  return 0;
}

/*
 *===========================================================================
 */
/*  7.48  */
static int ChRefDec(PARDATA d)
{
/*  Functional Description
 *
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  UINT32   _ch = 0;

/*  Code  */
  while(GOOD) {
      if(IS(';')) {
          INCR;
          if(IS_XMLCHAR(_ch)) {
              APPEND((WCHAR)_ch);
              return 1;
          }
          break;
      }
      else if( DIGIT ) {
          _ch*=10;
          _ch+=(CH-'0');
          INCR;
      }
      else {
          break;
      }
      if(_ch>=0xffffU && (sizeof(WCHAR)<4U)) {
          break;
      }
  }
  callError(d,XML_ERROR_LEGAL_CHAR,0);     /* XR082 */
  return 0;
}

/*
 *===========================================================================
 */
/*  7.49  */
int RuleCharRef(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "CharRef"
 *
 *  Append: The referenced character.
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */

/*  Code  */
  if(GOOD && IS('&') && INC && IS('#') && INC) {
      if(IS('x') && INC) {
          return ChRefHex(d);
      }
      else {
          return ChRefDec(d);
      }
  }
  return 0;
}

/*
 *===========================================================================
 */
/*  7.50  */
int RuleEntityRef(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  This function just checks the syntax, and builds the name
 *  of the entity reference. This function is called by
 *  DeReference() only (via the Parse() of course).
 *  The real processing takes place in the local funcion: DeReference().
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Code  */
  if(GOOD && IS('&')) {
      INCR;
      if(parse(d,RULE_NAME,' ')
         && IS(';')) {
          INCR;
          return 1;
      }
  }
  return 0;
}

/*
 *===========================================================================
 */
/*  7.51  */
int RulePEReference(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  This function just checks the syntax/context, and builds the name
 *  of the parameter entity reference.
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Code  */
  DBG_ASSERT(isParent(d,RULE_DOCTYPEDECL));

  if(GOOD && IS('%')) {
      INCR;
      if(parse(d,RULE_NAME,' ')
         && IS(';')) {
          INCR;
          if(inRootDocument(d) && isParent(d,RULE_MARKUPDECL)) {  /* XR012 */
              callError(d,XML_ERROR_PE_IN_INTERNAL_SUBSET,0);
          }
          return 1;
      }
  }
  return 0;
}

/*
 *===========================================================================
 */
/*  7.52  */
int RuleEntityDecl(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "EntityDecl"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Code  */
  return parse(d,RULE_GEDECL,' ')
      || parse(d,RULE_PEDECL,' ');
}

/*
 *===========================================================================
 */
/*  7.53  */
int RuleGEDecl(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "GEDecl"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  SGEntity *_ge;

/*  Code  */
  PUSH;
  if(EXPECTED(TOK_ENTITY_START)) {
      STR_CLEAR(PUBID);
      STR_CLEAR(SYSID);
      STR_CLEAR(d->m_notation);
      if( NeedS(d)
          && parse(d,RULE_NAME,' ') && COPY_TO(TMP)
          && NeedS(d)
          && parse(d,RULE_ENTITYDEF,' ')    /* sets unpars/literal flags */
          && SkipS(d)
          && IS('>')) {
          INCR;
          if(LOOKUP_GE(d,TMP))
              callWarning(d,XML_WARNING_MORE_ENTITY_DECL,TMP);   /* XR090 */
          else {
              _ge = NEW_GE(d,TMP);
              if(d->m_literalFlag) {
                  _ge->m_type = d->m_unParsedFlag ?
                      ET_EXTERNAL_UNPARSED : ET_EXTERNAL_PARSED;
                  _ge->u.m_extId.m_pubId= XTRDUP(PUBID);
                  _ge->u.m_extId.m_sysId= XTRDUP(SYSID);
                  if(_ge->m_type==ET_EXTERNAL_UNPARSED) {
                      _ge->m_notation = XTRDUP(d->m_notation);
                  }
              }
              else {
                  _ge->m_type = ET_INTERNAL;
                  _ge->u.m_value= XTRDUP(CUR_VALUE);
              }
          }
          POP;
          return 1;
      }
      else {
          /* TODO */
         /* callError(d,XML_ERROR_GENERIC,0); */
      }
  }
  POP;
  return 0;
}

/*
 *===========================================================================
 */
/*  7.54  */
int RulePEDecl(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "PEDecl"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/* Data Structures */
  int _retv = 0;
  SPEntity *_pe;
/*  Code  */
  if(EXPECTED(TOK_ENTITY_START)
     && NeedS(d)
     && (IS('%')) && INC
     && NeedS(d)) {
      PUSH;
      STR_CLEAR(PUBID);
      STR_CLEAR(SYSID);
      d->m_literalFlag = 0;

      if( parse(d,RULE_NAME,' ') && COPY_TO(TMP)
          && NeedS(d)
          && parse(d,RULE_PEDEF,' ')
          && SkipS(d)
          && IS('>')) {

          INCR;
          if(LOOKUP_PE(d,TMP))
              callWarning(d,XML_WARNING_MORE_ENTITY_DECL,TMP);   /* XR090 */
          else {
              _pe = NEW_PE(d,TMP);
              if(!d->m_literalFlag) {
                  _pe->m_type = ET_INTERNAL;
                  _pe->u.m_value = XTRDUP(CUR_VALUE);
              }
              else {
                  _pe->m_type = ET_EXTERNAL_PARSED;
                  _pe->u.m_extId.m_pubId= XTRDUP(PUBID);
                  _pe->u.m_extId.m_sysId= XTRDUP(SYSID);
              }
          }
          _retv++;
      }
      POP;
  }
  return _retv;
}

/*
 *===========================================================================
 */
/*  7.55  */
int RuleEntityDef(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "EntityDef"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Code  */
  d->m_unParsedFlag= 0;     /* initialize here */
  d->m_literalFlag = 0;

  if(parse(d,RULE_ENTITYVALUE,' ')) {
      return 1;
  }
  else if(parse(d,RULE_EXTERNALID,' ')
          && parse(d,RULE_NDATADECL,'?') ) {
      return 1;
  }
  return 0;
}

/*
 *===========================================================================
 */
/*  7.56  */
int RulePEDef(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "PEDef"
 *
 *  Append: leading/trailing spaces.
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Code  */
  APPEND(' ');                      /* XR120 */
  if( parse(d,RULE_ENTITYVALUE,' ')
      || parse(d,RULE_EXTERNALID,' ')) {
      APPEND(' ');                  /* XR120 */
      return 1;
  }
  return 0;
}

/*
 *===========================================================================
 */
/*  7.57  */
int RuleExternalID(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "ExternalID"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  int _retv = 0;

/*  Code  */
  PUSH;
  if(EXPECTED(TOK_SYSTEM)
     && NeedS(d)
     && parse(d,RULE_SYSTEMLITERAL,' ') ) {
      COPY_TO(SYSID);
      STR_CLEAR(PUBID);
      _retv++;
  }
  else if(EXPECTED(TOK_PUBLIC)
          && NeedS(d)
          && parse(d,RULE_PUBIDLITERAL,' ') && COPY_TO(PUBID)
          && NeedS(d)
          && parse(d,RULE_SYSTEMLITERAL,' ') ) {
      COPY_TO(SYSID);
      _retv++;
  }
  POP;
  return _retv;
}

/*
 *===========================================================================
 */
/*  7.58  */
int RuleNDataDecl(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "NDataDecl"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  int _retv = 0;

/*  Code  */
  if(NeedS(d) && EXPECTED(TOK_NDATA)) {
      PUSH;
      if( NeedS(d) && parse(d,RULE_NAME,' ') ) {
          d->m_unParsedFlag=1;
          if(!LOOKUP_NOTATION(d,CUR_VALUE))     /* XR091 */
              callError(d,XML_ERROR_NOTATION_DECALRED,CUR_VALUE);
          strCpy(d->m_notation,CUR_VALUE);
          _retv++;
      }
      POP;
  }
  return _retv;
}

/*
 *===========================================================================
 */
/*  7.59  */
int RuleTextDecl(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "TextDecl"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Code  */
  if(EXPECTED(TOK_XML_START)
     && parse(d,RULE_VERSIONINFO,'?')
     && parse(d,RULE_ENCODINGDECL,' ')
     && SkipS(d)
     && EXPECTED(TOK_XML_PI_END) ) {
      return 1;
  }
  return 0;
}

/*
 *===========================================================================
 */
/*  7.60  */
int RuleExtParsedEnt(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "ExtParsedEnt"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Code  */
  return parse(d,RULE_TEXTDECL,'?') && parse(d,RULE_CONTENT,' ');
}

/*
 *===========================================================================
 */
/*  7.61  */
int RuleExtPE(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "extPE"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Code  */
  return parse(d,RULE_TEXTDECL,'?') && parse(d,RULE_EXTSUBSETDECL,' ');
}

/*
 *===========================================================================
 */
/*  7.62  */
int RuleEncodingDecl(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "EncodingDecl"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  WCHAR _delim;

/*  Code  */
  if(NeedS(d)
     && EXPECTED(TOK_ENCODING)
     && parse(d,RULE_EQ,' ')
     && GOOD ) {
      if(IS('\"') || IS('\'')) {
          _delim=CH;
          INCR;
          if(parse(d,RULE_ENCNAME,' ')
             && IS(_delim) ) {
              INCR;
              return 1;
          }
      }
  }
  return 0;
}

/*
 *===========================================================================
 */
/*  7.63  */
int RuleEncName(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "EncName"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Code  */
  if( GOOD && LETTER ) {
      while(INC && (LETTER || DIGIT || IS('_') || IS('.') || IS('-')))
          ;
      return 1;
  }
  return 0;
}

/*
 *===========================================================================
 */
/*  7.64  */
int RuleNotationDecl(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "NotationDecl"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  SNotation *_notation;

/*  Code  */
  PUSH;
  if(EXPECTED(TOK_NOTATION_START)) {
      STR_CLEAR(PUBID);
      STR_CLEAR(SYSID);
      if(NeedS(d)
         && parse(d,RULE_NAME,' ') && COPY_TO(TMP)
         && NeedS(d)
         && (parse(d,RULE_EXTERNALID,' ') || parse(d,RULE_PUBLICID,' '))
         && SkipS(d)
         && IS('>')) {
          INCR;
          if(LOOKUP_NOTATION(d,TMP))
              callWarning(d,XML_WARNING_MORE_NOTA_DECL,TMP);
          else {
              _notation = NEW_NOTATION(d,TMP);
              _notation->m_extId.m_pubId = XTRDUP(PUBID);
              _notation->m_extId.m_sysId = XTRDUP(SYSID);
          }
          POP;
          return 1;
      }
  }
  POP;
  return 0;
}

/*
 *===========================================================================
 */
/*  7.65  */
int RulePublicID(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Production of the nonterminal "publicID"
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Code  */
  PUSH;
  if(EXPECTED(TOK_PUBLIC)
     && NeedS(d)
     && parse(d,RULE_PUBIDLITERAL,' ') ) {
      COPY_TO(PUBID);
      STR_CLEAR(SYSID);
      POP;
      return 1;
  }
  POP;
  return 0;
}

/*
 *===========================================================================
 */
/*  7.66  */
static void PEReplInEntityVal(PARDATA  d, PString  name, SPEntity *pe)
{
/*  Functional Description
 *
 *  Return Values:  -
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  WCHAR *_data;
/*  Code  */

  if(pe->m_type==ET_EXTERNAL_PARSED) {
      callError(d,XML_ERROR_PROPER_GROUP,name);      /* TODO: message! */
  }
  else if(pe->u.m_value
          && (STR_LEN(pe->u.m_value)>2)) {
      _data = STR_DATA(pe->u.m_value);
      /* Strip the leading/trailing spaces ! */
      /* XR120 */
      pushBuffer(d,_data+1,_data+STR_LEN(pe->u.m_value)-1);
      if(GOOD && IS('%'))                   /* followed by another PE */
          PERepl(d);
  }
}

/*
 *===========================================================================
 */
/*  7.67  */
void ChkPEInContentModel(PARDATA d, PString  name, PString  value)
{
/*  Functional Description
 *
 *  Return Values:  -
 *---------------------------------------------------------------------------
 */
/* Data Structures */
  WCHAR    *_data;
  int       _i, _left, _right;
  PString _tmp = XTRDUP(value);

/* Code */
  if(!strStrip(_tmp))                           /* Empty */
      callError(d,XML_ERROR_PROPER_GROUP,name);        /* XR046 */
  else {
      _data = STR_DATA(_tmp);
      if(   _data[0]=='|'
            || _data[0]==','
            || _data[STR_LEN(_tmp)-1]=='|'
            || _data[STR_LEN(_tmp)-1]==',' )     /* XR047 */
          callError(d,XML_ERROR_PROPER_GROUP,name);
      else {
          _left=_right=0;
          for(_i=0; _i<STR_LEN(_tmp); _i++) {
              if(_data[_i]=='(')
                  _left++;
              else if(_data[_i]==')')
                  _right++;
          }
          if(_left!=_right)                     /* XR045 */
              callError(d,XML_ERROR_PROPER_GROUP,name);
      }
  }
  STRDEL(_tmp);
}

/*
 *===========================================================================
 */
/*  7.68  */
int PERepl(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  This function is called when a '%' sign was encountered reading
 *  the entities. ( macros: INC, INCR )
 *
 *  - If we are outside the DTD, it just simple returns with Success, because
 *    the '%' sign has special meaning only in DTD.
 *
 *  Return Values:  Failure - fatal error /deepness/ was encountered
 *                  Success - otherwise
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  WCHAR    *_data;
  PString   _value, _name;
  SPEntity *_pe;

/*  Code  */
  if(isParent(d, RULE_COMMENT))                 /* Not recognized   */
      return 1;

  if(!isParent(d, RULE_DOCTYPEDECL))            /* Not recognized   */
     return 1;

  if(isParent(d, RULE_ATTVALUE))                /* Not recognized   */
     return 1;


  if(++d->m_deepnessOfPERepl>NRECURSION) {      /* XR088 */
      callError(d,XML_ERROR_RECURSION_IN_PE,0);
      --d->m_deepnessOfPERepl;
      return 0;
  }

  PUSH;
  if( !parse(d,RULE_PEREFERENCE,' ') ) {        /* read the name */
      POP;
      d->m_deepnessOfPERepl--;
      return 1;
  }
  _name = XTRDUP(CUR_VALUE);
  POP;

  if( !(_pe=LOOKUP_PE(d,_name)) ) {
      callError(d,XML_ERROR_PE_NOT_DECLARED,_name);
  }
  else if(isParent(d,RULE_ENTITYVALUE)) {       /* Included in literal */
      PEReplInEntityVal(d, _name, _pe);
  }
  else if(_pe->m_type==ET_EXTERNAL_PARSED) {    /* Included as PE   */
      parseExternal(d,RULE_EXTPE,&(_pe->u.m_extId));
  }
  else if( (_value=_pe->u.m_value) ) {
      /* enlarging by leading/trailing space is done
       * at RulePEDef()
       * XR120
       */
      if(isParent(d,RULE_CONTENTSPEC)) {
          ChkPEInContentModel(d,_name,_value);
      }

      _data = STR_DATA(_value);
      pushBuffer(d,_data,_data+STR_LEN(_value));
      if(GOOD && IS('%'))
          PERepl(d);
  }

  STRDEL(_name);

  d->m_deepnessOfPERepl--;
  return 1;
}

/*
 *===========================================================================
 */
/*  7.69  */
UINT8 XmlIsSpecial(PString s)
{
/*  Functional Description
 *
 *  This function checks if the string 's' was the name of a special
 *  kind of entity, and if match was found returns that character.
 *  This function is called from DeReference only.
 *
 *  Return Values:  The special character
 *                  or 0 on unmatched name
 *---------------------------------------------------------------------------
 */
/*  Code  */
  /* XR122 */
  if(STR_SAME(s,g_strAmp))
      return '&';
  else if(STR_SAME(s,g_strLt))
      return '<';
  else if(STR_SAME(s,g_strGt))
      return '>';
  else if(STR_SAME(s,g_strApos))
      return '\'';
  else if(STR_SAME(s,g_strQuot))
      return '\"';
  return 0;
}

/*
 *===========================================================================
 */
/*  7.70  */
static int DeRefChar(PARDATA d, ERefContext context)
{
/*  Functional Description
 *
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  PString   _value;

/* Code */
  PUSH;
  if( parse(d,RULE_CHARREF,' ') ) {     /* XR121 */
      _value = XTRDUP(CUR_VALUE);
      POP;
      strCat(CUR_VALUE,_value);
      STRDEL(_value);
      if( !((RC_IN_CONTENT|RC_IN_ATTR_VALUE|RC_IN_ENTITY_VALUE) & context ) )
          callError(d, XML_ERROR_CHAR_REF,0);
      return 1;
  }
  POP;
  return 0;
}

/*
 *===========================================================================
 */
/*  7.71  */
static void DeRefInContent(PARDATA d, PString name)
{
/*  Functional Description
 *
 *
 *  Return Values:  -
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  UINT8     _spec;
  PString   _value;
  SGEntity *_ge;

/* Code */

  if( (_spec=XmlIsSpecial(name)) ) {                /* XR122 */
      APPEND(_spec);                                /* XR084 */
  }
  else if( !(_ge=LOOKUP_GE(d,name)) ) {
      callError(d,XML_ERROR_GE_NOT_DECLARED,name);
  }
  else if(_ge->m_type==ET_INTERNAL){
      _value=_ge->u.m_value;
      pushBuffer(d,STR_DATA(_value),STR_DATA(_value)+STR_LEN(_value));
  }
  else if(_ge->m_type==ET_EXTERNAL_PARSED) {
      /* Load external parsed entity for validating */
      parseExternal(d,RULE_EXTPARSEDENT,&(_ge->u.m_extId));
  }
  else  /* ET_EXTERNAL_UNPARSED */
      callError(d,XML_ERROR_UNPARSED_IN_CONTENT,name);        /* XR112 */
}


/*
 *===========================================================================
 */
/*  7.72  */
static void DeRefInAttrVal(PARDATA d, PString name)
{
/*  Functional Description
 *
 *
 *  Return Values:  -
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  UINT8     _spec;
  PString   _value;
  SGEntity *_ge;

/* Code */
  if( (_spec=XmlIsSpecial(name)) ) {                /* XR122 */
      APPEND(_spec);
  }
  else if( !(_ge=LOOKUP_GE(d,name)) ) {
      callError(d,XML_ERROR_GE_NOT_DECLARED,name);
  }
  else if(_ge->m_type==ET_INTERNAL){
      _value=_ge->u.m_value;
      pushBuffer(d,STR_DATA(_value),STR_DATA(_value)+STR_LEN(_value));
  }
  else  /* ET_EXTERNAL_UNPARSED */
      callError(d,XML_ERROR_EXT_IN_ATTVAL,name);             /* XR114 */
}

/*
 *===========================================================================
 */
/*  7.73  */
static void DeRefInEntityVal(PARDATA d, PString name)
{
/*  Functional Description
 *
 *
 *  Return Values:  -
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  UINT8     _spec;
  SGEntity *_ge;

/* Code */

  if( !(_ge=LOOKUP_GE(d,name)) && !(_spec=XmlIsSpecial(name)) ) {
      callError(d,XML_ERROR_GE_NOT_DECLARED,name);
  }
  else if(_ge && _ge->m_type==ET_EXTERNAL_UNPARSED) {
      callError(d,XML_ERROR_UNPARSED_IN_EV,name);             /* XR112 */
  }
  else {    /* bypassed */                          /* XR117 */
      APPEND('&');
      strCat(CUR_VALUE,name);
      APPEND(';');
  }
}
/*
 *===========================================================================
 */
/*  7.74  */
void DeReference(PARDATA d, ERefContext context)
{
/*  Functional Description
 *
 *  This function processes the character and entity references,
 *  according the current context.
 *
 *  Append: the referenced values by calling the proper
 *          rules, or the reference as is, including & and ;
 *
 *  Return Values:  -
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  PString   _name;

/*  Code  */

  if(!DeRefChar(d,context)) {
      PUSH;
      if(!parse(d,RULE_ENTITYREF,' ')) {
          POP;
          callError(d,XML_ERROR_NOT_REFERENCE,0);
          APPEND(CH);       /* No luck, we have to eat '&' */
          INCR;             /* to avoid infinite loop      */
          return;
      }

      _name = XTRDUP(CUR_VALUE);
      POP;
      switch(context) {
         case RC_IN_CONTENT      : DeRefInContent(d,_name);
                                   break;
         case RC_IN_ATTR_VALUE   : DeRefInAttrVal(d,_name);
                                   break;
         case RC_IN_ENTITY_VALUE : DeRefInEntityVal(d,_name);
                                   break;
         default                 : DBG_ASSERT(0);
                                   break;
      }
      STRDEL(_name);
  }
}

/*
 *===========================================================================
 */
/*  7.75  */
int FuncExpected(PARDATA d, const char *s)
{
/*  Functional Description
 *
 *  This function scans the input and compare it with string 's'.
 *  Note: The INC, INCR macros are not used but real pointers.
 *        That means this function can recognise strings /lex tokens/
 *        only if they occured entirely in the current entity buffer.
 *        This is not a conflict with XML spec, because this is a
 *        constrain.
 *
 *  Return Values:  1 - match
 *                  0 - does not match
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  const WCHAR *_p = GPTR;
  const WCHAR *_e = EPTR;

/*  Code  */
  if(*s) {
      while(_p<_e && *s && *_p==*s)
          _p++, s++;
      if(!*s) {
          GPTR = --_p;  /* that is safe */
          INCR;
          return 1;
      }
  }
  return 0;
}

/*
 *===========================================================================
 */
/*  7.76  */
int NextXmlToken(PARDATA d,         /* In/Out:  pointer to parser data  */
                 const int *tokens, /* In: 0 terminated array of tokens */
                 int    lookahead)  /* In: lookahead flag               */
{
/*  Functional Description
 *
 *  This function is roughly the replacement of lexer.
 *  Scans the input and try to match with a string that belongs to
 *  one of the token among 'tokens'.
 *
 *  If 'lookahead' is TRUE (other then zero), leaves GPTR as was.
 *  If 'lookahead' is FALSE ( zero) moves GPTR to next unreaded position.
 *
 *  The lex tokens are listed in file: "xmllex.h".
 *
 *
 *  Return Values:  the matched lex token
 *                  0 : no match
 *---------------------------------------------------------------------------
 */

#ifdef WML_DEBUG
  /*
   * The order of the tokens important. At first we have to look
   * for the longer names, and later for the shorter.
   */
  int i;
  for(i=1; tokens[i]; i++) {
      assert(tokens[i]>tokens[i-1]);
  }
#endif

/*  Code  */
  if(GOOD) {
      const WCHAR *_e = EPTR;
      for(; *tokens; ++tokens) {
          const WCHAR      *_p = GPTR;
          const char *_s = STR(*tokens);
          while(_p<_e && *_s && *_p==*_s)
              _p++, _s++;

          if(!*_s) {
              if(!lookahead) {
                  GPTR = _p;
                  if(_p>=_e)        /* as GOOD */
                      popBuffer(d);
              }
              return *tokens;
          }
      }
  }
  return 0;
}

/*
 *===========================================================================
 */
/*  7.77  */
int NeedS(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Shortcut to: parse(d,RULE_S,' ')
 *
 *  Return Values:  Success/Failure
 *---------------------------------------------------------------------------
 */
/*  Code  */
  return parse(d,RULE_S,' ');
}

/*
 *===========================================================================
 */
/*  7.78  */
int SkipS(PARDATA d)          /* In/Out:  pointer to parser data */
{
/*  Functional Description
 *
 *  Skips white spaces if any, and returns with Success.
 *
 *  Return Values:  Success
 *---------------------------------------------------------------------------
 */
/*  Code  */
  if(GOOD && IS_SPACE(CH))
      parse(d,RULE_S,' ');
  return 1;
}

/*
 * Some notes for me:
 *
 * - callError()/callWarning() must not change the parser sequence
 *   to avoid endless loop, though it can stop the parser clearing
 *   the buffer pointers.
 *   Return value of rule functions depends only on syntact. No VC and WFC
 *   consideration can modify the success of rule functions. However
 *   a syntact error can cause a WFC error.
 * - Don't forget to check local variables if error was occured
 * - Report error/warning as soon as detected. However hard
 *   to make a decision to give up parsing at low level
 *   rule functions.
 * - Use CH macro only if GOOD. After an error detected, pointer
 *   points to a 1 char 'fatal buffer', so safe to read the last
 *   character even after failure.
 * - Check PUSH/POP pairs
 * - TMP can be used ONLY in rules (not in eg. Repl)
 * - CUR_VALUE is valid till next POP
 * - Distinguish rule functions for declarations/definitions/both
 *   What about variables used by both of them (eg.ELEMENT)
 * - No validation take place here but via Validation functions.
 * - Recognition context
 *        - PERef is recognized via the pointer, with other words:
 *          PERef is recognized immediately as the pointer gets the '%' char,
 *          due to the context: 'Reference in DTD' (ie. anywhere in DTD)
 *        - GERef & CharRef is recognized by calling the Rule ??
 * - Recursion: checking recursion too much work. Instead of checking,
 *              the pushBuffer sends error message when stack overflow
 *              occured.
 * - Would be advisable to make faster functions to eat white spaces.
 * - ExpectedFunc uses real pointers because the speed. The price the
 *   speed: it can't walk through boundaries of entities. I suppose
 *   this is not an error. (XML tokens must be entirely in the same entity)
 */
/*
=============================================================================
*/


/*  End of File  */

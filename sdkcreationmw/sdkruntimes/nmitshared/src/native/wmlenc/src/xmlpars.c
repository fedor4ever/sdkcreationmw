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
/* File    : U:\wpsvob\wmlenc\src\common\xmlpars.c
   Version : \main\10
   Date    : 17-Oct-00.12:49:13
*/

/*
 * ========================================================================
 *  Name        : xmlpars.c
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

        Core of the XML parser.


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

        7.1   initXML
        7.2   termXML
        7.3   parseXML
        7.4   newParser
        7.5   deleteParser
        7.6   parse
        7.7   parseExternal
        7.8   callError
        7.9   callWarning
        7.10  callToken
        7.11  pushBuffer
        7.12  popBuffer
        7.13  getDTDID
        7.14  isParent
        7.15  inRootDocument
        7.16  printBuffer

*/



/*  3  GLOSSARY

        None

*/


/*  4  EXTERNAL RESOURCES  */

    /* None */

/*  4.1  Include Files  */

#include "xmlpars.h"
#include "xmlctyp.h"
#include "xmlstr.h"
#include "xmlmsg.h"
#include "xmlrules.h"
#include "xmlconst.h"
#include "xmlbuf.h"
#include "chenc.h"

/*  4.2 External Data Structures  */

    /* None */


/*  4.3 External Function Prototypes  */

    /* None */


/*  5  LOCAL CONSTANTS AND MACROS  */

#define TOP     d->m_bufStackSize
#define BSTACK  d->m_bufStack
#define CURBUF  d->m_eb
#define MAX_WML_STRLEN  50000U



/*  6  MODULE DATA STRUCTURES  */


/*  6.1 Local Data Structures  */

WCHAR   g_fatalBuffer[1];


/*  6.2 Local Function Prototypes  */

#ifdef WML_DEBUG
static char * printBuffer(PARDATA d);
#endif


static PARDATA newParser
                    (void           * usrData,
                     FXMLCallBack     tokenCB,
                     FXMLErrorCB      errorCB,
                     const char     * altDTD,
                     const BYTE     * urlOfDoc,
                     const WCHAR    * buffer,
                     UINT32           bufferSize);

static void deleteParser
                    (PARDATA d);


/*  7 MODULE CODE  */


/*
 *===========================================================================
 */
/*  7.1  */
int   initXML(void)
{
/*  Functional Description
 *
 *  This function must be called once, before threads, and before
 *  any call to XML Parser.
 *
 *  Return Value:  success
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/*  Code  */

  initXCType();
  initRuleFunctions();

  return 1;
}

/*
 *===========================================================================
 */
/*  7.2  */
void  termXML(void)
{
/*  Functional Description
 *
 *  Call this only once.
 *
 *  Return Value:  None
 *---------------------------------------------------------------------------
 */

  /* Code */
}

/*
 *===========================================================================
 */
/*  7.3  */
int  parseXML(void *           usrData,
               UINT16 *        buffer,
               UINT32          bufferSize,
               FXMLCallBack    tokenCB,
               FXMLErrorCB     errorCB,
               const char   *  altDTD,
               const BYTE   *  urlOfDoc)
{
/*  Functional Description
 *
 *  
 *
 *  Return Value:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */
  int     _retv;
  PARDATA _d = newParser(usrData,
                         tokenCB,
                         errorCB,
                         altDTD,
                         urlOfDoc,
                         buffer,
                         bufferSize);

  /* Code */

  _retv = parse(_d,RULE_DOCUMENT,' ') && (!_d->m_error);

  callToken(_d, XTOK_PUBID_OF_DTD, _d->m_dtdId.m_pubId);

  deleteParser(_d);

  return _retv;
}

/*
 *===========================================================================
 */
/*  7.4  */
static PARDATA newParser(void           * usrData,
                         FXMLCallBack     tokenCB,
                         FXMLErrorCB      errorCB,
                         const char     * altDTD,
                         const BYTE     * urlOfDoc,
                         const WCHAR    * buffer,
                         UINT32           bufferSize)
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
  struct SParserData *_retv;


/*  Code  */

  if( (_retv = (struct SParserData*)XML_ALLOC(sizeof(struct SParserData))) ) {

      /* Clear everything */
      memset(_retv, 0, sizeof(struct SParserData));

#ifdef WML_DEBUG
      _retv->m_appStack   = (char*)&i;
      _retv->m_st         = 0;
      _retv->m_spaces     = XML_ALLOC(256);
      _retv->m_dbgbuf     = XML_ALLOC(1024);
      memset(_retv->m_spaces,' ',256);
      _retv->m_spaces[255]= 0;
#endif

      /* Common Data */
      constrCommonData(&_retv->m_commonData);


      /* Stack of entity buffers */
      _retv->m_bufStack = (SEntityBuffer*)XML_ALLOC(64*sizeof(SEntityBuffer));
      memset(_retv->m_bufStack,0,64*sizeof(SEntityBuffer));
      _retv->m_bufStackSize = 64;

      /* Stack of CUR_VALUE and TMP strings */
      _retv->m_valSP    = _retv->m_valStack;
      _retv->m_tmpSP    = _retv->m_tmpStack;
      _retv->m_ruleSP   = 0;
      for(i=0; i<STRING_STACK_SIZE; i++) {
          _retv->m_valStack[i] = newString(STR_POOL(_retv));
          _retv->m_tmpStack[i] = newString(STR_POOL(_retv));
      }
      
      /* track the stack */
      _retv->m_popFrom = 0;
      _retv->m_popPtr  = 0;
      

      _retv->m_errorCB  = errorCB;
      _retv->m_tokenCB  = tokenCB;
      _retv->m_usrData  = usrData; 

      _retv->m_chardata = newString(STR_POOL(_retv));
      _retv->m_nUsedAttr= 0;
      _retv->m_error   = 0;
      
      _retv->m_nameOfRootElement = newString(STR_POOL(_retv));
      _retv->m_elementNestingLevel = 0;
      _retv->m_cParserOfRoot     = 0;
      _retv->m_deepnessOfPERepl  = 0;
      _retv->m_standalone        = 0;
      _retv->m_attribute         = newAttr(&_retv->m_commonData);
      _retv->m_element           = 0;
      _retv->m_extId.m_pubId     = newString(STR_POOL(_retv));
      _retv->m_extId.m_sysId     = newString(STR_POOL(_retv));
      _retv->m_dtdId.m_pubId     = newString(STR_POOL(_retv));
      _retv->m_dtdId.m_sysId     = newString(STR_POOL(_retv));
      _retv->m_pubIdAltDTD       = newString(STR_POOL(_retv));
      _retv->m_notation          = newString(STR_POOL(_retv));
      _retv->m_idOnlyFlag        = 0;
      _retv->m_contentParser     = 0;

      _retv->m_bufferIdList.m_root = 0;

      _retv->m_location          = urlOfDoc;

      if(altDTD) {
          while(*altDTD) {
              strAppendChar(_retv->m_pubIdAltDTD, *altDTD++);
          }
      }

      /* Push root document */
      _retv->m_rootBuffer    = buffer;
      _retv->m_rootBufferEnd = buffer + bufferSize;
      pushBuffer( _retv, buffer, buffer + bufferSize);


  }

  return _retv;
}

/*
 *===========================================================================
 */
/*  7.5  */
static void deleteParser(PARDATA d)
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
  SIdNode *_tb;
  
/*  Code  */

  /* Release text buffers */
  while(SLL_GET(d->m_bufferIdList,_tb)) {
      releaseBuffer(_tb->m_id);
      if(_tb->m_buffer)
          XML_FREE(_tb->m_buffer);
      XML_FREE(_tb);
  }

  deleteString(d->m_nameOfRootElement,STR_POOL(d));
  deleteString(d->m_chardata,STR_POOL(d));
  deleteAttr(d->m_attribute,&d->m_commonData);
  deleteString(d->m_extId.m_pubId,STR_POOL(d));
  deleteString(d->m_extId.m_sysId,STR_POOL(d));
  deleteString(d->m_dtdId.m_pubId,STR_POOL(d));
  deleteString(d->m_dtdId.m_sysId,STR_POOL(d));
  deleteString(d->m_notation,STR_POOL(d));
  deleteString(d->m_pubIdAltDTD,STR_POOL(d));


  /* Stack of entity buffers */
  XML_FREE(d->m_bufStack);

  /* Stack of CUR_VALUE and TMP strings */
  for(i=0; i<STRING_STACK_SIZE; i++) {
      deleteString(d->m_valStack[i],STR_POOL(d));
      deleteString(d->m_tmpStack[i],STR_POOL(d));
  }

  /* Common Data */
  destrCommonData(&d->m_commonData);

#ifdef WML_DEBUG
  DBG_LOG( (DBG_MEM,"Stack usage: %i",(char*)(&i)-d->m_appStack) );
  XML_FREE(d->m_spaces);
  XML_FREE(d->m_dbgbuf);
#endif
  XML_FREE(d);
}

/*
 *===========================================================================
 */
/*  7.6  */
int  parse(PARDATA d, int id, int option)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  SEntityBuffer  _localEB;
  int            _retv  = 0;
  
/*  Code  */
  if(d->m_tmpSP>= &(d->m_tmpStack[STRING_STACK_SIZE-2])) {
      callError(d,XML_ERROR_NO_RECURSION,0);
      return 0;
  }

  d->m_ruleStack[++d->m_ruleSP] = id;

  memcpy(&_localEB,&(d->m_eb),sizeof(SEntityBuffer));
  STR_CLEAR(*++d->m_tmpSP);


#ifdef WML_DEBUG
      d->m_st+=2;
      if((char*)(&_retv)<d->m_appStack)
          d->m_appStack = (char*)(&_retv);

      d->m_spaces[d->m_st]=0;
      DBG_LOG( (DBG_PAR, "%s%s %s",
                d->m_spaces,
                g_nameOfRule[id],
                printBuffer(d)) );
      d->m_spaces[d->m_st]=' ';
#endif

  switch(option) {
     case ' ' :  _retv = (*g_rule[id])(d);      /* one          */
                 break;
     case '+' :  if( (*g_rule[id])(d) ) {       /* one or more  */
                     while( parse(d,id,' ') )
                         ;
                     _retv = 1;
                 }
                 break;
     case '*' :  if( !(*g_rule[id])(d) )         /* zero or more */
                     memcpy(&(d->m_eb),&_localEB,sizeof(SEntityBuffer));
                 else
                     while( parse(d,id,' ') )
                         ;
                 _retv = 1;
                 break;
     case '?' :  if(!(*g_rule[id])(d))          /* zero or one  */
                     memcpy(&(d->m_eb),&_localEB,sizeof(SEntityBuffer));
                 _retv = 1;
                 break;
     default  :  _retv = 0;
                 DBG_ASSERT(0);
                 break;
  }

#ifdef WML_DEBUG
      d->m_spaces[d->m_st]=0;
      if(_retv) {
          DBG_LOG( (DBG_PAR, "%s+ %s",d->m_spaces,g_nameOfRule[id]) );
      }
      else {
          DBG_LOG( (DBG_PAR, "%s- %s",d->m_spaces,g_nameOfRule[id]) );
      }
      d->m_spaces[d->m_st]=' ';
#endif

  if( !_retv ) {
      if(d->m_eb.m_stackPos>=(_localEB.m_stackPos-1))
          memcpy(&(d->m_eb),&_localEB,sizeof(SEntityBuffer));
      else {
           /*fatal error: can't restore stack !  */
          /* wmlLog( WSL_LOG_ERROR, "Nesting error 1" ); */
          callError(d,XML_ERROR_NESTING,0);
      }
  }
  else {
      if(   id==RULE_ELEMENT
            || id==RULE_COMMENT
            || id==RULE_PI
            || id==RULE_ENTITYREF) {
          /* XML Spec: pg.26
           * "No start-tag,end-tag,empty-element tag, element,
           *  comment, PI, CharRef, EntityRef can begin in one entity
           *  and end in another."
           */
          /* XR099 */
          if(d->m_eb.m_stackPos<_localEB.m_stackPos) {
              /* rule leaved it's entity buffer.
               * If this occured at the end of the buffer,
               * and no reading happened, still ok.
               */
              if(d->m_popFrom>_localEB.m_stackPos
                 || (d->m_eb.m_gptr!=d->m_popPtr) ) {
                  /* wmlLog( WSL_LOG_ERROR, "Nesting error 2"); */
                  callError(d,XML_ERROR_NESTING,0);
              }
          }
          else if(d->m_eb.m_stackPos>_localEB.m_stackPos) {
              /* rule finished in nested entity buffer */
              /* wmlLog( WSL_LOG_ERROR, "Nesting error 3"); */
              callError(d,XML_ERROR_NESTING,0);
          }
      }
  }
#ifdef WML_DEBUG
  d->m_st-=2;
#endif

  d->m_ruleSP--;
  d->m_tmpSP--;
  return _retv;
}

/*
 *===========================================================================
 */
/*  7.7  */
int  parseExternal(PARDATA d, int ruleId, SExtId *extId)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  int    _retv  = 0;
  int    _id;
  const WCHAR *_buffer;
  WCHAR         *_dlBuffer;
  UINT32    _bufferSize;
  SIdNode  *_idNode;
  WML_STATUS _dlStatus;
  

/*  Code  */

  if(!extId) {
      return 0;
  }

  if(d->m_tmpSP>= &(d->m_tmpStack[STRING_STACK_SIZE-2])) {
      callError(d,XML_ERROR_NO_RECURSION,0);
      return 0;
  }

  d->m_ruleStack[++d->m_ruleSP] = ruleId;
  STR_CLEAR(*++d->m_tmpSP);

#ifdef WML_DEBUG
      d->m_st+=2;
      if((char*)(&_retv)<d->m_appStack)
          d->m_appStack = (char*)(&_retv);

      d->m_spaces[d->m_st]=0;
      DBG_LOG( (DBG_PAR, "X%s%s %s",
                d->m_spaces,
                g_nameOfRule[ruleId],
                printBuffer(d)) );
      d->m_spaces[d->m_st]=' ';
#endif

  if( (_id=getBuffer(extId->m_pubId,&_buffer,&_bufferSize)) ) {

      /* found in cache */
      _idNode = XML_ALLOC(sizeof(SIdNode));
      _idNode->m_id = _id;
      _idNode->m_buffer = 0;
      SLL_INSERT(d->m_bufferIdList, _idNode);

      pushBuffer(d, _buffer, _buffer+_bufferSize);
      _retv = (*g_rule[ruleId])(d);
  }
  else if( (_dlStatus=downLoadExternal(extId,
                                       d->m_location,
                                       &_dlBuffer,
                                       &_bufferSize)) == WML_OK ) {

      /* downloaded successfuly */
      _idNode = XML_ALLOC(sizeof(SIdNode));
      _idNode->m_id     = _id;
      _idNode->m_buffer = _dlBuffer;
      SLL_INSERT(d->m_bufferIdList, _idNode);

      pushBuffer(d, _dlBuffer, _dlBuffer+_bufferSize);
      _retv = (*g_rule[ruleId])(d);

      addBufferToCache (extId->m_pubId, _dlBuffer, _bufferSize);
  }
  else if(    (ruleId==RULE_EXTSUBSET)
           && (_id=getBuffer(d->m_pubIdAltDTD,&_buffer,&_bufferSize)) ) {
      /* alternative DTD found in cache */
      strCpy(d->m_dtdId.m_pubId, d->m_pubIdAltDTD);     /* FIX DTD for WML */
      _idNode = XML_ALLOC(sizeof(SIdNode));
      _idNode->m_id = _id;
      _idNode->m_buffer = 0;
      SLL_INSERT(d->m_bufferIdList, _idNode);

      pushBuffer(d, _buffer, _buffer+_bufferSize);
      _retv = (*g_rule[ruleId])(d);
  }
  else {
      callError(d,_dlStatus,0);
      _retv = 0;
  }


#ifdef WML_DEBUG
      d->m_spaces[d->m_st]=0;
      if(_retv) {
          DBG_LOG( (DBG_PAR,"%s+ %s",d->m_spaces,g_nameOfRule[ruleId]) );
      }
      else {
          DBG_LOG( (DBG_PAR, "%s- %s",d->m_spaces,g_nameOfRule[ruleId]) );
      }
      d->m_spaces[d->m_st]=' ';
  d->m_st-=2;
#endif

  d->m_ruleSP--;
  d->m_tmpSP--;
  return _retv;
}
/*
 *===========================================================================
 */
/*  7.8  */
void  callError(PARDATA d,XML_STATUS msg, PString context)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  PString _tmp = 0;
  int n = 20;

/*  Code  */
  if(context && STR_LEN(context)) {
      _tmp = context;
  }
  else {
      _tmp = newString(STR_POOL(d));
      while (--n && (d->m_eb.m_gptr<d->m_eb.m_eptr)) {
          strAppendChar(_tmp,*(d->m_eb.m_gptr));
          d->m_eb.m_gptr++;
      }
  }

  d->m_eb.m_gptr     = g_fatalBuffer;
  d->m_eb.m_eptr     = g_fatalBuffer;
  d->m_eb.m_stackPos = 0;

  if(d->m_errorCB) {
     d->m_errorCB( msg,_tmp,d->m_usrData);
  }

  if(_tmp != context)
      deleteString(_tmp,STR_POOL(d));
}

/*
 *===========================================================================
 */
/*  7.9  */
void  callWarning(PARDATA d,XML_STATUS msg, PString context)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/*  Code  */

  if(context) {
      DBG_LOG2( ( DBG_XML, context, "XML warning: %s at ", (char*)msg) );
  }
  else {
      DBG_LOG( ( DBG_XML, "XML warning: %s ", (char*)msg) );
  }
}

/*
 *===========================================================================
 */
/*  7.10  */
void  callToken(PARDATA d, int token, PString value)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */


/*  Code  */

  if(d->m_tokenCB) {
      if(token==XTOK_PARSED_CHARDATA) {
          strCat(d->m_chardata, value);
          return;
      }

      if(STR_LEN(d->m_chardata)) {

          if(STR_LEN(d->m_chardata)<MAX_WML_STRLEN)
              d->m_tokenCB(XTOK_PARSED_CHARDATA,
                           d->m_chardata, d->m_usrData);
          else
              callError(d,XML_ERROR_STR_TOO_LONG,0);

          STR_CLEAR(d->m_chardata);
      }

      if(STR_LEN(value)<MAX_WML_STRLEN)
          d->m_tokenCB(token,value,d->m_usrData);
      else
          callError(d,XML_ERROR_STR_TOO_LONG,0);
  }
}

/*
 *===========================================================================
 */
/*  7.11  */
void pushBuffer(PARDATA d, const WCHAR *gptr, const WCHAR *eptr)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  int _sp = d->m_eb.m_stackPos;

/*  Code  */

  DBG_LOG( (DBG_PP, "PUSHa: %i %s",_sp, printBuffer(d)) );

  assert(_sp>=0 && _sp<TOP);

  BSTACK[_sp].m_gptr = CURBUF.m_gptr;    /* Save cur pointer */
  _sp++;

  if(_sp>=TOP) {
      /* well, I can resize the stack, but too much nested entity
       * possible means recursion / recursion not allowed: rule 68 /
       */
      /* XR086 */
      callError(d,XML_ERROR_NO_RECURSION,0);
  }

  BSTACK[_sp].m_gptr     = CURBUF.m_gptr     = gptr;
  BSTACK[_sp].m_eptr     = CURBUF.m_eptr     = eptr;
  BSTACK[_sp].m_stackPos = CURBUF.m_stackPos = _sp;
#ifdef WML_DEBUG
        DBG_LOG( (DBG_PP, "PUSHb: %i %s", _sp, printBuffer(d) ) );
#endif
}

/*
 *===========================================================================
 */
/*  7.12  */
int popBuffer(PARDATA d)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  int _sp = d->m_eb.m_stackPos;

/*  Code  */

  assert(_sp>=0 && _sp<TOP);
#ifdef WML_DEBUG
      DBG_LOG( (DBG_PP, "POPa: %i %s",_sp, printBuffer(d)) );
      /*DBG_LOG( ("POP1b: %i ",BSTACK[_sp].m_stackPos) ); */
      if(_sp>0)
          memset(&(BSTACK[_sp]),0xff,(TOP-_sp)*sizeof(SEntityBuffer));
#endif

  if(_sp<=0) {
      d->m_popFrom = 0;
      d->m_popPtr  = CURBUF.m_gptr;
#ifdef WML_DEBUG
      DBG_LOG( (DBG_PP,"POPb: no more") );
      /*DBG_LOG( ("POP2b: %i  ",BSTACK[_sp].m_stackPos) ); */
#endif
      return 0;
  }

  _sp--;

  DBG_ASSERT(BSTACK[_sp].m_stackPos == _sp);
  
  CURBUF.m_gptr     = BSTACK[_sp].m_gptr;
  CURBUF.m_eptr     = BSTACK[_sp].m_eptr;
  CURBUF.m_stackPos = _sp;

  DBG_LOG( (DBG_PP,"POPb: %i %s",_sp, printBuffer(d)) );

  if(CURBUF.m_gptr<CURBUF.m_eptr) {
      d->m_popFrom = _sp+1;
      d->m_popPtr  = CURBUF.m_gptr;
      return 1;
  }

  if(_sp>0)
      popBuffer(d);
  d->m_popFrom = _sp+1;
  d->m_popPtr  = CURBUF.m_gptr;
  return (CURBUF.m_stackPos>0);
}

/*
 *===========================================================================
 */
/*  7.13  */
int  getDTDID(void *     usrData,
               UINT16  * buffer,
               UINT32    bufferSize,
               UINT16    idBufLenU16,
               UINT16  * publicID,
               UINT16  * systemID)
{
/*  Functional Description
 *
 *  
 *
 *  Return Value:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  PARDATA   _d;
  int       _retv;
  UINT16    _i;

/* Code */
      
  _d = newParser(usrData,0,0,0,0,buffer,bufferSize);

  _d->m_idOnlyFlag = 1;

  if(parse(_d,RULE_PROLOG,' ')
     && (!_d->m_error)
     && (idBufLenU16>=(STR_LEN(_d->m_dtdId.m_pubId)+1))
     && (idBufLenU16>=(STR_LEN(_d->m_dtdId.m_sysId)+1))) {

      for(_i=0; _i<STR_LEN(_d->m_dtdId.m_pubId); _i++) {
          publicID[_i] = STR_DATA(_d->m_dtdId.m_pubId)[_i];
      }
      publicID[_i] = 0;

      for(_i=0; _i<STR_LEN(_d->m_dtdId.m_sysId); _i++) {
          systemID[_i] = STR_DATA(_d->m_dtdId.m_sysId)[_i];
      }
      systemID[_i] = 0;

      _retv = 1;
  }
  else {
      _retv = 0;
  }

  deleteParser(_d);

  return _retv;
}

/*
 *===========================================================================
 */
/*  7.14  */
int         isParent
                    (PARDATA        d,
                     int            ruleId)
{
  int idx = d->m_ruleSP;

  if (ruleId==RULE_COMMENT) {
      DBG_LOG( (~0,"Parents") );
      while(idx) {
          DBG_LOG( (~0," - %s",g_nameOfRule[d->m_ruleStack[idx]]) );
          if( d->m_ruleStack[idx] == ruleId )
              return ruleId;
          idx--;
      }
      return 0;
  }

  while(idx) {
      if( d->m_ruleStack[idx] == ruleId )
          return ruleId;
      idx--;
  }
  return 0;
}

/*
 *===========================================================================
 */
/*  7.15  */
int  inRootDocument(PARDATA d)
{
  return (    (d->m_rootBuffer <= d->m_eb.m_gptr)
           && (d->m_rootBufferEnd > d->m_eb.m_gptr) );
}


#ifdef WML_DEBUG
/*
 *===========================================================================
 */
/*  7.16  */
static char * printBuffer(PARDATA d)
{
/*  Functional Description
 *
 *  ...
 *
 *  Return Values:  ...
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */
  const WCHAR *_p = d->m_eb.m_gptr;
  const WCHAR *_e = d->m_eb.m_gptr+24;
  char  *_cp= d->m_dbgbuf;

/* Code */

  sprintf(_cp, "(%i)", d->m_eb.m_eptr - d->m_eb.m_gptr);

  if(_e>d->m_eb.m_eptr)
      _e = d->m_eb.m_eptr;

  wPrint(_e-_p, _p, 64, _cp+strlen(_cp));

  return d->m_dbgbuf;
}

#endif  /* WML_DEBUG */
/*
=============================================================================
*/


/*  End of File  */

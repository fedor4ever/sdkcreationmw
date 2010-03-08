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
/* File    : U:\wpsvob\wmlenc\src\common\wmlhndl.c
   Version : \main\12
   Date    : 17-Oct-00.12:47:16
*/

/*
 * ========================================================================
 *  Name        : wmlhndl.c
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

        This module contains the handler functions of the WML encoder.


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

        7.1   processCharData
        7.2   handlerRootElement
        7.3   handlerDefaultElement
        7.4   handlerNumber
        7.5   handlerUnknownAttr
        7.6   handlerVdata
        7.7   handlerURL
        7.8   handlerCDATA
        7.9   handlerLength
        7.10  handlerMeta
        7.11  handlerIgnore
        7.12  handlerOnce
        7.13  handlerNotZero
        7.14  handlerOneTask
        7.15  handlerTask
        7.16  handlerEvents
        7.17  handlerURLEvent
        7.18  handlerOnEventType
        7.19  handlerCard
        7.20  handlerDo
        7.21  handlerDoType
        7.22  handlerDoName
        7.23  handlerVarName
        7.24  handlerMeta2
        7.25  handlerPre
        7.26  handlerCDATA2

*/



/*  3  GLOSSARY

        None

*/


/*  4  EXTERNAL RESOURCES  */

    /* None */

/*  4.1  Include Files  */

#include "wmlvalid.h"

/*  4.2 External Data Structures  */

    /* None */


/*  4.3 External Function Prototypes  */

    /* None */


/*  5  LOCAL CONSTANTS AND MACROS  */
#ifndef WS_HANDLING
#define WS_HANDLING     2
#endif
#ifndef VARS_IN_CDSECTION
#define VARS_IN_CDSECTION   0
#endif

#define EVENT        data->m_eventList.m_head
#define NEXT_EVENT   data->m_eventList.m_head->m_next
#define ANAME        data->m_eventList.m_head->m_tokenValue
#define AVALUE       data->m_eventList.m_head->m_next->m_tokenValue
#define REMOVE_EVENT  { SXmlEvent * _thisEvent;\
                        FIFO_GET(data->m_eventList,_thisEvent);\
                        deleteSXmlEvent(_thisEvent,data); }
#define ISDIGIT(c)   (((c)>=(WCHAR)'0') && ((c)<=(WCHAR)'9'))
#define CONTENT     (data->m_metaContent[data->m_mCounter])
#define HTTPEQUIV   (data->m_metaHttpEquiv[data->m_mCounter])


/*  6  MODULE DATA STRUCTURES  */


/*  6.1 Local Data Structures  */


/*  6.2 Local Function Prototypes  */


/*  7 MODULE CODE  */

/*
=============================================================================
 */
/*  7.1  */
static void processCharData(SDocData *data, int counter)
{
/* - Validate variables in 'PCDATA' but not in CDATA sections
 * - WS handling (1.2)
 *      "<p> text1 <b> bold </b> text2 </p>"
 *      "<p> text1<b> bold </b>text2 </p>"
 * - WS handling (1.3)
 *      "<p> text1 <b> bold </b> text2 </p>"
 *      "<p>text1 <b>bold</b> text2</p>"
 * - HTML:
 *     Thus, authors, and in particular authoring tools, should write:
 *         <P>We offer free <A>technical support</A> for subscribers.</P>
 *     and not:
 *         <P>We offer free<A> technical support </A>for subscribers.</P>
 *---------------------------------------------------------------------------
 */

  /* Data structures */
  PString _s = EVENT->m_tokenValue;
  Segment * _root;

  /* Code */

  if(_s && STR_LEN(_s)) {

      if ( !data->m_preFlag && EVENT->m_token!=XTOK_UNPARSED_CHARDATA ) {
          
          strOneSpace(_s);

          if ( (STR_LEN(_s)==1) && (*(STR_DATA(_s))==' ') ) {
              REMOVE_EVENT;
              return;
          }

          if (    (!counter && (WS_HANDLING==3))
               || (counter  && (WS_HANDLING==2)) ) {
              /* cut the leading space */
              strStripLeading(_s);
          }

          if (    ((WS_HANDLING==2) && NEXT_EVENT->m_token==XTOK_START_ELEMENT)
               || ((WS_HANDLING==3) && NEXT_EVENT->m_token==XTOK_END_ELEMENT)) {
              /* cut the trailing space */
              strStripTrailing(_s);
          }
      }

      _root = newSegment(data,EVENT->m_tokenValue,0,0);
      EVENT->m_tokenValue = 0;  /* swap with 0 string */

      /* Ignore variables in CDATA section */
      if( EVENT->m_token!=XTOK_UNPARSED_CHARDATA || VARS_IN_CDSECTION) {
          variables(data, _root, NOESCAPE_VAR);
      }

      flushSegments(data, _root);
      deleteSegment(data, _root);
  }

  REMOVE_EVENT;
}

/*
=============================================================================
 */
/*  7.2  */
void handlerRootElement(SDocData * data)
{
/* This handler is called for root element.
 *---------------------------------------------------------------------------
 */

  /* Data structures */
  const SWmlNode  *_node;

  /* Code */

  DBG_LOG( (DBG_WML, "\nRootElement: START\n") );
  _node=lookupElement(data);
  handlerDefaultElement(data,_node);
  DBG_LOG( (DBG_WML, "\nRootElement: END\n") );
}
/*
=============================================================================
 */
/*  7.3  */
void handlerDefaultElement(SDocData       * data,
                           const SWmlNode * node)
{
/* Generic handler for elements.
 * - handle unknown elements
 * - call attribute handlers
 * - call handlers for child elements
 *---------------------------------------------------------------------------
 */
  /* Data structures */
  SWmlToken *_tag;
  SWmlToken *_mark;
  const SWmlNode * _tmp;
  int              _counter = 0;

  /* Code */

  DBG_LOG2( (DBG_WML, node->m_name, "DefaultElement:") );

  if((node->m_code&0xff)==GT_LITERAL) {
      _tag = addTokenWithStr(data, node->m_code,
                             EVENT->m_tokenValue, STR_IN_TABLE);
  }
  else {
      _tag = addToken(data, node->m_code);
  }

  _mark = _tag;
  REMOVE_EVENT;                                     /* XTOK_START_ELEMENT   */

  while(EVENT->m_token==XTOK_ATTR_NAME) {           /* Attributes */
      _tmp=lookupAttr(data, node);
      _tmp->m_handler(data,_tmp);
  }

  if(_tag->m_next) {                                /* has attribute */
      _tag->m_code|=HAS_ATTRIBUTE;
      _mark = addToken(data, GT_END);               /* END attribute list */
  }

  while(EVENT->m_token!=XTOK_END_ELEMENT) {         /* Content */
      if(   EVENT->m_token==XTOK_UNPARSED_CHARDATA
         || EVENT->m_token==XTOK_PARSED_CHARDATA) {
          processCharData(data,_counter++);
      }
      else {
          _tmp=lookupElement(data);
          _tmp->m_handler(data,_tmp);
      }
  }

  REMOVE_EVENT;                                     /* XTOK_END_ELEMENT     */

  if(_mark->m_next) {                               /* has content */
      _tag->m_code|=HAS_CONTENT;
      addToken(data, GT_END);                       /* END content of element */
  }
}

/*
=============================================================================
 */
/*  7.4  */
void handlerNumber(SDocData       * data,
                   const SWmlNode * node)
{
/* Validate attribute as pozitive integer number (zero included).
 *---------------------------------------------------------------------------
 */

  /* Data structures */
  PString _str = NEXT_EVENT->m_tokenValue;
  WCHAR  *_p   = STR_DATA(_str);
  WCHAR  *_e   = _p + STR_LEN(_str);

  /* Code */
  DBG_LOG2( (DBG_WML, node->m_name, "handlerNumber:") );

  if(!node->m_code) {
      handlerUnknownAttr(data,node);
      return;
  }

  REMOVE_EVENT;                 /* XTOK_ATTR_NAME */

  for(; _p<_e && ISDIGIT(*_p); _p++)
      ;

  if((_p!=_e) || (!STR_LEN(_str))) {
      setWmlError( WML_ERROR_NUMBER, _str, data);
  }

  addToken(data, node->m_code);
  addTokenWithStr(data, GT_STR_I, _str, STR_INLINE);


  REMOVE_EVENT;                 /* XTOK_ATTR_VALUE */
}

/*
=============================================================================
 */
/*  7.5  */
void handlerUnknownAttr(SDocData       * data,
                        const SWmlNode * node)
{
/* Generic handler for unknown attributes.
 *---------------------------------------------------------------------------
 */
  /* Data structures */

  /* Code */
  DBG_LOG2( (DBG_WML, EVENT->m_tokenValue, "UnknownAttr:") );

  addTokenWithStr(data, GT_LITERAL, EVENT->m_tokenValue, STR_IN_TABLE);
  REMOVE_EVENT;                 /* XTOK_ATTR_NAME */
  addTokenWithStr(data, GT_STR_I, EVENT->m_tokenValue, STR_INLINE);
  REMOVE_EVENT;                 /* XTOK_ATTR_VALUE */
}

/*
=============================================================================
 */
/*  7.6  */
void handlerVdata(SDocData       * data,
                  const SWmlNode * node)
{
/* Attribute handler for 'vdata'.
 * - Validate variable references (default: no escape)
 * - Substitute the known 'sub strings' with tokens.
 *---------------------------------------------------------------------------
 */
  /* Data structures */
  Segment   *_seg = makeSegments(data,node,NEXT_EVENT->m_tokenValue);

  /* Code */
  DBG_LOG2( (DBG_WML, node->m_name, "handlerVdata:") );

  if(!_seg) {
      handlerUnknownAttr(data,node);
      return;
  }

  REMOVE_EVENT;                 /* XTOK_ATTR_NAME */

  variables(data, _seg, NOESCAPE_VAR);

  valueSubstitution(data, _seg);

  flushSegments(data, _seg);

  deleteSegment(data, _seg);

  REMOVE_EVENT;                 /* XTOK_ATTR_VALUE */
}

/*
=============================================================================
 */
/*  7.7  */
void handlerURL(SDocData       * data,
                const SWmlNode * node)
{
/* Attribute handler for 'href'.
 * - Validate variable references (default: escape)
 * - Substitute the known 'sub strings' with tokens.
 *---------------------------------------------------------------------------
 */
  /* Data structures */
  Segment   *_seg = makeSegments(data,node,NEXT_EVENT->m_tokenValue);

  /* Code */
  DBG_LOG2( (DBG_WML, node->m_name, "handlerURL:") );

  if(!_seg) {
      handlerUnknownAttr(data,node);
      return;
  }

  REMOVE_EVENT;

  variables(data, _seg, ESCAPE_VAR);

  valueSubstitution(data, _seg);

  flushSegments(data, _seg);

  deleteSegment(data, _seg);

  REMOVE_EVENT;
}

/*
=============================================================================
 */
/*  7.8  */
void handlerCDATA(SDocData       * data,
                  const SWmlNode * node)
{
/* Attribute handler for 'CDATA' type.
 * - Substitute the known 'sub strings' with tokens.
 *---------------------------------------------------------------------------
 */
  /* Data structures */
  Segment   *_seg = makeSegments(data,node,NEXT_EVENT->m_tokenValue);

  /* Code */
  DBG_LOG2( (DBG_WML, node->m_name, "handlerCDATA:") );

  if(!_seg) {
      handlerUnknownAttr(data,node);
      return;
  }

  REMOVE_EVENT;                 /* XTOK_ATTR_NAME */

  valueSubstitution(data, _seg);

  flushSegments(data, _seg);

  deleteSegment(data, _seg);

  REMOVE_EVENT;                 /* XTOK_ATTR_VALUE */
}

/*
=============================================================================
 */
/*  7.9  */
void handlerLength(SDocData       * data,
                   const SWmlNode * node)
{
/* Validate attribute as length.
 *---------------------------------------------------------------------------
 */
  /* Data structures */
  PString _str = NEXT_EVENT->m_tokenValue;
  WCHAR  *_p   = STR_DATA(_str);
  WCHAR  *_e   = _p + STR_LEN(_str);

  /* Code */
  DBG_LOG2( (DBG_WML, node->m_name, "handlerLength:") );

  if(!node->m_code) {
      handlerUnknownAttr(data,node);
      return;
  }

  REMOVE_EVENT;                 /* XTOK_ATTR_NAME */

  for(; _p<_e && ISDIGIT(*_p); _p++)
      ;

  if( (_p==_e) || (*_p==(WCHAR)'%')) {
      addToken(data, node->m_code);
      addTokenWithStr(data, GT_STR_I, _str, STR_INLINE);
  }
  else {
      setWmlError( WML_ERROR_LENGTH, _str, data);
  }



  REMOVE_EVENT;                 /* XTOK_ATTR_VALUE */
}

/*
=============================================================================
 */
/*  7.10  */
void handlerMeta(SDocData       * data,
                 const SWmlNode * node)
{
/* Look forward and do ...
 *---------------------------------------------------------------------------
 */
  /* Data structures */
  SXmlEvent *_event = NEXT_EVENT;
  PString _http     = newString(DOCPOOL);
  PString _content  = newString(DOCPOOL);
  int     _skip         = 0;
  int     _http_seen    = 0;
  int     _content_seen = 0;
  int     _name_seen    = 0;

  /* Code */
  DBG_LOG2( (DBG_WML, node->m_name, "Meta 1:") );

  /* look forward */
  for(; _event->m_token==XTOK_ATTR_NAME; _event=_event->m_next) {
      if(strSameAscii(_event->m_tokenValue,"http-equiv")) {
          strCpy(_http, _event->m_next->m_tokenValue);
          _http_seen++;
      }
      else if(strSameAscii(_event->m_tokenValue,"content")) {
          strCpy(_content, _event->m_next->m_tokenValue);
          _content_seen++;
      }
      else if(strSameAscii(_event->m_tokenValue,"CONTENT")) {
          strCpy(_content, _event->m_next->m_tokenValue);
          _content_seen++;
      }
      else if(strSameAscii(_event->m_tokenValue,"HTTP-EQUIV")) {
          strCpy(_http, _event->m_next->m_tokenValue);
          _http_seen++;
      }
      else if( strSameAscii(_event->m_tokenValue,"forua")
         && strSameAscii(_event->m_next->m_tokenValue,"false")) {
          _skip = 1;
      }
      else if( strSameAscii(_event->m_tokenValue,"name") ) {
          _name_seen++;
      }
      _event=_event->m_next;
  }

  if (_name_seen) {
      if ( _http_seen)
          setWmlError( WML_ERROR_MANY_PROPERTY, 0, data);
      _skip++;
  }

  if(_skip)
      handlerIgnore(data, node);
  else
      handlerDefaultElement(data, node);

  if(!data->m_error && data->m_mAvailSiz && STR_LEN(_http)) {

      if(data->m_mMaxLen<STR_LEN(_content))
          data->m_mMaxLen = STR_LEN(_content);
      if(data->m_mMaxLen<STR_LEN(_http))
          data->m_mMaxLen = STR_LEN(_http);

      if(   (data->m_mCounter < data->m_mAvailSiz)
         && ((data->m_mMaxLen+1U)  <= data->m_mAvailLen)) {
          if(STR_LEN(_content))
              memcpy(CONTENT,STR_DATA(_content),
                     STR_LEN(_content)*sizeof(WCHAR));
          if(STR_LEN(_http))
              memcpy(HTTPEQUIV,STR_DATA(_http),
                     STR_LEN(_http)*sizeof(WCHAR));

         CONTENT[STR_LEN(_content)] = 0;
         HTTPEQUIV[STR_LEN(_http)]  = 0;
      }
      data->m_mCounter++;
  }

  deleteString(_http,   DOCPOOL);
  deleteString(_content,DOCPOOL);
}

/*
=============================================================================
 */
/*  7.11  */
void handlerIgnore(SDocData       * data,
                   const SWmlNode * node)
{
/* Element and attribute handler.
 * - Ignore the current element and its children, or
 * - ignore the current attribute.
 *---------------------------------------------------------------------------
 */
  DBG_LOG2( (DBG_WML, node->m_name, "handlerIgnore:") );

  if(EVENT->m_token == XTOK_START_ELEMENT) {

      REMOVE_EVENT;         /* XTOK_START_ELEMENT   */

      while( EVENT->m_token!=XTOK_END_ELEMENT ) {
          if(EVENT->m_token == XTOK_START_ELEMENT)
              handlerIgnore(data,node);
          else
              REMOVE_EVENT;
      }

      REMOVE_EVENT;         /* XTOK_END_ELEMENT   */
  }
  else if(EVENT->m_token == XTOK_ATTR_NAME) {
      REMOVE_EVENT;                 /* XTOK_ATTR_NAME  */
      REMOVE_EVENT;                 /* XTOK_ATTR_VALUE */
  }
}

/*
=============================================================================
 */
/*  7.12  */
void handlerOnce(SDocData       * data,
                   const SWmlNode * node)
{
/* Element handler.
 * "It is an error for a deck to contain more than one  access  element."
 * The implementation is independent of the name of the element.
 *---------------------------------------------------------------------------
 */
  /* Data structures */

  /* Code */
  DBG_LOG2( (DBG_WML, node->m_name, "Once:") );

  if( !insertUniq(node->m_name, &(data->m_uniqInDeck), data) )
      setWmlError( WML_ERROR_ONCE, node->m_name, data);

  handlerDefaultElement(data, node);
}

/*
=============================================================================
 */
/*  7.13  */
void handlerNotZero(SDocData       * data,
                    const SWmlNode * node)
{
/* Attribute handler.
 * Validate attribute as a non zero positiv number.
 *---------------------------------------------------------------------------
 */
  /* Data structures */
  PString _str = NEXT_EVENT->m_tokenValue;
  WCHAR  *_p   = STR_DATA(_str);
  WCHAR  *_e   = _p + STR_LEN(_str);

  /* Code */
  DBG_LOG2( (DBG_WML, node->m_name, "handlerNotZero:") );

  if(!node->m_code) {
      handlerUnknownAttr(data,node);
      return;
  }

  REMOVE_EVENT;                 /* XTOK_ATTR_NAME */
  
  if(_p==_e || *_p=='0')
      setWmlError( WML_ERROR_ZERO, node->m_name, data);

  for(; _p<_e && ISDIGIT(*_p); _p++)
      ;

  if(_p!=_e)
      setWmlError( WML_ERROR_NUMBER, node->m_name, data);

  addToken(data, node->m_code);
  addTokenWithStr(data, GT_STR_I, _str, STR_INLINE);


  REMOVE_EVENT;                 /* XTOK_ATTR_VALUE */
}

/*
=============================================================================
 */
/*  7.14  */
void handlerOneTask(SDocData       * data,
                    const SWmlNode * node)
{
/* Element handler.
 * "It is an error to specify other than one task element
 * (eg. go, prev or refresh) in an anchor element."
 *
 * Note: this handler collaborates with the 'handlerTask' function.
 *---------------------------------------------------------------------------
 */
  /* Data structures */
  int *_prev    = data->m_task_counter;
  int _counter  = 0;

  /* Code */
  DBG_LOG2( (DBG_WML, node->m_name, "OneTask:") );

  data->m_task_counter = &_counter;

  handlerDefaultElement(data, node);

  if(_counter>1)
      setWmlError( WML_ERROR_ONE_TASK, node->m_name, data);

  data->m_task_counter = _prev;
}

/*
=============================================================================
 */
/*  7.15  */
void handlerTask(SDocData       * data,
                 const SWmlNode * node)
{
/* Element handler.
 * Increment the task counter. (See also: handlerOneTask)
 * (go, prev, refresh, noop)
 *---------------------------------------------------------------------------
 */
  /* Data structures */

  /* Code */
  DBG_LOG2( (DBG_WML, node->m_name, "Task:") );

  if( data->m_task_counter )
      *data->m_task_counter += 1;

  handlerDefaultElement(data, node);
}

/*
=============================================================================
 */
/*  7.16  */
void handlerEvents(SDocData       * data,
                   const SWmlNode * node)
{
/* Element handler.
 * Ref. 9.10 : "Conflicting event bindings within an element are an error."
 * Event binding:
 *  a) element (card,template,option) specific attribute (the value
 *     specifies a URI):
 *      - ontimer
 *      - onenterforward
 *      - onenterbackward
 *      - onpick
 *     These cases must be handled by 'handlerURLEvent'.
 *
 *  b) the type value of the 'onevent' element.
 *     These cases must be handled by 'handlerOnEventType'.
 *
 * This handler initializes a list for the subsequent intrinsic events.
 *---------------------------------------------------------------------------
 */
  /* Data structures */
  StringNode *_prev = data->m_uniqEvent.m_root;

  /* Code */
  DBG_LOG2( (DBG_WML, node->m_name, "Event bindings ") );
  constrUniq(&(data->m_uniqEvent), data);

  handlerDefaultElement(data, node);

  destrUniq(&(data->m_uniqEvent), data);

  data->m_uniqEvent.m_root = _prev;
}

/*
=============================================================================
 */
/*  7.17  */
void handlerURLEvent(SDocData       * data,
                     const SWmlNode * node)
{
/* Attribute handler.
 * (onenterbackward, onenterforward, onpick, ontimer)
 * - register the type of the intrinsic event into the current
 *   m_uniqEvent list.
 * - validate the attribute as URL
 *---------------------------------------------------------------------------
 */
  /* Code */
  DBG_LOG2( (DBG_WML, node->m_name, "handlerURLEvent:") );

  if( !insertUniq(EVENT->m_tokenValue, &(data->m_uniqEvent), data) )
      setWmlError( WML_ERROR_EVENT_BINDING, node->m_name, data);

  handlerURL(data,node);
}

/*
=============================================================================
 */
/*  7.18  */
void handlerOnEventType(SDocData       * data,
                        const SWmlNode * node)
{
/* Attribute handler.
 * ( <onevent type="on...">
 * - register the type of the intrinsic event into the current
 *   m_uniqEvent list.
 * - validate the attribute value as CDATA
 *---------------------------------------------------------------------------
 */
  /* Code */
  DBG_LOG2( (DBG_WML, node->m_name, "handlerOnEventType:") );

  if( !insertUniq(NEXT_EVENT->m_tokenValue, &(data->m_uniqEvent), data) )
      setWmlError( WML_ERROR_EVENT_BINDING, node->m_name, data);

  handlerCDATA(data,node);
}

/*
=============================================================================
 */
/*  7.19  */
void handlerCard(SDocData       * data,
                 const SWmlNode * node)
{
/* Element handler for 'card' and 'template' elements.
 * - initializes a list for the unique 'do' elements
 * - call 'handlerEvents' to validate event bindings.
 * Note:
 * This handler collaborates with the 'handlerDoType'.
 *---------------------------------------------------------------------------
 */
  /* Data structures */
  StringNode *_prev = data->m_uniqDo.m_root;

  /* Code */
  DBG_LOG2( (DBG_WML, node->m_name, "handler 'card'") );
  constrUniq(&(data->m_uniqDo), data);

  handlerEvents(data, node);

  destrUniq(&(data->m_uniqDo), data);

  data->m_uniqDo.m_root = _prev;
}

/*
=============================================================================
 */
/*  7.20  */
void handlerDo(SDocData       * data,
               const SWmlNode * node)
{
/* 
 * Element handler.
 * Figure out the name of the 'do' element, and register it into the
 * current list of 'do' elements.
 * Ref. 9.7
 * Note: this handler collaborates with the
 *       'handlerDoName' and 
 *       'handlerDoType' functions.
 *---------------------------------------------------------------------------
 */
  /* Data structures */
  PString *_prev_name    = data->m_do_name;
  PString *_prev_type    = data->m_do_type;
  PString _name = 0;
  PString _type = 0;

  /* Code */
  DBG_LOG2( (DBG_WML, node->m_name, "Do:") );

  data->m_do_name = &_name;
  data->m_do_type = &_type;

  handlerDefaultElement(data, node);

  if(_name && STR_LEN(_name)) {
      if( !insertUniq(_name, &(data->m_uniqDo), data) )
          setWmlError( WML_ERROR_DUP_DO, node->m_name, data);
  }
  else if(_type && STR_LEN(_type)) {
      if( !insertUniq(_type, &(data->m_uniqDo), data) )
          setWmlError( WML_ERROR_DUP_DO, node->m_name, data);
  }
  else {
      setWmlError( WML_ERROR_MISSING_DO_NAME, node->m_name, data);
  }

  deleteString(_name, DOCPOOL);
  deleteString(_type, DOCPOOL);
  data->m_do_name = _prev_name;
  data->m_do_type = _prev_type;
}

/*
=============================================================================
 */
/*  7.21  */
void handlerDoType(SDocData       * data,
                   const SWmlNode * node)
{
/* Attribute handler.
 * - store the type of current 'do' element.
 * - validate the attribute as CDATA.
 * Note:
 * This handler collaborates with the 'handlerDo' function.
 *---------------------------------------------------------------------------
 */
  /* Code */
  DBG_LOG2( (DBG_WML, node->m_name, "handlerDoType:") );

  if( data->m_do_type && !*data->m_do_type)
      *data->m_do_type = strXDup(NEXT_EVENT->m_tokenValue, DOCPOOL);

  handlerCDATA(data,node);
}

/*
=============================================================================
 */
/*  7.22  */
void handlerDoName(SDocData       * data,
                   const SWmlNode * node)
{
/* Attribute handler.
 * - store the name of current 'do' element.
 * - validate the attribute as CDATA.
 * Note:
 * This handler collaborates with the 'handlerDo' function.
 *---------------------------------------------------------------------------
 */
  /* Code */
  DBG_LOG2( (DBG_WML, node->m_name, "handlerDoType:") );

  if( data->m_do_name && !*data->m_do_name)
      *data->m_do_name = strXDup(NEXT_EVENT->m_tokenValue, DOCPOOL);

  handlerCDATA(data,node);  /* NMTOKEN */
}

/*
=============================================================================
 */
/*  7.23  */
void handlerVarName(SDocData       * data,
                    const SWmlNode * node)
{
/* Attribute handler for 'variable names'.
 * - Validate variable references (default: no escape)
 * - Substitute the known 'sub strings' with tokens.
 * - 
 *---------------------------------------------------------------------------
 */
  Segment   *_seg = makeSegments(data,node,NEXT_EVENT->m_tokenValue);
  Segment   *_tmp;
  int        _cnt;

  /* Code */
  DBG_LOG2( (DBG_WML, node->m_name, "handlerVdata:") );

  if(!_seg) {
      handlerUnknownAttr(data,node);
      return;
  }

  REMOVE_EVENT;                 /* XTOK_ATTR_NAME */

  variables(data, _seg, NOESCAPE_VAR);

  for(_cnt=0, _tmp=_seg; _tmp; _tmp=_tmp->m_next)
      if(_tmp->m_str && !isGoodVarName(_tmp->m_str,++_cnt))
          setWmlError( WML_ERROR_VARNAME, node->m_name, data);

  valueSubstitution(data, _seg);

  flushSegments(data, _seg);

  deleteSegment(data, _seg);

  REMOVE_EVENT;                 /* XTOK_ATTR_VALUE */
}

/*
=============================================================================
 */
/*  7.24  */
void handlerMeta2(SDocData       * data,
                  const SWmlNode * node)
{
/* 
 * *** WML Spec. 1.2 ***
 * Exactly one property.
 *---------------------------------------------------------------------------
 */
  /* Data structures */
  SXmlEvent *_event = NEXT_EVENT;
  PString _http     = newString(DOCPOOL);
  PString _content  = newString(DOCPOOL);
  int     _skip         = 0;
  int     _http_seen    = 0;
  int     _content_seen = 0;
  int     _name_seen    = 0;

  /* Code */

  /* look forward */
  for(; _event->m_token==XTOK_ATTR_NAME; _event=_event->m_next) {
      if(strSameAscii(_event->m_tokenValue,"http-equiv")) {
          strCpy(_http, _event->m_next->m_tokenValue);
          _http_seen++;
      }
      else if(strSameAscii(_event->m_tokenValue,"content")) {
          strCpy(_content, _event->m_next->m_tokenValue);
          _content_seen++;
      }
      else if( strSameAscii(_event->m_tokenValue,"forua")
         && strSameAscii(_event->m_next->m_tokenValue,"false")) {
          _skip = 1;
      }
      else if( strSameAscii(_event->m_tokenValue,"name") ) {
          _name_seen++;
      }
      _event=_event->m_next;
  }

  if ( (_name_seen+_http_seen)!=1 ) {
      if ( (_name_seen+_http_seen) > 1 )
          setWmlError( WML_ERROR_MANY_PROPERTY, 0, data);
      else
          setWmlError( WML_ERROR_NO_PROPERTY, 0, data);
      _skip++;
  }

  if(_skip)
      handlerIgnore(data, node);
  else
      handlerDefaultElement(data, node);

  if(!data->m_error && data->m_mAvailSiz && STR_LEN(_http)) {

      if(data->m_mMaxLen<STR_LEN(_content))
          data->m_mMaxLen = STR_LEN(_content);
      if(data->m_mMaxLen<STR_LEN(_http))
          data->m_mMaxLen = STR_LEN(_http);

      if(   (data->m_mCounter < data->m_mAvailSiz)
         && ((data->m_mMaxLen+1U)  <= data->m_mAvailLen)) {
          if(STR_LEN(_content))
              memcpy(CONTENT,STR_DATA(_content),
                     STR_LEN(_content)*sizeof(WCHAR));
          if(STR_LEN(_http))
              memcpy(HTTPEQUIV,STR_DATA(_http),
                     STR_LEN(_http)*sizeof(WCHAR));

         CONTENT[STR_LEN(_content)] = 0;
         HTTPEQUIV[STR_LEN(_http)]  = 0;
      }
      data->m_mCounter++;
  }

  deleteString(_http,   DOCPOOL);
  deleteString(_content,DOCPOOL);
}

/*
=============================================================================
 */
/*  7.25  */
void handlerPre(SDocData       * data,
                const SWmlNode * node)
{
/* 
 * *** WML Spec. 1.2 ***
 * Element handler.
 * Set the m_preFlag of 'data'. If this flag is set, processCharData() treats
 * strings/lines as preformatted, so it does not modify white-spaces.
 *---------------------------------------------------------------------------
 */
  /* Data structures */

  /* Code */
  DBG_LOG2( (DBG_WML, node->m_name, "Pre:") );

  data->m_preFlag++;

  handlerDefaultElement(data, node);

  data->m_preFlag--;
}

/*
=============================================================================
 */
/*  7.26  */
void handlerCDATA2(SDocData       * data,
                   const SWmlNode * node)
{
/* *** WML Spec. 1.2 ***
 * Attribute handler for 'CDATA' type.
 * - Collapse two dollar signs into one.
 * - Substitute the known 'sub strings' with tokens.
 *---------------------------------------------------------------------------
 */
  /* Data structures */
  Segment   *_seg;
  PString _str = NEXT_EVENT->m_tokenValue;
  WCHAR  *_p   = STR_DATA(_str);
  WCHAR  *_e   = _p + STR_LEN(_str);
  int     _cnt = 0;

  /* Code */
  DBG_LOG2( (DBG_WML, node->m_name, "handlerCDATA2:") );

  /* Deal with dollar signs */
  while( _p<_e) {

      _cnt = ( *_p==(WCHAR)'$') ? _cnt+1 : 0;

      if (_cnt==2) {
          WCHAR *_from = _p;
          WCHAR *_to   = _from-1;
          while ( ++_from<_e)
              *++_to = *_from;
          _str->m_nChar--;       /* Adjust the length of the string. */
          _e--;
          _cnt = 0;
      }
      else {
          _p++;
      }
  }


  /* The rest of the functions is identical with 'handlerCDATA' */

  _seg = makeSegments(data,node,NEXT_EVENT->m_tokenValue);

  if(!_seg) {
      handlerUnknownAttr(data,node);
      return;
  }

  REMOVE_EVENT;                 /* XTOK_ATTR_NAME */

  valueSubstitution(data, _seg);

  flushSegments(data, _seg);

  deleteSegment(data, _seg);

  REMOVE_EVENT;                 /* XTOK_ATTR_VALUE */
}

/*
=============================================================================
 */
/*  7.XX  */
void siHandlerCDATA(SDocData       * data,
                    const SWmlNode * node)
{
/* Attribute handler for 'CDATA' type.
 * - Substitute the known 'sub strings' with tokens.
 *---------------------------------------------------------------------------
 */
  /* Data structures */
  Segment   *_seg = makeSegments(data,node,NEXT_EVENT->m_tokenValue);

  /* Code */
  DBG_LOG2( (DBG_WML, node->m_name, "SI handlerCDATA:") );

  if(!_seg) {
      handlerIgnore(data,node);
      return;
  }

  REMOVE_EVENT;                 /* XTOK_ATTR_NAME */

  valueSubstitution(data, _seg);

  flushSegments(data, _seg);

  deleteSegment(data, _seg);

  REMOVE_EVENT;                 /* XTOK_ATTR_VALUE */
}

/*
=============================================================================
 */
/*  7.XX  */
#define CHAR_ZERO   48U
#define CHAR_T      84U
#define CHAR_Z      90U
#define CHAR_DASH   45U
#define CHAR_DDOT   58U
static int isInRange(const WCHAR *p,
                     unsigned     from,
                     unsigned     to)
{
  unsigned hi = *p     - CHAR_ZERO;
  unsigned lo = *(p+1) - CHAR_ZERO;
  unsigned n  = (hi*10U) + lo;

  if(hi>9 || lo>9)
      return 0;

  return n>=from && n<=to;
}

/*
=============================================================================
 */
/*  7.XX  */
void siHandlerDateTime(SDocData       * data,
                       const SWmlNode * node)
{
  /* 
   *    2  5  8  11 14 17
   *    |  |  |  |  |  |
   * "YYYY-MM-DDThh:mm:ssZ"
   */

  /* Data structures */
  PString _str = NEXT_EVENT->m_tokenValue;
  WCHAR  *_p   = STR_DATA(_str);

  /* Code */
  REMOVE_EVENT;                 /* XTOK_ATTR_NAME */

  if(    (STR_LEN(_str) < 20U)
      || (_p[10]!=CHAR_T)
      || (_p[19]!=CHAR_Z)
      || (_p[4] !=CHAR_DASH)
      || (_p[7] !=CHAR_DASH)
      || (_p[13]!=CHAR_DDOT)
      || (_p[16]!=CHAR_DDOT)
      || !isInRange(_p   ,0U,99U)    /* centuary */
      || !isInRange(_p+2 ,0U,99U)    /* years    */
      || !isInRange(_p+5 ,1U,12U)    /* month    */
      || !isInRange(_p+8 ,1U,31U)    /* days     */
      || !isInRange(_p+11,0U,23U)    /* hours    */
      || !isInRange(_p+14,0U,59U)    /* minutes  */
      || !isInRange(_p+17,0U,59U)){  /* seconds  */

      setWmlError(SI_ERROR_INVALID_DATETIME , node->m_name, data);
  }
      
  addTokenWithStr(data, node->m_code, _str, STR_OPAQUE_DATE);

  REMOVE_EVENT;                 /* XTOK_ATTR_VALUE */
}

/*
=============================================================================
 */
/*  7.XX  */
void siHandlerHREF(SDocData       * data,
                   const SWmlNode * node)
{
  /* Data structures */
  Segment   *_seg = makeSegments(data,node,NEXT_EVENT->m_tokenValue);

  /* Code */
  DBG_LOG2( (DBG_WML, node->m_name, "SI handlerCDATA:") );

  if(!_seg) {
      handlerIgnore(data,node);
      return;
  }

  REMOVE_EVENT;                 /* XTOK_ATTR_NAME */

  valueSubstitution(data, _seg);

  flushSegments(data, _seg);

  deleteSegment(data, _seg);

  REMOVE_EVENT;                 /* XTOK_ATTR_VALUE */
}

/*
=============================================================================
 */

/*
=============================================================================
 */

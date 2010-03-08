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
/* File    : U:\wpsvob\wmlenc\src\common\wmlstruc.c
   Version : \main\9
   Date    : 17-Oct-00.12:47:58
*/

/*
 * ========================================================================
 *  Name        : wmlstruc.c
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

#define WMLMSG_OWNER
#include "wmlvalid.h"

/*  4.2 External Data Structures  */

/*  4.3 External Function Prototypes  */

    /* None */

/*  5  LOCAL CONSTANTS AND MACROS  */


/*  6  MODULE DATA STRUCTURES  */

    /* None */

/*  6.1 Local Data Structures  */


/*  6.2 Local Function Prototypes  */



/*  7 MODULE CODE  */

/*
=============================================================================
 */
/*  7.1  */
const SWmlNode * lookupElement(SDocData *data)
{
/* 1. Pick up the name of the current element from event list.
 * 2. Look into table of tags/elements of SWmlDescriptor for it.
 * 3. Return the corresponding node, or the node of the unknown element.
 * 
 * Note: The value of the current XML event must exits.
 * Assume the calling takes place from the proper context.
 *---------------------------------------------------------------------------
 */
/* Data Structures */
  PString           _str;   /* name of the element                  */
  unsigned          _idx;   /* the index of element in the table    */
  const SWmlNode  * _node;  /* traverses the list                   */
  
/*  Code  */
  _str  = data->m_eventList.m_head->m_tokenValue;
  _idx  = (!STR_LEN(_str)) ? 0 : (*STR_DATA(_str))&0x7f;
  _node = FIRST_NODE(data->m_desc->m_elements[_idx]);
  

  while(_node && !STR_SAME(_node->m_name, _str))
      _node = _node->m_next;

  return _node ? _node : g_unknownElement;
}

/*
=============================================================================
 */
/*  7.2  */
const SWmlNode * lookupAttr(SDocData *data,
                            const SWmlNode *element)
{
/* 1. Pick up the name of the current current attribute from event list.
 * 2. If the 'element' had that attribute: return it.
 * 3. If the attribute was found in the table of attributes of the
 *    SWmlDescriptor: return it.
 * 4. Otherwise return the 'unknown' attribute.
 *
 * Note: The value of the current XML event must exits.
 * Assume the calling takes place from the proper context.
 *---------------------------------------------------------------------------
 */
/* Data Structures */
  PString           _str  = data->m_eventList.m_head->m_tokenValue;
  const SWmlNode  * _node = FIRST_NODE(element->m_children);
  unsigned          _idx;
  
/*  Code  */
  DBG_LOG2( (DBG_WML, _str, "lookupAttr:") );
  while(_node && !STR_SAME(_node->m_name, _str)) {
      DBG_LOG2( (DBG_WML, _node->m_name, "   ?:") );
      _node = _node->m_next;
  }

  if(!_node) { 
      /* global attributes */
      _idx  = (!STR_LEN(_str)) ? 0 : (*STR_DATA(_str))&0x7f;
      _node = FIRST_NODE(data->m_desc->m_attributes[_idx]);

      while(_node && !STR_SAME(_node->m_name, _str))
          _node = _node->m_next;
  }
  DBG_LOG( (DBG_WML, "lookupAttr: found: %p",_node ) );

  return _node ? _node : g_unknownAttribute;
}

/*
=============================================================================
 */
/*  7.3  */
SWmlString  * lookupString(SDocData *data,
                           PString str)
{
/*  Search for 'str' in 'stringTable' of DocData.
 *  Return the node or NULL.
 *---------------------------------------------------------------------------
 */
/* Data Structures */
  unsigned     _idx  = (!STR_LEN(str)) ? 0 : (*STR_DATA(str))&0x7f;
  SWmlString * _node = FIRST_NODE(data->m_stringTable[_idx]);
  
/*  Code  */
  while(_node && !STR_SAME(_node->m_string, str))
      _node = _node->m_next;

  return _node;
}

/*
=============================================================================
 */
/*  7.4  */
SWmlString * insertString(SDocData *data,
                          PString s)
{
/*  
 *  Place a copy of 's' into the 'm_stringTable' of DocData,
 *  and treat it as 'inline'.
 *
 *---------------------------------------------------------------------------
 */
/* Data Structures */
  SWmlString *_node;

/*  Code  */

  POOL_ALLOC(DOCPOOL, _node);
  _node->m_string = strXDup(s,DOCPOOL);
  _node->m_site   = STR_INLINE;
  _node->m_offset = ~0;
  if(STR_LEN(s)) {
      SLL_INSERT(data->m_stringTable[ (*(STR_DATA(s)))&0x7f], _node);
  }
  else {
      SLL_INSERT(data->m_stringTable[0], _node);
  }

  return _node;
}

/*
=============================================================================
 */
/*  7.5  */
SWmlToken * addToken(SDocData *data,
                     UINT16    code)
{
/* Allocate a new node for 'code' and append it to the list of WML tokens.
 *---------------------------------------------------------------------------
 */
/* Data Structures */
  SWmlToken * _node;

/* Code */

  POOL_ALLOC(DOCPOOL, _node);
  _node->m_code     = code;
  _node->m_strEntry = 0;
  FIFO_INSERT(data->m_tokenList, _node);
  
  return _node;
}

/*
=============================================================================
 */
/*  7.6  */
static void appendToTbl(SDocData   * data,
                        SWmlString * entry,
                        int          must)
{
/*  
 * Append string to the string table of the output binary stream,
 * according to the output character encoding.
 * Note: this function can fail if the output character encoding cannot
 *       represent one of the character.( I cannot use character
 *       entity in the string table.)
 *---------------------------------------------------------------------------
 */
/* Data Structures */
  UINT32  _n;                               /* string length            */
  WCHAR  *_s;                               /* ptr to string            */
  SCCOut *_scc    = &data->m_scc;           /* character converter      */
  UINT32  _offset = data->m_array.m_size;   /* save the current offset  */
  int     _failed = 0;                      /* assume: success          */


/* Code */
  if(entry->m_site==STR_IN_TABLE)
      return;                                   /* it is already there */

  /* Convert and copy */

  _n = STR_LEN(entry->m_string);
  _s = STR_DATA(entry->m_string);

  for( ; _n; _n--,_s++) {
      _scc->m_char = *_s;

      if(!(_scc->m_convert(_scc))) {
          if(must) {
              setWmlError( WML_ERROR_CHAR_CONV, 0, data);
          }
          _failed = 1;
          break;
      }
  }

  _scc->m_char = 0;
  _scc->m_convert(_scc);                /* terminating zero */

  if(!_failed) {
      entry->m_site=STR_IN_TABLE;
      entry->m_offset = _offset;
  }
}

/*
=============================================================================
 */
/*  7.7  */
SWmlToken * addTokenWithStr(SDocData *data,
                            UINT16    code,
                            PString   str,
                            ESite     site)
{
/* 
 * Append a new node for 'code' and 'str' to the list of WML tokens.
 * If str has never been seen before, create a new entry for it to remember,
 * otherwise treat the string entry as going to string table of the
 * output stream.
 * Note: site==STR_IN_TABLE only when it was called for unknown element
 *       or attribute.
 *---------------------------------------------------------------------------
 */
/* Data Structures */
  SWmlToken * _node;
  SWmlString * _strEntry = str ? lookupString(data,str) : 0; 

/* Code */

  if(site==STR_OPAQUE_DATE && str && STR_LEN(str)) {
      POOL_ALLOC(DOCPOOL, _strEntry);
      _strEntry->m_string = strXDup(str,DOCPOOL);
      _strEntry->m_site   = STR_OPAQUE_DATE;
      _strEntry->m_offset = ~0;

      SLL_INSERT(data->m_stringTable[0], _strEntry);
  }
  else if(_strEntry) {                      /* it has been seen */
      appendToTbl(data,_strEntry, 0);       /* 0: try to insert */
  }
  else if(str) {
      _strEntry = insertString(data,str);
      if(site==STR_IN_TABLE) {
          appendToTbl(data,_strEntry, 1);   /* 1: must insert   */
      }
  }
  /* If _strEntry is still NULL, it behaves as 'addToken'. */

  POOL_ALLOC(DOCPOOL, _node);
  _node->m_code     = code;
  _node->m_strEntry = _strEntry;
  FIFO_INSERT(data->m_tokenList, _node);
  
  return _node;
}

/*
=============================================================================
 */
/*  7.8  */
void lookupGenVal(SDocData *data,
                  Segment  *seg)
{
/* The string of the segment can contain subststrings what were specified
 * as 'Attribute value tokens' in SWmlDescriptor. ('.com/','.edu', etc.)
 * Search for these strings and substitute them, breaking the segment into
 * furter segments.
 *---------------------------------------------------------------------------
 */
/* Data Structures */
  PString   str       = seg->m_str;
  UINT16    pos       = 0;
  const WCHAR      *s = STR_DATA(str);
  const SWmlNode *node;


/*  Code  */
  for(; pos<STR_LEN(str); ++pos, ++s) {
      node = FIRST_NODE(data->m_desc->m_generalValues[(*s)&0x7f]);
      for(; node; node=node->m_next) {
      DBG_LOG2( (DBG_WML, node->m_name, "     :") );
          if(STR_LEN(str)>=(STR_LEN(node->m_name)+pos)) {
              const WCHAR *p = STR_DATA(node->m_name);
              const WCHAR *e = p + STR_LEN(node->m_name);
              const WCHAR *t = s;
              for(; p<e && *p==*t; p++,t++)
                  ;
              if(p==e) {
                  if(pos) {
                      seg->m_str = newSubString(str, 0, pos, DOCPOOL);
                      seg        = newSegment(data,0,0,seg);
                  }

                  seg->m_str  = 0;
                  seg->m_code = node->m_code;

                  if(STR_LEN(str)>(STR_LEN(node->m_name)+pos)) {
                      newSegment(data,
                                 newSubString(str,
                                           (UINT16)(pos+STR_LEN(node->m_name)),
                                           (UINT16) STR_LEN(str),
                                              DOCPOOL),
                                 0,
                                 seg);
                  }
                deleteString(str, DOCPOOL);
                return; 
              }
          }
      }
  }
}

/*
=============================================================================
 */
/*  7.9  */
void valueSubstitution(SDocData * data,
                       Segment  * seg)
{
/* Here is the driver of 'lookupGenVal'.
 *---------------------------------------------------------------------------
 */
/* Data structures */
/* Code */
  for(; seg; seg=seg->m_next) {
      if( (!seg->m_code) && seg->m_str) {
          lookupGenVal(data,seg);
      }
  }
}

/*
=============================================================================
 */
/*  7.10  */
void setWmlError(WML_VALIDATION_ERROR   status,
                 PString     context,
                 SDocData   *data)
{
/* Set the status of the document if it was clean.
 *---------------------------------------------------------------------------
 */
/* Data structures */
  char _buf[256];
/* Code */

  if(!data->m_error) {
      if(context) {
          wPrint(STR_LEN(context),STR_DATA(context),
                 64,_buf);
          if(data->m_docURL)
              wmlLOGssss( WSL_LOG_ERROR,
                      "WML error:",
                      (char*)data->m_docURL,
                      (char*)error2str(status),
                      _buf);
          else
              wmlLOGsss( WSL_LOG_ERROR,
                      "WML error: URL?:",
                      (char*)error2str(status),
                      _buf);
      }
      else {
          if(data->m_docURL)
              wmlLOGsss( WSL_LOG_ERROR,
                      "WML error:",
                      (char*)data->m_docURL,
                      (char*)error2str(status));
          else
              wmlLOGss( WSL_LOG_ERROR,
                      "WML error: URL?:",
                      (char*)error2str(status));
      }

      data->m_error = status;
  }

}

/*
=============================================================================
 */
/*  7.11  */
void constrUniq (SLIST(StringNode) *list,
                 SDocData          *data)
{
/* 
 *---------------------------------------------------------------------------
 */
/* Code */
  list->m_root = 0;
}

/*
=============================================================================
 */
/*  7.12  */
void destrUniq  (SLIST(StringNode) *list,
                 SDocData          *data)
{
/* 
 *---------------------------------------------------------------------------
 */
/* Data structures */
  StringNode *_node;

/* Code */
  while( (_node=SLL_GET( (*list), _node)) ) {
      deleteString(_node->m_str,DOCPOOL);
      POOL_FREE(DOCPOOL, _node);
  }
}

/*
=============================================================================
 */
/*  7.13  */
int  insertUniq (PString            str,
                 SLIST(StringNode) *list,
                 SDocData          *data)
{
/* 
 *---------------------------------------------------------------------------
 */
/* Data structures */
  StringNode *_node;

/* Code */
  for(_node=list->m_root; _node; _node=_node->m_next)
      if(STR_SAME(_node->m_str, str))
          return 0;

  POOL_ALLOC(DOCPOOL, _node);
  _node->m_str  = strXDup(str,DOCPOOL);
  SLL_INSERT( (*list), _node );
  return 1;
}
/*
=============================================================================
*/


/*  End of File  */

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
/* File    : U:\wpsvob\wmlenc\src\common\wmlsegm.c
   Version : \main\7
   Date    : 17-Oct-00.12:47:53
*/

/*
 * ========================================================================
 *  Name        : wmlsegm.c
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

#include "wmlvalid.h"

/*  4.2 External Data Structures  */


/*  4.3 External Function Prototypes  */

    /* None */

/*  5  LOCAL CONSTANTS AND MACROS  */


/*  6  MODULE DATA STRUCTURES  */

    /* None */

/*  6.1 Local Data Structures  */


/*  6.2 Local Function Prototypes  */

static int isPrefix(const PString val,
             const PString pref);


/*  7 MODULE CODE  */


/*
=============================================================================
 */
/*  7.1  */
Segment * newSegment(SDocData  *data,
                     PString    str,
                     UINT16     code,
                     Segment   *prev)
{
/*  Create new segment and initialize to contain 'str' snd 'code'.
 *  Append the new segment to 'prev' segment, if it was not NULL.
 *---------------------------------------------------------------------------
 */
/* Data Structures */
  Segment * _seg;

/* Code */
  POOL_ALLOC(DOCPOOL, _seg);
  if(prev) {
      _seg->m_next = prev->m_next;
      prev->m_next= _seg;
  }
  else {
      _seg->m_next = 0;
  }
  _seg->m_str  = str;
  _seg->m_code = code;

  return _seg;
}

/*
=============================================================================
 */
/*  7.2  */
void deleteSegment(SDocData  *data,
                   Segment   *seg)
{
/*  Delete 'seg' and all of its members.
 *  Note: this could be a recursive call.
 *---------------------------------------------------------------------------
 */
/* Data Structures */

/* Code */
  if(seg) {
      if(seg->m_next)
          deleteSegment(data, seg->m_next);
      deleteString(seg->m_str,DOCPOOL);
      POOL_FREE(DOCPOOL, seg);
  }
}

/*
=============================================================================
 */
/*  7.3  */
Segment * makeSegments(SDocData       * data,
                       const SWmlNode * attrNode,
                       const PString    value)
{
/* Create segments (tokens, strings) of attribute values.
 *
 *  1. If 'value' exactly matches with one of the possible attribute value,
 *     return a segment that contains the 'attribute start token'.
 *     (optimistic approach).
 *  2. If the 'value' is a real prefix,
 *      ( One of the attribute prefix is a substring of the value,
 *        beginning at the first character of 'value')
 *     return with two segments. The first contain
 *     the code of the prefix, the second contain the rest of
 *     the value to process.
 *  3. If the 'value' is not prefix, but the attribute has its own
 *     binary code, return with two segment. The first contain the
 *     binary code of the attribute, the next contain the rest of
 *     the value to process.
 *  4. Worst case: no prefix, no binary code for attribute.
 *     It will be treated as unknown attribute.
 *     (The table was not defined properly or a strange DTD !)
 *     Return a NULL pointer.
 *
 *---------------------------------------------------------------------------
 */
  /* Data structures */
  const SWmlNode *_node = FIRST_NODE(attrNode->m_children);
  Segment        *_root = 0;

  /* Code */
  DBG_ASSERT(value);
  DBG_LOG2( (DBG_WML, attrNode->m_name, "make:") );

  for(; _node; _node = _node->m_next) {
      if(STR_LEN(_node->m_name) > STR_LEN(value) )
          continue;
      if(isPrefix(value,_node->m_name)) {
          _root = newSegment(data,0,_node->m_code,0);
          if(STR_LEN(value)!=STR_LEN(_node->m_name)) {
              /* real prefix */
              newSegment(data,
                         newSubString(value,
                                      STR_LEN(_node->m_name),
                                      STR_LEN(value),
                                      DOCPOOL),
                                    0,
                                    _root);
              DBG_LOG2( (DBG_WML, value, "real prefix:") );
          }
          else {
              DBG_LOG2( (DBG_WML, value, "exact prefix:") );
          }
          break;
      }
  }

  if(!_root && attrNode->m_code) {
      /* no prefix but binary code for attribute */
      DBG_LOG2( (DBG_WML, value, "not prefix:") );
      _root = newSegment(data,0,attrNode->m_code,0);
      newSegment(data,
                 strXDup(value, DOCPOOL),
                 0,
                 _root);
  }

  return _root;
}

/*
=============================================================================
 */
/*  7.4  */
void flushSegments(SDocData * data,
                   Segment  * seg)
{
/*  
 * Process the segments of the attribute value.
 *
 *---------------------------------------------------------------------------
 */
  /* Data structures */

  /* Code */
  for(; seg; seg=seg->m_next) {
      if(seg->m_code && seg->m_str) {
          addTokenWithStr(data, seg->m_code, seg->m_str, STR_INLINE);
      }
      else if(seg->m_code) {
          addToken(data, seg->m_code);
      }
      else if(seg->m_str) {
          addTokenWithStr(data, GT_STR_I, seg->m_str, STR_INLINE);
      }
  }
}

/*
=============================================================================
 */
/*  7.5  */
static int isPrefix(const PString val,
             const PString pref)
{
/*  Is 'pref' a substring of the 'val', beginning
 *  at the first character of 'val' ?
 *  
 *  Return : bool
 */
  /* Data structures */
  const WCHAR *pv, *pr, *pe;

  /* Code */
  DBG_ASSERT(val && pref && (STR_LEN(val)>=STR_LEN(pref)));

  pv = STR_DATA(val);
  pr = STR_DATA(pref);
  pe = STR_DATA(pref)+STR_LEN(pref);

  while( (pr<pe) && (*pr++==*pv++) )
      ;

  return pr==pe;
}
/*
=============================================================================
*/


/*  End of File  */

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
/* File    : U:\wpsvob\wmlenc\src\common\wmlvar.c
   Version : \main\7
   Date    : 17-Oct-00.12:48:13
*/

/*
 * ========================================================================
 *  Name        : wmlvar.c
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

#define GOOD    (_p<_e)
#define INCR    (++_p<_e)
#define CH      (*_p)
#define IS(ch)  (CH==(WCHAR)(ch))
#define CHAR(ch) ((IS(ch) || IS((ch)+('a'-'A'))))
#define VLETTER  ((CH>=(WCHAR)'a' && CH<=(WCHAR)'z')||\
                  (CH>=(WCHAR)'A' && CH<=(WCHAR)'Z') || (CH==(WCHAR)'_'))
#define ISDIGIT(c)   (((c)>=(WCHAR)'0') && ((c)<=(WCHAR)'9'))
#define DIGIT    ISDIGIT(CH)
#define DRAIN_ESCAPE ( CHAR('S') && INCR && CHAR('C') && INCR && CHAR('A') &&\
        INCR && CHAR('P') && INCR && CHAR('E') && INCR )
#define DRAIN_NOESC ( CHAR('O') &&\
        INCR && CHAR('E') && INCR && CHAR('S') && INCR && CHAR('C') && INCR)
#define DRAIN_UNESC ( CHAR('N') &&\
        INCR && CHAR('E') && INCR && CHAR('S') && INCR && CHAR('C') && INCR)

/*  6  MODULE DATA STRUCTURES  */

    /* None */

/*  6.1 Local Data Structures  */


/*  6.2 Local Function Prototypes  */

static void checkVar(SDocData  *data,
                     Segment   *seg,
                     EscapeMode mode);


/*  7 MODULE CODE  */



/*
=============================================================================
 */
/*  7.1  */
void variables(SDocData     * data,
               Segment      * seg,
               EscapeMode     mode)
{
/* The string of the segment possible contain variable references.
 * Search for references, and substitute them with the proper segments,
 * breaking the original into new segments.
 * The 'mode' indicates the default escape mode in the current context.
 *---------------------------------------------------------------------------
 */
  /* Data structures */

  /* Code */
  for(; seg; seg=seg->m_next) {
      if( (!seg->m_code) && seg->m_str) {
          PString str= seg->m_str;
          WCHAR * _p = STR_DATA(str);
          WCHAR * _e = _p + STR_LEN(str);

          do {
              for(; _p<_e && (*_p!=(WCHAR)'$'); _p++)
                  ;
              if( ((_p+1)<_e) && (*(_p+1) == (WCHAR)'$')) {
                  WCHAR *_to   = ++_p;  /* Collaps the two '$' into one.    */
                  WCHAR *_from = _to+1;
                  while(_from < _e) {
                      *_to++ = *_from++;
                  }
                  str->m_nChar--;       /* Adjust the length of the string. */
                  _e--;
              }
              else {
                  break;
              }
          } while (1) ;

          if(_p>=_e) {
              continue;                 /* No variable. */
          }
          DBG_LOG2( (DBG_WML, str, "VARIABLE1:") );

          if(_p==STR_DATA(str)) {
              checkVar(data,seg,mode);
          }
          else {
              UINT16 pos = _p - STR_DATA(str);

              newSegment(data,
                         newSubString(str, pos, STR_LEN(str), DOCPOOL),
                         0,
                         seg);
              str->m_nChar = pos;
          }
      }
  }
}

/*
=============================================================================
 */
/*  7.2  */
static void checkVar(SDocData  *data,
                     Segment   *seg,
                     EscapeMode mode)
{
/*  
 *---------------------------------------------------------------------------
 */
/* Data Structures */
  PString _name= newString(DOCPOOL);
  PString _str = seg->m_str;
  WCHAR  *_p   = STR_DATA(_str);
  WCHAR  *_e   = _p + STR_LEN(_str);
  int     _bra = 0;
  UINT16  _code;

/* Code */
  DBG_ASSERT(*(STR_DATA(_str)) == ((WCHAR)'$'));

  if(!INCR) {
      DBG_LOG2( (DBG_WML, _str, "ERROR: !INCR") );
      setWmlError( WML_ERROR_VAR, _str, data);
      deleteString(_name,DOCPOOL);
      return;
  }
  if( IS('(') ) {
      _bra++;
      if(!INCR) {
          DBG_LOG2( (DBG_WML, _str, "ERROR: IS('(')") );
          setWmlError( WML_ERROR_VAR, _str, data);
          deleteString(_name,DOCPOOL);
          return;
      }
  }
  /* Build the name */
  if(!VLETTER) {
      DBG_LOG2( (DBG_WML, _str, "ERROR: !VLETTER") );
      setWmlError( WML_ERROR_VAR_CHAR, _str, data);
      deleteString(_name,DOCPOOL);
      return;
  }
  strAppendChar(_name,CH);
  while(INCR && (VLETTER || DIGIT)) {
      strAppendChar(_name,CH);
  }

  if(_bra) {
      if( GOOD && IS(':') && INCR) {
          if( (IS('E') || IS('e')) && INCR && (IS(')') || DRAIN_ESCAPE) ) {
              mode = ESCAPE_VAR;
          }
          else if( (IS('N') || IS('n')) && INCR && (IS(')') || DRAIN_NOESC) ) {
              mode = NOESCAPE_VAR;
          }
          else if( (IS('U') || IS('u')) && INCR && (IS(')') || DRAIN_UNESC) ) {
              mode = UNESCAPE_VAR;
          }
          else {
               DBG_LOG2( (DBG_WML, _str, "ERROR: esc") );
               setWmlError( WML_ERROR_VAR_ESC, _str, data);
               deleteString(_name,DOCPOOL);
               return;
          }
      }
      if( (!GOOD) || (!IS(')')) ) {
          DBG_LOG2( (DBG_WML, _str, "ERROR: bra") );
          setWmlError( WML_ERROR_VAR_KET, _str, data);
          deleteString(_name,DOCPOOL);
          return;
      }
      _p++;
  }
  switch(mode) {
     case ESCAPE_VAR   : _code = GT_EXT_I_0; break;
     case UNESCAPE_VAR : _code = GT_EXT_I_1; break;
     default           : _code = GT_EXT_I_2; break;
  }

  if(_p<_e) {
      newSegment(data,
                 newSubString(_str,
                              (UINT16)(_p-STR_DATA(_str)),
                              (UINT16)STR_LEN(_str),
                              DOCPOOL),
                 0,
                 seg);
  }
  deleteString(_str,DOCPOOL);
  seg->m_str = _name;
  seg->m_code= _code;
}

/*
=============================================================================
 */
/*  7.3  */
int isGoodVarName(const PString str,
                  int           cnt)
{
  /* Data */
  int           _ok = 0;

  /* Code */
  if ( str && STR_LEN(str) ) {
      const WCHAR  *_p  = STR_DATA(str);
      const WCHAR  *_e  = _p + STR_LEN(str);

      if(cnt==1) {
          if( VLETTER ) {
              _ok=1;
              while(INCR) {
                  if( !(VLETTER || ISDIGIT(*_p)) )
                      _ok = 0;
              }
          }
      }
      else {
          _ok=1;
          while(INCR) {
              if( !(VLETTER || ISDIGIT(*_p)) )
                  _ok = 0;
          }
      }
  }

  return _ok;
}
/*
=============================================================================
*/


/*  End of File  */

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
/* File    : U:\wpsvob\wmlenc\src\common\xmlstr.c
   Version : \main\8
   Date    : 17-Oct-00.12:49:47
*/

/*
 * ========================================================================
 *  Name        : xmlstr.c
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

        The 'xml string' functions were designed to serve the XML Parser,
        to provide a simple and reasonable fast string functions for
        Unicode (non zero terminated) strings.

    1.3 Specification/Design Reference

        doc/wmlarch.doc

    1.4 Module Test Specification Reference

        none

    1.5 Compilation Information

        See: building.txt

    1.6 Notes

        - The allocated size of the strings can grow only.
          (except the deleteString() func).

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
        7.1   newString
        7.2   deleteString
        7.3   strXDup
        7.4   strCpy
        7.5   strCat
        7.6   strAppendChar
        7.7   strSame
        7.8   strStrip
        7.9   strOneSpace
        7.10  strNormalize
        7.11  wPrint
        7.12  invPString
        7.13  newSubString
        7.14  strCatAscii
        7.15  constrString
        7.16  destrString
        7.17  strSameAscii

*/



/*  3  GLOSSARY

        string: I use the term 'string' for objects, described
                by 'struct String_s', as it was declared in xmlstr.h.
                Also I use it as a synonym of 'PString' (a pointer
                to 'struct String_s') when I am speaking about return
                values and function parameters.

*/


/*  4  EXTERNAL RESOURCES  */

    /* None */

/*  4.1  Include Files  */

#include "xmlstr.h"
#include "xmlpool.h"
#include "xmlctyp.h"

/*  4.2 External Data Structures  */

    /* None */


/*  4.3 External Function Prototypes  */

    /* None */


/*  5  LOCAL CONSTANTS AND MACROS  */

#define XLLOC(pool,p,n) {\
    POOL_ALLOC((pool),p);\
    p->m_nChar=(n);\
    if((n)<=STR_INITIAL_SIZE) {\
        p->m_nAllocatedChar = STR_INITIAL_SIZE; p->m_data=p->m_smallBuf;\
    } else { p->m_nAllocatedChar = n; p->m_data= (WCHAR*)XML_ALLOC((n)<<1);\
    }\
}
                   
#define DEL_MEM(pool,p) {\
    if(p->m_data!=p->m_smallBuf)\
        XML_FREE(p->m_data);\
    POOL_FREE((pool),p);\
}
                  
#define NEWBUF(p, newlen) {\
    if(p->m_data!=p->m_smallBuf)\
        XML_FREE(p->m_data);\
    p->m_data = XML_ALLOC((newlen)<<1);\
    p->m_nAllocatedChar = (newlen);\
}

#define REXLLOC(p, newlen) {\
    WCHAR *_tmp=XML_ALLOC((newlen)<<1);\
    p->m_nAllocatedChar = (newlen);\
    memcpy(_tmp,p->m_data,p->m_nChar<<1);\
    if(p->m_data!=p->m_smallBuf)\
        XML_FREE(p->m_data);\
    p->m_data = _tmp;\
}

#define CR	0xdU
#define NL	0xaU


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
PString  newString(SPool *pool)     /* In: pool for alloc */
{
/*  Functional Description
 *
 *  Creates a new String
 *
 *  Return Value:  the new String.
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

    PString   _retv;        /* return value */

/*  Code  */
    XLLOC(pool,_retv,0);

    return _retv;
}

/*
 *===========================================================================
 */
/*  7.2  */
void deleteString(PString str,      /* In: string to free   */
                  SPool *pool)      /* In: pool of str      */
{
/*  Functional Description
 *
 *  Release memory used for str.
 *
 *  Return Value:  -
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/*  Code  */
  if(str) {
      DEL_MEM(pool,str);
  }
}

/*
 *===========================================================================
 */
/*  7.3  */
PString  strXDup(PString src,    /* In: string to duplicate */
                SPool *pool)    /* In: pool for new string */
{
/*  Functional Description
 *
 *  Duplicates a string.
 *
 *  Return Value:  the new string
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

    PString _retv;

/*  Code  */
    if(!src) {
        return newString(pool);
    }

    XLLOC(pool,_retv,src->m_nChar);

    if(_retv->m_nChar)
        memcpy(_retv->m_data, src->m_data, _retv->m_nChar<<1);

    return _retv;
}
/*
 *===========================================================================
 */
/*  7.4  */
PString  strCpy(PString dst,          /* In: destination */
                const PString src)    /* In: source      */
{
/*  Functional Description
 *
 *  String copy.
 *
 *  Return Value:  the destination string
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/*  Code  */
    if( dst->m_nAllocatedChar < src->m_nChar)
        NEWBUF(dst,src->m_nChar);

    dst->m_nChar = src->m_nChar;
    memcpy(dst->m_data, src->m_data, dst->m_nChar<<1);

    return dst;
}

/*
 *===========================================================================
 */
/*  7.5  */
PString  strCat(PString dst,            /* In: destination */
                const PString src)      /* In: source      */
{
/*  Functional Description
 *
 *  Concatenates 'src' to 'dst'.
 *
 *  Return Value:  the destination string
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/*  Code  */
    if( dst->m_nAllocatedChar < (dst->m_nChar+src->m_nChar) )
        REXLLOC(dst,dst->m_nChar+src->m_nChar+16U);

    memcpy(dst->m_data+dst->m_nChar, src->m_data, src->m_nChar<<1);
    dst->m_nChar += src->m_nChar;

    return dst;
}

/*
 *===========================================================================
 */
/*  7.6  */
void strAppendChar(PString dst,     /* In: destination string */
                   WCHAR ch)        /* In: character to append */
{
/*  Functional Description
 *
 *  Appends 'ch' to 'dst'.
 *
 *  Return Value:  -
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  if( dst->m_nAllocatedChar < (dst->m_nChar+1U))
      REXLLOC(dst,dst->m_nChar+16U);

  *(dst->m_data+dst->m_nChar) = ch;
  dst->m_nChar++;
}

/*
 *===========================================================================
 */
/*  7.7  */
int strSame(const PString s1,   /* In: string 1 */
            const PString s2)   /* In: string 2 */
{
/*  Functional Description
 *
 *  Compares two string.
 *
 *  Return Values:  0        - differ
 *                  not zero - same
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

    WCHAR   *_p1;
    WCHAR   *_pe;
    WCHAR   *_p2;

/*  Code  */

    if(s1->m_nChar != s2->m_nChar)
        return 0;

    _p2 = s2->m_data;
    _p1 = s1->m_data;
    _pe = _p1+s1->m_nChar;
    while(_p1<_pe)
        if( *_p1++ != *_p2++ )
            return 0;

    return 1;
}

/*
 *===========================================================================
 */
/*  7.8  */
int strStrip(PString s)     /* In: the string to strip  */
{
/*  Functional Description
 *
 *  Cuts the leading and trailing white spaces
 *
 *  Return Value:  the length of result string
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */
  WCHAR * _end;
  WCHAR * _to;
  WCHAR * _cp;

  if(!s || !STR_LEN(s))
      return 0;

  _cp = STR_DATA(s)+STR_LEN(s);

  /* strip trailing white space */
  while(--_cp>STR_DATA(s) && IS_SPACE(*_cp))
      ;

  if(IS_SPACE(*_cp))             /* If white space, that is the first ch */
      return (s->m_nChar=0);
  
  /* There is at least one non ws character */

  /* adjust size */
  s->m_nChar = (_cp - s->m_data)+1;

  /* strip leading white space */
  _end = s->m_data + s->m_nChar;
  _cp  = s->m_data;
  while(_cp<_end && IS_SPACE(*_cp))  /* find the first non ws */
      _cp++;   
  
  if(_cp==s->m_data) {
      return s->m_nChar;            /* no leading ws */
  }
      
  /* copy the rest of the string to the beginning */
  _to = s->m_data;
  while( _cp<_end)
      *_to++ = *_cp++;

  /* adjust size */

  return (s->m_nChar = _to - s->m_data);
}

/*
 *===========================================================================
 */
/*  7.9  */
void strOneSpace(PString s)     /* In: the string to process  */
{
/*  Functional Description
 *
 *  Collapses the sequence of white spaces into a single 0x20.
 *
 *  Return Value:  -
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  WCHAR * _end = STR_DATA(s)+STR_LEN(s);
  WCHAR * _to  = STR_DATA(s);
  WCHAR * _cp  = _to+1;

/*  Code  */
  if(!STR_LEN(s))
      return;

  if(IS_SPACE(*_to))
      *_to = ' ';

  for(; _cp<_end; _cp++) {
      if( IS_SPACE(*_cp) ) {
          if(!IS_SPACE(*_to))
              *++_to = ' ';
      }
      else
          *++_to=*_cp;
  }

  /* adjust size */
  s->m_nChar = _to-s->m_data+1;
}

/*
 *===========================================================================
 */
/*  7.10  */
void strNormalize(PString s)     /* In: the string to process  */
{
/*  Functional Description
 *
 *  Replaces white space characters with #x20, except the
 *  #xd,#xa sequence, what replaced with a single #x20.
 *
 *  Return Value:  -
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  WCHAR * _end = STR_DATA(s)+STR_LEN(s);
  WCHAR * _to  = STR_DATA(s);
  WCHAR * _cp  = _to;

/*  Code  */
  for(; _cp<_end; _cp++, _to++) {
      if( IS_SPACE(*_cp) ) {
          if( (*_cp==0x0d) && ((_cp+1)<_end) && (*(_cp+1)=='\n') )
              _cp++;

          *_to = ' ';
      }
      else
          *_to = *_cp;
  }

  /* adjust size */
  s->m_nChar = _to - s->m_data;
}

/*
 *===========================================================================
 */
/*  7.11  */
char * wPrint(UINT32     nWChar,
            const WCHAR     *wBuf,
            UINT32    nChar,
            char      *buf)
{
/*  Functional Description
 *
 *  Print string to stdout.
 *
 *  Return Value:  -
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

    UINT32  _iw = 0;     /* character counter */
    UINT32  _i  = 0;     /* character counter */
    UINT32  _ch;
    UINT32  _len;
    char tmp[128];
    
/*  Code  */

    if(nChar<4U) {
        DBG_ASSERT(0);
        *buf = 0;
        return buf;
    }
    nChar-=4;

    buf[0] ='>';

    for(_i=1 ; (_i<nChar) && (_iw<nWChar); _iw++ ) {
        _ch = wBuf[_iw];
        if( _ch == (_ch&0x7f) ) {
            buf[_i++] = (_ch==CR || _ch==NL) ? '\\' : (char)_ch;
        }
        else {
            sprintf(tmp,"(x%lx)", (unsigned long)_ch);
            _len = strlen(tmp);
            if( (_len+_i) < nChar ) {
                strcpy( &(buf[_i]), tmp);
                _i+=_len;
            }
            else {
                break;
            }
        }
    }
    if(_iw==nWChar)
        buf[_i] = '<';
    buf[++_i] = 0;
    return buf;
}
#ifdef INVARIANTS
/*
 *===========================================================================
 */
/*  7.12  */
void invPString(PString s)
{
/*  Functional Description
 *
 *  Check status of string 's'.
 *
 *  Return Values:  -
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

  assert(s);
  assert(s->m_nChar<=s->m_nAllocatedChar);
  assert(s->m_data);
  if(s->m_nChar>STR_INITIAL_SIZE) {
      assert(s->m_data!=s->m_smallBuf);
  }
  else {
      assert(s->m_data==s->m_smallBuf);
  }
}
#endif  /* INVARIANTS */

/*
 *===========================================================================
 */
/*  7.13  */
PString   newSubString  (const PString    src,
                         UINT16           fromPos,  /* included */
                         UINT16           toPos,    /* excluded */
                         SPool          * pool)
{
/*  Functional Description
 *
 *  Duplicates a string.
 *
 *  Return Value:  the new string
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

    PString _retv;
    UINT16  _nch;

/*  Code  */

    if(toPos>STR_LEN(src))
        toPos = STR_LEN(src);

    _nch = toPos>fromPos ? (toPos-fromPos) : 0;

    XLLOC(pool, _retv, _nch);

    if(_nch) {
        memcpy(_retv->m_data, src->m_data+fromPos, _nch<<1);
    }

    return _retv;
}

/*
 *===========================================================================
 */
/*  7.14  */
PString  strCatAscii(PString     dst,      /* In: destination */
                     const char *src)      /* In: source      */
{
/*  Functional Description
 *
 *  Concatenates 'src' to 'dst'.
 *
 *  Return Value:  the destination string
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/*  Code  */
  if(src && dst) {
      while(*src) {
          strAppendChar(dst,(WCHAR)*src++);
      }
  }

  return dst;
}

/*
 *===========================================================================
 */
/*  7.15  */
void      constrString(PString p)
{
  p->m_nChar          = 0;
  p->m_nAllocatedChar = STR_INITIAL_SIZE;
  p->m_data           = p->m_smallBuf;
}

/*
 *===========================================================================
 */
/*  7.16  */
void   destrString(PString p)
{
  if(p->m_data!=p->m_smallBuf) {
        XML_FREE(p->m_data);
  }
}

/*
 *===========================================================================
 */
/*  7.17  */
int  strSameAscii(const PString s,
                  const char *ascii)
{
/*  Compare with ascii string.
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */
  const WCHAR * p;
  UINT16        n;

/*  Code  */
  if(!s || !ascii)
      return 0;

  n = STR_LEN(s);
  p = STR_DATA(s);

  for(; n && *ascii && *ascii==*p; n--, ascii++, p++)
      ;

  return !*ascii && !n;
}


/*
 *===========================================================================
 */
/*  7.  */
void strStripLeading(PString s)
{
/* Cuts the leading white spaces
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */
  WCHAR * _end;
  WCHAR * _to;
  WCHAR * _cp;

  if(!s || !STR_LEN(s))
      return;

  /* strip leading white space */
  _end = s->m_data + s->m_nChar;
  _cp  = s->m_data;
  while(_cp<_end && IS_SPACE(*_cp))  /* find the first non ws */
      _cp++;   
  
  if(_cp==s->m_data) {
      return;                       /* no leading ws */
  }
      
  /* copy the rest of the string to the beginning */
  _to = s->m_data;
  while( _cp<_end)
      *_to++ = *_cp++;

  /* adjust size */

  s->m_nChar = _to - s->m_data;
}

/*
 *===========================================================================
 */
/*  7.  */
void strStripTrailing(PString s)
{
/*  Cuts the trailing white spaces
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */
  WCHAR * _cp;

  if(!s || !STR_LEN(s))
      return;

  _cp = STR_DATA(s)+STR_LEN(s);

  /* strip trailing white space */
  while(--_cp>STR_DATA(s) && IS_SPACE(*_cp))
      ;

  if(IS_SPACE(*_cp))             /* If white space, that is the first ch */
      s->m_nChar=0;
  else
      s->m_nChar = (_cp - s->m_data)+1;
}

/*
=============================================================================
*/


/*  End of File  */

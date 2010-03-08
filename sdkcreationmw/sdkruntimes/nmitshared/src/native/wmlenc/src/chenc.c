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
/* File    : U:\wpsvob\wmlenc\src\common\chenc.c
   Version : \main\10
   Date    : 17-Oct-00.12:45:20
*/

/*
 * ========================================================================
 *  Name        : chenc.c
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

        Character conversions.


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
        7.1   initCharEncoding
        7.2   termCharEncoding
        7.3   convertToUnicode
        7.4   cnvMap2
        7.5   cnvBytesToUni1
        7.6   cnvBytesToUni
        7.7   cnvUTF8ToUni
        7.8   cnvUTF16ToUni
        7.9   cnvUCS4ToUni
        7.10  lookForEncoding
        7.11  checkByteFormat
        7.12  checkFormat
        7.13  funcUni2UTF16
        7.14  funcUni2Byte1
        7.15  funcUni2Byte2
        7.16  funcUni2UTF8
        7.17  funcUni2Failure
        7.18  constrSCCOut
        7.19  destrSCCOut

*/



/*  3  GLOSSARY

        None

*/


/*  4  EXTERNAL RESOURCES  */

    /* None */

/*  4.1  Include Files  */

#include "chenc.h"
#include "xmlctyp.h"


/*  4.2 External Data Structures  */

    /* None */


/*  4.3 External Function Prototypes  */

    /* None */


/*  5  LOCAL CONSTANTS AND MACROS  */


#define MAP(buff)     (buff->m_enc->m_map)
#define NAME(buff)    (buff->m_enc->m_name)
#define FORMAT(buff)  (buff->m_enc->m_format)

#define CH              scc->m_char
#define ADD(v)          ADD_TO_ARRAY(scc->m_array,(BYTE)((v)&0xffU))
#define HI_BYTE(a)     ((BYTE)(((a)>>8)&0xffU))
#define LO_BYTE(a)     ((BYTE)((a)&0xffU))



/*  6  MODULE DATA STRUCTURES  */


/*  6.1 Local Data Structures  */

typedef enum {
    E_LITTLE_ENDIAN,
    E_BIG_ENDIAN
}EByteOrder;

static EByteOrder s_internalByteOrder = E_LITTLE_ENDIAN;

/*  6.2 Local Function Prototypes  */


static int cnvUTF8ToUni
                    (SEncBuffers *enc);

static int cnvUTF16ToUni
                    (SEncBuffers *enc,
                     EByteOrder order);

static int cnvUCS4ToUni
                    (SEncBuffers *enc,
                     EByteOrder order);

static int cnvBytesToUni
                    (SEncBuffers *buf);

static int cnvBytesToUni1
                    (SEncBuffers *buf);

static int cnvMap2
                    (SEncBuffers *buf);

static int lookForEncoding
                    (const UINT8 *buf,
                     UINT32     bufsiz,
                     char *     encname,
                     WCHAR     encsiz);

static ECharFormat checkByteFormat
                    (SEncBuffers *  buf);

static ECharFormat checkFormat
                    (SEncBuffers *  buf,
                     EByteOrder  *  pOrder);


/*  7 MODULE CODE  */

/*
=============================================================================
*/
/*  7.1  */
void initCharEncoding()
{
/*
-----------------------------------------------------------------------------
*/
/*  Data structures */
  union { UINT8 b[2]; WCHAR s;} _u;

/* Code */
  
  _u.s = 1;
  s_internalByteOrder = _u.b[0] ? E_LITTLE_ENDIAN : E_BIG_ENDIAN;

  initChMap();              /* Initialize character maps */
}

/*
=============================================================================
*/
/*  7.2  */
void termCharEncoding()
{
/*
-----------------------------------------------------------------------------
*/
/* Code */
  
  termChMap();
}

/*
=============================================================================
*/
/*  7.3  */
int convertToUnicode(SEncBuffers *buffers)
{
/*
-----------------------------------------------------------------------------
*/
/*  Data Structures  */
  EByteOrder  _order= s_internalByteOrder;
  int         _retv;
  WCHAR     * _dst;
  WCHAR     * _dstEnd;

/*  Code  */
  if(!buffers->m_enc) {
      DBG_ASSERT(0);
      return 0;
  }

  switch(checkFormat(buffers, &_order) ) {
     case CF_BYTE  : _retv = cnvBytesToUni(buffers);
                   break;
     case CF_UTF8  : _retv = cnvUTF8ToUni(buffers);
                   break;
     case CF_UTF16 : _retv = cnvUTF16ToUni(buffers, _order);
                   break;
     case CF_UCS4  : _retv = cnvUCS4ToUni(buffers, _order);
                   break;
     default     : _retv = 0;
                   break;
  }
  if(_retv) {
      /* XML characters ? */
      _dst    = buffers->m_dst;
      _dstEnd = buffers->m_dst+buffers->m_dstSize;
      for(; _dst<_dstEnd; _dst++) {
          if(!IS_XMLCHAR(*_dst)) {
              wmlLOGs(WSL_LOG_ERROR, "Not XML character");
              buffers->m_dstSize = 0;
              _retv = 0;
              break;
          }
      }
  }
  return _retv;
}

/*
=============================================================================
*/
/*  7.4  */
static int cnvMap2(SEncBuffers *buf)
/*
-----------------------------------------------------------------------------
*/
{
  /* Data Structures */
  const BYTE       * src    = buf->m_src;
  const BYTE       * srcEnd = (buf->m_src + buf->m_srcSize);
  WCHAR            * dst    = buf->m_dst;
  const SLIST(SCharNode) * nodes;

  /* Code */

  nodes = MAP(buf)->m_fromBC;

  for(; src<srcEnd; src++) {

      WCHAR value, tmp;

      if (*src<0x80U) {
          value = *src;
      }
      else {

          tmp = (WCHAR)*src;      /* high byte first  */
          tmp <<= 8;
          tmp |= (WCHAR)*++src;   /* low byte   */
          if(tmp<0x80U) {
              value = tmp;
          }
          else {
              CHCONVERT(nodes, tmp, value);
              if(!value) {
                  return 0;
              }
          }
      }
      *dst++ = value;;
  }


  buf->m_dstSize = dst - buf->m_dst;

  return buf->m_dstSize != 0U;
}

/*
=============================================================================
*/
/*  7.5  */
static int cnvBytesToUni1(SEncBuffers *buf)
{
/*
-----------------------------------------------------------------------------
*/
  /* Data Structures */
  const BYTE       * _src    = buf->m_src;
  const BYTE       * _srcEnd = buf->m_src + buf->m_srcSize;
  WCHAR            * _dst    = buf->m_dst;
  const SLIST(SCharNode) * _nodes  = MAP(buf)->m_fromBC;

  /* Code */
  for(; _src<_srcEnd; _src++, _dst++) {
      if(*_src<0x80U) {
          *_dst = *_src;
      }
      else {
          CHCONVERT(_nodes, *_src, *_dst);
          if(!*_dst) {
              DBG_LOG( (DBG_ENC,"cnvBytesToUni1:%c -> ? ",*_src) );
              return 0;
          }
      }
  }
  buf->m_dstSize = _dst - buf->m_dst;

  return buf->m_dstSize != 0U;
}

/*
=============================================================================
*/
/*  7.6  */
static int cnvBytesToUni(SEncBuffers *buf)
{
/*
-----------------------------------------------------------------------------
*/
  /* Data Structures */
  int _retv = 0;

  /* Code */
  DBG_ASSERT(MAP(buf));
  if(MAP(buf)) {
      if((buf->m_srcSize) <= buf->m_dstSize) {
          if(MAP(buf)->m_nByte==2)
              _retv = cnvMap2(buf);
          else
              _retv = cnvBytesToUni1(buf);
      }
  }
  if(!_retv)
      buf->m_dstSize = 0;
  return _retv;
}

/*
=============================================================================
*/
/*  7.7  */
static int cnvUTF8ToUni(SEncBuffers *enc)
{
/*
-----------------------------------------------------------------------------
*/
  /* Data Structures */
  const UINT8  * _src    = enc->m_src;
  const UINT8  * _srcEnd = enc->m_src + enc->m_srcSize;
  WCHAR       * _dst    = enc->m_dst;
  const WCHAR * _dstEnd = enc->m_dst + enc->m_dstSize;
  int            _good   = 1;

  /* Code */

  for(; _dst<_dstEnd && _src<_srcEnd; _dst++, _src++) {
      if( !(*_src&0x80) ) {                 /* 1 byte sequence */
          *_dst = (WCHAR) *_src;
      }
      else if( (*_src&0xe0)==0xc0           /* 2 byte sequence */
               && (*_src&0x1f)              /* must be         */
               && ((_src+1)<_srcEnd)        /* available       */
               && ((*(_src+1)&0xc0)==0x80)  /* next is valid   */
               ) {
          *_dst = (((WCHAR)(*_src&0x1f)) << 6 );
          *_dst |= (*++_src&0x3f); 
      }
      else if( (*_src&0xf0)==0xe0           /* 3 byte sequence */
               && (*_src&0x0f)              /* must be         */
               && ((_src+2)<_srcEnd)        /* available       */
               && ((*(_src+1)&0xc0)==0x80)  /* 2. is valid     */
               && ((*(_src+2)&0xc0)==0x80)  /* 3. is valid     */
               ) {
          *_dst = (((WCHAR)(*_src&0x0f)) << 12 );
          *_dst |= (((WCHAR)(*++_src&0x3f)) << 6 );
          *_dst |= (*++_src&0x3f); 
      }
      else {
          wmlLOGs(WSL_LOG_ERROR,"Invalid UTF-8 sequence.");
          _good = 0;
          break;
      }
  }
  enc->m_dstSize = (_good && _src==_srcEnd) ? _dst-enc->m_dst : 0;

  return enc->m_dstSize!=0U;
}

/*
=============================================================================
*/
/*  7.8  */
static int cnvUTF16ToUni(SEncBuffers *enc, EByteOrder order)
{
/*
-----------------------------------------------------------------------------
*/
/* Data Structures */

  const BYTE  * _src;
  WCHAR       * _dst;
  const WCHAR * _dstEnd;
  BYTE          _lo;

/* Code */
  if((enc->m_srcSize>1U) && (enc->m_dstSize>=enc->m_srcSize/2)) {
      _src          = enc->m_src;
      _dst          = enc->m_dst;
      enc->m_dstSize= (enc->m_srcSize/2);
      _dstEnd       = (enc->m_dst + enc->m_dstSize);

      if(order==E_BIG_ENDIAN) {
          for(; _dst<_dstEnd; _dst++, _src++) {
              *_dst = *_src;
              *_dst <<= 8;
              *_dst |= *++_src;
          }
      }
      else {
          for(; _dst<_dstEnd; _dst++, _src++) {
              _lo = *_src;
              *_dst = *++_src;
              *_dst <<= 8;
              *_dst |= _lo;
          }
      }
  }
  else {
      enc->m_dstSize = 0;
      wmlLOGs(WSL_LOG_ERROR,"Empty or small buffer");
  }
  
  return enc->m_dstSize!=0U;
}

/*
=============================================================================
*/
/*  7.9  */
static int cnvUCS4ToUni(SEncBuffers *enc, EByteOrder order)
{
/*
-----------------------------------------------------------------------------
*/
/* Data Structures */

  const BYTE  * _src;
  WCHAR       * _dst;
  const WCHAR * _dstEnd;
  BYTE          _b;

/* Code */

  if((enc->m_srcSize>3U) && (enc->m_dstSize >= (enc->m_srcSize/4))) {
      enc->m_dstSize = enc->m_srcSize/4;

      _src    = enc->m_src;
      _dst    = enc->m_dst;
      _dstEnd = (enc->m_dst + enc->m_dstSize);

      if(order==E_BIG_ENDIAN) {
          for(; _dst<_dstEnd; _dst++, _src++) {
              if(*_src)     break;
              if(*++_src)   break;
              *_dst = *++_src;
              *_dst |= *++_src;
          }
      }
      else {
          for(; _dst<_dstEnd; _dst++, _src++) {
              _b = *_src;
              *_dst = *++_src;
              *_dst |= _b;
              if(*++_src)   break;
              if(*++_src)   break;
          }
      }
      if(_dst<_dstEnd) {
          wmlLOGs(WSL_LOG_ERROR,"Doc. contains character >=0xffff");
          enc->m_dstSize = 0;
      }
  }
  else {
      enc->m_dstSize = 0;
      wmlLOGs(WSL_LOG_ERROR,"Empty or small buffer");
  }
  
  return enc->m_dstSize!=0U;
}

/*
=============================================================================
*/
/*  7.10  */
static int lookForEncoding(const UINT8 *buf,
                           UINT32 bufsiz,
                           char *encname,
                           WCHAR encsiz)
{
/*
-----------------------------------------------------------------------------
*/
/* Data Structures */

  static char s_strEnc[]="encoding";
  UINT8 *p    = (UINT8*)((void*)s_strEnc);
  const UINT8 * const endp = buf + bufsiz;

/* Code */
  
  /* search for "encoding" */
  while( *p && (buf<endp) && (*buf!='>') ) {
      if(*buf=='e') {
          for(p=(UINT8*)((void*)s_strEnc);
              (buf<endp) && (*buf==*p) ;
              buf++, p++)
              ;
      }
      else
          buf++;
  }

  if(*p)
      return 0;     /* not found */

  /* skip white space and '=' and delimiter */
  while( (buf<endp) && (!IS_NAMECHAR(*buf)) )
      buf++;

  /* strcpy */
  while(--encsiz && (buf<endp) && (IS_NAMECHAR(*buf)))
      *encname++ = *buf++;

  *encname = 0;
  return 1;
}

/*
=============================================================================
*/
/*  7.11  */
static ECharFormat checkByteFormat(SEncBuffers *  buf)
{
/*
 * See: REC-xml-19980210 Appendix F. Autodetection of Character Encodings
-----------------------------------------------------------------------------
*/
  /* Data Structures */
  const UINT32   _siz  = buf->m_srcSize;
  const UINT8   * p    = buf->m_src;
  ECharFormat    _retv = CF_UTF16;
  SCharEncoding *_enc  = 0;
  char           _encname[256];

  /* Code */

  /* ASCII, UTF-8, ISO-8859 */
  if( (_siz>=3U) && (p[0]=='<') && (p[1]=='?') && (p[2]=='x')) {
      if( lookForEncoding(p, _siz, _encname, 256) ) {
          if(!(_enc=getCharEncoding(_encname)) ) { 

              wmlLOGss(WSL_LOG_ERROR,"Unknown character encoding: ", _encname);
              _retv = eInvalidCharFormat;
          }
          else {
              if(_enc->m_format==CF_BYTE || _enc->m_format==CF_UTF8) {
                  buf->m_enc = _enc;
                  DBG_LOG( (DBG_ENC,"encoding=\"%s\"",NAME(buf)) );
                  _retv  = _enc->m_format;
              }
              else {
                  wmlLOGss( WSL_LOG_ERROR,"encoding mismatch: ", _enc->m_name);
                  _retv = eInvalidCharFormat;
              }
          }
      }
      else {
          if(FORMAT(buf)==CF_UTF16 || FORMAT(buf)==CF_UCS4) {
              buf->m_enc = getCharEncoding("UTF-8");
              DBG_LOG( (DBG_ENC,"Changed: %s --> UTF-8",NAME(buf)) );
              _retv = CF_UTF8;
          }
          else {
              DBG_LOG( (DBG_ENC,"Encoding: <?x : %s",NAME(buf)) );
              _retv  = FORMAT(buf);
          }
      }
  }
  return _retv;
}

/*
=============================================================================
*/
/*  7.12  */
static ECharFormat checkFormat(SEncBuffers  *  buf,
                               EByteOrder   *  pOrder)
{
/*
 * See: REC-xml-19980210 Appendix F. Autodetection of Character Encodings
-----------------------------------------------------------------------------
*/
  /* Data Structures */
  const UINT32   _siz  = buf->m_srcSize;
  const UINT8   * p    = buf->m_src;
  ECharFormat    _retv = eInvalidCharFormat;


  /* Code */
  *pOrder = s_internalByteOrder;

  /* ASCII, UTF-8, ISO-8859, ... */
  if( (_retv=checkByteFormat(buf))!=CF_UTF16) {
      return _retv;
  }

  /* UTF-16 little endian */
  if( ((_siz)>=2U) && (p[0]==0xFF) && (p[1]==0xFE) ) {
      buf->m_srcSize -= 2;
      buf->m_src     += 2;
      *pOrder = E_LITTLE_ENDIAN;
      _retv   = CF_UTF16;
      DBG_LOG( (DBG_ENC,"Encoding: UTF-16 little endian") );
  }
  /* UTF-16 big endian */
  else if( (_siz>=2U) && (p[1]==0xFF) && (p[0]==0xFE)) {
      buf->m_srcSize -= 2;
      buf->m_src     += 2;
      *pOrder = E_BIG_ENDIAN;
      _retv   = CF_UTF16;
      DBG_LOG( (DBG_ENC,"Encoding: UTF-16 big endian") );
  }
  /* UTF-16 little endian without byte order mark */
  else if( (_siz>=4U) && (p[0]==0x3c) && (p[2]==0x3f) && ((p[1]|p[3])==0) ) {
      *pOrder = E_LITTLE_ENDIAN;
      _retv   = CF_UTF16;
      DBG_LOG( (DBG_ENC,"Encoding: UTF-16 little endian (no mark)") );
  }
  /* UTF-16 big endian without byte order mark */
  else if( (_siz>=4U) && (p[1]==0x3c) && (p[3]==0x3f) && ((p[0]|p[2])==0) ) {
      *pOrder = E_BIG_ENDIAN;
      _retv   = CF_UTF16;
      DBG_LOG( (DBG_ENC,"Encoding: UTF-16 big endian (no mark)") );
  }
  /* UCS4 big endian */
  else if(_siz>=4U &&  (p[0]|p[1]|p[2])==0 && p[3]==0x3c) {
      *pOrder = E_BIG_ENDIAN;
      _retv   = CF_UCS4;
      DBG_LOG( (DBG_ENC,"Encoding: UCS4 big endian") );
  }
  /* UCS4 little endian */
  else if( (_siz>=4U) && ((p[1]|p[2]|p[3])==0) && (p[0]==0x3c)) {
      *pOrder = E_LITTLE_ENDIAN;
      _retv   = CF_UCS4;
      DBG_LOG( (DBG_ENC,"Encoding: UCS4 little endian") );
  }
  else {
      /* Use default */
      _retv = FORMAT(buf);
      DBG_LOG( (DBG_ENC,"Default encoding: %s",NAME(buf)) );
  }

  return _retv;
}


/*
=============================================================================
*/
/*  7.13  */
int funcUni2UTF16(SCCOut *scc)
{
  ADD(HI_BYTE(CH));
  ADD(LO_BYTE(CH));
  return 1;
}

/*
=============================================================================
*/
/*  7.14  */
int funcUni2Byte1(SCCOut *scc)
{
  BYTE _ch;

  if(CH<0x80U) {
      ADD(CH);
  }
  else {
      CHCONVERT(scc->m_nodes, CH, _ch);
      if(!_ch) {
          return 0;
      }
      ADD(_ch);
  }
  return 1;
}

/*
=============================================================================
*/
/*  7.15  */
int funcUni2Byte2(SCCOut *scc)
{
  WCHAR _value;

  if(CH<0x80U) {
      ADD(CH);
      return 1;
  }


  CHCONVERT(scc->m_nodes, CH, _value);
  if(!_value) {
      return 0;
  }

  ADD(HI_BYTE(_value));     /* first byte must be >= 0x80   */
  ADD(LO_BYTE(_value));     /* second byte                  */

  return 1;
}

/*
=============================================================================
*/
/*  7.16  */
int funcUni2UTF8(SCCOut *scc)
{
/*
   UCS-4 range (hex.)           UTF-8 octet sequence (binary)
   0000 0000-0000 007F   0xxxxxxx
   0000 0080-0000 07FF   110xxxxx 10xxxxxx
   0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx

   0001 0000-001F FFFF   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
   0020 0000-03FF FFFF   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
   0400 0000-7FFF FFFF   1111110x 10xxxxxx ... 10xxxxxx
-----------------------------------------------------------------------------
*/
/* Data Structures */
  WCHAR  _ch;
  BYTE one, two, three;

  if( !(CH&0xff80) ) {                  /* 1 byte sequence */
      ADD(CH);
  }
  else if( !(CH&0xf800) ) {          /* 2 byte sequence */
      ADD( 0xc0U | (CH>>6) );
      ADD( 0x80U | (CH&0x3f) );
  }
  else {                                /* 3 byte sequence */
      _ch = CH;
      three = 0x80U | (_ch&0x3f); _ch>>=6;
      two   = 0x80U | (_ch&0x3f); _ch>>=6;
      one   = 0xe0U | _ch;
      ADD( one );
      ADD( two );
      ADD( three );
  }
  return 1;
}

/*
=============================================================================
*/
/*  7.17  */
int funcUni2Failure(SCCOut *scc)
{
  return 0;
}
/*
=============================================================================
*/
/*  7.18  */
int constrSCCOut(SCCOut        *scc,
                 PArray         array,
                 const char    *encName)
{
  const SCharEncoding *enc = getCharEncoding(encName);

  scc->m_convert = funcUni2Failure;
  scc->m_array   = array;
  scc->m_nodes   = 0;
  scc->m_MIBenum = 0;

  if(enc) {
      scc->m_MIBenum = enc->m_MIBenum;
      switch(enc->m_format) {
         case CF_BYTE  : if(enc->m_map) {
                             if(enc->m_map->m_nByte == 2 )
                                 scc->m_convert = funcUni2Byte2;
                             else
                                 scc->m_convert = funcUni2Byte1;
                             scc->m_nodes = enc->m_map->m_fromUC;
                         }
                         break;
         case CF_UTF8  : scc->m_convert = funcUni2UTF8;
                         break;
         case CF_UTF16 : scc->m_convert = funcUni2UTF16;
                         break;
         case CF_UCS4:
         default     : return 0;
                       break;
      };
  }

  return scc->m_convert!=funcUni2Failure;
}

/*
=============================================================================
*/
/*  7.19  */
void destrSCCOut(SCCOut *scc)
{
}

/*
=============================================================================
*/


/*  End of File  */

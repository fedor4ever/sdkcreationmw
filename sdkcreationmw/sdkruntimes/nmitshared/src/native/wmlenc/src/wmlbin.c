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
/* File    : U:\wpsvob\wmlenc\src\common\wmlbin.c
   Version : \main\12
   Date    : 17-Oct-00.12:47:02
*/

/*
 * ========================================================================
 *  Name        : wmlbin.c
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

        This module called as a last phase of WML encoding
        to create the binary output.


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

        7.1   writeMultiByte
        7.2   appendString
        7.3   convertToWBXML

*/



/*  3  GLOSSARY

        None

*/


/*  4  EXTERNAL RESOURCES  */

    /* None */

/*  4.1  Include Files  */

#include "wmlapi.h"
#include "wmlarray.h"
#include "chenc.h"
#include "wmlvalid.h"

/*  4.2 External Data Structures  */

    /* None */


/*  4.3 External Function Prototypes  */

    /* None */


/*  5  LOCAL CONSTANTS AND MACROS  */

#define ADD(val)    ADD_TO_ARRAY(&(data->m_array),(val))
#define HI_BYTE(a)     (((a)>>8)&0xffU)

#define SET_PAIR(i,j)   _time[i]=_bs[j];\
                        _time[i]<<=4;\
                        _time[i]|=_bs[j+1];


/*  6  MODULE DATA STRUCTURES  */


/*  6.1 Local Data Structures  */


/*  6.2 Local Function Prototypes  */

    /* None */


/*  7 MODULE CODE  */


/*
=============================================================================
*/
/*  7.1  */
static void writeMultiByte(PArray array,
                    unsigned long data)
{
/*  Functional Description
 *
 *  Description: To encode a 32bit data using the multibyte representation
 *               specified in XML documentations.
 *
 *
 *  Input:       array - points to the destination dynamic array
 *               data  - 32bit data
 *
 *  Returns:     void
*---------------------------------------------------------------------------
 */

/*  Data Structures  */
/*  Code  */

  unsigned char buffer[16];
  unsigned char *e = &(buffer[8]);
  unsigned char *p = e;

  *p++ = (unsigned char)data & 0x7f;

  data >>= 7;

  while (data) {
    *p++ = (unsigned char)data | 0x80;
    data >>= 7;
  }

  for (p--; p >= e; p--)
      ADD_TO_ARRAY(array, *p);
}

/*
=============================================================================
*/
/*  7.2  */
static void appendString(SDocData *data, PString str)
{
/* 
 *---------------------------------------------------------------------------
 */
  /* Data structures */

  UINT32 _n    = STR_LEN(str);
  WCHAR *_s    = STR_DATA(str);
  SCCOut *_scc = &(data->m_scc);

  /* Code */

  for( ; _n; _n--,_s++) {
      _scc->m_char = *_s;
      if(!_scc->m_convert(_scc)) {
          _scc->m_char = 0;
          _scc->m_convert(_scc);                /* zero */

          ADD_TO_ARRAY(_scc->m_array, GT_ENTITY);
          writeMultiByte(_scc->m_array, *_s );
          ADD_TO_ARRAY(_scc->m_array, GT_STR_I );
      }
  }
  _scc->m_char = 0;
  _scc->m_convert(_scc);                /* zero */
}

/*
=============================================================================
*/
/*  7.2  */
static void appendDate(SDocData *data, PString str)
{
/* 
 *---------------------------------------------------------------------------
 */
  /* Data structures */

  WCHAR *_s    = STR_DATA(str);
  BYTE   _time[8], *_p;
  BYTE   _bs[20];
  int    _i    = 0;

  /* Code */
  for(_i=0; _i<8; _i++)
      _time[_i]=0;

  for(_i=0; _i<20; _i++)
      _bs[_i] =  (STR_LEN(str)>_i) ? (BYTE)(_s[_i]-48U) : (BYTE)0U;


                    /* Example: "1999-04-30T06:40:00Z" */
  SET_PAIR(0,0);    /* centuary */
  SET_PAIR(1,2);    /* decades  */
  SET_PAIR(2,5);    /* month    */
  SET_PAIR(3,8);    /* day      */
  SET_PAIR(4,11);   /* hours    */
  SET_PAIR(5,14);   /* minutes  */
  SET_PAIR(6,17);   /* seconds  */

  for(_i=7; _i>=0 && !_time[_i]; _i--)
      ;

  if(++_i>0) {
      _p = _time;

      ADD(GT_OPAQUE);
      writeMultiByte(&data->m_array, (unsigned long)_i);
      while(_i--) {
          ADD(*_p);
          _p++;
      }
  }
}

/*
=============================================================================
*/
/*  7.3  */
void convertToWBXML(SDocData *data, UINT32 wbxmlVersion)
{
  /*  Data structures */

  SWmlToken *_tok;
  UINT32     _strTblSize = data->m_array.m_size;
  BYTE       _curCodePage = 0;
  BYTE        _code;
  UINT32     _offsetEnd;
  UINT32     _headerSize;

  /* Code */
  
  /******************************/
  _tok = data->m_tokenList.m_head ;
  while(_tok) {

      if(HI_BYTE(_tok->m_code)!=(UINT16)_curCodePage) {
          /* change the code page */
          _curCodePage = HI_BYTE(_tok->m_code);
          ADD(GT_SWITCH_PAGE);
          ADD(_curCodePage);
      }

      _code = _tok->m_code & 0xff;

      if(_tok->m_strEntry) {
          if(_tok->m_strEntry->m_site==STR_IN_TABLE) {
              switch( _code ) {
                 case GT_STR_I   : _code = GT_STR_T;        break;
                 case GT_EXT_I_0 : _code = GT_EXT_T_0;      break;
                 case GT_EXT_I_1 : _code = GT_EXT_T_1;      break;
                 case GT_EXT_I_2 : _code = GT_EXT_T_2;      break;
              }
              ADD(_code);
              writeMultiByte(&data->m_array,_tok->m_strEntry->m_offset);
          }
          else if(_tok->m_strEntry->m_site==STR_OPAQUE_DATE) {
              ADD(_code);
              appendDate(data,_tok->m_strEntry->m_string);
          }
          else {
              ADD(_code);
              appendString(data,_tok->m_strEntry->m_string);
          }
      }
      else {
          ADD(_code);
      }
      _tok = _tok->m_next;
  }
  /******************************/

  _offsetEnd = data->m_array.m_size;


  if( wbxmlVersion == 0 ) {         /* WBXML version 1.0  */
      ADD(0);
      writeMultiByte(&data->m_array, data->m_dtdPUBIDCode);
      if ( !data->m_dtdPUBIDCode )
          writeMultiByte(&data->m_array, 0);
  }
  else {
      if( wbxmlVersion == 1 ) {     /* WBXML version 1.1  */
          ADD(1);
      }
      else if( wbxmlVersion == 2 ) {/* WBXML version 1.2  */
          ADD(2);
      }
      else {                        /* WBXML version 1.3  */
          ADD(3);
      }
      writeMultiByte(&data->m_array, data->m_dtdPUBIDCode );
      if ( !data->m_dtdPUBIDCode )
          writeMultiByte(&data->m_array, 0);
      writeMultiByte(&data->m_array, data->m_scc.m_MIBenum);    /* charset */
  }

  writeMultiByte(&data->m_array, _strTblSize );

  _headerSize = data->m_array.m_size - _offsetEnd;

  if(data->m_array.m_size<=*(data->m_oBufferSize)) {
      *(data->m_oBufferSize) = data->m_array.m_size;
      memcpy(data->m_oBuffer, data->m_array.m_data+_offsetEnd, _headerSize);
      memcpy(data->m_oBuffer+_headerSize, data->m_array.m_data, _offsetEnd);
  }
  else
      *(data->m_oBufferSize) =  0;


}

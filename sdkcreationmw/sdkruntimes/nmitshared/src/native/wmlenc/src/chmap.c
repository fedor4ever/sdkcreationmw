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
/* File    : U:\wpsvob\wmlenc\src\common\chmap.c
   Version : \main\8
   Date    : 17-Oct-00.12:45:29
*/

/*
 * ========================================================================
 *  Name        : chmap.c
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

        
        Managing, mapping character sets.


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
        7.1   initChMap
        7.2   termChMap
        7.3   newSCharEncoding
        7.4   deleteSCharEncoding
        7.5   addAliasName
        7.6   addCharacterSet
        7.7   getCharEncoding
        7.8   newSCharMap
        7.9   deleteSCharMap
        7.10  getChFromList
        7.11  sameName

*/



/*  3  GLOSSARY

        None

*/


/*  4  EXTERNAL RESOURCES  */

    /* None */

/*  4.1  Include Files  */

#include "chenc.h"
#include "xmlsymt.h"


/*  4.2 External Data Structures  */

    /* None */


/*  4.3 External Function Prototypes  */

    /* None */


/*  5  LOCAL CONSTANTS AND MACROS  */

    /* None */



/*  6  MODULE DATA STRUCTURES  */


/*  6.1 Local Data Structures  */

SPool                        s_charPool;

DECLARE_SLIST(SCharMap)      s_ListOfMaps;
DECLARE_SLIST(SCharEncoding) s_ListOfNames;

static UINT16                s_ascii_map[]={0,0};

/*  6.2 Local Function Prototypes  */

static SCharMap      *  newSCharMap
                            (const UINT16 *array);

static void             deleteSCharMap
                            (SCharMap *map);

static SCharEncoding *  newSCharEncoding
                            (const char *name,
                             const SCharMap *map,
                             ECharFormat format,
                             unsigned long MIBenum); 

static void             deleteSCharEncoding
                            (SCharEncoding *sname);

static int              sameName
                            (const char *s1,
                             const char *s2);

/*  7 MODULE CODE  */

/*
=============================================================================
*/
/*  7.1  */
void initChMap()
{
/*
-----------------------------------------------------------------------------
*/
/*  Data structures */
  SCharEncoding *_sName;

/* Code */
  initMIBenum();

  constrPool(&s_charPool, sizeof(SCharNode));

  /* ASCII */
  /* An empty map is added, because it is an error,
   * to encounter a byte >=128
   */
  addCharacterSet("ASCII", s_ascii_map);

  /* UTF-8 */
  _sName = newSCharEncoding("UTF-8", 0, CF_UTF8, 106);
  SLL_INSERT(s_ListOfNames, _sName);

  /* UTF-16 */
  _sName = newSCharEncoding("UTF-16", 0, CF_UTF16, 1000);
  SLL_INSERT(s_ListOfNames, _sName);

  /* UCS4 */
  _sName = newSCharEncoding("UCS4", 0, CF_UCS4, 1001);
  SLL_INSERT(s_ListOfNames, _sName);

}

/*
=============================================================================
*/
/*  7.2  */
void termChMap()
{
/*
-----------------------------------------------------------------------------
*/
/*  Data structures */

  SCharMap *_map;
  SCharEncoding *_name;

/* Code */

  while(SLL_GET(s_ListOfMaps,_map))
        deleteSCharMap(_map);

  while(SLL_GET(s_ListOfNames,_name))
        deleteSCharEncoding(_name);

  destrPool(&s_charPool);

  termMIBenum();
}

/*
=============================================================================
*/
/*  7.3  */
static SCharEncoding *newSCharEncoding(const char *name,
                                const SCharMap *map,
                                ECharFormat format,
                                unsigned long mibenum)
{
/*
-----------------------------------------------------------------------------
*/
/*  Data structures */

  SCharEncoding  *_retv = XML_ALLOC(sizeof(SCharEncoding)+strlen(name));

/* Code */

  strcpy(_retv->m_name,name);
  _retv->m_map = map;
  _retv->m_next= 0;
  _retv->m_format  = format;
  _retv->m_MIBenum = mibenum;
  return _retv;
}

/*
=============================================================================
*/
/*  7.4  */
static void deleteSCharEncoding(SCharEncoding *sname)
{
/*
-----------------------------------------------------------------------------
*/
/*  Data structures */

/* Code */

  if(sname)
      XML_FREE(sname);
}

/*
=============================================================================
*/
/*  7.5  */
WML_STATUS addAliasName(const char *alias, const char *name)
{
/*
-----------------------------------------------------------------------------
*/
/*  Data structures */

  SCharEncoding *_p = s_ListOfNames.m_root;
  SCharEncoding *_tmp;

/* Code */

  for(; _p; _p=_p->m_next) {
      if(sameName(_p->m_name, alias)) {
          wmlLOGss(WSL_LOG_ERROR, "Add alias: already exists:",alias);
          return WML_ERROR_ENCODING_EXISTS;
      }
  }

  _p = s_ListOfNames.m_root;
  for(; _p; _p=_p->m_next) {
      if(sameName(_p->m_name, name)) {
          _tmp = newSCharEncoding(alias, _p->m_map, _p->m_format,_p->m_MIBenum);
          SLL_INSERT(s_ListOfNames, _tmp);
          wmlLOGsss(WSL_LOG_INFO, alias,"is alias of",name);
          return WML_OK;
      }
  }
  /* doesn't exist */
  wmlLOGss(WSL_LOG_ERROR, "Add alias: does not exist: ",name);
  return WML_ERROR_INVALID_ENCODING;
}

/*
=============================================================================
*/
/*  7.6  */
WML_STATUS addCharacterSet(const char *name, const UINT16 *arr)
{
/*
-----------------------------------------------------------------------------
*/
  /* Data structures */
  const UINT16 *_p = arr;
  SCharMap *_cMap;
  SCharEncoding *_sName;
  unsigned long _mibenum;

  /* Code */
  while(*_p>=128U)
      _p++;

  if(*_p) {
      wmlLOGs(WSL_LOG_ERROR, "AddCharEnc: invalid map");
      return WML_ERROR_INVALID_MAP;
  }
  _sName = s_ListOfNames.m_root;
  for(; _sName; _sName=_sName->m_next) {
      if(sameName(_sName->m_name, name)) {
          wmlLOGss(WSL_LOG_ERROR, "AddCharEnc: already exists:",name);
          return WML_ERROR_ENCODING_EXISTS;
      }
  }

  if(!(_mibenum=getMIBenum(name))) {
          wmlLOGss(WSL_LOG_ERROR, "AddCharEnc: has no MIBenum:",name);
          return WML_ERROR_NO_MIBENUM;
  }

  _cMap  = newSCharMap(arr);
  _sName = newSCharEncoding(name, _cMap, CF_BYTE, _mibenum);
  SLL_INSERT(s_ListOfMaps,  _cMap);
  SLL_INSERT(s_ListOfNames, _sName);
  wmlLOGss(WSL_LOG_INFO,"Character set is added: ", name);
  return WML_OK;
}

/*
=============================================================================
*/
/*  7.7  */
SCharEncoding * getCharEncoding(const char *name)
{
/*
-----------------------------------------------------------------------------
*/
/*  Data structures */

  SCharEncoding *_p = s_ListOfNames.m_root;

/* Code */

  for(; _p; _p=_p->m_next) {
      if(sameName(_p->m_name, name))
          return _p;
  }
  return 0;
}

/*
=============================================================================
*/
/*  7.8  */
static SCharMap *newSCharMap(const UINT16 *map)
{
/*
-----------------------------------------------------------------------------
*/
/*  Data structures */

  const UINT16    *_p         = map;
  SCharMap  *_retv      = XML_ALLOC(sizeof(SCharMap));
  SCharNode *_chNode;

/* Code */

  memset(_retv,0,sizeof(SCharMap));
  _retv->m_nByte = 1;
  for(; *_p; _p+=2) {
      if(*_p>0xffU)
          _retv->m_nByte = 2;
      POOL_ALLOC(&s_charPool,_chNode);
      _chNode->m_from = *_p;
      _chNode->m_to   = *(_p+1);
      _chNode->m_next = 0;
      SLL_INSERT(_retv->m_fromBC[(*_p)&0xff],_chNode);

      POOL_ALLOC(&s_charPool,_chNode);
      _chNode->m_from = *(_p+1);
      _chNode->m_to   = *_p;
      _chNode->m_next = 0;
      SLL_INSERT(_retv->m_fromUC[(*(_p+1))&0xff],_chNode);
  }

  return _retv;
}

/*
=============================================================================
*/
/*  7.9  */
static void deleteSCharMap(SCharMap *map)
{
/*
-----------------------------------------------------------------------------
*/
/*  Data structures */

  int _i;
  SCharNode *_chNode;

/* Code */

  if(!map)
      return;

  for(_i=0; _i<256; _i++) {
      while(SLL_GET(map->m_fromBC[_i],_chNode)) {
          POOL_FREE(&s_charPool,_chNode);
      }
  }
  for(_i=0; _i<256; _i++) {
      while(SLL_GET(map->m_fromUC[_i],_chNode)) {
          POOL_FREE(&s_charPool,_chNode);
      }
  }
  XML_FREE(map);
}

/*
=============================================================================
*/
/*  7.10  */
WCHAR getChFromList(SCharNode *node,WCHAR ch)
{
/*
-----------------------------------------------------------------------------
*/
/*  Data structures */

/* Code */

  while(node && node->m_from!=ch)
      node = node->m_next;
  return (node && (node->m_from==ch)) ? node->m_to : 0;
}

/*
=============================================================================
*/
/*  7.11  */
static int sameName(const char *s1, const char *s2)
{
/*
-----------------------------------------------------------------------------
*/
/*  Data structures */

/* Code */

  if( !s1 || !s2 )
      return 0;

  while(*s1 && *s2 && (toupper(*s1)==toupper(*s2)) )
      ++s1, ++s2;

  return !(*s1|*s2);
}
/*
=============================================================================
*/


/*  End of File  */

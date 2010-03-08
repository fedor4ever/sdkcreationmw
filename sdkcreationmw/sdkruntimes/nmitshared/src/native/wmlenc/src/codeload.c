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
/* File    : U:\wpsvob\wmlenc\src\common\codeload.c
   Version : \main\10
   Date    : 17-Oct-00.12:46:05
*/

/*
 * ========================================================================
 *  Name        : codeload.c
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

/*  5  LOCAL CONSTANTS AND MACROS  */

/*  6  MODULE DATA STRUCTURES  */

/*  6.1 Local Data Structures  */

typedef struct 
{
  const char    * m_name;
  FHandler        m_handler;
}SHdlrTable;
static SHdlrTable s_aHandlers[]= {
    { "NUMBER"      ,       handlerNumber      },
    { "NOTZERO"     ,       handlerNotZero     },
    { "LENGTH"      ,       handlerLength      },
    { "VDATA"       ,       handlerVdata       },
    { "URL"         ,       handlerURL         },
    { "HREF"        ,       handlerURL         },
    { "ID"          ,       handlerCDATA       },
    { "NMTOKEN"     ,       handlerCDATA       },
    { "CDATA"       ,       handlerCDATA       },
    { "BOOLEAN"     ,       handlerCDATA       },
    { "ENUM"        ,       handlerCDATA       },
    { "IGNORE"      ,       handlerIgnore      },
    { "ONEVENTTYPE" ,       handlerOnEventType },
    { "HREF_EVENT"  ,       handlerURLEvent    },
    { "DO_TYPE"     ,       handlerDoType      },
    { "DO_NAME"     ,       handlerDoName      },
    { "VAR_NAME"    ,       handlerVarName     },
    { "CDATA2"      ,       handlerCDATA2      },
    { "SI_CDATA"    ,       siHandlerCDATA     },
    { "SI_DATETIME" ,       siHandlerDateTime  },
    { "SI_HREF"     ,       siHandlerHREF      },
    { 0             ,       0                  }
};
static SHdlrTable s_eHandlers[]= {
    { "DEFAULT"     ,       handlerDefaultElement },
    { "META"        ,       handlerMeta        },
    { "ONCE"        ,       handlerOnce        },
    { "ACCESS"      ,       handlerOnce        },
    { "ONETASK"     ,       handlerOneTask     },
    { "ANCHOR"      ,       handlerOneTask     },
    { "TASK"        ,       handlerTask        },
    { "IGNORE"      ,       handlerIgnore      },
    { "EVENTS"      ,       handlerEvents      },
    { "OPTION"      ,       handlerEvents      },
    { "DO"          ,       handlerDo          },
    { "CARD"        ,       handlerCard        },
    { "TEMPLATE"    ,       handlerCard        },
    { "META2"       ,       handlerMeta2       },
    { "PRE"         ,       handlerPre         },
    { 0             ,       0                  }
};


/*  6.2 Local Function Prototypes  */


/*  7 MODULE CODE  */
#define SINSERT(list,node) insert(&(list),node)

/*
=============================================================================
 */
/*  7.1  */
static void insert(SLIST(SWmlNode) *list,
           SWmlNode        *node)
{
  /* Data structures */
  SWmlNode *prev= 0;
  SWmlNode *n   = list->m_root;
  UINT16    len = STR_LEN(node->m_name);

  for(; n && STR_LEN(n->m_name)>len; prev=n, n=n->m_next)
      ;

  if(!prev) {
      node->m_next = list->m_root;
      list->m_root = node;
  }
  else {
      node->m_next = prev->m_next;
      prev->m_next = node;
  }
}

/*
=============================================================================
 */
/*  7.2  */
FHandler attrHandlerOf(PString name)
{
/*
 * Lookup for 'name' in table, and return the corresponding
 * handler, or NULL.
 *
 *---------------------------------------------------------------------------
 */
  /* Data structures */
  unsigned   i, j;
  WCHAR   c;

  /* Code */

  for(i=0; s_aHandlers[i].m_name; i++) {
      for(j=0; j<STR_LEN(name); j++) {
          c = s_aHandlers[i].m_name[j];
          if((STR_DATA(name)[j] != c) && (STR_DATA(name)[j] != (c+('a'-'A'))))
             break;
      }
      if(j==STR_LEN(name) && (!s_aHandlers[i].m_name[j]))
          return s_aHandlers[i].m_handler;
  }
  return 0;
}
/*
=============================================================================
 */
/*  7.3  */
FHandler elementHandlerOf(PString name)
{
/*
 * Lookup for 'name' in table, and return the corresponding
 * handler, or NULL.
 *
 *---------------------------------------------------------------------------
 */
  /* Data structures */
  unsigned   i, j;
  WCHAR   c;

  /* Code */

  for(i=0; s_eHandlers[i].m_name; i++) {
      for(j=0; j<STR_LEN(name); j++) {
          c = s_eHandlers[i].m_name[j];
          if((STR_DATA(name)[j] != c) && (STR_DATA(name)[j] != (c+('a'-'A'))))
             break;
      }
      if(j==STR_LEN(name) && (!s_eHandlers[i].m_name[j]))
          return s_eHandlers[i].m_handler;
  }
  return (FHandler)handlerDefaultElement;
}

/*
=============================================================================
 */
/*  7.4  */
SWmlNode * newSWmlNode( PString         name,       /* Name of the node     */
                        UINT32          code,       /* Code with code page  */
                        FHandler        handler,    /* handler function     */
                        SWmlDescriptor *desc)       /* for pool             */
{
  SWmlNode *_retv = XML_ALLOC(sizeof(SWmlNode));

  memset(_retv,0,sizeof(SWmlNode));
  _retv->m_name     = strXDup(name, &(desc->m_pool));
  _retv->m_code     = (UINT16)code;
  _retv->m_handler  = handler;

  return _retv;
}

/*
=============================================================================
 */
/*  7.5  */
void deleteSWmlNode(SWmlNode       *node,
				   SWmlDescriptor *desc)
{
  SWmlNode *_child; 

  if(node) {
      while(SLL_GET(node->m_children,_child)) {
          deleteSWmlNode(_child,desc);
      }
      deleteString(node->m_name, &(desc->m_pool));
      XML_FREE(node);
  }
}



/*
=============================================================================
 */
/*  7.6  */
void skipWS(const char **p,
            const char *endp)
{
  for( ; *p<endp && isspace(**p) && (**p!='\n');  (*p)++ )
      ;
}
/*
=============================================================================
 */
/*  7.7  */
void skipRestOfTheLine(const char **p,
              const char *endp)
{
  for( ; *p<endp && (**p!='\n');  (*p)++ )
      ;
  if(*p<endp)
      (*p)++;
}
/*
=============================================================================
 */
/*  7.8  */
void readWord(const char **p,
              const char *endp,
              PString     str)
{
/*  Data Structures  */

/*  Code  */
  STR_CLEAR(str);

  skipWS(p,endp);

  for(; (*p<endp) && (!isspace(**p)); (*p)++)
      strAppendChar(str, (WCHAR)**p );
}
/*
=============================================================================
 */
/*  7.9  */
UINT32 readHexNumber(const char **p,
                     const char *endp)
{
/*  Data Structures  */
  UINT32 _num = 0;

/*  Code  */

  skipWS(p,endp);

  for(; (*p<endp) && (!isspace(**p)); (*p)++) {           /* read the code */
      if( **p>='0' && **p<='9' ) {
          _num*=16;
          _num+=(**p-'0');
      }
      else if(**p>='a' && **p<='f') {
          _num*=16;
          _num+=(**p-'a'+10);
      }
      else if(**p>='A' && **p<='F') {
          _num*=16;
          _num+=(**p-'A'+10);
      }
      else {
          break;
      }
  }
  return _num;
}

/*
=============================================================================
 */
/*  7.10  */
int isGoodName(const PString name)
{
/*
 *  Return: zero if name is empty or equal with "-"
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/*  Code  */

  if(   (!name)
     || (!STR_LEN(name))
     || ((STR_LEN(name)==1) && (*(STR_DATA(name))==(WCHAR)'-'))) {
      return 0;
  }
  return 1;
}
/*
=============================================================================
 */
/*  7.11  */
SWmlNode *getElement(SWmlDescriptor * desc,
                     const PString    name)
{
/*
 *  Assume: name is not empty.
 *  Return: the founded node or NULL.
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */
  unsigned   _idx  = (*STR_DATA(name))&0x7f;
  SWmlNode * _node = FIRST_NODE(desc->m_elements[_idx]);

/*  Code  */

  while(_node && !STR_SAME(_node->m_name, name))
      _node = _node->m_next;

  return _node;
}
/*
=============================================================================
 */
/*  7.12  */
SWmlNode *getAttribute(SWmlDescriptor * desc,
                       const PString    name)
{
/*
 *  Assume: name is not empty.
 *  Return: the founded node or NULL.
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */
  unsigned   _idx  = (*STR_DATA(name))&0x7f;
  SWmlNode * _node = FIRST_NODE(desc->m_attributes[_idx]);

/*  Code  */

  while(_node && !STR_SAME(_node->m_name, name))
      _node = _node->m_next;

  return _node;
}

/*
=============================================================================
 */
/*  7.13  */
int readTag(SWmlDescriptor * desc,
            const char     * p,
            const char     * endp)
{
/*
 *
 *  Return Values:  
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */
  int         _retv = 0;
  SWmlNode *  _node = 0;
  UINT32      _code = 0;
  FHandler _handler = 0;
  PString     _name = newString(&(desc->m_pool));
  PString     _func = newString(&(desc->m_pool));

/*  Code  */

  readWord(&p, endp, _name);            /* name                 */
  _code = readHexNumber(&p, endp);      /* code with code page  */
  readWord(&p, endp, _func);            /* function             */
  
  if(STR_LEN(_func)) {
          _handler = elementHandlerOf(_func);
  }
  else {
      _handler = handlerDefaultElement;
  }


  if(STR_LEN(_name) && (_code&0xff) && _handler) {
      _node = newSWmlNode(_name, _code, _handler, desc);
      SINSERT(desc->m_elements[ (*(STR_DATA(_name)))&0x7f], _node);
      DBG_LOG2( (DBG_WML, _name, "TAG (%x) default:", _code) );
      _retv++;
  }

  skipRestOfTheLine(&p, endp);

  deleteString(_func, &(desc->m_pool));
  deleteString(_name, &(desc->m_pool));

  return _retv;
}

/*
=============================================================================
 */
/*  7.14  */
void copyChildren(SWmlDescriptor * desc,
                  const SWmlNode * from,
                  SWmlNode       * to)
{
/*
 * Search for attr in global tables of attributes, and copy
 * the corresponding prefixes into attr.
 *
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */
  SWmlNode        * _child;
  
/*  Code  */

  from = FIRST_NODE(from->m_children);

  for(; from; from=from->m_next) {
      _child = newSWmlNode(from->m_name, from->m_code, 0, desc);
      SINSERT(to->m_children, _child);
  }
}
/*
=============================================================================
 */
/*  7.15  */
int readContext(SWmlDescriptor * desc,
                const char     * p,
                const char     * endp)
{
/*
 *
 *  Return Values:  
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */
  int         _retv  = 0;
  SWmlNode  * _enode = 0;
  SWmlNode  * _anode = 0;
  SWmlNode  * _node  = 0;
  FHandler  _handler = 0;
  PString     _ename = newString(&(desc->m_pool));
  PString     _aname = newString(&(desc->m_pool));
  PString     _fname = newString(&(desc->m_pool));


/*  Code  */

  /* parse */
  readWord(&p, endp, _ename); /* element name       */
  readWord(&p, endp, _aname); /* attribute name     */
  readWord(&p, endp, _fname); /* handler name       */

  _handler = attrHandlerOf(_fname);
  _enode   = getElement(desc, _ename);
  _anode   = getAttribute(desc, _aname);

  if( _enode && _anode && _handler) {

      _node = newSWmlNode(_aname, _anode->m_code, _handler, desc);
      SINSERT(_enode->m_children, _node);
      copyChildren(desc, _anode, _node);
      _retv++;
  }
  else {
      DBG_LOG2( (DBG_WML, _aname, "DROPPED  : ") );
      /* wmlLog(WSL_LOG_ERROR, ""); */
      /* TODO: indicate the error */
  }



  skipRestOfTheLine(&p, endp);

  deleteString(_fname, &(desc->m_pool));
  deleteString(_aname, &(desc->m_pool));
  deleteString(_ename, &(desc->m_pool));

  return _retv;
}

/*
=============================================================================
 */
/*  7.16  */
int readAttr(SWmlDescriptor * desc,
             const char     * p,
             const char     * endp)
{
/*
 *  Parse the line as "Name Prefix Code Handler".
 *  Return Values:  
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */
  int         _retv    = 0;
  UINT32      _code    = 0;
  PString     _name    = newString(&(desc->m_pool));
  PString     _prefix  = newString(&(desc->m_pool));
  PString     _func    = newString(&(desc->m_pool));
  FHandler    _handler = 0;
  SWmlNode  * _node    = 0;

/*  Code  */

  /* parse */
  readWord(&p, endp, _name);            /* name                 */
  readWord(&p, endp, _prefix);          /* prefix               */
  _code = readHexNumber(&p, endp);      /* code with code page  */
  readWord(&p, endp, _func);            /* function             */
  _handler = attrHandlerOf(_func);

  if(STR_LEN(_name)
     && STR_LEN(_prefix)
     && ((_code && _handler) || ((!_code) && _handler==handlerIgnore)) )  {
      SWmlNode * _attr = getAttribute(desc, _name);

      if(!_attr) {                      /* new attribute    */
          if(!isGoodName(_prefix)) {    /* without prefix   */
              _attr = newSWmlNode(_name, _code, _handler, desc);
              SINSERT(desc->m_attributes[ (*(STR_DATA(_name)))&0x7f], _attr);
          }
          else {                        /* with prefix */
              _attr = newSWmlNode(_name, 0, _handler, desc);
              SINSERT(desc->m_attributes[ (*(STR_DATA(_name)))&0x7f], _attr);
              _node = newSWmlNode(_prefix, _code, 0, desc);
              SINSERT(_attr->m_children, _node);
          }
      }
      else {                            /* new prefix */
          _node = newSWmlNode(_prefix, _code, 0, desc);
          SINSERT(_attr->m_children, _node);
      }
      _retv++;
  }


  skipRestOfTheLine(&p, endp);

  deleteString(_func,   &(desc->m_pool));
  deleteString(_prefix, &(desc->m_pool));
  deleteString(_name,   &(desc->m_pool));

  return _retv;
}

/*
=============================================================================
 */
/*  7.17  */
int readGenValue(SWmlDescriptor * desc,
                 const char     * p,
                 const char     * endp)
{
/*
 *
 *  Return Values:  
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */
  int         _retv  = 0;
  SWmlNode  * _node  = 0;
  UINT32      _code  = 0;
  PString     _value = newString(&(desc->m_pool));

/*  Code  */

  readWord(&p, endp, _value);           /* value                */

  _code = readHexNumber(&p, endp);      /* code with code page  */

  if(STR_LEN(_value)) {
      _node = newSWmlNode(_value, _code, 0, desc);
      SINSERT(desc->m_generalValues[ (*(STR_DATA(_value)))&0x7f], _node);
      _retv++;
  }

  DBG_LOG2( (DBG_WML, _value, "GEN VAL (%x): ", _code) );

  skipRestOfTheLine(&p, endp);

  deleteString(_value, &(desc->m_pool));

  return _retv;
}

/*
=============================================================================
 */
/*  7.18  */
SWmlDescriptor * newSWmlDescriptor(const char *publicId,
                                   const char *text,
                                   UINT32 length)
{
/*
 *
 *  Return Values:  
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */
  const char     * _endp = text+length;
  SWmlDescriptor * _desc = XML_ALLOC(sizeof(SWmlDescriptor));
  int               _ok  = 1;
  int               _line= 1;

  memset(_desc,0,sizeof(SWmlDescriptor));
  constrPool(&(_desc->m_pool), sizeof(struct String_s));

  _desc->m_pubId = newString(&(_desc->m_pool));
  while( publicId && *publicId) {
      strAppendChar(_desc->m_pubId, *publicId++);
  }

  while(_ok && text<_endp) {
      switch(*text++) {
         case 'T' : _ok = readTag(_desc, text, _endp);
                    break;
         case 'A' : _ok = readAttr(_desc, text, _endp);
                    break;
         case 'C' : _ok = readContext(_desc, text, _endp);
                    break;
         case 'V' : _ok = readGenValue(_desc, text, _endp);
                    break;
         case '\n': break;
         default  : skipRestOfTheLine(&text, _endp);
                    break;
      }
      if(!_ok) {
          wmlLOGs(WSL_LOG_ERROR, "Token-set specification error");
          deleteSWmlDescriptor(_desc);
          _desc = 0;
          break;
      }
      _line++;
  }

  return _desc;
}

/*
=============================================================================
 */
/*  7.19  */
void deleteSWmlDescriptor(SWmlDescriptor *desc)
{
/*  Data Structures  */
  int      i;
  SWmlNode *_node;

/*  Code  */
  if(desc) {
      for(i=0; i<128; i++) {
          while(SLL_GET(desc->m_elements[i],_node)) {
              deleteSWmlNode(_node, desc);
          }
          while(SLL_GET(desc->m_attributes[i],_node)) {
              deleteSWmlNode(_node, desc);
          }
          while(SLL_GET(desc->m_generalValues[i],_node)) {
              deleteSWmlNode(_node, desc);
          }
      }
      deleteString(desc->m_pubId,&(desc->m_pool));

      destrPool(&(desc->m_pool));
      XML_FREE(desc);
  }
}


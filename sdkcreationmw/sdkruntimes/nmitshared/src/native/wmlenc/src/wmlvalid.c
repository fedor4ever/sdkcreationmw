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
/* File    : U:\wpsvob\wmlenc\src\common\wmlvalid.c
   Version : \main\12
   Date    : 17-Oct-00.12:48:09
*/

/*
 * ========================================================================
 *  Name        : wmlvalid.c
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
#include "wbxml_config.h"

/*  4.2 External Data Structures  */

    /* None */

/*  4.3 External Function Prototypes  */

    /* None */

/*  5  LOCAL CONSTANTS AND MACROS  */

#define EVENT        data->m_eventList.m_head
#define GUESS   ((EVENT && (*(STR_DATA(EVENT->m_tokenValue))==(WCHAR)'W')) ? \
                 (&s_pubId0) : (&s_pubId1))

/*  6  MODULE DATA STRUCTURES  */

/*  6.1 Local Data Structures  */

union UWmlObject
{
  SWmlString    m_x;
  SWmlToken     m_y;
  SXmlEvent     m_z;
  Segment       m_s;
  struct String_s m_ss;
};

static SLIST(SWmlDescriptor)   s_list;
SWmlDescriptor * s_emptyDescriptor;


static SWmlNode         s_unknownElement;
static SWmlNode         s_unknownAttribute;
static struct String_s  s_ueName;
static struct String_s  s_uaName;


SWmlNode * g_unknownElement   = &s_unknownElement;
SWmlNode * g_unknownAttribute = &s_unknownAttribute;

/*  6.2 Local Function Prototypes  */

static int isSame ( PString s1,
                    const char *s2 );

static void getWmlDescriptor (SDocData * data,
                              UINT32 *   wbxmlVersion);

static void insertDtdId(SDocData   * data );

/*  7 MODULE CODE  */

/*
=============================================================================
*/


/*
=============================================================================
 */
/*  7.1  */
WML_STATUS codeLoad(char   * publicId,
                    char   * content,
                    UINT32   contentSize)
{
/*  Load tokenset form 'content' for 'publicId'.
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */
  SWmlDescriptor * _desc;

/* Code */
  _desc = newSWmlDescriptor(publicId, content, contentSize);

  if(!_desc) {
      return WML_ERROR_SPEC_FAILURE;
  }

  SLL_INSERT(s_list, _desc);

  return WML_OK;
}

/*
=============================================================================
 */
/*  7.2  */
SWmlDescriptor * lookupWmlDescriptor(PString pubId)
{
/*  Search for tokenset for 'pubId'.
 *  Return the tokenset or NULL.
 *---------------------------------------------------------------------------
 */
/*  Data Structures  */
  SWmlDescriptor * _desc;

/* Code */
  for(_desc=FIRST_NODE(s_list); _desc ; _desc=_desc->m_next) {
      if(STR_SAME(pubId,_desc->m_pubId)) {
         break;
      }
  }
  return _desc;
}

/*
=============================================================================
 */
/*  7.3  */
WML_STATUS initWML(WML_DTD_DESCRIPTOR *desc[])
{
/*  Initialize the global WML structures.
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/* Code */

  constrString(&s_ueName);
  constrString(&s_uaName);

  s_unknownElement.m_next = 0;
  s_unknownElement.m_name = &s_ueName;
  s_unknownElement.m_code = GT_LITERAL;
  s_unknownElement.m_handler = handlerDefaultElement;
  s_unknownElement.m_children.m_root = 0;

  s_unknownAttribute.m_next = 0;
  s_unknownAttribute.m_name = &s_uaName;
  s_unknownAttribute.m_code = GT_LITERAL;
  s_unknownAttribute.m_handler = handlerUnknownAttr;
  s_unknownAttribute.m_children.m_root = 0;

  s_emptyDescriptor = newSWmlDescriptor(" ", "", 0);
  return WML_OK;
}

/*
=============================================================================
 */
/*  7.4  */
void termWML()
{
/*  Terminate the global WML structures.
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */
  SWmlDescriptor *_desc;

/* Code */

  deleteSWmlDescriptor( s_emptyDescriptor );

  while( SLL_GET(s_list,_desc) ) {
      deleteSWmlDescriptor( _desc );
  }
  destrString(&s_ueName);
  destrString(&s_uaName);
}

/*
=============================================================================
 */
/*  7.5  */
WML_STATUS isValidWMLDocument(SDocData   * data)
{
/*  Search for a tokenset for the document, and validate it.
 *  Return the status.
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */
    UINT32  wbxmlVersion;

/*  Code */
    if ( !EVENT ) {
        setWmlError(WML_ERROR_NO_TOKENS, data->m_dtdPUBID, data);
    }
    else {

        getWmlDescriptor (data, &wbxmlVersion);

        insertDtdId(data);

        handlerRootElement(data);
        convertToWBXML(data, wbxmlVersion);

        if(data->m_mAvailSiz) {
            *data->m_arraySize    = data->m_mCounter;
            *data->m_stringLength = data->m_mMaxLen+1;
            if( data->m_mCounter > data->m_mAvailSiz) {
                setWmlError(WML_ERROR_MANY_META,0,data);
            }
            if(data->m_mMaxLen  >= data->m_mAvailLen) {
                setWmlError(WML_ERROR_LONG_META,0,data);
            }
        }
    }

    return data->m_error;
}

/*
=============================================================================
 */
/*  7.6  */
SDocData * newSDocData(const char*  oEncoding,  
                       UINT32 *     oBufferSize,
                       BYTE *       oBuffer,    
                       int *        arraySize,
                       UINT16 **    metaHttpEquiv,
                       int *        stringLength, 
                       UINT16 **    metaContent,
                       const BYTE * docURL)
{
/*  Allocate and initialize 'SDocData' structure for document.
 *  Return the created structure.
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */
  SDocData * _retv = XML_ALLOC(sizeof(SDocData));

/*  Code */
  memset(_retv,0,sizeof(SDocData));
  constrPool(&_retv->m_pool, sizeof(union UWmlObject));
  constrArray(&_retv->m_array);

  if(!constrSCCOut(&_retv->m_scc, &_retv->m_array, oEncoding)) {
      _retv->m_error = WML_ERROR_CONVERSION_FAILED;
  }

  _retv->m_oBufferSize    = oBufferSize   ;
  _retv->m_oBuffer        = oBuffer       ;
  _retv->m_arraySize      = arraySize     ;
  _retv->m_metaHttpEquiv  = metaHttpEquiv ;
  _retv->m_stringLength   = stringLength  ;
  _retv->m_metaContent    = metaContent   ;

  if(   (!arraySize || !metaHttpEquiv || !stringLength || !metaContent)
     || (!*arraySize || !*stringLength) ) {
      _retv->m_mAvailSiz = 0;
      _retv->m_mAvailLen = 0;
      _retv->m_mMaxLen   = 0;
      _retv->m_mCounter  = 0;
  }
  else {
      _retv->m_mAvailSiz  = *arraySize;
      _retv->m_mAvailLen  = *stringLength;
      _retv->m_mMaxLen    = 0;
      _retv->m_mCounter   = 0;
      *arraySize     = 0;
      *stringLength  = 0;
  }

  _retv->m_docURL = docURL;

  constrUniq(&(_retv->m_uniqInDeck), _retv);
  constrUniq(&(_retv->m_uniqEvent),  _retv);
  constrUniq(&(_retv->m_uniqDo),     _retv);

  return _retv;
}

/*
=============================================================================
 */
/*  7.7  */
void  deleteSDocData(SDocData *data)
{
/*  Terminate and free the document related data structure.
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */
  SXmlEvent  *_event; 
  SWmlToken  *_tokNode; 
  SWmlString *_strNode;
  int         _i;

/*  Code */
  if(data) {

      if(data->m_mAvailSiz) {
          *data->m_arraySize    = data->m_mCounter; 
          *data->m_stringLength = data->m_mMaxLen+1;
      }

      deleteString(data->m_dtdPUBID, DOCPOOL);

      while(FIFO_GET(data->m_eventList,_event)) {
          deleteSXmlEvent(_event, data);
      }

      while(FIFO_GET(data->m_tokenList,_tokNode)) {
          POOL_FREE(DOCPOOL, _tokNode);
      }

      for(_i=0; _i<128; _i++) {
          while(SLL_GET(data->m_stringTable[_i],_strNode)) {
              deleteString(_strNode->m_string, DOCPOOL);
              POOL_FREE(DOCPOOL, _strNode);
          }
      }


      destrUniq(&(data->m_uniqInDeck), data);
      destrUniq(&(data->m_uniqEvent),  data);
      destrUniq(&(data->m_uniqDo),     data);

      destrSCCOut(&data->m_scc);
      destrArray(&data->m_array);
	  destrPool(&data->m_pool);
      XML_FREE(data);
  }
}

/*
=============================================================================
 */
/*  7.8  */
void setWmlPubId(PString   str,
                 SDocData *data)
{
/*  Attach a PUBLIC ID (string) to the document.
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */
/*  Code */
  DBG_ASSERT(data && str);

  deleteString(data->m_dtdPUBID,DOCPOOL);
  data->m_dtdPUBID = strXDup(str, DOCPOOL);
}

/*
=============================================================================
 */
/*  7.9  */
SXmlEvent * newSXmlEvent(EXmlToken tok,
                         PString   str,
                         SDocData *data)
{
/*  Allocate and initialize a new event for document.
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */
/*  Code */
  SXmlEvent * _event;

  POOL_ALLOC(DOCPOOL, _event);
  _event->m_token      = tok;
  _event->m_tokenValue = strXDup(str,DOCPOOL);

  return _event;
}

/*
=============================================================================
 */
/*  7.10  */
void  deleteSXmlEvent(SXmlEvent *event,
                      SDocData  *data)
{
/*  Free the document related event.
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */
/*  Code */
  if(event) {
      deleteString(event->m_tokenValue, DOCPOOL);
      POOL_FREE(DOCPOOL, event);
  }
}

/*
=============================================================================
 */
/*  7.11  */
static int isSame ( PString s1,
                    const char *s2 )
{
    UINT16 len;
    const WCHAR* p;

    if ( !s1 || !s2 || !*STR_DATA(s1) || !*s2 )
        return 0;

    len = STR_LEN(s1);
    p   = STR_DATA(s1);

    for (; len; len--, p++, s2++) {
        if ( !*s2 || (*p != (WCHAR)*s2) )
            return 0;
    }

    return !*s2;
}

/*
=============================================================================
 */
/*  7.12  */
static void getWmlDescriptor (SDocData * data,
                              UINT32 *   wbxmlVersion)
{
    const int  n = sizeof(wbxml_cfg_table)/sizeof(wbxml_cfg_table[0]);
    int        i;

    data->m_desc = 0;

    if( data->m_dtdPUBID && STR_LEN(data->m_dtdPUBID) ) {

        if ( (data->m_desc=lookupWmlDescriptor(data->m_dtdPUBID)) ) {

            for (i=0; i<n; i++) {
                if (isSame(data->m_dtdPUBID, wbxml_cfg_table[i].pubIdStr)) {
                    data->m_dtdPUBIDCode = wbxml_cfg_table[i].pubIdCode;
                    *wbxmlVersion        = wbxml_cfg_table[i].wbxmlVersion;
                    return;
                }
            }
        }

    } else {
        for (i=0; i<n; i++) {
            if (isSame(EVENT->m_tokenValue,
                       wbxml_cfg_table[i].defaultForRootElement)) {

                SWmlDescriptor * _desc;
                for(_desc=FIRST_NODE(s_list); _desc ; _desc=_desc->m_next) {
                    if(isSame(_desc->m_pubId,wbxml_cfg_table[i].pubIdStr)) {
                        data->m_desc = _desc;
                        data->m_dtdPUBIDCode = wbxml_cfg_table[i].pubIdCode;
                        *wbxmlVersion        = wbxml_cfg_table[i].wbxmlVersion;
                        return;
                    }
                }
            }
        }
    }

    *wbxmlVersion = 1;
    for (i=0; i<n; i++) {
        if ( wbxml_cfg_table[i].pubIdCode==1 ) {
            *wbxmlVersion = wbxml_cfg_table[i].wbxmlVersion;
            break;
        }
    }

    if ( !data->m_desc ) {
        data->m_desc         = s_emptyDescriptor;
    }
    data->m_dtdPUBIDCode = 1;
}


/*
=============================================================================
 */
/*  7.13  */
static void insertDtdId(SDocData   * data )
{
/*  
 * Append DTD ID string into the string table of the output binary stream,
 * according to the output character encoding, if it represents an
 * unknown pubid.
 *---------------------------------------------------------------------------
 */
/* Data Structures */
  UINT32  _n;                               /* string length            */
  WCHAR  *_s;                               /* ptr to string            */
  SCCOut *_scc    = &data->m_scc;           /* character converter      */
  UINT32  _offset = data->m_array.m_size;   /* save the current offset  */
  int     _failed = 0;                      /* assume: success          */

  if (     (data->m_dtdPUBIDCode!=1)
       || (!data->m_dtdPUBID)
       || (!(STR_LEN(data->m_dtdPUBID)))
       || (_offset) ) {

      return;
  }

/* Code */

  /* Convert and copy */

  _n = STR_LEN(data->m_dtdPUBID);
  _s = STR_DATA(data->m_dtdPUBID);

  for( ; _n; _n--,_s++) {
      _scc->m_char = *_s;

      if(!(_scc->m_convert(_scc))) {
          _failed = 1;
          break;
      }
  }

  _scc->m_char = 0;
  _scc->m_convert(_scc);                /* terminating zero */

  if ( !_failed ) {
      data->m_dtdPUBIDCode = 0;
  }
}

/*
=============================================================================
*/


/*  End of File  */

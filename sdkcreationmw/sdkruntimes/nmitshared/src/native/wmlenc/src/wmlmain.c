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
/* File    : U:\wpsvob\wmlenc\src\common\wmlmain.c
   Version : \main\11
   Date    : 17-Oct-00.12:47:37
*/

/*
 * ========================================================================
 *  Name        : wmlmain.c
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

        Most of the API functions.


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

        7.1   WML_StartUp
        7.2   WML_ShutDown
        7.3   WML_Encode
        7.4   WML_GetDTDID
        7.5   WML_AddCharEnc
        7.6   WML_AddAliasName
        7.7   WML_IsCharEnc
        7.8   WML_AddCodepage
        7.9   WML_SetFeatures
        7.10  ErrorHandler
        7.11  TokenHandler

*/



/*  3  GLOSSARY

        None

*/


/*  4  EXTERNAL RESOURCES  */

    /* None */

/*  4.1  Include Files  */

#include "xmlstr.h"
#include "xmlapi.h"
#include "xmlbuf.h"
#include "wmlapi.h"
#include "chenc.h"
#include "wmlvalid.h"

/*  4.2 External Data Structures  */

    /* None */


/*  4.3 External Function Prototypes  */

    /* None */


/*  5  LOCAL CONSTANTS AND MACROS  */

    /* None */



/*  6  MODULE DATA STRUCTURES  */


/*  6.1 Local Data Structures  */

static int s_isInitialized = 0;     /* sentinel */



/*  6.2 Local Function Prototypes  */

static int  ErrorHandler(XML_STATUS   errorCode,
                         PString      context,
                         void       * usrData);
static int  TokenHandler(EXmlToken  xToken,
                         PString    value,
                         void     * usrData);


/*  7 MODULE CODE  */


/*
=============================================================================
*/
/*  7.1  */
WML_STATUS WML_StartUp(WML_URLLOAD         downloader,
                       WML_DTD_DESCRIPTOR *DTDs[])
{
/*
-----------------------------------------------------------------------------
*/
/*  Data structures */

/* Code */

  if (s_isInitialized++) {
      wmlLOGs(WSL_LOG_ERROR, "Encoder already is initialized !");
      return WML_ERROR_IS_STARTED;
  }

  MEM_DBG_INIT;                 /* doesn't belong to the project */

  initXML();                    /* Initialize the XML parser    */

  initCharEncoding();           /* Initialize character maps    */

  addISOTables();               /* Some character set/encoding  */

  initWML(DTDs);                /* Initialize the WML Validate  */

  return initBufferCache(downloader,DTDs);
}

/*
=============================================================================
*/
/*  7.2  */
void WML_ShutDown(void)
{
/*
-----------------------------------------------------------------------------
*/
/*  Data structures */

/* Code */

  if(!--s_isInitialized) {

      termBufferCache();

      termWML();

      termCharEncoding();

      termXML();
      
      MEM_DBG_TERM;
  }
  if(s_isInitialized<0) {
      s_isInitialized++;
  }
}

/*
=============================================================================
*/
/*  7.3  */
WML_STATUS WML_Encode(UINT32        iBytes,
                      const BYTE *  wmlDoc,
                      const char *  iCharEnc,
                      UINT32     *  oBytes,
                      BYTE       *  binDoc,
                      const char *  oCharEnc,
                      int        *  arraySize,
                      UINT16     ** metaHttpEquiv,
                      int        *  stringLength,
                      UINT16     ** metaContent,
                      const char *  altDTDID,
                      const BYTE *  URLofDoc)
{
/*
-----------------------------------------------------------------------------
*/
/*  Data structures */

  WML_STATUS	rc = WML_OK;
  SEncBuffers _buffers;
  WCHAR*		_uBuffer;
  SDocData   * _pWmlData;
  UINT32       _oSize = 0;

/* Code */

  if(oBytes) {
      _oSize  = *oBytes;
      *oBytes = 0;
  }


  /***** CHECK ARGUMENTS *****/
  if(!s_isInitialized) {
      wmlLOGs(WSL_LOG_ERROR, "Not initialized");
      return WML_ERROR_NOT_INITIALIZED;
  }

  if( !wmlDoc || !binDoc || !iCharEnc || !oCharEnc || !oBytes) {
      wmlLOGs(WSL_LOG_ERROR, "WML_Encode: NULL pointer as argument");
      return WML_ERROR_EMPTY_ARG;
  }

  if(!iBytes ||  !_oSize || (iBytes>=XML_BUFFER_SIZE)) {
      wmlLOGs(WSL_LOG_ERROR, "WML_Encode: Invalid buffer size.");
      return WML_ERROR_INVALID_BUFFER_SIZE;
  }

  if(!getCharEncoding(iCharEnc)) {
      wmlLOGss(WSL_LOG_ERROR,
             "WML_Encode: Character set does not exist: ",iCharEnc);
      return WML_ERROR_INVALID_ENCODING;
  }

  if(!getCharEncoding(oCharEnc)) {
      wmlLOGss(WSL_LOG_ERROR,
             "WML_Encode: Character set does not exist: ",oCharEnc);
      return WML_ERROR_INVALID_ENCODING;
  }

  /***** PREPARE THE BUFFERS *****/
  _uBuffer           = XML_ALLOC(sizeof(WCHAR)*iBytes);
  _buffers.m_enc     = getCharEncoding(iCharEnc);
  _buffers.m_src     = wmlDoc;
  _buffers.m_srcSize = iBytes;
  _buffers.m_dst     = _uBuffer;
  _buffers.m_dstSize = iBytes;

  /***** CONVERT *****/
  if(!convertToUnicode(&_buffers)) {
      XML_FREE(_uBuffer);
      return WML_ERROR_CONVERSION_FAILED;
  }

  /***** DATA STRUCTURES FOR CALLBACK FUNCTIONS *****/
  _pWmlData = newSDocData(oCharEnc,
                          oBytes,
                          binDoc,
                          arraySize,
                          metaHttpEquiv,
                          stringLength,
                          metaContent,
                          URLofDoc);

  /***** XML - WML *****/
  if (parseXML(_pWmlData,
               _uBuffer,
               _buffers.m_dstSize,
               TokenHandler,
               ErrorHandler,
               altDTDID,
               URLofDoc) != 0) {

      if(_pWmlData->m_error) {
          rc = _pWmlData->m_error;
      }
      else if(FIFO_EMPTY(_pWmlData->m_eventList)) {
          rc = WML_ERROR_XMLPARSER_FAILED;
      }
      else {
          *oBytes = _oSize;
          rc = isValidWMLDocument(_pWmlData);
          if(rc!=WML_OK) {
              *oBytes = 0;
          }
      }
  }
  else {
      rc = WML_ERROR_XMLPARSER_FAILED;
      if(_pWmlData->m_error) {
          rc = _pWmlData->m_error;
      }
  }

  /***** RELEASE MEMORY *****/
  deleteSDocData(_pWmlData);

  XML_FREE(_uBuffer);

  return rc;
}

/*
=============================================================================
*/
/*  7.4  */
WML_STATUS WML_GetDTDID(UINT32      iBytes,
                        const BYTE* wmlDoc,
                        const char* iCharEnc,
                        UINT16      idBufLenU16,
                        UINT16    * publicID,
                        UINT16    * systemID)
{
/*
-----------------------------------------------------------------------------
*/
/*  Data structures */

  WML_STATUS	rc = WML_OK;
  SEncBuffers _buffers;
  WCHAR*		_uBuffer;

/* Code */

  /***** CHECK ARGUMENTS *****/
  if(!s_isInitialized) {
      wmlLOGs(WSL_LOG_ERROR, "Not initialized");
      return WML_ERROR_NOT_INITIALIZED;
  }

  if(!wmlDoc || !iCharEnc || !publicID || !systemID) {
      wmlLOGs(WSL_LOG_ERROR, "WML_GetDTDID: NULL pointer as argument");
      return WML_ERROR_EMPTY_ARG;
  }

  if((!iBytes) || (iBytes>=XML_BUFFER_SIZE)) {
      wmlLOGs(WSL_LOG_ERROR, "WML_GetDTDID: Invalid buffer size.");
      return WML_ERROR_INVALID_BUFFER_SIZE;
  }


  if(!getCharEncoding(iCharEnc)) {
      wmlLOGss(WSL_LOG_ERROR, "Unknown character encoding: ",iCharEnc);
      return WML_ERROR_INVALID_ENCODING;
  }

  /***** PREPARE THE BUFFERS *****/
  _uBuffer           = XML_ALLOC((sizeof(WCHAR)*iBytes)+20);
  _buffers.m_enc     = getCharEncoding(iCharEnc);
  _buffers.m_src     = wmlDoc;
  _buffers.m_srcSize = iBytes;
  _buffers.m_dst     = _uBuffer;
  _buffers.m_dstSize = iBytes+(20/sizeof(WCHAR));

  /***** CONVERT *****/
  if(!convertToUnicode(&_buffers)) {
      rc = WML_ERROR_CONVERSION_FAILED;
  }
  /***** XML *****/
  else if(!getDTDID(0,
                    _uBuffer,
                    _buffers.m_dstSize,
                    idBufLenU16,
                    publicID,
                    systemID)) {
      rc = WML_ERROR_XMLPARSER_FAILED;
  }
  else {
      rc = WML_OK;
  }

  /***** RELEASE MEMORY *****/

  XML_FREE(_uBuffer);

  return rc;
}

/*
=============================================================================
*/
/*  7.5  */
WML_STATUS WML_AddCharEnc(const char * charEncName,
                      const UINT16 * charEncMap) 
{
/*
-----------------------------------------------------------------------------
*/
/*  Data structures */


/* Code */

  if(!s_isInitialized) {
      wmlLOGs(WSL_LOG_ERROR, "Not initialized");
      return WML_ERROR_NOT_INITIALIZED;
  }

  if(!charEncName || !charEncMap || !*charEncName) {
      wmlLOGs(WSL_LOG_ERROR, "WML_AddCharEnc: NULL pointer as argument");
      return WML_ERROR_EMPTY_ARG;
  }

  return addCharacterSet(charEncName, charEncMap);
}

/*
=============================================================================
*/
/*  7.6  */
WML_STATUS WML_AddAliasName(const char *charEncName,
                        const char *aliasName)
{
/*
-----------------------------------------------------------------------------
*/
/*  Data structures */

/* Code */

  if(!s_isInitialized)
      return WML_ERROR_NOT_INITIALIZED;

  if(!charEncName || !aliasName || !*charEncName || !*aliasName)
      return WML_ERROR_EMPTY_ARG;

  return addAliasName(aliasName,charEncName);
}

/*
=============================================================================
*/
/*  7.7  */
BOOL WML_IsCharEnc(const char *charEncName)
{
/*
-----------------------------------------------------------------------------
*/
/*  Data structures */

/* Code */

  if(!s_isInitialized || !charEncName || !*charEncName) {
      return FALSE;
  }

  return getCharEncoding(charEncName) ? TRUE : FALSE;
}

/*
=============================================================================
*/
/*  7.8  */
WML_STATUS  WML_AddCodepage(char    * publicId,
                            UINT32    contentSize,
                            char    * content)
{
/*
-----------------------------------------------------------------------------
*/
/*  Data structures */

/* Code */

  if(!s_isInitialized) {
      wmlLOGs(WSL_LOG_ERROR, "Not initialized");
      return WML_ERROR_NOT_INITIALIZED;
  }

  if(!content || !publicId || !contentSize) {
      wmlLOGs(WSL_LOG_ERROR, "WML_AddTokenSet: Empty argument.");
      return WML_ERROR_EMPTY_ARG;
  }

  return codeLoad(publicId, content, contentSize);
}

/*
=============================================================================
*/
/*  7.9  */
void WML_SetFeatures(WML_FEATURE feature)
{
/*
-----------------------------------------------------------------------------
*/
/*  Code */
  if(feature&WML_EMIT_DEF_ATT_VALUES) {
      xmlEmitDefaults();
  }
}

/*
=============================================================================
*/
/*  7.10  */
static int  ErrorHandler(XML_STATUS   errorCode,
                         PString      context,
                         void       * usrData)
{
/*
-----------------------------------------------------------------------------
*/
/*  Data structures */

  char _buf[256];
  SDocData *_s = (SDocData*)usrData;

/* Code */

  if(!_s->m_error) {
      if(context) {
          wPrint(STR_LEN(context),STR_DATA(context),
                 64,_buf);
          if(_s->m_docURL)
              wmlLOGssss( WSL_LOG_ERROR,
                      "XML error:",
                      (char*)_s->m_docURL,
                      (char*)error2str(errorCode),
                      _buf);
          else
              wmlLOGsss( WSL_LOG_ERROR,
                      "XML error: URL?:",
                      (char*)error2str(errorCode),
                      _buf);
      }
      else {
          if(_s->m_docURL)
              wmlLOGsss( WSL_LOG_ERROR,
                      "XML error: ",
                      (char*)_s->m_docURL,
                      (char*)error2str(errorCode));
          else
              wmlLOGss( WSL_LOG_ERROR,
                      "XML error: URL?: %s",
                      (char*)error2str(errorCode));
      }
      _s->m_error = errorCode;
  }

  return 1;
}

/*
=============================================================================
*/
/*  7.11  */
static int  TokenHandler(EXmlToken  xToken,
                         PString    value,
                         void     * usrData)
{
/*
-----------------------------------------------------------------------------
*/
/*  Data structures */

  SDocData  * _data = (SDocData*)usrData;
  SXmlEvent * _event;

/* Code */

#ifdef WML_DEBUG
  switch(xToken) {
     case XTOK_START_ELEMENT    :
                 DBG_LOG2((DBG_XTOK,value,"Token: START_ELEM="));
                 break;
     case XTOK_END_ELEMENT      :
                 DBG_LOG2((DBG_XTOK,value,"Token: END_ELEM  ="));
                 break;
     case XTOK_ATTR_NAME        :
                 DBG_LOG2((DBG_XTOK,value,"Token: ATTR_NAME ="));
                 break;
     case XTOK_ATTR_VALUE       :
                 DBG_LOG2((DBG_XTOK,value,"Token: ATTR_VALUE="));
                 break;
     case XTOK_PARSED_CHARDATA  :
                 DBG_LOG2((DBG_XTOK,value,"Token: CHARDATA  ="));
                 break;
     case XTOK_UNPARSED_CHARDATA:
                 DBG_LOG2((DBG_XTOK,value,"Token: CDATA     ="));
                 break;

     case XTOK_PI_TARGET        :
                 DBG_LOG2((DBG_XTOK,value,"Token: PI        ="));
                 break;
     case XTOK_PI_VALUE         :
                 DBG_LOG2((DBG_XTOK,value,"Token: PI        ="));
                 break;
     case XTOK_NOTE_UNPARSED_ENTITY :
                  DBG_LOG2((DBG_XTOK,value,"Token: NOTE Entity ="));
                  break;
     case XTOK_NOTE_ENTITY_PUBID :
                  DBG_LOG2((DBG_XTOK,value,"Token: NOTE En.pubId="));
                  break;
     case XTOK_NOTE_ENTITY_SYSID :
                  DBG_LOG2((DBG_XTOK,value,"Token: NOTE En.sysId="));
                  break;
     case XTOK_NOTE_NOTIFICATION :
                  DBG_LOG2((DBG_XTOK,value,"Token: NOTIFICATION="));
                  break;
     case XTOK_NOTE_NOTIFICATION_PUBID :
                  DBG_LOG2((DBG_XTOK,value,"Token: NOTE pubId="));
                  break;
     case XTOK_NOTE_NOTIFICATION_SYSID :
                  DBG_LOG2((DBG_XTOK,value,"Token: NOTE sysId="));
                  break;
     case XTOK_PUBID_OF_DTD :
                  DBG_LOG2((DBG_XTOK,value,"Token: DTD ="));
                  break;
     default : assert(0);
               break;
  }
#endif

  if(xToken<XTOK_PI_TARGET) {
      _event = newSXmlEvent(xToken,value,_data);
      FIFO_INSERT(_data->m_eventList, _event);
  }
  else if(xToken==XTOK_PUBID_OF_DTD) {
      setWmlPubId(value,_data);
  }

  return 1;
}

/*
=============================================================================
*/


/*  End of File  */

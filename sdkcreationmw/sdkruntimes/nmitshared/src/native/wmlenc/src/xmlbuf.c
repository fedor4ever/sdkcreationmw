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
/* File    : U:\wpsvob\wmlenc\src\common\xmlbuf.c
   Version : \main\9
   Date    : 17-Oct-00.12:48:28
*/

/*
 * ========================================================================
 *  Name        : xmlbuf.c
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

        Buffer cache for external documents (mainly for DTDs).


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
        7.1   initBufferCache
        7.2   termBufferCache
        7.3   addBufferToCache
        7.4   getBuffer
        7.5   releaseBuffer
        7.6   loadKnownDTDs
        7.7   available
        7.8   lookUp
        7.9   touch
        7.10  cacheLock
        7.11  cacheUnlock
        7.12  convert2ASCII
        7.13  downLoadExternal

*/



/*  3  GLOSSARY

        None

*/


/*  4  EXTERNAL RESOURCES  */

    /* None */

/*  4.1  Include Files  */

#include "xmlbuf.h"
#include "wmlapi.h"
#include "chenc.h"
#include "wps_service.h"

/*  4.2 External Data Structures  */

    /* None */


/*  4.3 External Function Prototypes  */

    /* None */


/*  5  LOCAL CONSTANTS AND MACROS  */

#define NCHAR_IN_CACHE  (XML_BUFFER_SIZE/sizeof(WCHAR))
#define USE_LOCK        1
#define DONT_USE_LOCK   0

#define BUFID       (_pBufHead->m_id)
#define NRUSR       (_pBufHead->m_nrUser)
#define PUBID       (_pBufHead->m_extId.m_pubId)
#define DATA        (_pBufHead->m_buffer)
#define NCHAR       (_pBufHead->m_nChar)
#define PREV        (_pBufHead->m_lruPrev)
#define NEXT        (_pBufHead->m_lruNext)
#define FIRST       (s_cache.m_first)
#define POOL        (&(s_cache.m_strPool))



/*  6  MODULE DATA STRUCTURES  */


/*  6.1 Local Data Structures  */

static WML_URLLOAD s_download;

typedef struct SBufferHead_s
{
  UINT16    m_id;
  UINT16    m_nrUser;
  SExtId    m_extId;                    /* external id (PUBLIC and SYSTEM)  */
  UINT32    m_nChar;                    /* used buffer size as number of ch */
  struct SBufferHead_s *   m_lruPrev;   /* least recently used list: prev   */
  struct SBufferHead_s *   m_lruNext;   /* least recently used list: next   */
  WCHAR     m_buffer[NCHAR_IN_CACHE];
}SBufferHead;

static struct 
{
  SPool         m_strPool;
  SBufferHead   m_buffers[MAX_NUM_OF_BUFFER];
  SBufferHead  *m_first;
  WSL_Mutex     m_mutex;
}s_cache;


/*  6.2 Local Function Prototypes  */

static SBufferHead * lookUp
                        (PString  id);
                    
static void     touch
                    (SBufferHead  *bh);

static void     cacheLock
                    (void);

static void     cacheUnlock
                    (void);
                    
static WML_STATUS      loadKnownDTDs
                    (WML_DTD_DESCRIPTOR *desc[]);
                    

static SBufferHead * available
                        (void);


static int convert2ASCII(PString w,char *s);


/*  7 MODULE CODE  */


/*
=============================================================================
*/
/*  7.1  */
WML_STATUS initBufferCache(WML_URLLOAD         download,
                    WML_DTD_DESCRIPTOR *desc[])
{
/*
 * a) Initialize the circular list of 'buffer heads'.
 *    (Do not allocate memory for buffers.)
 * b) Insert the textual representations of the DTDs into the cache.
 *
 * Return Value: 0        - character conversion of one of the DTD failed.
 *               non zero - success
-----------------------------------------------------------------------------
*/
/*  Data structures */

  int           _i;
  SBufferHead  *_pBufHead = s_cache.m_buffers;

/* Code */

  s_download = download;

  constrPool(POOL, sizeof(struct String_s));

  FIRST = &(s_cache.m_buffers[0]);
  for(_i=0; _i<MAX_NUM_OF_BUFFER; _i++, _pBufHead++) {
      BUFID      = _i+1;
      NRUSR      = 0;
      PUBID      = newString(POOL);
      NCHAR      = 0;
      PREV       = _pBufHead-1;
      NEXT       = _pBufHead+1;
  }
  s_cache.m_buffers[0].m_lruPrev = &(s_cache.m_buffers[MAX_NUM_OF_BUFFER-1]);
  s_cache.m_buffers[MAX_NUM_OF_BUFFER-1].m_lruNext = &(s_cache.m_buffers[0]);

  WSL_OS_MutexInit(&(s_cache.m_mutex));

  return loadKnownDTDs(desc);
}

/*
=============================================================================
*/
/*  7.2  */
void termBufferCache(void)
{
/*
 * Release all allocated memory.
-----------------------------------------------------------------------------
*/
/*  Data structures */

  int _i;
  SBufferHead  * _pBufHead = s_cache.m_buffers;

/* Code */

  for(_i=0; _i<MAX_NUM_OF_BUFFER; _i++, _pBufHead++) {
      deleteString(PUBID, POOL );
  }

  destrPool(POOL);

  WSL_OS_MutexDestroy(&(s_cache.m_mutex));
}

/*
=============================================================================
*/
/*  7.3  */
void    addBufferToCache
                (PString        pubId,
                 const WCHAR  * buffer,
                 UINT32         nWChar)
{
/*  Data structures */
  SBufferHead  *_pBufHead;

/* Code */

  if(   !pubId
     || !STR_LEN(pubId)
     || !buffer
     || !nWChar
     || (nWChar>NCHAR_IN_CACHE)) {
      return;
  }


  cacheLock();

  if( (_pBufHead=lookUp(pubId)) ) {
      touch(_pBufHead);
  }
  else if( (_pBufHead=available()) ) {

      touch(_pBufHead);     /* now this is the most recently */

      memcpy(DATA, buffer, (nWChar*sizeof(WCHAR)) );
      NCHAR = nWChar;

      strCpy(PUBID, pubId);
      strNormalize(PUBID);
      strStrip(PUBID);

  }

  cacheUnlock();

  return;
}

/*
=============================================================================
*/
/*  7.4  */
int  getBuffer
                (PString        pubId,
                 const WCHAR ** buffer,
                 UINT32       * nWChar)
{
/*
 * Try to retrieve the document, identified by the public/system id.
 * 
 * Return: 0 - the requested document is not available
 *         id of the requested buffer
 *         
 *         In case of success '*buffer' points to the buffer, 
 *         'nChar' gives the # of Unicode characters in the buffer.
 *
-----------------------------------------------------------------------------
*/
/*  Data structures */
  SBufferHead  *_pBufHead;
  int           _retv = 0;

/* Code */
  cacheLock();

  if( (_pBufHead=lookUp(pubId)) ) {
      touch(_pBufHead);
      _retv = BUFID;
      *buffer = DATA;
      *nWChar = NCHAR;
      NRUSR++;
  }
  else {
      *buffer = 0;
      *nWChar  = 0;
  }

  cacheUnlock();
  return _retv;
}

/*
=============================================================================
*/
/*  7.5  */
void  releaseBuffer(int bufferId)
{
/*
 * The previously asked buffer (belong to the 'bufferId') is no longer
 * in use. If threads do not use it all, it will be available for
 * subsequent requests.
-----------------------------------------------------------------------------
*/
/*  Data structures */

  int _i;
  SBufferHead  * _pBufHead = FIRST;

/* Code */

  cacheLock();

  for(_i=0; _i<MAX_NUM_OF_BUFFER; _i++, _pBufHead=NEXT) {
      if(bufferId==BUFID) {
          NRUSR--;
          break;
      }
  }

  cacheUnlock();                        /* Release resource here  */
}
/*
=============================================================================
*/
/*  7.6  */
static WML_STATUS loadKnownDTDs(WML_DTD_DESCRIPTOR *desc[])
{
/*
 * Convert and insert DTDs into the cache.
 *
 * Return Values: 0 - one of the conversion failed
 *                non zero - success
-----------------------------------------------------------------------------
*/
/*  Data structures */

  int           _i;
  UINT32        _nWChar;
  int           _retv = WML_OK;

  WCHAR  *      _cnvBuffer;
  char   *      _cp;
  SEncBuffers   _buffers;
  SBufferHead * _pBufHead = FIRST;

/* Code */

  _cnvBuffer = (WCHAR*)XML_ALLOC(XML_BUFFER_SIZE);

  for(_i=0; desc && desc[_i]; _i++) {

      if(_i>=MAX_NUM_OF_BUFFER) {
          wmlLOGss(WSL_LOG_ERROR,
                 "No more room for DTD :",desc[_i]->m_publicId);
          _retv = WML_ERROR_CACHE_FULL;
          break;
      }

      if(    (desc[_i]->m_textBuffer == NULL)
          || (desc[_i]->m_bufferSize == 0) ) {
          break;
      }

      _buffers.m_enc     = getCharEncoding("UTF-8");
      _buffers.m_src     = desc[_i]->m_textBuffer;
      _buffers.m_srcSize = desc[_i]->m_bufferSize;
      _buffers.m_dst     = _cnvBuffer;
      _buffers.m_dstSize = XML_BUFFER_SIZE/sizeof(WCHAR);

      if(!convertToUnicode(&_buffers)) {
          wmlLOGss(WSL_LOG_ERROR,"Cannot convert DTD :",desc[_i]->m_publicId);
          _retv = WML_ERROR_CONVERSION_FAILED;
      }
      else {
          wmlLOGss(WSL_LOG_INFO,"Preloaded DTD:",desc[_i]->m_publicId);
          _nWChar = _buffers.m_dstSize;
          _cp = desc[_i]->m_publicId;
          while(*_cp) {
              strAppendChar(PUBID, *_cp++);
          }
          strNormalize(PUBID);
          strStrip(PUBID);
          memcpy(DATA, _cnvBuffer, (_nWChar*sizeof(WCHAR)) );
          NRUSR++;      /* never discard this buffer */
          NCHAR = _nWChar;
          _pBufHead = NEXT;
      }
  }
  XML_FREE(_cnvBuffer);
  return _retv;
}

/*
=============================================================================
*/
/*  7.7  */
static SBufferHead * available(void)
{
/*
 * Look for an unused buffer, and touch() it.
 * At first look among the least recently used buffers.
 *
 * Returns: 0   - failure (each buffer is used by a thread)
 *          otherwise success
-----------------------------------------------------------------------------
*/
/*  Data structures */

  SBufferHead  *_pBufHead = FIRST->m_lruPrev;
  int _i;

/* Code */

  for(_i=0; _i<MAX_NUM_OF_BUFFER; _i++, _pBufHead=PREV) {

      if(!NRUSR)
          return _pBufHead;

  }

  return (SBufferHead*)0;
}

/*
=============================================================================
*/
/*  7.8  */
static SBufferHead * lookUp(PString  pubId)
{
/*
 * Search for buffer matches with public ID of 'id' or system ID of 'id'
 * in cache.
 * 
 * Returns: 0 - failure
 *          1 - success
-----------------------------------------------------------------------------
*/
/*  Data structures */

  PString       _tmpStr;
  SBufferHead  *_pBufHead;

/* Code */

  if(pubId && STR_LEN(pubId)) {

      _tmpStr = newString(POOL);

      strCpy(_tmpStr, pubId);
      strNormalize(_tmpStr);             /* XR096 */

      if(strStrip(_tmpStr)) {

          _pBufHead = FIRST;
          do {

              if(STR_SAME(_tmpStr, PUBID)) {
                  deleteString(_tmpStr,POOL);
                  return _pBufHead;
              }

          }while( (_pBufHead=NEXT) != FIRST ) ;

      }

      deleteString(_tmpStr,POOL);
  }
  return (SBufferHead*)0;
}
/*
=============================================================================
*/
/*  7.9  */
static void touch(SBufferHead  *bh)
{
/*
 * Link out 'bh' from the circular list of 'buffer heads' and place
 * it into the first position. The first position means the most
 * recently position.
-----------------------------------------------------------------------------
*/
/*  Data structures */

/* Code */

  if(bh!=s_cache.m_first) {
      bh->m_lruPrev->m_lruNext = bh->m_lruNext;     /* link out */
      bh->m_lruNext->m_lruPrev = bh->m_lruPrev;

      bh->m_lruPrev = s_cache.m_first->m_lruPrev;    /* link in  */
      bh->m_lruNext = s_cache.m_first;
      s_cache.m_first->m_lruPrev->m_lruNext = bh;
      s_cache.m_first->m_lruPrev            = bh;
      s_cache.m_first = bh;
  }
  /*            prevOfFirst   First   nextOfFirst
   *              (LRU)       (MRU)
   * ----------------------------------------------
   *   before:      X           A        B
   *   after :      X          New       A
   */
}

/*
=============================================================================
*/
/*  7.10  */
static void cacheLock(void)
{
/*
 * Enter into a critical section.
-----------------------------------------------------------------------------
*/
/*  Data structures */

/* Code */

  WSL_OS_MutexLock(&(s_cache.m_mutex));
}

/*
=============================================================================
*/
/*  7.11  */
static void cacheUnlock(void)
{
/*
 * Leave the critical section.
-----------------------------------------------------------------------------
*/
/*  Data structures */

/* Code */

  WSL_OS_MutexUnlock(&(s_cache.m_mutex));
}

/*
=============================================================================
*/
/*  7.12  */
static int convert2ASCII(PString w,char *s)
{
/*
 * Dumb conversion to ASCII.
-----------------------------------------------------------------------------
*/
/*  Data structures */
  WCHAR * _w = STR_DATA(w);
  int     _n = STR_LEN(w);

/* Code */
  while(_n--) {
      if(*_w>0xffU)
          return 0;
      *s++ = (char) *_w++;
  }
  *s = 0;

  return 1;
}
/*
=============================================================================
*/
/*  7.13  */
WML_STATUS downLoadExternal
                   (SExtId       *   extId,
                    const BYTE   *   loc,
                    WCHAR        **  buffer,     /* Out: Unicode buffer   */
                    UINT32       *   nChar)      /* Out: number of chars  */
{
/*
 * Try to download/convert document from the URL 'sysId'.
 *
 * Return: the id of the downloaded buffer or zero.
-----------------------------------------------------------------------------
*/
/*  Data structures */
  char          _urlString[MAX_LENGTH_OF_SYSTEM_ID+1];
  UINT8       * _urlBuffer;
  WCHAR       * _cnvBuffer;
  SEncBuffers   _buffers;
  UINT32        _siz = 0;
  WML_STATUS    _error = WML_OK;


/* Code */

  *buffer = (WCHAR*)0;
  *nChar  = 0U;

  if(!extId || !extId->m_sysId)
      return WML_ERROR_INVALID_URL;

  if(    (!strStrip(extId->m_sysId))
      || (STR_LEN(extId->m_sysId)>=MAX_LENGTH_OF_SYSTEM_ID))
      return WML_ERROR_INVALID_URL;

  if(!convert2ASCII(extId->m_sysId,_urlString))
      return WML_ERROR_INVALID_URL;

  if(!s_download)
      return WML_ERROR_NETWORK_ERROR;


  _urlBuffer = (UINT8*)XML_ALLOC(XML_BUFFER_SIZE);

  _siz = XML_BUFFER_SIZE;

  _error = s_download((BYTE*)_urlString,
                     loc,
                     &_siz,
                     _urlBuffer);
  if(!_error) {
      if(!_siz || _siz>=XML_BUFFER_SIZE)
          _error = WML_ERROR_NETWORK_ERROR;
  }

  if (!_error) {

      _cnvBuffer = (WCHAR*)XML_ALLOC(XML_BUFFER_SIZE);

      /* Convert to Unicode */
      _buffers.m_enc     = getCharEncoding("UTF-8");
      _buffers.m_src     = _urlBuffer;
      _buffers.m_srcSize = _siz;
      _buffers.m_dst     = _cnvBuffer;
      _buffers.m_dstSize = XML_BUFFER_SIZE/sizeof(WCHAR);

      if(!convertToUnicode(&_buffers)) {
          _error = WML_ERROR_CONVERSION_FAILED;
          XML_FREE(_cnvBuffer);
      }
      else {
          *nChar  = _buffers.m_dstSize;
          *buffer = _cnvBuffer;
      }

  }

  XML_FREE(_urlBuffer);

  return _error;
}

/*
=============================================================================
*/


/*  End of File  */

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
/* File    : U:\wpsvob\wmlenc\src\include\wmlvalid.h
   Version : \main\10
   Date    : 17-Oct-00.12:51:37
*/

/*
 * ========================================================================
 *  Name        : wmlvalid.h
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
-----------------------------------------------------------------------------

    DESCRIPTION

    This file contains constant, data type, exported function definitions of 
	the WML Validate module

-----------------------------------------------------------------------------
*/

#ifndef WMLVALID_H
#define WMLVALID_H

#include "wmlapi.h"
#include "xmlapi.h"
#include "xmlstr.h"
#include "xmlsymt.h"
#include "wmlarray.h"
#include "wmlmsg.h"
#include "chenc.h"

/*  CONSTANTS  */

#define GT_SWITCH_PAGE     0x00
#define GT_END             0x01
#define GT_ENTITY          0x02
#define GT_STR_I           0x03
#define GT_LITERAL         0x04

#define GT_EXT_I_0         0x40
#define GT_EXT_I_1         0x41
#define GT_EXT_I_2         0x42
#define GT_EXT_T_0         0x80
#define GT_EXT_T_1         0x81
#define GT_EXT_T_2         0x82
#define GT_EXT_0           0xC0
#define GT_EXT_1           0xC1
#define GT_EXT_2           0xC2

#define GT_PI              0x43
#define GT_LITERAL_C       0x44
#define GT_STR_T           0x83
#define GT_LITERAL_A       0x84
#define GT_OPAQUE          0xC3
#define GT_LITERAL_AC      0xC4
#define GT_END             0x01
#define GT_ENTITY          0x02
#define GT_STR_I           0x03
#define GT_LITERAL         0x04

#define HAS_ATTRIBUTE   0x80
#define HAS_CONTENT     0x40

typedef enum {
    ESCAPE_VAR,
    NOESCAPE_VAR,
    UNESCAPE_VAR
}EscapeMode;


/*  MACROS  */

#define FIFO(type)          struct name2(type,Fifo)
#define DECLARE_FIFO(type)  FIFO(type) { type  *m_head; type *m_tail; }
#define FIFO_INSERT(fifo,ptr) { if(!fifo.m_tail) fifo.m_head=fifo.m_tail=ptr;\
                                else fifo.m_tail->m_next=ptr, fifo.m_tail=ptr;\
                                ptr->m_next=0;}

#define FIFO_EMPTY(f)    (f.m_head==0)
#define FIFO_GET(f,retv) (f.m_head ?\
                          (retv=f.m_head, (f.m_head=f.m_head->m_next) ? 0 : (f.m_tail=0), retv)\
                          : (retv=0))

#define FIRST_NODE(list)        (list.m_root)
#define NEXT_NODE(prev_node)    ( (!prev_node) ? 0 : prev_node->m_next)

#define DOCPOOL      (&(data->m_pool))

/*  DATA TYPES  */


/*      _____________________________     _________________________
 *     |       SDocData              |   |      SWmlDescriptor     |
 *    _____________________________  |   | ---------------------   | 
 *   |       SDocData              | |   |     Static tables,      | 
 *   |     -----------             | |   |  WML Spec. chapter 14.3 | 
 *   |  Dynamic data per document  | |   +-------------------------+ 
 *   +-----------------------------+ |   | Tags/elements (14.3.2)  | 
 *   | o list of XML events (the   | |   | Attributes    (14.3.3)  | 
 *   |   output of the XML parser) | |   | Attr. Values  (14.3.4)  | 
 *   | o list of WML tokens (inter-| |    -------------------------- 
 *   |   mediate state) as appear  | |                               
 *   |   in the XML event list     | |                               
 *   | o table of strings (inter-  |--                               
 *   |   mediate state of the      |                                 
 *   |   'string table')           |                                 
 *   -------------------------------                                 
 */


typedef struct SWmlNode SWmlNode;
typedef struct SDocData SDocData;
typedef void  (*FHandler)(SDocData *, const SWmlNode *);
DECLARE_SLIST(SWmlNode);


/* SWmlNode structure contains static information of the WML encoding.
 * This kind of objects are initialized before processing the 
 * documents, and they are handled as constant object until termination
 * of the application.
 */
struct SWmlNode
{
  SWmlNode      * m_next;       /* points to next node              */
  PString         m_name;       /* name of the tag/attr/value       */
  UINT16          m_code;       /* lo: code,  hi: code page         */
  FHandler        m_handler;    /* pointer to a handler function    */
  SLIST(SWmlNode) m_children;   /* values/attributes                */
};

/*          A SWmlDescriptor type object
 *          /             |             \
 *  elements          attributes        generalValues
 *   / | \            /     |    \         /  |  \
 *  E1 E2 ...       A1      A2    ...     V1  V2  ...
 *                 / \     / | \
 *                P1 P2   P1 P2 ...
 *
 *
 */
typedef struct SWmlDescriptor SWmlDescriptor;
struct SWmlDescriptor
{
  SWmlDescriptor  * m_next;
  PString           m_pubId;
  SPool             m_pool;                 /* pool for strings         */
  SLIST(SWmlNode)	m_elements[128];        /* Hash table of elements   */
  SLIST(SWmlNode)	m_attributes[128];      /* Hash table of attributes */
  SLIST(SWmlNode)	m_generalValues[128];   /* Hash table of gen. values */
};
DECLARE_SLIST(SWmlDescriptor);


/* The list of XML events is implemented as nodes of SXmlEvent. */
typedef struct SXmlEvent
{
  struct SXmlEvent  * m_next;
  EXmlToken           m_token;
  PString             m_tokenValue;
}SXmlEvent;

SXmlEvent * newSXmlEvent(EXmlToken tok,
                         PString   str,
                         SDocData *data);

void        deleteSXmlEvent(SXmlEvent *event,
                            SDocData  *data);

DECLARE_FIFO(SXmlEvent);

typedef enum 
{
  STR_INLINE,
  STR_IN_TABLE,
  STR_OPAQUE_DATE
}ESite;


typedef struct SWmlString SWmlString;
struct SWmlString
{
  SWmlString    * m_next;
  PString         m_string;
  ESite           m_site;       /* STR_INLINE or STR_IN_TABLE   */
  UINT32          m_offset;     /* in table  */
};

typedef struct SWmlToken SWmlToken;
struct SWmlToken
{
  SWmlToken     * m_next;
  UINT16          m_code;
  SWmlString    * m_strEntry;
};


DECLARE_SLIST(SWmlString);
DECLARE_FIFO(SWmlToken);

typedef struct Segment Segment;
struct Segment
{
  Segment   *m_next;
  PString   m_str;
  UINT16    m_code;
};

typedef struct StringNode StringNode;
struct StringNode
{
  StringNode    *m_next;
  PString        m_str;
};
DECLARE_SLIST(StringNode);


struct SDocData
{
  FIFO(SXmlEvent)         m_eventList;  /* events has got from XML parser   */
  FIFO(SWmlToken)         m_tokenList;
  SLIST(SWmlString)       m_stringTable[128];
  WML_STATUS              m_error;      /* remember the error               */
  SPool                   m_pool;       /* SWmlString/SXmlEvent/SWmlToken   */
  const SWmlDescriptor  * m_desc;       /* see above                        */

  SArray                  m_array;

  PString                 m_dtdPUBID;
  UINT32                  m_dtdPUBIDCode;
  const BYTE *            m_docURL;

  SCCOut                  m_scc;

  UINT32                * m_oBufferSize;
  BYTE                  * m_oBuffer;
  int                   * m_arraySize;
  UINT16               ** m_metaHttpEquiv;
  int                   * m_stringLength;
  UINT16               ** m_metaContent;
  UINT16                  m_mAvailSiz;
  UINT16                  m_mAvailLen;
  UINT16                  m_mMaxLen;
  UINT16                  m_mCounter;

  SLIST(StringNode)       m_uniqInDeck; /* unique elements/document */
  SLIST(StringNode)       m_uniqEvent;  /* unique bindings/element  */
  SLIST(StringNode)       m_uniqDo;     /* unique 'do' elements/card*/

  int                   * m_task_counter;
  PString               * m_do_name;
  PString               * m_do_type;
  int                     m_preFlag;
};

/*  EXTERNAL DATA STRUCTURES  */

extern SWmlNode * g_unknownElement;
extern SWmlNode * g_unknownAttribute;

/*  FUNCTION PROTOTYPES  */

/* wmlvalid.c   */
WML_STATUS  initWML(WML_DTD_DESCRIPTOR *desc[]);
void        termWML(void);

WML_STATUS isValidWMLDocument(SDocData   *);
SDocData * newSDocData(const char*  oEncoding,
                       UINT32 *     oBufferSize,
                       BYTE *       oBuffer,
                       int *        numHttpEquiv,
                       UINT16 **    metaHttpEquiv,
                       int *        numContent,
                       UINT16 **    metaContent,
                       const BYTE * docURL);

void       deleteSDocData(SDocData *data);

SWmlDescriptor * lookupWmlDescriptor(PString pubId);

void setWmlPubId(PString   str,
                 SDocData *data);

WML_STATUS codeLoad
                (char   * publicId,
                 char   * content,
                 UINT32   contentSize);


/* codeload.c   */
SWmlNode * newSWmlNode 
                      (PString         name,
                       UINT32          code,
                       FHandler        handler,
                       SWmlDescriptor *desc);

void deleteSWmlNode 
                   (SWmlNode       *node,
                    SWmlDescriptor *desc);

SWmlDescriptor * newSWmlDescriptor
                    (const char * pubId,
                     const char *text,
                     UINT32      length);

void 			 deleteSWmlDescriptor(
                    SWmlDescriptor *desc);


/* wmlsegm.c */
Segment *   newSegment
                    (SDocData  *data,
                     PString    str,
                     UINT16     code,
                     Segment   *prev);
void        deleteSegment
                    (SDocData  *data,
                     Segment   *seg);
Segment *   makeSegments
                    (SDocData       * data,
                     const SWmlNode * attrNode,
                     const PString    value);
void        flushSegments
                    (SDocData * data,
                     Segment  * seg);

/* wmlvar.c  */
void variables(SDocData     * data,
               Segment      * seg,
               EscapeMode     mode);

int isGoodVarName(const PString str,
                  int   _cnt);


/* wmlstruc.c   */
const SWmlNode * lookupElement(SDocData *data);
const SWmlNode * lookupAttr(SDocData *data,
                            const SWmlNode *element);
SWmlString     * lookupString(SDocData    * data,
                              PString       s);
SWmlString     * insertString(SDocData    * data,
                              PString       s);
SWmlToken      * addToken    (SDocData *    data,
                              UINT16        code);
SWmlToken      * addTokenWithStr(SDocData *    data,
                              UINT16        code,
                              PString       str,
                              ESite         site);
void valueSubstitution(SDocData * data,
                       Segment  * seg);
void lookupGenVal(SDocData *data,
                  Segment *seg);
void setWmlError(WML_VALIDATION_ERROR  status,
                 PString     context,
                 SDocData   *data);

void constrUniq (SLIST(StringNode) *list,
                 SDocData          *data);
void destrUniq  (SLIST(StringNode) *list,
                 SDocData          *data);
int  insertUniq (PString            str,
                 SLIST(StringNode) *list,
                 SDocData          *data);



/* wmlbin.c */
void       convertToWBXML(SDocData * data,
                          UINT32     version);

/* wmlhndl.c    */
void handlerRootElement     (SDocData * data);
void handlerDefaultElement  (SDocData *data, const SWmlNode *node);
void handlerUnknownAttr     (SDocData *data, const SWmlNode *node);
void handlerMeta            (SDocData *data, const SWmlNode *node);
void handlerOnce            (SDocData *data, const SWmlNode *node);
void handlerOneTask         (SDocData *data, const SWmlNode *node);
void handlerTask            (SDocData *data, const SWmlNode *node);
void handlerEvents          (SDocData *data, const SWmlNode *node);
void handlerDo              (SDocData *data, const SWmlNode *node);
void handlerCard            (SDocData *data, const SWmlNode *node);

void handlerNMTOKEN         (SDocData *data, const SWmlNode *node);
void handlerCDATA           (SDocData *data, const SWmlNode *node);
void handlerNumber          (SDocData *data, const SWmlNode *node);
void handlerNotZero         (SDocData *data, const SWmlNode *node);
void handlerLength          (SDocData *data, const SWmlNode *node);
void handlerVdata           (SDocData *data, const SWmlNode *node);
void handlerURL             (SDocData *data, const SWmlNode *node);
void handlerEnum            (SDocData *data, const SWmlNode *node);
void handlerURLEvent        (SDocData *data, const SWmlNode *node);
void handlerOnEventType     (SDocData *data, const SWmlNode *node);
void handlerDoType          (SDocData *data, const SWmlNode *node);
void handlerDoName          (SDocData *data, const SWmlNode *node);
void handlerVarName         (SDocData *data, const SWmlNode *node);

void handlerIgnore          (SDocData *data, const SWmlNode *node);

/* Handlers for WML 1.2 */
void handlerMeta2           (SDocData *data, const SWmlNode *node);
void handlerPre             (SDocData *data, const SWmlNode *node);

void handlerCDATA2          (SDocData *data, const SWmlNode *node);

/* SI elements */

/* SI attributes */
void siHandlerCDATA         (SDocData *data, const SWmlNode *node);
void siHandlerDateTime      (SDocData *data, const SWmlNode *node);
void siHandlerHREF          (SDocData *data, const SWmlNode *node);

const char *error2str(WML_VALIDATION_ERROR xst);

#endif  /*  WMLVALID_H  */

/*  End of File  */


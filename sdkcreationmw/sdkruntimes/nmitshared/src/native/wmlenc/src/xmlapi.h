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
/* File    : U:\wpsvob\wmlenc\src\include\xmlapi.h
   Version : \main\8
   Date    : 17-Oct-00.12:51:39
*/

/*
 * ========================================================================
 *  Name        : xmlapi.h
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

    This header file contains the internal API of the XML parser.

-----------------------------------------------------------------------------
*/

#ifndef XMLAPI_H
#define XMLAPI_H

#include "xmlstr.h"
#include "xmlmsg.h"

/*  CONSTANTS  */

/*  MACROS  */

/*  DATA TYPES  */

/* Definition of XML token types */
typedef enum {
    XTOK_START_ELEMENT = 1,
    XTOK_END_ELEMENT,
    XTOK_ATTR_NAME,
    XTOK_ATTR_VALUE,
    XTOK_UNPARSED_CHARDATA,
    XTOK_PARSED_CHARDATA,
    XTOK_PI_TARGET,
    XTOK_PI_VALUE,
    /* Notifications */
    XTOK_NOTE_UNPARSED_ENTITY,       /* Notification for             */
    XTOK_NOTE_ENTITY_PUBID,
    XTOK_NOTE_ENTITY_SYSID,
    XTOK_NOTE_NOTIFICATION,          /* The notification             */
    XTOK_NOTE_NOTIFICATION_PUBID,
    XTOK_NOTE_NOTIFICATION_SYSID,

    XTOK_PUBID_OF_DTD
}EXmlToken;

typedef struct 
{
  PString m_pubId;
  PString m_sysId;
}SExtId;

typedef int  (*FXMLCallBack)    (EXmlToken  xmlToken,
                                 PString    value,
                                 void     * usrData);

typedef int  (*FXMLErrorCB)     (XML_STATUS   errorCode,
                                 PString      context,
                                 void       * usrData);
typedef struct SParserData *PARDATA;

/*  EXTERNAL DATA STRUCTURES  */

/*  FUNCTION PROTOTYPES  */

int   initXML(void);

void  termXML(void);

int   parseXML
              (void        * usrData,
               UINT16      * buffer,
               UINT32        bufferSize,
               FXMLCallBack  tokenCB,
               FXMLErrorCB   errorCB,
               const char  * altDTD,
               const BYTE  * urlOfDoc);

int   getDTDID
              (void    * usrData,
               UINT16  * buffer,
               UINT32    bufferSize,
               UINT16    idBufLenU16,
               UINT16  * publicID,
               UINT16  * systemID);

void  xmlEmitDefaults(void);

#endif  /* XMLAPI_H */

/*  End of File  */

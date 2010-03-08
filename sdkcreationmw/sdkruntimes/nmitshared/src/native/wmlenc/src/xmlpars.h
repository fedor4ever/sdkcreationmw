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
/* File    : U:\wpsvob\wmlenc\src\include\xmlpars.h
   Version : \main\7
   Date    : 17-Oct-00.12:51:53
*/

/*
 * ========================================================================
 *  Name        : xmlpars.h
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

    This header file contains the macros and function declarations
    used by the XML parser.

-----------------------------------------------------------------------------
*/

#ifndef XMLPARS_H
#define XMLPARS_H

#include "xmlconst.h"
#include "xmlsymt.h"
#include "xmlstruc.h"
#include "xmlmsg.h"

/*  CONSTANTS  */

/*  MACROS  */

#define NEW_GE(d,name)        ((SGEntity*)symTblNewItem\
                               (&(d->m_commonData.m_GEntities),name))
#define NEW_NOTATION(d,name) ((SNotation*)symTblNewItem\
                              (&(d->m_commonData.m_Notations),name))
#define NEW_PE(d,name)        ((SPEntity*)symTblNewItem\
                               (&(d->m_commonData.m_PEntities),name))
#define NEW_ELEMENT(d,name)    ((SElement*)symTblNewItem\
                                (&(d->m_commonData.m_Elements),name))
#define LOOKUP_GE(d,name)      ((SGEntity*)symTblLookUp\
                                (&(d->m_commonData.m_GEntities),name))
#define LOOKUP_NOTATION(d,nme) ((SNotation*)symTblLookUp\
                                (&(d->m_commonData.m_Notations),nme))
#define LOOKUP_PE(d,name)      ((SPEntity*)symTblLookUp\
                                (&(d->m_commonData.m_PEntities),name))
#define LOOKUP_ELEMENT(d,name) ((SElement*)symTblLookUp\
                                (&(d->m_commonData.m_Elements),name))

#define NEW_ID(d,name)          symTblNewItem (&(d->m_commonData.m_IDs),name)
#define LOOKUP_ID(d,name)       (symTblLookUp (&(d->m_commonData.m_IDs),name)!=0)

/*  DATA TYPES  */

/*  EXTERNAL DATA STRUCTURES  */

/*  FUNCTION PROTOTYPES  */

int     parse                           /* call a rule function */
                    (PARDATA d,
                     int     id,
                     int     option);

int     parseExternal                   /* parse external entity */
                     (PARDATA d,
                      int     ruleId,
                      SExtId *extId);

void    pushBuffer                      /* start reading buffer */
                    (PARDATA d,
                     const WCHAR *gptr,
                     const WCHAR *eptr);

int     popBuffer                       /* stop reading buffer   */
                    (PARDATA d);

void    callError                       /* error notification    */
                    (PARDATA d,
                     XML_STATUS msg,
                     PString context);

void    callWarning                     /* warning notification     */
                    (PARDATA d,
                     XML_STATUS msg,
                     PString context);

void    callToken                       /* send next token          */
                    (PARDATA    d,
                     int        token,
                     PString    value);

void        addAttr                     /* link into list of element */
                    (SElement * element,
                     SAttribute *attr);

SAttribute *lookUpAttr                  /* search in list of element */
                    (SElement * element,
                     PString    name);

void        addTypeValue                /* link val int list of attr */
                    (SAttribute *attr,
                     PString    val,
                     PCommon    pc);
                    
int         typeValueExists             /* search for val in attr's list*/
                    (SAttribute *attr,
                     PString     val);

void        checkContent
                    (PARDATA         d,
                     SContentParser *p,
                     PString         where);

int         checkChild
                    (SContentParser *parser,
                     PString         child);

int         isParent
                    (PARDATA        d,
                     int            ruleId);

int         inRootDocument
                    (PARDATA        d);

#endif  /* XMLPARS_H */

/*  End of File  */

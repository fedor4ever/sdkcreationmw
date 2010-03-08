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
/* File    : U:\wpsvob\wmlenc\src\include\xmlconst.h
   Version : \main\6
   Date    : 17-Oct-00.12:51:43
*/

/*
 * ========================================================================
 *  Name        : xmlconst.h
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

    This header file contains the global constants for the XML parser.

-----------------------------------------------------------------------------
*/

#ifndef XMLCONST_H
#define XMLCONST_H

#include "xmlstr.h"


/*  CONSTANTS  */

/*  MACROS  */

/*  DATA TYPES  */

/*  EXTERNAL DATA STRUCTURES  */

extern const PString g_strID;       /* const: "ID"          */
extern const PString g_strREQUIRED; /* const: "#REQUIRED"   */
extern const PString g_strIMPLIED;  /* const: "#IMPLIED"    */
extern const PString g_strEmpty;    /* const: "EMPTY"       */
extern const PString g_strAny;      /* const: "ANY"         */
extern const PString g_strPCDATA;   /* const: "#PCDATA"     */
extern const PString g_strAmp;      /* const: "amp"         */
extern const PString g_strLt;       /* const: "lt"          */
extern const PString g_strGt;       /* const: "gt"          */
extern const PString g_strApos;     /* const: "apos"        */
extern const PString g_strQuot;     /* const: "quot"        */
extern const PString g_strHEAD;     /* const: "HEAD"        */
extern const PString g_strTEMPLATE; /* const: "TEMPLATE"    */
extern const PString g_strCARD;     /* const: "CARD"        */
extern const PString g_strAuxName;  /* const: "!AuXiLiArY!" */

extern const PString g_strXmlVer;   /* const: "1.0" */
extern const PString g_strXmlLang;  /* const: "xml:lang" */


/*  FUNCTION PROTOTYPES  */

#endif  /* XMLCONST_H */

/*  End of File  */

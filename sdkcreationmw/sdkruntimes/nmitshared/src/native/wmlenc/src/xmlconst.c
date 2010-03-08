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
/* File    : U:\wpsvob\wmlenc\src\common\xmlconst.c
   Version : \main\6
   Date    : 17-Oct-00.12:48:41
*/

/*
 * ========================================================================
 *  Name        : xmlconst.c
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

    /* None */

/*  4.1  Include Files  */

#include "xmlconst.h"



/*  4.2 External Data Structures  */

    /* None */

/*  4.3 External Function Prototypes  */

    /* None */


/*  5  LOCAL CONSTANTS AND MACROS  */

    /* None */

/*  6  MODULE DATA STRUCTURES  */

    /* None */

/*  6.1 Local Data Structures  */


/*
 *
 * Contants PStrings 
 *
 */
static WCHAR s_ID[]    =   {'I','D'};
static WCHAR s_REQ[]   =   {'#','R','E','Q','U','I','R','E','D'};
static WCHAR s_IMP[]   =   {'#','I','M','L','I','E','D'};
static WCHAR s_empty[] =   {'E','M','P','T','Y'};
static WCHAR s_any[]   =   {'A','N','Y'};
static WCHAR s_PCDATA[]=   {'#','P','C','D','A','T','A'};
static WCHAR s_amp[]   =   {'a','m','p'};
static WCHAR s_lt[]    =   {'l','t'};
static WCHAR s_gt[]    =   {'g','t'};
static WCHAR s_apos[]  =   {'a','p','o','s'};
static WCHAR s_quot[]  =   {'q','u','o','t'};
static WCHAR s_xver[]  =   {'1','.','0'};
static WCHAR s_head[]  =   {'H','E','A','D'};
static WCHAR s_template[] ={'T','E','M','P','L','A','T','E'};
static WCHAR s_card[]  =   {'C','A','R','D'};
static WCHAR s_aux[]   =   {'!','A','u','X','i','L','i','A','r','Y','!'};
static WCHAR s_xlang[] =   {'x','m','l',':','l','a','n','g'};

static
struct String_s s_strID     ={ sizeof(s_ID)/sizeof(WCHAR),
                               sizeof(s_ID)/sizeof(WCHAR),
                                      s_ID, {0}};
static
struct String_s s_strREQ    ={ sizeof(s_REQ)/sizeof(WCHAR),
                               sizeof(s_REQ)/sizeof(WCHAR),
                                      s_REQ, {0}};
static
struct String_s s_strIMP    ={ sizeof(s_IMP)/sizeof(WCHAR),
                               sizeof(s_IMP)/sizeof(WCHAR),
                                      s_IMP, {0}};
static
struct String_s s_strEmptyS ={ sizeof(s_empty)/sizeof(WCHAR),
                               sizeof(s_empty)/sizeof(WCHAR),
                                      s_empty, {0}};
static
struct String_s s_strAnyS   ={ sizeof(s_any)/sizeof(WCHAR),
                               sizeof(s_any)/sizeof(WCHAR),
                                      s_any, {0}};
static
struct String_s s_strPCDATAS={ sizeof(s_PCDATA)/sizeof(WCHAR),
                               sizeof(s_PCDATA)/sizeof(WCHAR),
                                      s_PCDATA, {0}};
static
struct String_s s_strAmpS   ={ sizeof(s_amp)/sizeof(WCHAR),
                               sizeof(s_amp)/sizeof(WCHAR),
                                      s_amp, {0}};
static
struct String_s s_strLtS    ={ sizeof(s_lt)/sizeof(WCHAR),
                               sizeof(s_lt)/sizeof(WCHAR),
                                      s_lt, {0}};
static
struct String_s s_strGtS    ={ sizeof(s_gt)/sizeof(WCHAR),
                               sizeof(s_gt)/sizeof(WCHAR),
                                      s_gt, {0}};
static
struct String_s s_strAposS  ={ sizeof(s_apos)/sizeof(WCHAR),
                               sizeof(s_apos)/sizeof(WCHAR),
                                      s_apos, {0}};
static
struct String_s s_strQuotS  ={ sizeof(s_quot)/sizeof(WCHAR),
                               sizeof(s_quot)/sizeof(WCHAR),
                                      s_quot, {0}};
static
struct String_s s_strXmlVS  ={ sizeof(s_xver)/sizeof(WCHAR),
                               sizeof(s_xver)/sizeof(WCHAR),
                                      s_xver, {0}};
static
struct String_s s_strHead   ={ sizeof(s_head)/sizeof(WCHAR),
                               sizeof(s_head)/sizeof(WCHAR),
                                      s_head, {0}};
static
struct String_s s_strTempl  ={ sizeof(s_template)/sizeof(WCHAR),
                               sizeof(s_template)/sizeof(WCHAR),
                                      s_template, {0}};
static
struct String_s s_strCard   ={ sizeof(s_card)/sizeof(WCHAR),
                               sizeof(s_card)/sizeof(WCHAR),
                                      s_card, {0}};
static
struct String_s s_strAux    ={ sizeof(s_aux)/sizeof(WCHAR),
                               sizeof(s_aux)/sizeof(WCHAR),
                                      s_aux, {0}};
static
struct String_s s_strXLang  ={ sizeof(s_xlang)/sizeof(WCHAR),
                               sizeof(s_xlang)/sizeof(WCHAR),
                                      s_xlang, {0}};


const PString g_strID     = &s_strID;
const PString g_strREQUIRED=&s_strREQ;
const PString g_strIMPLIED= &s_strIMP;
const PString g_strEmpty  = &s_strEmptyS;
const PString g_strAny    = &s_strAnyS;
const PString g_strPCDATA = &s_strPCDATAS;
const PString g_strAmp    = &s_strAmpS;
const PString g_strLt     = &s_strLtS;
const PString g_strGt     = &s_strGtS;
const PString g_strApos   = &s_strAposS;
const PString g_strQuot   = &s_strQuotS;
const PString g_strXmlVer = &s_strXmlVS;
const PString g_strHEAD   = &s_strHead;
const PString g_strTEMPLATE=&s_strTempl;
const PString g_strCARD   = &s_strCard;
const PString g_strAuxName= &s_strAux;
const PString g_strXmlLang= &s_strXLang;



/*  6.2 Local Function Prototypes  */

/*  7 MODULE CODE  */
/*
=============================================================================
*/


/*  End of File  */

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
/* File    : U:\wpsvob\wmlenc\src\common\xmllex.c
   Version : \main\7
   Date    : 17-Oct-00.12:48:52
*/

/*
 * ========================================================================
 *  Name        : xmllex.c
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

    type  : data only

    1.2 Functional Description

        Constant strings for the XML parser.

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

          None

*/



/*  3  GLOSSARY

        None

*/


/*  4  EXTERNAL RESOURCES  */

    /* None */

/*  4.1  Include Files  */

#include "xmllex.h"

/*  4.2 External Data Structures  */

    /* None */


/*  4.3 External Function Prototypes  */

    /* None */


/*  5  LOCAL CONSTANTS AND MACROS  */

    /* None */



/*  6  MODULE DATA STRUCTURES  */


/*  6.1 Local Data Structures  */

const char *const g_LexTable[]={
    "???"        ,
    "yes"        ,
    "version"    ,
    "standalone" ,
    "no"         ,
    "encoding"   ,
    "]]>"        ,
    "SYSTEM"     ,
    "PUBLIC"     ,
    "NOTATION"   ,
    "NMTOKENS"   ,
    "NMTOKEN"    ,
    "NDATA"      ,
    "INCLUDE"    ,
    "IGNORE"     ,
    "IDREFS"     ,
    "IDREF"      ,
    "ID"         ,
    "ENTITY"     ,
    "ENTITIES"   ,
    "EMPTY"      ,
    "CDATA"      ,
    "ANY"        ,
    "?>"         ,
    "<?xml"      ,
    "<?"         ,
    "<![CDATA["  ,
    "<!["        ,
    "<!NOTATION" ,
    "<!ENTITY"   ,
    "<!ELEMENT"  ,
    "<!DOCTYPE"  ,
    "<!ATTLIST"  ,
    "<!--"       ,
    "--"         ,
    "-->"        ,
    "#REQUIRED"  ,
    "#PCDATA"    ,
    "#IMPLIED"   ,
    "#FIXED"     ,
    ")"          ,
    "("          ,
    "???????"    
};


/*  6.2 Local Function Prototypes  */

    /* None */


/*  7 MODULE CODE  */


/*
=============================================================================
*/


/*  End of File  */

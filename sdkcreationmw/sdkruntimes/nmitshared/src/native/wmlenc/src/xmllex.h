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
/* File    : U:\wpsvob\wmlenc\src\include\xmllex.h
   Version : \main\7
   Date    : 17-Oct-00.12:51:49
*/

/*
 * ========================================================================
 *  Name        : xmllex.h
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

    This header file contains the XML lex token definitions.

-----------------------------------------------------------------------------
*/

#ifndef XMLLEX_H
#define XMLLEX_H

/*  CONSTANTS  */

enum {
    /* Reverse sorted list of lex tokens */
    TOK_SOMETHING     = 0,      /*              */
    TOK_YES,                    /* "yes"        */
    TOK_VERSION,                /* "version"    */
    TOK_STANDALONE,             /* "standalone" */
    TOK_NO,                     /* "no"         */
    TOK_ENCODING,               /* "encoding"   */
    TOK_CD_SECT_END,            /* "]]>"        */
    TOK_SYSTEM,                 /* "SYSTEM"     */
    TOK_PUBLIC,                 /* "PUBLIC"     */
    TOK_NOTATION,               /* "NOTATION"   */
    TOK_NMTOKENS,               /* "NMTOKENS"   */
    TOK_NMTOKEN,                /* "NMTOKEN"    */
    TOK_NDATA,                  /* "NDATA"      */
    TOK_INCLUDE,                /* "INCLUDE"    */
    TOK_IGNORE,                 /* "IGNORE"     */
    TOK_IDREFS,                 /* "IDREFS"     */
    TOK_IDREF,                  /* "IDREF"      */
    TOK_ID,                     /* "ID"         */
    TOK_ENTITY,                 /* "ENTITY"     */
    TOK_ENTITIES,               /* "ENTITIES"   */
    TOK_EMPTY,                  /* "EMPTY"      */
    TOK_CDATA,                  /* "CDATA"      */
    TOK_ANY,                    /* "ANY"        */
    TOK_XML_PI_END,             /* "?>"         */
    TOK_XML_START,              /* "<?xml"      */
    TOK_PI_START,               /* "<?"         */
    TOK_CD_START,               /* "<![CDATA["  */
    TOK_SECTION_START,          /* "<!["        */
    TOK_NOTATION_START,         /* "<!NOTATION" */
    TOK_ENTITY_START,           /* "<!ENTITY"   */
    TOK_ELEMENT_START,          /* "<!ELEMENT"  */
    TOK_DOCTYPE,                /* "<!DOCTYPE"  */
    TOK_ATTLIST_START,          /* "<!ATTLIST"  */
    TOK_COMMENT_START,          /* "<!--"       */
    TOK_DOUBLE_HYPHEN,          /* "--"         */
    TOK_COMMENT_END,            /* "-->"        */
    TOK_REQUIRED,               /* "#REQUIRED"  */
    TOK_PCDATA,                 /* "#PCDATA"    */
    TOK_IMPLIED,                /* "#IMPLIED"   */
    TOK_FIXED,                  /* "#FIXED"     */
    TOK_GRPC,                   /* ")"          */
    TOK_GRPO,                   /* "("          */
    TOK_SPACE                   /*              */
};

/*  MACROS  */


/*  DATA TYPES  */

/*  EXTERNAL DATA STRUCTURES  */

extern const char * const g_LexTable[];

/*  FUNCTION PROTOTYPES  */

#endif  /* XMLLEX_H */

/*  End of File: XMLLEX.H  */




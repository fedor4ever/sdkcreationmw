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
/* File    : U:\wpsvob\wmlenc\src\common\xmlmsg.c
   Version : \main\6
   Date    : 17-Oct-00.12:49:07
*/

/*
 * ========================================================================
 *  Name        : xmlmsg.c
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

        This file contains the array of function pointers belong
        to the XML grammar rules.


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

        7.1   initRuleFunctions

*/



/*  3  GLOSSARY

        None

*/


/*  4  EXTERNAL RESOURCES  */

    /* None */

/*  4.1  Include Files  */

#define XMLMSG_OWNER    /* define it before includes ! */

#include "xmlmsg.h"
#include "xmlrules.h"


/*  4.2 External Data Structures  */

    /* None */


/*  4.3 External Function Prototypes  */

    /* None */


/*  5  LOCAL CONSTANTS AND MACROS  */

    /* None */



/*  6  MODULE DATA STRUCTURES  */


/*  6.1 Local Data Structures  */

RuleFunction g_rule[N_OF_RULE];

#ifdef WML_DEBUG
const char *g_nameOfRule[N_OF_RULE]={
    "RULE 0 !!!!       ",
    "document          ",
    "Char              ",
    "S                 ",
    "NameChar          ",
    "Name              ",
    "Names             ",
    "Nmtoken           ",
    "Nmtokens          ",
    "EntityValue       ",
    "AttValue          ",
    "SystemLiteral     ",
    "PubidLiteral      ",
    "PubidChar         ",
    "CharData          ",
    "Comment           ",
    "PI                ",
    "PITarget          ",
    "CDSect            ",
    "CDStart           ",
    "CData             ",
    "CDEnd             ",
    "prolog            ",
    "XMLDecl           ",
    "VersionInfo       ",
    "Eq                ",
    "VersionNum        ",
    "Misc              ",
    "doctypedecl       ",
    "markupdecl        ",
    "extSubset         ",
    "extSubsetDecl     ",
    "SDDecl            ",
    "LanguageID        ",
    "Langcode          ",
    "ISO639code        ",
    "IanaCode          ",
    "UserCode          ",
    "Subcode           ",
    "element           ",
    "STag              ",
    "Attribute         ",
    "ETag              ",
    "content           ",
    "EmptyElemTag      ",
    "elementdecl       ",
    "contentspec       ",
    "children          ",
    "cp                ",
    "choice            ",
    "seq               ",
    "Mixed             ",
    "AttlistDecl       ",
    "AttDef            ",
    "AttType           ",
    "StringType        ",
    "TokenizedType     ",
    "EnumeratedType    ",
    "NotationType      ",
    "Enumeration       ",
    "DefaultDecl       ",
    "conditionalSect   ",
    "includeSect       ",
    "ignoreSect        ",
    "ignoreSectContents",
    "Ignore            ",
    "CharRef           ",
    "Reference         ",
    "EntityRef         ",
    "PEReference       ",
    "EntityDecl        ",
    "GEDecl            ",
    "PEDecl            ",
    "EntityDef         ",
    "PEDef             ",
    "ExternalID        ",
    "NDataDecl         ",
    "TextDecl          ",
    "extParsedEnt      ",
    "extPE             ",
    "EncodingDecl      ",
    "EncName           ",
    "NotationDecl      ",
    "PublicID          ",
    "Letter            ",
    "BaseChar          ",
    "Ideographic       ",
    "CombiningChar     ",
    "Digit             ",
    "Extender          "
};
#endif


/*  6.2 Local Function Prototypes  */

    /* None */


/*  7 MODULE CODE  */


/*
 *===========================================================================
 */
/*  7.1  */
void initRuleFunctions(void)
{
/*  Functional Description
 *
 *  Initializes the g_rule[] vector.
 *
 *  Return Value:  -
 *---------------------------------------------------------------------------
 */

/*  Data Structures  */

/*  Code  */

  g_rule[0                       ] = 0                   ;
  g_rule[RULE_DOCUMENT           ] = RuleDocument        ;
  g_rule[RULE_S                  ] = RuleS               ;
  g_rule[RULE_NAME               ] = RuleName            ;
  g_rule[RULE_NMTOKEN            ] = RuleNmtoken         ;
  g_rule[RULE_ENTITYVALUE        ] = RuleEntityValue     ;
  g_rule[RULE_ATTVALUE           ] = RuleAttValue        ;
  g_rule[RULE_SYSTEMLITERAL      ] = RuleSystemLiteral   ;
  g_rule[RULE_PUBIDLITERAL       ] = RulePubidLiteral    ;
  g_rule[RULE_CHARDATA           ] = RuleCharData        ;
  g_rule[RULE_COMMENT            ] = RuleComment         ;
  g_rule[RULE_PI                 ] = RulePI              ;
  g_rule[RULE_PITARGET           ] = RulePITarget        ;
  g_rule[RULE_CDSECT             ] = RuleCDSect          ;
  g_rule[RULE_PROLOG             ] = RuleProlog          ;
  g_rule[RULE_XMLDECL            ] = RuleXMLDecl         ;
  g_rule[RULE_VERSIONINFO        ] = RuleVersionInfo     ;
  g_rule[RULE_EQ                 ] = RuleEq              ;
  g_rule[RULE_VERSIONNUM         ] = RuleVersionNum      ;
  g_rule[RULE_MISC               ] = RuleMisc            ;
  g_rule[RULE_DOCTYPEDECL        ] = RuleDoctypedecl     ;
  g_rule[RULE_MARKUPDECL         ] = RuleMarkupdecl      ;
  g_rule[RULE_EXTSUBSET          ] = RuleExtSubset       ;
  g_rule[RULE_EXTSUBSETDECL      ] = RuleExtSubsetDecl   ;
  g_rule[RULE_SDDECL             ] = RuleSDDecl          ;
  g_rule[RULE_ELEMENT            ] = RuleElement         ;
  g_rule[RULE_ATTRIBUTE          ] = RuleAttribute       ;
  g_rule[RULE_CONTENT            ] = RuleContent         ;
  g_rule[RULE_ELEMENTDECL        ] = RuleElementdecl     ;
  g_rule[RULE_CONTENTSPEC        ] = RuleContentspec     ;
  g_rule[RULE_CHILDREN           ] = RuleChildren        ;
  g_rule[RULE_CP                 ] = RuleCp              ;
  g_rule[RULE_CHOICE             ] = RuleChoice          ;
  g_rule[RULE_MIXED              ] = RuleMixed           ;
  g_rule[RULE_ATTLISTDECL        ] = RuleAttlistDecl     ;
  g_rule[RULE_ATTDEF             ] = RuleAttDef          ;
  g_rule[RULE_ATTTYPE            ] = RuleAttType         ;
  g_rule[RULE_STRINGTYPE         ] = RuleStringType      ;
  g_rule[RULE_TOKENIZEDTYPE      ] = RuleTokenizedType   ;
  g_rule[RULE_ENUMERATEDTYPE     ] = RuleEnumeratedType  ;
  g_rule[RULE_NOTATIONTYPE       ] = RuleNotationType    ;
  g_rule[RULE_ENUMERATION        ] = RuleEnumeration     ;
  g_rule[RULE_DEFAULTDECL        ] = RuleDefaultDecl     ;
  g_rule[RULE_CONDITIONALSECT    ] = RuleConditionalSect ;
  g_rule[RULE_INCLUDESECT        ] = RuleIncludeSect     ;
  g_rule[RULE_IGNORESECT         ] = RuleIgnoreSect      ;
  g_rule[RULE_IGNORESECTCONTENTS ] = RuleIgnoreSectContents;
  g_rule[RULE_CHARREF            ] = RuleCharRef         ;
  g_rule[RULE_ENTITYREF          ] = RuleEntityRef       ;
  g_rule[RULE_PEREFERENCE        ] = RulePEReference     ;
  g_rule[RULE_ENTITYDECL         ] = RuleEntityDecl      ;
  g_rule[RULE_GEDECL             ] = RuleGEDecl          ;
  g_rule[RULE_PEDECL             ] = RulePEDecl          ;
  g_rule[RULE_ENTITYDEF          ] = RuleEntityDef       ;
  g_rule[RULE_PEDEF              ] = RulePEDef           ;
  g_rule[RULE_EXTERNALID         ] = RuleExternalID      ;
  g_rule[RULE_NDATADECL          ] = RuleNDataDecl       ;
  g_rule[RULE_TEXTDECL           ] = RuleTextDecl        ;
  g_rule[RULE_EXTPARSEDENT       ] = RuleExtParsedEnt    ;
  g_rule[RULE_EXTPE              ] = RuleExtPE           ;
  g_rule[RULE_ENCODINGDECL       ] = RuleEncodingDecl    ;
  g_rule[RULE_ENCNAME            ] = RuleEncName         ;
  g_rule[RULE_NOTATIONDECL       ] = RuleNotationDecl    ;
  g_rule[RULE_PUBLICID           ] = RulePublicID        ;
}

/*
=============================================================================
*/


/*  End of File  */

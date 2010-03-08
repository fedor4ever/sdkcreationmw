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
/* File    : U:\wpsvob\wmlenc\src\include\xmlrules.h
   Version : \main\6
   Date    : 17-Oct-00.12:51:57
*/

/*
 * ========================================================================
 *  Name        : xmlrules.h
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

    This header file contains the XML rule functions related
    macros and function declarations.

-----------------------------------------------------------------------------
*/

#ifndef XMLRULES_H
#define XMLRULES_H

#include "xmlpars.h"

/*  CONSTANTS  */

#define RULE_BAD                0
#define RULE_DOCUMENT           1
#define RULE_S                  3
#define RULE_NAME               5
#define RULE_NMTOKEN            7
#define RULE_ENTITYVALUE        9
#define RULE_ATTVALUE           10
#define RULE_SYSTEMLITERAL      11
#define RULE_PUBIDLITERAL       12
#define RULE_CHARDATA           14
#define RULE_COMMENT            15
#define RULE_PI                 16
#define RULE_PITARGET           17
#define RULE_CDSECT             18
#define RULE_PROLOG             22
#define RULE_XMLDECL            23
#define RULE_VERSIONINFO        24
#define RULE_EQ                 25
#define RULE_VERSIONNUM         26
#define RULE_MISC               27
#define RULE_DOCTYPEDECL        28
#define RULE_MARKUPDECL         29
#define RULE_EXTSUBSET          30
#define RULE_EXTSUBSETDECL      31
#define RULE_SDDECL             32
#define RULE_ELEMENT            39
#define RULE_ATTRIBUTE          41
#define RULE_CONTENT            43
#define RULE_ELEMENTDECL        45
#define RULE_CONTENTSPEC        46
#define RULE_CHILDREN           47
#define RULE_CP                 48
#define RULE_CHOICE             49
#define RULE_MIXED              51
#define RULE_ATTLISTDECL        52
#define RULE_ATTDEF             53
#define RULE_ATTTYPE            54
#define RULE_STRINGTYPE         55
#define RULE_TOKENIZEDTYPE      56
#define RULE_ENUMERATEDTYPE     57
#define RULE_NOTATIONTYPE       58
#define RULE_ENUMERATION        59
#define RULE_DEFAULTDECL        60
#define RULE_CONDITIONALSECT    61
#define RULE_INCLUDESECT        62
#define RULE_IGNORESECT         63
#define RULE_IGNORESECTCONTENTS 64
#define RULE_CHARREF            66
#define RULE_ENTITYREF          68
#define RULE_PEREFERENCE        69
#define RULE_ENTITYDECL         70
#define RULE_GEDECL             71
#define RULE_PEDECL             72
#define RULE_ENTITYDEF          73
#define RULE_PEDEF              74
#define RULE_EXTERNALID         75
#define RULE_NDATADECL          76
#define RULE_TEXTDECL           77
#define RULE_EXTPARSEDENT       78
#define RULE_EXTPE              79
#define RULE_ENCODINGDECL       80
#define RULE_ENCNAME            81
#define RULE_NOTATIONDECL       82
#define RULE_PUBLICID           83

#define N_OF_RULE               90

/*  MACROS  */

/*  DATA TYPES  */

typedef int (*RuleFunction)(PARDATA);

/*  EXTERNAL DATA STRUCTURES  */
extern const char *g_nameOfRule[N_OF_RULE];
extern RuleFunction g_rule[N_OF_RULE];

/*  FUNCTION PROTOTYPES  */

int RuleDocument            (PARDATA );
int RuleS                   (PARDATA );
int RuleName                (PARDATA );
int RuleNmtoken             (PARDATA );
int RuleEntityValue         (PARDATA );
int RuleAttValue            (PARDATA );
int RuleSystemLiteral       (PARDATA );
int RulePubidLiteral        (PARDATA );
int RuleCharData            (PARDATA );
int RuleComment             (PARDATA );
int RulePI                  (PARDATA );
int RulePITarget            (PARDATA );
int RuleCDSect              (PARDATA );
int RuleProlog              (PARDATA );
int RuleXMLDecl             (PARDATA );
int RuleVersionInfo         (PARDATA );
int RuleEq                  (PARDATA );
int RuleVersionNum          (PARDATA );
int RuleMisc                (PARDATA );
int RuleDoctypedecl         (PARDATA );
int RuleMarkupdecl          (PARDATA );
int RuleExtSubset           (PARDATA );
int RuleExtSubsetDecl       (PARDATA );
int RuleSDDecl              (PARDATA );
int RuleElement             (PARDATA );
int RuleAttribute           (PARDATA );
int RuleContent             (PARDATA );
int RuleElementdecl         (PARDATA );
int RuleContentspec         (PARDATA );
int RuleChildren            (PARDATA );
int RuleCp                  (PARDATA );
int RuleChoice              (PARDATA );
int RuleMixed               (PARDATA );
int RuleAttlistDecl         (PARDATA );
int RuleAttDef              (PARDATA );
int RuleAttType             (PARDATA );
int RuleStringType          (PARDATA );
int RuleTokenizedType       (PARDATA );
int RuleEnumeratedType      (PARDATA );
int RuleNotationType        (PARDATA );
int RuleEnumeration         (PARDATA );
int RuleDefaultDecl         (PARDATA );
int RuleConditionalSect     (PARDATA );
int RuleIncludeSect         (PARDATA );
int RuleIgnoreSect          (PARDATA );
int RuleIgnoreSectContents  (PARDATA );
int RuleIgnore              (PARDATA );
int RuleCharRef             (PARDATA );
int RuleEntityRef           (PARDATA );
int RulePEReference         (PARDATA );
int RuleEntityDecl          (PARDATA );
int RuleGEDecl              (PARDATA );
int RulePEDecl              (PARDATA );
int RuleEntityDef           (PARDATA );
int RulePEDef               (PARDATA );
int RuleExternalID          (PARDATA );
int RuleNDataDecl           (PARDATA );
int RuleTextDecl            (PARDATA );
int RuleExtParsedEnt        (PARDATA );
int RuleExtPE               (PARDATA );
int RuleEncodingDecl        (PARDATA );
int RuleEncName             (PARDATA );
int RuleNotationDecl        (PARDATA );
int RulePublicID            (PARDATA );

void initRuleFunctions(void);

#endif  /* XMLRULES_H */

/*  End of File  */

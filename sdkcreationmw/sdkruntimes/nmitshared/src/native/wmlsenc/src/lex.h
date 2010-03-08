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
/* File    : U:\wpsvob\wmlscript\work\ported\lex.h
   Version : \main\4
   Date    : 01-Sep-99.10:35:29
*/

/*
 * ========================================================================
 *  Name        : lex.h
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

      WMLScript lexer definitions and interface function declarations.

-----------------------------------------------------------------------------
*/

#ifndef LEX_H
#define LEX_H


#define TOKEN_END               0
#define NOQUOTE_STRING_LITERAL  999

#define RESERVED_NONE       0
#define RESERVED_FUTURE     1
#define RESERVED_TRUE       2
#define RESERVED_FALSE      3
#define RESERVED_INVALID    4


struct LexStruct
{
    BYTE*   pbData;         /* pointer to script data */
    char*   pcTokenText;    /* pointer to pbData where identifier or literal starts */
    char*   pcTokenTextPrev;/* pointer to pbData where previous identifier or literal starts */
 
	int     iDataLen;       /* len of pbData buffer */
    int     iDataIndex;     /* current parsing index to pbdata */
    
    int     iToken;         /* recognized token */
    int     iPrevToken;     /* previous token */
    int     iReserved;      /* RESERVED_NONE, RESERVED_FUTURE, RESERVED_TRUE,
                               RESERVED_FALSE or RESERVED_INVALID */
    
    int     iTokenTextLen;  /* len of pcTokenText */
    int     iTokenColumn;   /* which column pcTokenText started */

    int     iTokenTextLenPrev;  /* len of previous pcTokenText */
    int     iReservedPrev;      /* was previous keyword reserved */

    int     iRow;           /* current row while parsing */
    int     iColumn;        /* current column while parsing */
    int     iStatus;        /* current parsing status */
};


/* global function protos */
void    LexInitFile(struct LexStruct* pLex, char* pcFilename);
void    LexInit();
void    InitializeTables();


/*int       ParseNext(struct LexStruct* pLex);*/
int     yylex(struct LexStruct* pLex);
void    LexCleanUp(struct LexStruct* pLex);


#endif



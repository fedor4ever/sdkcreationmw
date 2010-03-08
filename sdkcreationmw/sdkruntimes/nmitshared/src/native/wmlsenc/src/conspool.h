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
/* File    : U:\wpsvob\wmlscript\work\ported\conspool.h
   Version : \main\4
   Date    : 01-Sep-99.10:35:24
*/

/*
 * ========================================================================
 *  Name        : conspool.h
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

      Definitions and interface function declarations for constants pool.

-----------------------------------------------------------------------------
*/
#if !defined(TABLES_H)
#define TABLES_H

#include "symtable.h"


#define MEMORY_ERROR -99


#define CONST_TYPE_M1       -1
#define CONST_TYPE_0        -2
#define CONST_TYPE_1        -3
#define CONST_TYPE_ES       -4
#define CONST_TYPE_INVALID  -5
#define CONST_TYPE_TRUE     -6
#define CONST_TYPE_FALSE    -7

#define CONST_NOT_FOUND     -8

#define CONST_VALUE     300
#define CONST_STRING    301

typedef enum
{
    VARTYPE_INT8,
    VARTYPE_INT16,
    VARTYPE_INT32,
    VARTYPE_FLOAT,
    VARTYPE_STRING,
    VARTYPE_STRING_EMPTY,
    VARTYPE_STRING_EXTERNAL,
    VARTYPE_EXTERNAL
} VariableType;


/* global protos */
int     ConstTableAddString(struct HashTable* pTable, char* pcValue, BOOL bCheckInternal);
int     ConstTableAddValue(struct HashTable* pTable, char* pcValue);

int     ConstTableGenerateBytecode(struct HashTable* pTable, struct StorageStruct* pStorage);

BYTE    DecValidate(char* pcValue);
BYTE    FloatValidate(char* pcValue);
BYTE    HexValidate(char* pcHex);
BYTE    OctValidate(char* pcOct);

INT32   HexToDec(char* pcHex);
INT32   OctToDec(char* pcOct);

#endif

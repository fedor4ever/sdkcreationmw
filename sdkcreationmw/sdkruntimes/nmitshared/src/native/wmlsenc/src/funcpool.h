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
/* File    : U:\wpsvob\wmlscript\work\ported\funcpool.h
   Version : \main\3
   Date    : 01-Sep-99.10:35:26
*/

/*
 * ========================================================================
 *  Name        : funcpool.h
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

      Function pool definitions and interface function declarations.

-----------------------------------------------------------------------------
*/
#if !defined(FUNCPOOL_H)
#define FUNCPOOL_H



#define FUNCTION_EXTERNAL       1
#define FUNCTION_INTERNAL       2
#define FUNCTION_NOT_DECLARED   3




/* global protos */
INT32   FunctionCompile(struct Environment* pEnv, struct StorageStruct* pStorage, struct CodeNode* pNode);
int     ValidateFunctionTable(struct Environment* pEnv);
int     FunctionTableGenerateBytecode(struct HashTable* pFunctionTable, struct StorageStruct* pStorage);
void    FunctionTableCleanUp(struct HashTable* pFunctionTable);
INT16   GetJumpCodeSize(INT16 iSize);

#endif

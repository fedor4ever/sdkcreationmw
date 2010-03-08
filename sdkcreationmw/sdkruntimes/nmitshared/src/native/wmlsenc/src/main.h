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
/* File    : U:\wpsvob\wmlscript\work\ported\main.h
   Version : \main\10
   Date    : 22-Nov-00.17:24:58
*/

/*
 * ========================================================================
 *  Name        : main.h
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

      Contains collective structs and definitions for Bison, node tree and
      interpreter parts of the program.

-----------------------------------------------------------------------------
*/

#if !defined(MAIN_H)
#define MAIN_H


/* memory wrappers for better portability */
#define WMLS_malloc(size)			WSL_OS_Malloc( size )
#define WMLS_alignmalloc(size)		WSL_OS_Malloc( size )
#define WMLS_free(pointer)			WSL_OS_Free( pointer )


#define SIZE_OF_PERMANENT_MEMORY_NODE   2000
#define SIZE_OF_TEMPORARY_MEMORY_NODE   2000
#define HASH_TABLE_SIZE                 100
#define FUNCTION_NAME_MAXLENGTH			255

#define WMLSCRIPT_VERSION_MAJOR 1
#define WMLSCRIPT_VERSION_MINOR 1

#include "wps_service.h"

#include "wmlsapi.h"
#include "memmana.h"
#include "symtable.h"
#include "pragpool.h"
#include "bytecode.h"
#include "wmlsyacc.h"
#include "lex.h"

/* these are used to track down illegal function calls in case
   called function is not yet declared */
struct FuncCall
{
    INT16   iRow;
    INT16   iFunctionID;
    INT16   iNumArguments;
    struct FuncCall* pNext;
};


struct Environment
{
    /* memory manager nodes */
    struct MemoryNode* pMemPermanent;
    struct MemoryNode* pMemTemporary;

    /* hashtables */
    struct HashTable Constants;
    struct HashTable Variables;
    struct HashTable Functions;

    /* pragmas */
    struct Pragma*  pPragmas;

    /* pointer to currently compliled function */
    struct HashNode* pCurrentFunction;
    struct CodeNode* pTemp;
    struct CodeNode* pCurrentLoop;

    /*storage structs */
    struct StorageStruct StorageConstants;
    struct StorageStruct StoragePragmas;
    struct StorageStruct StorageFunctions;

    struct LexStruct lex;
   
    UINT32* metaNameStrings;
    BYTE*   metaNameBuffer;
    UINT32* httpEquivStrings;
    BYTE*   httpEquivBuffer;
    char*   pcErrorInfo;
    struct  FuncCall* pCalls;

	INT16   metaNameBufferSize;
    INT16   httpEquivBufferSize;
 
    WMLS_STATUS status;
    WMLS_STATUS warning;
    INT16       iErrorLineNumber;
    INT16       iErrorColumnNumber;
 
};

void *WMLS_realloc(void *pointer, size_t size, size_t old_size);


#endif


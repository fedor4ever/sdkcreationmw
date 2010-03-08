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
/* File    : U:\wpsvob\wmlscript\work\ported\symtable.h
   Version : \main\6
   Date    : 13-Sep-99.12:29:22
*/

/*
 * ========================================================================
 *  Name        : symtable.h
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

      Hash table definitions and interface function declarations.

-----------------------------------------------------------------------------
*/

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "common.h"
#include "memmana.h"


#define HASH_TABLE_STORE_MODE_ERROR 1
#define HASH_TABLE_ITEM_NOT_FOUND   2
#define HASH_TABLE_OUT_OF_MEMORY    3



typedef enum
{
    STORE_MODE_MAY_EXIST,
    STORE_MODE_MAY_NOT_EXIST
} StoreMode;



struct HashNode
{
    char*               pcKey;          /* pointer to value */
    struct HashNode*    pNext;          /* pointer to next node or NULL  */
    BYTE*               pbCode;         /* code for function */

    INT32               iIndex;         /* unique index for this node */
    INT32               iExtraKey;      /* used in identification, otherwise store extra data */
    INT32               iExtra;	        /* store extra data */
    BYTE                bNumArguments;  /* how many arguments this function has */
    BYTE                bNumVariables;  /* how many local variables this function has */
    INT16               iCodeSize;      /* size of the code */
};


struct HashTable
{
    struct HashNode**   ppNodes;        /* pointer to iTableSize allocated HashNode pointers */
    struct MemoryNode*  pMemoryNode;    /* pointer to struct that is used to allocate memory for strings */
    INT16               iTableSize;     /* size of the hash table */
    
    INT16               iNumItems;      /* how many items is currently in table */
    INT16               iLastError;     /* place holder for last error */
};





INT16               HashTableInitialize(struct HashTable* pTable, INT16 iTableSize, struct MemoryNode* pMem);
void                HashTableRemoveNodes(struct HashTable* pTable);
void                HashTableCleanUp(struct HashTable* pTable);

struct HashNode*    HashTableStore(struct HashTable* pTable, char* pcKey, INT32 iExtraKey, StoreMode mode);
struct HashNode*    HashTableFind(struct HashTable* pTable, char* pcKey, INT32 iExtraKey);

struct HashNode**   GetItemsInOrder(struct HashTable* pTable, INT16 *piNumItems);
struct HashNode*    HashTableFindByIndex(struct HashTable* pTable, INT16 iIndex);


#endif

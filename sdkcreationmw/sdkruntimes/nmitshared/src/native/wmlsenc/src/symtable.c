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
/* File    : U:\wpsvob\wmlscript\work\ported\symtable.c
   Version : \main\6
   Date    : 28-Feb-01.13:04:20
*/

/*
 * ========================================================================
 *  Name        : symtable.c
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

      Hash table handling.

-----------------------------------------------------------------------------
*/
#include "main.h"
#include "common.h"
#include "memmana.h"
#include "symtable.h"


/* local prototypes */
INT16 HashTableCalculateValue(char* pcKey, int iMaxSize);
struct HashNode* HashTableFindByHashValue(struct HashTable* pTable, INT16 iKey, char* pcKey, INT32 iExtraKey);








/***************************************************************************
                            PUBLIC FUNCTIONS
***************************************************************************/
     

/*********************************************************************

 FUNCTION     : HashTableInitialize

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: INT16 

 PARAMETERS:
        struct HashTable* pTable    -in: pointer to HashTable struct
        INT16 iTableSize            -in: size of the table
        struct MemoryNode* pMem     -in: pointer to MemoryNode where
                                    all the memory allocations are made

 DESCRIPTION:
        Initializes HahsTable.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
INT16 HashTableInitialize(struct HashTable* pTable, INT16 iTableSize, struct MemoryNode* pMem)
{
    INT16   iRet = WPSS_OUT_OF_MEMORY;
    int     i;

    if(pTable)
    {
        /* initialize counter */
        pTable->iNumItems = 0;
        
        pTable->iTableSize = iTableSize;

        /* create hashtable */
        pTable->ppNodes = (struct HashNode**) WMLS_malloc(iTableSize * sizeof(struct HashNode*));

#ifdef MEMORY_OUT_TEST
        if(rand()%MEMORY_MAX < MEMORY_LIMIT)
        {
            if(pTable->ppNodes)
            {
                WMLS_free(pTable->ppNodes);
                pTable->ppNodes = NULL;
            }
        }
#endif

        if(pTable->ppNodes)
        {
            iRet = WPSS_NO_ERROR;

            /* initialize hash table */
            for(i=0;i<pTable->iTableSize;i++)
                pTable->ppNodes[i] = NULL;

            /* enable usage of memory manager nodes */
            pTable->pMemoryNode = pMem;
            
        }
    }

    return iRet;
}





/*********************************************************************

 FUNCTION     : HashTableCleanUp

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct HashTable* pTable

 DESCRIPTION:
        Frees any memory reserved by HashTable.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void HashTableCleanUp(struct HashTable* pTable)
{
    if(pTable)
    {
        /* free hashtable, nodes are freed with memory nodes */
        if(pTable->ppNodes)
        {
            WMLS_free(pTable->ppNodes);
            pTable->ppNodes = NULL;
        }
    }
}




/*********************************************************************

 FUNCTION     : HashTableStore

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct HashNode* 

 PARAMETERS:
        struct HashTable* pTable
        char* pcKey                 -in: key string
        INT32 iExtraKey             -in: extra info
        StoreMode mode              -in: mode

 DESCRIPTION:
        Adds new data to HashTable or returns pointer to existing
        hashnode. If store mode is STORE_MODE_MAY_NOT_EXIST then
        error is returned if already exists.
    
 NOTE: 

 CHANGES: 
            210699 JoS Paranoia check added to make sure that iHashValue is positive.

*********************************************************************/
struct HashNode* HashTableStore(struct HashTable* pTable, char* pcKey, INT32 iExtraKey, StoreMode mode)
{
    struct HashNode*    pRet = NULL;
    INT16               iHashValue;


    iHashValue = HashTableCalculateValue(pcKey,pTable->iTableSize);

     
    /* first check if we already have this key */
    pRet = HashTableFindByHashValue(pTable, iHashValue, pcKey, iExtraKey);

    /* check that there is for instance no double declaration for variable */
    if( pRet && (mode == STORE_MODE_MAY_NOT_EXIST) )
    {
        pTable->iLastError = HASH_TABLE_STORE_MODE_ERROR;
        return NULL;
    }

    if(pRet == NULL)
    {
        /* if error occurs it is this */
        pTable->iLastError = HASH_TABLE_OUT_OF_MEMORY;
        

        /* If there can be more elements, insert it to the hash table. 
         * Check wether the number of items is overflowed or not (max value of INT16).
         * If it is return NULL (the error is HASH_TABLE_OUT_OF_MEMORY). */
        if (pTable->iNumItems < 32767) {
            pRet = (struct HashNode*) MemoryAlloc(pTable->pMemoryNode,sizeof(struct HashNode));
        }

        if(pRet)
        {
            pRet->pcKey = (char*) MemoryAlloc(pTable->pMemoryNode,(INT16)(strlen(pcKey)+1));

            if(pRet->pcKey)
            {
                strcpy(pRet->pcKey,pcKey);
                
                pRet->pNext = NULL;
                pRet->iExtraKey = iExtraKey;
                pRet->iExtra = 0;
                pRet->bNumArguments = 0;
                pRet->bNumVariables = 0;
                pRet->pbCode = NULL;
                pRet->iCodeSize = 0;

                pRet->iIndex = (pTable->iNumItems)++;
    


                /* link to hash table */
                if(pTable->ppNodes[iHashValue])
                {
                    /* put it to the first node  */
                    pRet->pNext = pTable->ppNodes[iHashValue];
                }
                
                pTable->ppNodes[iHashValue] = pRet;
            }
        }
    }
    

    return pRet;
}



/*********************************************************************

 FUNCTION     : HashTableFind

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct HashNode* 

 PARAMETERS:
        struct HashTable* pTable
        char* pcKey
        INT32 iExtraKey

 DESCRIPTION:
        Returns pointer to existing node.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct HashNode* HashTableFind(struct HashTable* pTable, char* pcKey, INT32 iExtraKey)
{   
    INT16 iHashValue = HashTableCalculateValue(pcKey,pTable->iTableSize);

    return  HashTableFindByHashValue(pTable, iHashValue, pcKey, iExtraKey);
}



/*********************************************************************

 FUNCTION     : HashTableFindByIndex

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct HashNode* 

 PARAMETERS:
        struct HashTable* pTable
        INT16 iIndex

 DESCRIPTION:
        Returns pointer to existing node.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct HashNode* HashTableFindByIndex(struct HashTable* pTable, INT16 iIndex)
{   
    int                 i = 0;
    struct HashNode*    pNode = NULL;
    
    while(i<pTable->iTableSize)
    {
        pNode = pTable->ppNodes[i++];

        while(pNode)
        {
            if(pNode->iIndex == iIndex)
                return pNode;

            pNode = pNode->pNext;
        }
    }

    return NULL;
}




/*********************************************************************

 FUNCTION     : HashTableRemoveNodes

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct HashTable* pTable

 DESCRIPTION:
        Empties hashtable. No memory is freed, because MemoryNodes
        handle alllocation and deallocation
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void HashTableRemoveNodes(struct HashTable* pTable)
{
    int i;

    if(pTable)
    {
        for(i=0; i < pTable->iTableSize; i++)
            pTable->ppNodes[i] = NULL;  

            pTable->iNumItems=0;
    }


}



/*********************************************************************

 FUNCTION     : GetItemsInOrder

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct HashNode** 

 PARAMETERS:
        struct HashTable* pTable
        INT16 *piNumItems

 DESCRIPTION:
        Returns table containing all items sorted by iIndex.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct HashNode** GetItemsInOrder(struct HashTable* pTable, INT16 *piNumItems)
{
    int                 i = 0;
    struct HashNode* pNode=NULL;    
    struct HashNode** pItems=NULL;

    /* is there anything to do */
    if( (pTable->iNumItems == 0) || (pTable->ppNodes == NULL) )
        return NULL;

    
    pItems  = (struct HashNode**) WMLS_malloc( sizeof(void*) * pTable->iNumItems);

#ifdef MEMORY_OUT_TEST
    if(rand()%MEMORY_MAX < MEMORY_LIMIT)
    {
        if(pItems)
        {
            WMLS_free(pItems);
            pItems = NULL;
        }
    }
#endif

    if(pItems)
    {
        *piNumItems = pTable->iNumItems;
        
        while(i<pTable->iTableSize)
        {
            pNode = pTable->ppNodes[i++];

            while(pNode)
            {
                pItems[pNode->iIndex] = pNode;

                pNode = pNode->pNext;
            }
        }
    }

    return pItems;
}





/***************************************************************************
                            LOCAL FUNCTIONS
***************************************************************************/


/*********************************************************************

 FUNCTION     : HashTableFindByHashValue

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct HashNode* 

 PARAMETERS:
        struct HashTable* pTable
        INT16 iKey                  -in: hash value for item
        char* pcKey                 -in: key string
        INT32 iExtraKey             -in: extra info

 DESCRIPTION:
        Returns existing node from hahs table.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct HashNode* HashTableFindByHashValue(struct HashTable* pTable, INT16 iKey, char* pcKey, INT32 iExtraKey)
{   
    struct HashNode* pNode = NULL;

    pNode = pTable->ppNodes[iKey];

    while(pNode)
    {
        if(iExtraKey == pNode->iExtraKey )
        {
            if(strcmp(pNode->pcKey,pcKey) == 0)
            {       
                break;
            }
        }

        pNode = pNode->pNext;
    }

    return pNode;
}





/*********************************************************************

 FUNCTION     : HashTableCalculateValue

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: INT16 

 PARAMETERS:
        char* pcKey
        int iMaxSize

 DESCRIPTION:
        Calculates hash value for key string. Returned number is integer
        between 0 and iMaxSize-1
    
 NOTE: 

 CHANGES: 

*********************************************************************/
INT16 HashTableCalculateValue(char* pcKey, int iMaxSize)
{
    UINT16  uiValue = 0;
    int     i=0;

    while(pcKey[i])
    {
        uiValue += pcKey[i] * (i+1);
        i++;
    }

    return (INT16)(uiValue % iMaxSize);
}





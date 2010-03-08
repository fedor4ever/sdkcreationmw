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
/* File    : U:\wpsvob\wmlscript\work\ported\pragpool.c
   Version : \main\6
   Date    : 22-Nov-00.17:25:00
*/

/*
 * ========================================================================
 *  Name        : pragpool.c
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

      Pragma pool's node creation, initialisation, linking and resource freeing.
      Also generates byte code out of pragma pool. 

-----------------------------------------------------------------------------
*/
#include "main.h"
#include "common.h"
#include "bytecode.h"
#include "pragpool.h"



/* local protos */
struct Pragma* PragmaTableGetNew(struct Pragma** ppPragmas);







/***************************************************************************
                            PUBLIC FUNCTIONS
***************************************************************************/



/*********************************************************************

 FUNCTION     : PragmaTableCleanUp

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct Pragma* pPragmas

 DESCRIPTION:
        Frees any memory reserved by pragma table
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void PragmaTableCleanUp(struct Pragma* pPragmas)
{
    struct Pragma* p;

    while(pPragmas)
    {
        p = pPragmas->pNext;
        WMLS_free(pPragmas);
        
        pPragmas = p;
    }

}




/*********************************************************************

 FUNCTION     : PragmaTableAdd

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: INT16 

 PARAMETERS:
        struct Pragma** ppPragmas
        PragmaType type             -in: Enumered pragma type
        INT16 iIndex1               -in: pragma specific value
        INT16 iIndex2               -in: pragma specific value
        INT16 iIndex3               -in: pragma specific value

 DESCRIPTION:
        Appends any of the supported pragma types to pragma table.
        Pragma table is linked list of struct Pragma
    
 NOTE: 

 CHANGES: 

*********************************************************************/
INT16 PragmaTableAdd(struct Pragma** ppPragmas, PragmaType type, INT16 iIndex1, INT16 iIndex2, INT16 iIndex3)
{
    INT16 iRet = MEMORY_ERROR;

    struct Pragma* pNew = PragmaTableGetNew(ppPragmas);

    if(pNew)
    {
        iRet = RET_OK;

        pNew->type = type;
        pNew->iIndex1 = iIndex1;
        pNew->iIndex2 = iIndex2;
        pNew->iIndex3 = iIndex3;
    }

    return iRet;
}



/*********************************************************************

 FUNCTION     : PragmaTableGenerateBytecode

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: INT16 

 PARAMETERS:
        struct Pragma* pPragmas
        struct StorageStruct* pStorage

 DESCRIPTION:
        Generates bytecode from pragma table.
    
 NOTE: 

 CHANGES: 
		Pragma type ACCESS_CONTROl_DISABLED removed.	JOS/CCC 040799


*********************************************************************/
INT16 PragmaTableGenerateBytecode(struct Pragma* pPragmas, struct StorageStruct* pStorage)
{
    INT16       iNumPragmas = 0;
    struct Pragma* p;
    

    if(pPragmas)
    {
        iNumPragmas++;

        p = pPragmas->pNext;

        while(p)
        {
            p = p->pNext;
            iNumPragmas++;
        }
    }
    
    /* Initialize PragmaPool */
    if(ByteCodeInitialize(pStorage,50,20)==RET_OK)
    {
    
        /* add number of Pragmas */
        ByteCodeAppendMBInt((UINT32)iNumPragmas, pStorage);

        /* add Pragmas one by one */
        p = pPragmas;

        while(p)
        {
            ByteCodeAppendByte( (BYTE) p->type, pStorage);
            
            switch(p->type)
            {
            case PRAGMA_ACCESS_DOMAIN:
            case PRAGMA_ACCESS_PATH:
                ByteCodeAppendMBInt((UINT32)p->iIndex1, pStorage);
                break;

                    
            case PRAGMA_USER_AGENT_PROPERTY:
                ByteCodeAppendMBInt((UINT32)p->iIndex1, pStorage);
                ByteCodeAppendMBInt((UINT32)p->iIndex2, pStorage);
                break;
                    
            case PRAGMA_USER_AGENT_PROPERTY_AND_SCHEME:
                ByteCodeAppendMBInt((UINT32)p->iIndex1, pStorage);
                ByteCodeAppendMBInt((UINT32)p->iIndex2, pStorage);
                ByteCodeAppendMBInt((UINT32)p->iIndex3, pStorage);
                break;

            }

            p = p->pNext;
        }

        ByteCodeResize(pStorage);
    }

    return ByteCodeGetStatus(pStorage);
}














/***************************************************************************
                            LOCAL FUNCTIONS
***************************************************************************/



/*********************************************************************

 FUNCTION     : PragmaTableGetNew

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: struct Pragma* 

 PARAMETERS:
        struct Pragma** ppPragmas

 DESCRIPTION:
        Creates new pragma node and links it to linked list.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct Pragma* PragmaTableGetNew(struct Pragma** ppPragmas)
{
    struct Pragma*  p;
    struct Pragma* pNewPragma = (struct Pragma*) WMLS_malloc( sizeof(struct Pragma) );
    
#ifdef MEMORY_OUT_TEST
    if(rand()%MEMORY_MAX < MEMORY_LIMIT)
    {
        if(pNewPragma)
        {
            WMLS_free(pNewPragma);
            pNewPragma = NULL;
        }
    }
#endif

    if(pNewPragma)
    {
        pNewPragma->pNext = NULL;
        
        if(*ppPragmas)
        {
            /* add to last node */
            p = (*ppPragmas);

            while(p->pNext)
            {
                p = p->pNext;       
            }
            
            p->pNext = pNewPragma;
        }
        else
        {
            *ppPragmas = pNewPragma;
        }
    }
    
    
    return pNewPragma;
}

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
/* File    : U:\wpsvob\wmlscript\work\ported\memmana.c
   Version : \main\8
   Date    : 22-Nov-00.17:24:59
*/

/*
 * ========================================================================
 *  Name        : memmana.c
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

      Memory management functions for reserving, initialising and freeing 
      nodes.

-----------------------------------------------------------------------------
*/
#include "wps_service.h"

#include "main.h"
#include "common.h"
#include "memmana.h"


/***************************************************************************
                            PUBLIC FUNCTIONS
***************************************************************************/



/*********************************************************************

 FUNCTION     : MemoryDealloc

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct MemoryNode* pNodes

 DESCRIPTION:
        Frees all but first memory node, which is marked as not used.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void MemoryDealloc(struct MemoryNode* pNodes)
{
    struct MemoryNode* pCurrent = pNodes;

    if(pCurrent)
    {
        /* first node is not used */
        pCurrent->iUsed = 0;
        
        /* free all but first node */
        MemoryDelete(pCurrent->pNext);

        /* mark that no more nodes exist anymore */
        pCurrent->pNext = NULL;
    }

}



/*********************************************************************

 FUNCTION     : MemoryDelete

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: void 

 PARAMETERS:
        struct MemoryNode* pNodes

 DESCRIPTION:
        Frees all memory reserved by memory nodes. Then frees also
        memory nodes.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void MemoryDelete(struct MemoryNode* pNodes)
{
    struct MemoryNode* pCurrent = pNodes;
    struct MemoryNode* pNext;

    while(pCurrent)
    {
        pNext = pCurrent->pNext;
        
        /* free node buffer */
        if(pCurrent->pbBuffer)
        {
            WMLS_free(pCurrent->pbBuffer);
            pCurrent->pbBuffer = NULL;

        }

        /* free node */
        WMLS_free(pCurrent);

        pCurrent = pNext;
    }
}



/*********************************************************************

 FUNCTION     : MemoryAllocNewNode

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN:        struct MemoryNode*  OR NULL

 PARAMETERS:
        INT16 iSize     -in: size of memory to allocate in node

 DESCRIPTION:
        Allocates new memory node with given size.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
struct MemoryNode*  MemoryAllocNewNode(INT16 iSize)
{
    struct MemoryNode* pCurrent;
    
    /* allocate memory for node */
    pCurrent = (struct MemoryNode*) WMLS_malloc(sizeof(struct MemoryNode));

#ifdef MEMORY_OUT_TEST
    if(rand()%MEMORY_MAX < MEMORY_LIMIT)
    {
        if(pCurrent)
        {
            WMLS_free(pCurrent);
            pCurrent = NULL;
        }
    }
#endif

    /* out of memory -> return NULL */
    if(pCurrent)
    {
        /* allocate memory for node buffer */
        pCurrent->pbBuffer = (BYTE*) WMLS_malloc(iSize);

        if(pCurrent->pbBuffer)
        {
            pCurrent->iTotalSize = iSize;
            pCurrent->iUsed = 0;
            pCurrent->pNext = NULL;
        }
        else
        {
            /* could not allocate memory for buffer -> free node and return NULL */
            WMLS_free(pCurrent);
            pCurrent = NULL;
        }
    }

    return pCurrent;
}



/*********************************************************************

 FUNCTION     : MemoryAlloc

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: BYTE* 

 PARAMETERS:
        struct MemoryNode* pNodes
        INT16 iSize                 -in: size to allocate

 DESCRIPTION:
        Reserves memory block from memory node. If existing nodes
        do not have enough memory allocates new node.
    
 NOTE: 

 CHANGES: 


*********************************************************************/

#define ALLIGNCOUNT (sizeof(void *))
#define ALLIGNMASK  (ALLIGNCOUNT - 1)

BYTE* MemoryAlloc(struct MemoryNode* pNodes, INT16 par_iSize)
{
    BYTE*               pBuffer = NULL;
    struct MemoryNode*  pCurrent = pNodes;
    long                iSizeToAllocate;
    long		iSize = par_iSize + ALLIGNCOUNT - 1;
    
    long                iAvailable = pCurrent->iTotalSize - pCurrent->iUsed;
    
    while( iAvailable<iSize )
    {   
        /* is there space in next node */
        if(pCurrent->pNext)
        {
            pCurrent = pCurrent->pNext;
            iAvailable = pCurrent->iTotalSize - pCurrent->iUsed;
        }
        else 
        {   
            /* no next node, we need new container object */
            iSizeToAllocate = pCurrent->iTotalSize;

            /* if asking more than default size -> be nice and do it */
            if(iSize > iSizeToAllocate)
                iSizeToAllocate = iSize;

            pCurrent->pNext = MemoryAllocNewNode((UINT16)iSizeToAllocate);

            if(pCurrent->pNext == NULL) 
                return NULL;

            pCurrent = pCurrent->pNext;
            iAvailable = pCurrent->iTotalSize - pCurrent->iUsed;
        }
    }

    /* there is room allocate and update target pointer */
    if(pCurrent->pbBuffer)
    {
        pBuffer = pCurrent->pbBuffer + pCurrent->iUsed;
        pCurrent->iUsed += (UINT16)iSize;
    }

    if(( (long)pBuffer & ALLIGNMASK ) == 0 )
      return pBuffer;
    else return (BYTE *)(( (long)pBuffer + ALLIGNCOUNT - 1 ) & ~ALLIGNMASK );
}


/*********************************************************************

 FUNCTION     : MemoryAlloc

**********************************************************************

 AUTHOR:        Gabor Valyi Nagy
 DATE:          23-May-2000

 RETURN: BYTE* 

 PARAMETERS:
			pointer:	old pointer to be released
			size:		size of the new buffer needed
			old_size:	size of old buffer

 DESCRIPTION:
			Reallocate the memory. The new memory contains the old 
			buffer. It returns NULL if out of memory.

    
 NOTE: 

 CHANGES: 


*********************************************************************/

void *WMLS_realloc(void *pointer, size_t size, size_t old_size)	
{
	void *pNew = NULL;
	/* If the size is 0 and the original pointer is not null */
	if (size == 0 && pointer != NULL)
	{
		pNew = NULL;
		WSL_OS_Free(pointer);
	}
	else
	{
		pNew = WSL_OS_Malloc(size);
		/* if there is no space to allocate buffer, don't free the old buffer */
		if (pNew != NULL)
		{
			if (old_size < size)
			{
				memcpy(pNew, pointer, old_size);
			}
			else
			{
				memcpy(pNew, pointer, size);
			}
	
			WSL_OS_Free(pointer);
		}
	}

	return pNew;
}



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
/* File    : U:\wpsvob\wmlscript\work\ported\main.c
   Version : \main\12
   Date    : 22-Nov-00.17:24:58
*/

/*
 * ========================================================================
 *  Name        : main.c
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

      WMLSCript Encoder API for initialise, call and free resources from 
      external application.

-----------------------------------------------------------------------------
*/
#include "common.h"
#include "main.h"
#include "codenode.h"
#include "memmana.h"
#include "symtable.h"
#include "pragpool.h"
#include "bytecode.h"
#include "conspool.h"
#include "funcpool.h"
#include "library.h"

#include "lex.h"
#include "wmlsyacc.h"
#include "wmlsapi.h"
#include "wps_service.h"

/* external function proto */
int yyparse(struct Environment *env);

/* external variables */
extern int yydebug;



/***************************************************************************
                            STATIC VARIABLES
***************************************************************************/

static WSL_Mutex WMLS_countlock = NULL;
static INT16 WMLS_count = 0;
static BYTE WMLS_runflag = 0;

/***************************************************************************
                            PUBLIC FUNCTIONS
***************************************************************************/




/*********************************************************************

 FUNCTION     : WMLS_Startup

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: WMLS_STATUS 

 PARAMETERS:
        void

 DESCRIPTION:
        Initialize encoder static data. Called only only once per
        GateWay lifetime.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
WMLS_STATUS WMLS_Startup(void)
{ WMLS_STATUS retv;

  if( WMLS_runflag )     /* if running then exit with Ok */
    return WMLS_STATUS_OK;

  LexInit();

  retv = InitializeLibraryTables();
  if( retv == WMLS_STATUS_OK )
    { WSL_OS_MutexInit( &WMLS_countlock );
      WMLS_runflag = 1;
    }
  
  return retv;
}




/*********************************************************************

 FUNCTION     : WMLS_Shutdown

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: WMLS_STATUS 

 PARAMETERS:
        void

 DESCRIPTION:
        Any clean up work need to free statically allocated memory...
    
 NOTE: 

 CHANGES: 

*********************************************************************/
WMLS_STATUS WMLS_Shutdown(void)
{ WSL_Semaphore waitsem;

  if( !WMLS_runflag )     /* if not running then exit with error */
    return WMLS_STATUS_ERROR_NOT_STARTED;

  WMLS_runflag = 0;

  while( WMLS_count )
    { /* Use a semaphore to wait for 1 second. 
         Cannot use sleep because of portablity problems */
       WSL_OS_SemInit( &waitsem,0 );
       if( waitsem )
         { WSL_OS_SemTimedWait( &waitsem,1000 );
           WSL_OS_SemDestroy( &waitsem );
         }
    }

  /* Check that mutex is not locked */
  WSL_OS_MutexLock( &WMLS_countlock );
  WSL_OS_MutexUnlock( &WMLS_countlock );

  WSL_OS_MutexDestroy( &WMLS_countlock );

	FreeLibraryTables();
	
	return WMLS_STATUS_OK;
}






/*********************************************************************

 FUNCTION     : WMLS_FreeBuffers

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          17.5.1999.

 RETURN: void 

 PARAMETERS:
        BYTE**  outBuffer
        BYTE**  metaNameBuffer
        BYTE**  httpEquivBuffer

 DESCRIPTION:
        Frees the buffers only if they were allocated during compile
    
 NOTE: 

 CHANGES: 

*********************************************************************/
void WMLS_FreeBuffers( BYTE**  outBuffer, BYTE**  metaNameBuffer, BYTE**  httpEquivBuffer)
{
    if(*outBuffer)
        WMLS_free(*outBuffer);

    if(*metaNameBuffer)
        WMLS_free(*metaNameBuffer);

    if(*httpEquivBuffer)
        WMLS_free(*httpEquivBuffer);

    *outBuffer = NULL;
    *metaNameBuffer = NULL;
    *httpEquivBuffer = NULL;

}



/*********************************************************************

 FUNCTION     : WMLS_Encode

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: WMLS_STATUS 

 PARAMETERS:
                - inBytes: number of source code bytes
                - inBuffer: source code
                - outBytes (in/out): 
                       out: size of compiled bytecode
                - outBuffer (in/out): pointer to buffer for compiled bytecode
                - metaNamePragmas (in/out): 
                       out: number of meta name pragmas
                - metaNameBuffer (in/out): pointer to buffer for meta name information:
                        property1\0value1\0scheme1\0property2\0value2\0scheme2\0...
                - httpEquivPragmas (in/out): 
                      out: number of meta http equiv pragmas
                - httpEquivBuffer (in/out): pointer to buffer for http equivalent
                        information: property1\0value1\0scheme1\0property2\0value2\0
                        scheme2\0...
                - errorLine (out): line of the compilation error (0 if not valid).
                - errorColumn (out): column of the compilation error (0 if not valid).
                - pcErrorInfo (out): erroneous part of code or \0

 DESCRIPTION:
   Encodes (compiles) the given WMLScript source buffer into bytecode. If
   compilation does not succeed, an error status is returned with the line
   number of the compilation error in the source code (if line number is
   not applicable for the error type, then line number is zero).

   The possible meta information contained in the "http equiv" and "meta
   name" pragmas are returned in the given buffers. Each meta name pragma
   in source code results into three successive null terminated UTF-8
   strings in the out buffer as <name>\0<content>\0<scheme>\0.  If scheme
   is not defined, the string is empty (just the ending \0).
    
 NOTE: 

 CHANGES: 

*********************************************************************/
WMLS_STATUS WMLS_Encode(UINT32  inBytes,
                        BYTE*   inBuffer,
                        UINT32* outBytes,
                        BYTE**  outBuffer,
                        UINT32* metaNamePragmas,
                        BYTE**  metaNameBuffer,
                        UINT32* httpEquivPragmas,
                        BYTE**  httpEquivBuffer,
                        UINT32* errorLine,
                        UINT32* errorColumn,
                        char*   pcErrorInfo)
{
    struct Environment* pEnv= WMLS_malloc(sizeof(struct Environment));
    int iCodeSize;
    UINT32 iTotalSize;

    struct StorageStruct* pMain;
    BYTE bByte;
    INT16   iReturn = WMLS_STATUS_ERROR_OUT_OF_MEMORY;

    if( !WMLS_runflag )     /* if not running then exit with error */
       return WMLS_STATUS_ERROR_NOT_STARTED;

#ifdef MEMORY_OUT_TEST
    if(rand()%MEMORY_MAX < MEMORY_LIMIT)
    {
        if(pEnv)
        {
            WMLS_free(pEnv);
            pEnv = NULL;
        }
    }
#endif

    /* check that all basic things are OK */
    if(inBuffer == NULL) 
        return WMLS_STATUS_ERROR_INVALID_BUFFER;
    
    /* increase run counter */
    WSL_OS_MutexLock( &WMLS_countlock );
    WMLS_count++;
    WSL_OS_MutexUnlock( &WMLS_countlock );

    /* initiliaze buffer pointers */
    *outBuffer = NULL;
    *metaNameBuffer = NULL;
    *httpEquivBuffer = NULL;

    *metaNamePragmas = 0;
    *httpEquivPragmas = 0;

    /* initialize environment struct */
    if(pEnv)
    {
        
        pEnv->iErrorLineNumber = 0;
        pEnv->iErrorColumnNumber = 0;
        pEnv->status = WMLS_STATUS_OK;
        pEnv->warning = WMLS_STATUS_OK;

        pEnv->pcErrorInfo = pcErrorInfo;
        if(pEnv->pcErrorInfo)
		{
            pcErrorInfo[0] = 0;
		}

        
        pEnv->pMemPermanent = MemoryAllocNewNode(SIZE_OF_PERMANENT_MEMORY_NODE);
        pEnv->pMemTemporary = MemoryAllocNewNode(SIZE_OF_TEMPORARY_MEMORY_NODE);
    
        if(pEnv->pMemPermanent==NULL || pEnv->pMemTemporary==NULL)
            pEnv->status = WMLS_STATUS_ERROR_OUT_OF_MEMORY;

        if(pEnv->status == WMLS_STATUS_OK)
        {
            if(HashTableInitialize(&(pEnv->Constants), HASH_TABLE_SIZE, pEnv->pMemPermanent) != WPSS_NO_ERROR)
                pEnv->status = WMLS_STATUS_ERROR_OUT_OF_MEMORY;

            if(HashTableInitialize(&(pEnv->Variables), HASH_TABLE_SIZE, pEnv->pMemTemporary) != WPSS_NO_ERROR)
                pEnv->status = WMLS_STATUS_ERROR_OUT_OF_MEMORY;
            
            if(HashTableInitialize(&(pEnv->Functions), HASH_TABLE_SIZE, pEnv->pMemPermanent) != WPSS_NO_ERROR)
                pEnv->status = WMLS_STATUS_ERROR_OUT_OF_MEMORY;
                        
		}
        else
        {
            pEnv->Constants.ppNodes = NULL;
            pEnv->Variables.ppNodes = NULL;
            pEnv->Functions.ppNodes = NULL;
        }
    
                     
        pEnv->pPragmas = NULL;
        pEnv->pCurrentLoop = NULL;
        pEnv->pCurrentFunction = NULL;
        pEnv->pTemp = NULL;

        pEnv->lex.pbData = inBuffer;
        pEnv->lex.iDataLen = inBytes;
        pEnv->lex.iDataIndex = 0;
        pEnv->lex.iRow = 1;
        pEnv->lex.iColumn = 0;
        pEnv->lex.iTokenColumn = 0;
        pEnv->lex.iStatus = WMLS_STATUS_OK;

        pEnv->lex.pcTokenText = NULL;
        pEnv->lex.pcTokenTextPrev = NULL;
        pEnv->lex.iToken = 0;
        pEnv->lex.iPrevToken = 0;
        pEnv->lex.iReserved = 0;
        pEnv->lex.iTokenTextLen = 0;
        pEnv->lex.iTokenTextLenPrev = 0;
        pEnv->lex.iReservedPrev = 0;
		

        pEnv->metaNameStrings = metaNamePragmas;
        pEnv->httpEquivStrings = httpEquivPragmas;
        pEnv->httpEquivBuffer = NULL;
        pEnv->metaNameBuffer = NULL;
        pEnv->metaNameBufferSize = 0;
        pEnv->httpEquivBufferSize = 0;


        pEnv->StorageConstants.pbData = NULL;
		pEnv->StorageConstants.iIndex = 0;
		pEnv->StorageConstants.iCurrentlyAllocated = 0;
		pEnv->StorageConstants.iInitialSize = 0;
		pEnv->StorageConstants.iIncreaseSize = 0;

        pEnv->StorageFunctions.pbData = NULL;
		pEnv->StorageFunctions.iIndex = 0;
		pEnv->StorageFunctions.iCurrentlyAllocated = 0;
		pEnv->StorageFunctions.iInitialSize = 0;
		pEnv->StorageFunctions.iIncreaseSize = 0;

        pEnv->StoragePragmas.pbData = NULL;
		pEnv->StoragePragmas.iIndex = 0;
		pEnv->StoragePragmas.iCurrentlyAllocated = 0;
		pEnv->StoragePragmas.iInitialSize = 0;
		pEnv->StoragePragmas.iIncreaseSize = 0;

        pEnv->pCalls = NULL;

        if(pEnv->status == WMLS_STATUS_OK)
        {
			if (pEnv->lex.iTokenTextLen!=0)
			{
				pEnv = pEnv;
			}
            iReturn = yyparse(pEnv);
            
            /* compile OK, check that there is no illegal function calls */
            if(iReturn==0)
            {
                ValidateFunctionTable(pEnv);
            }
            
            if(pEnv->status == WMLS_STATUS_OK)
            {
                
                /* initialize here to error */
                pEnv->status = WMLS_STATUS_ERROR_OUT_OF_MEMORY;


                iReturn = ConstTableGenerateBytecode(&(pEnv->Constants),&(pEnv->StorageConstants));

                if(iReturn)
                    iReturn = PragmaTableGenerateBytecode(pEnv->pPragmas,&(pEnv->StoragePragmas));

                if(iReturn)
                    iReturn = FunctionTableGenerateBytecode(&(pEnv->Functions),&(pEnv->StorageFunctions));


                if(iReturn)
                {
                    iCodeSize = pEnv->StorageConstants.iIndex + pEnv->StoragePragmas.iIndex +pEnv->StorageFunctions.iIndex;

                    iTotalSize = iCodeSize;

                    /* add place for bytecode len */
                    if(iTotalSize < 128)
                        iTotalSize += 1;
                    else if(iTotalSize < 16383)
                        iTotalSize += 2;
                    else if(iTotalSize < 2097151)
                        iTotalSize += 3;
                    else
                        iTotalSize += 4;

                    /* add place for version */
                    iTotalSize++;

                    pMain = (struct StorageStruct*)WMLS_malloc(sizeof(struct StorageStruct));

#ifdef MEMORY_OUT_TEST
                    if(rand()%MEMORY_MAX < MEMORY_LIMIT)
                    {
                        if(pMain)
                        {
                            WMLS_free(pMain);
                            pMain = NULL;
                        }
                    }
#endif

                    if(pMain)
                    {
                        /* link output to callers buffer */

                        pMain->pbData = (BYTE*) WMLS_malloc(iTotalSize+10);

#ifdef MEMORY_OUT_TEST
                        if(rand()%MEMORY_MAX < MEMORY_LIMIT)
                        {
                            if(pMain->pbData)
                            {
                                WMLS_free(pMain->pbData);
                                pMain->pbData = NULL;
                            }
                        }
#endif

                        if(pMain->pbData)
                        {
                            /* link buffer to callers buffer pointer -> do not free this buffer */
                            *outBuffer = pMain->pbData;

                            pMain->iInitialSize = iTotalSize + 10;
                            pMain->iCurrentlyAllocated = iTotalSize + 10;
                            pMain->iIndex = 0;
                            pMain->iIncreaseSize = 50;
                            
                            /* add version */
                            bByte = WMLSCRIPT_VERSION_MAJOR - 1;
                            bByte = bByte << 4;
                            bByte += WMLSCRIPT_VERSION_MINOR;

                            ByteCodeAppendByte(bByte,pMain);


                            /* add length */
                            ByteCodeAppendMBInt(iCodeSize,pMain);


                            /* add constpool */
                            ByteCodeAppendBytes(pEnv->StorageConstants.pbData, pEnv->StorageConstants.iIndex,pMain);


                            /* add pragma pool */
                            ByteCodeAppendBytes(pEnv->StoragePragmas.pbData, pEnv->StoragePragmas.iIndex,pMain);

                            
                            /* add function pool */
                            ByteCodeAppendBytes(pEnv->StorageFunctions.pbData, pEnv->StorageFunctions.iIndex,pMain);

                        
                            /* return bytecode size to caller */
                            *outBytes = iTotalSize;

                            /* compile went OK */
                            pEnv->status = WMLS_STATUS_OK;
                        }
                        else
                        {
                            pEnv->status = WMLS_STATUS_ERROR_OUT_OF_MEMORY;
                        }

                        WMLS_free(pMain);
                    }
                    else
                    { pEnv-> status = WMLS_STATUS_ERROR_OUT_OF_MEMORY;
                    }
                }
                else
                {
                    pEnv->status = WMLS_STATUS_ERROR_OUT_OF_MEMORY;
                }

                
                /* clean up  */
                ByteCodeCleanUp(&(pEnv->StorageConstants));
                ByteCodeCleanUp(&(pEnv->StoragePragmas));
                ByteCodeCleanUp(&(pEnv->StorageFunctions));
            }
        } 
    }



    /* clean up environment struct */
    if(pEnv)
    {
        /* return possible error line */
        if(errorLine)
            *errorLine = pEnv->iErrorLineNumber;

        if(errorColumn)
            *errorColumn = pEnv->iErrorColumnNumber;

        if (pEnv->status != WMLS_STATUS_OK)
		{
			iReturn = pEnv->status;
		}
		else
		{
			iReturn = pEnv->warning;
		}

        /* some of functions may have been generated -> free bytecode memory */
        if(pEnv->status != WMLS_STATUS_OK)
            FunctionTableCleanUp(&(pEnv->Functions));

        /* return pragmas */
        *httpEquivBuffer = pEnv->httpEquivBuffer;
        *metaNameBuffer = pEnv->metaNameBuffer;

    
        MemoryDelete(pEnv->pMemPermanent);
        MemoryDelete(pEnv->pMemTemporary);

        HashTableCleanUp(&(pEnv->Constants));
        HashTableCleanUp(&(pEnv->Variables));
        HashTableCleanUp(&(pEnv->Functions));
 
        PragmaTableCleanUp(pEnv->pPragmas);

        WMLS_free(pEnv);
        
    }

    /* decrease run counter */
    WSL_OS_MutexLock( &WMLS_countlock );
    WMLS_count--;
    WSL_OS_MutexUnlock( &WMLS_countlock );

    return iReturn;

}

/*********************************************************************

 FUNCTION     : WMLS_AddLibrary

**********************************************************************

 AUTHOR:        Jouni Sipola / CCC Companies
 DATE:          05.08.1999.

 RETURN: WMLS_STATUS				- WMLS_STATUS_OK
									- WMLS_STATUS_ERROR_OUT_OF_MEMORY
									- WMLS_STATUS_LIBRARY_EXISTS
									- WMLS_STATUS_FUNCTION_NAME_CONFLICT

 PARAMETERS:
        const char* libraryName		- name of the library
        UINT16 libraryId			- numeric bytecode id of the library
        BYTE functionCount			- number of functions in functionArray
        FUNCTION_DEF* functionArray	- ptr to array of FUNCTION_DEF

 DESCRIPTION: 
		This function adds a system library to the WMLS Encoder.
		WMLS_Startup must have been called before. 
		At WMLS_Shutdown the added libraries are forgotten. 
		This function must not be called during any other call to the
		WMLS encoder.
	
 NOTE: 

 CHANGES: 

*********************************************************************/
WMLS_STATUS WMLS_AddLibrary(	const char* libraryName, 
								UINT16 libraryId, 
								BYTE functionCount, 
								FUNCTION_DEF* functionArray)
{
	WMLS_STATUS status;
	
	status = LibraryAddLibrary(libraryName,libraryId, functionCount, functionArray);
	
	return status;
}




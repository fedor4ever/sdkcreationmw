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
/* File    : U:\wpsvob\wmlscript\work\ported\library.c
   Version : \main\11
   Date    : 22-Nov-00.17:24:56
*/

/*
 * ========================================================================
 *  Name        : library.c
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

      Checks the correctiveness of the WMLScript library calls and returns
      the index of the called library.

-----------------------------------------------------------------------------
*/

#include "main.h"

#include "common.h"
#include "library.h"
#include "wmlsapi.h"



/* defines */
#define LIBRARY_HASH_TABLE_SIZE				50
#define	LIBRARY_FUNCTION_HASH_TABLE_SIZE	500

#define SIZE_OF_LIBRARY_MEMORY_NODE			2000


#define LIBRARY_INDEX_LANG			0
#define LIBRARY_INDEX_FLOAT			1
#define LIBRARY_INDEX_STRING		2
#define LIBRARY_INDEX_URL			3
#define LIBRARY_INDEX_WMLBROWSER	4
#define LIBRARY_INDEX_DIALOGS		5
#define LIBRARY_INDEX_CRYPTO		6
#define LIBRARY_INDEX_DEBUG			0xffff


/* static local variables */
struct HashTable	Libraries;
struct HashTable	LibFuncs;
struct MemoryNode*	pLibMem = NULL;



/* local protos */
#ifdef STANDARD_LIBRARIES
WMLS_STATUS AddStandardLibraries();
#endif

/* local protos */
#ifdef CRYPTO_LIBRARIES
WMLS_STATUS AddCryptoLibraries();
#endif






/***************************************************************************
                            PUBLIC FUNCTIONS
***************************************************************************/



/*********************************************************************

 FUNCTION     : InitializeLibraryTables

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          4.8.1999.

 RETURN: WMLS_STATUS 

 PARAMETERS:

 DESCRIPTION:
	Allocs memory for two hashtables and memorynode needed by them.
	
 NOTE: 
	MUST be called before libraries are added in StartUp

 CHANGES: 

*********************************************************************/
WMLS_STATUS InitializeLibraryTables()
{
	WMLS_STATUS status = WMLS_STATUS_OK;

	/* initialize to empty tables */
	Libraries.ppNodes = NULL;
	LibFuncs.ppNodes = NULL;
	
	
	/* create memory node for hashtables */
	pLibMem = MemoryAllocNewNode(SIZE_OF_LIBRARY_MEMORY_NODE);

	if(pLibMem != NULL)
	{
		if(HashTableInitialize(&(Libraries), LIBRARY_HASH_TABLE_SIZE, pLibMem) != WPSS_NO_ERROR)
			status = WMLS_STATUS_ERROR_OUT_OF_MEMORY;

		if(HashTableInitialize(&(LibFuncs), LIBRARY_FUNCTION_HASH_TABLE_SIZE, pLibMem) != WPSS_NO_ERROR)
			status = WMLS_STATUS_ERROR_OUT_OF_MEMORY;

	}
	else
	{
		status = WMLS_STATUS_ERROR_OUT_OF_MEMORY;
	}


#ifdef STANDARD_LIBRARIES
	status = AddStandardLibraries();
#endif

#ifdef CRYPTO_LIBRARIES
	status = AddCryptoLibraries();
#endif
	
	if(status != WMLS_STATUS_OK)
	{
		if(pLibMem != NULL)
		{
			MemoryDelete(pLibMem);
			pLibMem = NULL;
		}

		HashTableCleanUp(&Libraries);
		HashTableCleanUp(&LibFuncs);
	}

	return status;

}





/*********************************************************************

 FUNCTION     : FreeLibraryTables

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          4.8.1999.

 RETURN: void 

 PARAMETERS:

 DESCRIPTION:
	Frees any resources reserved by Library tables
	
 NOTE: 
	MUST be called during ShutDown.

 CHANGES: 

*********************************************************************/
void FreeLibraryTables()
{
	
	HashTableCleanUp(&Libraries);
	HashTableCleanUp(&LibFuncs);
	
	if(pLibMem != NULL)
		MemoryDelete(pLibMem);
}




/*********************************************************************

 FUNCTION     : LibraryAddLibrary

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          5.8.1999.

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
	Adds one library with its functions to library hash table. Error is
	returned if library or function name conflict is occured.
	
 NOTE: 

 CHANGES: 

*********************************************************************/
WMLS_STATUS LibraryAddLibrary(const char* libraryName, UINT16 libraryId, 
							  BYTE functionCount, FUNCTION_DEF* functionArray)
{
	WMLS_STATUS		status = WMLS_STATUS_OK;
	struct			HashNode* p = NULL;
	int				i;

	/* Sanity checks */
    if( libraryName == NULL )
        return WMLS_STATUS_ERROR_INVALID_BUFFER;

    if( functionArray == NULL )
        return WMLS_STATUS_ERROR_INVALID_BUFFER;

	/* add library */
	if(Libraries.ppNodes)
	{
		/* library must not be already declared */
		p = HashTableStore(&(Libraries),(char*)libraryName,0,STORE_MODE_MAY_NOT_EXIST);

		if(p)
		{
			p->iIndex = libraryId;
		}
		else
		{
			if(Libraries.iLastError == HASH_TABLE_STORE_MODE_ERROR)
				status = WMLS_STATUS_ERROR_LIBRARY_EXISTS;
			else
				status = WMLS_STATUS_ERROR_OUT_OF_MEMORY;
		}
	}
	else
	{
		status = WMLS_STATUS_ERROR_OUT_OF_MEMORY;
	}


	/* add functions */
	
	if(status == WMLS_STATUS_OK)
	{
		if(LibFuncs.ppNodes)
		{
			for(i=0;i<functionCount;i++)
			{
				
				if(functionArray[i].functionName != NULL)
				{
					p = HashTableStore(&(LibFuncs),(char*)functionArray[i].functionName,
								libraryId,STORE_MODE_MAY_NOT_EXIST);

					if(p)
					{
						p->iIndex = functionArray[i].functionIndex;
						p->bNumArguments = functionArray[i].argumentCount;
					}
					else
					{
						if(LibFuncs.iLastError == HASH_TABLE_STORE_MODE_ERROR)
							status = WMLS_STATUS_ERROR_FUNCTION_NAME_CONFLICT;
						else
							status = WMLS_STATUS_ERROR_OUT_OF_MEMORY;

						break;
					}
				}
				else
				{
					status = WMLS_STATUS_ERROR_FUNCTION_NAME_CONFLICT;
				}
			}
		}
		else
		{
			status = WMLS_STATUS_ERROR_OUT_OF_MEMORY;
		}
	}


    return status;		
}



/*********************************************************************

 FUNCTION     : LibraryGetIDLibID

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: INT32 

 PARAMETERS:
        char* pcName        -in: name of the library

 DESCRIPTION:
        Return the index of given library.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
INT32 LibraryGetIDLibID(char* pcName)
{
    struct HashNode	*p = NULL;
	INT32		iIndex = UNIDENTIFIED;

    p = HashTableFind(&Libraries,pcName,0);

	if(p)
		iIndex = (INT32)p->iIndex;

	return iIndex;
}





/*********************************************************************

 FUNCTION     : LibraryGetFuncID

**********************************************************************

 AUTHOR:        Timo Rautiala / CCC Companies
 DATE:          10.5.1999.

 RETURN: BYTE 

 PARAMETERS:
        UINT16 uiLibID      -in: library id
        char* pcName        -in: name of the function
        BYTE* pbNumArgs     -out: number of arguments this function takes

 DESCRIPTION:
        Return the function id and num of arguments of given function.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
BYTE LibraryGetFuncID(UINT16 uiLibID, char* pcName, BYTE* pbNumArgs)
{
    struct HashNode	*p = NULL;
	INT32		iIndex = UNIDENTIFIED;

    p = HashTableFind(&LibFuncs,pcName,uiLibID);

	if(p)
	{
		iIndex = (INT32)p->iIndex;
		*pbNumArgs = p->bNumArguments;
	}

	return (BYTE)iIndex;
}



/*********************************************************************

 FUNCTION     : LibrarySetFuncID

**********************************************************************

 AUTHOR:        Gabor Valyi Nagy / Nokia
 DATE:          09-Mar-2000

 RETURN: BOOL	- if the function id is changed succesfully

 PARAMETERS:
        UINT16 uiLibID      -in: library id
        char* pcName        -in: name of the function
        BYTE  nNewId		-in: new id of the function

 DESCRIPTION:
        Sets the given function's id to th new.
    
 NOTE: 

 CHANGES: 

*********************************************************************/
BOOL LibrarySetFuncID(UINT16 uiLibID, char* pcName, BYTE nNewId)
{
    struct HashNode	*p = NULL;
	BOOL bRes = FALSE;

    p = HashTableFind(&LibFuncs,pcName,uiLibID);

	if(p)
	{
		p->iIndex = nNewId;
		bRes = TRUE;
	}

	return bRes;
}



/***************************************************************************
                            LOCAL FUNCTIONS
***************************************************************************/


#if defined STANDARD_LIBRARIES || defined CRYPTO_LIBRARIES


/*********************************************************************

 FUNCTION     : AddSpecifiedLibrary

**********************************************************************

 AUTHOR:        Jouni Sipola / CCC Companies
 DATE:          5.8.1999.

 RETURN: WMLS_STATUS 

 PARAMETERS:
        UINT16 uiLibIndex	
        char* pcLibName
        char* *pcFuncs
        BYTE *pbArgs
        INT16 iNumFuncs

 DESCRIPTION:
	Adds one Standard library with its function to library hash
	tables. Builds structures required by WMLS_AddLibrary.
	
 NOTE: 

 CHANGES: 

*********************************************************************/
WMLS_STATUS AddSpecifiedLibrary(UINT16 uiLibIndex, const char* pcLibName, const char* *pcFuncs, BYTE *pbArgs, INT16 iNumFuncs)
{
	INT16 i;
	WMLS_STATUS ws_Return = WMLS_STATUS_ERROR_OUT_OF_MEMORY;

	struct FUNCTION_DEF *pFuncDef = WMLS_malloc(sizeof(struct FUNCTION_DEF) * iNumFuncs);

	if(pFuncDef != NULL)
	{
		for(i=0;i<iNumFuncs;i++)
		{
			pFuncDef[i].functionName = pcFuncs[i];
			pFuncDef[i].functionIndex = (BYTE)i;
			pFuncDef[i].argumentCount = pbArgs[i];
		}

		ws_Return = WMLS_AddLibrary(pcLibName, uiLibIndex, (BYTE)iNumFuncs, pFuncDef);

		WMLS_free(pFuncDef);
	}

	return ws_Return;
}

#endif

#ifdef STANDARD_LIBRARIES

/*********************************************************************

 FUNCTION     : AddStandardLibraries

**********************************************************************

 AUTHOR:        Jouni Sipola / CCC Companies
 DATE:          5.8.1999.

 RETURN: WMLS_STATUS 

 PARAMETERS:

 DESCRIPTION:
	Adds all standard libraries to library hashtables.
	
 NOTE: 

 CHANGES: 

*********************************************************************/
WMLS_STATUS AddStandardLibraries()
{
	WMLS_STATUS ws_Return = WMLS_STATUS_OK;

	const char *pcLang[]={"abs","min","max","parseInt","parseFloat","isInt","isFloat","maxInt",
			"minInt","float","exit","abort","random","seed","characterSet"};
	const char *pcFloat[]={"int","floor","ceil","pow","round","sqrt","maxFloat","minFloat"};
	const char *pcString[]={"length","isEmpty","charAt","subString","find","replace","elements",
			"elementAt","removeAt","replaceAt","insertAt","squeeze","trim","compare","toString","format"};
	const char *pcURL[]={"isValid","getScheme","getHost","getPort","getPath","getParameters","getQuery",
			"getFragment","getBase","getReferer","resolve","escapeString","unescapeString","loadString"};
	const char *pcWMLBrowser[]={"getVar","setVar","go","prev","newContext","getCurrentCard","refresh"};
	const char *pcDialogs[]={"prompt","confirm","alert"};


	BYTE	pbArgLang[]={
			1, /* abs		0 */
			2, /* min		1 */
			2, /* max		2 */
			1, /* parseInt		3 */
			1, /* parseFloat	4 */
			1, /* isInt		5 */
			1, /* isFloat		6 */
			0, /* maxInt		7 */
			0, /* minInt		8 */
			0, /* float		9 */
			1, /* exit		10*/
			1, /* abort		11*/
			1, /* random		12*/
			1, /* seed		13*/
			0  /* characterSet	14*/
		};

	BYTE	pbArgFloat[]={
			1, /* int		0 */
			1, /* floor		1 */
			1, /* ceil		2 */
			2, /* pow		3 */
			1, /* round		4 */
			1, /* sqrt		5 */
			0, /* maxFloat		6 */
			0  /* minFloat		7 */
		};

	BYTE	pbArgString[]={
			1, /* length		0 */
			1, /* isEmpty		1 */
			2, /* charAt		2 */
			3, /* subString		3 */
			2, /* find		4 */
			3, /* replace		5 */
			2, /* elements		6 */
			3, /* elementAt		7 */
			3, /* removeAt		8 */
			4, /* replaceAt		9 */
			4, /* insertAt		10*/
			1, /* squeeze		11*/
			1, /* trim		12*/
			2, /* compare		13*/
			1, /* toString		14*/
			2  /* format		15*/
		};
	BYTE	pbArgURL[]={
			1, /* isValid		0  */
			1, /* getScheme		1  */
			1, /* getHost		2  */
			1, /* getPort		3  */
			1, /* getPath		4  */
			1, /* getParameters	5  */
			1, /* getQuery		6  */
			1, /* getFragment	7  */
			0, /* getBase		8  */
			0, /* getReferer	9  */
			2, /* resolve		10 */
			1, /* escapeString	11 */
			1, /* unescapeString	12 */
			2  /* loadString	13 */
		};


	BYTE	pbArgWMLBrowser[]={
			1, /* getVar		0 */
			2, /* setVar		1 */
			1, /* go		2 */
			0, /* prev		3 */
			0, /* newContext	4 */
			0, /* getCurrentCard 	5 */
			0  /* refresh		6 */
		};

	BYTE	pbArgDialogs[]={
			2, /* prompt		0 */
			3, /* confirm		1 */
			1  /* alert		2 */
		};	


#ifdef DEBUG_TEST

	char *pcDebug[]={"openFile","println","closeFile"};
	
	BYTE	pbArgDebug[]={
			2, /* openFile		0 */
			1, /* closeFile		1 */
			0  /* println		2 */
		};	
#endif

	
	ws_Return = AddSpecifiedLibrary(LIBRARY_INDEX_LANG, "Lang", pcLang, pbArgLang, 15);

	if(ws_Return == WMLS_STATUS_OK)
		ws_Return = AddSpecifiedLibrary(LIBRARY_INDEX_FLOAT, "Float", pcFloat, pbArgFloat, 8);

	if(ws_Return == WMLS_STATUS_OK)
		ws_Return = AddSpecifiedLibrary(LIBRARY_INDEX_STRING, "String", pcString, pbArgString, 16);

	if(ws_Return == WMLS_STATUS_OK)
		ws_Return = AddSpecifiedLibrary(LIBRARY_INDEX_URL, "URL", pcURL, pbArgURL, 14);

	if(ws_Return == WMLS_STATUS_OK)
		ws_Return = AddSpecifiedLibrary(LIBRARY_INDEX_WMLBROWSER, "WMLBrowser", pcWMLBrowser, pbArgWMLBrowser, 7);

	if(ws_Return == WMLS_STATUS_OK)
		ws_Return = AddSpecifiedLibrary(LIBRARY_INDEX_DIALOGS, "Dialogs", pcDialogs, pbArgDialogs, 3);


#ifdef DEBUG_TEST
	if(ws_Return == WMLS_STATUS_OK)
		ws_Return = AddSpecifiedLibrary(LIBRARY_INDEX_DEBUG, "Debug", pcDebug, pbArgDebug, 3);
#endif
	

	return ws_Return;
}

#endif  /* STANDARD_LIBRARIES */






#ifdef CRYPTO_LIBRARIES

/*********************************************************************

 FUNCTION     : AddCryptoLibraries

**********************************************************************

 AUTHOR:        Gabor Valyi Nagy
 DATE:          17-Mar-2000.

 RETURN: WMLS_STATUS 

 PARAMETERS:

 DESCRIPTION:
	Adds all crypto libraries to library hashtables.
	
 NOTE: 

 CHANGES: 

*********************************************************************/
WMLS_STATUS AddCryptoLibraries()
{
	WMLS_STATUS ws_Return = WMLS_STATUS_OK;

	const char *pcCrypto[]= {"signtext"};

	BYTE	pbArgCrypto[]={
			4	/* signtext */
		};	

	if(ws_Return == WMLS_STATUS_OK)
		ws_Return = AddSpecifiedLibrary(LIBRARY_INDEX_CRYPTO, "Crypto", pcCrypto, pbArgCrypto, 1);

	if (LibrarySetFuncID(LIBRARY_INDEX_CRYPTO, (char*)"signtext", 16) == FALSE)
	{
		ws_Return = WMLS_STATUS_ERROR_FUNCTION_NAME_CONFLICT;
	}

	return ws_Return;
}

#endif  /* CRYPTO_LIBRARIES */



/* End of file */













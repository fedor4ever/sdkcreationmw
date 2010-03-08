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
/* File    : U:\wpsvob\wmlscript\work\ported\library.h
   Version : \main\5
   Date    : 22-Nov-00.17:24:57
*/

/*
 * ========================================================================
 *  Name        : library.h
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

      WMLScript standard library call definitions and interface function 
      declarations.

-----------------------------------------------------------------------------
*/

#if !defined(LIBRARY_H)
#define LIBRARY_H

#include "common.h"
#include "main.h"

#define UNIDENTIFIED    0xff



WMLS_STATUS InitializeLibraryTables();
void		FreeLibraryTables();
WMLS_STATUS LibraryAddLibrary(const char* libraryName, UINT16 libraryId, 
							  BYTE functionCount, FUNCTION_DEF* functionArray);

INT32		LibraryGetIDLibID(char* pcName);
BYTE		LibraryGetFuncID(UINT16 uiLibIDD, char* pcName, BYTE* pbNumArgs);

#endif

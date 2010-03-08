/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _C_SYS_INI_H_
#define _C_SYS_INI_H_

// Includes
#include <e32std.h>


// Class definition 
class CSysIni  
	{
protected:
	CSysIni();
	void CostructL(const TDesC& filename);
public:	
	IMPORT_C virtual ~CSysIni();

	IMPORT_C static CSysIni* NewL(const TDesC& filename);

	// Search for a boolean (0/1)
	IMPORT_C bool					// Returns true if line was found
		SeekBool(
		const char* string);		// Search for this string

	// Search for a string 
	IMPORT_C bool					// Returns true if line was found
		SeekStr(				
		const char* string			// Search for this string
		, TDes8& output);			// OUT: string

	// Fetch a whole line eg. DESCRIPTION This is a description
	IMPORT_C bool					// Returns true if line was found 
		SeekLine(
		  const char* string		// Search for this string
		, TDes8& output);			// OUT: Rest of the line

	// Seach for single integer eg. UDP_PORT 2948
	IMPORT_C bool					// Returns true if line was found 
		SeekInt(
		const char* string			// Search for this string
		, int& i);					// OUT: found int

	// Serach for integer lists. eg. UDP_PORTS 2948, 2949 
	IMPORT_C bool					// Returns true if line was found 
		SeekIntList(
		  const char* string		// Search for this string
		, int* results				// OUT: results table
		, int  max_results			// IN:  max results
		, int& actual_results);		// OUT: Number of actual results

	IMPORT_C int GetDataLength();	// Return length of actual data in bytes

protected:
	inline bool SeekField(const char* string, TDes8& p, bool allow_spaces = false);

	void RemoveComments(TPtr8& src);

	HBufC8*	iData;
	HBufC8*	iFiltered;
	TPtr8	iFil;	
	};

#endif // _C_SYS_INI_H_

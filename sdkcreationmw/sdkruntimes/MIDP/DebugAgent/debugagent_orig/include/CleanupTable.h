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


#ifndef HEADER_CLEANUP_TABLE
#define HEADER_CLEANUP_TABLE

#include <e32base.h>
#include "Driver.h"

_LIT(KDebugTempFolder,"c:\\midp2\\jdebugagent\\");
_LIT(KCleanupIndexFile,"c:\\midp2\\jdebugagent\\index.bin");

/**
*	Maintains a table of MIDlet suites installed for debugging
*	and provides methods for removing them. Also, a call to Cleanup
*	method will remov all temporary files created during previous
*	debugging sessions.
*/
class CCleanupTable 
	{

public:
	CCleanupTable(CDriver* aDriver);
	~CCleanupTable(void);

public:

	// called when a suite is installed
	void SuiteInstalledL(TInt aId);
	// called when a suite is removed
	void SuiteRemovedL(TInt aId);

	// clean all temp installations and files
	void CleanupL(void);
	
	// close the table
	void CloseL();

private:
	void PruneL();
	void LoadL();
	void SaveL();

private:
	CDriver*		iDriver;
	RArray<TInt>	iCache;
	};

#endif
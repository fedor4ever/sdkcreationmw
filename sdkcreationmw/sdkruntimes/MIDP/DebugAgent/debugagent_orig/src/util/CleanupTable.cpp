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



#include "CleanupTable.h"
#include "AgentEngine.h"
#include "jdebug.h"

/**
*	Maintains a table of MIDlet suites installed for debugging
*	and provides methods for removing them. Also, a call to Cleanup
*	method will remov all temporary files created during previous
*	debugging sessions.
*/

// instance vars
//	CDriver*		iDriver;
//	RArray<TInt>	iCache;
//	RFile&			iFile;


CCleanupTable::CCleanupTable(CDriver* aDriver) :
	iDriver(aDriver)
	{
	TRAPD(error,LoadL()); // TODO this should be done using two phase construction
	}

CCleanupTable::~CCleanupTable(void)
	{
	iCache.Reset();
	}

// called when a suite is installed
void CCleanupTable::SuiteInstalledL(TInt aId)
	{
	TInt index = iCache.Find(aId);
	if ( index < 0 )
		{
		iCache.Append(aId);
		SaveL();
		}
	}

// called when a suite is removed
void CCleanupTable::SuiteRemovedL(TInt aId)
	{
	int index = iCache.Find(aId);
	if ( index >= 0 )
		{
		iCache.Remove(index);
		SaveL();
		}
	}

// clean all temp installations and files
void CCleanupTable::CleanupL(void)
	{
	DEBUG("+CCleanupTable::Cleanup");
	PruneL();
	CAgentEngine* engine = iDriver->Engine();
	int count = iCache.Count();
	for ( TInt i = count-1; i >= 0; i-- )
		{
		TRAPD(error, engine->RemoveL(iCache[i]));
		}
	SaveL();
	// delete temp files
	RFs fs = iDriver->Fs();
	CFileMan* fileman = CFileMan::NewL(fs);
	CleanupStack::PushL(fileman);
	fileman->RmDir(KDebugTempFolder);
	CleanupStack::PopAndDestroy(1);
	DEBUG("-CCleanupTable::Cleanup");
	}
	
void CCleanupTable::PruneL()
	{
	// verify that suites we think are installed are really there
	// (user may manually install stuff)
	RArray<TUint32> installed;
	TInt count = iCache.Count();
	iDriver->Engine()->ListSuiteIDsL(installed);
	for ( TInt i = 0 ; i < count ; i ++ )
		{
		TUint32 id = (TUint32)iCache[i];
		TInt index = installed.Find(id);
		if ( index < 0 )
			{
			iCache.Remove(i);
			// ensure loop is not broken by removing
			// an element from iCache
			i--;
			count = iCache.Count();
			}
		}
	installed.Reset();
	}


void CCleanupTable::LoadL()
	{
	RFile file;
	TInt err = file.Open(iDriver->Fs(),KCleanupIndexFile,EFileRead|EFileShareAny);
	if ( err != KErrNone )
		{
		// not there, probaly the first run
		return;
		}
	iCache.Reset();
	int count;
	file.Size(count);
	count = count / 4;
	// prepare buffer
	HBufC8* buf = HBufC8::NewLC(4);	
	TPtr8 ptr ((TUint8*)(buf->Ptr()),4,4);
	// read
	for ( TInt j = 0 ; j < count ; j++  )
		{
		file.Read(ptr,4);
		TInt val = *((TInt*)ptr.Ptr());
		iCache.Append(val);
		}
	file.Close();
	CleanupStack::PopAndDestroy(buf);
	}

void CCleanupTable::SaveL()
	{
	PruneL();
	RFile file;
	TInt err = file.Open(iDriver->Fs(),KCleanupIndexFile,EFileWrite|EFileShareAny);
	if ( err != KErrNone )
		{
		err = file.Create(iDriver->Fs(),KCleanupIndexFile,EFileWrite|EFileShareAny);
		if ( err != KErrNone )
			{
			// cant create, probaly the first run
			return;
			}
		}
	file.SetSize(0);
	// File is open
	int count = iCache.Count();
	for(TInt i = 0 ; i < count ; i++ )
		{
		TInt val = iCache[i];
		TPtr8 ptr((TUint8*)(&val),4,4);
		file.Write(ptr);
		}
	file.Close();
	}

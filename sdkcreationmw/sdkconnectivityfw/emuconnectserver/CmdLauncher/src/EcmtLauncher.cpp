/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Standalone .exe that launces Ecmt framework without emulator
*
*/



// INCLUDE FILES
#include <e32base.h>
#include <f32file.h>

// MODULE DATA STRUCTURES
typedef TInt (*ExeEntryPoint)();

// CONSTANTS
const TInt KDefaultMinHeapSize=0x10000; // !! for now
const TInt KDefaultMaxHeapSize=0x1000000; // !! for now
_LIT( KEcmtCoreFileName, "Z:\\Sys\\bin\\ECMTCORE.EXE" );

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ExeThreadStartFunction .
// Generic thread entry point that calls the first export of the library
// given as parameter.
// -----------------------------------------------------------------------------

TInt ExeThreadStartFunction(TAny* aParam)
	{
	const TDesC& libraryName=(*(const TDesC*)aParam);
	RLibrary lib;
	TInt err=lib.Load(libraryName);
	User::Free(aParam);
	if (!err)
		{
		ExeEntryPoint exeFunc=(ExeEntryPoint)lib.Lookup(1);
		if (!exeFunc)
			err=KErrBadLibraryEntryPoint;
		else
			err=(*exeFunc)();
		}
	return(err);
	}

// -----------------------------------------------------------------------------
// LaunchEcmtCoreL
// 
// -----------------------------------------------------------------------------

void LaunchEcmtCoreL()
	{
	RFs fs;
	fs.Connect();
	TFindFile findExe( fs );

	TInt err = findExe.FindByPath( KEcmtCoreFileName, NULL );
	User::LeaveIfError( err );

	TParse fileName;
	TThreadId threadId;
	User::LeaveIfError(fileName.Set(KEcmtCoreFileName, NULL, NULL));
	
/* RThread process;

	TBuf<KMaxFileName> threadName;
	TPtrC baseName=fileName.Name();

	TInt num=0;
	_LIT(KThreadFormat,"%S%02d");
	do
		{
		threadName.Format(KThreadFormat,&baseName,num++);
		err=process.Create(threadName,ExeThreadStartFunction,KDefaultStackSize,KDefaultMinHeapSize,KDefaultMaxHeapSize,NULL);
		} while(err==KErrAlreadyExists);

	User::LeaveIfError(err);
	RHeap* heap=process.Heap();
	RHeap* originalHeap=User::SwitchHeap(heap);
	HBufC* commandLine=fileName.FullName().Alloc();
	User::SwitchHeap(originalHeap);
	if (!commandLine)
		{
		process.Close();
		User::LeaveNoMemory();
		}
	process.SetInitialParameter(commandLine);
	threadId = process.Id();

	process.Resume();
	process.Close();
*/	

	RProcess process;

	TBuf<KMaxFileName> processName;
	TPtrC baseName=fileName.Name();

	HBufC* commandLine=fileName.FullName().Alloc();
	if (!commandLine)
		{
		User::LeaveNoMemory();
		}

	TInt num=0;
	_LIT(KProcessFormat,"%S%02d");
	do
		{
		processName.Format(KProcessFormat,&baseName,num++);
//		err=process.Create(processName,ExeThreadStartFunction,KDefaultStackSize,KDefaultMinHeapSize,KDefaultMaxHeapSize,NULL); 
		err = process.Create( processName, commandLine->Des() );
		} while ( err == KErrAlreadyExists );

	process.Resume();
	process.Close();
	}

// -----------------------------------------------------------------------------
// E32Main
// Entry point called by system
// -----------------------------------------------------------------------------

GLDEF_C TInt E32Main()
    {
	CTrapCleanup* cleanup=CTrapCleanup::New();
	TRAPD(error, LaunchEcmtCoreL() );
	delete cleanup;

	RThread self;
	self.Suspend();
	return 0;
    }

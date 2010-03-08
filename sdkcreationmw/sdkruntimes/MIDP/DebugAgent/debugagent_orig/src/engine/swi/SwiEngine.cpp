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


#include <e32base.h>
#include "PassiveLogger.h"
#include "SwiInstallTask.h"
#include "SwiRemoveTask.h"
#include "SwiEngine.h"
#include "jdebug.h"

_LIT(KLF,"\n");
_LIT16(KMIDletName, "MIDlet-Name:");
_LIT16(KMIDletVendor, "MIDlet-Vendor:");

CSwiEngine::CSwiEngine(CDriver* aDriver):
	CAgentEngine(aDriver)
	{ 
	DEBUG("*CSwiEngine::<C'tor>");
	}

CSwiEngine::~CSwiEngine()
	{
	}

TInt CSwiEngine::InstallL(
		HBufC* aJad, 
		HBufC* aJar, 
		TBool /*aTrusted*/)
	{
	DEBUG("+CSwiEngine::InstallL");
	// the main challenge here is how to find SystemAMS assigned midlet
	// suite ID. SystemAms returns this value to software install, but
	// we do not get it. The only viable and realiable option here is 
	// to parse jad for name and vendor, and then try to map it to suite ID.
	
	// At this point, just load jad
	HBufC* jad = LoadJadL(iDriver, *aJad);
	CleanupStack::PushL(jad);
        	
   	CPassiveLogger* logger = CPassiveLogger::NewL(iDriver);
	CSwiInstallTask* task = new (ELeave)CSwiInstallTask(iDriver, logger, aJad, aJar);
	User::LeaveIfError(RunInNewThreadL(task));
	
	// parse pre-loaded jad to find the ID of the newly installed suite
	HBufC* midletName = FindInJadLC(jad, KMIDletName);
	HBufC* midletVendor = FindInJadLC(jad, KMIDletVendor);
	
	TInt id = GetIdL(midletName, midletVendor);
		
	CleanupStack::PopAndDestroy(3);
	SuiteInstalled(id);
	DEBUG("-CSwiEngine::InstallL");
	return id;
	}

void CSwiEngine::RemoveL(
		HBufC* aName, 
		HBufC* aVendor)
	{
	DEBUG("+CSwiEngine::RemoveL");/*
	// find UID by name/vendor
	//iDriver->Log(_L("CSwiEngine::RemoveL - Getting ID"));
	TInt id = GetIdL(aName, aVendor);
	//iDriver->Log(_L("CSwiEngine::RemoveL - Calling FindMIDletUidL"));
	TUid uid = FindMIDletUidL(id);
	//iDriver->Log(_L("CSwiEngine::RemoveL - Got UID"));
	
   	CPassiveLogger* logger = CPassiveLogger::NewL(iDriver);
	CSwiRemoveTask* task = new (ELeave)CSwiRemoveTask(this, iDriver, logger, uid);
	TInt error = RunInNewThreadL(task);
   	//User::Leave(error);
	//not leaving to allow command response to be written back
	//to the SEI
	SuiteRemoved(id);*/
	DEBUG("-CSwiEngine::RemoveL");
	}

_LIT16(KThreadName,"SwiEngineThread");
TInt CSwiEngine::RunInNewThreadL(CSwiTask* aTask)
	{
	CPassiveLogger* logger = aTask->Logger();
	
	RThread installRunnerThread;       
	TInt status = installRunnerThread.Create(       
		KThreadName,       
		reinterpret_cast<TThreadFunction>(ThreadFunction),       
		KDefaultStackSize,       
		NULL,
		aTask     
		);       

	User::LeaveIfError(status);       

	TRequestStatus threadStat;
	threadStat=KRequestPending;       

	installRunnerThread.Resume();       
	installRunnerThread.Logon(threadStat);

	User::WaitForRequest(threadStat);
	installRunnerThread.Close();       

	TInt result = threadStat.Int();  
	//NOTE: if removal result==-14 (KErrInUse) it's because the VM didnt exit yet.
	// Security checks if the suite is in use in a very naive way, it simply
	// tries to open the Jar file..
	// However, in that case, it is undefined if the removal was successfull.
	if(result == KErrNone)
		{
		logger->AppendL(_L("Swi:Task complete"));
		}
	else
		{
	    _LIT(KErrorPattern, "Swi:Task failed: %d");
    	TBuf16<64> buf;
    	buf.AppendFormat(KErrorPattern, result);
    	logger->AppendL(buf);
    	//User::Leave(error);
		//not leaving to allow command response to be written back
		//to the SEI
		}
	logger->Flush();     
	delete logger;
	return result;
	}
	
TInt CSwiEngine::ThreadFunction(CSwiTask* aTask)       
	{       
	// set up essentials for this thread
	CTrapCleanup* cleanupStack = CTrapCleanup::New();       
	if (cleanupStack == NULL)       
		{       
		return KErrNoMemory;       
		}
		
	// need to use code that leaves, wrap
	TRAPD(result, ThreadFunctionL(aTask));
	
	delete cleanupStack;       
	if ( result < KErrNone )
		{
		delete aTask;
		return result;
		}
	TInt retval = aTask->TaskStatus().Int();       
	delete aTask;
	return retval;
	}       
	
void CSwiEngine::ThreadFunctionL(CSwiTask* aTask)
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);


	// this will add some active objects to AS...
	aTask->WrapTaskL();


	// make it do the work
	scheduler->Start();
	
	// cleanup the worker thread
	CleanupStack::PopAndDestroy(scheduler);
	}


HBufC* CSwiEngine::FindInJadLC(HBufC* aJad, const TDesC& aProp)
	{
	TInt lineIndex = aJad->Find(aProp);
	TPtrC tmpPtr = aJad->Right(aJad->Length()-lineIndex-aProp.Length());
	TInt crlfIndex = tmpPtr.Find(KCRLF);
	if(crlfIndex == -1)
		{
		// maybe bad jad
		crlfIndex = tmpPtr.Find(KLF);
		}
		
	TPtrC tmpPtr2 = tmpPtr;
	
	if(crlfIndex != -1)
		{
		// maybe this is the last line and there is no newline at the eol
		tmpPtr2.Set(tmpPtr.Left(crlfIndex));
		}
		
	HBufC* ret = tmpPtr2.AllocLC();
	TPtr ptr= ret->Des();
	ptr.Trim();
	return ret;
	}

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



#include "SwiRemoveTask.h"

#include "SwiUi.h"
#include "SwiRemoverUi.h"
#include "jdebug.h"

#include "SWInstDefs.h"

#include "javaregistry.h"
#include "javaregistryentry.h"

class CJavaRegistry;
class CJavaRegistryPackageEntry;
class CJavaRegistryEntry;

CSwiRemoveTask::CSwiRemoveTask(
	CDriver* aDriver, CPassiveLogger* aLogger, TUid aUid) :
	CSwiTask(aDriver, aLogger), iUid(aUid)
	, iRemoverUi(NULL),
	iRemover(NULL)
	{
	}
	
CSwiRemoveTask::~CSwiRemoveTask()
	{	
		delete iRemoverUi;
		delete iRemover; 
	}


void CSwiRemoveTask::DoTaskL(void)
	{
	
	/*
	DEBUG("+CSwiRemoveTask::DoTaskL");
	iRemoverUi = new (ELeave) CSwiRemoverUi(this);
	TRAPD(err, iRemover = CJavaRemover::NewL(iUid, *iRemoverUi));
	if (err != KErrNone )
		{
		DEBUG_INT("error in CSwiRemoveTask::DoTaskL %d", err);

		Log(_L("Unistallation failed"));
		User::Leave(err);
		}
	Log(_L("Starting uninstall..."));
	iRemover->Remove(iStatus);
	*/
	
	iInstaller.Connect();
	iUnInstallOptions.iKillApp = EPolicyAllowed;
	iUnInstallPckg = iUnInstallOptions;
    iInstaller.SilentUninstall( iStatus, 
                       iUid ,//const TUid& aUid,
                       iUnInstallPckg,//const TDesC8& aOptions,
                       KJarMIMEType);//const TDesC8& aMIME );

	//We can get UID differently also:
	/*
	iRegistry = Java::CJavaRegistry::NewL();
	RArray<TUid> uids;
	iRegistry ->GetRegistryEntryUidsL( uids );
    for(TInt i=0; i<uids.Count(); i++)
    {
		Java::CJavaRegistryEntry* entry = 
			iRegistry->RegistryEntryL( uids[i] );
		//CleanupStack::PushL( entry );
		TBufC<200> buf = entry->Name();
		if((entry->Name()).Compare(_L("SysOutMIDlet")//nameOfOurJad)==0)
		{
            iInstaller.SilentUninstall( iStatus, 
                                   uids[i],//const TUid& aUid,
                                   optionsPckg,//const TDesC8& aOptions,
                                   KJarMIMEType);//const TDesC8& aMIME );
			break;	                                   
		}

    }*/
	//DEBUG("-CSwiRemoveTask::DoTaskL");
	}

void CSwiRemoveTask::RunL(void)
	{
	CSwiTask::RunL();
	/*
	iAttempt++;
	if ((iStatus == KErrInUse ) && iAttempt < 5)
		{
		// Try again
		delete iRemover;
		iRemover = NULL;
		iRemover = CJavaRemover::NewL(iUid, *iRemoverUi);
		iStatus = KRequestPending;
		SetActive();
		Log(_L("Uninstalling..."));
        User::After(1000000);		
		iRemover->Remove(iStatus);
		
		iStatus = KRequestPending;
		SetActive();
		User::After(1000000);
		
		//iUnInstallOptions.iKillApp = EPolicyAllowed;
		//iUnInstallPckg = iUnInstallOptions;
	    //iInstaller.SilentUninstall( iStatus, 
	                       iUid ,//const TUid& aUid,
	                       iUnInstallPckg,//const TDesC8& aOptions,
	                       KJarMIMEType);//const TDesC8& aMIME );
		}
	else
		{
		CSwiTask::RunL();
		}*/
	}

void CSwiRemoveTask::DoTaskCleanupL(void)
	{
		iInstaller.Close();
	}

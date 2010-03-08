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


#include "SwiInstallTask.h"
#include "SwiUi.h"
#include "SwiDownloaderUi.h"
#include <MIDPInstallationLaunchInfo.h>
#include "jdebug.h"


CSwiInstallTask::CSwiInstallTask(
	CDriver* aDriver, CPassiveLogger* aLogger,
	HBufC* aJad, HBufC* aJar) :
	CSwiTask(aDriver, aLogger), iJad(aJad), iJar(aJar)
	{
	}
	
CSwiInstallTask::~CSwiInstallTask()
	{
	delete iInstaller;	
	delete iLaunchInfo;	
	delete iUi;	
	delete iDlUi;	
	}

void CSwiInstallTask::DoTaskL(void)
	{
	DEBUG("+CSwiInstallTask::DoTaskL");
	//iDriver->Log(_L("This is a log entry from a different thread"));
	// create installer
	iUi = new (ELeave) CSwiUi(this);
	iDlUi = new (ELeave) CSwiDownloaderUi(this);
	iInstaller = CJavaInstaller::NewL(
		*iUi, //MJavaInstallerUI& aUiHandler, 
		*iDlUi, // MJarDownloaderUI& aDownloaderUI, 
		KEmpty, // const TDesC8& aRevocationServerURI,
		ETrue, // TBool aAutoDelete, 
		KDefaultIAP); // TUint32 aIapNumber);
		
	// create launch info
	iLaunchInfo = 
		CMIDPInstallationLaunchInfo::NewL(
			iDriver->Fs(), 
			*iJad, 
			KEmpty16,
			KDefaultJadMimeType, 
			*iJar, 
			KEmpty16, 
			KDefaultJarMimeType);

	iInstaller->Install(*iLaunchInfo, iStatus);
	DEBUG("-CSwiInstallTask::DoTaskL");
	}

void CSwiInstallTask::DoTaskCleanupL(void)
	{
	
	}

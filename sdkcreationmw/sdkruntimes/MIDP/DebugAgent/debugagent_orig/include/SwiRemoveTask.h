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


#ifndef HEADER_SWI_REMOVE_TASK
#define HEADER_SWI_REMOVE_TASK

#include "SwiTask.h"

#include "SwiRemoverUi.h"
#include "SWInstApi.h"
#include "ifeui.h"

class CSwiRemoveTask : public CSwiTask
	{
public:
	CSwiRemoveTask(
		CDriver* aDriver, 
		CPassiveLogger* aLogger,
		TUid aUid
		);
		
	~CSwiRemoveTask();

	void RunL(void);
	void DoTaskL(void);
	void DoTaskCleanupL(void);

public:
	TUid 				iUid;
	TInt 				iAttempt;
	CSwiRemoverUi*		iRemoverUi;
	CJavaRemover*		iRemover;
	
	RTimer iTimer;
	RSWInstSilentLauncher iInstaller;
	TUninstallOptions iUnInstallOptions;
	TUninstallOptionsPckg iUnInstallPckg;
	
	};

#endif


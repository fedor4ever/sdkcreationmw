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


#include "SwiTask.h"
#include "PassiveLogger.h"

CSwiTask::CSwiTask(CDriver* aDriver, CPassiveLogger* aLogger) :
	CActive(EPriorityNormal), iDriver(aDriver), iPassiveLogger(aLogger)
	{
	}
	
CSwiTask::~CSwiTask()
	{	
	Cancel();
	}


void CSwiTask::RunL()
	{
	CActiveScheduler::Stop();
	DoTaskCleanupL();
	}
	
TInt CSwiTask::RunError(TInt aError)
	{
	return aError;
	}
	
void CSwiTask::DoCancel()
	{
	
	}

void CSwiTask::WrapTaskL()
	{
	CActiveScheduler::Add(this);
	DoTaskL();
	SetActive();
	}

void CSwiTask::Log(const TDesC& aLine)
	{
	TRAPD(e,iPassiveLogger->AppendL(aLine));
	}
	

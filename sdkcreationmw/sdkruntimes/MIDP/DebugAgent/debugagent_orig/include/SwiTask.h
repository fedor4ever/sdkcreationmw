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


#ifndef HEADER_SWI_TASK
#define HEADER_SWI_TASK

#include <e32base.h>

class CDriver;
class CPassiveLogger;

class CSwiTask : public CActive
	{
public:
	CSwiTask(CDriver* aDriver, CPassiveLogger* aLogger);
	~CSwiTask();

	TRequestStatus& TaskStatus()
		{
		return iStatus;
		}
	
	// 
	void WrapTaskL();
	CPassiveLogger* Logger() 
		{
		return iPassiveLogger;
		}
	
	//	
	virtual void DoTaskL(void) = 0;
	virtual void DoTaskCleanupL(void) = 0;

	void Log(const TDesC& aString);

public:	
	// CActive
	virtual void RunL();
	virtual TInt RunError(TInt aError);
	virtual void DoCancel();

public:
	CDriver* 			iDriver;
	CPassiveLogger*		iPassiveLogger;
//	TRequestStatus iStatus;
	};

#endif
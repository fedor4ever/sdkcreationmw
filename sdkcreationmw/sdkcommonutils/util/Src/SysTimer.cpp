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


#include "SysTimer.h"

/****************************************************************************
*	CSysTimer()
*****************************************************************************
*
*
*
****************************************************************************/
EXPORT_C CSysTimer::CSysTimer(TInt aPriority)
:CTimer(aPriority)
	{
	CActiveScheduler::Add(this);
	}
/****************************************************************************
*	~CSysTimer()
*****************************************************************************
*
*
*
****************************************************************************/
EXPORT_C CSysTimer::~CSysTimer()
	{
	DoCancel();
	iClient = 0;
	}
/****************************************************************************
*	ConstructL
*****************************************************************************
*
*
*
****************************************************************************/
EXPORT_C void CSysTimer::ConstructL(CSysTimer_if* client, int num)
	{
	iClient = client;
	iUserNum = num;
	CTimer::ConstructL();
	}
/****************************************************************************
*	NewL
*****************************************************************************
*
*
*
****************************************************************************/
EXPORT_C CSysTimer* CSysTimer::NewL(CSysTimer_if* client, int num
									, TInt aPriority)
	{
	CSysTimer* self = new (ELeave)CSysTimer(aPriority);
	CleanupStack::PushL(self);
	self->ConstructL(client, num);
	CleanupStack::Pop();
	return self;
	}
/****************************************************************************
*	RunL()
*****************************************************************************
*
*
*
****************************************************************************/
EXPORT_C void CSysTimer::RunL()
	{
	iClient->Timer(iUserNum);
	}
/****************************************************************************
*	Seconds(int s)
*****************************************************************************
*
*
*
****************************************************************************/
EXPORT_C void CSysTimer::Seconds(int s)
	{
	//TTimeIntervalMicroSeconds32 ms(1000000*s); // One second
	//After(ms);
	// Maximum for this type of timer is 35 minutes 47 seconds
#ifdef _DEBUG
	if(s > (35*60)+47)
		{
		User::Panic(_L("CSysTimer"), KErrTooBig);
		}
#endif
	MilliSeconds(s*1000);
	}
/****************************************************************************
*	MilliSeconds(int s)
*****************************************************************************
*
*
*
****************************************************************************/
EXPORT_C void CSysTimer::MilliSeconds(int s)
	{
	// Maximum for this type of timer is 35 minutes 47 seconds
#ifdef _DEBUG
	if(s > ((35*60)+47)*1000)
		{
		User::Panic(_L("CSysTimer"), KErrTooBig);
		}
#endif
	MicroSeconds(s*1000);
	}
/****************************************************************************
*	MicroSeconds(int s)
*****************************************************************************
*
*
*
****************************************************************************/
EXPORT_C void CSysTimer::MicroSeconds(int s)
	{
	// Maximum for this type of timer is 35 minutes 47 seconds
#ifdef _DEBUG
	if(s > ((35*60)+47)*1000*1000)
		{
		User::Panic(_L("CSysTimer"), KErrTooBig);
		}
#endif
	TTimeIntervalMicroSeconds32 ms(s); // One second

	if(IsActive())
		{
		Cancel();
		}
	After(ms);
	}

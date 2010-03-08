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


// Includes
#include "waiter.h"


/****************************************************************************
*	CWaiter()
*****************************************************************************
*
*
*
****************************************************************************/
EXPORT_C CWaiter::CWaiter(TInt aPriority)
: CActive(aPriority)
	{
	CActiveScheduler::Add(this);
	}
/****************************************************************************
*	ConstructL(CWaiter_if* cb, int idx)
*****************************************************************************
*
*
*
****************************************************************************/
EXPORT_C void CWaiter::ConstructL(CWaiter_if* cb, int idx)
	{
	iCb = cb;
	iOwner = RThread().Id();	
	iUserIdx = idx;
	User::LeaveIfError(iMutex.CreateLocal());
	}
/****************************************************************************
*	NewL(CWaiter_if* cb, int idx)
*****************************************************************************
*
*
*
****************************************************************************/
EXPORT_C CWaiter* CWaiter::NewL(CWaiter_if* cb, int idx, TInt aPriority)
	{	
	CWaiter* self = new (ELeave)CWaiter(aPriority);
	CleanupStack::PushL(self);
	self->ConstructL(cb, idx);
	CleanupStack::Pop();
	return self;
	}
/****************************************************************************
*	DoCancel()
*****************************************************************************
*
*
*
****************************************************************************/
EXPORT_C void CWaiter::DoCancel()
	{
	}
/****************************************************************************
*	~CWaiter()
*****************************************************************************
*
*
*
****************************************************************************/
EXPORT_C CWaiter::~CWaiter()
	{
	}
/****************************************************************************
*	RunL()
*****************************************************************************
*
*
*
****************************************************************************/
EXPORT_C void CWaiter::RunL()
	{
	iCb->now(iUserIdx);
	// To make sure only one cllback is delivered at the time
	iMutex.Signal();
	}
/****************************************************************************
*	Wait()
*****************************************************************************
*
*
*
****************************************************************************/
EXPORT_C void CWaiter::Wait()
	{
	iStatus = KRequestPending;
	SetActive();
	}
/****************************************************************************
*	Complete()
*****************************************************************************
*
*
*
****************************************************************************/
EXPORT_C void CWaiter::Complete()
{
	// To make sure only one cllback is delivered at the time
	iMutex.Wait();	// Do not deliver new event's before old has been processed

	RThread srv;
	srv.Open(iOwner);
	TRequestStatus* p = &iStatus;
	srv.RequestComplete(p,KErrNone);
}

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



#include "timeoutimer.h"
#include "MsgCommandConnection.h"

CTimeOutTimer::CTimeOutTimer(const TInt aPriority)
    : CTimer(aPriority)
    {
    }

CTimeOutTimer::~CTimeOutTimer()
    {
	Cancel();
    }

CTimeOutTimer* CTimeOutTimer::NewL(
    const TInt aPriority,
    CMsgCommandConnection* aConnection)
    {
    CTimeOutTimer *p = new (ELeave) CTimeOutTimer(aPriority);
    CleanupStack::PushL(p);
	p->ConstructL(aConnection);
	CleanupStack::Pop();
    return p;
    }

void CTimeOutTimer::ConstructL(CMsgCommandConnection* aConnection)
    {
	iCommandConnection=aConnection;
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
    }

void CTimeOutTimer::RunL()
// Timer request has completed, so notify the timer's owner
    {
	iCommandConnection->ConnectionTimeOutL();
	}
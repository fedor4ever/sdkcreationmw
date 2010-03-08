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


#include "ShutdownObject.h"

CShutdownObject* CShutdownObject::NewL(void)
	{
	CShutdownObject* so = new (ELeave) CShutdownObject;

	CleanupStack::PushL(so);
	User::LeaveIfError(so->iHandle.Open(so->iHandle.Id()));
	CActiveScheduler::Add(so);
	so->iStatus = KRequestPending;
	so->SetActive();
	CleanupStack::Pop(so);
	return so;
	}

CShutdownObject::CShutdownObject():
	CActive(EPriorityNormal)
	{
	}

CShutdownObject::~CShutdownObject()
	{
	Cancel();
	iHandle.Close();
	}

void CShutdownObject::DoCancel(void)
	{
	}

void CShutdownObject::RunL(void)
	{
	CActiveScheduler::Stop();
	}

void CShutdownObject::Shutdown(void)
	{
	TRequestStatus* localStatus = &iStatus;

	iHandle.RequestComplete(localStatus, KErrNone);
	}


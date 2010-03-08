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
#include "Driver.h"


CPassiveLogger* CPassiveLogger::NewL(CDriver* aDriver) 
	{
	CPassiveLogger* object = new (ELeave) CPassiveLogger(aDriver);
	CleanupStack::PushL(object);
	object->ConstructL();
	CleanupStack::Pop(1);
	return object;
	}

CPassiveLogger::CPassiveLogger(CDriver* aDriver) 
	: iDriver(aDriver)
	{
	}

void CPassiveLogger::ConstructL() 
	{
	}

CPassiveLogger::~CPassiveLogger()
	{
	iBuffer.Reset();
	}

void CPassiveLogger::AppendL(const TDesC& aLogLine)
	{
	iBuffer.Append(aLogLine.AllocL());
	}

void CPassiveLogger::Flush()
	{
	TInt count = iBuffer.Count();
	for ( int i = 0 ; i < count ; i++ )
		{
		HBufC* buf = iBuffer[i];
		iDriver->Log(*buf);
		delete buf;
		}
	iBuffer.Reset();
	}


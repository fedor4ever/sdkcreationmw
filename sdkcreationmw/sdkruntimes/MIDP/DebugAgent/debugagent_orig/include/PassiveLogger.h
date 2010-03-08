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


#ifndef HEADER_ACTIVE_LOGGER
#define HEADER_ACTIVE_LOGGER

#include <e32base.h>

class CDriver;

class CPassiveLogger 
	{
public:
	static CPassiveLogger* NewL(CDriver* aDriver);

	~CPassiveLogger();

	void AppendL(const TDesC& aLogLine);

	void Flush();
		
private:
	CPassiveLogger(CDriver* aDriver);
	void ConstructL();
	
	void SyncLock();
	void SyncUnlock();

private:
	CDriver* 			iDriver;
	RArray<HBufC*>		iBuffer;
	};

#endif
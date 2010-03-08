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


#ifndef HEADER_CMD_LINE_DRIVER
#define HEADER_CMD_LINE_DRIVER

#include <e32cons.h>

#include "Driver.h"
#include "ShutdownObject.h"
#include "TcpServer.h"

class CCmdLineDriver : public CDriver
	{
public:
	static CCmdLineDriver* NewL();
	virtual ~CCmdLineDriver();

	void InitL();
	void Log(const TDesC& aString);
	void Message(const TDesC& aString);
	RFs& Fs();
	CAgentEngine* Engine();
	void Shutdown();

private:
	void ConstructL();
	CCmdLineDriver();

private:
	CConsoleBase*		iConsole; 
	CShutdownObject*	iShutdownObject;
	RFs					iFs;
	CAgentEngine*		iEngine;
	};

#endif

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


#ifndef HEADER_UI_DRIVER
#define HEADER_UI_DRIVER


#include "Driver.h"
#include "TcpServer.h"

class CDebugAgentView;


_LIT(KLogFileName, "c:\\midp2\\jdebugagent\\agent.log");

class CUiDriver : public CDriver
	{
public:
	static CUiDriver* NewL(CDebugAgentView* aView);
	virtual ~CUiDriver();

	// from CDriver
	void InitL();
	void Log(const TDesC& aString);
	void Message(const TDesC& aString);
	RFs& Fs();
	CAgentEngine* Engine();
	void Shutdown();
	void SettingsChange();
	void StartTcpServerL();
	CTcpServer* GetTcpServer();

private:
	void ConstructL();
	CUiDriver(CDebugAgentView* aView);
	void LogToFileL(const TDesC& aString);
	void InitLog();
	void SetFileLog(TBool aFileLog );

private:
	RFs					iFs;
	RFile 				iLogFile;
	CAgentEngine*		iEngine;
	CTcpServer* 		iServer;

	CDebugAgentView* 	iView;
	};

#endif

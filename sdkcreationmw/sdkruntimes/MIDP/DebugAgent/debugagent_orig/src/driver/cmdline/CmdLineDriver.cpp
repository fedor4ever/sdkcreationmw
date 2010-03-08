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


#include "CmdLineDriver.h"
#include "jdebug.h"

#ifdef DEBUG_AGENT_ENGINE_AMS
#include "AmsEngine.h"
#else
#include "SwiEngine.h"
#endif


#define DEV_DEBUG 0


	
CCmdLineDriver* CCmdLineDriver::NewL()
	{
	CCmdLineDriver* object = new (ELeave) CCmdLineDriver();
    CleanupStack::PushL(object);
	object->ConstructL();
    CleanupStack::Pop(object);
	return object;
	}

CCmdLineDriver::CCmdLineDriver()
	{
	}

CCmdLineDriver::~CCmdLineDriver()
	{
	}

void CCmdLineDriver::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	//need to share the session BEFORE the swi installation on another Thread
	iFs.ShareProtected();
	// On EKA2, window server may not have started
	TInt retries = 100;
	TInt result = -1;
	while ( 0 < (retries--) ) 
		{
		TRAP( result, iConsole = Console::NewL(KProgName,TSize(KConsFullScreen,KConsFullScreen)) );
		if ( result == KErrNone )
			{
			break;
			}
		// wait a bit
		User::After(1000000);
		}
	if ( result != KErrNone )
		{
		User::Leave(result);
		}
	// we should have a console now
#ifdef DEBUG_AGENT_ENGINE_AMS
	iEngine = new (ELeave)CAmsEngine(this);
#else
	iEngine = new (ELeave)CSwiEngine(this);
#endif
	}




void CCmdLineDriver::InitL()
	{
	CActiveScheduler* as = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(as);
	CActiveScheduler::Install(as);
	iShutdownObject = CShutdownObject::NewL();

#if DEV_DEBUG
    _LIT(KTxtPressToContinue, "Press any key to start...");
    iConsole->Printf(KTxtPressToContinue);
	iConsole->Getch();
#endif

#if (defined (__WINS__) || defined(__WINSCW__))    
	_LIT(KPleaseWait, "Please wait...\r\n");
    iConsole->Printf(KPleaseWait);
	// wait for the emulator to start
	User::After(10000000);
#endif

    _LIT(KTxtPort, "Running on port %d.\r\n");
    TBuf<256> txt;
    txt.Format(KTxtPort, GetAgentPort());
    iConsole->Printf(txt);

    // create & start SEI slave
	_LIT(KTCP, "tcp");
	TSockAddr address;
	address.SetPort(GetAgentPort());
	CTcpServer* server = CTcpServer::NewLC(this);
	server->StartListeningL(
		KTCP, 
		address, 
		10 ); // server socket queue len

	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(server);
	CleanupStack::PopAndDestroy(as);
	}

void CCmdLineDriver::Log(const TDesC& aString)
	{
	DEBUG_STR("%S", aString);
	iConsole->Printf(aString);
	iConsole->Printf(KCRLF);
	}

void CCmdLineDriver::Message(const TDesC& aString)
	{
	Log(aString);
	}

RFs& CCmdLineDriver::Fs()
	{
	return iFs;
	}

CAgentEngine* CCmdLineDriver::Engine() 
	{
	return iEngine;
	}

void CCmdLineDriver::Shutdown()
	{
	iShutdownObject->Shutdown();
	}



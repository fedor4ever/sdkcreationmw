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


#include "UiDriver.h"
#include "ui/DebugAgentView.h"
#include <e32math.h>

#ifdef DEBUG_AGENT_ENGINE_AMS
#include "AmsEngine.h"
#else
#include "SwiEngine.h"
#endif


const TInt KPinLength = 4;
_LIT8(KCRLF8,"\r\n");
_LIT(KPinPrintString, "Pin code for this session:");
CUiDriver* CUiDriver::NewL(CDebugAgentView* aView)
	{
	CUiDriver* object = new (ELeave) CUiDriver(aView);
    CleanupStack::PushL(object);
	object->ConstructL();
    CleanupStack::Pop(object);
	return object;
	}

CUiDriver::CUiDriver(CDebugAgentView* aView):
	CDriver(), iEngine(NULL), iServer(NULL), iView(aView)
	{
	}

CUiDriver::~CUiDriver()
	{
	if (IsFileLog())
		{
		iLogFile.Close();
		}
	iFs.Close();
	if ( iServer != NULL )
		{
		delete iServer;
		}
	if ( iEngine != NULL )
		{
		delete iEngine;
		}
	}

void CUiDriver::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	//need to share the session BEFORE the swi installation on another Thread
	iFs.ShareProtected();
#ifdef DEBUG_AGENT_ENGINE_AMS
	iEngine = new (ELeave)CAmsEngine(this);
#else
	iEngine = new (ELeave)CSwiEngine(this);
#endif
	}

void CUiDriver::InitL()
	{
	LoadPrefsL();
	if ( IsFileLog())
		{
		InitLog();
		}
	if (IsAutoCleanEnabled())
		{
		Log(_L("Cleaning up old data..."));
		iEngine->Cleanup();
		Log(_L("Done cleaning..."));
		}
	// setup pin code
	if ( IsUsePin() )
		{
		// generate random KPinLength digit pin
		TInt rand = (TInt)(0x7fffffff & (Math::Random()));
		TBuf<32> buf;
		buf.AppendNum(rand);
		SetPinCodeL(buf.Left(KPinLength));
		Log(KPinPrintString);
		Log(*GetPinCode());
		}
	}

void CUiDriver::StartTcpServerL()
	{
    // create & start SEI slave
	_LIT(KTCP, "tcp");
	TSockAddr address;
	address.SetPort(GetAgentPort());
	iServer = CTcpServer::NewLC(this);
    _LIT(KStartingServer, "Starting server on port %d...");
    TBuf16<128> buf;
    buf.Format(KStartingServer, GetAgentPort());
    Log(buf);
	iServer->StartListeningL(
		KTCP, 
		address, 
		10 ); // server socket queue len
	CleanupStack::Pop(1);
	Log(_L("Server started"));
	}

void CUiDriver::Log(const TDesC& aString)
	{
	// ignore error
	TRAPD(e,iView->AppendL(aString));
	if ( IsFileLog() )
		{
		TRAPD(err,LogToFileL(aString));
		}
	}

void CUiDriver::SetFileLog(TBool aFileLog)
	{
	if ( aFileLog != IsFileLog() )
		{
		CDriver::SetFileLog(aFileLog);
		if ( aFileLog )
			{
			InitLog();
			}
		else
			{
			iLogFile.Close();
			}
		}
	}

void CUiDriver::LogToFileL(const TDesC& aString)
	{
	HBufC8* buf = HBufC8::NewLC(2*aString.Length());
	TPtr8 ptr = buf->Des();
	ptr.Copy(aString);
	iLogFile.Write(ptr);
	iLogFile.Write(KCRLF8);
	CleanupStack::PopAndDestroy(buf);
	}

RFs& CUiDriver::Fs()
	{
	return iFs;
	}

void CUiDriver::Shutdown()
	{
	if ( iServer != NULL )
		{
		delete iServer;
		iServer = NULL;
		}
	}


CAgentEngine* CUiDriver::Engine() 
	{
	return iEngine;
	}

void CUiDriver::Message(const TDesC& aString)
	{
	Log(aString);
	}

void CUiDriver::SettingsChange()
	{
	//TODO: ignore .. iView->Shutdown();
	}

void CUiDriver::InitLog()
	{
	TInt res = iLogFile.Open(iFs, KLogFileName, EFileWrite|EFileShareAny);
	if ( res != KErrNone )	
		{
		TParse dir;
		dir.Set(KLogFileName,NULL,NULL);
		iFs.MkDirAll(dir.DriveAndPath());
		res = iLogFile.Create(iFs, KLogFileName, EFileWrite|EFileShareAny);
		if ( res != KErrNone )	
			{
			SetFileLog(EFalse);
			}
		}
	TInt sz;
	iLogFile.Size(sz);
	iLogFile.Seek(ESeekEnd,sz);
	}
	
CTcpServer* CUiDriver::GetTcpServer()
	{
	return iServer;
	}


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



#include "Driver.h"
	
#include <es_sock.h>
#include <in_sock.h>
#include <e32base.h>
#include <es_sock.h>
#include <jdebug.h>
	
CDriver::CDriver()
	: iFileLog(EFalse), iRandomPort(KRandomPortStart),
	  iPinCode(NULL), iPCPort(KPCPort)
	{
	iPCHost = KPCHost().AllocL();
	}
	
CDriver::~CDriver()
	{
	if ( iPinCode != NULL )
		{
		delete iPinCode;
		}
	if ( iPCHost != NULL )
		{
		delete iPCHost;
		}
	}
	
void CDriver::Log(TDes8& aString, TBool aUnicode)
    {
    TPtrC8 ptrc(aString);
    Log(ptrc, aUnicode);
    }

void CDriver::Log(TDes& aString)
    {
    TPtrC ptrc(aString);
    Log(ptrc);
    }

void CDriver::Log(TDesC8& aString, TBool aUnicode)
    {
    if ( aUnicode )
        {
        TPtrC ptr ((TUint16*)(aString.Ptr()), aString.Length()/2 );
        Log(ptr);
        }
    else 
        {
		TRAPD(e, LogAllocL(aString));
        }
    }


void CDriver::LogAllocL(TDesC8& aString)
	{
    HBufC* buff = HBufC::NewLC(aString.Length()); 
    TPtr ptr = buff->Des();
    ptr.Copy(aString);
    TPtrC ptrc(ptr);
    Log(ptrc);
    CleanupStack::PopAndDestroy(buff);
	}

TBool CDriver::IsAutoCleanEnabled()
	{
	return iAutoClean;
	}
	
void CDriver::SetAutoCleanEnabled(TBool aAutoClean)
	{
	if ( iAutoClean != aAutoClean)
		{
		iAutoClean = aAutoClean;
		SettingsChange();
		}
	}
	
TBool CDriver::IsFileLog()
	{
	return iFileLog;
	}
	
void CDriver::SetFileLog(TBool aFileLog)
	{
	if ( iFileLog != aFileLog)
		{
		iFileLog = aFileLog;
		SettingsChange();
		}
	}
	
TInt CDriver::GetAgentPort()
	{
	return iPort;	
	}
	
void CDriver::SetAgentPort(TInt aPort)
	{
	if ( aPort != iPort )
		{
		iPort = aPort;
		SettingsChange();
		}
	}

void CDriver::LoadPrefsL()
	{
	RFs& fs = Fs();
	RFile prefs;
	TInt res = prefs.Open(fs,KPrefsFile,EFileRead);
	if ( res != KErrNone )
		{
		// defaults
		iAutoClean = ETrue;
		iPort = KSeiSlavePort;
		return;
		}
	// file is open
	TInt fileSize;
	prefs.Size(fileSize);
	CleanupClosePushL(prefs);
	HBufC8* buf = HBufC8::NewLC(fileSize);
	TPtr8 ptr = buf->Des();
	prefs.Read(ptr,fileSize);
	
	TUint8* data = (TUint8*)ptr.Ptr();

	TUint8 autoclean = *data;
	if ( autoclean )
		{
		iAutoClean = ETrue;
		}
	else
		{
		iAutoClean = EFalse;
		}
		
	data++;

	TUint8 fileLog = *data;
	if ( fileLog )
		{
		iFileLog = ETrue;
		}
	else
		{
		iFileLog = EFalse;
		}
		
	data++;
	TUint8 usePin = *data;
	if ( usePin )
		{
		iUsePin = ETrue;
		}
	else
		{
		iUsePin = EFalse;
		}
		
	data++;
	TUint portL = *data;
	data++;
	TUint portH = *data;
	iPCPort = (portH<<8) + portL;
	
	data++;
	TPtrC8 hostPtr(data, fileSize-5);
	
	delete iPCHost;
	iPCHost = NULL;
	iPCHost = HBufC::NewL(fileSize-5);
	iPCHost->Des().Copy(hostPtr);
	
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PopAndDestroy(&prefs);
	}
	
void CDriver::SavePrefsL()	
	{
	RFs& fs = Fs();
	RFile prefs;
	TInt res = prefs.Replace(fs,KPrefsFile,EFileWrite);
	if ( res != KErrNone )
		{
		res = fs.MkDirAll(KPrefsFile);
		res = prefs.Create(fs,KPrefsFile,EFileWrite);
		User::LeaveIfError(res);
		}
		
	CleanupClosePushL(prefs);
	HBufC8* buf = HBufC8::NewLC(5+iPCHost->Length());
	TPtr8 ptr = buf->Des();
	TUint8 *data = (TUint8*)ptr.Ptr();
	
	*data = iAutoClean?((TUint8)1):((TUint8)0);
	data++;
	*data = iFileLog?((TUint8)1):((TUint8)0);
	data++;
	*data = iUsePin?((TUint8)1):((TUint8)0);
	data++;
	TUint8 port = iPCPort;
	*data = port;
	data++;
	port = iPCPort>>8;
	*data = port;
	
	ptr.SetLength(5);
	ptr.Append(*iPCHost);
	
	prefs.Write(ptr);
	
	CleanupStack::PopAndDestroy(buf);
	CleanupStack::PopAndDestroy(&prefs);
	}

TInt CDriver::NextRandomPort()
	{
	return iRandomPort++;
	}

HBufC* CDriver::GetPinCode()
	{
	return iPinCode;
	}
	
void CDriver::SetPinCodeL(const TDesC& aPin)
	{
	if ( iPinCode != NULL )
		{
		delete iPinCode;
		}
	iPinCode = aPin.AllocL();		
	}

TBool CDriver::IsUsePin()
	{
#if ( defined(__WINS__) || defined(__WINSCW__) )
	return EFalse;
#else
	return iUsePin;
#endif
	}
	
void CDriver::SetUsePin(TBool aUsePin)
	{
	if ( iUsePin != aUsePin)
		{
		iUsePin = aUsePin;
		SettingsChange();
		}
	}
	


/**
* Done on startup for test purpose.
* Pinging a server will open EPOC network interfaces and
* get an IP from the DHCP server.
*/
TInt CDriver::PingL(TDesC& aHost, TInt aPort)
	{
	DEBUG("+CDriver::PingL");
	TInt		hostLength  = aHost.Length();
	TUint16*	host = new (ELeave) TUint16[hostLength];

	for (TInt i = 0; i < hostLength; i++)
		{
		host[i] = static_cast<TUint16>(aHost[i]);
		}

	TPtrC		hostPtr(host, hostLength);
	TInetAddr	address;

	DEBUG("Opening socketserver...");
	RSocketServ socketServer;
	User::LeaveIfError(socketServer.Connect());

	TRequestStatus	status;

	TInt statusint = address.Input(hostPtr);

	if (statusint != KErrNone)
		{
		RHostResolver	resolver;

		DEBUG("Opening resolver...");
		TInt err = resolver.Open(socketServer, KAfInet, KProtocolInetTcp);
		User::LeaveIfError(err);
		DEBUG("Opening socketserver...");

		TNameEntry entry;

		DEBUG_STR("GetByName %S", hostPtr);
		resolver.GetByName(hostPtr, entry, status);
		User::WaitForRequest(status);

		statusint = status.Int();

		if ( statusint != KErrNone )
			{
			DEBUG_INT("Resolver error : %d" , statusint);
			User::Leave(statusint);
			}
	
		resolver.Close();

		DEBUG("Get address");
		address = entry().iAddr;
		}
	address.SetPort(aPort);
	delete [] host;

	TRequestStatus	connstatus;
	DEBUG("Open socket");
	RSocket socket;
	socket.Open(socketServer, KAfInet, KSockStream, KProtocolInetTcp );
	DEBUG("Connect");
	socket.Connect(address, connstatus);
	DEBUG("Waiting...");
	User::WaitForRequest(connstatus);
	DEBUG_INT("OpenL connect status = %d", connstatus.Int());
	DEBUG("Closing socket");
	socket.Close();
	socketServer.Close();
	DEBUG("-CDriver::PingL");
	return statusint;
	}



TInt CDriver::GetPCPort()
	{
	return iPCPort;
	}
	
void CDriver::SetPCPort(TInt aPort)
	{ 
	iPCPort = aPort;
	}
	
HBufC* CDriver::GetPCHost()
	{
	return iPCHost;
	}
	
void CDriver::SetPCHostL(const TDesC& aHost)
	{
	if ( iPCHost != NULL )
		{
		delete iPCHost;
		}
	iPCHost = aHost.AllocL();		
	}

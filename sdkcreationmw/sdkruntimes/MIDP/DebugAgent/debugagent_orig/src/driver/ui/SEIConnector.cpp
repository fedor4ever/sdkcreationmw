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



#include "SEIConnector.h"
#include "TcpServerHandler.h"
#include "Driver.h"
#include <jdebug.h>




CSEIConnector* CSEIConnector::NewL(CDriver* aDriver)
	{
	CSEIConnector* object = new (ELeave) CSEIConnector(aDriver);
	object->ConstructL();
	CActiveScheduler::Add(object);
	return object;
	}

CSEIConnector::CSEIConnector(CDriver* aDriver)
:   CActive(CActive::EPriorityStandard), iDriver(aDriver)
	{
	}

void CSEIConnector::ConstructL()
	{
	User::LeaveIfError(iSocketServer.Connect());
	}

CSEIConnector::~CSEIConnector()
	{
	Cancel();
	TODO: iSocket.Close();
	}

void CSEIConnector::Reschedule()
	{
	iStatus = KRequestPending;
	TRequestStatus* ptrStat = &iStatus;
	SetActive();
	User::RequestComplete(ptrStat,KErrNone);
	}


void CSEIConnector::MakeOutgoingConnectionL(const TDesC& aHost, TInt aPort)
	{
	GetByNameL(aHost, aPort);
	}
	

/*
//got 7 and 1 from SEI => Socket configured properly.
void CSEIConnector::MakeOutgoingConnectionL(const TDesC& aHost, TInt aPort)
	{
	TInt		hostLength  = aHost.Length();
	TUint16*	host = new (ELeave) TUint16[hostLength];

	for (TInt i = 0; i < hostLength; i++)
		{
		host[i] = static_cast<TUint16>(aHost[i]);
		}

	TPtrC		hostPtr(host, hostLength);
	TInetAddr	address;

	TInt statusint = address.Input(hostPtr);

	if (statusint != KErrNone)
		{
		RHostResolver	resolver;

		DEBUG("Opening resolver");
		TInt err = resolver.Open(iSocketServer, KAfInet, KProtocolInetTcp);
		if(err != KErrNone)
			{
			iDriver->Log(_L("Failed to open Resolver"));
			return;
			}
		
		CleanupClosePushL(resolver);

		TNameEntry entry;

		DEBUG_STR("GetByName %S", hostPtr);
		resolver.GetByName(hostPtr, entry, iStatus);
		User::WaitForRequest(iStatus);
		CleanupStack::PopAndDestroy(&resolver);
		statusint = iStatus.Int();
	
		if ( statusint != KErrNone )
			{
			iDriver->Log(_L("Resolver error"));
			return;
			}
	
		DEBUG("Get address");
		address = entry().iAddr;
		}
	address.SetPort(aPort);
	delete [] host;

	DEBUG("Open socket");
	CTcpServerHandler* handler = CTcpServerHandler::NewL(iDriver);
	RSocket& socket = handler->InitSocketL(iSocketServer);
	socket.Open(iSocketServer, KAfInet, KSockStream, KProtocolInetTcp );
	iDriver->Log(_L("Opening outgoing connection.."));
	socket.Connect(address, iStatus);//TODO: is it possible to have more then 1 pending request??
	//TODO: SetActive();
	User::WaitForRequest(iStatus);
	if(iStatus.Int() != KErrNone)
		{
		iDriver->Log(_L("Connection failed"));
		}
		else
		{
		iDriver->Log(_L("Established outgoing connection"));
		}
	TInt            iCommandCode;
	TPtr8			iCommandPtr((TUint8*)(&iCommandCode), 0, 4);
	iStatus = KRequestPending;
	socket.Read(iCommandPtr, iStatus);
	User::WaitForRequest(iStatus);
	TInt commandCode = ByteOrder::Swap32(iCommandCode);	
	
	TInt            portsNum;
	TPtr8			portsPtr((TUint8*)(&portsNum), 0, 4);
	iStatus = KRequestPending;
	socket.Read(portsPtr, iStatus);
	User::WaitForRequest(iStatus);
	TInt ports = ByteOrder::Swap32(portsNum);	
    

		
	}	
*/	

void CSEIConnector::RunL()
	{
	TInt error = KErrNone;
	switch(iState)
		{
		case EGetByName:
			{
			ConnectSocketL();
			break;
			}
		case ESocketConnect:
			{
			ProcessRequestL();
			break;
			}
		}
	}

TInt CSEIConnector::RunError(TInt aError)
	{
	//TODO: see how to handle this
	_LIT(KErrorPattern, "@CSEIConnector::RunError: %d");
	TBuf16<128> buf;
	buf.Format(KErrorPattern, aError);
	iDriver->Log(buf);
	return KErrNone;
	}



void CSEIConnector::DoCancel()
	{
/*
	if ( iCommand != NULL )
		{
		iCommand->Cancel();
		}
*/
	}


void CSEIConnector::GetByNameL(const TDesC& aHost, TInt aPort)
	{
	iPort = aPort;
	TInt hostLength  = aHost.Length();
	iHost = new (ELeave) TUint16[hostLength];

	for (TInt i = 0; i < hostLength; i++)
		{
		iHost[i] = static_cast<TUint16>(aHost[i]);
		}

	TPtrC hostPtr(iHost, hostLength);

	TInt statusint = iAddress.Input(hostPtr);
	if (statusint != KErrNone)
		{
		DEBUG("Opening resolver");
		TInt err = iResolver.Open(iSocketServer, KAfInet, KProtocolInetTcp);
		if(err != KErrNone)
			{
			iDriver->Log(_L("Failed to open Resolver"));
			return;
			}
		
		DEBUG_STR("GetByName %S", hostPtr);
		iStatus = KRequestPending;
		iState = EGetByName;
		iResolver.GetByName(hostPtr, iEntry, iStatus);
		SetActive();
		}
		else
		{
		iDriver->Log(_L("Host address error"));	
		}
	}

void CSEIConnector::ConnectSocketL(void)
	{
	if (iStatus.Int() != KErrNone )
		{
		iDriver->Log(_L("Resolver error"));
		return;
		}

	DEBUG("Get address");
	iAddress = iEntry().iAddr;
	iAddress.SetPort(iPort);
	delete[] iHost;

	DEBUG("Open socket");
	iHandler = CTcpServerHandler::NewL(iDriver);
	iHandler->InitSocketL(iSocketServer);
	iHandler->Socket()->Open(iSocketServer, KAfInet, KSockStream, KProtocolInetTcp);
	iDriver->Log(_L("Opening outgoing connection.."));
	iState = ESocketConnect;
	iStatus = KRequestPending;
	iHandler->Socket()->Connect(iAddress, iStatus);
	SetActive();
	}
	
	
	
void CSEIConnector::ProcessRequestL()	
	{
	iState = EProcessRequest;
	if(iStatus.Int() != KErrNone)
		{
		iDriver->Log(_L("Connection failed"));
		}
		else
		{
		iDriver->Log(_L("Established outgoing connection"));
		iHandler->ProcessRequest();
		}
	
	}



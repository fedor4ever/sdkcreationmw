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



#include "TcpServer.h"

#include <es_sock.h>
#include <in_sock.h>
#include <e32base.h>
#include <es_sock.h>
#include <jdebug.h>


CTcpServer::CTcpServer(CDriver* aDriver)
: CActive(CActive::EPriorityStandard),
	iDriver(aDriver)
	{
    CActiveScheduler::Add(this); 
	}


void CTcpServer::ConstructL()
	{
    User::LeaveIfError(iServer.Connect());
	}


CTcpServer::~CTcpServer()
	{
	Cancel();
	delete iHandler;
	TRequestStatus stat;
	iListener.Shutdown(RSocket::EImmediate,stat);
	User::WaitForRequest(stat);
	iListener.Close();
	iServer.Close();
	}


CTcpServer* CTcpServer::NewLC(CDriver* aDriver)
	{
	CTcpServer* object = new (ELeave) CTcpServer(aDriver);
	CleanupStack::PushL(object);
	object->ConstructL();
	return object;
	}

void CTcpServer::RunL()
	{
	if (iStatus.Int() == KErrNone)
		{
		iDriver->Log(_L("New incoming connection"));
		iHandler->ProcessRequest();
		AcceptNextConnectionL();
		}
	else 
		{
		iDriver->Log(_L("Error"));
		}	
	}

void CTcpServer::DoCancel()
	{
	iListener.CancelAll();
	iHandler->Cancel();
	iDriver->Log(_L("Stopped."));
	}


void CTcpServer::StartListeningL(
		const TDesC& aProtocol, 
		const TSockAddr& aAddr, 
		TInt aQLength
		)
	{
	TProtocolDesc desc;
    User::LeaveIfError(iServer.FindProtocol(aProtocol, desc));

	User::LeaveIfError(iListener.Open( iServer
									 , desc.iAddrFamily
									 , desc.iSockType
									 , desc.iProtocol
									 ));

	iListenAddr = aAddr;
	TInt err = iListener.Bind(iListenAddr);
	if (err != KErrNone)
	{
		// Find another port
		iListenAddr.SetPort(0);
		User::LeaveIfError(iListener.Bind(iListenAddr));
	}

	User::LeaveIfError(iListener.Listen(aQLength));
	iListener.LocalName(iListenAddr);

	TBuf<255> output;
	TBuf<64> addrBuf;
	TInetAddr::Cast(iListenAddr).OutputWithScope(addrBuf);
	output.Format(_L("IP address %S"),&addrBuf);
	iDriver->Log(output);
	output.Format(_L("Listening on port %d"),iListenAddr.Port());
	iDriver->Log(output);

	AcceptNextConnectionL();
	}


void CTcpServer::AcceptNextConnectionL()
	{
	iHandler = CTcpServerHandler::NewL(iDriver);

	RSocket& client = iHandler->InitSocketL(iServer);

	iListener.Accept(client, iStatus);

	SetActive();
	}


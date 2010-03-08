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



#include "msgsvreng.h"
#include "msgcommandconnection.h"
#include <CommDbConnPref.h>

CMsgSvrEng::CMsgSvrEng() :
    CActive(CActive::EPriorityStandard)
    {
    iCommandSocket = NULL;
    }
CMsgSvrEng::~CMsgSvrEng()
    {
	iSocket.CancelAccept();
    iSocket.Close();
    delete iCommandSocket;
    Cancel();
    }
void CMsgSvrEng::ConstructL(MMsgCommandObserver* aNotify)
    {
    iNotify = aNotify;

   	CActiveScheduler::Add( this );

    }

CMsgSvrEng* CMsgSvrEng::NewL(MMsgCommandObserver* aNotify)
    {
    CMsgSvrEng* self = CMsgSvrEng::NewLC(aNotify);
    CleanupStack::Pop();
    return self;
    }

CMsgSvrEng* CMsgSvrEng::NewLC(MMsgCommandObserver* aNotify)
    {
    CMsgSvrEng* self = new ( ELeave) CMsgSvrEng();
    CleanupStack::PushL(self);
    self->ConstructL(aNotify);
    return self;
    }

void CMsgSvrEng::RunL()
    {

    if (iStatus == KErrNone)
        {
        switch ( iServerState )
            {
            case EWaitingForConnections:
                // create connection class
                iConnection = CMsgCommandConnection::NewL(iNotify, this);
                ASSERT(iConnection != NULL);
			    iConnection->StartConnection();
				break;
            /*case EClosingConnection:
                iNotify->WriteToConsole(_L("Connection closed\n"));
                delete iCommandSocket;
                iCommandSocket = NULL;
                WaitForConnection();
                break;*/
            case EShuttingDownServer:
                DestroyConnectionL();
                delete this;
                break;
            case EErrorRecovery:
                break;
            default:
                User::Panic(_L("CMsgSvrEng - Panic :"), KErrUnknown);
            }
    	}
	else
	    {
		DestroyConnectionL();
        delete this;
	    }
    }

void CMsgSvrEng::DoCancel()
    {
    TRequestStatus *status = (&iStatus);
    User::RequestComplete(status, KErrCancel);
    }

TInt CMsgSvrEng::RunError(TInt aError)
    {
    ShutDownServer();
    return aError;
    }


TInt CMsgSvrEng::StartL()
    {
    TInt err(KErrNone);
	// Make a connection to the Socket Server.
	iNotify->WriteToConsole(_L("Connecting to the Socket Server.\n"));
    User::LeaveIfError(iSocketServ.Connect());
    //open network connection
    User::LeaveIfError(iRConnection.Open(iSocketServ));
    TCommDbConnPref connPrefs;
    connPrefs.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
    connPrefs.SetDirection(ECommDbConnectionDirectionOutgoing);
	//In the SDK we remove all possible Access Points and add 
	//Winsock, which should then have an IapId=1
	//We assume no one plays with the APs (adds, removes)
	//after SDK creation and before running TCK tests
	//Therefore we can set preferred IapId=1,
	//since we connect over Winsock AP.
	//Thus we eliminate redundant AP query dialog
	//on every startup
	connPrefs.SetIapId(1);
    TRAP(err, iRConnection.Start(connPrefs));
    switch (err)
        {
        case KErrNone:
        case KErrAlreadyExists:
            break;  // this is ok
        default:
            User::Leave(err);
        }

	// Open a socket to listen on.
	iNotify->WriteToConsole(_L("Opening the Socket.\n"));
	User::LeaveIfError(iSocket.Open(iSocketServ,
        KAfInet,
        KSockStream,
        KProtocolInetTcp));

	// keep the port alive
	iSocket.SetOpt(KSoTcpKeepAlive, KSolInetIp, 1);
    
	// Bind the socket to the port.
	iNotify->WriteToConsole(_L("Binding the Socket.\n"));

    TInetAddr msgPort(KInetAddrAny, KSvrPort);
    msgPort.SetFamily(0);
	User::LeaveIfError(iSocket.Bind(msgPort));

	// Listen for incoming connections.
	iNotify->WriteToConsole(_L("Listening through the Socket.\n"));
	User::LeaveIfError(iSocket.Listen(100));

    WaitForConnection();

    return KErrNone;
    }



void CMsgSvrEng::WaitForConnection()
    {
	// create connection socket
    iCommandSocket = new (ELeave) RSocket;
    User::LeaveIfError(iCommandSocket->Open(iSocketServ));
    //iCommandSocket->SetOpt(KSoKeepInterfaceUp, KSolInetIp, 1);
    iSocket.Accept(*iCommandSocket,iStatus);
    //wait for connection
    iServerState = EWaitingForConnections;
    SetActive();
    iNotify->WriteToConsole(_L("Waiting for a Connection...\n"));
    }

void CMsgSvrEng::CloseConnection()
    {
    iNotify->WriteToConsole(_L("Closing Connection...\n"));
	DestroyConnectionL();
    delete iCommandSocket;
    iCommandSocket = NULL;
    WaitForConnection();
    }

void CMsgSvrEng::DestroyConnectionL()
{
	delete iConnection;
	iConnection = NULL;
}
void CMsgSvrEng::ShutDownServer()
    {
	iServerState = EShuttingDownServer;
    // SetActive();
    }
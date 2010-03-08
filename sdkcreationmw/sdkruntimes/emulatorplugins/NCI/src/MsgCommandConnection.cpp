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



#include "MsgCommandConnection.h"
#include "Message.h"
#include "Message.hrh"
#include "replystatus.hrh"
#include "timeoutimer.h"
#include "msgsvreng.h"

_LIT8(KCommandBegin, "<");


CMsgCommandConnection::CMsgCommandConnection() :
CActive( CActive::EPriorityStandard )
    {
    }

CMsgCommandConnection::~CMsgCommandConnection()
    {
    if (this->IsActive())
        Cancel();
    delete iTimer;
    iTimer = NULL;

    if (iMsg != NULL)
        delete iMsg;
    }

void CMsgCommandConnection::ConstructL(
	MMsgCommandObserver* aCmdLst_if,
	CMsgSvrEng* aEngine )
    {
	iNotify = aCmdLst_if;
	iCommand = ENoCommand;
	iEngine = aEngine;

    iSocket = aEngine->iCommandSocket;

	// Initialization of the timeout timer
	TimeOutTime = TTimeIntervalMicroSeconds32(KTimeOutTime);
	iTimer = CTimeOutTimer::NewL(EPriorityHigh,this);

	CActiveScheduler::Add( this );
    }

CMsgCommandConnection* CMsgCommandConnection::NewLC(
	MMsgCommandObserver* aCmdLst_if,
	CMsgSvrEng* aEngine )
    {
	CMsgCommandConnection* self = new ( ELeave ) CMsgCommandConnection();
	CleanupStack::PushL( self );
	self->ConstructL(aCmdLst_if, aEngine);
	return self;
    }

CMsgCommandConnection* CMsgCommandConnection::NewL(
    MMsgCommandObserver* aCmdLst_if,
    CMsgSvrEng* aEngine )
    {
	CMsgCommandConnection* self = CMsgCommandConnection::NewLC(
        aCmdLst_if,
        aEngine );
	CleanupStack::Pop();
	return self;
    }

void CMsgCommandConnection::DoCancel()
    {
     if (iTimer->IsActive())
    	iTimer->Cancel();
    }

void CMsgCommandConnection::RunL()
    {
    iTimer->Cancel();
    TInt err(0);

    switch (iStatus.Int())
        {
        case KErrNone:
            switch ( iConState )
                {
                case EWaitingCommand:
                    WaitForCommand();
                    break;
                case EReceivingCommand:
                    TRAP(err ,ParseCommandL() );
                    switch(err)
                        {
                        case KErrNone:
                            // command compelete 
                            ProcessCommandL();
                            break;
                        case KErrNotReady:
                            // wait for command to complete
                            iTimer->After(TimeOutTime);
                            WaitForCommand();
                            break;
                        case KErrNotSupported:
                            // command seems to be unvalid
                            Reply((TDesC8&)KStatusBadRequest);
                            break;
                        default:
                            // some other error occured
                            Reply((TDesC8&)KStatusInternalServerError);
                        }
                    break;
                case ESendReply:
                    // Command completed clean command buffer
                    iCmdData.SetLength(0);
                    iCmdData.FillZ();
                    iReplyData.SetLength(0);
                    iReplyData.FillZ();
                    // wait for new command
                    WaitForCommand();
                    break;
                default:
                    User::Panic(_L("Unknown connection status"), KErrUnknown);
                }
            break;
        case KErrTimedOut:
        case KErrEof:
        case KErrDisconnected:
            iSocket->Close();
            iEngine->CloseConnection();
            break;
        default:
            User::Panic(_L("CMsgCommandConnection - Panic :"), iStatus.Int());
        }
    }
TInt CMsgCommandConnection::ParseCommandL()
    {
    __ASSERT_ALWAYS(
        iSockBuf.Length() < (iCmdData.MaxLength() - iCmdData.Length()),
        User::Leave(KErrOverflow));
    
    if (iSockBuf.Find(KCommandBegin) != KErrNotFound)
        { //This could be actual command
        iCmdData.Append(iSockBuf);
        if ((iCmdData.Find(KEndSend) != KErrNotFound)
            || (iCmdData.Find(KEndReceive) != KErrNotFound))
            {
            // command seems complete
            if (iCmdData.Find(KBeginSend) != KErrNotFound)
                {// send command
                // check send fields
                if (iCmdData.Find(KEndSend) != KErrNotFound &&
                    iCmdData.Find(KBeginAddress) != KErrNotFound && 
                    iCmdData.Find(KEndAddress) != KErrNotFound &&
                    iCmdData.Find(KBeginType) != KErrNotFound &&
                    iCmdData.Find(KEndType) != KErrNotFound &&
                    iCmdData.Find(KBeginTimestamp) != KErrNotFound &&
                    iCmdData.Find(KEndTimestamp) != KErrNotFound &&
                    iCmdData.Find(KBeginPayload) != KErrNotFound &&
                    iCmdData.Find(KEndPayload) != KErrNotFound)
                        iCommand = ESendSms;
                else
                    User::Leave(KErrNotSupported);
                }
            else if (iCmdData.Find(KBeginReceive) != KErrNotFound)
                {//receive command
                iCommand = EReceiveSms;
                }
            else
                {//this is not a valid message command
                User::Leave(KErrNotSupported);
                }      
            }
        else
            {
            // wait command to complete
            User::Leave(KErrNotReady);
            }
        }
    else
        User::Leave(KErrNotSupported);

    return KErrNone;
    }

TInt CMsgCommandConnection::ProcessCommandL()
    {
    //
    //command received and needs to be handled
    //

    iNotify->WriteToConsole(_L("Command received:\n"));
    
    TInt err=KErrNone;

    switch( iCommand )
        {
	    case ESendSms:
		    iNotify->WriteToConsole(_L("\tCommand - Send SMS\n"));
            delete iMsg;
            iMsg = new (ELeave) CMessage();
            iMsg->setMessage(iCmdData);
		    TRAP(err, iNotify->ProcessSendL(*iMsg));
            switch(err)
                {
                case KErrNone:
                    Reply(KStatusOK);
                    break;
                default:
                    Reply(KStatusInternalServerError);
                    break;
                }
		    break;
	    case EReceiveSms:
		    iNotify->WriteToConsole(_L("\tCommand - Receive SMS\n"));
            delete iMsg;
            iMsg = new (ELeave) CMessage();
            TRAP(err, iNotify->ProcessReceiveL(*iMsg));
            switch(err)
                {
                case KErrNone:
                    Reply(*iMsg);
		            break;
                case KErrNotFound:
                    Reply(KStatusNotFound);
                    break;
                default:
                    Reply(KStatusInternalServerError);
                    break;
                }
            break;
    	case EStopServer:
		    Stop();
            break;
	    default:
		    iNotify->WriteToConsole(_L("\tCommand - Unknown\n"));
            Reply(KStatusBadRequest);
		}
   
    return KErrNone;
    }

TInt CMsgCommandConnection::RunError(TInt aError)
    {
	return aError;
    }

void CMsgCommandConnection::Stop()
    {
	DoCancel();
	iEngine->ShutDownServer();
    }

void CMsgCommandConnection::StartConnection()
{
	//iSocket->SetOpt(KSOBlockingIO,KSOLSocket);
	WaitForCommand(); 	
	iTimer->After(TimeOutTime); // Activate timeout wait
}

void CMsgCommandConnection::WaitForCommand()
    {
    iNotify->WriteToConsole(_L("Waiting for command ...\n"));
    iConState=EReceivingCommand;
	iLen = iSockBuf.MaxLength();
    iSockBuf.SetLength(0);
    iSockBuf.FillZ();
    iSocket->RecvOneOrMore(iSockBuf, 0, iStatus, iLen);
    SetActive();
    }

void CMsgCommandConnection::Reply(const TDesC8& aValue)
    {
    iReplyData.FillZ();
    iReplyData = aValue;
    iNotify->WriteToConsole(_L("Sending reply...\n"));
    iConState = ESendReply;
    iSocket->Send(iReplyData,0,iStatus,iLen);
    SetActive();
    }

void CMsgCommandConnection::Reply(CMessage& aMsg)
    {
    iNotify->WriteToConsole(_L("Sending reply...\n"));
    iConState = ESendReply;
    iReplyData.FillZ();
    HBufC8* message = aMsg.getMessage();
    CleanupStack::PushL(message);
    iReplyData = message->Des();
    iSocket->Send(iReplyData,0,iStatus,iLen);
    CleanupStack::PopAndDestroy(); // message
    SetActive();
    }

void CMsgCommandConnection::ConnectionTimeOutL()
    {
    
    Cancel();
	SignalRunL(KErrTimedOut);
    }

void CMsgCommandConnection::SignalRunL(TInt aError)
{
	TRequestStatus* p=&iStatus;
	SetActive();
	User::RequestComplete(p,aError);
}
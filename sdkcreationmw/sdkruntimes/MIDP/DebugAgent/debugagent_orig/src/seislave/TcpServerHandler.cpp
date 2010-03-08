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



#include "TcpServerHandler.h"
#include "jdebug.h"


CTcpServerHandler* CTcpServerHandler::NewL(CDriver* aDriver)
	{
	CTcpServerHandler* object = new (ELeave) CTcpServerHandler(aDriver);
	object->ConstructL();
	CActiveScheduler::Add(object);
	return object;
	}

CTcpServerHandler::CTcpServerHandler(CDriver* aDriver)
:   CActive(CActive::EPriorityStandard),
    iReader(NULL), iWriter(NULL),
    iCommandPtr((TUint8*)(&iCommandCode), 0, 4), 
    iDriver(aDriver), iAuthenticated (EFalse)
	{
	}

CTcpServerHandler::~CTcpServerHandler()
	{
	Cancel();
    if ( iCommand != NULL )
        {
        delete iCommand;
        }
    if ( iReader != NULL )
        {
        delete iReader;
        }
    if ( iWriter != NULL )
        {
        delete iWriter;
        }
	iSocket.Close();
	}

RSocket& CTcpServerHandler::InitSocketL(RSocketServ& aServer)
	{
	iSocket.Open(aServer);
	if ( iReader != NULL )
		{
		delete iReader;
		iReader = NULL;
		}
	iReader = CSocketReader::NewL(&iSocket);
	iWriter = CSocketWriter::NewL(&iSocket);
	return iSocket;
	}
	
RSocket* CTcpServerHandler::Socket()
	{
	return &iSocket;
	}

void CTcpServerHandler::ProcessRequest()
	{
	iState = EInitial;
	iDriver->ClientConnected(this);
	Reschedule();
	}

void CTcpServerHandler::RunL()
	{
	TInt error = KErrNone;
	switch ( iState )
		{
		case EInitial:
            {
			CreateCommandL();
			iState = ECreating;
			break;
            }
		case ECreating:
            {
            if ( iStatus.Int() != KErrNone )
                {
                iErroredState = ECreating;
                iState = EErrored;
                Reschedule();
                }
            else
           		{
           		TInt commandCode = ByteOrder::Swap32(iCommandCode);
#if !( defined(__WINS__) || defined(__WINSCW__) )
     			// are we authenticated?
     			if ( 
     				!iAuthenticated 
     				&& commandCode != KHandshakeCommand 
     				&& iDriver->IsUsePin() )
     				{
     				// not authenticated, send error
	                iErroredState = ECreating;
     				iState = EErrored;
     				Reschedule();
     				break;
     				}
#endif      		
				// Temporarily set state to EProcessing in case if one of 
				// the statements below decides to leave us
				iState = EProcessing;
				// create command
				iCommand = CSeiCommand::CreateCommandL(iDriver, iReader, iWriter, this, commandCode); 
				ReadCommandL();
				iState = EReading;
           		}
			break;
            }
        case EReading:
            {
            if ( iStatus.Int() != KErrNone )
                {
                iProcessingStatus = iStatus.Int();
                iErroredState = EReading;                
                iState = EErrored;
                Reschedule();
                }
            else
                {
			    iState = EProcessing;
			    ProcessCommandL();
                }
            break;
            }
		case EProcessing:
            {
            iProcessingStatus = iStatus.Int();
			WriteResponseL();
        	iState = EWriting;
			break;
            }
		case EWriting:
            {
            if ( iStatus.Int() != KErrNone )
                {
                iErroredState = EWriting;
                iState = EErrored;
                }
            else
                {
    			iState = EComplete;
                }
			Reschedule();
			break;
            }
        case EComplete:
            {
			iState = EInitial;
            if(iCommand->isFinal())
            	{
            	_LIT(KDisconnect, "Client disconnecting");	
    			iDriver->Log(KDisconnect);            		
    			iDriver->ClientDisconnected(this);
            	}
            else
            	{
	            Reschedule();           	
            	}
			break;
            }
		case EErrored:
            {
	    	_LIT(KRunL, "@CActive::RunL");	
           	ProcessError(error, KRunL);            	
			break;
            }
		}
	}

TInt CTcpServerHandler::RunError(TInt aError)
	{
	// see how to handle this
    iErroredState = iState;
    iState = EErrored;
    _LIT(KRunError, "@CActive::RunError");	
    ProcessError(aError, KRunError);
	return KErrNone;
	}

void CTcpServerHandler::ProcessError(TInt /*aError*/, const TDesC& /*aDesc*/)
    {
	_LIT(KReadError, "Socket read error");
	_LIT(KWriteError, "Socket write error");
	_LIT(KProcessError, "Protocol error");
	_LIT(KOtherError, "Internal error");
	const TDesC* errMsg;
	switch (iErroredState)
		{
		case ECreating:
		case EReading:
			errMsg = &KReadError();
			break;
		case EProcessing:
			errMsg = &KProcessError();
			break;
		case EWriting:
			errMsg = &KWriteError();
			break;
		default:
			errMsg = &KOtherError();
			break;
		}
	DEBUG_STR("*CTcpServerHandler::ProcessError %S", *errMsg);
	iDriver->Log(*errMsg);
	iDriver->ClientDisconnected(this);
	//iDriver->Engine()->Cleanup();
    }

void CTcpServerHandler::Reschedule()
	{
    iStatus = KRequestPending;
	TRequestStatus* ptrStat = &iStatus;
	SetActive();
	User::RequestComplete(ptrStat,KErrNone);
	}

void CTcpServerHandler::DoCancel()
	{
	if ( iCommand != NULL )
		{
		iCommand->Cancel();
		}
	else if ( iState == ECreating )
		{
		iSocket.CancelRead();
		}
	_LIT(KDisconnected, "Client disconnected");
	iDriver->Log(KDisconnected);            		
	}

void CTcpServerHandler::ConstructL()
	{
	}

void CTcpServerHandler::CreateCommandL(void)
	{
    if ( iCommand != NULL )
        {
        delete iCommand;
        iCommand = NULL;
        }
	// async read command code
	iSocket.Read(iCommandPtr, iStatus);
	SetActive();
	}

void CTcpServerHandler::ReadCommandL(void)
	{
	iCommand->ReadL(iStatus); 
    SetActive();
	}

void CTcpServerHandler::ProcessCommandL(void)
	{
	iCommand->ExecuteL(iStatus);
    SetActive();
	}

void CTcpServerHandler::WriteResponseL(void)
	{
	iCommand->WriteL(iStatus);
    SetActive();
	}


TBool CTcpServerHandler::IsAuthenticated()
	{
	return iAuthenticated;
	}

void CTcpServerHandler::SetAuthenticated(TBool aAuthenticated)
	{
	iAuthenticated = aAuthenticated;
	}


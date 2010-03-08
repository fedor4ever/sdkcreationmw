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


#include "SeiCommands.h"
	
	
CSeiCommand* CSeiCommand::CreateCommandL(
        CDriver* aDriver, 
        CSocketReader* aReader,
        CSocketWriter* aWriter,
        CTcpServerHandler* aHandler,
        TInt aCommandCode)
	{
    CSeiCommand* command = NULL;
    switch( aCommandCode )
        {
        case KInstallCommand:
            {
            command = CInstallCommand::NewL(aDriver, aReader, aWriter);
            break;
            }

        case KRemoveCommand:
            {
            command = CRemoveCommand::NewL(aDriver, aReader, aWriter);
            break;
            }

        case KStartCommand:
            {
            command = CStartCommand::NewL(aDriver, aReader, aWriter);
            break;
            }

        case KCloseCommand:
            {
            command = CCloseCommand::NewL(aDriver, aReader, aWriter);
            break;
            }

        case KListCommand:
            {
            command = CListCommand::NewL(aDriver, aReader, aWriter);
            break;
            }

        case KGetInfoCommand:
            {
            command = CGetInfoCommand::NewL(aDriver, aReader, aWriter);
            break;
            }

        case KHandshakeCommand:
            {
            command = CHandshakeCommand::NewL(aDriver, aReader, aWriter, aHandler);
            break;
            }


        case KTestCommand:
            {
            command = CTestCommand::NewL(aDriver, aReader, aWriter);
            break;
            }

        case KQuitCommand:
            {
            command = CQuitCommand::NewL(aDriver, aReader, aWriter);
            break;
            }

        case KGetPortCommand:
            {
            command = CGetPortCommand::NewL(aDriver, aReader, aWriter);
            break;
            }
        default:
            {
            User::Leave(KErrNotFound);
            }
        }
    return command;
	}



CSeiCommand::CSeiCommand(
        CDriver* aDriver, 
        CSocketReader* aReader,
        CSocketWriter* aWriter)
    : CActive(EPriorityNormal),
    iDriver(aDriver),
    iReader(aReader),
    iWriter(aWriter),
    iOperation(ENone)
    {
    }

CSeiCommand::~CSeiCommand()
    {
    Cancel();
    }


// CActive
void CSeiCommand::RunL()
    {
    // just checking cleanupstack
    switch ( iOperation )
        {
        case ERead:
            {
            DoReadL();
            Complete(KErrNone);
            break;
            }
        case EExecute:
            {
            DoExecuteL();
            Complete(KErrNone);
            break;
            }
        case EWrite:
            {
            DoWriteL();
            Complete(KErrNone);
            iOperation = EDone;
            Deque();
            break;
            }
        default:
            {
            // this should never happen
            }
        }
    }

TInt CSeiCommand::RunError(TInt aError)
    {
    Complete(aError);
    iOperation = EError; // prevent any further requests
    Deque();
    return KErrNone;
    }

void CSeiCommand::DoCancel()
    {
    Complete(KErrCancel);
    }


// submit Read request 
void CSeiCommand::ReadL(TRequestStatus& aStatus)
    {
    if ( iOperation != ENone )
        {
        User::Leave(KErrNotSupported);
        }
	CActiveScheduler::Add(this);
    iOperation = ERead;
    aStatus = KRequestPending;
    iRequestStatus = &aStatus;
    Reschedule();
    }

// submit Execute request 
void CSeiCommand::ExecuteL(TRequestStatus& aStatus)
    {
    if ( iOperation != ERead )
        {
        User::Leave(KErrNotSupported);
        }
    iOperation = EExecute;
    aStatus = KRequestPending;
    iRequestStatus = &aStatus;
    Reschedule();
    }

// submit Write request 
void CSeiCommand::WriteL(TRequestStatus& aStatus)
    {
    if ( iOperation != EExecute )
        {
        User::Leave(KErrNotSupported);
        }
    iOperation = EWrite;
    aStatus = KRequestPending;
    iRequestStatus = &aStatus;
    Reschedule();
    }
    
    
TBool CSeiCommand::isFinal()
	{
	return EFalse;
	}


// Active object utils
void CSeiCommand::Reschedule()
	{
    iStatus = KRequestPending;
	TRequestStatus* ptrStat = &iStatus;
	User::RequestComplete(ptrStat,KErrNone);
	SetActive();
	}


void CSeiCommand::Complete(TInt aResult)
    {
    if ( iOperation > ENone && iOperation < EDone && iRequestStatus != NULL )
        {
    	User::RequestComplete(iRequestStatus,aResult);
        }
    }

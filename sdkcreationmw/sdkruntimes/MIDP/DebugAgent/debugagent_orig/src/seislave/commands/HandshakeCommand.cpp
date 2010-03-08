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
#include "TcpServerHandler.h"


CHandshakeCommand* CHandshakeCommand::NewL(
    CDriver* aDriver, 
    CSocketReader* aReader,
    CSocketWriter* aWriter,
    CTcpServerHandler* aHandler)
    {
    CHandshakeCommand* object = new (ELeave) CHandshakeCommand(aDriver, aReader, aWriter, aHandler);
    CleanupStack::PushL(object);
    object->ConstructL();
    CleanupStack::Pop(object);
    return object;
    }

CHandshakeCommand::CHandshakeCommand(
    CDriver* aDriver, 
    CSocketReader* aReader,
    CSocketWriter* aWriter,
    CTcpServerHandler* aHandler)
    : 	CSeiCommand(aDriver, aReader, aWriter), 
    	iPinCode(NULL), iHandler(aHandler)
    {
    }

CHandshakeCommand::~CHandshakeCommand()
    {
    if ( iPinCode != NULL )
    	{
    	delete iPinCode;
    	iPinCode = NULL;
    	}
    }

void CHandshakeCommand::ConstructL()
    {
    }

void CHandshakeCommand::DoReadL()
    {
	iDriver->Log(_L("Authenticating"));
	iReader->ReadUtfL(iPinCode);
    }

void CHandshakeCommand::DoExecuteL()
    {
    if ( iDriver->IsUsePin() )
    	{
    	HBufC* pin = iDriver->GetPinCode();
    	iResult = pin->Compare(*iPinCode);
    	}
    else 
    	{
    	iResult = KErrNone;	
    	}
    iHandler->SetAuthenticated(iResult==KErrNone);
    }

void CHandshakeCommand::DoWriteL()
    {
    if ( iResult != KErrNone )
    	{
		iDriver->Log(_L("Authentication failed."));
    	}
    else
    	{
		iDriver->Log(_L("Authentication ok."));
    	}
    iWriter->WriteIntL(iResult);
    }

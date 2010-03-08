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

CCloseCommand* CCloseCommand::NewL(
    CDriver* aDriver, 
    CSocketReader* aReader,
    CSocketWriter* aWriter)
    {
    CCloseCommand* object = new (ELeave) CCloseCommand(aDriver, aReader, aWriter);
    CleanupStack::PushL(object);
    object->ConstructL();
    CleanupStack::Pop(object);
    return object;
    }

CCloseCommand::CCloseCommand(
    CDriver* aDriver, 
    CSocketReader* aReader,
    CSocketWriter* aWriter)
    : CSeiCommand(aDriver, aReader, aWriter)
    {
    }

CCloseCommand::~CCloseCommand()
    {
    }

void CCloseCommand::ConstructL()
    {
    }

void CCloseCommand::DoReadL()
    {
	iDriver->Log(_L("Reading close command"));
	//on EKA2 and PlatSec, Terminating a VM requires PowerMngmt capabilities.
	//=> Closing the MIDlet which should also terminate the VM.
    TUint8 terminate = iReader->ReadL();
    iMsId = iReader->ReadIntL();
    if ( terminate == 0 )
        {
		iDriver->Log(_L("Close MIDlet request (Terminate VM not specified)"));
        }
    else
        {
		iDriver->Log(_L("Close MIDlet request (Terminate VM)"));
        }
	iMId = iReader->ReadIntL();
    }

void CCloseCommand::DoExecuteL()
    {
    iDriver->Engine()->CloseL(iMsId, iMId);
    }

void CCloseCommand::DoWriteL()
    {
	iDriver->Log(_L("Closing MIDlet request complete"));
    iWriter->WriteIntL(KErrNone);
    iWriter->WriteL((TUint8) 0);
    }

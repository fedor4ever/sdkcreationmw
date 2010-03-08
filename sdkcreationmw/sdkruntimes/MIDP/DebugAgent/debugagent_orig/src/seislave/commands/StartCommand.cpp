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

CStartCommand* CStartCommand::NewL(
    CDriver* aDriver, 
    CSocketReader* aReader,
    CSocketWriter* aWriter)
    {
    CStartCommand* object = new (ELeave) CStartCommand(aDriver, aReader, aWriter);
    CleanupStack::PushL(object);
    object->ConstructL();
    CleanupStack::Pop(object);
    return object;
    }

CStartCommand::CStartCommand(
    CDriver* aDriver, 
    CSocketReader* aReader,
    CSocketWriter* aWriter)
    : CSeiCommand(aDriver, aReader, aWriter)
    {
    }

CStartCommand::~CStartCommand()
    {
    if ( iCommandLineOptions != NULL )
        {
        delete iCommandLineOptions;
        }
    }

void CStartCommand::ConstructL()
    {
    // TODO
    }

void CStartCommand::DoReadL()
    {
	iDriver->Log(_L("Reading Start MIDlet request"));
    iMsId = iReader->ReadIntL();
    iMId = iReader->ReadIntL();
    iReader->ReadUtfL(iCommandLineOptions);
    TUint8 tmpDefer = iReader->ReadL();
    if ( tmpDefer == 0 )
        {
        iDefer = EFalse;
        }
    else 
        {
		iDriver->Log(_L("Deferred start requested"));
        iDefer = ETrue;
        }
    }

void CStartCommand::DoExecuteL()
    {
	iDriver->Log(_L("Starting MIDlet"));
    iDriver->Engine()->StartL(iMsId, iMId, iCommandLineOptions, iDefer);
	iDriver->Log(_L("Please wait..."));
    }

void CStartCommand::DoWriteL()
    {
    iWriter->WriteIntL(KErrNone);
    }

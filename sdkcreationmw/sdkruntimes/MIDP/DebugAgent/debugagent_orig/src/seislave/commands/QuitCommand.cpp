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

CQuitCommand* CQuitCommand::NewL(
    CDriver* aDriver, 
    CSocketReader* aReader,
    CSocketWriter* aWriter)
    {
    CQuitCommand* object = new (ELeave) CQuitCommand(aDriver, aReader, aWriter);
    CleanupStack::PushL(object);
    object->ConstructL();
    CleanupStack::PopAndDestroy(object);
    return object;
    }

CQuitCommand::CQuitCommand(
    CDriver* aDriver, 
    CSocketReader* aReader,
    CSocketWriter* aWriter)
    : CSeiCommand(aDriver, aReader, aWriter)
    {
    }

CQuitCommand::~CQuitCommand()
    {
    }

void CQuitCommand::ConstructL()
    {
    }

void CQuitCommand::DoReadL()
    {
	iDriver->Log(_L("Quit agent request"));
    }

void CQuitCommand::DoExecuteL()
    {
	iDriver->Log(_L("Shutting down"));
    iDriver->Shutdown();
    }

void CQuitCommand::DoWriteL()
    {
    iWriter->WriteIntL(KErrNone);
    }

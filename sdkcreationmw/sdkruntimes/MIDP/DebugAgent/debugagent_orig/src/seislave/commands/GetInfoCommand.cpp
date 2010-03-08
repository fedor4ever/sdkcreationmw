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

CGetInfoCommand* CGetInfoCommand::NewL(
    CDriver* aDriver, 
    CSocketReader* aReader,
    CSocketWriter* aWriter)
    {
    CGetInfoCommand* object = new (ELeave) CGetInfoCommand(aDriver, aReader, aWriter);
    CleanupStack::PushL(object);
    object->ConstructL();
    CleanupStack::Pop(object);
    return object;
    }

CGetInfoCommand::CGetInfoCommand(
    CDriver* aDriver, 
    CSocketReader* aReader,
    CSocketWriter* aWriter)
    : CSeiCommand(aDriver, aReader, aWriter)
    {
    }

CGetInfoCommand::~CGetInfoCommand()
    {
    delete iInfo;
    }

void CGetInfoCommand::ConstructL()
    {
    }

void CGetInfoCommand::DoReadL()
    {
	iDriver->Log(_L("Get MIDlet info request"));
    iMsId = iReader->ReadIntL();
    }

void CGetInfoCommand::DoExecuteL()
    {
	iDriver->Log(_L("Loading MIDlet info..."));
    iInfo = iDriver->Engine()->GetSuiteInfoL(iMsId);
    }

void CGetInfoCommand::DoWriteL()
    {
    iWriter->WriteIntL(KGetInfoCommand);
    iWriter->WriteUtf8L(*iInfo);
	iDriver->Log(_L("Get MIDlet info request complete"));
    }

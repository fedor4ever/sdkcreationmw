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

CListCommand* CListCommand::NewL(
    CDriver* aDriver, 
    CSocketReader* aReader,
    CSocketWriter* aWriter)
    {
    CListCommand* object = new (ELeave) CListCommand(aDriver, aReader, aWriter);
    CleanupStack::PushL(object);
    object->ConstructL();
    CleanupStack::Pop(object);
    return object;
    }

CListCommand::CListCommand(
    CDriver* aDriver, 
    CSocketReader* aReader,
    CSocketWriter* aWriter)
    : CSeiCommand(aDriver, aReader, aWriter)
    {
    }

CListCommand::~CListCommand()
    {
    iResult.Reset();
    iResult.Close();
    }

void CListCommand::ConstructL()
    {
    }

void CListCommand::DoReadL()
    {
	iDriver->Log(_L("List MIDlet suites request"));
    // Nothing to read
    }

void CListCommand::DoExecuteL()
    {
	iDriver->Log(_L("Obtaining a list of MIDlet suites"));
    iDriver->Engine()->ListSuiteIDsL(iResult);
    }

void CListCommand::DoWriteL()
    {
    iWriter->WriteIntL(KErrNone);
    iWriter->WriteIntL(iResult.Count());
    for( TInt i = 0 ; i < iResult.Count() ; i++ )
        {
        iWriter->WriteIntL(iResult[i]);
        }
	iDriver->Log(_L("List MIDlet suites request complete"));
    }

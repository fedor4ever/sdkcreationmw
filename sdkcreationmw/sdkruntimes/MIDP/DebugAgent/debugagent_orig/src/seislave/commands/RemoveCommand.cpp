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

CRemoveCommand* CRemoveCommand::NewL(
    CDriver* aDriver, 
    CSocketReader* aReader,
    CSocketWriter* aWriter)
    {
    CRemoveCommand* object = new (ELeave) CRemoveCommand(aDriver, aReader, aWriter);
    CleanupStack::PushL(object);
    object->ConstructL();
    CleanupStack::Pop(object);
    return object;
    }

CRemoveCommand::CRemoveCommand(
    CDriver* aDriver, 
    CSocketReader* aReader,
    CSocketWriter* aWriter)
    : CSeiCommand(aDriver, aReader, aWriter)
    {
    }

CRemoveCommand::~CRemoveCommand()
    {
    if ( iName != NULL )
        {
        delete iName;
        }
    if ( iVendor != NULL )
        {
        delete iVendor;
        }
    }

void CRemoveCommand::ConstructL()
    {
    }

void CRemoveCommand::DoReadL()
    {
	iDriver->Log(_L("Reading remove MIDlet request"));
    TUint8 tmpFinal = iReader->ReadL();
    if (tmpFinal == 0)
        {
        iFinal = EFalse;
        }
    else 
        {
        iFinal = ETrue;
        }
    iReader->ReadUtfL(iName);
    iReader->ReadUtfL(iVendor);
    }

void CRemoveCommand::DoExecuteL()
    {
	iDriver->Log(_L("Removing MIDlet..."));
    iDriver->Engine()->RemoveL(iName, iVendor);
    }

void CRemoveCommand::DoWriteL()
    {
	iDriver->Log(_L("Remove MIDlet request complete"));
    iWriter->WriteIntL(KErrNone);
    }
    
TBool CRemoveCommand::isFinal()
	{
	return iFinal;
	}    

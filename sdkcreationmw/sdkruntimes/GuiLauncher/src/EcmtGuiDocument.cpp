/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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


#include "EcmtGuiApplication.h"
#include "EcmtGuiAppUi.h"
#include "EcmtGuiDocument.h"

CEcmtGuiDocument* CEcmtGuiDocument::NewL(CEikApplication& aApp, TUid aUid)
    {
    CEcmtGuiDocument* self = NewLC(aApp, aUid);
    CleanupStack::Pop(self);
    return self;
    }

CEcmtGuiDocument* CEcmtGuiDocument::NewLC(CEikApplication& aApp, TUid aUid)
    {
    CEcmtGuiDocument* self = new (ELeave) CEcmtGuiDocument(aApp, aUid);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CEcmtGuiDocument::ConstructL()
    {
	// No implementation required
    }    

CEcmtGuiDocument::CEcmtGuiDocument(CEikApplication& aApp, TUid aUid) : CAknDocument(aApp), iUid( aUid ) 
    {
	// No implementation required
    }

CEcmtGuiDocument::~CEcmtGuiDocument()
    {
	// No implementation required
    }

TUid CEcmtGuiDocument::GetUid()
    {
        return iUid;
    }

CEikAppUi* CEcmtGuiDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it;
    // the framework takes ownership of this object
	return (static_cast <CEikAppUi*> (new (ELeave) CEcmtGuiAppUi)); 
    }


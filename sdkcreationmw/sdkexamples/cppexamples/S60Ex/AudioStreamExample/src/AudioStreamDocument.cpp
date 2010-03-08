/*
* Copyright (c) 2006 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDE FILES
#include "AudioStreamDocument.h"
#include "AudioStreamAppUi.h"


CAudioStreamDocument* CAudioStreamDocument::NewL(
        CEikApplication& aApp)     // CAudioStreamApp reference
    {
    CAudioStreamDocument* self = new (ELeave) CAudioStreamDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// CAudioStreamDocument::CAudioStreamDocument(CEikApplication& aApp)
//
// constructor
// ----------------------------------------------------------------------------
CAudioStreamDocument::CAudioStreamDocument(CEikApplication& aApp):
	CAknDocument(aApp)    
    {
    }

// ----------------------------------------------------------------------------
// CAudioStreamDocument::~CAudioStreamDocument()
//
// destructor
// ----------------------------------------------------------------------------
CAudioStreamDocument::~CAudioStreamDocument()
    {
    }

// ----------------------------------------------------------------------------
// CAudioStreamDocument::ConstructL()
//
// Standard EPOC 2nd phase constructor
// ----------------------------------------------------------------------------
void CAudioStreamDocument::ConstructL()
    {
    }

    
// ----------------------------------------------------------------------------
// CAudioStreamDocument::CreateAppUiL()
// constructs CAudioStreamAppUi
// ----------------------------------------------------------------------------
CEikAppUi* CAudioStreamDocument::CreateAppUiL()
    {
    return new (ELeave) CAudioStreamAppUi;
    }

// End of File  


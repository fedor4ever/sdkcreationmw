/*
* Copyright (c) 2004 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include "HelpExampleAppUi.h"
#include "HelpExampleDocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHelpExampleDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHelpExampleDocument* CHelpExampleDocument::NewL( CEikApplication& aApp )
    {
    CHelpExampleDocument* self = NewLC( aApp );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CHelpExampleDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHelpExampleDocument* CHelpExampleDocument::NewLC( CEikApplication& aApp )
    {
    CHelpExampleDocument* self = new ( ELeave ) CHelpExampleDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CHelpExampleDocument::ConstructL()
// 2nd phase construction
// ----------------------------------------------------------------------------
//
void CHelpExampleDocument::ConstructL()
    {
    // No implementation required
    }

// ----------------------------------------------------------------------------
// CHelpExampleDocument::CHelpExampleDocument()
// Constructor.
// ----------------------------------------------------------------------------
//
CHelpExampleDocument::CHelpExampleDocument( CEikApplication& aApp ):
    CAknDocument( aApp ) 
    {
    // No implementation required
    }

// ----------------------------------------------------------------------------
// CHelpExampleDocument::~CHelpExampleDocument()
// Destructor.
// ----------------------------------------------------------------------------
//
CHelpExampleDocument::~CHelpExampleDocument()
    {
    // No implementation required
    }

// ----------------------------------------------------------------------------
// CHelpExampleDocument::CreateAppUiL()
// Create the application user interface, and return a pointer to it;
// the framework takes ownership of this object
// ----------------------------------------------------------------------------
//
CEikAppUi* CHelpExampleDocument::CreateAppUiL()
    {
    return ( static_cast<CEikAppUi*> ( new ( ELeave ) CHelpExampleAppUi ) );
    }

// End of File

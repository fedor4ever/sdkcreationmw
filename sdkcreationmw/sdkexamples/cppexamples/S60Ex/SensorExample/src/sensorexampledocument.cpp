/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include "sensorexampleappui.h"
#include "sensorexampledocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSensorExampleDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSensorExampleDocument* CSensorExampleDocument::NewL( CEikApplication& aApp )
    {
    CSensorExampleDocument* self = NewLC( aApp );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CSensorExampleDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSensorExampleDocument* CSensorExampleDocument::NewLC( CEikApplication& aApp )
    {
    CSensorExampleDocument* self = new ( ELeave ) CSensorExampleDocument( aApp );

    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CSensorExampleDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSensorExampleDocument::ConstructL()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CSensorExampleDocument::CSensorExampleDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CSensorExampleDocument::CSensorExampleDocument( CEikApplication& aApp )
    : CAknDocument( aApp )
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CSensorExampleDocument::~CSensorExampleDocument()
// Destructor.
// ---------------------------------------------------------------------------
//
CSensorExampleDocument::~CSensorExampleDocument()
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CSensorExampleDocument::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CSensorExampleDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it.
    // The framework takes ownership of this object
    return ( static_cast <CEikAppUi*> ( new ( ELeave ) CSensorExampleAppUi ) );
    }

// End of File

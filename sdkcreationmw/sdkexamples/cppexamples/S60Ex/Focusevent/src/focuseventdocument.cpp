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
#include "FocusEventAppUi.h"
#include "FocusEventDocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CFocusEventDocument::CFocusEventDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CFocusEventDocument::CFocusEventDocument( CEikApplication& aApp )
    : CAknDocument( aApp ) 
    {
    // No implementation required
    } 

// -----------------------------------------------------------------------------
// CFocusEventDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFocusEventDocument* CFocusEventDocument::NewL( CEikApplication& aApp )
    {
    CFocusEventDocument* self = NewLC( aApp );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CFocusEventDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFocusEventDocument* CFocusEventDocument::NewLC( CEikApplication& aApp )
    {
    CFocusEventDocument* self = new ( ELeave ) CFocusEventDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CFocusEventDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CFocusEventDocument::ConstructL()
    {
    // No implementation required
    }    

// -----------------------------------------------------------------------------
// CFocusEventDocument::~CFocusEventDocument()
// Destructor.
// -----------------------------------------------------------------------------
//
CFocusEventDocument::~CFocusEventDocument()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CFocusEventDocument::CreateAppUiL()
// Constructs CreateAppUi.
// -----------------------------------------------------------------------------
//
CEikAppUi* CFocusEventDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it
    return ( static_cast<CEikAppUi*>( new ( ELeave ) CFocusEventAppUi ) );
    }

// End of File

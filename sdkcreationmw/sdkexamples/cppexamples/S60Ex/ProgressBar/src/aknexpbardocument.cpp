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
#include "AknExPbarDocument.h"
#include "AknExPbarAppUi.h"


// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CAknExPbarDocument::CAknExPbarDocument()
// C++ default constructor can NOT contain any code, that might leave. 
// -----------------------------------------------------------------------------
//
CAknExPbarDocument::CAknExPbarDocument( CEikApplication& aApp )
: CEikDocument( aApp ) 
    {
    // No implementation required
    }


// -----------------------------------------------------------------------------
// CAknExPbarDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAknExPbarDocument::ConstructL()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CAknExPbarDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAknExPbarDocument* CAknExPbarDocument::NewL( CEikApplication& aApp )
    {
    CAknExPbarDocument* self = NewLC( aApp );
    CleanupStack::Pop( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// CAknExPbarDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAknExPbarDocument* CAknExPbarDocument::NewLC( CEikApplication& aApp )
    {
    CAknExPbarDocument* self = new ( ELeave ) CAknExPbarDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CAknExPbarDocument::~CAknExPbarDocument()
// Destructor.
// -----------------------------------------------------------------------------
//
CAknExPbarDocument::~CAknExPbarDocument()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CAknExPbarDocument::CreateAppUiL()
// Create the application user interface, and return a pointer to it
// -----------------------------------------------------------------------------
//
CEikAppUi* CAknExPbarDocument::CreateAppUiL()
    {
    return new( ELeave ) CAknExPbarAppUi;
    }

// End of File  

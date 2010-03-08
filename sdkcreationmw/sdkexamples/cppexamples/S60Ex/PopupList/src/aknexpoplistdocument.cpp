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
#include "AknExPopListDocument.h"
#include "AknExPopListAppUi.h"

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CAknExPopListDocument::CAknExPopListDocument()
// C++ default constructor can NOT contain any code, that might leave. 
// -----------------------------------------------------------------------------
//
CAknExPopListDocument::CAknExPopListDocument( CEikApplication& aApp ):
                                              CEikDocument( aApp )

    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CAknExPopListDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAknExPopListDocument* CAknExPopListDocument::NewL( CEikApplication& aApp )  
    {
    CAknExPopListDocument* self = NewLC( aApp );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CAknExPopListDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAknExPopListDocument* CAknExPopListDocument::NewLC( CEikApplication& aApp )  
    {

    CAknExPopListDocument* self = new ( ELeave ) CAknExPopListDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// -----------------------------------------------------------------------------
// CAknExPopListDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAknExPopListDocument::ConstructL()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CAknExPopListDocument::~CAknExPopListDocument()
// Destructor.
// -----------------------------------------------------------------------------
//
CAknExPopListDocument::~CAknExPopListDocument()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CAknExPopListDocument::CreateAppUiL()
// Create the application user interface, and return a pointer to it
// -----------------------------------------------------------------------------
//
CEikAppUi* CAknExPopListDocument::CreateAppUiL()
    {
    return new( ELeave ) CAknExPopListAppUi;
    }

// End of File  

/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include "CSSyncAppUi.h"
#include "CSSyncDocument.h"

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CCSSyncDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCSSyncDocument* CCSSyncDocument::NewL( CEikApplication& aApp )
    {
    CCSSyncDocument* self = NewLC( aApp );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCSSyncDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCSSyncDocument* CCSSyncDocument::NewLC( CEikApplication& aApp )
    {
    CCSSyncDocument* self = new ( ELeave ) CCSSyncDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CCSSyncDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCSSyncDocument::ConstructL()
    {
    User::LeaveIfError( iServer.Connect() );
    }

// -----------------------------------------------------------------------------
// CCSSyncDocument::CCSSyncDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CCSSyncDocument::CCSSyncDocument( CEikApplication& aApp )
: CAknDocument( aApp )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CCSSyncDocument::~CCSSyncDocument()
// Destructor.
// -----------------------------------------------------------------------------
//
CCSSyncDocument::~CCSSyncDocument()
    {
    iServer.Close();
    }

// -----------------------------------------------------------------------------
// CCSSyncDocument::CreateAppUiL()
// Creates a CCSAsyncAppUi object and return a pointer to it.
// -----------------------------------------------------------------------------
//
CEikAppUi* CCSSyncDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it
    return ( static_cast<CEikAppUi*>( new ( ELeave ) CCSSyncAppUi() ) );
    }

// -----------------------------------------------------------------------------
// CCSSyncDocument::UpdateTime()
// Asks the time server to update this documents locally stored time.
// -----------------------------------------------------------------------------
//
void CCSSyncDocument::UpdateTime()
    {
    iServer.RequestTime( iTime );
    }

// -----------------------------------------------------------------------------
// CCSSyncDocument::Time()
// Returns the currently stored time.
// -----------------------------------------------------------------------------
//
const TTime& CCSSyncDocument::Time() const
    {
    return iTime;
    }


// End of File

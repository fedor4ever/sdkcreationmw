/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include "CSAsyncAppUi.h"
#include "CSAsyncDocument.h"
#include "CSAsyncRequestHandler.h"

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CCSAsyncDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCSAsyncDocument* CCSAsyncDocument::NewL( CEikApplication& aApp )
    {
    CCSAsyncDocument* self = NewLC( aApp );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CCSAsyncDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCSAsyncDocument* CCSAsyncDocument::NewLC( CEikApplication& aApp )
    {
    CCSAsyncDocument* self = new ( ELeave ) CCSAsyncDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CCSAsyncDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCSAsyncDocument::ConstructL()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CCSAsyncDocument::CCSAsyncDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
CCSAsyncDocument::CCSAsyncDocument( CEikApplication& aApp )
: CAknDocument( aApp )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CCSAsyncDocument::~CCSAsyncDocument()
// Destructor.
// -----------------------------------------------------------------------------
//
CCSAsyncDocument::~CCSAsyncDocument()
    {
    delete iHandler;
    }

// -----------------------------------------------------------------------------
// CCSAsyncDocument::CreateAppUiL()
// Creates a CCSAsyncAppUi object and return a pointer to it.
// -----------------------------------------------------------------------------
//
CEikAppUi* CCSAsyncDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it
    CCSAsyncAppUi* appUi = new ( ELeave ) CCSAsyncAppUi;
    CleanupStack::PushL( appUi );
    iHandler = CCSAsyncRequestHandler::NewL( *appUi );
    CleanupStack::Pop( appUi ) ;
    return appUi;
    }

// -----------------------------------------------------------------------------
// CCSAsyncDocument::UpdateTime()
// Asks the time server to update this documents locally stored time.
// -----------------------------------------------------------------------------
//
void CCSAsyncDocument::UpdateTime()
    {
    iHandler->RequestTime();
    }

// -----------------------------------------------------------------------------
// CCSAsyncDocument::StopClock()
// Stops the clock.
// -----------------------------------------------------------------------------
//
void CCSAsyncDocument::StopClock()
    {
    if ( ClockActive() )
        {
        iHandler->CancelRequest() ;
        }
    }

// -----------------------------------------------------------------------------
// CCSAsyncDocument::Time()
// Returns the currently stored time.
// -----------------------------------------------------------------------------
//
TTime CCSAsyncDocument::Time() const
    {
    return iHandler->Time();
    }

// -----------------------------------------------------------------------------
// CCSAsyncDocument::ClockActive()
// Checks whether the clock has been started.
// -----------------------------------------------------------------------------
//
TBool CCSAsyncDocument::ClockActive() const
    {
    return iHandler->IsActive();
    }

// End of File


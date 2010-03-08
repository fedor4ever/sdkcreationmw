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
#include "HelpExampleAppView.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHelpExampleAppView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHelpExampleAppView* CHelpExampleAppView::NewL( const TRect& aRect )
    {
    CHelpExampleAppView* self = NewLC( aRect );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CHelpExampleAppView::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHelpExampleAppView* CHelpExampleAppView::NewLC( const TRect& aRect )
    {
    CHelpExampleAppView* self = new ( ELeave ) CHelpExampleAppView;
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }

// -----------------------------------------------------------------------------
// CHelpExampleAppView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHelpExampleAppView::ConstructL( const TRect& aRect )
    {
    CreateWindowL();

    SetRect ( aRect );

    ActivateL();
    }

// -----------------------------------------------------------------------------
// CHelpExampleAppView::CHelpExampleAppView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CHelpExampleAppView::CHelpExampleAppView()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CHelpExampleAppView::~CHelpExampleAppView()
// Destructor.
// -----------------------------------------------------------------------------
//
CHelpExampleAppView::~CHelpExampleAppView()
    {
    // No implementation required
    }
// ----------------------------------------------------------------------------
// CHelpExampleAppView::Draw()
// Draw this application's view to the screen
// ----------------------------------------------------------------------------
//
void CHelpExampleAppView::Draw( const TRect& /*aRect*/ ) const
    {
    // Clear the screen
    CWindowGc& gc = SystemGc();
    gc.Clear ( Rect() );
    }


// End of File
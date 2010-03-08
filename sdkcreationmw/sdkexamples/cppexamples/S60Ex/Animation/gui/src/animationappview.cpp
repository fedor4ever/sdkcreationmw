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
#include <coemain.h>
#include <eikenv.h>

#include "AnimationAppView.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAnimationAppView::CAnimationAppView()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CAnimationAppView::CAnimationAppView()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CAnimationAppView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAnimationAppView* CAnimationAppView::NewL( const TRect& aRect )
    {
    CAnimationAppView* self = CAnimationAppView::NewLC( aRect );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CAnimationAppView::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAnimationAppView* CAnimationAppView::NewLC( const TRect& aRect )
    {
    CAnimationAppView* self = new ( ELeave ) CAnimationAppView();
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }

// -----------------------------------------------------------------------------
// CAnimationAppView::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAnimationAppView::ConstructL( const TRect& aRect )
    {
    // Create a window for this application view
    CreateWindowL();

    // Set the windows size
    SetRect( aRect );

    // Activate the window, which makes it ready to be drawn
    ActivateL();
    }

void CAnimationAppView::HandleResolutionChange(TRect aRect)
    {
    SetRect( aRect );
    }

// -----------------------------------------------------------------------------
// CAnimationAppView::~CAnimationAppView()
// Destructor.
// -----------------------------------------------------------------------------
//
CAnimationAppView::~CAnimationAppView()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CAnimationAppView::Draw()
// Draws the display.
// -----------------------------------------------------------------------------
//
void CAnimationAppView::Draw( const TRect& /*aRect*/ ) const
    {
    // Clear the screen
    CWindowGc& gc = SystemGc();
    TRect drawRect( Rect() );
    gc.Clear( drawRect );
    }

// End of File

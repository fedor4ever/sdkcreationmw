/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include "MessagingContainer.h"


// ================= MEMBER FUNCTIONS ==========================================

// -----------------------------------------------------------------------------
// CFilelistContainer::NewL()
// Symbian two-phased constructor.
// -----------------------------------------------------------------------------
//
CMessagingContainer* CMessagingContainer::NewL( const TRect& aRect )
    {
    CMessagingContainer* self = CMessagingContainer::NewLC( aRect );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CFilelistContainer::NewLC()
// Symbian two-phased constructor.
// -----------------------------------------------------------------------------
//
CMessagingContainer* CMessagingContainer::NewLC( const TRect& aRect )
    {
    CMessagingContainer* self = new ( ELeave ) CMessagingContainer();
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }

// -----------------------------------------------------------------------------
// CMessagingContainer::ConstructL(const TRect& aRect)
// Symbian two phased constructor
// -----------------------------------------------------------------------------
//
void CMessagingContainer::ConstructL( const TRect& aRect )
    {
    CreateWindowL();	    
    // Set the windows size
    SetRect( aRect );
    // Activate the window, which makes it ready to be drawn
    ActivateL();
    }
    
// -----------------------------------------------------------------------------
// CMessagingContainer::~CMessagingContainer()
// Destructor
// -----------------------------------------------------------------------------
//
CMessagingContainer::~CMessagingContainer()
    {
    // no implementation needed here
    }

// -----------------------------------------------------------------------------
// CMessagingContainer::SizeChanged()
// Called by framework when the view size is changed
// -----------------------------------------------------------------------------
//
void CMessagingContainer::SizeChanged()
    {
    // no implementation needed here
    }

// -----------------------------------------------------------------------------
// CMessagingContainer::CountComponentControls() 
// Called by the framework in compound controls    
// @return the number of controls inside container
// -----------------------------------------------------------------------------
//
TInt CMessagingContainer::CountComponentControls() const
    {
    return 0; // there are no components
    }

// -----------------------------------------------------------------------------
// CMessagingContainer::ComponentControl(TInt aIndex) const
// Called by the framework in compound controls    
// @param The index of the control to return
// @return The control for aIndex
// -----------------------------------------------------------------------------
//
CCoeControl* CMessagingContainer::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        default:
            return NULL; // there are no components
        }
    }

// -----------------------------------------------------------------------------
// CMessagingContainer::Draw(const TRect& aRect) const
// Draw this application's view to the screen
// -----------------------------------------------------------------------------
//
void CMessagingContainer::Draw( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushColor( KRgbWhite );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.DrawRect( aRect );
    }

// -----------------------------------------------------------------------------
// CMessagingContainer::HandleControlEventL()
// Handles control events.
// -----------------------------------------------------------------------------
//
void CMessagingContainer::HandleControlEventL(
    CCoeControl*, TCoeEvent )
    {
    // no implementation needed here
    }

// End of File  
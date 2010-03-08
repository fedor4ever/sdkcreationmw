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
#include <eiklabel.h> 
#include "AknExPopListContainer.h"


// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CAknExPopListContainer::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAknExPopListContainer::ConstructL( const TRect& aRect )
    {
    CreateWindowL();
    SetRect( aRect );
    ActivateL();
    }

// -----------------------------------------------------------------------------
// CAknExPopListContainer::~CAknExPopListContainer()
// Destructor.
// -----------------------------------------------------------------------------
//
CAknExPopListContainer::~CAknExPopListContainer()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CAknExPopListContainer::SizeChanged()
// Called by framework when the view size is changed
// -----------------------------------------------------------------------------
//
void CAknExPopListContainer::SizeChanged()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CAknExPopListContainer::CountComponentControls() const
// return number of controls inside this container.
// -----------------------------------------------------------------------------
//
TInt CAknExPopListContainer::CountComponentControls() const
    {
    return KNumberOfControls;
    }

// -----------------------------------------------------------------------------
// CAknExPopListContainer::ComponentControl() const
// returns pointer of controls inside this container
// -----------------------------------------------------------------------------
//
CCoeControl* CAknExPopListContainer::ComponentControl( TInt /*aIndex*/ ) const
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CAknExPopListContainer:Draw() const
// Draws the display
// -----------------------------------------------------------------------------
//
void CAknExPopListContainer::Draw( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushColor( KRgbColorGray );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.DrawRect( aRect );
    }

// -----------------------------------------------------------------------------
// CAknExPopListContainer::HandleControlEventL()
// Handles an event of type aEventType reported
// by the control aControl to this observer.
// -----------------------------------------------------------------------------
//
void CAknExPopListContainer::HandleControlEventL( CCoeControl* /*aControl*/,
                                                  TCoeEvent /*aEventType*/ )
    {
    // No implementation required
    }

// End of File  

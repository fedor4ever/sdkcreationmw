/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of square custom control
*
*/


// INCLUDE FILES
#include <touchfeedback.h>
#include <akndef.h>
#include "tactileexamplesquare.h"


// ============================ MEMBER FUNCTIONS =============================


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CTactileExampleSquare* CTactileExampleSquare::NewL(
    const TRect& aRect, const CCoeControl* aParent )
    {
    CTactileExampleSquare* self = new ( ELeave ) CTactileExampleSquare;
    CleanupStack::PushL( self );
    self->ConstructL( aRect, aParent );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CTactileExampleSquare::~CTactileExampleSquare()
    {
    if ( iFeedback )
        {
        iFeedback->RemoveFeedbackForControl( this );
        }
    }


// ---------------------------------------------------------------------------
// Enables/disables feedback for this control
// ---------------------------------------------------------------------------
//
void CTactileExampleSquare::EnableFeedback( TBool aEnabled )
    {
    iFeedback->EnableFeedbackForControl( this, aEnabled );
    }


// ---------------------------------------------------------------------------
// From class MCoeControlHitTest.
// Tests whether a pointer event occurred inside the control's hit region
// ---------------------------------------------------------------------------
//
TBool CTactileExampleSquare::HitRegionContains( const TPoint& aPoint,
      const CCoeControl& /*aControl*/ ) const
    {
    return IsVisible() ? Rect().Contains( aPoint ) : EFalse;
    }


// ---------------------------------------------------------------------------
// From class CCoeControl.
//
// ---------------------------------------------------------------------------
//
void CTactileExampleSquare::Draw( const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.SetBrushColor( KRgbRed );
    gc.DrawRect( Rect() );
    }


// ---------------------------------------------------------------------------
// From class CCoeControl.
// Updates feedback area to match new position of the control
// ---------------------------------------------------------------------------
//
void CTactileExampleSquare::PositionChanged()
    {
    iFeedback->ChangeFeedbackArea( this, 0, Rect() );
    }


// ---------------------------------------------------------------------------
// From class CCoeControl.
// Updates feedback area to match new size of the control
// ---------------------------------------------------------------------------
//
void CTactileExampleSquare::SizeChanged()
    {
    iFeedback->SetFeedbackArea( this, 0, Rect(),
        ETouchFeedbackBasic, ETouchEventStylusDown );
    }


// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CTactileExampleSquare::CTactileExampleSquare()
    {
    // No implementation required
    }


// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CTactileExampleSquare::ConstructL(
    const TRect& aRect, const CCoeControl* aParent )
    {
    // Get a pointer to touch feedback instance
    iFeedback = MTouchFeedback::Instance();
    User::LeaveIfNull( iFeedback );

    SetContainerWindowL( *aParent );
    SetRect( aRect );
    
    // Set hit tester to be object itself
    SetHitTest( this );

    // Set feedback area for this control
    iFeedback->SetFeedbackArea( this, 0, Rect(),
        ETouchFeedbackBasic, ETouchEventStylusDown );

    ActivateL();
    }

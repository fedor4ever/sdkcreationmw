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
* Description:  Circle custom control implementation
*
*/


// INCLUDE FILES
#include <e32math.h>
#include <akndef.h>
#include <touchfeedback.h>
#include "tactileexamplecircle.h"


// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CTactileExampleCircle* CTactileExampleCircle::NewL( const TRect& aRect,
    const CCoeControl* aParent )
    {
    CTactileExampleCircle* self = new ( ELeave ) CTactileExampleCircle;
    CleanupStack::PushL( self );
    self->ConstructL( aRect, aParent );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CTactileExampleCircle::~CTactileExampleCircle()
    {
    }


// ---------------------------------------------------------------------------
// Enables/disables feedback for this control
// ---------------------------------------------------------------------------
//
void CTactileExampleCircle::EnableFeedback( TBool aEnabled )
    {
    MTouchFeedback* feedback = MTouchFeedback::Instance();
    if ( feedback )
        {
        feedback->EnableFeedbackForControl( this, aEnabled );
        }
    }


// ---------------------------------------------------------------------------
// From class CCoeControl.
//
// ---------------------------------------------------------------------------
//
void CTactileExampleCircle::Draw( const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.SetBrushColor( KRgbBlue );
    gc.DrawEllipse( Rect() );
    }


// ---------------------------------------------------------------------------
// From class CCoeControl.
// Gets called when circle is touched
// Plays feedback using instant feedback method
// ---------------------------------------------------------------------------
//
void CTactileExampleCircle::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    if ( aPointerEvent.iType == TPointerEvent::EButton1Down )
        {
        MTouchFeedback* feedback = MTouchFeedback::Instance();
        if ( feedback )
            {
            feedback->InstantFeedback( this, ETouchFeedbackBasic );
            }
        }
    CCoeControl::HandlePointerEventL( aPointerEvent );
    }


// ---------------------------------------------------------------------------
// From class MCoeControlHitTest.
// Tests whether a pointer event occurred inside the control's hit region
// Calculates distance of the pointer event from center of the circle and compares
// it to the radius
// ---------------------------------------------------------------------------
//
TBool CTactileExampleCircle::HitRegionContains( const TPoint& aPoint,
      const CCoeControl& /*aControl*/ ) const
    {
    TBool hit;

    if ( this->IsVisible() )
        {
        TReal distance; // aPoint's distance from center of the circle
        TReal pow1;
        TReal pow2;
        TPoint offSet = Rect().Center() - aPoint;
        Math::Pow( pow1, offSet.iX, 2 );
        Math::Pow( pow2, offSet.iY, 2 );
        Math::Sqrt( distance,  pow1 + pow2 );

        hit = ( distance > ( Rect().Width()/2 )) ? EFalse : ETrue;
        }
    else
        {
        hit = EFalse;
        }

    return hit;
    }


// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CTactileExampleCircle::CTactileExampleCircle()
    {
    // No implementation required
    }


// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CTactileExampleCircle::ConstructL( const TRect& aRect, const CCoeControl* aParent )
    {
    SetContainerWindowL( *aParent );
    SetRect( aRect );

    // Set hit tester to be object itself
    SetHitTest( this );

    ActivateL();
    }

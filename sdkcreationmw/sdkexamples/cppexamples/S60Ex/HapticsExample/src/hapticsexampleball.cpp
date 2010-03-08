/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the custom control class "Ball"
*
*/


#include <e32math.h>
#include <akndef.h>
#include <touchfeedback.h>
#include "hapticsexampleball.h"

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CHapticsExampleBall* CHapticsExampleBall::NewL()
    {
    CHapticsExampleBall* self = new ( ELeave ) CHapticsExampleBall;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CHapticsExampleBall::CHapticsExampleBall()
    {
    
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CHapticsExampleBall::ConstructL()
    {
    ActivateL();
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CHapticsExampleBall::~CHapticsExampleBall()
    {
    }

// ---------------------------------------------------------------------------
// From class CCoeControl.
// ---------------------------------------------------------------------------
//
void CHapticsExampleBall::Draw( const TRect& /*aRect*/ ) const
    {
    CWindowGc& gc = SystemGc();

    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.SetBrushColor( KRgbBlue );
    gc.DrawEllipse( Rect() );

    }

// ---------------------------------------------------------------------------
// From class MCoeControlHitTest.
// Tests whether a pointer event occurred inside the control's hit region
// Calculates distance of the pointer event from center of the circle and compares
// it to the radius
// ---------------------------------------------------------------------------
//
TBool CHapticsExampleBall::HitRegionContains( const TPoint& aPoint,
      const CCoeControl& /*aControl*/ ) const
    {
    TBool hit;

    TReal distance; // aPoint's distance from center of the ball
    TReal pow1;
    TReal pow2;
    TPoint offSet = Rect().Center() - aPoint;
    Math::Pow( pow1, offSet.iX, 2 );
    Math::Pow( pow2, offSet.iY, 2 );
    Math::Sqrt( distance,  pow1 + pow2 );

    hit = ( distance > ( Rect().Width()/2 )) ? EFalse : ETrue;

    return hit;
    }

// end of file

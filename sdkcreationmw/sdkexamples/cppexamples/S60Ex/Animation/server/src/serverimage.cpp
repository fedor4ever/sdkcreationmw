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
#include <e32std.h>
#include <e32base.h>
#include <txtfrmat.h>

#include "servertimeouttimer.h"
#include "serverimage.h"
#include "animation.pan"

// ============================ MEMBER FUNCTIONS ===============================

// Animation server object class functions.
// These functions are called by the windows server from
// requests made by the client animation dll.

// -----------------------------------------------------------------------------
// CImage::CImage()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CImage::CImage()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CImage::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CImage::ConstructL( TAny* /* aArgs */, TBool /* aHasFocus */ )
    {
    // Initialise starting points to the top left hand corner
    iLastPoint.SetXY( 0, 0 );
    iNewPoint.SetXY( 1, 1 );

    // Set up how far the shape moves each timer event
    iDeltaX = KStepSize;
    iDeltaY = KStepSize;

    // Animation reset not requested by user
    iAnimationReset = EFalse;

    // initiates an initial position update
    iDrawn = ETrue;

    // Set the rectangles size
    iSize.SetSize( 20,20 );

    // Store the screen size
    iScreenSize = iWindowFunctions->WindowSize();

    // Set the screen visible
    iWindowFunctions->SetVisible( ETrue );

    // We are using a timer, not the built in synchronising, so turn it off
    iFunctions->SetSync( MAnimGeneralFunctions::ESyncNone );

    // Create the timeout timer
    iServerTimer = CTimeOutTimer::NewL( EPriorityHigh, *this );

    // Set the timeout period
    iServerTimer->After( KTimerPeriod );
    }

// -----------------------------------------------------------------------------
// CImage::~CImage()
// Destructor.
// -----------------------------------------------------------------------------
//
CImage::~CImage()
    {
    // If the CTimer derived object exists and has an outstanding request,
    // cancel it.
    if ( iServerTimer )
        {
        iServerTimer->Cancel();
        }

    // Then delete the timer object
    delete iServerTimer;
    iServerTimer = NULL;
    }

// -----------------------------------------------------------------------------
// CImage::Redraw()
// Redraw the animation server object, includes rubbing out the old object.
// -----------------------------------------------------------------------------
//
void CImage::Redraw()
    {
    // get the background colour
    TLogicalRgb backgroundColour( TLogicalRgb::ESystemBackgroundColor );

    // Clear the old image.
    iGc->SetPenSize( TSize( 3,3 ) );
    iGc->SetPenColor( backgroundColour );
    iGc->DrawRect( TRect( iLastPoint, iSize ) );

    // Draw a shape at the new position
    iGc->SetPenSize( TSize( 1,1 ) );
    iGc->SetPenColor( KRgbBlack );
    iGc->DrawRect( TRect( iNewPoint, iSize ) );

    // signal that the current position has been drawn
    iDrawn = ETrue;
    }

// -----------------------------------------------------------------------------
// CImage::FocusChanged()
// Not bothered if the focus has changed.
// -----------------------------------------------------------------------------
//
void CImage::FocusChanged( TBool /* aState */ )
    {
    }

// -----------------------------------------------------------------------------
// CImage::Animate()
// Animate the animation server object.
// -----------------------------------------------------------------------------
//
void CImage::Animate( TDateTime* /* aDateTime */ )
    {

    // if the last position has been drawn, update the position
    if ( iDrawn )
        {
        // this position has not been drawn, so clear the flag
        iDrawn = EFalse;


        // Save the current position, so we can rub it out later
        iLastPoint = iNewPoint;

        // Check current position, 
        // reverse the direction if either limit is reached
        if ( ( iNewPoint.iX == iScreenSize.iTl.iX ) ||
           ( ( iNewPoint.iX + iSize.iWidth ) == iScreenSize.iBr.iX ) )
            {
            iDeltaX = -iDeltaX;
            }

        // Setup the next X coordinate
        iNewPoint.iX += iDeltaX;

        // Repeat for y coordinate
        if ( ( iNewPoint.iY == iScreenSize.iTl.iY ) ||
           ( ( iNewPoint.iY + iSize.iHeight ) == iScreenSize.iBr.iY ) )
            {
            iDeltaY = -iDeltaY;
            }
        iNewPoint.iY += iDeltaY;

        // Check for user requested reset
        if ( iAnimationReset )
            {
            iNewPoint.iX = 1;
            iNewPoint.iY = 1;
            iDeltaX = KStepSize;
            iDeltaY = KStepSize;
            iAnimationReset = EFalse;
            }


        // Alternative method, works well on the MARM target, jumps on the
        // windows emulator, simply uncomment and recompile
        //iFunctions->ActivateGc();
        //Redraw();

        // Force a draw to be performed by declaring the screen invalid
        iWindowFunctions->Invalidate( iScreenSize );
        }
    }

// -----------------------------------------------------------------------------
// CImage::CommandReplyL()
// Function to be used for nonbuffered commands. Not used in this example.
// -----------------------------------------------------------------------------
//
TInt CImage::CommandReplyL( TInt aOpcode, TAny* /* aArgs */ )
    {
    return aOpcode;
    }

// -----------------------------------------------------------------------------
// CImage::Command()
// Function for buffered commands and commands that cannot fail/leave.
// -----------------------------------------------------------------------------
//
void CImage::Command( TInt aOpcode, TAny* /* aArgs */ )
    {
    switch ( aOpcode )
        {
    case KAnimationReset:
        // Set a flag to reset the animation back to the top left
        iAnimationReset = ETrue;
        break;
        
    case KResolutionChange:
        iAnimationReset = ETrue;
        iScreenSize = iWindowFunctions->WindowSize();
        break;

    default:
        User::Panic( KAnimation, EPanicAnimationServer );
        break;
        }
    }

// -----------------------------------------------------------------------------
// CImage::TimedOut()
// Timer control, called everytime the time times out.
// -----------------------------------------------------------------------------
//
void CImage::TimedOut()
    {
    // Reset the timer timeout period
    iServerTimer->After( KTimerPeriod );

    // Call the animation function for this image
    iFunctions->Animate( 0 );
    }

// -----------------------------------------------------------------------------
// CImage::OfferRawEvent()
// For handling raw events. Not used in this example.
// -----------------------------------------------------------------------------
//
TBool CImage::OfferRawEvent( const TRawEvent& /*aRawEvent*/ )
    {
    return ETrue;
    }

// End of File

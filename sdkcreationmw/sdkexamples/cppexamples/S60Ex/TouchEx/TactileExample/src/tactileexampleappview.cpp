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
* Description:  Implementation of view class
*
*/


// INCLUDE FILES
#include <aknbutton.h>
#include <stringloader.h>
#include <touchfeedback.h>
#include <tactileexample.rsg>
#include "tactileexampleappview.h"
#include "tactileexamplesquare.h"
#include "tactileexamplecircle.h"


//ENUMERATION
enum TPointerToComponentView
    {
    ESquare = 0,
    ECircle,
    EButton
    };


//CONSTANTS
const TInt KNumberOfComponents = 3;
const TInt KMovementGrid = 10; // in pixels
const TInt KButtonHeight = 4;  // divisions of screen height
const TInt KObjSize      = 4;  // divisions of screen height or width


// ============================ MEMBER FUNCTIONS =============================


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CTactileExampleAppView* CTactileExampleAppView::NewL( const TRect& aRect )
    {
    CTactileExampleAppView* self = CTactileExampleAppView::NewLC( aRect );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CTactileExampleAppView* CTactileExampleAppView::NewLC( const TRect& aRect )
    {
    CTactileExampleAppView* self = new ( ELeave ) CTactileExampleAppView;
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }


// ---------------------------------------------------------------------------
// Frees reserved resources
// ---------------------------------------------------------------------------
//
CTactileExampleAppView::~CTactileExampleAppView()
    {
    delete iButton;
    delete iSquare;
    delete iCircle;
    }


// ---------------------------------------------------------------------------
// From class CCoeControl.
// return nbr of controls inside this container.
// ---------------------------------------------------------------------------
//
TInt CTactileExampleAppView::CountComponentControls() const
    {
    return KNumberOfComponents;
    }


// ---------------------------------------------------------------------------
// From class CCoeControl.
// return pointer of controls inside this container
// ---------------------------------------------------------------------------
//
CCoeControl* CTactileExampleAppView::ComponentControl(TInt aIndex) const
    {
    switch( aIndex )
        {
        case EButton:
            return iButton;
        case ESquare:
            return iSquare;
        case ECircle:
            return iCircle;
        default:
            return NULL;
        }
    }


// ---------------------------------------------------------------------------
// From class CCoeControl.
// Draw this CTactileExampleAppView to the screen.
// ---------------------------------------------------------------------------
//
void CTactileExampleAppView::Draw( const TRect& aRect ) const
    {
    // Get the standard graphics context
    CWindowGc& gc = SystemGc();

    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushColor( KRgbWhite );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );

    // Clears the area
    gc.Clear( aRect );
    }


// ---------------------------------------------------------------------------
// From class CCoeControl.
// Called by framework when the view size is changed.
// Set objects to default positions
// ---------------------------------------------------------------------------
//
void CTactileExampleAppView::SizeChanged()
    {
    // Gets the control's extent
    TRect rect( Rect() );

    // Set button's extent
    TRect buttonRect = TRect( rect.iTl.iX,
                              rect.iBr.iY - rect.Height() / KButtonHeight,
                              rect.iBr.iX, rect.iBr.iY );
    iButton->SetRect( buttonRect );

    // Set area where dragging is possible
    iDragArea.SetRect( rect.iTl.iX, rect.iTl.iY, rect.iBr.iX, buttonRect.iTl.iY );

    // Update object dimension
    ObjectDimension();
    }


// ---------------------------------------------------------------------------
// From class CCoeControl.
// Moves objects and plays feedback during dragging
// ---------------------------------------------------------------------------
//
void CTactileExampleAppView::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    TPoint point = aPointerEvent.iPosition;

    if ( iDragArea.Contains( point ))
        {
        if ( aPointerEvent.iType == TPointerEvent::EButton1Down &&
             iSquare->Rect().Center() != iCircle->Rect().Center() )
            {
            if ( iCircle->HitRegionContains( point, *iCircle ))
                {
                // dragging of circle started
                iDragCircle = ETrue;
                iDraggingOffset = iCircle->Position() - point;
                }
            else if ( iSquare->HitRegionContains( point, *iSquare ))
                {
                // dragging of square started
                iDragSquare = ETrue;
                iDraggingOffset = iSquare->Position() - point;
                }
            else
                {
                // Cancel dragging just in case
                iDragCircle = EFalse;
                iDragSquare = EFalse;
                }
            }

        if ( aPointerEvent.iType == TPointerEvent::EDrag )
            {
            if( iDragCircle )
                {
                Move( iCircle, point + iDraggingOffset );
                }
            else if( iDragSquare )
                {
                Move( iSquare, point + iDraggingOffset );
                }

            if( iSquare->Rect().Center() == iCircle->Rect().Center() )
                {
                // Objects are in end positions -> cancel dragging
                iDragCircle = EFalse;
                iDragSquare = EFalse;

                // and disable all feedbacks
                iSquare->EnableFeedback( EFalse );
                iCircle->EnableFeedback( EFalse );
                }
            }
        }

    if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
        {
        // possible dragging is cancelled if button is released
        iDragCircle = EFalse;
        iDragSquare = EFalse;
        }

    CCoeControl::HandlePointerEventL( aPointerEvent );
    }


// ---------------------------------------------------------------------------
// From class CCoeControl.
// Enables feedback when layout is changed
// ---------------------------------------------------------------------------
//
void CTactileExampleAppView::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        // Set circle and square to default positions
        SetObjects();

        // enable feedback again since it might have been disabled
        // if square & circle are in end positions
        iCircle->EnableFeedback( ETrue );
        iSquare->EnableFeedback( ETrue );

        // ohjects are made visible/hidden depending 
        // is touch supported for new layout
        iSquare->MakeVisible( AknLayoutUtils::PenEnabled() );
        iCircle->MakeVisible( AknLayoutUtils::PenEnabled() );
        }
    }


// ---------------------------------------------------------------------------
// From class MCoeControlObserver
// Checks if button is pressed and returns objects to default positions
// ---------------------------------------------------------------------------
//
void CTactileExampleAppView::HandleControlEventL(
    CCoeControl* aControl, TCoeEvent aEventType )
    {
    if ( aControl == iButton )
        {
        if ( aEventType == EEventStateChanged )
            {
            // Button press resets object positions
            SetObjects();
            DrawDeferred();

            // enable feedback again since it might have been disabled
            // if square & circle are in end positions
            iCircle->EnableFeedback( ETrue );
            iSquare->EnableFeedback( ETrue );
            }
        }
    }


// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CTactileExampleAppView::CTactileExampleAppView()
    {
    // No implementation required
    }


// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CTactileExampleAppView::ConstructL( const TRect& aRect )
    {
    // Create a window for this application view
    CreateWindowL();

    // Load a string from the resource file to be used as button label
    HBufC* textResource = StringLoader::LoadLC( R_TACT_BUTTON_TEXT );

    // Create button
    iButton = CAknButton::NewL( NULL, NULL, NULL, NULL,
              *textResource, KNullDesC, KAknButtonSizeFitText, 0 );
    User::LeaveIfNull( iButton );
    iButton->SetContainerWindowL( *this );
    iButton->MakeVisible( ETrue );
    iButton->SetObserver( this );

    // Update object dimesion
    ObjectDimension();

    // Create square
    iSquare = CTactileExampleSquare::NewL( TRect( TSize( iObjectDimension,
                                          iObjectDimension )), this );
    iSquare->SetContainerWindowL( *this );
    iSquare->MakeVisible( AknLayoutUtils::PenEnabled() );

    // Create circle
    iCircle = CTactileExampleCircle::NewL( TRect( TSize( iObjectDimension,
                                          iObjectDimension )), this );
    iCircle->SetContainerWindowL( *this );
    iCircle->MakeVisible( AknLayoutUtils::PenEnabled() );

    // Pop HBuf from CleanUpStack and Destroy it.
    CleanupStack::PopAndDestroy( textResource );


    // Set the windows size
    SetRect( aRect );

    // Set objects to default positions
    SetObjects();

    // Get a pointer to touch feedback instance
    iFeedback = MTouchFeedback::Instance();
    User::LeaveIfNull( iFeedback );

    EnableDragEvents();

    // Activate the window, which makes it ready to be drawn
    ActivateL();
    }


// ---------------------------------------------------------------------------
// Sets the circle and square objects to default position
// ---------------------------------------------------------------------------
//
void CTactileExampleAppView::SetObjects()
    {
    TPoint squarePos = TPoint( iDragArea.iBr.iX - iObjectDimension,
                               iDragArea.iBr.iY - iObjectDimension );

    // modify coordinates to match the grid
    squarePos.iX = squarePos.iX - squarePos.iX % KMovementGrid;
    squarePos.iY = squarePos.iY - squarePos.iY % KMovementGrid;

    iSquare->SetExtent( squarePos, TSize( iObjectDimension, iObjectDimension ));
    iCircle->SetExtent( TPoint( iDragArea.iTl.iX, iDragArea.iTl.iY ),
                        TSize( iObjectDimension, iObjectDimension ));
    }


// ---------------------------------------------------------------------------
// Adjusts aPoint to match the grid and moves the control if needed
// ---------------------------------------------------------------------------
//
void CTactileExampleAppView::Move( CCoeControl* aControl, TPoint aPoint )
    {
    TPoint newPosition;
    
    // adjust aPoint to match the grid    
    newPosition.iX = aPoint.iX - aPoint.iX % KMovementGrid;
    newPosition.iY = aPoint.iY - aPoint.iY % KMovementGrid;
    
    if ( newPosition != aControl->Position() )
        {
        // Feedback is given as early as possible
        iFeedback->InstantFeedback( aControl, ETouchFeedbackSensitive );

        // Define the changed area
        TRect drawRect = TRect( newPosition, aControl->Size() );
        drawRect.BoundingRect( aControl->Rect() );

        // set control to new position
        aControl->SetPosition( newPosition );

        // draw only changed area
        DrawNow( drawRect );
        }
    }


// ---------------------------------------------------------------------------
// Calculates objects dimesion related to screen size.
// ---------------------------------------------------------------------------
//
void CTactileExampleAppView::ObjectDimension()
    {
    TPixelsAndRotation ScreenSize;
    iCoeEnv->ScreenDevice()->GetDefaultScreenSizeAndRotation( ScreenSize );
    TInt side = ScreenSize.iPixelSize.iWidth < ScreenSize.iPixelSize.iHeight ?
            ScreenSize.iPixelSize.iWidth / KObjSize : ScreenSize.iPixelSize.iHeight / KObjSize;

    // size is adjusted to match the grid
    iObjectDimension = ( side - side % ( KMovementGrid * 2 ));
    }
// End of File

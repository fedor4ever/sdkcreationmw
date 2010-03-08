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
#include <AknUtils.h>

#include "GuiEngine.pan"
#include "GuiEngineAppView.h"
#include "GuiEngineDocument.h"
#include "Circle.h"
#include "Rectangle.h"


using namespace NShapes;

// ========================= MEMBER FUNCTIONS ==================================

// ------------------------------------------------------------------------------
// CGuiEngineAppView::CGuiEngineAppView()
// C++ default constructor can NOT contain any code, that might leave.
// ------------------------------------------------------------------------------
//
CGuiEngineAppView::CGuiEngineAppView( CGuiEngineDocument* aDocument )
: iDocument(aDocument), iPosition ( 10, 10 )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CGuiEngineAppView::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGuiEngineAppView* CGuiEngineAppView::NewL( const TRect& aRect,
                                            CGuiEngineDocument* aDocument )
    {
    CGuiEngineAppView* self = NewLC( aRect, aDocument );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CGuiEngineAppView::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGuiEngineAppView* CGuiEngineAppView::NewLC( const TRect& aRect,
                                             CGuiEngineDocument* aDocument )
    {
    CGuiEngineAppView* self = new ( ELeave ) CGuiEngineAppView( aDocument );
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
    }

// ------------------------------------------------------------------------------
// CGuiEngineAppView::ConstructL()
// Symbian 2nd phase constructor can leave.
// ------------------------------------------------------------------------------
//
void CGuiEngineAppView::ConstructL( const TRect& aRect )
    {
    // Create a window for this application view
    CreateWindowL();

    // Set the windows size
    SetRect( aRect );

    // Activate the window, which makes it ready to be drawn
    ActivateL();

    Window().SetPointerCursor( KBlackPointer );
    }

// -----------------------------------------------------------------------------
// CGuiEngineAppView::~CGuiEngineAppView()
// Destructor.
// -----------------------------------------------------------------------------
//
CGuiEngineAppView::~CGuiEngineAppView()
    {
    iDocument = NULL;   //  Not the owner
    }

// -----------------------------------------------------------------------------
// CGuiEngineAppView::Draw()
// Draws the display
// -----------------------------------------------------------------------------
//
void CGuiEngineAppView::Draw( const TRect& /*aRect*/ ) const
    {

    CWindowGc& graphicsContext = SystemGc();

    // Clear the application view
    graphicsContext.Clear();

	// Gets the control's extent
	TRect rect( Rect() );
	graphicsContext.DrawRect( rect );

    // Draw the 'cursor' crosshair, size 10 pixels by 10 pixels
    graphicsContext.SetPenSize( TSize( 1,1 ) );

    //graphicsContext.SetPenColor( RgbBlack );
    graphicsContext.DrawLine ( TPoint( iPosition.iX - 10, iPosition.iY ),
                               TPoint( iPosition.iX + 10, iPosition.iY ) );

    graphicsContext.DrawLine ( TPoint( iPosition.iX, iPosition.iY - 10 ),
                               TPoint( iPosition.iX, iPosition.iY + 10 ) );


    // Draw all the current shapes
    TShape* shape = iDocument->Model()->GetNextShape();
    while ( shape )
    {
        shape->Draw( graphicsContext );
        shape = iDocument->Model()->GetNextShape();
        }
    }

// -----------------------------------------------------------------------------
// CGuiEngineAppView::Clear()
// Clears the display
// -----------------------------------------------------------------------------
//
void CGuiEngineAppView::Clear()
    {
    iDocument->Model()->Clear();
    DrawNow();
    }

// -----------------------------------------------------------------------------
// CGuiEngineAppView::SetBrushShapeType()
// Sets the BrushShapeType
// -----------------------------------------------------------------------------
//
void CGuiEngineAppView::SetBrushShapeType( TBrushShapeType aShapeType )
    {
    iBrushShapeType = aShapeType;
    }

// -----------------------------------------------------------------------------
// CGuiEngineAppView::BrushShapeType()
// Get the BrushShapeType
// -----------------------------------------------------------------------------
//
CGuiEngineAppView::TBrushShapeType CGuiEngineAppView::BrushShapeType() const
    {
    return iBrushShapeType;
    }

// -----------------------------------------------------------------------------
// CGuiEngineAppView::ExternalizeL()
// Externalizes BrushShapeType (write)
// -----------------------------------------------------------------------------
//
void CGuiEngineAppView::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt8L( static_cast<TInt8>( iBrushShapeType ) );
    }

// -----------------------------------------------------------------------------
// CGuiEngineAppView::InternalizeL()
// Internalize BrushShapeType (read)
// -----------------------------------------------------------------------------
//
void CGuiEngineAppView::InternalizeL( RReadStream& aStream )
    {
    iBrushShapeType = static_cast<TBrushShapeType>( aStream.ReadInt8L() );
    }



// -----------------------------------------------------------------------------
// CGuiEngineAppView::OfferKeyEventL()
// Notify key events.
// -----------------------------------------------------------------------------
//
TKeyResponse CGuiEngineAppView::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                TEventCode aType)
    {
    if ( aType != EEventKey )
        {
        return EKeyWasNotConsumed;
        }

    // Move left
    if ( aKeyEvent.iScanCode == EStdKeyLeftArrow )
        {
        if ( iPosition.iX >  ( Rect().iTl.iX + 10 ) )
            {
            --iPosition.iX;
            DrawNow();
            }
        return EKeyWasConsumed;
        }

    // Move right
    else if ( aKeyEvent.iScanCode == EStdKeyRightArrow )
        {
        if ( iPosition.iX < ( Rect().iBr.iX - 10 ) )
            {
            ++iPosition.iX;
            DrawNow();
            }
        return EKeyWasConsumed;
        }

    // Move up
    else if ( aKeyEvent.iScanCode == EStdKeyUpArrow )
        {
        if (iPosition.iY > ( Rect().iTl.iY + 10 ) )
            {
            --iPosition.iY;
            DrawNow();
            }
        return EKeyWasConsumed;
        }

    // Move down
    else if ( aKeyEvent.iScanCode == EStdKeyDownArrow )
        {
        if ( iPosition.iY < ( Rect().iBr.iY - 10 ) )
            {
            ++iPosition.iY;
            DrawNow();
            }
        return EKeyWasConsumed;
        }


    // Place a shape
    else if ( aKeyEvent.iScanCode == EStdKeyDevice3 )
        {

        TShape* newShape = NULL;
        // Update the co-ordinates in the model to the position at which the
        // pointer event occurred.

        switch ( iBrushShapeType )
            {
            case ECircle:

                newShape = new ( ELeave ) TCircle( iPosition, KBrushRadius );

                //  Takes ownership
                iDocument->Model()->AddShapeL( newShape );
                break;

            case ERectangle:
                newShape = new ( ELeave ) TRectangle( iPosition, KBrushHeight, KBrushWidth );

                //  Takes ownership
                iDocument->Model()->AddShapeL( newShape );
                break;

            default:
                User::Panic( KGuiEnginePanicName, EGuiEngineInvalidBrushType );
            }

        DrawNow();
        return EKeyWasConsumed;
        }

    return EKeyWasNotConsumed;
    }



// -----------------------------------------------------------------------------
// CGuiEngineAppView::HandleResourceChange()
// Handles responses to screen size changes (and resource changes)
// -----------------------------------------------------------------------------
//
void CGuiEngineAppView::HandleResourceChange( TInt aType )
	{
	CCoeControl::HandleResourceChange( aType );
    // ADDED FOR SCALABLE UI SUPPORT
    // *****************************
    if ( aType==KEikDynamicLayoutVariantSwitch )
        {
        TRect rect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
        SetRect(rect);
        }
	}


// End of File

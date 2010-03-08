/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include "SIPExGameContainer.h"
#include "SIPExGameEngine.h"
#include "SIPExDoc.h"

#include <akndef.h>
#include <aknutils.h>
#include <eiklabel.h>
#include <eikenv.h>

#include <AknsDrawUtils.h>
#include <AknsBasicBackgroundControlContext.h>

const TInt KComponentCount( 0 );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPExGameContainer::CSIPExGameContainer
// C++ default constructor
// -----------------------------------------------------------------------------
//
CSIPExGameContainer::CSIPExGameContainer( CSIPExEngine& aEngine )
    : iEngine( aEngine )
    {
    // no implementation required
    }

// -----------------------------------------------------------------------------
// CSIPExGameContainer::ConstructL
// Two phased constructor
// -----------------------------------------------------------------------------
//
void CSIPExGameContainer::ConstructL( const TRect& aRect )
    {
	CalculateBoardDimensions( aRect );
	
	iBgContext = 
	    CAknsBasicBackgroundControlContext::NewL( KAknsIIDQsnBgAreaMain, 
	                                              aRect, 
	                                              ETrue );
	
    CreateWindowL();

    SetRect( aRect );
    
    ActivateL();
    }
    
// -----------------------------------------------------------------------------
// CSIPExGameContainer::~CSIPExGameContainer
// Destructor
// -----------------------------------------------------------------------------
//
CSIPExGameContainer::~CSIPExGameContainer()
    {
    delete iBgContext;
    }

// -----------------------------------------------------------------------------
// CSIPExGameContainer::SizeChanged
// Called by framework when the view size is changed
// -----------------------------------------------------------------------------
//
void CSIPExGameContainer::SizeChanged()
    {
    // Control resize code
    }

// -----------------------------------------------------------------------------
// CSIPExGameContainer::CountComponentControls
// -----------------------------------------------------------------------------
//
TInt CSIPExGameContainer::CountComponentControls() const
    {
    return KComponentCount; // return nbr of controls inside this container
    }

// -----------------------------------------------------------------------------
// CSIPExGameContainer::ComponentControl
// -----------------------------------------------------------------------------
//
CCoeControl* CSIPExGameContainer::ComponentControl( TInt /*aIndex*/ ) const
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CSIPExGameContainer::Draw
// -----------------------------------------------------------------------------
//
void CSIPExGameContainer::Draw( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc();
    
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.SetBrushColor( KRgbWhite );
    gc.Clear();
    
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    MAknsControlContext* cc = AknsDrawUtils::ControlContext( this );
    
    AknsDrawUtils::Background( skin, cc, this, gc, aRect );

    DrawGame( gc, aRect );
    }
   
// -----------------------------------------------------------------------------
// CSIPExGameView::DrawGame
// Draws the game view. Decides whether we should draw the board or cursor 
// or not
// -----------------------------------------------------------------------------
//
void CSIPExGameContainer::DrawGame( CWindowGc& aGc, const TRect& aRect ) const 
    {
    if( iEngine.DrawBoard() )
        {
		DrawBoard( aGc, aRect );

        if( iEngine.DrawCursor() )
            {
			DrawCursor( aGc, aRect );
            }
	    }    
    }

// -----------------------------------------------------------------------------
// CSIPExGameView::DrawBoard
// Draws the game board.
// -----------------------------------------------------------------------------
//
void CSIPExGameContainer::DrawBoard( CWindowGc& aGc, const TRect& aRect ) const 
    {
    CWindowGc& gc = aGc;

    gc.SetPenSize( TSize(1,1) );
    gc.SetPenStyle( CGraphicsContext::ESolidPen );
    gc.SetPenColor( TRgb(0,0,0) );
    gc.SetBrushColor( TRgb(0,0,255) );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );


	TPoint p1 = aRect.iTl;
	p1.iX = p1.iX + iXOffset;
	p1.iY = p1.iY + iYOffset;

	TPoint p2 = aRect.iBr;
	p2.iX = p1.iX + ( ( KBoxCountX * iBoxW ) + ( ( KBoxCountX + 1 ) * iMidW ) );
	p2.iY = p1.iY + ( ( KBoxCountY * iBoxW ) + ( ( KBoxCountY + 1 ) * iMidW ) );
	TRect box2 = TRect(p1, p2);
	gc.DrawRect( box2 );


	gc.SetPenColor( TRgb(0,0,0) );
    gc.SetBrushColor( TRgb(255,255,255) );
    gc.SetPenStyle( CGraphicsContext::ESolidPen );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );

	for (TInt i=0 ; i < KBoxCountX; i++) 
        {
	    for (TInt j=0 ; j < KBoxCountY; j++) 
            {
	        p1 = aRect.iTl;
	        p2 = aRect.iBr;

	        p1.iX = p1.iX + iXOffset + iMidW + (i*iMidW) + (i*iBoxW);
	        p1.iY = p1.iY + iYOffset + iMidW + (j*iMidW) + (j*iBoxW);

	        p2.iX = p1.iX + iBoxW;
	        p2.iY = p1.iY + iBoxW;

	        if ( iEngine.BoardValue( i, j ) == -1 )
                {
		        gc.SetBrushColor( TRgb(255,255,255) );
                }
	        else if ( iEngine.BoardValue( i, j ) == 1 )
                {
		        gc.SetBrushColor( TRgb(255,0,0) );
                }
	        else if ( iEngine.BoardValue( i, j ) == 2 )
                {
		        gc.SetBrushColor( TRgb(0,255,0) );
                }
            else { /* Nothing to do */ }

	        TRect rect( p1, p2 ); 
	        gc.DrawEllipse( rect );
	        }
        }
    }

// -----------------------------------------------------------------------------
// CSIPExGameView::DrawCursor
// Draws the cursor.
// -----------------------------------------------------------------------------
//
void CSIPExGameContainer::DrawCursor( CWindowGc& aGc, const TRect& aRect ) const
    {
    CWindowGc& gc = aGc;

	gc.SetPenColor(TRgb(0,0,0));
	gc.SetBrushColor(TRgb(255,0,0));
    gc.SetPenStyle(CGraphicsContext::ESolidPen);
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);


	TInt i = iEngine.Cursor();

	TPoint p1 = aRect.iTl;
	TPoint p2 = aRect.iBr;

	p1.iX = p1.iX + iXOffset + iMidW + (i*iMidW) + (i*iBoxW);
	p1.iY = p1.iY + iYOffset - iBoxW;

	p2.iX = p1.iX + iBoxW;
	p2.iY = p1.iY + iBoxW;

	TRect rect( p1, p2 ); 
	gc.DrawEllipse( rect );
    }

// -----------------------------------------------------------------------------
// CSIPExGameContainer::CalculateBoardDimensions
// -----------------------------------------------------------------------------
//   
void CSIPExGameContainer::CalculateBoardDimensions( const TRect& aRect )
    {
	TInt boxAndMid( 0 );
    if ( aRect.Height() < aRect.Width() )
        {
        boxAndMid = ( aRect.Height() ) / ( KBoxCountY + 2 );
        iXOffset = ( aRect.Width() - KBoxCountX * boxAndMid ) / 2;
        }
    else
        {
        boxAndMid = aRect.Width() / ( KBoxCountX + 2 );
        iXOffset = boxAndMid;
        }
    iYOffset = 2 * boxAndMid;

	iMidW = boxAndMid / 4;
	iBoxW = boxAndMid - iMidW;
    }

// -----------------------------------------------------------------------------
// CSIPExGameContainer::HandleControlEventL
// -----------------------------------------------------------------------------
//
void CSIPExGameContainer::HandleControlEventL(
    CCoeControl* /*aControl*/,TCoeEvent /*aEventType*/ )
    {
    
    }

// -----------------------------------------------------------------------------
// CSIPExGameContainer::OfferKeyEventL
// From CCoeControl.
// -----------------------------------------------------------------------------
//
TKeyResponse CSIPExGameContainer::OfferKeyEventL( 
    const TKeyEvent& aKeyEvent, 
    TEventCode aType )
    {
    if( aType == EEventKeyUp )
        {
        switch( aKeyEvent.iScanCode )
            {
            case EStdKeyLeftArrow:
                iEngine.CursorLeft();
                DrawDeferred();
                return EKeyWasConsumed;
            case EStdKeyRightArrow:
                iEngine.CursorRight();
                DrawDeferred();
                return EKeyWasConsumed;
            case EStdKeyDownArrow:
            case EStdKeyEnter:
			case EStdKeyDevice7: // FLOWTHROUGH
                iEngine.CursorPressed();
                DrawDeferred();
                return EKeyWasConsumed;
            default:
                return EKeyWasNotConsumed;
            }
        }

    return EKeyWasNotConsumed;
    }
    
// -----------------------------------------------------------------------------
// CSIPExGameContainer::HandleResourceChange
// From CCoeControl.
// -----------------------------------------------------------------------------
// 
void CSIPExGameContainer::HandleResourceChange( TInt aType )
	{
	CCoeControl::HandleResourceChange( aType );
	
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        TRect mainPaneRect;
        AknLayoutUtils::LayoutMetricsRect( AknLayoutUtils::EMainPane, 
                                           mainPaneRect );
        SetRect( mainPaneRect );
        
        CalculateBoardDimensions( mainPaneRect );
        
        if ( iBgContext )
            {
            iBgContext->SetRect( mainPaneRect );
            }
        
        DrawNow();
        }
	}

// -----------------------------------------------------------------------------
// CSIPExGameContainer::MopSupplyObject
// From CCoeControl.
// -----------------------------------------------------------------------------
// 	
TTypeUid::Ptr CSIPExGameContainer::MopSupplyObject( TTypeUid aId )
    {
    if ( aId.iUid == MAknsControlContext::ETypeId ) 
        { 
        return MAknsControlContext::SupplyMopObject( aId, iBgContext );
        }
    return CCoeControl::MopSupplyObject( aId );
    }


// End of File  

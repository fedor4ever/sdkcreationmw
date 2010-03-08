/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include <coemain.h>
#include <badesca.h> 
#include <eiklabel.h> 
#include <aknbutton.h> 
#include <barsread.h>
#include <longtapdetectorexample.rsg>
#include <aknsdrawutils.h>
#include <aknsbasicbackgroundcontrolcontext.h>
#include <stringloader.h>

#include "longtapdetectorexcontainer.h"


// ============================== CONSTANTS ==================================

// =========================== MEMBER FUNCTIONS ==============================


// ---------------------------------------------------------------------------
// C++ default Constructor
// ---------------------------------------------------------------------------
//
CLongTapDetectorExContainer::CLongTapDetectorExContainer() : iLongTapCallbackReceived( EFalse )
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CLongTapDetectorExContainer::~CLongTapDetectorExContainer()
    {

    delete iLabel;
    delete iBgContext;
    delete iLongTapDetector;
    }

// ---------------------------------------------------------------------------
// Symbian 2nd Constructor
// ---------------------------------------------------------------------------
//
void CLongTapDetectorExContainer::ConstructL( const TRect& aRect )
    {
    CreateWindowL();
    SetRect( aRect );
    iLongTapDetector = CAknLongTapDetector::NewL( this );
    // Label for showing messages    
    iLabel = new ( ELeave ) CEikLabel;
    iLabel->SetContainerWindowL( *this );
    iLabel->SetLabelAlignment( ELayoutAlignCenter );
    HBufC* appName = StringLoader::LoadL( R_LONGTAPDETECTOREX_APPNAME );  
    iLabel->SetTextL( *appName );
    delete appName;
    
    // Background context for skinned background
    iBgContext = CAknsBasicBackgroundControlContext::NewL( 
        KAknsIIDSkinBmpMainPaneUsual, TRect( 0, 0, 0, 0 ), ETrue 
    );
    iBgContext->SetRect( Rect() );
    iBgContext->SetParentPos( PositionRelativeToScreen() );
    SizeChanged();
    DrawableWindow()->SetPointerGrab( ETrue );
    EnableDragEvents();
    ActivateL();
    }

// ---------------------------------------------------------------------------
// CLongTapDetectorExContainer::Draw
// ---------------------------------------------------------------------------
//
void CLongTapDetectorExContainer::Draw( const TRect& aRect ) const
    {
    // Draw background
    CWindowGc& gc = SystemGc();
        	
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    if ( !AknsDrawUtils::Background( skin, iBgContext, gc, aRect ) )
        {
        SystemGc().Clear( aRect );
        }
    
    TRgb textColor = KRgbBlack;
    AknsUtils::GetCachedColor( AknsUtils::SkinInstance(), textColor,
        KAknsIIDQsnTextColors, EAknsCIQsnTextColorsCG55 );
    AknLayoutUtils::OverrideControlColorL( *iLabel, EColorLabelText, textColor );
    }

// ---------------------------------------------------------------------------
// CLongTapDetectorExContainer::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CLongTapDetectorExContainer::CountComponentControls() const
    {
    TInt count ( 1 ); // iLabel
    // TODO
    return count;
    }

// ---------------------------------------------------------------------------
// CLongTapDetectorExContainer::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CLongTapDetectorExContainer::ComponentControl( TInt aIndex ) const
    {
    if ( aIndex == 0 )
        {
        return iLabel;
        }
    
    // TODO
        
    return NULL;
    }

void CLongTapDetectorExContainer::SizeChanged()
    {
    if( iLabel )
        {
        iLabel->SetRect( TRect( TPoint(Rect().iTl.iX, Rect().iTl.iY + 35), TPoint(Rect().iBr.iX, Rect().iTl.iY + 75  ) ) );
        }
    DrawDeferred();
    }

void CLongTapDetectorExContainer::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );
    if  ( aType == KEikDynamicLayoutVariantSwitch )
    	{
        TRect rect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
        SetRect(rect);
        
        iBgContext->SetRect( Rect() );
        iBgContext->SetParentPos( PositionRelativeToScreen() );
        }
    }
void CLongTapDetectorExContainer::HandlePointerEventL( const TPointerEvent& aEvent )
    {
    if ( AknLayoutUtils::PenEnabled() )
        {
        CCoeControl::HandlePointerEventL( aEvent );
    
        if( aEvent.iType == TPointerEvent::EButton1Down )
            {
            iLongTapCallbackReceived = EFalse;
            HBufC* appName = StringLoader::LoadLC( R_LONGTAPDETECTOREX_APPNAME );  
            iLabel->SetTextL( *appName );
            CleanupStack::PopAndDestroy();
            DrawDeferred();
         }
         
        if( aEvent.iType == TPointerEvent::EButton1Up && !iLongTapCallbackReceived )
            {
            HBufC* appName = StringLoader::LoadLC( R_LONGTAPDETECTOREX_SINGLETAP_RECEIVED );  
            iLabel->SetTextL( *appName );
            CleanupStack::PopAndDestroy();
            DrawDeferred();
            }

        iLongTapDetector->PointerEventL( aEvent );   
        }
    }
    
void CLongTapDetectorExContainer::HandleLongTapEventL( const TPoint& /*aPenEventLocation*/, 
                                      const TPoint& /*aPenEventScreenLocation*/ )
    {
    iLongTapCallbackReceived = ETrue;
    HBufC* labelText = StringLoader::LoadLC( R_LONGTAPDETECTOREX_CALLBACK_RECEIVED );  
    iLabel->SetTextL( *labelText );
    CleanupStack::PopAndDestroy();
    DrawDeferred();
    }
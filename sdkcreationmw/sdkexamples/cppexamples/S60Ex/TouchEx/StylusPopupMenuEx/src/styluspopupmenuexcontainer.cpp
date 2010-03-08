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
#include <styluspopupmenuexample.rsg>
//#include <styluspopupmenuexample.mbg>
#include <aknsdrawutils.h>
#include <aknsbasicbackgroundcontrolcontext.h>
#include <stringloader.h>

#include "styluspopupmenuexcontainer.h"
#include "styluspopupmenuex.hrh"

// ============================== CONSTANTS ==================================

_LIT(KMenuItem1, "Item 1");
_LIT(KMenuItem2, "Item 2");
_LIT(KMenuItem3, "Item 3");
_LIT(KMenuItem4, "Item 4");

// =========================== MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// C++ default Constructor
// ---------------------------------------------------------------------------
//
CStylusPopupMenuExContainer::CStylusPopupMenuExContainer() : iUseResource( EFalse )
    {
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CStylusPopupMenuExContainer::~CStylusPopupMenuExContainer()
    {

    delete iLabel;
    delete iBgContext;
    delete iPopup;
    }

// ---------------------------------------------------------------------------
// Symbian 2nd Constructor
// ---------------------------------------------------------------------------
//
void CStylusPopupMenuExContainer::ConstructL( const TRect& aRect )
    {
    CreateWindowL();
    SetRect( aRect );

    // Label for showing messages    
    iLabel = new ( ELeave ) CEikLabel;
    iLabel->SetContainerWindowL( *this );
    iLabel->SetLabelAlignment( ELayoutAlignCenter );
    HBufC* appName = StringLoader::LoadL( R_STYLUSPOPUPMENUEX_APPNAME );  
    iLabel->SetTextL( *appName );
    delete appName;
    
    // Background context for skinned background
    iBgContext = CAknsBasicBackgroundControlContext::NewL( 
        KAknsIIDSkinBmpMainPaneUsual, TRect( 0, 0, 0, 0 ), ETrue 
    );
    iBgContext->SetRect( Rect() );
    iBgContext->SetParentPos( PositionRelativeToScreen() );
    
    SizeChanged();
    ActivateL();
    }

// ---------------------------------------------------------------------------
// CStylusPopupMenuExContainer::Draw
// ---------------------------------------------------------------------------
//
void CStylusPopupMenuExContainer::Draw( const TRect& aRect ) const
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
// CStylusPopupMenuExContainer::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CStylusPopupMenuExContainer::CountComponentControls() const
    {
    TInt count ( 1 ); // iLabel
    return count;
    }

// ---------------------------------------------------------------------------
// CStylusPopupMenuExContainer::ComponentControl
// ---------------------------------------------------------------------------
//
CCoeControl* CStylusPopupMenuExContainer::ComponentControl( TInt aIndex ) const
    {
    if ( aIndex == 0 )
        {
        return iLabel;
        }
    
    return NULL;
    }

void CStylusPopupMenuExContainer::SizeChanged()
    {
    if( iLabel )
        {
        iLabel->SetRect( TRect( TPoint(Rect().iTl.iX, Rect().iTl.iY + 35), TPoint(Rect().iBr.iX, Rect().iTl.iY + 75  ) ) );
        }
    DrawDeferred();
    }
    
void CStylusPopupMenuExContainer::HandlePointerEventL(const TPointerEvent &aPointerEvent)
    {
    CCoeControl::HandlePointerEventL( aPointerEvent );
    if( aPointerEvent.iType ==TPointerEvent::EButton1Up )
        {
        if( iUseResource )
            {
            CreatePopupMenuFromResourceL( aPointerEvent.iPosition );
            }
        else
            {
            CreatePopupMenuL( aPointerEvent.iPosition );  
            }
        }
    
    }

void CStylusPopupMenuExContainer::HandleResourceChange( TInt aType )
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

void CStylusPopupMenuExContainer::CreatePopupMenuFromResourceL( const TPoint &aPosition )
    {
    delete iPopup;
    
    iPopup = CAknStylusPopUpMenu::NewL( this, aPosition );
    TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC( reader, R_STYLUSPOPUPMENUEX_STYLUSPOPUPMENU );
    iPopup->ConstructFromResourceL( reader );
    CleanupStack::PopAndDestroy();
    
    HBufC* labelText = StringLoader::LoadLC( R_STYLUSPOPUPMENUEX_MENU_CREATED );
    iLabel->SetTextL( *labelText );
    CleanupStack::PopAndDestroy();
    DrawDeferred();
    
    iPopup->ShowMenu();
    }

void CStylusPopupMenuExContainer::UseResourceForPopupMenuL( TBool aValue )
    {
    iUseResource = aValue;
    
    HBufC* labelText = StringLoader::LoadLC( R_STYLUSPOPUPMENUEX_SETTING_CHANGED );
    iLabel->SetTextL( *labelText );
    CleanupStack::PopAndDestroy();
    DrawDeferred();
    }
void CStylusPopupMenuExContainer::CreatePopupMenuL( const TPoint &aPosition )
    {
    delete iPopup;
    
    iPopup = CAknStylusPopUpMenu::NewL( this, aPosition );
    iPopup->AddMenuItemL( KMenuItem1, EMenuItem1 );
    iPopup->AddMenuItemL( KMenuItem2, EMenuItem2 );
    iPopup->AddMenuItemL( KMenuItem3, EMenuItem3 );
    iPopup->AddMenuItemL( KMenuItem4, EMenuItem4 );
    //iPopup->SetPosition( aPosition );
    
    HBufC* labelText = StringLoader::LoadLC( R_STYLUSPOPUPMENUEX_MENU_CREATED );
    iLabel->SetTextL( *labelText );
    CleanupStack::PopAndDestroy();
    DrawDeferred();
    
    iPopup->ShowMenu();
    }

void CStylusPopupMenuExContainer::ProcessCommandL( TInt aCommandId )
    {
        
    TBuf<32> buf;
    HBufC* labelText;
    switch( aCommandId )
        {
        case EMenuItem1:
            labelText = StringLoader::LoadLC( R_STYLUSPOPUPMENUEX_ITEM_SELECTED );
            buf.Format( *labelText, 1 );
            CleanupStack::PopAndDestroy(); // labelText
            iLabel->SetTextL( buf );
            break;
        case EMenuItem2:
            labelText = StringLoader::LoadLC( R_STYLUSPOPUPMENUEX_ITEM_SELECTED );
            buf.Format( *labelText, 2 );
            CleanupStack::PopAndDestroy(); // labelText
            iLabel->SetTextL( buf );
            break;
        case EMenuItem3:
            labelText = StringLoader::LoadLC( R_STYLUSPOPUPMENUEX_ITEM_SELECTED );
            buf.Format( *labelText, 3 );
            CleanupStack::PopAndDestroy(); // labelText
            iLabel->SetTextL( buf );
            break;
        case EMenuItem4:
            labelText = StringLoader::LoadLC( R_STYLUSPOPUPMENUEX_ITEM_SELECTED );
            buf.Format( *labelText, 4 );
            CleanupStack::PopAndDestroy(); // labelText
            iLabel->SetTextL( buf );
            break;
        case KErrCancel:
            labelText = StringLoader::LoadLC( R_STYLUSPOPUPMENUEX_POPUPMENU_CANCELLED );
            iLabel->SetTextL( *labelText );
            CleanupStack::PopAndDestroy(); // labelText
            break;
        }
    
    DrawDeferred();
    }
    
void CStylusPopupMenuExContainer::UpdateMarkableMenus( CEikMenuPane *aMenuPane )
    {
        aMenuPane->SetItemButtonState( ECmdCreateMenuFromCode, iUseResource ? EEikMenuItemSymbolIndeterminate:EEikMenuItemSymbolOn );
        aMenuPane->SetItemButtonState( ECmdCreateMenuFromResource, iUseResource ? EEikMenuItemSymbolOn:EEikMenuItemSymbolIndeterminate );
    }

/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include <aknutils.h> // for AKN_LAF_COLOR
#include <eikenv.h>
#include <eiklabel.h> // for label
#include <aknform.h>  // for form control
#include <eikseced.h> // for secret editor
#include <eikedwin.h> // for edwins
#include <IsvTelInfoApp.rsg>
#include "IsvTelInfoApp.hrh"
#include "IsvTelInfoAppView.h"
#include "IsvTelInfoAppContainer.h"
#include "IsvTelInfoAppDlg.h"

// MACROS
#define ISVINFOAPP_BACKGROUND_COLOR AKN_LAF_COLOR( 219 )

// CONSTANTS
const TInt KIsvTelInfoAppCountComponent = 1;
const TInt KIsvTelInfoAppMessageBufLength = 256;

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CIsvTelInfoAppContainer::CIsvTelInfoAppContainer( CIsvTelInfoAppView* )
// Overload constructor.
// ----------------------------------------------------------------------------
//
CIsvTelInfoAppContainer::CIsvTelInfoAppContainer( 
    CIsvTelInfoAppView* aView )
:   iView( aView ),
    iLabel( NULL )
    {
    }

// ----------------------------------------------------------------------------
// void CIsvTelInfoAppContainer::ConstructL( const TRect& )
// EPOC constructor.
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppContainer::ConstructL( const TRect& aRect )
    {
    // Creates window.
    CreateWindowL();

    // Creates label object.
    CreateLabelL();

    SetRect( aRect ); // Sets rectangle of frame.
    ActivateL(); // Activates the window. (Ready to draw)
    }

// ----------------------------------------------------------------------------
// CIsvTelInfoAppContainer::~CIsvTelInfoAppContainer()
// Destructor.
// ----------------------------------------------------------------------------
//
CIsvTelInfoAppContainer::~CIsvTelInfoAppContainer()
    {
    delete iLabel;
    }

// ----------------------------------------------------------------------------
// void CIsvTelInfoAppContainer::SetTextToLabelL( TInt )
//
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppContainer::SetTextToLabelL( TInt aResourceId )
    {
    TBuf<KIsvTelInfoAppMessageBufLength> messageString( NULL );
    iCoeEnv->ReadResource( messageString, aResourceId );
    iLabel->SetTextL( messageString );
    }

// ----------------------------------------------------------------------------
// void CIsvTelInfoAppContainer::DisplayFormL( TInt, TInt )
//
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppContainer::DisplayFormL( 
    TInt aResourceId,
    TInt aPageId)
    {
    CIsvTelInfoAppDlg* form = CIsvTelInfoAppDlg::NewL();
    form->SetTitlePane(iView->GetTitlePaneL());
    form->SetContainer(this);
    form->SetDlgPage(aPageId);
    form->ExecuteLD( aResourceId );
    }

// ----------------------------------------------------------------------------
// void CIsvTelInfoAppContainer::BringToForeGroundL( )
//
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppContainer::BringToForeGroundL( )
    {
    iView->ActivateViewL(iView->ViewId());
    }

// ----------------------------------------------------------------------------
// void CIsvTelInfoAppContainer::CreateLabelL()
// Creates label object.
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppContainer::CreateLabelL()
    {
    if ( !iLabel )
        {
        iLabel = new( ELeave ) CEikLabel;
        iLabel->SetContainerWindowL( *this );
        iLabel->SetAlignment( EHCenterVCenter );
#ifdef __AVKON_APAC__
        iLabel->SetFont( ApacPlain16() );
#else
        iLabel->SetFont( LatinBold19() );
#endif
        }
    SetTextToLabelL( R_ISVTELINFOAPP_TEXT_LABEL_TITLE );
    }

// ----------------------------------------------------------------------------
// TKeyResponse CIsvTelInfoAppContainer::OfferKeyEventL( const TKeyEvent&,
//  TEventCode )
// Handles the key events.
// ----------------------------------------------------------------------------
//
TKeyResponse CIsvTelInfoAppContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    if ( aType != EEventKey ) // Is not key event?
        {
        return EKeyWasNotConsumed;
        }
    switch ( aKeyEvent.iCode ) // The code of key event is...
        {
        default:
            return EKeyWasNotConsumed;
        }
    }

// ----------------------------------------------------------------------------
// void CIsvTelInfoAppContainer::SizeChanged()
// Called by framework when the view size is changed
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppContainer::SizeChanged()
    {
    iLabel->SetRect( Rect() );
    }

// ----------------------------------------------------------------------------
// TInt CIsvTelInfoAppContainer::CountComponentControls() const
// Returns number of component.
// ----------------------------------------------------------------------------
//
TInt CIsvTelInfoAppContainer::CountComponentControls() const
    {
    // returns nbr of controls inside this container
    return KIsvTelInfoAppCountComponent;
    }

// ----------------------------------------------------------------------------
// CCoeControl* CIsvTelInfoAppContainer::ComponentControl( TInt ) const
// Returns pointer to particular component.
// ----------------------------------------------------------------------------
//
CCoeControl* CIsvTelInfoAppContainer::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex ) // Component is...
        {
        case EIsvTelInfoAppComponentLabel: // Label object.
            return iLabel;
        default: // Exception : returns NULL value.
            return NULL;
        }
    }

// ----------------------------------------------------------------------------
// void CIsvTelInfoAppContainer::Draw( const TRect& ) const
// Clears the window's rectangle.
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppContainer::Draw( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushColor( ISVINFOAPP_BACKGROUND_COLOR );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.DrawRect( aRect );
    }

// End of File

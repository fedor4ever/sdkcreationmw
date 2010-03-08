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
#include <IsvTelCallApp.rsg>
#include "IsvTelCallApp.hrh"
#include "IsvTelCallAppView.h"
#include "IsvTelCallAppContainer.h"
#include "IsvTelCallAppDlg.h"

// MACROS
#define ISVTELCALLAPP_BACKGROUND_COLOR AKN_LAF_COLOR( 219 )

// CONSTANTS
const TInt KIsvTelCallAppCountComponent = 1;
const TInt KIsvTelCallAppMessageBufLength = 256;

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CIsvTelCallAppContainer::CIsvTelCallAppContainer( CIsvTelCallAppView* )
// Overload constructor.
// ----------------------------------------------------------------------------
//
CIsvTelCallAppContainer::CIsvTelCallAppContainer( 
    CIsvTelCallAppView* aView )
:   iView( aView ),
    iLabel( NULL )
    {
    }

// ----------------------------------------------------------------------------
// void CIsvTelCallAppContainer::ConstructL( const TRect& )
// EPOC constructor.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppContainer::ConstructL( const TRect& aRect )
    {
    // Creates window.
    CreateWindowL();

    // Creates label object.
    CreateLabelL();

    SetRect( aRect ); // Sets rectangle of frame.
    ActivateL(); // Activates the window. (Ready to draw)
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppContainer::~CIsvTelCallAppContainer()
// Destructor.
// ----------------------------------------------------------------------------
//
CIsvTelCallAppContainer::~CIsvTelCallAppContainer()
    {
    delete iLabel;
    }

// ----------------------------------------------------------------------------
// void CIsvTelCallAppContainer::SetTextToLabelL( TInt )
//
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppContainer::SetTextToLabelL( TInt aResourceId )
    {
    TBuf<KIsvTelCallAppMessageBufLength> messageString( NULL );
    iCoeEnv->ReadResource( messageString, aResourceId );
    iLabel->SetTextL( messageString );
    }

// ----------------------------------------------------------------------------
// void CIsvTelCallAppContainer::DisplayFormL( TInt, TInt )
//
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppContainer::DisplayFormL( 
    TInt aResourceId,
    TInt aPageId)
    {
    CIsvTelCallAppDlg* form = CIsvTelCallAppDlg::NewL();
    form->SetTitlePane(iView->GetTitlePaneL());
    form->SetContainer(this);
    form->SetDlgPage(aPageId);
    form->ExecuteLD( aResourceId );
    }

// ----------------------------------------------------------------------------
// void CIsvTelCallAppContainer::BringToForeGroundL( )
//
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppContainer::BringToForeGroundL( )
    {
    iView->ActivateViewL(iView->ViewId());
    }

// ----------------------------------------------------------------------------
// void CIsvTelCallAppContainer::CreateLabelL()
// Creates label object.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppContainer::CreateLabelL()
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
    SetTextToLabelL( R_ISVTELCALLAPP_TEXT_LABEL_TITLE );
    }

// ----------------------------------------------------------------------------
// TKeyResponse CIsvTelCallAppContainer::OfferKeyEventL( const TKeyEvent&,
//  TEventCode )
// Handles the key events.
// ----------------------------------------------------------------------------
//
TKeyResponse CIsvTelCallAppContainer::OfferKeyEventL(
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
// void CIsvTelCallAppContainer::SizeChanged()
// Called by framework when the view size is changed
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppContainer::SizeChanged()
    {
    iLabel->SetRect( Rect() );
    }

// ----------------------------------------------------------------------------
// TInt CIsvTelCallAppContainer::CountComponentControls() const
// Returns number of component.
// ----------------------------------------------------------------------------
//
TInt CIsvTelCallAppContainer::CountComponentControls() const
    {
    // returns nbr of controls inside this container
    return KIsvTelCallAppCountComponent;
    }

// ----------------------------------------------------------------------------
// CCoeControl* CIsvTelCallAppContainer::ComponentControl( TInt ) const
// Returns pointer to particular component.
// ----------------------------------------------------------------------------
//
CCoeControl* CIsvTelCallAppContainer::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex ) // Component is...
        {
        case EIsvTelCallAppComponentLabel: // Label object.
        	{
        	return iLabel;
        	}            
        default: // Exception : returns NULL value.
        	{
        	return NULL;
        	}
            
        }
    }

// ----------------------------------------------------------------------------
// void CIsvTelCallAppContainer::Draw( const TRect& ) const
// Clears the window's rectangle.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppContainer::Draw( const TRect& aRect ) const
    {
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushColor( ISVTELCALLAPP_BACKGROUND_COLOR );
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.DrawRect( aRect );
    }

// End of File

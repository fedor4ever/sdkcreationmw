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
#include <aknviewappui.h>
#include <eikmenup.h> // CEikMenupane
#include <avkon.hrh>
#include <SIPEx.rsg>
#include "SIPExGameView.h"
#include "SIPExGameContainer.h" 
#include "SIPExAppUi.h"
#include "SIPExDoc.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPExGameView::NewL
// EPOC two-phase construction
// -----------------------------------------------------------------------------
//
CSIPExGameView* CSIPExGameView::NewLC( CSIPExEngine& aEngine )
    {
    CSIPExGameView* self = new ( ELeave ) CSIPExGameView( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CSIPExGameView::CSIPExGameView
// C++ default constructor
// -----------------------------------------------------------------------------
//
CSIPExGameView::CSIPExGameView( CSIPExEngine& aEngine )
    : iEngine( aEngine )
    {
    }

// -----------------------------------------------------------------------------
// CSIPExGameView::ConstructL
// EPOC second-phased constructor
// -----------------------------------------------------------------------------
//
void CSIPExGameView::ConstructL()
    {
    BaseConstructL( R_SIP_EX_VIEW1 );
    }

// -----------------------------------------------------------------------------
// CSIPExGameView::~CSIPExGameView
// Default destructor
// -----------------------------------------------------------------------------
//
CSIPExGameView::~CSIPExGameView()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }

    delete iContainer;
    }

// -----------------------------------------------------------------------------
// TUid CSIPExGameView::Id
// This returns the view ID
// -----------------------------------------------------------------------------
//
TUid CSIPExGameView::Id() const
    {
    return KGameViewId;
    }
    
// -----------------------------------------------------------------------------
// CSIPExGameView::HandleCommandL
// Here we handle commands for this view. 
// -----------------------------------------------------------------------------
//
void CSIPExGameView::HandleCommandL( TInt aCommand )
    {   
    switch ( aCommand )
        {

        case EAknSoftkeyBack:
            {
            AppUi()->HandleCommandL( EEikCmdExit );
            break;
            }

        default:
            {
            AppUi()->HandleCommandL( aCommand );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CSIPExGameView::HandleViewRectChange()
// -----------------------------------------------------------------------------
//
void CSIPExGameView::HandleViewRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }
    
// -----------------------------------------------------------------------------
// CSIPExGameView::DynInitMenuPaneL
// Handles menu initialization event.
// -----------------------------------------------------------------------------
//
void CSIPExGameView::DynInitMenuPaneL( 
    TInt aResourceId, 
    CEikMenuPane* aMenuPane )
    {
    if ( aResourceId != R_SIP_EX_MENU )
        {
        // Not this view's menu, ignore
        return;
        }
        
    switch ( static_cast< CSIPExAppUi* >( AppUi() )->EngineState() )
        {
        case CSIPExEngine::EIdle:
            aMenuPane->SetItemDimmed( ECmdInviteForGame, ETrue );
            aMenuPane->SetItemDimmed( ECmdEnableProfile, EFalse );
            aMenuPane->SetItemDimmed( ECmdDisableProfile, ETrue );
            aMenuPane->SetItemDimmed( ECmdSendIM, ETrue );
            aMenuPane->SetItemDimmed( ECmdEndGame, ETrue );
            break;
        case CSIPExEngine::EEnabled:
            aMenuPane->SetItemDimmed( ECmdInviteForGame, EFalse );
            aMenuPane->SetItemDimmed( ECmdEnableProfile, ETrue );
            aMenuPane->SetItemDimmed( ECmdDisableProfile, EFalse );
            aMenuPane->SetItemDimmed( ECmdSendIM, EFalse );
            aMenuPane->SetItemDimmed( ECmdEndGame, ETrue );
            break;
        case CSIPExEngine::EActivating:
            aMenuPane->SetItemDimmed( ECmdInviteForGame, ETrue );
            aMenuPane->SetItemDimmed( ECmdEnableProfile, ETrue );
            aMenuPane->SetItemDimmed( ECmdDisableProfile, ETrue );
            aMenuPane->SetItemDimmed( ECmdSendIM, EFalse );
            aMenuPane->SetItemDimmed( ECmdEndGame, ETrue );
            break;
        case CSIPExEngine::EActive:
            aMenuPane->SetItemDimmed( ECmdInviteForGame, ETrue );
            aMenuPane->SetItemDimmed( ECmdEnableProfile, ETrue );
            aMenuPane->SetItemDimmed( ECmdDisableProfile, ETrue );
            aMenuPane->SetItemDimmed( ECmdSendIM, EFalse );
            aMenuPane->SetItemDimmed( ECmdEndGame, EFalse );
            break;
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CSIPExGameView::DoActivateL
// This is called when a view needs to be activated. 
// This creates container with its controls.
// It also receives messages sent to the view.
// -----------------------------------------------------------------------------
//
void CSIPExGameView::DoActivateL(
    const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
    const TDesC8& /*aCustomMessage*/ )
    {
    if ( !iContainer )
        {
        iContainer = new ( ELeave ) CSIPExGameContainer( iEngine );
        iContainer->SetMopParent( this );
        iContainer->ConstructL( ClientRect() );
        AppUi()->AddToStackL( *this, iContainer );
        }
   }

// -----------------------------------------------------------------------------
// CSIPExGameView::HandleCommandL
// This is called when a view needs to be deactivated. 
// This destroys container with its controls.
// -----------------------------------------------------------------------------
//
void CSIPExGameView::DoDeactivate()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }
    
    delete iContainer;
    iContainer = NULL;
    }
    
// -----------------------------------------------------------------------------
// CSIPExGameView::Container
// -----------------------------------------------------------------------------
//
CSIPExGameContainer* CSIPExGameView::Container()
    {
    return iContainer;
    }

// End of File


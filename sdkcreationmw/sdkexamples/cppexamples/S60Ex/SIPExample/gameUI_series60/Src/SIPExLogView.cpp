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
#include "SIPExLogView.h"
#include "SIPExLogContainer.h" 
#include "SIPExAppUi.h"
#include "SIPExGameEngine.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPExLogView::NewL
// EPOC two-phase construction
// -----------------------------------------------------------------------------
//
CSIPExLogView* CSIPExLogView::NewLC( CSIPExEngine& aEngine )
    {
    CSIPExLogView* self = new ( ELeave ) CSIPExLogView( aEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
    
// -----------------------------------------------------------------------------
// CSIPExLogView::CSIPExLogView
// C++ default constructor
// -----------------------------------------------------------------------------
//
CSIPExLogView::CSIPExLogView( CSIPExEngine& aEngine )
    : iEngine( aEngine )
    {
    // no implementation required
    }
    
// -----------------------------------------------------------------------------
// CSIPExLogView::ConstructL
// EPOC second-phased constructor
// -----------------------------------------------------------------------------
//
void CSIPExLogView::ConstructL()
    {
    BaseConstructL( R_SIP_EX_VIEW2 );
    }

// -----------------------------------------------------------------------------
// CSIPExLogView::~CSIPExLogView
// Default destructor
// -----------------------------------------------------------------------------
//
CSIPExLogView::~CSIPExLogView()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }

    delete iContainer;
    }

// -----------------------------------------------------------------------------
// TUid CSIPExLogView::Id
// This returns the view ID
// -----------------------------------------------------------------------------
//
TUid CSIPExLogView::Id() const
    {
    return KLogViewId;
    }

// -----------------------------------------------------------------------------
// CSIPExLogView::HandleCommandL
// Here we handle commands for this view. 
// Each view has their own HandleCommandL()
// -----------------------------------------------------------------------------
//
void CSIPExLogView::HandleCommandL( TInt aCommand )
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
// CSIPExLogView::HandleViewRectChange
// -----------------------------------------------------------------------------
//
void CSIPExLogView::HandleViewRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        }
    }
    
// -----------------------------------------------------------------------------
// CSIPExLogView::DynInitMenuPaneL
// Handles menu initialization event.
// -----------------------------------------------------------------------------
//
void CSIPExLogView::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    if ( aResourceId != R_SIP_EX_MENU )
        {
        // Not this view's menu, ignore
        return;
        }
        
    switch( static_cast< CSIPExAppUi* >( AppUi() )->EngineState() )
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
// CSIPExLogView::DoActivateL
// This is called when a view needs to be activated. 
// This creates container with its controls.
// -----------------------------------------------------------------------------
//
void CSIPExLogView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/ )
    {
   
    // Create Container
    if (!iContainer)
        {
        iContainer = new (ELeave) CSIPExLogContainer( iEngine );
        iContainer->SetMopParent( this );
        iContainer->ConstructL( ClientRect() );
        AppUi()->AddToStackL( *this, iContainer );
        }
   }

// -----------------------------------------------------------------------------
// CSIPExLogView::HandleCommandL
// This is called when a view needs to be deactivated. 
// This destroys container with its controls.
// -----------------------------------------------------------------------------
//
void CSIPExLogView::DoDeactivate()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }
    
    delete iContainer;
    iContainer = NULL;
    }
    
// -----------------------------------------------------------------------------
// CSIPExLogView::Container
// -----------------------------------------------------------------------------
//
CSIPExLogContainer* CSIPExLogView::Container()
    {
    return iContainer;
    }

// End of File


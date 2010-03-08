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
#include <avkon.hrh>
#include <akntitle.h>
#include <aknviewappui.h>
#include <IsvTelInfoApp.rsg>

#include <eikmfne.h>
#include <eikedwin.h>
#include <eikcapc.h>

#include "IsvTelInfoApp.hrh"
#include "IsvTelInfoAppView.h"
#include "IsvTelInfoAppContainer.h"

// CONSTANTS
const TUid KViewId = { 1 }; // UID of view

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CIsvTelInfoAppView::CIsvTelInfoAppView()
// Default constructor.
// ----------------------------------------------------------------------------
//
CIsvTelInfoAppView::CIsvTelInfoAppView()
    :   iContainer( NULL )
    {
    }

// ----------------------------------------------------------------------------
// void CIsvTelInfoAppView::ConstructL()
// A Second phase constructor
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppView::ConstructL()
    {
    BaseConstructL( R_ISVTELINFOAPP_VIEW );
    iContainer = new( ELeave ) CIsvTelInfoAppContainer( this );
    }

// ----------------------------------------------------------------------------
// CIsvTelInfoAppView::~CIsvTelInfoAppView()
// Destructor.
// ----------------------------------------------------------------------------
//
CIsvTelInfoAppView::~CIsvTelInfoAppView()
    {
    // Deletes the container class object.
    if ( iContainer )
        {
        // Removes container from view control stack.
        AppUi()->RemoveFromStack( iContainer );
        }
    delete iContainer;
    }

// ----------------------------------------------------------------------------
// TUid CIsvTelInfoAppView::Id()
// Returns ID of view.
// ----------------------------------------------------------------------------
//
TUid CIsvTelInfoAppView::Id() const
    {
    return KViewId;
    }

// ----------------------------------------------------------------------------
// void CIsvTelInfoAppView::HandleCommandL( TInt )
// Handles the commands.
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppView::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        /*case EAknSoftkeyBack and EAknCmdExitare handled by the AppUi class.*/        
        case EIsvTelInfoAppCmdLaunchPhOps:
            iContainer->DisplayFormL( R_ISVTELINFOAPP_NAVIPANE_DIALOG,
                EIsvTelInfoAppPageCtrlIdPagePhoneInfo);
            break;
        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
// CAknTitlePane* CIsvTelInfoAppView::GetTitlePaneL(void)
// ----------------------------------------------------------------------------
//
CAknTitlePane* CIsvTelInfoAppView::GetTitlePaneL(void)
    {
    CAknTitlePane* titlePane = STATIC_CAST(
    CAknTitlePane*,
    StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );

    return(titlePane);
    }

// ----------------------------------------------------------------------------
// void CIsvTelInfoAppView::DoActivateL( const TVwsViewId&, TUid, const TDesC8& )
// Creates container class object.
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppView::DoActivateL(
    const TVwsViewId& /*aPrevViewId*/,
    TUid /*aCustomMessageId*/,
    const TDesC8& /*aCustomMessage*/ )
    {
    // Creates container object.
    if ( iContainer )
        {
        iContainer->SetMopParent(this);
        iContainer->ConstructL( ClientRect() );
        // Adds container to view control stack.
        AppUi()->AddToStackL( *this, iContainer );
        }
    }

// ----------------------------------------------------------------------------
// void CIsvTelInfoAppView::DoDeactivate()
// Deletes container class object.
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppView::DoDeactivate()
    {
    // Deletes the container class object.
    if ( iContainer )
        {
        // Removes container from view control stack.
        AppUi()->RemoveFromStack( iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    }

// End of File

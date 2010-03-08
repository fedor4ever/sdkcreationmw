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
#include <IsvTelCallApp.rsg>

#include <eikmfne.h>
#include <eikedwin.h>
#include <eikcapc.h>

#include "IsvTelCallApp.hrh"
#include "IsvTelCallAppView.h"
#include "IsvTelCallAppContainer.h"

// CONSTANTS
const TUid KViewId = { 1 }; // UID of view

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CIsvTelCallAppView::CIsvTelCallAppView()
// Default constructor.
// ----------------------------------------------------------------------------
//
CIsvTelCallAppView::CIsvTelCallAppView()
:   iContainer( NULL )
    {
    }

// ----------------------------------------------------------------------------
// void CIsvTelCallAppView::ConstructL()
// A Second phase constructor
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppView::ConstructL()
    {
    BaseConstructL( R_ISVTELCALLAPP_VIEW );
    iContainer = new( ELeave ) CIsvTelCallAppContainer( this );
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppView::~CIsvTelCallAppView()
// Destructor.
// ----------------------------------------------------------------------------
//
CIsvTelCallAppView::~CIsvTelCallAppView()
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
// TUid CIsvTelCallAppView::Id()
// Returns ID of view.
// ----------------------------------------------------------------------------
//
TUid CIsvTelCallAppView::Id() const
    {
    return KViewId;
    }

// ----------------------------------------------------------------------------
// void CIsvTelCallAppView::HandleCommandL( TInt )
// Handles the commands.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppView::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        //case EAknSoftkeyBack and EAknCmdExit are handled by the AppUi class.
        case EIsvTelCallAppCmdLaunchPhOps:
        	{
        	iContainer->DisplayFormL( R_ISVTELCALLAPP_NAVIPANE_DIALOG,
                EIsvTelCallAppPageCtrlIdPagePhone1);
        	}
            break;
        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
// CAknTitlePane* CIsvTelCallAppView::GetTitlePaneL(void)
// Gets the title of pane
// ----------------------------------------------------------------------------
//
CAknTitlePane* CIsvTelCallAppView::GetTitlePaneL(void)
    {
    CAknTitlePane* titlePane = STATIC_CAST(
    CAknTitlePane*,
    StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );

    return(titlePane);
    }

// ----------------------------------------------------------------------------
// void CIsvTelCallAppView::DoActivateL( const TVwsViewId&, TUid, const TDesC8& )
// Creates container class object.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppView::DoActivateL(
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
// void CIsvTelCallAppView::DoDeactivate()
// Deletes container class object.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppView::DoDeactivate()
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

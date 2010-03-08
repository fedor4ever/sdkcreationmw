/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of UI class
*
*/


// INCLUDE FILES
#include <aknnotewrappers.h>
#include <tactileexample.rsg>
#include <touchfeedback.h>
#include "tactileexampleappui.h"
#include "tactileexampleappview.h"



// ============================ MEMBER FUNCTIONS =============================


// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CTactileExampleAppUi::CTactileExampleAppUi()
    {
    // No implementation required
    }


// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CTactileExampleAppUi::ConstructL()
    {
    // Initialise app UI with standard value.
    BaseConstructL( CAknAppUi::EAknEnableSkin );


    // Create view object
    iAppView = CTactileExampleAppView::NewL( ClientRect() );
    // We need to set the object provider parent for the view, so that
    // automatic feedback area disabling works for dimmed and invisible
    // controls.
    iAppView->SetMopParent( this );
    AddToStackL( iAppView );
    }


// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CTactileExampleAppUi::~CTactileExampleAppUi()
    {
    RemoveFromStack( iAppView );
    if ( iAppView )
        {
        delete iAppView;
        iAppView = NULL;
        }
    }


// ---------------------------------------------------------------------------
// From class CEikAppUi.
// Takes care of command handling.
// ---------------------------------------------------------------------------
//
void CTactileExampleAppUi::HandleCommandL( TInt aCommand )
    {
    switch( aCommand )
        {
        case EAknSoftkeyBack:
        case EAknSoftkeyExit:
        case EEikCmdExit:
            Exit();
            break;

        default:
            break;
        }
    }


// ---------------------------------------------------------------------------
// From class CEikAppUi.
// Called by framework when layout is changed.
// Passes the new client rectangle to the AppView
// ---------------------------------------------------------------------------
//
void CTactileExampleAppUi::HandleResourceChangeL( TInt aType )
    {
    // base-class call also
    CAknAppUi::HandleResourceChangeL( aType );
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        if ( iAppView )
            {
            iAppView->SetRect( ClientRect() );
            }
        }
    }
// End of File


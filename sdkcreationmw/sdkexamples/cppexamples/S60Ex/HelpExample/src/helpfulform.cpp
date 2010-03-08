/*
* Copyright (c) 2004 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include <AknAppUi.h>
#include <Avkon.rsg>
#include <eikmenup.h>

#include "HelpfulForm.h"
#include "HelpExampleApplication.h"
#include "HelpExample.hrh"
#include "HelpExample.hlp.hrh"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHelpfulForm::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHelpfulForm* CHelpfulForm::NewL( TInt aMenuResource )
    {
    CHelpfulForm* self = NewLC( aMenuResource );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CHelpfulForm::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHelpfulForm* CHelpfulForm::NewLC( TInt aMenuResource )
    {
    CHelpfulForm* self = new ( ELeave ) CHelpfulForm();
    CleanupStack::PushL( self );
    self->ConstructL( aMenuResource );
    return self;
    }

// -----------------------------------------------------------------------------
// CHelpfulForm::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHelpfulForm::ConstructL( TInt aMenuResource )
    {
    // Ensure the base class construction is complete or some of the form's
    // features will not work
    CAknForm::ConstructL( aMenuResource );
    }

// -----------------------------------------------------------------------------
// CHelpfulForm::GetHelpContext()
// Gets the control's help context. Returns a NULL context by default.
// -----------------------------------------------------------------------------
//
void CHelpfulForm::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    // Get any special help context for the control with focus, else a default
    aContext.iMajor = KUidHelpFile;

    switch ( IdOfFocusControl() )
        {
        case EHelpExampleEdwin1:
            // Provide special help context if first control has focus
            aContext.iContext = KContextEdwin1;
            break;

        case EHelpExampleEdwin2:
            // Provide special help context if second control has focus
            aContext.iContext = KContextEdwin2;
            break;

        default:
            // Provide default help context for this dialog otherwise
            aContext.iContext = KContextHelpfulForm;
            break;
        }
    }

// -----------------------------------------------------------------------------
// CHelpfulForm::ProcessCommandL
// Processes user commands.
// -----------------------------------------------------------------------------
//
void CHelpfulForm::ProcessCommandL( TInt aCommandId )
    {
    switch ( aCommandId )
        {
        case ELaunchHelpForContext :
            // Pass it back to the AppUi to deal with
            iAvkonAppUi->HandleCommandL( aCommandId );
            break;
        default :
            break;
        }

    // Need to do this in all cases or menu does not disappear after
    // a selection is made
    CAknForm::ProcessCommandL( aCommandId );
    }

// -----------------------------------------------------------------------------
// CHelpfulForm::DynInitMenuPaneL
// Dynamically initialises a menu pane.
// -----------------------------------------------------------------------------
//
void CHelpfulForm::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    CAknForm::DynInitMenuPaneL( aResourceId, aMenuPane );
    if ( aResourceId == R_AVKON_FORM_MENUPANE )
        {
        // Override the default form options. N/A in this case
        aMenuPane->SetItemDimmed( EAknFormCmdAdd, ETrue );
        aMenuPane->SetItemDimmed( EAknFormCmdSave, ETrue );
        aMenuPane->SetItemDimmed( EAknFormCmdLabel, ETrue );
        aMenuPane->SetItemDimmed( EAknFormCmdDelete, ETrue );
        }
    }

// End of File
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
#include "RegistrationAppui.h"
#include "RegistrationView.h"

// ============================ MEMBER FUNCTIONS ==============================
//
// ----------------------------------------------------------------------------
// CRegistrationAppUi::ConstructL()
// 
// ----------------------------------------------------------------------------
//
void CRegistrationAppUi::ConstructL()
    {
    BaseConstructL( CAknAppUi::EAknEnableSkin );
    
    iAppView = CRegistrationView::NewL();
    CleanupStack::PushL( iAppView );
    AddViewL( iAppView );
    CleanupStack::Pop( iAppView );
    ActivateLocalViewL( iAppView->Id() );
    }

// ----------------------------------------------------------------------------
// CRegistrationAppUi::~CRegistrationAppUi()
// Destructor
// Frees reserved resources
// ----------------------------------------------------------------------------
//
CRegistrationAppUi::~CRegistrationAppUi()
    { 
   	}

// ----------------------------------------------------------------------------
// CRegistrationAppUi::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* 
// aMenuPane)
// This function is called by the EIKON framework just before it displays
// a menu pane. Its default implementation is empty, and by overriding it,
// the application can set the state of menu items dynamically according
// to the state of application data.
// ----------------------------------------------------------------------------
//
void CRegistrationAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {
    // No implementation required
    }

// ----------------------------------------------------------------------------
// CRegistrationAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
// takes care of key event handling
// ----------------------------------------------------------------------------
//
TKeyResponse CRegistrationAppUi::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/,TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------------------------------
// CRegistrationAppUi::HandleCommandL( TInt aCommand )
// takes care of command handling
// ----------------------------------------------------------------------------
//
void CRegistrationAppUi::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EAknSoftkeyExit:
        case EEikCmdExit:
            {
            Exit();
            break;
            }
        case ERegistrationCmdAppRegister:
            { 
            if ( iAppView->ValidateInput() )
              {       
              // Add here your own code to handle registration data.
              //
              iCoeEnv->ReadResourceL( iNote, R_REGISTRATION_INFO_TEXT );
              CAknInformationNote* dlg = new ( ELeave ) CAknInformationNote;
              dlg->ExecuteLD( iNote );
              }
            else
              {
              iCoeEnv->ReadResourceL( iNote, R_REGISTRATION_ERR_TEXT );
              CAknErrorNote* dlg = new ( ELeave ) CAknErrorNote;
              dlg->ExecuteLD( iNote );  
              }
            break;
            }

        default:
            break;      
        }
    }

// ----------------------------------------------------------------------------
// CRegistrationAppUi::HandleResourceChangeL( TInt aType )
// Called by framework when layout is changed.
// ----------------------------------------------------------------------------
//
void CRegistrationAppUi::HandleResourceChangeL( TInt aType )
    {
    CAknAppUi::HandleResourceChangeL( aType );
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {		
        iAppView->SizeChanged();
        }
    }

// End of File  

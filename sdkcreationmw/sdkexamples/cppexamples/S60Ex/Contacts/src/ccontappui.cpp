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

#include <contacts.rsg>
#include <avkon.hrh>
#include <eikmenup.h>
#include <eikmenub.h>
#include <akntitle.h>
#include <stringloader.h>

#include "CContAppUi.h"
#include "CContContainer.h"
#include "contacts.hrh"


// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CContAppUi::ConstructL()
// Two phased constructor
// ----------------------------------------------------------
//
void CContAppUi::ConstructL()
    {
    BaseConstructL(EAknEnableSkin);

    iAppContainer = new (ELeave) CContContainer;
    iAppContainer->SetMopParent( this );
    iAppContainer->ConstructL( ClientRect() );
    AddToStackL( iAppContainer );
    }

// ----------------------------------------------------
// CContAppUi::~CContAppUi()
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CContAppUi::~CContAppUi()
    {
    if ( iAppContainer )
        {
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
        }
   }

// ------------------------------------------------------------------------------
// CContAppUi::::DynInitMenuPaneL()
// This function is called by the EIKON framework just before it displays
// a menu pane. Its default implementation is empty, and by overriding it,
// the application can set the state of menu items dynamically according
// to the state of application data.
// ------------------------------------------------------------------------------
//
void CContAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/)
    {

    }

// ----------------------------------------------------
// CContAppUi::HandleKeyEventL()
// Handles key events
// ----------------------------------------------------
//
TKeyResponse CContAppUi::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------
// CContAppUi::HandleCommandL()
// Handles user commands.
// ----------------------------------------------------
//
void CContAppUi::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        // flowtrough
        case EAknSoftkeyExit:
        case EEikCmdExit:
            Exit();
            break;
		case EContactsDefaultDB:
            iAppContainer->OpenDefaultDatabaseL();
            if ( iAppContainer->IfContactDbSelected() )
                {
                UpdateTitleL( R_CONTACTS_DEFAULT_DB );
                }
            else
                {
                UpdateTitleL( R_CONTACTS_NO_DB );
                }
            break;
		case EContactsCreateTemplate:
            iAppContainer->CreateContactTemplateL();
            break;
		case EContactsCreateContact:
            iAppContainer->AddNewContactDlgL();
            break;
		case EContactsDeleteContact:
            iAppContainer->DeleteContactDlgL();
            break;
		case EContactsEditContact:
            iAppContainer->EditContactDlgL();
            break;
		case EContactsSelectGrp:
            if ( !iAppContainer->CanAddContactToGroupL() )
                {
                return;
                }
            iAppContainer->GetSelectedContactL();
            CEikonEnv::Static()->AppUiFactory()->
                MenuBar()->SetMenuTitleResourceId( R_CONTACTS_SELECT_GRP_MENUBAR );
            break;
		case EContactsAddContactToGrp:
            iAppContainer->AddContactToGrpL();
            // Show contact on the listbox
            iAppContainer->ShowContactsL();
            // Change menu
            CEikonEnv::Static()->AppUiFactory()->
                MenuBar()->SetMenuTitleResourceId( R_CONTACTS_CONTACTS_MENUBAR );
            break;
		case EContactsCancelSelectGrp:
            iAppContainer->ShowContactsL();
            // Change menu back to original
            CEikonEnv::Static()->AppUiFactory()->
                MenuBar()->SetMenuTitleResourceId( R_CONTACTS_CONTACTS_MENUBAR );
            break;
		case EContactsShowContacts:
            iAppContainer->ShowContactsL();
            CEikonEnv::Static()->AppUiFactory()->
                MenuBar()->SetMenuTitleResourceId( R_CONTACTS_CONTACTS_MENUBAR );
            break;
		case EContactsShowGroups:
            {
            iAppContainer->ShowContactGroupsL();
            CEikonEnv::Static()->AppUiFactory()->
                MenuBar()->SetMenuTitleResourceId( R_CONTACTS_GROUPS_MENUBAR );
            }
            break;
		case EContactsCreateContactGroup:
            iAppContainer->CreateContactGroupL();
            break;
		case EContactsDeleteContactGroup:
            iAppContainer->DeleteContactGroupL();
            break;
        default:
            break;
        }
    }

// ----------------------------------------------------
// CContAppUi::UpdateTitleL()
// Update title
// ----------------------------------------------------
//
void CContAppUi::UpdateTitleL( TInt aResourceId )
    {
    HBufC* textResource = StringLoader::LoadLC( aResourceId );

    CAknTitlePane* titlePane = STATIC_CAST( CAknTitlePane*,
            StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );

    titlePane->SetTextL( textResource->Des());
    CleanupStack::PopAndDestroy( textResource );
    }

// ----------------------------------------------------------------------------
// void CContAppUi::HandleResourceChangeL()
// Handles a resource change
// ----------------------------------------------------------------------------
//
void CContAppUi::HandleResourceChangeL( TInt aType )
    {
    CAknAppUi::HandleResourceChangeL( aType );
    if  ( aType == KEikDynamicLayoutVariantSwitch )
    	{
        iAppContainer->SetRect( ClientRect() );
        }
    }

// End of File

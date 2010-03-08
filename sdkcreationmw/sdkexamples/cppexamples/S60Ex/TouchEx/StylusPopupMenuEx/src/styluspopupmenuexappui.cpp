/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include <styluspopupmenuexample.rsg>
#include <aknstyluspopupmenu.h>

#include "styluspopupmenuexappui.h"
#include "styluspopupmenuex.hrh"
#include "styluspopupmenuexcontainer.h"


// ============================ MEMBER FUNCTIONS ===============================
    
// -----------------------------------------------------------------------------
// CStylusPopupMenuExAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CStylusPopupMenuExAppUi::ConstructL()
    {
    BaseConstructL( EAknEnableSkin );
    
    iAppContainer = new ( ELeave ) CStylusPopupMenuExContainer();
    iAppContainer->SetMopParent( this );
    iAppContainer->ConstructL( ClientRect() );
    
    AddToStackL( iAppContainer );
    }

// -----------------------------------------------------------------------------
// CStylusPopupMenuExAppUi::CStylusPopupMenuExAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CStylusPopupMenuExAppUi::CStylusPopupMenuExAppUi()                              
    {
	// no implementation required
    }

// -----------------------------------------------------------------------------
// CStylusPopupMenuExAppUi::~CStylusPopupMenuExAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CStylusPopupMenuExAppUi::~CStylusPopupMenuExAppUi()
    {
    if ( iAppContainer )
        {
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
        }
    }

// -----------------------------------------------------------------------------
// CStylusPopupMenuExAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CStylusPopupMenuExAppUi::HandleCommandL( TInt aCommand )
    {
    
    switch( aCommand )
        {
        case ECmdCreateMenuFromResource:
            iAppContainer->UseResourceForPopupMenuL( ETrue );
            break;
        case ECmdCreateMenuFromCode:
            iAppContainer->UseResourceForPopupMenuL( EFalse );
            break;
        case EEikCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;
        default:
            return;
        }
    //iAppContainer->DrawDeferred();        
        }
void CStylusPopupMenuExAppUi::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane *aMenuPane)
    {
    //aMenuPane->SetItemButtonState( ECmdCreateMenuFromResource, iUseResource );
    if ( aResourceId == R_STYLUSPOPUPMENUEX_MENU_CASCADE )
        {
        iAppContainer->UpdateMarkableMenus( aMenuPane );
        }
    }

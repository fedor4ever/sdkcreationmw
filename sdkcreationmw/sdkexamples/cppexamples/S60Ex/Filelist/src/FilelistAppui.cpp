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
#include "FilelistAppui.h"
#include "FilelistContainer.h" 
#include <Filelist.rsg>
#include "Filelist.hrh"

#include <avkon.hrh>

// ================= MEMBER FUNCTIONS ==========================================

// -----------------------------------------------------------------------------
// CFilelistAppUi::ConstructL()
// Symbian two phased constructor
// -----------------------------------------------------------------------------
//
void CFilelistAppUi::ConstructL()
    {
    BaseConstructL( CAknAppUi::EAknEnableSkin );
    iAppContainer = CFilelistContainer::NewL( ClientRect() );
    AddToStackL( iAppContainer );
    }

// -----------------------------------------------------------------------------
// CFilelistAppUi::~CFilelistAppUi()
// Destructor
// Frees reserved resources
// -----------------------------------------------------------------------------
//
CFilelistAppUi::~CFilelistAppUi()
    {
    if ( iAppContainer )
        {
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
        iAppContainer = NULL;
        }
   }

// -----------------------------------------------------------------------------
// CFilelistAppUi::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CFilelistAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {
    // no implementation required 
    }

// -----------------------------------------------------------------------------
// CFilelistAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
// takes care of key event handling
// -----------------------------------------------------------------------------
//
TKeyResponse CFilelistAppUi::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/,TEventCode /*aType*/ )
    {
    return EKeyWasNotConsumed;
    }

// -----------------------------------------------------------------------------
// CFilelistAppUi::HandleCommandL(TInt aCommand)
// takes care of command handling
// -----------------------------------------------------------------------------
//
void CFilelistAppUi::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EAknSoftkeyExit:
        case EEikCmdExit:
            {
            Exit();
            break;
            }
        case EFilelistCmdAppPrivFolder1:
            {
            iAppContainer->ChangePrivateFolderL( EPrivateFolder1 );
            break;
            }
       case EFilelistCmdAppPrivFolder2:
            {
            iAppContainer->ChangePrivateFolderL( EPrivateFolder2 );
            break;
            }
        case EFilelistCmdAppProtFolder:
            {
            iAppContainer->ChangeProtectedFolderL();
            break;
            }
        default:
            break;      
        }
    }

// -----------------------------------------------------------------------------
// CFilelistAppUi::HandleResourceChangeL( TInt aType )
// Called by framework when layout is changed.
// -----------------------------------------------------------------------------
//
void CFilelistAppUi::HandleResourceChangeL( TInt aType )
    {
    CAknAppUi::HandleResourceChangeL( aType );

    // ADDED FOR SCALABLE UI SUPPORT
    // *****************************
    if ( aType==KEikDynamicLayoutVariantSwitch )
        {
        iAppContainer->SetRect( ClientRect() );
        }
    //Controls derived from CCoeControl, handled in container class
    iAppContainer->HandleResourceChange( aType );
    }
// End of File  

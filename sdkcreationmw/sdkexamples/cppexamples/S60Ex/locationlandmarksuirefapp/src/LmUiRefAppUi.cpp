/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include "LmUiRefAppDocument.h"
#include "LmUiRefAppUi.h"
#include <lmuirefapp.rsg>
#include "lmuirefapp.hrh"
#include <avkon.hrh>
#include <AknViewAppUi.h>
#include "LmUiRefAppView.h"
#include <AknsUtils.h>




// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CLmUiRefAppUi::ConstructL() 
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------
//
void CLmUiRefAppUi::ConstructL()
    {
    BaseConstructL( EAknEnableSkin | EAknEnableMSK);
    CLmUiRefAppView* lmView =   CLmUiRefAppView::NewLC();
    AddViewL(lmView); // Transfer Ownership
    CleanupStack::Pop();
    SetDefaultViewL(*lmView);
    }

// ----------------------------------------------------
// CLmUiRefAppUi::~CLmUiRefAppUi:()
// Destructor Frees reserved resources
// ----------------------------------------------------
//
CLmUiRefAppUi::~CLmUiRefAppUi()
   {

   }

//------------------------------------------------------------------------------
// CLmUiRefAppUi::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
//------------------------------------------------------------------------------
//
void CLmUiRefAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {
    }

// ----------------------------------------------------
// CLmUiRefAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
// Handles key events. It is called by HandleWsEventL()
// when a key press event occurs
// ----------------------------------------------------
//
TKeyResponse CLmUiRefAppUi::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/,TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------
// CLmkAppUi::Document
// ----------------------------------------------------
//
CLmUiRefAppDocument& CLmUiRefAppUi::Document() const
    {
    // Explicit cast: the document must always be of type CLmkDocument
    CLmUiRefAppDocument* document =
        static_cast<CLmUiRefAppDocument*>( CEikAppUi::Document() );
    
    return *document;
    }


// ----------------------------------------------------
// CLmUiRefAppUi::HandleCommandL(TInt aCommand)
// Handles commands defined in resource files
// ----------------------------------------------------
//
void CLmUiRefAppUi::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        // these all are same: exit
        case EAknSoftkeyBack:
        case EAknSoftkeyExit:
        case EEikCmdExit:
            {
	        Exit();
	        break;
            }
        default:
            break;      
        }
    }

// End of File  

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
#include "MultiTexAppUi.h"
#include "MultiTexContainer.h"
#include "multitex.hrh"

#include <MultiTex.rsg>
#include <avkon.hrh>
#include <eikmenup.h>

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CMultiTexAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------
//
void CMultiTexAppUi::ConstructL()
{
    BaseConstructL();

    iAppContainer = new (ELeave) CMultiTexContainer;
    iAppContainer->SetMopParent(this);

    iAppContainer->ConstructL( ClientRect(), this );
    AddToStackL( iAppContainer );
}

// ----------------------------------------------------
// CMultiTexAppUi::~CMultiTexAppUi()
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CMultiTexAppUi::~CMultiTexAppUi()
{
    if ( iAppContainer )
    {
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
    }
}

// ------------------------------------------------------------------------------
// CMultiTexAppUi::::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CMultiTexAppUi::DynInitMenuPaneL(
  TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
{
}

// ----------------------------------------------------
// CMultiTexAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
// Key event handler
// ----------------------------------------------------
//
TKeyResponse CMultiTexAppUi::HandleKeyEventL(
  const TKeyEvent& /*aKeyEvent*/ ,TEventCode /*aType*/)
{
    return EKeyWasNotConsumed;
}

// ----------------------------------------------------
// CMultiTexAppUi::HandleCommandL(TInt aCommand)
// Command handler
// ----------------------------------------------------
//
void CMultiTexAppUi::HandleCommandL(TInt aCommand)
{
    switch ( aCommand )
    {
        case EAknSoftkeyBack:
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

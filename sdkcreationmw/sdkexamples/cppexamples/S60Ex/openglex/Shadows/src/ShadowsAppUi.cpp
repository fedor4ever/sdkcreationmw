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
#include "ShadowsAppUi.h"
#include "ShadowsContainer.h"
#include <Shadows.rsg>
#include "shadows.hrh"

#include <avkon.hrh>

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CShadowsAppUi::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------
//
void CShadowsAppUi::ConstructL()
    {
    BaseConstructL();
    iAppContainer = new (ELeave) CShadowsContainer;
    iAppContainer->SetMopParent(this);
    iAppContainer->ConstructL( ClientRect(), this );
    AddToStackL( iAppContainer );
    }

// Destructor
CShadowsAppUi::~CShadowsAppUi()
    {
    if ( iAppContainer )
        {
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
        }
   }

// ------------------------------------------------------------------------------
// CShadowsAppUi::DynInitMenuPaneL
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CShadowsAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {
    }

// ----------------------------------------------------
// CShadowsAppUi::HandleKeyEventL
// Key event handler
// ----------------------------------------------------
//
TKeyResponse CShadowsAppUi::HandleKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    if ( aType == EEventKeyDown )
        {
        TUint scan = aKeyEvent.iScanCode;

        switch ( scan )
            {
            case(14):
                iAppContainer->iShadows->RotateLightLeft();
                return EKeyWasConsumed;
            case(15):
                iAppContainer->iShadows->RotateLightRight();
                return EKeyWasConsumed;
            default:
                return EKeyWasNotConsumed;
            }
        }
    else if ( aType == EEventKeyUp )
        {
        TUint scan = aKeyEvent.iScanCode;

        switch ( scan )
            {
            case(14):
                iAppContainer->iShadows->RotateLightLeft();
                return EKeyWasConsumed;
            case(15):
                iAppContainer->iShadows->RotateLightRight();
                return EKeyWasConsumed;
            default:
                return EKeyWasNotConsumed;
            }
        }
    else
        {
        return EKeyWasNotConsumed;
        }
    }

// ----------------------------------------------------
// CShadowsAppUi::HandleCommandL
// Command handler
// ----------------------------------------------------
//
void CShadowsAppUi::HandleCommandL(TInt aCommand)
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

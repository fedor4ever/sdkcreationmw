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
#include "PickSelectAppUi.h"
#include "PickSelectContainer.h"
#include <PickSelect.rsg>
#include "pickselect.hrh"

#include <avkon.hrh>

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CPickSelectAppUi::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------
//
void CPickSelectAppUi::ConstructL()
    {
    BaseConstructL();
    iAppContainer = new (ELeave) CPickSelectContainer;
    iAppContainer->SetMopParent(this);
    iAppContainer->ConstructL( ClientRect(), this );
    AddToStackL( iAppContainer );
    }

// Destructor
CPickSelectAppUi::~CPickSelectAppUi()
    {
    if ( iAppContainer )
        {
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
        }
   }

// ------------------------------------------------------------------------------
// CPickSelectAppUi::DynInitMenuPaneL
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CPickSelectAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {
    }

// ----------------------------------------------------
// CPickSelectAppUi::HandleKeyEventL
// Key event handler
// ----------------------------------------------------
//
TKeyResponse CPickSelectAppUi::HandleKeyEventL(
    const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    if ( aType == EEventKeyDown )
        {
        TUint scan = aKeyEvent.iScanCode;

        switch ( scan )
            {
            case(EStdKeyUpArrow):
                iAppContainer->iPickSelect->MoveUp(ETrue);
                return EKeyWasConsumed;
            case(EStdKeyDownArrow):
                iAppContainer->iPickSelect->MoveDown(ETrue);
                return EKeyWasConsumed;
            case(EStdKeyLeftArrow):
                iAppContainer->iPickSelect->MoveLeft(ETrue);
                return EKeyWasConsumed;
            case(EStdKeyRightArrow):
                iAppContainer->iPickSelect->MoveRight(ETrue);
                return EKeyWasConsumed;
            case(EStdKeyDevice3):  // Rocker Middle Button
                iAppContainer->iPickSelect->SelectObject();
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
            case(16):
                iAppContainer->iPickSelect->MoveUp(EFalse);
                return EKeyWasConsumed;
            case(17):
                iAppContainer->iPickSelect->MoveDown(EFalse);
                return EKeyWasConsumed;
            case(14):
                iAppContainer->iPickSelect->MoveLeft(EFalse);
                return EKeyWasConsumed;
            case(15):
                iAppContainer->iPickSelect->MoveRight(EFalse);
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
// CPickSelectAppUi::HandleCommandL(TInt aCommand)
// Command handler
// ----------------------------------------------------
//
void CPickSelectAppUi::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
        case EEikCmdExit:
            {
            Exit();
            break;
            }
        case EPickSelectPolygon:
            {
            iAppContainer->iPickSelect->SetPolygonSelection();
            break;
            }
        case EPickSelectObject:
            {
            iAppContainer->iPickSelect->SetObjectSelection();
            break;
            }
        default:
            break;
        }
    }

// End of File

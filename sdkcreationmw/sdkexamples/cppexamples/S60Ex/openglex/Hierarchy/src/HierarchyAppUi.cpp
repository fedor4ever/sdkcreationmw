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
#include "HierarchyAppUi.h"
#include "HierarchyContainer.h"
#include <Hierarchy.rsg>
#include "hierarchy.hrh"

#include <avkon.hrh>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CHierarchyAppUi::ConstructL
// Symbian 2nd phase constroctor can leave
// ----------------------------------------------------------
//
void CHierarchyAppUi::ConstructL()
    {
    BaseConstructL();
    iAppContainer = new (ELeave) CHierarchyContainer;
    iAppContainer->SetMopParent(this);
    iAppContainer->ConstructL( ClientRect(), this );
    AddToStackL( iAppContainer );
    }

// ----------------------------------------------------
// CHierarchyAppUi::~CHierarchyAppUi
// Destructor
// ----------------------------------------------------
//
CHierarchyAppUi::~CHierarchyAppUi()
    {
    if ( iAppContainer )
        {
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
        }
   }

// ------------------------------------------------------------------------------
// CHierarchyAppUi::DynInitMenuPaneL
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CHierarchyAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {
    }

// ----------------------------------------------------
// CHierarchyAppUi::HandleKeyEventL
// Key event handler
// ----------------------------------------------------
//
TKeyResponse CHierarchyAppUi::HandleKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    if ( aType == EEventKeyDown )
        {
        TUint scan = aKeyEvent.iScanCode;

        switch ( scan )
            {
            case(EStdKeyUpArrow):
                iAppContainer->iHierarchy->RotateObjectUp(ETrue);
                return EKeyWasConsumed;
            case(EStdKeyDownArrow):
                iAppContainer->iHierarchy->RotateObjectDown(ETrue);
                return EKeyWasConsumed;
            case(EStdKeyDevice3):
                iAppContainer->iHierarchy->SwitchRotateMode();
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
            case(EStdKeyUpArrow):
                iAppContainer->iHierarchy->RotateObjectUp(EFalse);
                return EKeyWasConsumed;
            case(EStdKeyDownArrow):
                iAppContainer->iHierarchy->RotateObjectDown(EFalse);
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
// CHierarchyAppUi::HandleCommandL
// Command handler
// ----------------------------------------------------
//
void CHierarchyAppUi::HandleCommandL(TInt aCommand)
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

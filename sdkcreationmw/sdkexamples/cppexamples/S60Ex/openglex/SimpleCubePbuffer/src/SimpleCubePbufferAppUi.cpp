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
#include "SimpleCubePbufferAppUi.h"
#include "SimpleCubePbufferContainer.h"
#include <SimpleCubePbuffer.rsg>
#include "simplecubepbuffer.hrh"

#include <avkon.hrh>

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CSimpleCubePbufferAppUi::ConstructL
//
// ----------------------------------------------------------
//
void CSimpleCubePbufferAppUi::ConstructL()
    {
    BaseConstructL();

    iAppContainer = new (ELeave) CSimpleCubePbufferContainer;
    iAppContainer->SetMopParent( this );
    iAppContainer->ConstructL( ClientRect(), this );
    AddToStackL( iAppContainer );
    }

// ----------------------------------------------------
// CSimpleCubePbufferAppUi::~CSimpleCubePbufferAppUi
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CSimpleCubePbufferAppUi::~CSimpleCubePbufferAppUi()
    {
    if ( iAppContainer )
        {
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
        }
   }

// ------------------------------------------------------------------------------
// CSimpleCubePbufferAppUi::DynInitMenuPaneL
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CSimpleCubePbufferAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {
    }

// ----------------------------------------------------
// CSimpleCubePbufferAppUi::HandleKeyEventL
// takes care of key event handling
// ----------------------------------------------------
//
TKeyResponse CSimpleCubePbufferAppUi::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/,TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------
// CSimpleCubePbufferAppUi::HandleCommandL
// takes care of command handling
// ----------------------------------------------------
//
void CSimpleCubePbufferAppUi::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
        case EEikCmdExit:
            {
            Exit();
            break;
            }
        case ESimpleCubeFlat:
	        iAppContainer->iSimpleCubePbuffer->FlatShading();
	        break;
        case ESimpleCubeSmooth:
	        iAppContainer->iSimpleCubePbuffer->SmoothShading();
	        break;
        default:
            break;
        }
    }

// End of File

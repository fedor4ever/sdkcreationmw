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
#include "SimpleCubeAppUi.h"
#include "SimpleCubeContainer.h"
#include <SimpleCube.rsg>
#include "simplecube.hrh"

#include <avkon.hrh>

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CSimpleCubeAppUi::ConstructL()
// ----------------------------------------------------------
//
void CSimpleCubeAppUi::ConstructL()
    {
    BaseConstructL();
    iAppContainer = new (ELeave) CSimpleCubeContainer;
    iAppContainer->SetMopParent(this);
    iAppContainer->ConstructL( ClientRect(), this );
    AddToStackL( iAppContainer );
    }

// ----------------------------------------------------
// CSimpleCubeAppUi::~CSimpleCubeAppUi()
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CSimpleCubeAppUi::~CSimpleCubeAppUi()
{
    if ( iAppContainer )
        {
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
        }
}

// ------------------------------------------------------------------------------
// CSimpleCubeAppUi::::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CSimpleCubeAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {
    }

// ----------------------------------------------------
// CSimpleCubeAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
// Key event handler
// ----------------------------------------------------
//
TKeyResponse CSimpleCubeAppUi::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/ ,TEventCode /*aType*/)
{
    return EKeyWasNotConsumed;
}

// ----------------------------------------------------
// CSimpleCubeAppUi::HandleCommandL(TInt aCommand)
// Command handler
// ----------------------------------------------------
//
void CSimpleCubeAppUi::HandleCommandL(TInt aCommand)
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
            iAppContainer->iSimpleCube->FlatShading();
            break;
        case ESimpleCubeSmooth:
            iAppContainer->iSimpleCube->SmoothShading();
            break;
        case ESimpleCubeTriangles:
            iAppContainer->iSimpleCube->TriangleMode();
            break;
        case ESimpleCubeTriangleFans:
            iAppContainer->iSimpleCube->TriangleFanMode();
            break;
        default:
            break;
        }
    }

// End of File

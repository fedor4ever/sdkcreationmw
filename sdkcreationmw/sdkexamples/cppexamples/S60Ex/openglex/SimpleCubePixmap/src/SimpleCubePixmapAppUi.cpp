/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include "SimpleCubePixmapAppUi.h"
#include "SimpleCubePixmapContainer.h" 
#include <SimpleCubePixmap.rsg>
#include "simplecubepixmap.hrh"

#include <avkon.hrh>

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CSimpleCubePixmapAppUi::ConstructL
// 
// ----------------------------------------------------------
//
void CSimpleCubePixmapAppUi::ConstructL()
    {
    BaseConstructL();

    iAppContainer = new (ELeave) CSimpleCubePixmapContainer;
    iAppContainer->SetMopParent( this );
   iAppContainer->ConstructL( ClientRect(), this );
    AddToStackL( iAppContainer );
    }

// ----------------------------------------------------
// CSimpleCubePixmapAppUi::~CSimpleCubePixmapAppUi
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CSimpleCubePixmapAppUi::~CSimpleCubePixmapAppUi()
    {
    if ( iAppContainer )
        {
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
        }
   }

// ------------------------------------------------------------------------------
// CSimpleCubePixmapAppUi::DynInitMenuPaneL
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CSimpleCubePixmapAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {
    }

// ----------------------------------------------------
// CSimpleCubePixmapAppUi::HandleKeyEventL
// takes care of key event handling
// ----------------------------------------------------
//
TKeyResponse CSimpleCubePixmapAppUi::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/,TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------
// CSimpleCubePixmapAppUi::HandleCommandL
// takes care of command handling
// ----------------------------------------------------
//
void CSimpleCubePixmapAppUi::HandleCommandL(TInt aCommand)
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
	        iAppContainer->iSimpleCubePixmap->FlatShading();
	        break; 
        case ESimpleCubeSmooth:
	        iAppContainer->iSimpleCubePixmap->SmoothShading();
	        break;
        default:
            break;      
        }
    }

// End of File  

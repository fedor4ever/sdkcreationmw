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
#include "BumpMapAppUi.h"
#include "BumpMapContainer.h"
#include <BumpMap.rsg>
#include "bumpmap.hrh"

#include <avkon.hrh>

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CBumpMapAppUi::ConstructL()
// ----------------------------------------------------------
//
void CBumpMapAppUi::ConstructL()
    {
    BaseConstructL();
    iAppContainer = new (ELeave) CBumpMapContainer;
    iAppContainer->SetMopParent(this);
    iAppContainer->ConstructL( ClientRect(), this );
    AddToStackL( iAppContainer );
    }

// ----------------------------------------------------
// CBumpMapAppUi::~CBumpMapAppUi()
// Destructor. Frees any reserved resources.
// ----------------------------------------------------
//
CBumpMapAppUi::~CBumpMapAppUi()
	{
    if ( iAppContainer )
		{
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
		}
	}

// ------------------------------------------------------------------------------
// CBumpMapAppUi::::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CBumpMapAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {
    }

// ----------------------------------------------------
// CBumpMapAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
// ----------------------------------------------------
//
TKeyResponse CBumpMapAppUi::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/,TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------
// CBumpMapAppUi::HandleCommandL(TInt aCommand)
// ----------------------------------------------------
//
void CBumpMapAppUi::HandleCommandL(TInt aCommand)
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

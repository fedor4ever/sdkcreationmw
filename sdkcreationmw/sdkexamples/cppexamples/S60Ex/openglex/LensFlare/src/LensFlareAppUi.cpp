/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include "LensFlareAppUi.h"
#include "LensFlareContainer.h"
#include <LensFlare.rsg>
#include "lensflare.hrh"

#include <avkon.hrh>

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CLensFlareAppUi::ConstructL()
// ----------------------------------------------------------
//
void CLensFlareAppUi::ConstructL()
    {
    BaseConstructL();
    iAppContainer = new (ELeave) CLensFlareContainer;
    iAppContainer->SetMopParent(this);
    iAppContainer->ConstructL( ClientRect(), this );
    AddToStackL( iAppContainer );
    }

// ----------------------------------------------------
// CLensFlareAppUi::~CLensFlareAppUi()
// Destructor. Frees any reserved resources.
// ----------------------------------------------------
//
CLensFlareAppUi::~CLensFlareAppUi()
	{
    if ( iAppContainer )
		{
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
		}
	}

// ------------------------------------------------------------------------------
// CLensFlareAppUi::::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CLensFlareAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {
    }

// ----------------------------------------------------
// CLensFlareAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
// ----------------------------------------------------
//
TKeyResponse CLensFlareAppUi::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/,TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------
// CLensFlareAppUi::HandleCommandL(TInt aCommand)
// ----------------------------------------------------
//
void CLensFlareAppUi::HandleCommandL(TInt aCommand)
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

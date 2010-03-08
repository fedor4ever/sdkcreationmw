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
#include "BillboardAppUi.h"
#include "BillboardContainer.h"
#include <Billboard.rsg>
#include "billboard.hrh"

#include <avkon.hrh>

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CBillboardAppUi::ConstructL()
// ----------------------------------------------------------
//
void CBillboardAppUi::ConstructL()
    {
    BaseConstructL();
    iAppContainer = new (ELeave) CBillboardContainer;
    iAppContainer->SetMopParent(this);
    iAppContainer->ConstructL( ClientRect(), this );
    AddToStackL( iAppContainer );

	// Series60 blocks simultaneous key presses by default.
	// To use multiple keys at the same time the keyblocking
	// must be disabled.
 	SetKeyBlockMode( ENoKeyBlock );
    }

// ----------------------------------------------------
// CBillboardAppUi::~CBillboardAppUi()
// Destructor. Frees any reserved resources.
// ----------------------------------------------------
//
CBillboardAppUi::~CBillboardAppUi()
	{
    if ( iAppContainer )
		{
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
		}
	}

// ------------------------------------------------------------------------------
// CBillboardAppUi::::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CBillboardAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {
    }

// ----------------------------------------------------
// CBillboardAppUi::HandleCommandL(TInt aCommand)
// ----------------------------------------------------
//
void CBillboardAppUi::HandleCommandL(TInt aCommand)
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

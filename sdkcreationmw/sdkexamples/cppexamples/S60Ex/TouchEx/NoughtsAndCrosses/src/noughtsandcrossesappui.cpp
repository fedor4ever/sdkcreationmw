/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
#include "noughtsandcrossesappui.h"
#include "noughtsandcrossessettingsview.h"
#include "noughtsandcrossesview.h"

#include <nac.rsg>
#include "nac.hrh"

#include <avkon.hrh>

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CNoughtsAndCrossesAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CNoughtsAndCrossesAppUi::ConstructL()
    {
    BaseConstructL(EAknEnableSkin);
	
    iGameView = CNoughtsAndCrossesView::NewL();
    iSettingsView = CNoughtsAndCrossesSettingsView::NewL();

    // Transfer ownership to base class
    AddViewL (iGameView);              
    AddViewL (iSettingsView);

    SetDefaultViewL (*iGameView);
    }

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CNoughtsAndCrossesAppUi::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        case EAknSoftkeyExit:
        case EEikCmdExit:
        case ENoughtsAndCrossesCmdExit:
            {
            Exit();
            break;
            }

        default:
            break;      
        }
    }

// End of File

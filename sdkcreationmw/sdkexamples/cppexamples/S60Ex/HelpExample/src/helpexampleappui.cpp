/*
* Copyright (c) 2004 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include <e32std.h>
#include <hlplch.h>
#include <Helpexample.rsg>

#include "HelpExample.pan"
#include "HelpExampleAppUi.h"
#include "HelpExampleAppView.h"
#include "HelpExampleApplication.h"
#include "HelpExample.hrh"
#include "HelpfulForm.h"
#include "HelpExample.hlp.hrh"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHelpExampleAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHelpExampleAppUi::ConstructL()
    {
    BaseConstructL( EAknEnableSkin );

    iAppView = CHelpExampleAppView::NewL( ClientRect() );
    AddToStackL( iAppView );
    }
// -----------------------------------------------------------------------------
// CHelpExampleAppUi::CHelpExampleAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CHelpExampleAppUi::CHelpExampleAppUi()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CHelpExampleAppUi::~CHelpExampleAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CHelpExampleAppUi::~CHelpExampleAppUi()
    {
    if ( iAppView )
        {
        iEikonEnv->RemoveFromStack( iAppView );
        delete iAppView;
        iAppView = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CHelpExampleAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CHelpExampleAppUi::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;

        case EShowHelpfulDialog:
            {
            CHelpfulForm* dlg = CHelpfulForm::NewL( R_FORM_MENUBAR );
            dlg->ExecuteLD( R_HELPFUL_DIALOG );
            }
            break;

        case ELaunchHelpForContext:
            {
            CArrayFix <TCoeHelpContext>* buf = CCoeAppUi::AppHelpContextL();
            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(), buf );
            }
            break;

        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CArrayFix <TCoeHelpContext>
// Return the help context for this application.
// -----------------------------------------------------------------------------
//
CArrayFix <TCoeHelpContext>* CHelpExampleAppUi::HelpContextL() const
    {
    CArrayFixFlat <TCoeHelpContext>* array =
                new ( ELeave )CArrayFixFlat <TCoeHelpContext> ( 1 );
    CleanupStack::PushL( array );
    array->AppendL( TCoeHelpContext( KUidHelpFile, KContextApplication ) );
    CleanupStack::Pop( array );
    return array;
    }

// End of File

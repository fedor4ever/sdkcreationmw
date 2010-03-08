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
#include <avkon.hrh>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <FocusEvent.rsg>

#include "FocusEvent.pan"
#include "FocusEventAppUi.h"
#include "FocusEventAppView.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CFocusEventAppUi::CFocusEventAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CFocusEventAppUi::CFocusEventAppUi()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CFocusEventAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CFocusEventAppUi::ConstructL()
    {
    BaseConstructL( EAknEnableSkin );

    iAppView = CFocusEventAppView::NewL( ClientRect() );
    AddToStackL( iAppView );
    }


// -----------------------------------------------------------------------------
// CFocusEventAppUi::~CFocusEventAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CFocusEventAppUi::~CFocusEventAppUi()
    {
    if ( iAppView )
        {
        iEikonEnv->RemoveFromStack( iAppView );
        delete iAppView;
        iAppView = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CFocusEventAppUi::HandleCommandL()
// Takes care of key event handling.
// -----------------------------------------------------------------------------
//
void CFocusEventAppUi::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
    case EEikCmdExit:
    case EAknSoftkeyExit:
        Exit();
        break;

    default:
        break;
        }
    }

// -----------------------------------------------------------------------------
// CFocusEventAppUi::HandleForegroundEventL()
// Displays message every time focus is gained or lost.
// -----------------------------------------------------------------------------
//
void CFocusEventAppUi::HandleForegroundEventL( TBool aForeground )
    {
    if ( !aForeground ) // Focus lost
        {
        // Load a string from the resource file.
        HBufC* lostFocusMessage = StringLoader::LoadLC
                                                 ( R_FOCU_LOST_FOCUS_TEXT );
#ifdef __WINS__
        User::InfoPrint( *lostFocusMessage );
#endif /* __WINS__ */
        iAppView->Print( *lostFocusMessage );
        iAppView->PrintNewLineL();
        // Pop HBuf from CleanupStack and destroy it.
        CleanupStack::PopAndDestroy( lostFocusMessage );
        }
    else // Focus gained
        {
        // Load a string from the resource file.
        HBufC* gainedFocusMessage = StringLoader::LoadLC
                                                   ( R_FOCU_GAINED_FOCUS_TEXT );
#ifdef __WINS__
        User::InfoPrint( *gainedFocusMessage );
#endif /* __WINS__ */
        iAppView->Print( *gainedFocusMessage );
        iAppView->PrintNewLineL();
        // Pop HBuf from CleanupStack and Destroy it.
        CleanupStack::PopAndDestroy( gainedFocusMessage );
        }

    CAknAppUi::HandleForegroundEventL( aForeground );
    }

// End of File

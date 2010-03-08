/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CDataMobilityAppUi implementation
*
*/


// INCLUDE FILES
#include <avkon.hrh>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <f32file.h>
#include <s32file.h>

#include "datamobility.pan"
#include "datamobilityappui.h"
#include "datamobilityappmenuview.h"
#include "datamobilityappoutputview.h"
#include "datamobility.hrh"

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CDataMobilityAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDataMobilityAppUi::ConstructL()
    {
    // Initialise app UI with standard value.
    BaseConstructL();

    // Create view objects
    CDataMobilityAppMenuView* menuView = CDataMobilityAppMenuView::NewL();
    CleanupStack::PushL( menuView );
    AddViewL( menuView );      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop( menuView );

    CDataMobilityAppOutputView* outputView = CDataMobilityAppOutputView::NewL();
    CleanupStack::PushL( outputView );
    AddViewL( outputView );      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop( outputView );

    SetDefaultViewL( *menuView );
    }

// -----------------------------------------------------------------------------
// CDataMobilityAppUi::CDataMobilityAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CDataMobilityAppUi::CDataMobilityAppUi()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CDataMobilityAppUi::~CDataMobilityAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CDataMobilityAppUi::~CDataMobilityAppUi()
    {
    }

// -----------------------------------------------------------------------------
// CDataMobilityAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CDataMobilityAppUi::HandleCommandL( TInt aCommand )
    {
    TInt currentView( 0 );
    TInt newView( 0 );

    currentView = ( iView->Id() ).iUid;

    switch( aCommand )
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            {
            Exit();
            break;
            }
        case EAppUIGoBack:
        case EAppUIGoToMenu:
            {
            newView = EMenuViewId;
            break;
            }
        case EAppUIGoToStandardApplication:
        case EAppUIGoToApplicationUsingSnap:
        case EAppUIGoToTriggeringConnectionDialog:
        case EAppUIGoToALRApplication:
        case EAppUIGoToALRApplicationUsingDefaultConnection:
        case EAppUIGoToUsingConnectionSetting:
            {
            newView = EOutputViewId;
            break;
            }
        default:
            // let the framework handle it
            return;
        }

    if( newView != currentView )
        {
        TBufC8<1> aBuf;
        ActivateLocalViewL( TUid::Uid( newView ), TUid::Uid( aCommand ), aBuf );
        }
    }

// End of File

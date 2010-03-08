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
#include <eikmenup.h>
#include <avkon.hrh>
#include <CSAsync.rsg>

#include "CSAsync.pan"
#include "CSAsyncAppUi.h"
#include "CSAsyncAppView.h"
#include "CSAsyncDocument.h"
#include "CSAsync.hrh"

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CCSAsyncAppUi::CCSAsyncAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CCSAsyncAppUi::CCSAsyncAppUi()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CCSAsyncAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCSAsyncAppUi::ConstructL()
    {
    BaseConstructL();

    iAppView = CCSAsyncAppView::NewL( ClientRect(), *AsyncDocument() );

    AddToStackL( iAppView );
    }

// -----------------------------------------------------------------------------
// CCSAsyncAppUi::~CCSAsyncAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CCSAsyncAppUi::~CCSAsyncAppUi()
    {
    if ( iAppView )
        {
        iEikonEnv->RemoveFromStack( iAppView );
        delete iAppView;
        iAppView = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CCSAsyncAppUi::AsyncDocument() const
// Casts iDocument member from CAknAppUi to CCSAsyncDocument.
// -----------------------------------------------------------------------------
//
CCSAsyncDocument* CCSAsyncAppUi::AsyncDocument() const
    {
    return ( static_cast<CCSAsyncDocument*>( iDocument ) );
    }

// -----------------------------------------------------------------------------
// CCSAsyncAppUi::HandleCommandL( TInt aCommand )
// Handles user menu selections.
// -----------------------------------------------------------------------------
//
void CCSAsyncAppUi::HandleCommandL( TInt aCommand )
    {
    switch( aCommand )
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;

        case EStartClock:
            AsyncDocument()->UpdateTime();
            break;

        case EStopClock:
            AsyncDocument()->StopClock();
            break;

        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CCSAsyncAppUi::HandleTimeUpdate()
// Updates view
// -----------------------------------------------------------------------------
//
void CCSAsyncAppUi::HandleTimeUpdate()
    {
    __ASSERT_ALWAYS( iAppView, User::Panic ( KCSAsyncClient, 
                                             ECSAsyncProgram ) );
    iAppView->DrawNow();
    }

// -----------------------------------------------------------------------------
// CCSAsyncAppUi::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
// Prepares the menu prior to display.
// -----------------------------------------------------------------------------
//
void CCSAsyncAppUi::DynInitMenuPaneL( TInt aResourceId, 
                                      CEikMenuPane* aMenuPane )
    {
    if ( aResourceId == R_CSASYNC_MENU )
        {
        if ( AsyncDocument()->ClockActive() )
            {
            aMenuPane->SetItemDimmed( EStartClock, ETrue );
            aMenuPane->SetItemDimmed( EStopClock, EFalse );
            }
        else
            {
            aMenuPane->SetItemDimmed( EStartClock, EFalse );
            aMenuPane->SetItemDimmed( EStopClock, ETrue );
            }
        }
    }

// -----------------------------------------------------------------------------
// CCSAsyncAppUi::HandleResourceChangeL( TInt aType )
// Called by framework when layout is changed.
// -----------------------------------------------------------------------------
//
void CCSAsyncAppUi::HandleResourceChangeL( TInt aType )
    {
    CAknAppUi::HandleResourceChangeL( aType );

    if ( aType==KEikDynamicLayoutVariantSwitch )
        {
        iAppView->SetRect( ClientRect() );
        }
    //Controls derived from CCoeControl, handled in container class
    iAppView->HandleResourceChange( aType );
    }

// End of File

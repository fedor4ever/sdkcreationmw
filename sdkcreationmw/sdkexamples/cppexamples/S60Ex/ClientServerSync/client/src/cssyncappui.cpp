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
#include <eikmenup.h>
#include <avkon.hrh>

#include "CSSync.pan"
#include "CSSyncAppUi.h"
#include "CSSyncAppView.h"
#include "CSSyncDocument.h"
#include "CSSync.hrh"


// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CCSSyncAppUi::CCSSyncAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CCSSyncAppUi::CCSSyncAppUi()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CCSSyncAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCSSyncAppUi::ConstructL()
    {
    BaseConstructL();

    iAppView = CCSSyncAppView::NewL( ClientRect(), *SyncDocument() );

    AddToStackL( iAppView );
    }

// -----------------------------------------------------------------------------
// CCSSyncAppUi::~CCSSyncAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CCSSyncAppUi::~CCSSyncAppUi()
    {
    if ( iAppView )
        {
        iEikonEnv->RemoveFromStack( iAppView );
        delete iAppView;
        iAppView = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CCSSyncAppUi::SyncDocument()
// Casts iDocument member from CAknAppUi to CCSSyncDocument.
// -----------------------------------------------------------------------------
//
CCSSyncDocument* CCSSyncAppUi::SyncDocument() const
    {
    return static_cast<CCSSyncDocument*>( iDocument );
    }

// -----------------------------------------------------------------------------
// CCSSyncAppUi::HandleCommandL()
// Handles user menu selections.
// -----------------------------------------------------------------------------
//
void CCSSyncAppUi::HandleCommandL( TInt aCommand )
    {
    switch( aCommand )
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;

        case EDisplayTime:
            // Pull the time from the server
            SyncDocument()->UpdateTime();

            // Get view to display the new time
            iAppView->DrawNow();
            break;

        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CCSSyncAppUi::HandleResourceChangeL( TInt aType )
// Called by framework when layout is changed.
// -----------------------------------------------------------------------------
//
void CCSSyncAppUi::HandleResourceChangeL( TInt aType )
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

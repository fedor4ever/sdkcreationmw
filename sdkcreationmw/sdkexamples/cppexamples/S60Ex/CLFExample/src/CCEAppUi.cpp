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
#include "CCEAppUi.h"

#include "CCEGroupView.h"
#include "CCEMusicView.h"
#include "CCEOperationObserver.h"
#include "CLFExample.hrh"

#include <MCLFContentListingEngine.h>
#include <ContentListingFactory.h>
#include <MCLFItemListModel.h>
#include <CLFContentListing.hrh>
#include <CLFExample.rsg>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCEAppUi::CCEAppUi
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCEAppUi::CCEAppUi()
    {
    }

// -----------------------------------------------------------------------------
// CCEAppUi::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCEAppUi::ConstructL()
    {
    BaseConstructL( CAknAppUi::EAknEnableSkin );

    // Create CLF Operation Observer to monitor, when
    // refresh operation has completed
    iObserver = CCEOperationObserver::NewL();

    // Create Content Listing Engine and a list model
    iEngine = ContentListingFactory::NewContentListingEngineLC();
    CleanupStack::Pop(); // iEngine
    iModel = iEngine->CreateListModelLC( *iObserver );
    CleanupStack::Pop(); // iModel

    // Create a view for music files listing
    iAppView = CCEMusicView::NewLC( iEngine, iModel, iObserver );
    AddViewL( iAppView ); // ownership transfers
    CleanupStack::Pop(); // iAppView

    // Create a view for groups listing
    iAppView = CCEGroupView::NewLC( iEngine, iModel, iObserver );
    AddViewL( iAppView ); // ownership transfers
    CleanupStack::Pop(); // iAppView
    SetDefaultViewL( *iAppView );

    // Create an array for wanted media types
    RArray<TInt> array;
    CleanupClosePushL( array );

    // Append music-type to wanted types array to exclude
    // all other files than music files from the list model
    array.AppendL( ECLFMediaTypeMusic );

    // Set wanted media types array to the model
    iModel->SetWantedMediaTypesL( array.Array() );
    CleanupStack::PopAndDestroy( &array );
    }

// Destructor
CCEAppUi::~CCEAppUi()
    {
    if( iModel )
        {
        iModel->CancelRefresh();
        delete iModel;
        }

    delete iEngine;
    delete iObserver;
    }

// -----------------------------------------------------------------------------
// CCEAppUi::HandleCommandL
// Handle user's commands.
// -----------------------------------------------------------------------------
//
void CCEAppUi::HandleCommandL( TInt aCommand )
    {
    switch( aCommand )
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
        case EAknCmdExit:
        case ECECmdExit:
            {
            Exit();
            break;
            }

        default:
            {
            break;
            }
        }
    }

//  End of File

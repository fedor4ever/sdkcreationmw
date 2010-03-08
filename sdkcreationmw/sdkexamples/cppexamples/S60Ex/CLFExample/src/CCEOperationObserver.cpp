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
#include "CCEOperationObserver.h"

#include "CLFExample.hrh"
#include "CCEMusicView.h"

#include <MCLFContentListingEngine.h>
#include <ContentListingFactory.h>
#include <MCLFItemListModel.h>
#include <CLFContentListing.hrh>
#include <CLFExample.rsg>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCEOperationObserver::CCEOperationObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCEOperationObserver::CCEOperationObserver()
    {
    }

// -----------------------------------------------------------------------------
// CCEOperationObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCEOperationObserver* CCEOperationObserver::NewL()
    {
    CCEOperationObserver* self = new (ELeave) CCEOperationObserver;
    return self;
    }

// Destructor
CCEOperationObserver::~CCEOperationObserver()
    {
    }

// -----------------------------------------------------------------------------
// CCEOperationObserver::HandleOperationEventL
// Method to handle list model operation events.
// -----------------------------------------------------------------------------
//
void CCEOperationObserver::HandleOperationEventL(
                                TCLFOperationEvent aOperationEvent,
                                TInt /*aError*/ )
    {
    // Waiting is stopped when an event for refresh completion is received
    if( aOperationEvent == ECLFRefreshComplete )
        {
        iRefreshOngoing = EFalse;

        if( iWait && iWait->IsStarted() )
            {
            iWait->AsyncStop();
            }
        }
    else if( aOperationEvent == ECLFModelOutdated )
        {
        if( iTarget )
            {
            // Report the music files view that there are new/changed files
            // in the file system.
            iTarget->ItemsChangedL();
            }
        }
    }

// -----------------------------------------------------------------------------
// CCEOperationObserver::IsRefreshOngoing
// Check if there is a refresh operation is ongoing.
// -----------------------------------------------------------------------------
//
TBool CCEOperationObserver::IsRefreshOngoing()
    {
    return iRefreshOngoing;
    }

// -----------------------------------------------------------------------------
// CCEOperationObserver::PrepareForRefresh
// Prepare for a refresh operation.
// -----------------------------------------------------------------------------
//
void CCEOperationObserver::PrepareForRefresh( CActiveSchedulerWait& aWait )
    {
    // Store the wait instance
    iWait = &aWait;

    // Refresh operation will be started in a moment
    iRefreshOngoing = ETrue;
    }

// -----------------------------------------------------------------------------
// CCEOperationObserver::SetMusicView
// -----------------------------------------------------------------------------
//
void CCEOperationObserver::SetMusicView( CCEMusicView* aTarget )
    {
    iTarget = aTarget;
    }

// -----------------------------------------------------------------------------
// CCEOperationObserver::RemoveMusicView
// -----------------------------------------------------------------------------
//
void CCEOperationObserver::RemoveMusicView()
    {
    iTarget = NULL;
    }

//  End of File

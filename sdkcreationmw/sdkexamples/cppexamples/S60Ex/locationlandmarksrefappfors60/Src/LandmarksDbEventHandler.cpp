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
* Description:  Implements the class CLandmarksDbEventHandler
*
*/


// INCLUDE FILES
#include <EPos_CPosLandmarkDatabase.h>

#include "LandmarksDbEventHandler.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLandmarksDbEventHandler::CLandmarksDbEventHandler
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLandmarksDbEventHandler::CLandmarksDbEventHandler(
    CPosLandmarkDatabase& aDb)
: CActive(CActive::EPriorityStandard), iDb(aDb)
    {
    CActiveScheduler::Add(this);
    ObserveEvents();
    }

// Destructor
CLandmarksDbEventHandler::~CLandmarksDbEventHandler()
    {
    Cancel();
    iDbObservers.Close();
    }

// -----------------------------------------------------------------------------
// CLandmarksDbEventHandler::AddObserverL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksDbEventHandler::AddObserverL(
    MLandmarksDbObserver* aObserver)
    {
    User::LeaveIfError(iDbObservers.Append(aObserver));
    }

// -----------------------------------------------------------------------------
// CLandmarksDbEventHandler::RunL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksDbEventHandler::RunL()
    {
    for (TInt i = 0; i < iDbObservers.Count(); i++)
        {
        // All observers must be notified. We cannot allow the observers to 
        // leave.
        TInt err;
        TRAP(err, iDbObservers[i]->NotifyDbEventL(iEvent, iStatus.Int()));
        }
    ObserveEvents();
    }

// -----------------------------------------------------------------------------
// CLandmarksDbEventHandler::DoCancel
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksDbEventHandler::DoCancel()
    {
    iDb.CancelNotifyDatabaseEvent();
    }

// -----------------------------------------------------------------------------
// CLandmarksDbEventHandler::RunError
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLandmarksDbEventHandler::RunError(TInt /*aError*/)
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CLandmarksDbEventHandler::ObserveEvents
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksDbEventHandler::ObserveEvents()
    {
    iStatus = KRequestPending;
    iDb.NotifyDatabaseEvent(iEvent, iStatus);
    SetActive();
    }

// End of File


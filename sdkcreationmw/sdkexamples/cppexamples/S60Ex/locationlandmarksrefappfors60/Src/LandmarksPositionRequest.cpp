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
* Description:  Implements the CLandmarksPositionRequest class
*
*/


// INCLUDE FILES
#include <aknwaitdialog.h>
#include <coemain.h>
#include <aknnotewrappers.h>

#include <LbsPositionInfo.h>

#include <LmRefApp.rsg>
#include "LandmarksUtils.h"
#include "LandmarksPositionRequest.h"
#include "LandmarksOperationObserver.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLandmarksPositionRequest::CLandmarksPositionRequest
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLandmarksPositionRequest::CLandmarksPositionRequest(
    MLandmarksOperationObserver* aObserver)
    : 
    CActive(CActive::EPriorityStandard), 
    iObserver(aObserver),
    iState(EIdle)
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CLandmarksPositionRequest::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLandmarksPositionRequest::ConstructL(const TDesC& aAppName)
    {
    iAppName = aAppName.AllocL();
    InitializeL();
    }

// -----------------------------------------------------------------------------
// CLandmarksPositionRequest::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLandmarksPositionRequest* CLandmarksPositionRequest::NewL(
    const TDesC& aAppName,
    MLandmarksOperationObserver* aObserver)
    {
    CLandmarksPositionRequest* self =
        new (ELeave) CLandmarksPositionRequest(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL(aAppName);
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CLandmarksPositionRequest::~CLandmarksPositionRequest()
    {
    Cancel();
    iPositioner.Close();
    iLocationServer.Close();
    DismissWaitNote();
    delete iAppName;
    }

// -----------------------------------------------------------------------------
// CLandmarksPositionRequest::FetchPositionInfoL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksPositionRequest::FetchPositionInfoL(
    const TDesC& aMessage,
    TPositionInfo& aPositionInfo)
    {
    Cancel();
    iPositionInfo = &aPositionInfo;

    // Initialize wait note
    CAknWaitDialog* waitNote = new (ELeave) CAknWaitDialog(
        reinterpret_cast <CEikDialog**> (&iWaitNote), ETrue); // show immedaitely
    CleanupStack::PushL(waitNote);
    waitNote->SetCallback(this);
    waitNote->SetTextL(aMessage);
    waitNote->PrepareLC(R_LMREFAPP_ACQUIRE_LOCATION_WAIT_NOTE);
    CleanupStack::Pop(waitNote); // waitNote will delete itself
    iWaitNote = waitNote;

    // Request location
    iState = EAcquiringDefaultLocation;
    ExecuteNextStep();

    // Launch wait note
    iWaitNote->RunLD();
    }

// -----------------------------------------------------------------------------
// CLandmarksPositionRequest::DismissWaitNote
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksPositionRequest::DismissWaitNote()
    {
    if (iWaitNote)
        {
        TInt err;
        TRAP(err, iWaitNote->ProcessFinishedL());
        iWaitNote = NULL;
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksPositionRequest::DoCancel
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksPositionRequest::DoCancel()
    {
    // Cancel location request
    if (iState == EAcquiringDefaultLocation)
        {
        iPositioner.CancelRequest(EPositionerNotifyPositionUpdate);
        }
    else if (iState == EAcquiringLastKnownLocation)
        {
        iPositioner.CancelRequest(EPositionerGetLastKnownPosition);
        }
    iState = EIdle;
    DismissWaitNote();
    }

// -----------------------------------------------------------------------------
// CLandmarksPositionRequest::RunL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksPositionRequest::RunL()
    {
    if (iStatus == KErrNone)
        {
        // Location retrieved. Operation finished.
        NotifyOperationCompleteL(KErrNone);
        }
    else // iStatus != KErrNone
        {
        if (iState == EAcquiringDefaultLocation)
            {
            // Fetching current location failed. Try last known location.
            iState = EAcquiringLastKnownLocation;
            ExecuteNextStep();
            }
        else if (iState == EAcquiringLastKnownLocation)
            {
            // Fetching last known location failed. 
            NotifyOperationCompleteL(iStatus.Int());
            }
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksPositionRequest::RunError
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLandmarksPositionRequest::RunError(TInt /*aError*/)
    {
    // Ignore possible errors.
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CLandmarksPositionRequest::DialogDismissedL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksPositionRequest::DialogDismissedL(TInt aButtonId)
    {
    if (aButtonId == EAknSoftkeyCancel)
        {
        // Dialog is already dismissed by framework
        iWaitNote = NULL;

        Cancel();
        }
    }

void CLandmarksPositionRequest::ShowErrorL(TInt aError)
    {
    HBufC* format = CCoeEnv::Static()->AllocReadResourceAsDes16LC(
        R_LMREFAPP_ACQUIRING_LOC_ERROR);
    
    // some extra characters needed for error code
    const TInt KErrorCodeTextLength = 10;
    HBufC* message = HBufC::NewLC(
        format->Length() + KErrorCodeTextLength);
    message->Des().Format(*format, aError);

    CAknInformationNote* informationNote = 
        new (ELeave) CAknInformationNote(ETrue);
    informationNote->ExecuteLD(*message);
    CleanupStack::PopAndDestroy(2, format);
    }

// -----------------------------------------------------------------------------
// CLandmarksPositionRequest::ExecuteNextStep
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksPositionRequest::ExecuteNextStep()
    {
    iStatus = KRequestPending;

    switch (iState)
        {
        case EAcquiringDefaultLocation:
            iPositioner.NotifyPositionUpdate(*iPositionInfo, iStatus);
            break;
        case EAcquiringLastKnownLocation:
            iPositioner.GetLastKnownPosition(*iPositionInfo, iStatus);
            break;
        default:    
            // this case should never happen, see RunL
            LandmarksUtils::Panic(KErrGeneral);
            break;
        }

    SetActive();
    }

// -----------------------------------------------------------------------------
// CLandmarksPositionRequest::NotifyOperationCompleteL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksPositionRequest::NotifyOperationCompleteL(TInt aError)
    {
    DismissWaitNote();
    iState = EIdle;

    // Notify user if location couldn't be fetched.
    if (aError) 
        {
        ShowErrorL(aError);
        }

    // Notify Observer
    iObserver->NotifyOperationProgressL(
        EAcquiringLocation, KOperationReady, aError);
    }

// -----------------------------------------------------------------------------
// CLandmarksPositionRequest::InitializeL()
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksPositionRequest::InitializeL()
    {
    // Connect to the location server
    User::LeaveIfError(iLocationServer.Connect());

    // Open the default positioner
    User::LeaveIfError(iPositioner.Open(iLocationServer));

    // Set this application as location requestor
    User::LeaveIfError(iPositioner.SetRequestor(
        CRequestor::ERequestorService,
        CRequestor::EFormatApplication,
        *iAppName));

    // Set maximum allowed time for a location request
    const TInt K30Seconds = 30000000;
    TTimeIntervalMicroSeconds timeOut(K30Seconds);
    TPositionUpdateOptions updateOptions;
    updateOptions.SetUpdateTimeOut(timeOut);
    User::LeaveIfError(iPositioner.SetUpdateOptions(updateOptions));
    }

// End of File


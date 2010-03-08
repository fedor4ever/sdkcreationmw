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
* Description:  Implements the CLandmarksEngine class
*
*/


// INCLUDE FILES
#include <aknview.h>
#include <EPos_CPosLandmarkDatabase.h>
#include <EPos_CPosLmPartialReadParameters.h>
#include <EPos_CPosLandmarkSearch.h>
#include <EPos_CPosLmTextCriteria.h>

#include "LandmarksEngine.h"
#include "LandmarksLmOpWrapper.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLandmarksEngine::CLandmarksEngine
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLandmarksEngine::CLandmarksEngine(
    CPosLandmarkDatabase& aDb) 
:   CLandmarksEngineBase(aDb),
    iSortPref(CPosLandmark::ELandmarkName, TPosLmSortPref::EAscending)
    {
    }

// -----------------------------------------------------------------------------
// CLandmarksEngine::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLandmarksEngine::ConstructL()
    {
    CLandmarksEngineBase::ConstructL();

    // Set partial read parameters
    CPosLmPartialReadParameters* partialReadParams = 
        CPosLmPartialReadParameters::NewLC();
    partialReadParams->SetRequestedAttributes(
        CPosLandmark::ELandmarkName | CPosLandmark::EIcon);
    iDb.SetPartialReadParametersL(*partialReadParams);
    CleanupStack::PopAndDestroy(partialReadParams);
    }

// -----------------------------------------------------------------------------
// CLandmarksEngine::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLandmarksEngine* CLandmarksEngine::NewL(
    CPosLandmarkDatabase& aDb)
    {
    CLandmarksEngine* self = new (ELeave) CLandmarksEngine(aDb);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CLandmarksEngine::~CLandmarksEngine()
    {
    Cancel();
    iItemIds.Close();
    }

// -----------------------------------------------------------------------------
// CLandmarksEngine::StartInitializeDbIfNecessaryL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CLandmarksEngine::StartInitializeDbIfNecessaryL(
    MLandmarksOperationObserver* aObserver)
    {
    if (iDb.IsInitializingNeeded())
        {
        // Create initialize operation
        CPosLmOperation* operation = iDb.InitializeL();

        iObserver = aObserver;
        iActiveOperation = EInitializeDb;
        iStatus = KRequestPending;
        SetPriorityAndSetActive();

        // Start asynchronous initialization
        TBool reportProgress = ETrue;
        iLmOpWrapper->StartOperation(operation, iStatus, reportProgress);

        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksEngine::AddLandmarkL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksEngine::AddLandmarkL(CPosLandmark& aLandmark)
    {
    iDb.AddLandmarkL(aLandmark);
    }

// -----------------------------------------------------------------------------
// CLandmarksEngine::CommitLandmarkL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksEngine::CommitLandmarkL(const CPosLandmark& aLandmark)
    {
    iDb.UpdateLandmarkL(aLandmark);
    }

// -----------------------------------------------------------------------------
// CLandmarksEngine::DeleteLandmarkL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksEngine::DeleteLandmarkL(TPosLmItemId aItemId)
    {
    iDb.RemoveLandmarkL(aItemId);
    }

// -----------------------------------------------------------------------------
// CLandmarksEngine::LandmarkLC
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CPosLandmark* CLandmarksEngine::LandmarkLC(TPosLmItemId aItemId)
    {
    return iDb.ReadLandmarkLC(aItemId);
    }

// -----------------------------------------------------------------------------
// CLandmarksEngine::StartSearchingLandmarksL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksEngine::StartSearchingLandmarksL(
    MLandmarksOperationObserver* aObserver)
    {
    // We need to cancel if we are searching/reading landmarks.
    Cancel(); 

    iObserver = aObserver;
    iSearchResultExists = EFalse;
    iFilterSearch = EFalse;
    
    // Complete ourselves and start search for all landmarks in db in RunL().
    iStatus = KRequestPending;
    SetPriorityAndSetActive();
    iActiveOperation = ELandmarkSearch;
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    }

// -----------------------------------------------------------------------------
// CLandmarksEngine::StartSearchingLandmarksL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksEngine::StartSearchingLandmarksL(
    const TDesC& aSearchPattern, 
    TBool aSearchOnlyInPreviousMatches,
    MLandmarksOperationObserver* aObserver)
    {
    // We need to cancel if we are searching/reading landmarks.
    Cancel();

    if (!iSearchResultExists)
        {
        // There are no previous results or previous search did not complete.
        // We cannot use the result from it.
        aSearchOnlyInPreviousMatches = EFalse;
        }
    iSearchResultExists = EFalse;
    iFilterSearch = ETrue;

    // Set search criteria
    CPosLmTextCriteria* criteria = CPosLmTextCriteria::NewLC();
    criteria->SetTextL(aSearchPattern);
    criteria->SetAttributesToSearch(CPosLandmark::ELandmarkName);

    // Create search operation
    CPosLmOperation* operation = iSearcher->StartLandmarkSearchL(
        *criteria, iSortPref, aSearchOnlyInPreviousMatches);
    CleanupStack::PopAndDestroy(criteria);

    // Start search operation
    iObserver = aObserver;
    iStatus = KRequestPending;
    iLmOpWrapper->StartOperation(operation, iStatus);
    SetPriorityAndSetActive();
    iActiveOperation = ELandmarkSearch;
    }

// -----------------------------------------------------------------------------
// CLandmarksEngine::FetchSearchResult
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RArray<TPosLmItemId>* CLandmarksEngine::FetchSearchResult()
    {
    return &iItemIds;
    }

// -----------------------------------------------------------------------------
// CLandmarksEngine::StartReadingLandmarksL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksEngine::StartReadingLandmarksL(
    TInt aNrOfItemsToReadPerBundle,
    MLandmarksOperationObserver* aObserver)
    {
    // We need to cancel if we are searching/reading landmarks.
    Cancel();

    iNrOfItemsToRead = aNrOfItemsToReadPerBundle;
    iObserver = aObserver;
    iCurrentItemId = 0;

    // Start reading a bundle of landmarks
    ReadSomeLandmarksL();
    iActiveOperation = ELandmarkRead;
    }

// -----------------------------------------------------------------------------
// CLandmarksEngine::FetchLandmarksLC
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CArrayPtr<CPosLandmark>* CLandmarksEngine::FetchLandmarksLC()
    {
    // Fetch the partially read landmarks.
    CArrayPtr<CPosLandmark>* result = 
        iDb.TakePreparedPartialLandmarksL(iLmOpWrapper->LmOperationPtr());
    CleanupStack::PushL(result);

    return result;
    }

// -----------------------------------------------------------------------------
// CLandmarksEngine::RunL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksEngine::RunL()
    {
    if (iActiveOperation == EInitializeDb)
        {
        // Report progress
        iObserver->NotifyOperationProgressL(
            iActiveOperation, iLmOpWrapper->Progress(), iStatus.Int());

        if (iStatus == KPosLmOperationNotComplete)
            {
            // Continue initializing database until finished
            iStatus = KRequestPending;
            SetPriorityAndSetActive();
            iLmOpWrapper->ExecuteNextStep(iStatus);
            }
        }
    else if (iStatus != KErrNone)
        {
        // Operation failed. Report error code
        NotifyOperationReadyL(iActiveOperation, iStatus.Int());
        }
    else if (iActiveOperation == ELandmarkSearch)
        {
        // Fetch search result
        PopulateItemIdArrayL();
        }
    else if (iActiveOperation == ELandmarkRead)
        {
        if (iCurrentItemId < iItemIds.Count())
            {
            // There are still found landmarks to read
            iObserver->NotifyOperationProgressL(iActiveOperation, KOperationNotReady, KErrNone);
            ReadSomeLandmarksL();
            }
        else
            {
            // No more matches to read
            NotifyOperationReadyL(iActiveOperation, KErrNone);
            }
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksEngine::DoCancel
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksEngine::DoCancel()
    {
    iLmOpWrapper->Cancel();

    if (iActiveOperation == ELandmarkSearch)
        {
        iSearchResultExists = EFalse;
        }
    iActiveOperation = ENoOperation;
    }

// -----------------------------------------------------------------------------
// CLandmarksEngine::RunError
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLandmarksEngine::RunError(TInt aError)
    {
    // Notify user and ignore error code
    TInt err;
    TRAP(err, NotifyOperationReadyL(iActiveOperation, aError));

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CLandmarksEngine::ReadSomeLandmarksL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksEngine::ReadSomeLandmarksL()
    {
    RArray<TPosLmItemId> subSetOfIds;
    CleanupClosePushL(subSetOfIds);

    // Create a subset of the found matches to read
    TInt nrOfMatches = iItemIds.Count();
    TInt i = 0;
    while (i < iNrOfItemsToRead && iCurrentItemId < nrOfMatches)
        {
        User::LeaveIfError(subSetOfIds.Append(iItemIds[iCurrentItemId]));
        iCurrentItemId++;
        i++;
        }

    // Start reading subset of found matches
    iStatus = KRequestPending;
    CPosLmOperation* operation = iDb.PreparePartialLandmarksL(subSetOfIds);
    iLmOpWrapper->StartOperation(operation, iStatus);
    SetPriorityAndSetActive();

    CleanupStack::PopAndDestroy(&subSetOfIds);
    }

// -----------------------------------------------------------------------------
// CLandmarksEngine::NotifyOperationReadyL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksEngine::NotifyOperationReadyL(
    TOperation aOperation, 
    TInt aErrorCode)
    {
    iActiveOperation = ENoOperation;
    iObserver->NotifyOperationProgressL(
        aOperation, 
        KOperationReady, 
        aErrorCode);
    }

// -----------------------------------------------------------------------------
// CLandmarksEngine::PopulateItemIdArrayL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksEngine::PopulateItemIdArrayL()
    {
    // Fetch itemIterator
    CPosLmItemIterator* itemIterator = NULL;
    if (iFilterSearch)
        {
        itemIterator = iSearcher->MatchIteratorL();
        iSearchResultExists = ETrue;
        }
    else
        {
        itemIterator = iDb.LandmarkIteratorL(iSortPref);
        }
    CleanupStack::PushL(itemIterator);
    itemIterator->Reset();

    // Fetch iItemIds
    TUint nrOfItems = itemIterator->NumOfItemsL();
    if (nrOfItems > 0)
        {
        TInt startIndex = 0;
        iItemIds.Reset();
        itemIterator->GetItemIdsL(iItemIds, startIndex, nrOfItems);
        NotifyOperationReadyL(iActiveOperation, KErrNone);
        }
    else
        {
        // No matches found
        NotifyOperationReadyL(iActiveOperation, KErrNotFound);
        }
    CleanupStack::PopAndDestroy(itemIterator);
    }

//  End of File


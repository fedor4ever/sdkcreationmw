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
* Description:  Implementation of the application engine component interface.
*
*/


// INCLUDE FILES
#include <EPos_CPosLandmarkDatabase.h>

#include "LmRefApp.hrh"
#include "LandmarksApplicationEngine.h"
#include "LandmarksCommonData.h"
#include "LandmarksEngine.h"
#include "LandmarksCategoriesEngine.h"
#include "LandmarksDbEventHandler.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLandmarksApplicationEngine::CLandmarksApplicationEngine
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLandmarksApplicationEngine::CLandmarksApplicationEngine()
    {
    }

// -----------------------------------------------------------------------------
// CLandmarksApplicationEngine::::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLandmarksApplicationEngine::ConstructL()
    {
    iDb = CPosLandmarkDatabase::OpenL();
    iLandmarksEngine = CLandmarksEngine::NewL(*iDb);
    iCategoriesEngine = CLandmarksCategoriesEngine::NewL(*iDb);
    }

// -----------------------------------------------------------------------------
// CLandmarksApplicationEngine::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLandmarksApplicationEngine* CLandmarksApplicationEngine::NewL()
    {
    CLandmarksApplicationEngine* self = 
        new (ELeave) CLandmarksApplicationEngine();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CLandmarksApplicationEngine::~CLandmarksApplicationEngine()
    {
    delete iLandmarksEngine;
    delete iCategoriesEngine;
    delete iDbEventHandler;
    delete iDb;
    }

// -----------------------------------------------------------------------------
// CLandmarksApplicationEngine::NotifyViewActivated
// The engine that corresponds to an active view must have higher priority than
// engines corresponding to deactivated views.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksApplicationEngine::NotifyViewActivated(
    TUid aViewId, 
    TBool aIsActive)
    {
    if (aViewId.iUid == ELandmarksViewId)
        {
        if (aIsActive)
            {
            iLandmarksEngine->SetPrio(CActive::EPriorityStandard);
            }
        else
            {
            iLandmarksEngine->SetPrio(CActive::EPriorityLow);
            }
        }
    else if (aViewId.iUid == ELandmarksCategoriesViewId)
        {
        if (aIsActive)
            {
            iCategoriesEngine->SetPrio(CActive::EPriorityStandard);
            }
        else
            {
            iCategoriesEngine->SetPrio(CActive::EPriorityLow);
            }
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksApplicationEngine::StartInitializeDbIfNecessaryL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CLandmarksApplicationEngine::StartInitializeDbIfNecessaryL(
    MLandmarksOperationObserver* aObserver)
    {
    return iLandmarksEngine->StartInitializeDbIfNecessaryL(aObserver);
    }

// -----------------------------------------------------------------------------
// CLandmarksApplicationEngine::AddDbObserverL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksApplicationEngine::AddDbObserverL(
    MLandmarksDbObserver* aObserver)
    {
    if (!iDbEventHandler)
        {
        iDbEventHandler = new (ELeave) CLandmarksDbEventHandler(*iDb);
        }
    iDbEventHandler->AddObserverL(aObserver);
    }

// -----------------------------------------------------------------------------
// CLandmarksApplicationEngine::LandmarkLC
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CPosLandmark* CLandmarksApplicationEngine::LandmarkLC(TPosLmItemId aItemId)
    {
    return iDb->ReadLandmarkLC(aItemId);
    }

// -----------------------------------------------------------------------------
// CLandmarksApplicationEngine::CommitLandmarkL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksApplicationEngine::CommitLandmarkL(
    const CPosLandmark& aLandmark)
    {
    // We need to cancel any search/read operation in any view engine. 
    // Otherwise database will be locked.
    iLandmarksEngine->Cancel();
    iCategoriesEngine->Cancel();

    iLandmarksEngine->CommitLandmarkL(aLandmark);
    CompactIfNeededL();
    }

// -----------------------------------------------------------------------------
// CLandmarksApplicationEngine::DeleteLandmarkL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksApplicationEngine::DeleteLandmarkL(TPosLmItemId aItemId)
    {
    // We need to cancel any search/read operation in any view engine. 
    // Otherwise database will be locked.
    iLandmarksEngine->Cancel();
    iCategoriesEngine->Cancel();

    iLandmarksEngine->DeleteLandmarkL(aItemId);
    CompactIfNeededL();
    }

// -----------------------------------------------------------------------------
// CLandmarksApplicationEngine::AddLandmarkL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksApplicationEngine::AddLandmarkL(CPosLandmark& aLandmark)
    {
    // We need to cancel any search/read operation in any view engine. 
    // Otherwise database will be locked.
    iLandmarksEngine->Cancel();
    iCategoriesEngine->Cancel();

    iLandmarksEngine->AddLandmarkL(aLandmark);
    CompactIfNeededL();
    }

// -----------------------------------------------------------------------------
// CLandmarksApplicationEngine::StartSearchingLandmarksL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksApplicationEngine::StartSearchingLandmarksL(
    MLandmarksOperationObserver* aObserver)
    {
    iLandmarksEngine->StartSearchingLandmarksL(aObserver);
    }

// -----------------------------------------------------------------------------
// CLandmarksApplicationEngine::StartSearchingLandmarksL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksApplicationEngine::StartSearchingLandmarksL(
    const TDesC& aSearchPattern, 
    TBool aSearchOnlyInPreviousMatches,
    MLandmarksOperationObserver* aObserver)
    {
    iLandmarksEngine->StartSearchingLandmarksL(
        aSearchPattern, 
        aSearchOnlyInPreviousMatches, 
        aObserver);
    }

// -----------------------------------------------------------------------------
// CLandmarksApplicationEngine::FetchLandmarkSearchResultL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RArray<TPosLmItemId>* CLandmarksApplicationEngine::FetchLandmarkSearchResult()
    {
    return iLandmarksEngine->FetchSearchResult();
    }

// -----------------------------------------------------------------------------
// CLandmarksApplicationEngine::StartReadingLandmarksL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksApplicationEngine::StartReadingLandmarksL(
    TInt aNrOfItemsToReadPerBundle,
    MLandmarksOperationObserver* aObserver)
    {
    iLandmarksEngine->StartReadingLandmarksL(
        aNrOfItemsToReadPerBundle, 
        aObserver);
    }

// -----------------------------------------------------------------------------
// CLandmarksApplicationEngine::FetchLandmarksLC
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CArrayPtr<CPosLandmark>* CLandmarksApplicationEngine::FetchLandmarksLC()
    {
    return iLandmarksEngine->FetchLandmarksLC();
    }

// -----------------------------------------------------------------------------
// CLandmarksApplicationEngine::CategoryLC
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CPosLandmarkCategory* CLandmarksApplicationEngine::CategoryLC(
    TPosLmItemId aItemId)
    {
    return iCategoriesEngine->CategoryLC(aItemId);
    }

// -----------------------------------------------------------------------------
// CLandmarksApplicationEngine::DeleteCategoryL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksApplicationEngine::DeleteCategoryL(TPosLmItemId aItemId)
    {
    // We need to cancel any search/read operation in any view engine. 
    // Otherwise database will be locked.
    iLandmarksEngine->Cancel();
    iCategoriesEngine->Cancel();

    iCategoriesEngine->DeleteCategoryL(aItemId);
    CompactIfNeededL();
    }

// -----------------------------------------------------------------------------
// CLandmarksApplicationEngine::UpdateCategoryL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksApplicationEngine::UpdateCategoryL(
    const CPosLandmarkCategory& aCategory)
    {
    // We need to cancel any search/read operation in any view engine. 
    // Otherwise database will be locked.
    iLandmarksEngine->Cancel();
    iCategoriesEngine->Cancel();

    iCategoriesEngine->UpdateCategoryL(aCategory);
    CompactIfNeededL();
    }

// -----------------------------------------------------------------------------
// CLandmarksApplicationEngine::AddCategoryL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksApplicationEngine::AddCategoryL(CPosLandmarkCategory& aCategory)
    {
    // We need to cancel any search/read operation in any view engine. 
    // Otherwise database will be locked.
    iLandmarksEngine->Cancel();
    iCategoriesEngine->Cancel();

    iCategoriesEngine->AddCategoryL(aCategory);
    CompactIfNeededL();
    }

// -----------------------------------------------------------------------------
// CLandmarksApplicationEngine::StartSearchingCategoriesL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksApplicationEngine::StartSearchingCategoriesL(
    const TDesC& aSearchPattern, 
    TBool aSearchOnlyInPreviousMatches,
    MLandmarksOperationObserver* aObserver)
    {
    iCategoriesEngine->StartSearchingCategoriesL(
        aSearchPattern, 
        aSearchOnlyInPreviousMatches, 
        aObserver);
    }

// -----------------------------------------------------------------------------
// CLandmarksApplicationEngine::StartSearchingCategoriesL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksApplicationEngine::StartSearchingCategoriesL(
    MLandmarksOperationObserver* aObserver)
    {
    iCategoriesEngine->StartSearchingCategoriesL(aObserver);
    }

// -----------------------------------------------------------------------------
// CLandmarksApplicationEngine::FetchCategorySearchResult
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
RArray<TPosLmItemId>* CLandmarksApplicationEngine::FetchCategorySearchResult()
    {
    return iCategoriesEngine->FetchSearchResult();
    }

// -----------------------------------------------------------------------------
// CLandmarksApplicationEngine::StartReadingCategoriesL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksApplicationEngine::StartReadingCategoriesL(
    TInt aNrOfItemsToReadPerBundle,
    MLandmarksOperationObserver* aObserver)
    {
    iCategoriesEngine->StartReadingCategoriesL(
        aNrOfItemsToReadPerBundle, aObserver);
    }

// -----------------------------------------------------------------------------
// CLandmarksApplicationEngine::FetchCategoriesLC
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CArrayPtr<CPosLandmarkCategory>* CLandmarksApplicationEngine::FetchCategoriesLC()
    {
    return iCategoriesEngine->FetchCategoriesLC();
    }

// -----------------------------------------------------------------------------
// CLandmarksApplicationEngine::CategoriesL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CArrayPtr<CPosLandmarkCategory>* CLandmarksApplicationEngine::CategoriesL()
    {
    return iCategoriesEngine->CategoriesL();
    }

// -----------------------------------------------------------------------------
// CLandmarksApplicationEngine::CompactIfNeededL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksApplicationEngine::CompactIfNeededL()
    {
    CPosLandmarkDatabase::TSize dbSize = iDb->SizeL();
    if (dbSize.iUsage < KDbUsageLimit)
        {
        ExecuteAndDeleteLD(iDb->CompactL());
        }
    }

// End of File


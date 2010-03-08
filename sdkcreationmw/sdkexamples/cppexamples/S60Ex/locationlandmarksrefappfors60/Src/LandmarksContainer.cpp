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
* Description:  Implements the CLandmarksContainer class
*
*/


// INCLUDE FILES
#include <aknsfld.h>
#include <aknnavi.h>
#include <eikclbd.h>
#include <AknView.h>
#include <AknQueryDialog.h>

#include "LmRefApp.hrh"
#include <LmRefApp.rsg>
#include "LandmarksContainer.h"
#include "LandmarksCommonData.h"
#include "LandmarksListBoxModel.h"
#include "LandmarksModel.h"
#include "LandmarksEditDialog.h"
#include "LandmarksApplicationEngine.h"
#include "LandmarksListBox.h"

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ResetAndDestroyLandmarks resets and destroys all landmarks owned by the 
// supplied landmark array.
// Param: aAny: an instance of CArrayPtr<CPosLandmark>* to reset and destroy
// -----------------------------------------------------------------------------
//
void ResetAndDestroyLandmarks(TAny* aAny)
    {
    CArrayPtr<CPosLandmark>* landmarks = 
        reinterpret_cast <CArrayPtr<CPosLandmark>*> (aAny);
    landmarks->ResetAndDestroy();
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLandmarksContainer::CLandmarksContainer
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLandmarksContainer::CLandmarksContainer(
    CAknView& aView,
    CLandmarksApplicationEngine& aEngine,
    CAknNavigationDecorator* aNaviDecorator)
:   CLandmarksContainerBase(aView, aEngine), 
    iNaviDecorator(aNaviDecorator)
    {
    }

// -----------------------------------------------------------------------------
// CLandmarksContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLandmarksContainer::ConstructL(
    const TRect& aRect)
    {
    // Create listbox and findbox.
    CLandmarksContainerBase::ConstructL(aRect);

    // Start observer db events.
    iEngine.AddDbObserverL(this);

    // Create data model.
    iModel = CLandmarksModel::NewL();

    // Create a descriptor array that will contain the landmark names and 
    // icon indeces.
    CLandmarksListBoxModel* model = iListBox->Model();
    model->SetItemTextArray(iModel->ItemList());
    model->SetOwnershipType(ELbmDoesNotOwnItemArray);

    // Create an icon array that will be displayed in the listbox.

    CIconList* icons = iModel->CreateIconListL();
    iListBox->ItemDrawer()->ColumnData()->SetIconArray(icons);

    // Observe listbox for state changes.
    iListBox->SetObserver(this);

    // Observe findbox for state changes.
    iFindBox->SetObserver(this);

    // Populate listbox with landmarks and icons.
    StartUpdateListBoxL();
    }

// Destructor
CLandmarksContainer::~CLandmarksContainer()
    {
    delete iModel;
    delete iCurrentName;
    }

// -----------------------------------------------------------------------------
// CLandmarksContainer::MakeNaviPaneVisibleL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksContainer::MakeNaviPaneVisibleL(TBool aVisible)
    {
    CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();
    CAknNavigationControlContainer* naviPane = 
        (CAknNavigationControlContainer*) statusPane->
            ControlL(TUid::Uid(EEikStatusPaneUidNavi));

    if (!aVisible)
        {
        // Remove tabs from this view's navi pane
        naviPane->Pop(iNaviDecorator);
        }
    else
        {
        // Recreate tabs to this view's navi pane
        naviPane->PushL(*iNaviDecorator);
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksContainer::CurrentLandmark
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPosLmItemId CLandmarksContainer::CurrentLandmark()
    {
    return iModel->CurrentItemId();
    }

// -----------------------------------------------------------------------------
// CLandmarksContainer::DeleteLandmarkL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksContainer::DeleteLandmarkL()
    {
    // Extract landmark name
    TPtrC name;
    CPosLandmark* currentLandmark = 
        iEngine.LandmarkLC(iModel->CurrentItemId());
    currentLandmark->GetLandmarkName(name);

    // Format query
    HBufC* queryFormat = 
        iCoeEnv->AllocReadResourceAsDes16LC(R_LMREFAPP_DELETE_LANDMARK);
    HBufC* queryBuf = HBufC::NewLC(name.Length() + queryFormat->Length());
    TPtr query = queryBuf->Des();
    query.Format(*queryFormat, &name);

    // Launch dialog
    CAknQueryDialog* queryDialog = CAknQueryDialog::NewL();
    if (queryDialog->ExecuteLD(R_LMREFAPP_GENERAL_QUERY, query))
        {
        iEngine.DeleteLandmarkL(currentLandmark->LandmarkId());
        }

    CleanupStack::PopAndDestroy(3, currentLandmark);
    }

// -----------------------------------------------------------------------------
// CLandmarksContainer::OfferKeyEventL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TKeyResponse CLandmarksContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType)
    {
	TKeyResponse response = EKeyWasNotConsumed;

    switch (aKeyEvent.iCode)
        {
        case EKeyUpArrow:
        case EKeyDownArrow:
            // Scrolling the list box. Forward to iListBox.
            response = iListBox->OfferKeyEventL(aKeyEvent, aType);
            break;

        case EKeyDevice3:
            // Select key pressed. Edit landmark.
            iView.ProcessCommandL(ELandmarksOpen);
            response = EKeyWasConsumed;
            break;

        case EKeyBackspace:
            if ((iFindBox->TextLength() == 0) && IsItemSelected())
                {
                // If search field is empty backspace deletes a landmark.
                DeleteLandmarkL();
                response = EKeyWasConsumed;
                break;
                }
            // remove letter in find box
        default:
            // Forward key events to find box.
            response = iFindBox->OfferKeyEventL(aKeyEvent, aType);
            break;
        }

    return response;
    }

// -----------------------------------------------------------------------------
// CLandmarksContainer::NotifyOperationProgressL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksContainer::NotifyOperationProgressL(
    TOperation aOperation,
    TInt aProgress,
    TInt aErrorCode)
    {
    switch (aOperation)
        {
        case ELandmarkSearch:
            HandleSearchReadyL(aErrorCode);
            break;

        case ELandmarkRead:
            HandleReadReadyL(aErrorCode, aProgress == KOperationReady);
            break;

        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksContainer::NotifyDbEventL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksContainer::NotifyDbEventL(
    TPosLmEvent& aEvent, 
    TInt aErrorCode)
    {
    if (aErrorCode == KErrNone)
        {
        switch (aEvent.iEventType)
            {
            case EPosLmEventUnknownChanges:
            case EPosLmEventLandmarkUnknownChanges:
            case EPosLmEventCategoryUnknownChanges:
            case EPosLmEventLandmarkCreated:
            case EPosLmEventLandmarkDeleted:
                StartUpdateListBoxL();
                break;
            // Any of the 3 events below might have cancelled an ongoing 
            // landmark search/read operation => must restart refreshing list
            case EPosLmEventCategoryCreated:
            case EPosLmEventCategoryDeleted:
            case EPosLmEventCategoryUpdated:
                if (iIsActive)
                    {
                    StartUpdateListBoxL();
                    }
                break;
            case EPosLmEventLandmarkUpdated:
                if (IsRefreshNeededL(aEvent.iLandmarkItemId))
                    {
                    StartUpdateListBoxL();
                    }
                break;
            case EPosLmEventNewDefaultDatabaseLocation:
                // Not supported by this application
                break;
            case EPosLmEventMediaRemoved:
                // Not supported by this application
                break;
            default:
                // Ignore other category events
                break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksContainer::HandleControlEventL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksContainer::HandleControlEventL(
    CCoeControl* aControl, 
    TCoeEvent aEventType)
    {
	if (aEventType == EEventStateChanged && aControl == iListBox)
		{
		// Take the index of the current item in the list
		TInt itemIndex = iListBox->CurrentItemIndex();

		// Set the current item in the model object regarding to the list box.
		iModel->SetCurrentItem(itemIndex);

        // Store the name of the current landmark
        StoreCurrentLandmarkNameL();
        }

	if (aEventType == EEventStateChanged && aControl == iFindBox)
		{
        // Fetch filter from search field
        TInt searchTextLength = iFindBox->TextLength();
        HBufC* filterBuf = HBufC::NewLC(searchTextLength);
        TPtr filter = filterBuf->Des();
        iFindBox->GetSearchText(filter);

        // If filter has changed repopulate list box. (It is necessary to check
        // if filter has changed since each filter change generates several 
        // EEventStateChanged events for find box)
        if (filter != *iOldFilter)
            {
            // Filter has changed. Start filtering.
            StartUpdateListBoxL();
            }

        CleanupStack::PopAndDestroy(filterBuf);
		}
    }

// -----------------------------------------------------------------------------
// CLandmarksContainer::StartUpdateListBoxL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksContainer::StartUpdateListBoxL()
    {
    // Fetch filter from find box
    TInt searchTextLength = iFindBox->TextLength();
    const TInt KExtraChars = 2; // 2 chars wildcards
    HBufC* filterBuf = HBufC::NewLC(searchTextLength + KExtraChars);
    TPtr filter = filterBuf->Des();
    iFindBox->GetSearchText(filter);

    // Find out if there is any prevoius result to limit the search to
    TInt oldFilterLength = iOldFilter->Length();
    TBool searchOnlyInPreviousMatches = 
        // If previous filter is empty there is no search results
        oldFilterLength > 0 && 

        // If chars have been removed from filter we abandon previous results
        filter.Length() > oldFilterLength && 

        // There must exist at least one previous match
        IsItemSelected();

    // old filter used - now we can update it
    delete iOldFilter;
    iOldFilter = NULL;
    iOldFilter = filter.AllocL();

	// Start to asynchronously populate the listbox model according to filter
    if (searchTextLength == 0)
        {
        iEngine.StartSearchingLandmarksL(this);
        }
    else
        {
        filter.Insert(0, KWildCard);
        filter.Append(KWildCard);
        iEngine.StartSearchingLandmarksL(
            filter, searchOnlyInPreviousMatches, this);
        }

    // Indicate we are active
    iIsActive = ETrue;

    CleanupStack::PopAndDestroy(filterBuf);
    }

// -----------------------------------------------------------------------------
// CLandmarksContainer::HandleSearchReadyL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksContainer::HandleSearchReadyL(TInt aErrorCode)
    {

    // Delete old icon array and add new
	CColumnListBoxData* columnData = iListBox->ItemDrawer()->ColumnData();
    CArrayPtr<CGulIcon>* iconList = columnData->IconArray();
    iconList->ResetAndDestroy();
    delete iconList;
    columnData->SetIconArray(iModel->CreateIconListL());

    if (aErrorCode == KErrNone)
        {
        // Update model
        iModel->SetItemIds(iEngine.FetchLandmarkSearchResult());
        iListBox->HandleItemRemovalL();

        iEngine.StartReadingLandmarksL(KNrOfItemsDisplayed, this);
        iReadingFirstPage = ETrue;
        }
    else if (aErrorCode == KErrNotFound)
        {
        // Updating list ready
        iIsActive = EFalse;

        // Update model (indicate no item is selected)
        iModel->SetItemIds(NULL);
        iModel->SetCurrentItem(iListBox->CurrentItemIndex());

		// Update the view
        iListBox->HandleItemRemovalL();
        iListBox->DrawDeferred();
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksContainer::HandleReadReadyL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksContainer::HandleReadReadyL(TInt aErrorCode, TBool aFinished)
    {
    // Update model with new landmarks
    CArrayPtr<CPosLandmark>* landmarks = iEngine.FetchLandmarksLC();
    CleanupStack::PushL(TCleanupItem(ResetAndDestroyLandmarks, landmarks));
    iModel->RepopulateModelL(landmarks);
    CleanupStack::PopAndDestroy(2, landmarks);

    if (aErrorCode == KErrNone)
        {
        if (aFinished)
            {
            // Updating list ready
            iIsActive = EFalse;
            }
        if (iReadingFirstPage)
            {
            // First page ready
            iReadingFirstPage = EFalse;

            // Add the first page
            iListBox->HandleItemAdditionL();

            // Set the first item selected by default. We must do this since 
            // HandleItemRemovalL in HandleSearchReadyL resets the listbox.
            iModel->SetCurrentItem(0);
            iListBox->SetCurrentItemIndex(0);
            StoreCurrentLandmarkNameL();
            }
        else
            {
            iListBox->HandleItemAdditionL();
            }
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksContainer::IsRefreshNeededL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CLandmarksContainer::IsRefreshNeededL(TPosLmItemId& aModifiedLandmark)
    {
    if (iIsActive)
        {
        return ETrue;
        }
    else
        {
        if (aModifiedLandmark == iModel->CurrentItemId())
            {
            // Investigate if we need to refresh. Compare name.
            CPosLandmark* landmark = iEngine.LandmarkLC(aModifiedLandmark);
            TPtrC name;
            landmark->GetLandmarkName(name);
            TBool isNameDifferent = 
                iCurrentName && *iCurrentName != name ||
                !iCurrentName && name != KNullDesC;
            CleanupStack::PopAndDestroy(landmark);

            // If name is different the list needs to be re-sorted. Otherwise 
            // some other field has been modified and the list is not affected.
            return isNameDifferent;
            }
        else 
            {
            // Some other process than this has modified a landmark
            return ETrue;
            }
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksContainer::StoreCurrentLandmarkNameL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksContainer::StoreCurrentLandmarkNameL()
    {
    CPosLandmark* landmark = iEngine.LandmarkLC(iModel->CurrentItemId());
    TPtrC name;
    landmark->GetLandmarkName(name);
    delete iCurrentName;
    iCurrentName = NULL;
    iCurrentName = name.AllocL();
    CleanupStack::PopAndDestroy(landmark);
    }

// End of File


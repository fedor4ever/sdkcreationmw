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
* Description:  Implements the CLandmarksCategoriesContainer class
*
*/


// INCLUDE FILES
#include <aknsfld.h>
#include <aknquerydialog.h>
#include <eikclbd.h>
#include <eikmenub.h>

#include <EPos_CPosLandmarkCategory.h>

#include <LmRefApp.rsg>
#include "LandmarksCategoriesContainer.h"
#include "LandmarksCommonData.h"
#include "LandmarksListBox.h"
#include "LandmarksListBoxModel.h"
#include "LandmarksApplicationEngine.h"
#include "LandmarksCategoriesModel.h"

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ResetAndDestroyCategories
// Resets and destroys an array of categories. This function is used as a 
// TCleanupOperation in CLandmarksCategoriesModel.
// Params: aAny: The categories array to reset ands destroy
// -----------------------------------------------------------------------------
//
void ResetAndDestroyCategories(TAny* aAny)
    {
    CArrayPtr<CPosLandmarkCategory>* categories = 
        reinterpret_cast <CArrayPtr<CPosLandmarkCategory>*> (aAny);
    categories->ResetAndDestroy();
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLandmarksCategoriesContainer::CLandmarksCategoriesContainer
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLandmarksCategoriesContainer::CLandmarksCategoriesContainer(
    CAknView& aView,
    CLandmarksApplicationEngine& aEngine, 
    CEikMenuBar& aMenuBar)
:   CLandmarksContainerBase(aView, aEngine), 
    iMenuBar(aMenuBar)
    {
    }

// -----------------------------------------------------------------------------
// CLandmarksCategoriesContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesContainer::ConstructL(
    const TRect& aRect)
    {
    // Create listbox and findbox.
    CLandmarksContainerBase::ConstructL(aRect);

    // Start observer db events.
    iEngine.AddDbObserverL(this);

    // Create data model.
    iModel = CLandmarksCategoriesModel::NewL();

    // Create a descriptor array that will contain the landmark names and 
    // icon indeces.
    CLandmarksListBoxModel* listBoxModel = iListBox->Model();
    listBoxModel->SetItemTextArray(iModel->ItemList());
    listBoxModel->SetOwnershipType(ELbmDoesNotOwnItemArray);

    // Create an icon array the will be displayed in the listbox.

    CIconList* icons = iModel->CreateIconListL();
    iListBox->ItemDrawer()->ColumnData()->SetIconArray(icons);

    // Observe listbox for state changes.
    iListBox->SetObserver(this);

    // Observe findbox for state changes.
    iFindBox->SetObserver(this);

    // Populate listbox with categories and icons.
    StartUpdateListBoxL();
    }

// Destructor
CLandmarksCategoriesContainer::~CLandmarksCategoriesContainer()
    {
    delete iModel;
    }

// -----------------------------------------------------------------------------
// CLandmarksCategoriesContainer::DeleteCategoryL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesContainer::DeleteCategoryL()
    {
    // Format query.
    HBufC* queryFormat = 
        iCoeEnv->AllocReadResourceAsDes16LC(R_LMREFAPP_DELETE_CATEGORY);

    CPosLandmarkCategory* category = iEngine.CategoryLC(CurrentCategoryId());
    TPtrC name;
    category->GetCategoryName(name);
    
    HBufC* query = HBufC::NewLC(name.Length() + queryFormat->Length());
    query->Des().Format(*queryFormat, &name);

    // Launch dialog.
    CAknQueryDialog* queryDialog = CAknQueryDialog::NewL();
    if (queryDialog->ExecuteLD(R_LMREFAPP_GENERAL_QUERY, *query))
        {
        // Delete category if positive response.
        iEngine.DeleteCategoryL(category->CategoryId());
        }
    CleanupStack::PopAndDestroy(3, queryFormat);
    }

// -----------------------------------------------------------------------------
// CLandmarksCategoriesContainer::CurrentCategoryId
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPosLmItemId CLandmarksCategoriesContainer::CurrentCategoryId()
    {
    return iModel->CurrentItemId();
    }

// -----------------------------------------------------------------------------
// CLandmarksCategoriesContainer::IsGlobalL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CLandmarksCategoriesContainer::IsGlobalL()
    {
    CPosLandmarkCategory* category = iEngine.CategoryLC(CurrentCategoryId());
    TBool isGlobal = (category->GlobalCategory() != KPosLmNullGlobalCategory);
    CleanupStack::PopAndDestroy(category);

    return isGlobal;
    }

// -----------------------------------------------------------------------------
// CLandmarksCategoriesContainer::OfferKeyEventL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TKeyResponse CLandmarksCategoriesContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, TEventCode aType)
    {
	TKeyResponse response = EKeyWasNotConsumed;

    switch (aKeyEvent.iCode)
        {
        case EKeyUpArrow:
        case EKeyDownArrow:
            // Scrolling the listbox. Forward to iListBox.
            response = iListBox->OfferKeyEventL(aKeyEvent, aType);
            break;

        case EKeyDevice3:
            // Select key pressed. Display menu.
            iMenuBar.SetMenuTitleResourceId(R_LMREFAPP_CATEGORIES_MENUBAR);
            iMenuBar.TryDisplayMenuBarL();
            response = EKeyWasConsumed;
            break;

        case EKeyBackspace:
            if (iFindBox->TextLength() == 0 &&
                IsItemSelected() &&
                !IsGlobalL())
                {
                // If there is no text in the search field backspace will 
                // delete a category.
                DeleteCategoryL();
                response = EKeyWasConsumed;
                break;
                }
            // erase character in find box
        default:
            // Forward to findbox.
            response =  iFindBox->OfferKeyEventL(aKeyEvent, aType);
            break;
        }

    return response;
    }

// -----------------------------------------------------------------------------
// CLandmarksCategoriesContainer::NotifyOperationProgressL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesContainer::NotifyOperationProgressL(
    TOperation aOperation, 
    TInt /*aProgress*/, 
    TInt aErrorCode)
    {
    switch (aOperation)
        {
        case ECategorySearch:
            HandleSearchReadyL(aErrorCode);
            break;

        case ECategoryRead:
            HandleReadReadyL(aErrorCode);
            break;

        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksCategoriesContainer::NotifyDbEventL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesContainer::NotifyDbEventL(
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
            // Any of the events below might have cancelled an ongoing 
            // categories search/read operation => we must restart refreshing listbox
            case EPosLmEventLandmarkCreated:
            case EPosLmEventLandmarkDeleted:
            case EPosLmEventLandmarkUpdated:
            case EPosLmEventCategoryCreated:
            case EPosLmEventCategoryDeleted:
            case EPosLmEventCategoryUpdated:
                StartUpdateListBoxL();
                break;
            case EPosLmEventNewDefaultDatabaseLocation:
                // Not supported by this application
                break;
            case EPosLmEventMediaRemoved:
                // Not supported by this application
                break;
            default:
                // Ignore other non-category events
                break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksCategoriesContainer::HandleControlEventL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesContainer::HandleControlEventL(
    CCoeControl* aControl, 
    TCoeEvent aEventType)
    {
	if (aEventType == EEventStateChanged && aControl == iListBox)
		{
		// Take the index of the current item in the list
		TInt itemIndex = iListBox->CurrentItemIndex();

		// Set the current item in the model object regarding to the list box.
		iModel->SetCurrentItem(itemIndex);
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
// CLandmarksCategoriesContainer::StartUpdateListBoxL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesContainer::StartUpdateListBoxL()
    {
    // Fetch filter from search field
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
    if (filter.Length() == 0)
        {
        iEngine.StartSearchingCategoriesL(this);
        }
    else
        {
        filter.Append(KWildCard);
        iEngine.StartSearchingCategoriesL(
            filter, 
            searchOnlyInPreviousMatches, 
            this);
        }

    CleanupStack::PopAndDestroy(filterBuf);
    }

// -----------------------------------------------------------------------------
// CLandmarksCategoriesContainer::HandleSearchReadyL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesContainer::HandleSearchReadyL(TInt aErrorCode)
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
        iModel->SetItemIds(iEngine.FetchCategorySearchResult());
        iListBox->HandleItemRemovalL();

        iEngine.StartReadingCategoriesL(KNrOfItemsDisplayed, this);
        iReadingFirstPage = ETrue;
        }
    else if (aErrorCode == KErrNotFound)
        {
        // Update model (indicate no item is selected)
        iModel->SetItemIds(NULL);
        iModel->SetCurrentItem(iListBox->CurrentItemIndex());

        // Update the listbox
        iListBox->HandleItemRemovalL();
        iListBox->DrawDeferred();
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksCategoriesContainer::HandleReadReadyL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesContainer::HandleReadReadyL(
    TInt aErrorCode)
    {
    // Update model with new categories
    CArrayPtr<CPosLandmarkCategory>* categories = iEngine.FetchCategoriesLC();
    CleanupStack::PushL(TCleanupItem(ResetAndDestroyCategories, categories));
    iModel->RepopulateModelL(categories);
    CleanupStack::PopAndDestroy(2, categories);

    if (aErrorCode == KErrNone)
        {
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
            }
        else
            {
            iListBox->HandleItemAdditionL();
            }
        }
    }

// End of File


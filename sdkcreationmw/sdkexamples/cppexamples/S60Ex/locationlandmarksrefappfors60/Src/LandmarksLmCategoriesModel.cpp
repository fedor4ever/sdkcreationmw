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
* Description:  Implements the CLandmarksLmCategoriesModel class
*
*/


// INCLUDE FILES
#include <eikenv.h>
#include <gulicon.h>
#include <akniconarray.h>
#include <avkon.mbg>

#include <EPos_CPosLandmarkCategory.h>

#include "LandmarksUtils.h"
#include "LandmarksCommonData.h"
#include "LandmarksLmCategoriesModel.h"
#include "LandmarksApplicationEngine.h"

// CONSTANTS
_LIT(KItemFormat, "\t%S");

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLandmarksLmCategoriesModel::CLandmarksLmCategoriesModel
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLandmarksLmCategoriesModel::CLandmarksLmCategoriesModel(
    RArray<TPosLmItemId>& aMarkedCategories)
:   iMarkedCategoryIds(aMarkedCategories)
	{
	}

// -----------------------------------------------------------------------------
// CLandmarksLmCategoriesModel::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLandmarksLmCategoriesModel::ConstructL(CLandmarksApplicationEngine& aEngine)
	{
    iCategories = aEngine.CategoriesL();
	}

// -----------------------------------------------------------------------------
// CLandmarksLmCategoriesModel::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLandmarksLmCategoriesModel* CLandmarksLmCategoriesModel::NewL(
    CLandmarksApplicationEngine& aEngine,
    RArray<TPosLmItemId>& aMarkedCategories)
    {
    CLandmarksLmCategoriesModel* self = new (ELeave) 
        CLandmarksLmCategoriesModel(aMarkedCategories);
    CleanupStack::PushL(self);
    self->ConstructL(aEngine);
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CLandmarksLmCategoriesModel::~CLandmarksLmCategoriesModel()
	{
    if (iCategories)
        {
        iCategories->ResetAndDestroy();
	    delete iCategories;
        }
	}

// -----------------------------------------------------------------------------
// CLandmarksLmCategoriesModel::SelectCategoriesL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksLmCategoriesModel::SelectCategoriesL(
    CArrayFixFlat<TInt>& aSelectedIndexes) 
    {
    // clear current list of selected categories
    iMarkedCategoryIds.Reset();

    for (TInt i = 0; i < aSelectedIndexes.Count(); i++)
        {
        TInt categoryIndex = aSelectedIndexes[i];

        // Fetch the id of the selected item.
        __ASSERT_ALWAYS((categoryIndex >= 0) && (categoryIndex < iCategories->Count()),
                        LandmarksUtils::Panic(KErrGeneral));
        TPosLmItemId selectedCategoryId = (*iCategories)[categoryIndex]->CategoryId();

        User::LeaveIfError(iMarkedCategoryIds.Append(selectedCategoryId));
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksLmCategoriesModel::SelectedCategoriesL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CArrayFixFlat<TInt>* CLandmarksLmCategoriesModel::SelectedCategoriesL() 
    {
    CArrayFixFlat<TInt>* selectedItems = new (ELeave) CArrayFixFlat<TInt>(KGranularity);
    CleanupStack::PushL(selectedItems);

    for (TInt i = 0; i < iCategories->Count(); i++)
        {
        TPosLmItemId categoryId = (*iCategories)[i]->CategoryId();

		TInt index = iMarkedCategoryIds.Find(categoryId);
        if (KErrNotFound != index)
        	{
	        selectedItems->AppendL(i);
        	}
        }

	CleanupStack::Pop(selectedItems);        
	return selectedItems;        
    }

// -----------------------------------------------------------------------------
// CLandmarksLmCategoriesModel::FormattedCategoryNamesL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CDesCArray* CLandmarksLmCategoriesModel::FormattedCategoryNamesL()
    {
    // Granularity is set to be 8.
    CDesCArrayFlat* listItems = new (ELeave) CDesCArrayFlat(KGranularity);
    CleanupStack::PushL(listItems);

    for (TInt i = 0; i < iCategories->Count(); i++)
        {
        // Create buffer that will contain the name and mark status. 3 extra 
        // chars for tab and icon index.
        const TInt KExtraChars = 3; 
        TPtrC categoryName;
        User::LeaveIfError((*iCategories)[i]->GetCategoryName(categoryName));
        HBufC* formattedCategoryName = HBufC::NewLC(categoryName.Length() + KExtraChars);

        // format string with category
        formattedCategoryName->Des().Format(KItemFormat, &categoryName);

        listItems->AppendL(*formattedCategoryName);
        CleanupStack::PopAndDestroy(formattedCategoryName);
        }

    CleanupStack::Pop(listItems);

    iListItems = listItems;
    return iListItems;
    }

// End of File


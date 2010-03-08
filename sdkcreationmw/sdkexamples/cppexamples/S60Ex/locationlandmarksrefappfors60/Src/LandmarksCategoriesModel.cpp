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
* Description:  Implements the CLandmarksCategoriesModel class and 
					some help functions
*
*/


// INCLUDE FILES
#include <eikenv.h>
#include <gulicon.h>
#include <AknsUtils.h>
#include <AknsItemID.h>

#include <EPos_CPosLandmarkCategory.h>

#include <LmRefApp.mbg>
#include "LandmarksCommonData.h"
#include "LandmarksCategoriesModel.h"

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// DefaultIconIndex returns the icon index of the appropriate default icon.
// User defined categories have different default icons than global ones.
//
// Returns: an icon index
// Param:   aCategory the category to investigate
// -----------------------------------------------------------------------------
//
TInt DefaultIconIndex(CPosLandmarkCategory& aCategory)
    {
    // Global categories have different default icon than user-defined.
    TBool isGlobal = aCategory.GlobalCategory() != KPosLmNullGlobalCategory;

    if (isGlobal)
        {
        return KDefaultGlobalIconIndex;
        }
    else
        {
        return KDefaultUserIconIndex;
        }
    }

// -----------------------------------------------------------------------------
// ResetAndDestroyIcons
// Resets and destroys an array of icons. This function is used as a 
// TCleanupOperation.
// Params: aAny: The icons array to reset ands destroy
// -----------------------------------------------------------------------------
//
void ResetAndDestroyIcons(TAny* aAny)
    {
    CIconList* icons = reinterpret_cast <CIconList*> (aAny);
    for (TInt i = 0; i < icons->Count(); i++)
        {
        delete &icons[i];
        }
    icons->Reset();
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLandmarksCategoriesModel::CLandmarksCategoriesModel
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLandmarksCategoriesModel::CLandmarksCategoriesModel()
	{
	}

// -----------------------------------------------------------------------------
// CLandmarksCategoriesModel::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesModel::ConstructL()
	{
	// The list box model uses MDesCArray interface class to take in text items.
	iListItems = new(ELeave) CDesCArraySeg(KGranularity);
	}

// -----------------------------------------------------------------------------
// CLandmarksCategoriesModel::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLandmarksCategoriesModel* CLandmarksCategoriesModel::NewL()
    {
    CLandmarksCategoriesModel* self = 
        new (ELeave) CLandmarksCategoriesModel();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CLandmarksCategoriesModel::~CLandmarksCategoriesModel()
	{
	delete iListItems;
	}

// -----------------------------------------------------------------------------
// CLandmarksCategoriesModel::SetItemIds
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesModel::SetItemIds(RArray<TPosLmItemId>* aItemIds)
    {
    iItemIds = aItemIds;

    // Empty descriptor model
    iListItems->Reset();
    }

// -----------------------------------------------------------------------------
// CLandmarksCategoriesModel::ItemList
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CDesCArraySeg* CLandmarksCategoriesModel::ItemList()
    {
    return iListItems;
    }

// -----------------------------------------------------------------------------
// CLandmarksCategoriesModel::SetCurrentItem
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesModel::SetCurrentItem(TInt aIndex) 
    {
    iCurrentItem = aIndex;
    }

// -----------------------------------------------------------------------------
// CLandmarksCategoriesModel::CurrentItemId
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPosLmItemId CLandmarksCategoriesModel::CurrentItemId()
    {
    if (iCurrentItem < 0 || iCurrentItem > iItemIds->Count() - 1)
        {
        return KPosLmNullItemId;
        }

    return (*iItemIds)[iCurrentItem];
    }

// -----------------------------------------------------------------------------
// CLandmarksCategoriesModel::CreateIconListL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CIconList* CLandmarksCategoriesModel::CreateIconListL()
    {
    CIconList* iconList = new (ELeave) CArrayPtrFlat<CGulIcon>(KGranularity);
    CleanupStack::PushL(TCleanupItem(ResetAndDestroyIcons, iconList));
    
    // Create the global category icon consisting of its image and mask.
    TAknsItemID aknsItemId ={0,0};
    CGulIcon* icon = AknsUtils::CreateGulIconL(NULL, aknsItemId,
        KLandmarksMbmFileName, 
        EMbmLmrefappCategory_global, 
        EMbmLmrefappCategory_global_mask);
    CleanupStack::PushL(icon);
    icon->SetBitmapsOwnedExternally(EFalse); 
    iconList->AppendL(icon);
    CleanupStack::Pop(icon);

    // Create the user defined category icon consisting of its image and mask.
    icon = AknsUtils::CreateGulIconL(NULL, aknsItemId,
        KLandmarksMbmFileName, 
        EMbmLmrefappCategory_user, 
        EMbmLmrefappCategory_user_mask);
    CleanupStack::PushL(icon);
    icon->SetBitmapsOwnedExternally(EFalse); 
    iconList->AppendL(icon);
    CleanupStack::Pop(icon);
    
    CleanupStack::Pop(); // iconList

    // initialze iIconList. We don't need to delete any possible previous 
    // iIconList since ownership is transferred to calling object.
    iIconList = iconList;

    return iconList;
    }

// -----------------------------------------------------------------------------
// CLandmarksCategoriesModel::RepopulateModelL
// Refreshes a part of the model. Those categories that have been read from the 
// database overwrites old categories. The model is refreshed incrementally 
// from top to bottom.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesModel::RepopulateModelL(
    CArrayPtr<CPosLandmarkCategory>* aCategories)
    {
    TInt nrOfReadItems = aCategories->Count();

    // Format and insert the read categories
    for (TInt i = 0; i < nrOfReadItems; i++)
        {
        // Format name and icon
        CPosLandmarkCategory* category = (*aCategories)[i];
        HBufC* formattedName = FormatListItemLC(*category);

        // Insert formatted descriptor into model
        iListItems->AppendL(*formattedName);

        CleanupStack::PopAndDestroy(formattedName);
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksCategoriesModel::FormatListItemLC
// Formats a list item to the following format "X\tcategoryName"
// where X = icon index
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC* CLandmarksCategoriesModel::FormatListItemLC(CPosLandmarkCategory& aCategory)
    {
    // Extract name
    TPtrC name;
    aCategory.GetCategoryName(name);
    // 10 extra chars for icon index and column separator
    const TInt KExtraChars = 10;
    HBufC* formattedNameBuf = HBufC::NewLC(name.Length() + KExtraChars);
    TPtr formattedNamePtr = formattedNameBuf->Des();

    // Extract icon info
    TPtrC mbmIconFile;
    TInt iconIndex, maskIndex;
    TInt res = aCategory.GetIcon(mbmIconFile, iconIndex, maskIndex);

    // Format list item
    if (res == KErrNone)
        {
        // Create icon and append it to icon array. 
        TRAPD(err, AppendIconL(mbmIconFile, iconIndex, maskIndex));
        if (err == KErrNone)
            {
            // Append the array index where the icon is appended
            formattedNamePtr.AppendNum(iIconList->Count() - 1);
            }
        else // Unable to create icon, use appropriate default icon.
            {
            formattedNamePtr.AppendNum(DefaultIconIndex(aCategory));
            }
        }
    else // res == KErrNotFound
        {
        formattedNamePtr.AppendNum(DefaultIconIndex(aCategory));
        }
    formattedNamePtr.Append(KTab);
    formattedNamePtr.Append(name);

    return formattedNameBuf;
    }

// -----------------------------------------------------------------------------
// CLandmarksCategoriesModel::AppendIconL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesModel::AppendIconL(
    const TDesC& aMbmFile, 
    TInt aIconOffset, 
    TInt aMaskOffset)
    {
    TAknsItemID aknsItemId = {0,0};
    CGulIcon* icon = AknsUtils::CreateGulIconL(NULL, aknsItemId,
        aMbmFile, 
        aIconOffset, 
        aMaskOffset);
    CleanupStack::PushL(icon);
    icon->SetBitmapsOwnedExternally(EFalse);
    iIconList->AppendL(icon);
    CleanupStack::Pop(icon);
    }

// End of File

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
* Description:  Implements the CLandmarksModel class
*
*/


// INCLUDE FILES
#include <eikenv.h>
#include <gulicon.h>
#include <AknsUtils.h>
#include <AknsItemID.h>
#include <AknIconArray.h>

#include <EPos_CPosLandmark.h>

#include <LmRefApp.mbg>
#include "LandmarksCommonData.h"
#include "LandmarksModel.h"

// CONSTANTS
const TInt KDefaultIconIndex = 0;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLandmarksModel::CLandmarksModel
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLandmarksModel::CLandmarksModel()
	{
	}

// -----------------------------------------------------------------------------
// CLandmarksModel::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLandmarksModel::ConstructL()
	{
	// The list box model uses MDesCArray interface class to take in text items.
	iListItems = new (ELeave) CDesCArraySeg(KGranularity);
	}

// -----------------------------------------------------------------------------
// CLandmarksModel::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLandmarksModel* CLandmarksModel::NewL()
    {
    CLandmarksModel* self = new (ELeave) CLandmarksModel();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CLandmarksModel::~CLandmarksModel()
	{
	delete iListItems;
	}

// -----------------------------------------------------------------------------
// CLandmarksModel::SetItemIds
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksModel::SetItemIds(RArray<TPosLmItemId>* aItemIds)
    {
    iItemIds = aItemIds;

    // Empty descriptor model
    iListItems->Reset();
    }

// -----------------------------------------------------------------------------
// CLandmarksModel::ItemList
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CDesCArraySeg* CLandmarksModel::ItemList()
    {
    return iListItems;
    }
		
// -----------------------------------------------------------------------------
// CLandmarksModel::SetCurrentItem
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksModel::SetCurrentItem(TInt aIndex) 
    {
    iCurrentItem = aIndex;
    }

// -----------------------------------------------------------------------------
// CLandmarksModel::CurrentItemId
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPosLmItemId CLandmarksModel::CurrentItemId()
    {
    if (!iItemIds || iCurrentItem < 0 || 
        iCurrentItem > iItemIds->Count() - 1)
        {
        return KPosLmNullItemId;
        }

    return (*iItemIds)[iCurrentItem];
    }

// -----------------------------------------------------------------------------
// CLandmarksModel::CreateIconListL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CIconList* CLandmarksModel::CreateIconListL()
    {
    CIconList* iconList = new (ELeave) CAknIconArray( KGranularity );

    CleanupStack::PushL(iconList);
    
    // Create the default landmark icon consisting of its image and mask
    TAknsItemID aknsItemId = {0,0};
    CGulIcon* icon = AknsUtils::CreateGulIconL(NULL, aknsItemId,
        KLandmarksMbmFileName, 
        EMbmLmrefappDefault_lm, 
        EMbmLmrefappDefault_lm_mask);
    CleanupStack::PushL(icon);
    icon->SetBitmapsOwnedExternally(EFalse);
    iconList->AppendL(icon);
    CleanupStack::Pop(2, iconList);

    // initialze iIconList. We don't need to delete any possible previous 
    // iIconList since ownership is transferred to calling object.
    iIconList = iconList;

    return iIconList;
    }

// -----------------------------------------------------------------------------
// CLandmarksModel::RepopulateModelL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksModel::RepopulateModelL(
    CArrayPtr<CPosLandmark>* aLandmarks)
    {
    TInt nrOfReadItems = aLandmarks->Count();

    // Format and insert the read landmarks
    for (TInt i = 0; i < nrOfReadItems; i++)
        {
        CPosLandmark* landmark = (*aLandmarks)[i];
        HBufC* formattedListItem = FormatListItemLC(*landmark);
        iListItems->AppendL(*formattedListItem);
        CleanupStack::PopAndDestroy(formattedListItem);
        }
    }

// -----------------------------------------------------------------------------
// CLandmarksModel::FormatListItemLC
// Formats a list item to the following format "X\tlandmarkName\t"
// where X = icon index
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
HBufC* CLandmarksModel::FormatListItemLC(CPosLandmark& aLandmark)
    {
    // Extract name
    TPtrC name;
    aLandmark.GetLandmarkName(name);
    // 10 extra chars for icon index and column separator
    const TInt KExtraChars = 10;
    HBufC* formattedNameBuf = HBufC::NewLC(name.Length() + KExtraChars);
    TPtr formattedNamePtr = formattedNameBuf->Des();

    // Extract icon info
    TPtrC mbmIconFile;
    TInt iconIndex, maskIndex;
    TInt res = aLandmark.GetIcon(mbmIconFile, iconIndex, maskIndex);

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
        else
            {
            formattedNamePtr.AppendNum(KDefaultIconIndex);
            }
        }
    else // res == KErrNotFound
        {
        formattedNamePtr.AppendNum(KDefaultIconIndex);
        }
    formattedNamePtr.Append(KTab);
    formattedNamePtr.Append(name);

    return formattedNameBuf;
    }

// -----------------------------------------------------------------------------
// CLandmarksModel::AppendIconL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksModel::AppendIconL(
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


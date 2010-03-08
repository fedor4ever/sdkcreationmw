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
* Description:  Implements the CLandmarksCategoriesDialog class
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <akntitle.h> 
#include <aknappui.h>
#include <eikmenup.h>

#include "LmRefApp.hrh"
#include <LmRefApp.rsg>
#include "LandmarksLmCategoriesModel.h"
#include "LandmarksApplicationEngine.h"
#include "LandmarksCategoriesDialog.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLandmarksCategoriesDialog::CLandmarksCategoriesDialog
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLandmarksCategoriesDialog::CLandmarksCategoriesDialog(
    TInt& aSelectedItem,
    CArrayFixFlat<TInt>* aMarkedItems,
    CDesCArray* aListItems)
    : 
    CAknMarkableListDialog(aSelectedItem, aMarkedItems, aListItems, -1, -1, NULL)
	{
	}

// Destructor
CLandmarksCategoriesDialog::~CLandmarksCategoriesDialog()
	{
    // Restore title pane. Ownership of iOriginalTitle transferred.
    if (iOriginalTitle && iTitlePane)
        {
        iTitlePane->SetText(iOriginalTitle);
        }
	}

// -----------------------------------------------------------------------------
// CLandmarksCategoriesDialog::RunDialogL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
TInt CLandmarksCategoriesDialog::RunDialogL(
    CLandmarksLmCategoriesModel& aLmCategoriesModel)
    {
    // Create array of marked indeces
    CArrayFixFlat<TInt>* markedItems = aLmCategoriesModel.SelectedCategoriesL();
    CleanupStack::PushL(markedItems);

    // Create name array
    CDesCArray* listItems = aLmCategoriesModel.FormattedCategoryNamesL();
    CleanupStack::PushL(listItems);

    // Create the dialog
    TInt selectedItem = 0;

    CLandmarksCategoriesDialog* dialog = 
        new (ELeave) CLandmarksCategoriesDialog(
            selectedItem, markedItems, listItems);
    
	dialog->PrepareLC(R_LMREFAPP_EDIT_CATEGORIES_DIALOG);

	// pre-mark currently selected categories 
    CEikListBox* listBox = (CEikListBox*) dialog->Control(ESelectionListControl);
    for (TInt i = 0; i < markedItems->Count(); i++)
    {
        listBox->View()->SelectItemL((*markedItems)[i]);
    }

    TInt res = dialog->RunLD();
    if (res)
        {
        aLmCategoriesModel.SelectCategoriesL(*markedItems);
        }
        
    CleanupStack::PopAndDestroy(2, markedItems);
    return res;
    }

// -----------------------------------------------------------------------------
// CLandmarksCategoriesDialog::PostLayoutDynInitL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesDialog::PostLayoutDynInitL()
    {
    CAknMarkableListDialog::PostLayoutDynInitL();
    
    // Save original title
    CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();
    iTitlePane = (CAknTitlePane*) statusPane->ControlL(
        TUid::Uid(EEikStatusPaneUidTitle));
    iOriginalTitle = iTitlePane->Text()->AllocL();

    // Update title
    HBufC* title = iCoeEnv->AllocReadResourceAsDes16L(
        R_LMREFAPP_EDIT_CAT_TITLE);
    iTitlePane->SetText(title);
    }

// -----------------------------------------------------------------------------
// CLandmarksCategoriesDialog::OfferKeyEventL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TKeyResponse CLandmarksCategoriesDialog::OfferKeyEventL(
    const TKeyEvent& aKeyEvent, 
    TEventCode aType)
    {
    if (aKeyEvent.iCode == EKeyDevice3)
        {
        // An item should be marked/unmarked.
        CEikListBox* listBox = (CEikListBox*) Control(ESelectionListControl);
        listBox->View()->ToggleItemL(listBox->View()->CurrentItemIndex());
        listBox->DrawDeferred();
        return EKeyWasConsumed;
        }

    return CAknMarkableListDialog::OfferKeyEventL(aKeyEvent, aType);
    }

// End of File


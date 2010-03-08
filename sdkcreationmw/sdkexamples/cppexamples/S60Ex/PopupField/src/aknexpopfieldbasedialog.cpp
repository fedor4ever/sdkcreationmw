/*
* Copyright (c) 2004 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/


// INCLUDE FILES
#include <eikmenub.h>
#include <eikappui.h>
#include <eikenv.h>
#include <avkon.hrh>
#include <avkon.rsg>
#include "AknExPopFieldBaseDialog.h"


// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CAknExPopFieldBaseDialog::CAknExPopFieldBaseDialog()
// C++ default constructor can NOT contain any code, that might leave.
// ----------------------------------------------------------------------------
//
CAknExPopFieldBaseDialog::CAknExPopFieldBaseDialog()
    {
    // No implementation required
    }


// ----------------------------------------------------------------------------
// CAknExPopFieldBaseDialog::~CAknExPopFieldBaseDialog()
// Destructor.
// ----------------------------------------------------------------------------
//
CAknExPopFieldBaseDialog::~CAknExPopFieldBaseDialog()
    {
    // No implementation required
    }


// ----------------------------------------------------------------------------
// void CAknExPopFieldBaseDialog::DynInitMenuPaneL()
// Dims "Label", "Add" and "Delete" menu items.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldBaseDialog::DynInitMenuPaneL( TInt aResourceId,
                                                 CEikMenuPane* aMenuPane )
    {
    CAknForm::DynInitMenuPaneL( aResourceId, aMenuPane );
    if ( aResourceId == R_AVKON_FORM_MENUPANE )
        {
        aMenuPane->SetItemDimmed( EAknFormCmdLabel, ETrue );
        aMenuPane->SetItemDimmed( EAknFormCmdAdd, ETrue );
        aMenuPane->SetItemDimmed( EAknFormCmdDelete, ETrue );
        }
    }

// ----------------------------------------------------------------------------
// void CAknExPopFieldBaseDialog::ProcessCommandL()
// Overridden to supply a location for calls to PopupField APIs.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldBaseDialog::ProcessCommandL( TInt aCommandId )
    {
    CAknForm::ProcessCommandL( aCommandId );
    }

// ----------------------------------------------------------------------------
// TBool CAknExPopFieldBaseDialog::OkToExitL()
// Overridden to supply a location for the PopupFieldText test.
// ----------------------------------------------------------------------------
//
TBool CAknExPopFieldBaseDialog::OkToExitL( TInt aButtonId )
    {
    return CAknForm::OkToExitL( aButtonId );
    }

// ----------------------------------------------------------------------------
// void CAknExPopFieldPopupFieldDialog::PreLayoutDynInitL()
// Initialises the dialog's controls before the dialog is sized and layed out.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldBaseDialog::PreLayoutDynInitL()
    {
    ConfigurePopfieldValueL();
    }

// ----------------------------------------------------------------------------
// void CAknExPopFieldPopupFieldDialog::PostLayoutDynInitL()
// Initialises the dialog's controls after the dialog has been sized
// but before it has been activated.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldBaseDialog::PostLayoutDynInitL()
    {
    SetEditableL( EFalse );
    SaveFormDataL();
    }

// End of File

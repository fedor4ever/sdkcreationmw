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
#include <eikenv.h>
#include <aknutils.h> // for Font Change
#include <avkon.hrh>
#include <avkon.rsg>
#include <aknpopupfield.h>
#include <aknqueryvaluetext.h>
#include <AknExPopField.rsg>

#include "AknExPopField.hrh"
#include "AknExPopFieldAttribSetDialog.h"


// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CAknExPopFieldAttribSetDialog::CAknExPopFieldAttribSetDialog()
// C++ default constructor can NOT contain any code, that might leave.
// ----------------------------------------------------------------------------
//
CAknExPopFieldAttribSetDialog::CAknExPopFieldAttribSetDialog()
    {
    for ( TInt index( KAknExPopFieldAttribSetIndexInitialize );
          index < KAknExPopFieldAttribSetControlCount; ++index )
        {
        iTextValues[ index ] = NULL;
        iTextEmptyArray[ index ] = NULL;
        iValueTextArrayWithNoElements[ index ] =  NULL;
        }
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldAttribSetDialog* CAknExPopFieldAttribSetDialog::NewLC()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CAknExPopFieldAttribSetDialog* CAknExPopFieldAttribSetDialog::NewLC()
    {
    CAknExPopFieldAttribSetDialog* self =
        new ( ELeave ) CAknExPopFieldAttribSetDialog;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldAttribSetDialog::~CAknExPopFieldAttribSetDialog()
// Destructor.
// ----------------------------------------------------------------------------
//
CAknExPopFieldAttribSetDialog::~CAknExPopFieldAttribSetDialog()
    {
    DiscardTextArray();
    }


// ----------------------------------------------------------------------------
// CAknExPopFieldAttribSetDialog::DiscardTextArray()
// Discard text array in popup field object.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldAttribSetDialog::DiscardTextArray()
    {
    for ( TInt index( KAknExPopFieldAttribSetIndexInitialize );
          index < KAknExPopFieldAttribSetControlCount; ++index )
        {
        if ( iTextValues[index] )
            {
            delete iTextValues[ index ];
            iTextValues[ index ] = NULL;
            }
        if ( iValueTextArrayWithNoElements[ index ] )
            {
            delete iValueTextArrayWithNoElements[ index ];
            iValueTextArrayWithNoElements[ index ] = NULL;
            }
        if ( iTextEmptyArray[ index ] )
            {
            delete iTextEmptyArray[ index ];
            iTextEmptyArray[ index ] = NULL;
            }
        } // for
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldAttribSetDialog::SetIsSaveMenu()
// Set flag to indicate if we are in the save menu.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldAttribSetDialog::SetIsSaveMenu( TBool aVal )
    {
    iIsSaveMenuFlag = aVal;
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldAttribSetDialog::IsSaveMenu()
// Check if we are in the save menu.
// ----------------------------------------------------------------------------
//
TBool CAknExPopFieldAttribSetDialog::IsSaveMenu()
    {
    return iIsSaveMenuFlag;
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldAttribSetDialog::DynInitMenuPaneL()
// Add menu item to menupane.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldAttribSetDialog::DynInitMenuPaneL( TInt aResourceId, 
                                                      CEikMenuPane* aMenuPane )
    {

    CAknExPopFieldBaseDialog::DynInitMenuPaneL( aResourceId, aMenuPane );
    if ( aResourceId == R_AVKON_FORM_MENUPANE && IsSaveMenu() )
        {
        aMenuPane->AddMenuItemsL( R_AKNEXPOPFIELD_EXTRA_MENU, 
                                  EAknFormCmdSave, ETrue );
        }
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldAttribSetDialog::ProcessCommandL()
// Overridden to supply a location for calls to PopupField APIs.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldAttribSetDialog::ProcessCommandL( TInt aCommandId )
    {
    switch ( aCommandId )
        {
        case EAknExPopFieldCmdEditEntry:
            {
            TInt ctrlId = IdOfFocusControl();
            // Don't call ActiveSelectionList on an empty list, 
            // or panic incurred!
            if( ctrlId == EAknExPopFieldDlgCtrlIdAttribSet02 ) 
                {
                CAknPopupField* popup; 
                popup = static_cast<CAknPopupField*> (ControlOrNull( ctrlId ) );
                if ( popup )
                    {
                    popup->ActivateSelectionListL();
                    }
                }
            break;
            }
        case EAknFormCmdEdit:
            {
            SetIsSaveMenu( ETrue );
            break;
            }
        case EAknFormCmdSave:
            {
            SetIsSaveMenu( EFalse );
            break;
            }
        }
    CAknExPopFieldBaseDialog::ProcessCommandL( aCommandId );
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldAttribSetDialog::OkToExitL()
// From CAknDialog, OkToExitL.
// ----------------------------------------------------------------------------
//
TBool CAknExPopFieldAttribSetDialog::OkToExitL( TInt aButtonId )
    {
    if ( aButtonId == EAknSoftkeyBack && IsSaveMenu() )
        {
        SetIsSaveMenu( EFalse );
        } 
    return CAknExPopFieldBaseDialog::OkToExitL( aButtonId );
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldAttribSetDialog::ConfigurePopfieldValueL()
// Configuration values (text) into Popup Fields and call CAknPopupField APIs
// ----------------------------------------------------------------------------
//
void CAknExPopFieldAttribSetDialog::ConfigurePopfieldValueL()
    {
    // Discard textarray.
    DiscardTextArray();
    
    // Control container.
    CAknPopupField* popup[ KAknExPopFieldAttribSetControlCount ]; 
    
    for ( TInt index( KAknExPopFieldAttribSetIndexInitialize ) ; 
            index < KAknExPopFieldAttribSetControlCount; ++index )
        {
        // Get PopFieldAttribSet controls.
        
		popup[ index ] = static_cast<CAknPopupField*> 
			(ControlOrNull( EAknExPopFieldDlgCtrlIdAttribSet01 + index ) );

        __ASSERT_ALWAYS( popup[ index ]!=NULL, User::Leave( KErrNotFound ) );
        
        // Fill the arrays with empty array instances.
        iTextEmptyArray[index] = new ( ELeave ) CDesCArrayFlat(
        KAknExPopFieldGranularityForEmptyArray ); // Granularity of 16
        
        // Set iValueTextValueArray with the empty arrays.
        iValueTextArrayWithNoElements[ index ] = 
                            CAknQueryValueTextArray::NewL();
        iValueTextArrayWithNoElements[ index ]->
                            SetArray( *iTextEmptyArray[index] );

        // Set the same textarray into all the values, and initial value index, and
        // make calls onto the CAknPopfield APIs
        iTextValues[ index ] = CAknQueryValueText::NewL();
        iTextValues[ index ]->
                    SetArrayL( iValueTextArrayWithNoElements[ index ] );
        iTextValues[ index ]->SetQueryCaption( R_AKNEXPOPFIELD_QUERY_PROMPT );
        
        // Set values into popup fields.
        popup[ index ]->SetQueryValueL( iTextValues[ index ] );
        popup[ index ]->SetFont( LatinPlain12() );
        popup[ index ]->SetMaxNumberOfLinesPermitted( 
                         KAknExPopFieldMaxNumberLines );

        if ( !index ) // Call APIs specific to the control.
            {
            popup[ index ]->
                SetEmptyListNoteL( R_AKNEXPOPFIELD_EMPTY_LIST_NOTE );
            TBuf16<KAknExPopFieldReplacementTextBufLength> replacementString01;
            iEikonEnv->ReadResource( replacementString01, 
                        R_AKNEXPOPFIELD_EMPTY_TEXT_RAPLACEMENT );
            popup[ index ]->SetEmptyTextL( replacementString01 );
            }
        else
            {
            popup[ index ]->SetAllowsUserDefinedEntry( ETrue );
            
            // Set the Other text replacement in attribset02
            TBuf16<KAknExPopFieldReplacementTextBufLength> replacementString02;
            iEikonEnv->ReadResource( replacementString02, 
                        R_AKNEXPOPFIELD_OTHER_TEXT_RAPLACEMENT );
            popup[ index ]->SetOtherTextL( replacementString02 );
            
            // Set the Empty text replacement in attribset02
            TBuf16<KAknExPopFieldReplacementTextBufLength> replacementString03;
            iEikonEnv->ReadResource( replacementString03, 
                        R_AKNEXPOPFIELD_EMPTY_TEXT_RAPLACEMENT );
            popup[ index ]->SetEmptyTextL( replacementString03 );
            }
        } // for
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldAttribSetDialog::SaveFormDataL()
// Save form data.
// ----------------------------------------------------------------------------
//
TBool CAknExPopFieldAttribSetDialog::SaveFormDataL()
    {
    for ( TInt i = KAknExPopFieldAttribSetIndexInitialize;
              i < KAknExPopFieldAttribSetControlCount; i++ )
        {
            if ( ControlOrNull( EAknExPopFieldDlgCtrlIdPop01+i ) )
                { 
                     iIndexValue[ i ] = iTextValues[ i ]->CurrentValueIndex();
                }
        }

    return ETrue;
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldAttribSetDialog::DoNotSaveFormDataL()
// Does not save form data.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldAttribSetDialog::DoNotSaveFormDataL()
    {
    for ( TInt i = KAknExPopFieldAttribSetIndexInitialize;
              i < KAknExPopFieldAttribSetControlCount; i++ )
        {
        if ( ControlOrNull( EAknExPopFieldDlgCtrlIdPop01+i ) )
            {
            iTextValues[ i ]->SetCurrentValueIndex( iIndexValue[ i ] );
            }
        }
    }

// End of File

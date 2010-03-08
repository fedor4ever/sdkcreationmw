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
#include <aknpopupfield.h>
#include <aknqueryvaluetext.h>
#include <AknExPopField.rsg>

#include "AknExPopField.hrh"
#include "AknExPopFieldOptionalListDialog.h"


// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CAknExPopFieldOptionalListDialog::CAknExPopFieldOptionalListDialog()
// C++ default constructor can NOT contain any code, that might leave.
// ----------------------------------------------------------------------------
//
CAknExPopFieldOptionalListDialog::CAknExPopFieldOptionalListDialog()
    {
    TInt index( KAknExPopFieldOLIndexInitialize );
    for ( index = KAknExPopFieldOLIndexInitialize;
          index < KAknExPopFieldMaxOLTextValueField; index++ )
        {
        iTextArray[ index ] = NULL;
        iValueTextArray[ index ] = NULL;
        iTextValues[ index ] = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldOptionalListDialog* CAknExPopFieldOptionalListDialog::NewLC()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CAknExPopFieldOptionalListDialog* CAknExPopFieldOptionalListDialog::NewLC()
    {
    CAknExPopFieldOptionalListDialog* self =
        new ( ELeave ) CAknExPopFieldOptionalListDialog;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldOptionalListDialog::~CAknExPopFieldOptionalListDialog()
// Destructor.
// ----------------------------------------------------------------------------
//
CAknExPopFieldOptionalListDialog::~CAknExPopFieldOptionalListDialog()
    {
    DiscardTextArray();
    }


// ----------------------------------------------------------------------------
// CAknExPopFieldOptionalListDialog::DiscardTextArray()
// Discard text array in popup field object.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldOptionalListDialog::DiscardTextArray()
    {
    TInt index( KAknExPopFieldOLIndexInitialize );
    for ( index = KAknExPopFieldOLIndexInitialize;
          index < KAknExPopFieldMaxOLTextValueField; index++ )
        {
        if ( iTextValues[ index ] )
            {
            delete iTextValues[ index ];
            iTextValues[ index ] = NULL;
            }
        if ( iValueTextArray[ index ] )
            {
            delete iValueTextArray[ index ];
            iValueTextArray[ index ] = NULL;
            }
        if ( iTextArray[ index ] )
            {
            delete iTextArray[ index ];
            iTextArray[ index ] = NULL;
            }
        }
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldOptionalListDialog::ConfigurePopfieldValueL()
// Configuration values (text) into Popup Fields.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldOptionalListDialog::ConfigurePopfieldValueL()
    {
    // Discard textarray.
    DiscardTextArray();

    // Calculate number of pop-up field.
    TInt index( KAknExPopFieldOLIndexInitialize );
    TInt count( KAknExPopFieldOLCountInitialize );
    TInt controlId( KAknExPopFieldOLControlIdInitialize );
    CAknPopupField* popup[ KAknExPopFieldMaxOLTextValueField ];
    for ( index = KAknExPopFieldOLIndexInitialize;
          index < KAknExPopFieldMaxOLTextValueField; index++ )
        {
        popup[ index ] = NULL;
        controlId = EAknExPopFieldDlgCtrlIdPop00 + index;
        CCoeControl* control = ControlOrNull( controlId );
        if ( control )
            {
            popup[ count ] = static_cast<CAknPopupField*> ( control );
            count++;
            }
        }

    // Create textarray.
    iTextArray[ EAknExPopFieldTextArrayIndex0 ] =
        new ( ELeave ) CDesCArrayFlat( KAknExPopFieldInitialArrayElementCount );
    iTextArray[ EAknExPopFieldTextArrayIndex1 ] =
        iCoeEnv->ReadDesCArrayResourceL( R_AKNEXPOPFIELD_ONE_TEXT_ARRAY );
    iTextArray[ EAknExPopFieldTextArrayIndex2 ] =
        iCoeEnv->ReadDesCArrayResourceL( R_AKNEXPOPFIELD_TWO_TEXT_ARRAY );
    iTextArray[ EAknExPopFieldTextArrayIndex3 ] =
        iCoeEnv->ReadDesCArrayResourceL( R_AKNEXPOPFIELD_THREE_TEXT_ARRAY );
    iTextArray[ EAknExPopFieldTextArrayIndex4 ] =
        iCoeEnv->ReadDesCArrayResourceL( R_AKNEXPOPFIELD_FOUR_TEXT_ARRAY );
    iTextArray[ EAknExPopFieldTextArrayIndex5 ] =
        iCoeEnv->ReadDesCArrayResourceL( R_AKNEXPOPFIELD_FIVE_TEXT_ARRAY );
    iTextArray[ EAknExPopFieldTextArrayIndex6 ] =
        iCoeEnv->ReadDesCArrayResourceL( R_AKNEXPOPFIELD_SIX_TEXT_ARRAY );
    iTextArray[ EAknExPopFieldTextArrayIndex7 ] =
        iCoeEnv->ReadDesCArrayResourceL( R_AKNEXPOPFIELD_SEVEN_TEXT_ARRAY );
    iTextArray[ EAknExPopFieldTextArrayIndex8 ] =
        iCoeEnv->ReadDesCArrayResourceL( R_AKNEXPOPFIELD_EIGHT_TEXT_ARRAY );
    iTextArray[ EAknExPopFieldTextArrayIndex9 ] =
        iCoeEnv->ReadDesCArrayResourceL( R_AKNEXPOPFIELD_NINE_TEXT_ARRAY );
    iTextArray[ EAknExPopFieldTextArrayIndex10 ] =
        iCoeEnv->ReadDesCArrayResourceL( R_AKNEXPOPFIELD_TEN_TEXT_ARRAY );
    iTextArray[ EAknExPopFieldTextArrayIndex11 ] =
        iCoeEnv->ReadDesCArrayResourceL( R_AKNEXPOPFIELD_ELEVEN_TEXT_ARRAY );
    iTextArray[ EAknExPopFieldTextArrayIndex12 ] =
        iCoeEnv->ReadDesCArrayResourceL( R_AKNEXPOPFIELD_TWELVE_TEXT_ARRAY );

    for ( index = KAknExPopFieldOLIndexInitialize; index < count; index++ )
        {
        iValueTextArray[ index ] = CAknQueryValueTextArray::NewL();
        iValueTextArray[ index ]->SetArray( *iTextArray[ index ] );
        }

    // Set the same textarray into all the values, and initial value index.
    for ( index = KAknExPopFieldOLIndexInitialize; index < count; index++ )
        {
        iTextValues[ index ] = CAknQueryValueText::NewL();
        iTextValues[ index ]->SetArrayL( iValueTextArray[ index ] );
        if ( index != KAknExPopFieldEmptyFieldIndex ) // array is no empty
            {
            iTextValues[ index ]->SetCurrentValueIndex( NULL );
            iTextValues[ index ]->
                        SetQueryCaption( R_AKNEXPOPFIELD_QUERY_PROMPT );
            }
        }

    // Set values into popup fields.
    for ( index = KAknExPopFieldOLIndexInitialize; index < count; index++ )
        {
        popup[ index ]->SetQueryValueL( iTextValues[ index ] );
        }
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldOptionalListDialog::SaveFormDataL()
// Save form data.
// ----------------------------------------------------------------------------
//
TBool CAknExPopFieldOptionalListDialog::SaveFormDataL()
    {
    for ( TInt i = KAknExPopFieldOLIndexInitialize;
             i < KAknExPopFieldMaxOLTextValueField; i++ )
        {
        if( ControlOrNull( EAknExPopFieldDlgCtrlIdPop00+i ) )
            {
            if ( i != KAknExPopFieldEmptyFieldIndex ) // array is no empty
                {
                iIndexValue[ i ] = iTextValues[ i ]->CurrentValueIndex();
                }
            }
        }

    return ETrue;
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldOptionalListDialog::DoNotSaveFormDataL()
// Does not save form data.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldOptionalListDialog::DoNotSaveFormDataL()
    {
    for( TInt i = KAknExPopFieldOLIndexInitialize;
             i < KAknExPopFieldMaxOLTextValueField; i++ )
        {
        if( ControlOrNull( EAknExPopFieldDlgCtrlIdPop00+i ) )
            {
            if ( i != KAknExPopFieldEmptyFieldIndex ) // array is no empty
                {
                iTextValues[ i ]->SetCurrentValueIndex( iIndexValue[ i ] );
                }
            }
        }
    }

// End of File

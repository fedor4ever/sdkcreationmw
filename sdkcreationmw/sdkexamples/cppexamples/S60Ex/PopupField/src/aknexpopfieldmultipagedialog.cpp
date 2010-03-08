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
#include "AknExPopFieldMultiPageDialog.h"


// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CAknExPopFieldMultiPageDialog* CAknExPopFieldMultiPageDialog::NewLC()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CAknExPopFieldMultiPageDialog* CAknExPopFieldMultiPageDialog::NewLC()
    {
    CAknExPopFieldMultiPageDialog* self =
        new ( ELeave ) CAknExPopFieldMultiPageDialog;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldMultiPageDialog::~CAknExPopFieldMultiPageDialog()
// Destructor.
// ----------------------------------------------------------------------------
//
CAknExPopFieldMultiPageDialog::~CAknExPopFieldMultiPageDialog()
    {
    DiscardArray();
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldMultiPageDialog::CAknExPopFieldMultiPageDialog()
// C++ default constructor can NOT contain any code, that might leave.
// ----------------------------------------------------------------------------
//
CAknExPopFieldMultiPageDialog::CAknExPopFieldMultiPageDialog() : 
                               iTextArray( NULL ),
                               iValueTextArray( NULL ),
                               iNumberArray( NULL ),
                               iValueNumberArray( NULL ),
                               iPhoneArray( NULL ),
                               iValuePhoneArray( NULL ),
                               iTimeArray( NULL ),
                               iValueTimeArray( NULL ),
                               iDateArray( NULL ),
                               iValueDateArray( NULL ),
                               iDurationArray( NULL ),
                               iValueDurationArray( NULL )
    {

    TInt index( KAknExPopFieldMultiPageIndexInitialize );
    for ( index = KAknExPopFieldMultiPageIndexInitialize;
          index < KAknExPopFieldMaxMultiPageValueField; index++ )
        {
        iTextValues[ index ] = NULL;
        iNumberValues[ index ] = NULL;
        iPhoneValues[ index ] = NULL;
        iTimeValues[ index ] = NULL;
        iDateValues[ index ] = NULL;
        iDurationValues[ index ] = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldMultiPageDialog::DiscardArray()
// Discard array in popup field object.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldMultiPageDialog::DiscardArray()
    {
    if ( iTextArray )
        {
        delete iTextArray;
        iTextArray = NULL;
        }
    if ( iValueTextArray )
        {
        delete iValueTextArray;
        iValueTextArray = NULL;
        }
    if ( iNumberArray )
        {
        delete iNumberArray;
        iNumberArray = NULL;
        }
    if ( iValueNumberArray )
        {
        delete iValueNumberArray;
        iValueNumberArray = NULL;
        }
    if ( iPhoneArray )
        {
        delete iPhoneArray;
        iPhoneArray = NULL;
        }
    if ( iValuePhoneArray )
        {
        delete iValuePhoneArray;
        iValuePhoneArray = NULL;
        }
    if ( iTimeArray )
        {
        delete iTimeArray;
        iTimeArray = NULL;
        }
    if ( iValueTimeArray )
        {
        delete iValueTimeArray;
        iValueTimeArray = NULL;
        }
    if ( iDateArray )
        {
        delete iDateArray;
        iDateArray = NULL;
        }
    if ( iValueDateArray )
        {
        delete iValueDateArray;
        iValueDateArray = NULL;
        }
    if ( iDurationArray )
        {
        delete iDurationArray;
        iDurationArray = NULL;
        }
    if ( iValueDurationArray )
        {
        delete iValueDurationArray;
        iValueDurationArray = NULL;
        }

    TInt index( KAknExPopFieldMultiPageIndexInitialize );
    for ( index = KAknExPopFieldMultiPageIndexInitialize;
          index < KAknExPopFieldMaxMultiPageValueField; index++ )
        {
        if ( iTextValues[index] )
            {
            delete iTextValues[ index ];
            iTextValues[ index ] = NULL;
            }
        if ( iNumberValues[ index ] )
            {
            delete iNumberValues[ index ];
            iNumberValues[ index ] = NULL;
            }
        if ( iPhoneValues[ index ] )
            {
            delete iPhoneValues[ index ];
            iPhoneValues[ index ] = NULL;
            }
        if ( iTimeValues[ index ] )
            {
            delete iTimeValues[ index ];
            iTimeValues[ index ] = NULL;
            }
        if ( iDateValues[ index ] )
            {
            delete iDateValues[ index ];
            iDateValues[ index ] = NULL;
            }
        if ( iDurationValues[ index ] )
            {
            delete iDurationValues[ index ];
            iDurationValues[ index ] = NULL;
            }
        }
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldMultiPageDialog::ConfigurePopfieldValueL()
// Configuration values into Popup Fields.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldMultiPageDialog::ConfigurePopfieldValueL()
    {
    // Discard array.
    DiscardArray();

    ConfigurePageTextL();
    ConfigurePageNumberL();
    ConfigurePagePhoneL();
    ConfigurePageTimeL();
    ConfigurePageDateL();
    ConfigurePageDurationL();
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldMultiPageDialog::ConfigurePageTextL()
// Configuration values (text) into Popup Fields.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldMultiPageDialog::ConfigurePageTextL()
    {
    // Calculate number of pop-up field.
    TInt index( KAknExPopFieldMultiPageIndexInitialize );
    TInt count( KAknExPopFieldMultiPageCountInitialize );
    TInt controlId( KAknExPopFieldMultiPageControlIdInitialize );
    CAknPopupField* popup[ KAknExPopFieldMaxMultiPageValueField ];
    for ( index = KAknExPopFieldMultiPageIndexInitialize;
          index < KAknExPopFieldMaxMultiPageValueField; index++ )
        {
        popup[ index ] = NULL;
        controlId = EAknExPopFieldDlgCtrlIdText01 + index;
        CCoeControl* control = ControlOrNull( controlId );
        if ( control )
            {
            popup[ count ] = static_cast<CAknPopupField*> ( control );
            count++;
            }
        }

    // Create text array.
    iTextArray = iCoeEnv->ReadDesCArrayResourceL(
        R_AKNEXPOPFIELD_SIX_TEXT_ARRAY );
    iValueTextArray = CAknQueryValueTextArray::NewL();
    iValueTextArray->SetArray( *iTextArray );

    // Set the same array into all the values, and initial value index.
    for ( index = KAknExPopFieldMultiPageIndexInitialize; 
                    index < count; index++ )
        {
        iTextValues[ index ] = CAknQueryValueText::NewL();
        iTextValues[ index ]->SetArrayL( iValueTextArray );
        iTextValues[ index ]->SetCurrentValueIndex( NULL );
        iTextValues[ index ]->SetQueryCaption( R_AKNEXPOPFIELD_QUERY_PROMPT );
        }

    // Set values into popup fields.
    for ( index = KAknExPopFieldMultiPageIndexInitialize; 
                    index < count; index++ )
        {
        popup[ index ]->SetQueryValueL( iTextValues[ index ] );
        }
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldMultiPageDialog::ConfigurePageNumberL()
// Configuration values (number) into Popup Fields.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldMultiPageDialog::ConfigurePageNumberL()
    {
    // Calculate number of pop-up field.
    TInt index( KAknExPopFieldMultiPageIndexInitialize );
    TInt count( KAknExPopFieldMultiPageCountInitialize );
    TInt controlId( KAknExPopFieldMultiPageControlIdInitialize );
    CAknPopupField* popup[KAknExPopFieldMaxMultiPageValueField];
    for ( index = KAknExPopFieldMultiPageIndexInitialize;
          index < KAknExPopFieldMaxMultiPageValueField; index++ )
        {
        popup[index] = NULL;
        controlId = EAknExPopFieldDlgCtrlIdNum01 + index;
        CCoeControl* control = ControlOrNull( controlId );
        if ( control )
            {
            popup[ count ] = static_cast<CAknPopupField*> ( control );
            count++;
            }
        }

    // create an array of numbers
    iNumberArray = new ( ELeave ) CArrayFixFlat<TInt>(
        KAknExPopFieldMultiPageInitialArrayElementCount );
    TInt value( KAknExPopFieldValueInitialize );
    for ( value = KAknExPopFieldNumberArrayMinimumValue;
          value < KAknExPopFieldNumberArrayMaximumValue ;
          value += KAknExPopFieldNumberArrayStepValue )
        {
        iNumberArray->AppendL( value );
        }

    // wrap the number array
    iValueNumberArray = CAknQueryValueNumberArray::NewL(
        R_AKNEXPOPFIELD_NUM_FORMAT_STRING_TEXT );
    iValueNumberArray->SetArray( *iNumberArray );

    // set the same array into all the values, and set initial index values
    for ( index = KAknExPopFieldMultiPageIndexInitialize; 
                    index < count; index++ )
        {
        iNumberValues[ index ] = CAknQueryValueNumber::NewL();
        iNumberValues[ index ]->SetArrayL( iValueNumberArray );
        iNumberValues[ index ]->SetCurrentValueIndex( NULL );
        iNumberValues[ index ]->SetQueryCaption( 
                                    R_AKNEXPOPFIELD_QUERY_PROMPT );
        }

    // set values into popup fields
    for ( index = KAknExPopFieldMultiPageIndexInitialize; 
                    index < count; index++ )
        {
        popup[ index ]->SetQueryValueL( iNumberValues[ index ] );
        }
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldMultiPageDialog::ConfigurePagePhoneL()
// Configuration values (phone) into Popup Fields.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldMultiPageDialog::ConfigurePagePhoneL()
    {
    // Calculate number of pop-up field.
    TInt index( KAknExPopFieldMultiPageIndexInitialize );
    TInt count( KAknExPopFieldMultiPageCountInitialize );
    TInt controlId( KAknExPopFieldMultiPageControlIdInitialize );
    CAknPopupField* popup[ KAknExPopFieldMaxMultiPageValueField ];
    for ( index = KAknExPopFieldMultiPageIndexInitialize;
          index < KAknExPopFieldMaxMultiPageValueField; index++ )
        {
        popup[ index ] = NULL;
        controlId = EAknExPopFieldDlgCtrlIdPhon01 + index;
        CCoeControl* control = ControlOrNull( controlId );
        if ( control )
            {
            popup[ count ] = static_cast<CAknPopupField*> ( control );
            count++;
            }
        }

    // create an array of phone
    iPhoneArray = iCoeEnv->ReadDesCArrayResourceL(
        R_AKNEXPOPFIELD_SIX_PHONE_ARRAY );
    iValuePhoneArray = CAknQueryValuePhoneArray::NewL();
    iValuePhoneArray->SetArray( *iPhoneArray );

    // set the same array into all the values, and set initial index values
    for ( index = KAknExPopFieldMultiPageIndexInitialize; 
                    index < count; index++ )
        {
        iPhoneValues[ index ] = CAknQueryValuePhone::NewL();
        iPhoneValues[ index ]->SetArrayL( iValuePhoneArray );
        iPhoneValues[ index ]->SetCurrentValueIndex( NULL );
        iPhoneValues[ index ]->SetQueryCaption( R_AKNEXPOPFIELD_QUERY_PROMPT );
        }
    // set values into popup fields
    for ( index = KAknExPopFieldMultiPageIndexInitialize; 
                    index < count; index++ )
        {
        popup[ index ]->SetQueryValueL( iPhoneValues[ index ] );
        }
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldMultiPageDialog::ConfigurePageTimeL()
// Configuration values (time) into Popup Fields.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldMultiPageDialog::ConfigurePageTimeL()
    {
    // Calculate number of pop-up field.
    TInt index( KAknExPopFieldMultiPageIndexInitialize );
    TInt count( KAknExPopFieldMultiPageCountInitialize );
    TInt controlId( KAknExPopFieldMultiPageControlIdInitialize );
    CAknPopupField* popup[ KAknExPopFieldMaxMultiPageValueField ];
    for ( index = KAknExPopFieldMultiPageIndexInitialize;
          index < KAknExPopFieldMaxMultiPageValueField; index++ )
        {
        popup[ index ] = NULL;
        controlId = EAknExPopFieldDlgCtrlIdTime01 + index;
        CCoeControl* control = ControlOrNull( controlId );
        if ( control )
            {
            popup[ count ] = static_cast<CAknPopupField*> ( control );
            count++;
            }
        }

    // create an array of time
    TBuf<KAknExPopFieldArrayTextBufLength> text( NULL );
    iCoeEnv->ReadResource( text, R_AKNEXPOPFIELD_TIME_INITIAL_TEXT );
    iTimeArray = new ( ELeave ) CArrayFixFlat<TTime>(
        KAknExPopFieldMultiPageInitialArrayElementCount );
    TTime value( text );
    TInt step( KAknExPopFieldStepInitialize );
    for ( step = KAknExPopFieldStepInitialize;
          step < KAknExPopFieldTimeArrayStepNumber; step++ )
        {
        iTimeArray->AppendL( value );
        value += KAknExPopFieldTimeArrayStepHour;
        }

    // wrap the time array
    iValueTimeArray = CAknQueryValueTimeArray::NewL(
        R_AKNEXPOPFIELD_TIME_FORMAT_STRING_TEXT );
    iValueTimeArray->SetArray( *iTimeArray );

    // set the same array into all the values, and set initial index values
    for ( index = KAknExPopFieldMultiPageIndexInitialize; 
                    index < count; index++ )
        {
        iTimeValues[ index ] = CAknQueryValueTime::NewL();
        iTimeValues[ index ]->SetArrayL( iValueTimeArray );
        iTimeValues[ index ]->SetCurrentValueIndex( NULL );
        iTimeValues[ index ]->SetQueryCaption( R_AKNEXPOPFIELD_QUERY_PROMPT );
        }

    // set values into popup fields
    for ( index = KAknExPopFieldMultiPageIndexInitialize; 
                    index < count; index++ )
        {
        popup[ index ]->SetQueryValueL( iTimeValues[ index ] );
        }
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldMultiPageDialog::ConfigurePageDateL()
// Configuration values (date) into Popup Fields.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldMultiPageDialog::ConfigurePageDateL()
    {
    // Calculate number of pop-up field.
    TInt index( KAknExPopFieldMultiPageIndexInitialize );
    TInt count( KAknExPopFieldMultiPageCountInitialize );
    TInt controlId( KAknExPopFieldMultiPageControlIdInitialize );
    CAknPopupField* popup[ KAknExPopFieldMaxMultiPageValueField ];
    for ( index = KAknExPopFieldMultiPageIndexInitialize;
          index < KAknExPopFieldMaxMultiPageValueField; index++ )
        {
        popup[ index ] = NULL;
        controlId = EAknExPopFieldDlgCtrlIdDate01 + index;
        CCoeControl* control = ControlOrNull( controlId );
        if ( control )
            {
            popup[ count ] = static_cast<CAknPopupField*> ( control );
            count++;
            }
        }

    // create an array of date
    TBuf<KAknExPopFieldArrayTextBufLength> text( NULL );
    iCoeEnv->ReadResource( text, R_AKNEXPOPFIELD_DATE_INITIAL_TEXT );
    iDateArray = new ( ELeave ) CArrayFixFlat<TTime>(
        KAknExPopFieldMultiPageInitialArrayElementCount );
    TTime value( text );
    TInt step( KAknExPopFieldStepInitialize );
    for ( step = KAknExPopFieldStepInitialize;
          step < KAknExPopFieldDateArrayStepNumber; step++ )
        {
        iDateArray->AppendL( value );
        value += KAknExPopFieldDateArrayStepDay;
        }

    // wrap the date array
    iValueDateArray = CAknQueryValueDateArray::NewL(
        R_AKNEXPOPFIELD_DATE_FORMAT_STRING_TEXT );
    iValueDateArray->SetArray( *iDateArray );

    // set the same array into all the values, and set initial index values
    for ( index = KAknExPopFieldMultiPageIndexInitialize; 
                    index < count; index++ )
        {
        iDateValues[ index ] = CAknQueryValueDate::NewL();
        iDateValues[ index ]->SetArrayL( iValueDateArray );
        iDateValues[ index ]->SetCurrentValueIndex( NULL );
        iDateValues[ index ]->SetQueryCaption( R_AKNEXPOPFIELD_QUERY_PROMPT );
        }
    // set values into popup fields
    for ( index = KAknExPopFieldMultiPageIndexInitialize; 
                    index < count; index++ )
        {
        popup[ index ]->SetQueryValueL( iDateValues[ index ] );
        }
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldMultiPageDialog::ConfigurePageDurationL()
// Configuration values (duration) into Popup Fields.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldMultiPageDialog::ConfigurePageDurationL()
    {
    // Calculate number of pop-up field.
    TInt index( KAknExPopFieldMultiPageIndexInitialize );
    TInt count( KAknExPopFieldMultiPageCountInitialize );
    TInt controlId( KAknExPopFieldMultiPageControlIdInitialize );
    CAknPopupField* popup[ KAknExPopFieldMaxMultiPageValueField ];
    for ( index = KAknExPopFieldMultiPageIndexInitialize;
          index < KAknExPopFieldMaxMultiPageValueField; index++ )
        {
        popup[index] = NULL;
        controlId = EAknExPopFieldDlgCtrlIdDura01 + index;
        CCoeControl* control = ControlOrNull( controlId );
        if ( control )
            {
            popup[ count ] = static_cast<CAknPopupField*> ( control );
            count++;
            }
        }

    // create an array of duration
    iDurationArray = new ( ELeave ) CArrayFixFlat<TTimeIntervalSeconds>(
        KAknExPopFieldMultiPageInitialArrayElementCount );
    TInt value( KAknExPopFieldValueInitialize ); 
    for ( value = KAknExPopFieldDuraArrayMinimumValue;
          value < KAknExPopFieldDuraArrayMaximumValue;
          value += KAknExPopFieldDuraArrayStepValue )
        {
        TTimeIntervalSeconds interval( value );
        iDurationArray->AppendL( interval );
        }

    // wrap the duration array
    iValueDurationArray = CAknQueryValueDurationArray::NewL(
        R_AKNEXPOPFIELD_DURA_FORMAT_STRING_TEXT );
    iValueDurationArray->SetArray( *iDurationArray );

    // set the same array into all the values, and set initial index values
    for ( index = KAknExPopFieldMultiPageIndexInitialize; 
                    index < count; index++ )
        {
        iDurationValues[ index ] = CAknQueryValueDuration::NewL();
        iDurationValues[ index ]->SetArrayL( iValueDurationArray );
        iDurationValues[ index ]->SetCurrentValueIndex( NULL );
        iDurationValues[ index ]->
                    SetQueryCaption( R_AKNEXPOPFIELD_QUERY_PROMPT );
        }

    // set values into popup fields
    for ( index = KAknExPopFieldMultiPageIndexInitialize; 
                    index < count; index++ )
        {
        popup[ index ]->SetQueryValueL( iDurationValues[ index ] );
        }
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldMultiPageDialog::SaveFormDataL()
// Save form data.
// ----------------------------------------------------------------------------
//
TBool CAknExPopFieldMultiPageDialog::SaveFormDataL()
    {
    for ( TInt i = KAknExPopFieldMultiPageIndexInitialize;
              i < KAknExPopFieldMaxMultiPageValueField; i++ ) 
        {
        if ( ControlOrNull( EAknExPopFieldDlgCtrlIdText01+i ) )
            {
            iIndexValueText[ i ] = iTextValues[ i ]->CurrentValueIndex();
            }
        if ( ControlOrNull( EAknExPopFieldDlgCtrlIdNum01+i ) )
            {
            iIndexValueNumber[ i ] = iNumberValues[ i ]->CurrentValueIndex();
            }
        if ( ControlOrNull( EAknExPopFieldDlgCtrlIdPhon01+ i ) )
            {
            iIndexValuePhone[ i ] = iPhoneValues[ i ]->CurrentValueIndex();
            }
        if ( ControlOrNull( EAknExPopFieldDlgCtrlIdTime01+i ) )
            {
            iIndexValueTime[ i ] = iTimeValues[ i ]->CurrentValueIndex();
            }
        if ( ControlOrNull( EAknExPopFieldDlgCtrlIdDate01+i ) )
            {
            iIndexValueDate[ i ] = iDateValues[ i ]->CurrentValueIndex();
            }
        if ( ControlOrNull( EAknExPopFieldDlgCtrlIdDura01+i ) )
            {
            iIndexValueDuration[ i ] = 
                    iDurationValues[ i ]->CurrentValueIndex();
            }
        }

    return ETrue;
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldMultiPageDialog::DoNotSaveFormDataL()
// Does not save form data.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldMultiPageDialog::DoNotSaveFormDataL()
    {
    for ( TInt i = KAknExPopFieldMultiPageIndexInitialize;
              i < KAknExPopFieldMaxMultiPageValueField; i++ )
        {
        if ( ControlOrNull( EAknExPopFieldDlgCtrlIdText01+i ) )
            {
            iTextValues[ i ]->SetCurrentValueIndex( iIndexValueText[ i ] );
            }
        if ( ControlOrNull( EAknExPopFieldDlgCtrlIdNum01+i ) )
            {
            iNumberValues[ i ]->SetCurrentValueIndex( iIndexValueNumber[ i ] );
            }
        if ( ControlOrNull( EAknExPopFieldDlgCtrlIdPhon01+i ) )
            {
            iPhoneValues[ i ]->SetCurrentValueIndex( iIndexValuePhone[ i ] );
            }
        if ( ControlOrNull( EAknExPopFieldDlgCtrlIdTime01+i ) )
            {
            iTimeValues[ i ]->SetCurrentValueIndex( iIndexValueTime[ i ] );
            }
        if ( ControlOrNull( EAknExPopFieldDlgCtrlIdDate01+i ) )
            {
            iDateValues[ i ]->SetCurrentValueIndex( iIndexValueDate[ i ] );
            }
        if ( ControlOrNull( EAknExPopFieldDlgCtrlIdDura01+i ) )
            {
            iDurationValues[ i ]->
                        SetCurrentValueIndex( iIndexValueDuration[ i ] );
            }
        }
    }

// End of File

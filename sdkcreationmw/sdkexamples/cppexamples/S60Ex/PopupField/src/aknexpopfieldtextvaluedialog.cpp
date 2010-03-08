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
#include <avkon.hrh>
#include <aknpopupfield.h>
#include <aknpopupfieldtext.h>
#include <aknqueryvaluetext.h>
#include <AknExPopField.rsg>

#include "AknExPopField.hrh"
#include "AknExPopFieldTextValueDialog.h"


// ================= MEMBER FUNCTIONS =========================================


// ----------------------------------------------------------------------------
// CAknExPopFieldTextValueDialog::CAknExPopFieldTextValueDialog()
// C++ default constructor can NOT contain any code, that might leave.
// ----------------------------------------------------------------------------
//
CAknExPopFieldTextValueDialog::CAknExPopFieldTextValueDialog(): 
                               iTextArray( NULL ),
                               iValueTextArray( NULL )
    {

    TInt index( KAknExPopFieldIndexInitialize );
    for ( index = KAknExPopFieldIndexInitialize;
          index < KAknExPopFieldMaxTextValueField; index++ )
        {
        iTextValues[ index ] = NULL;
        }
    }


// ----------------------------------------------------------------------------
// CAknExPopFieldTextValueDialog* CAknExPopFieldTextValueDialog::NewLC()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CAknExPopFieldTextValueDialog* CAknExPopFieldTextValueDialog::NewLC()
    {
    CAknExPopFieldTextValueDialog* self =
        new ( ELeave ) CAknExPopFieldTextValueDialog;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldTextValueDialog::~CAknExPopFieldTextValueDialog()
// Destructor.
// ----------------------------------------------------------------------------
//
CAknExPopFieldTextValueDialog::~CAknExPopFieldTextValueDialog()
    {
    DiscardTextArray();
    }


// ----------------------------------------------------------------------------
// CAknExPopFieldTextValueDialog::DiscardTextArray()
// Discard text array in popup field object.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldTextValueDialog::DiscardTextArray()
    {
    TInt index( KAknExPopFieldIndexInitialize );
    for ( index = KAknExPopFieldIndexInitialize;
          index < KAknExPopFieldMaxTextValueField; index++ )
        {
        if ( iTextValues[ index ] )
            {
            delete iTextValues[ index ];
            iTextValues[ index ] = NULL;
            }
        }

    if ( iValueTextArray )
        {
        delete iValueTextArray;
        iValueTextArray = NULL;
        }

    if ( iTextArray )
        {
        delete iTextArray;
        iTextArray = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldTextValueDialog::OkToExitL()
// Implemented to test PopupFieldText API's.
// ----------------------------------------------------------------------------
//
TBool CAknExPopFieldTextValueDialog::OkToExitL( TInt aButtonId )
    {
    switch ( aButtonId )
        {
        case EAknSoftkeyBack:
            {
            CAknPopupFieldText* popupFieldText[KAknExPopupFieldTextMaxValues];
            TBuf<256> textbuf;
            for ( TInt i = 0; i < KAknExPopupFieldTextMaxValues; i++ )
                {
                // Get the proper control
                popupFieldText[i] = ( CAknPopupFieldText* ) 
                        ControlOrNull( EAknExPopFieldDlgCtrlIdText01+i );

                if ( popupFieldText[ i ] )
                    {
                    // Perform the calls on the API 
                    // provided by CAknPopupFieldText
                    HBufC* text = popupFieldText[ i ]->CurrentValueTextLC();
                    TInt value = popupFieldText[ i ]->CurrentValueIndex();
                    popupFieldText[ i ]->SetCurrentValueIndex( i );
                    popupFieldText[ i ]->SetAutoAppend( ETrue );
                    popupFieldText[ i ]->SetAllowsUserDefinedEntry( ETrue );
                    const MDesCArray* array = popupFieldText[ i ]->MdcArray();
                    TInt mdcaCount = array->MdcaCount();

                    // Print results to Debug 
                    _LIT( KPopupFieldText,"PopupFieldText Control Vals -> Number %d, Value %d, Text : " );
                    textbuf.Format( KPopupFieldText,i,value );
                    textbuf.Append( *text );
                    RDebug::Print( textbuf );

                    _LIT( KMdcaCountText,"... MdcaCount = %d." );
                    textbuf.Format( KMdcaCountText, mdcaCount );
                    RDebug::Print( textbuf );

                    // Clean up text
                    CleanupStack::PopAndDestroy(); // text
                    }
                }
            break;
            }
        default:
            break;
        }
    return CAknExPopFieldBaseDialog::OkToExitL( aButtonId );
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldTextValueDialog::ConfigurePopfieldValueL()
// Configuration values (text) into Popup Fields.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldTextValueDialog::ConfigurePopfieldValueL()
    {
    // Discard textarray.
    DiscardTextArray();

    // Calculate number of pop-up field.
    TInt index( KAknExPopFieldIndexInitialize );
    TInt count( KAknExPopFieldCountInitialize );
    TInt controlId( KAknExPopFieldControlIdInitialize );
    CAknPopupField* popup[ KAknExPopFieldMaxTextValueField ];
    for ( index = KAknExPopFieldIndexInitialize;
          index < KAknExPopFieldMaxTextValueField; index++ )
        {
        popup[ index ] = NULL;
        controlId = EAknExPopFieldDlgCtrlIdPop01 + index;
        CCoeControl* control = ControlOrNull( controlId );
        if ( control )
            {
            popup[ count ] = static_cast<CAknPopupField*> ( control );
            count++;
            }
        }

    // Create textarray.
    iTextArray = iCoeEnv->ReadDesCArrayResourceL(
        R_AKNEXPOPFIELD_FIVE_TEXT_ARRAY );
    iValueTextArray = CAknQueryValueTextArray::NewL();
    iValueTextArray->SetArray( *iTextArray );

    // Set the same textarray into all the values, and initial value index.
    for ( index = KAknExPopFieldIndexInitialize; index < count; index++ )
        {
        iTextValues[ index ] = CAknQueryValueText::NewL();
        iTextValues[ index ]->SetArrayL( iValueTextArray );
        iTextValues[ index ]->SetCurrentValueIndex( 0 );
        iTextValues[ index ]->SetQueryCaption( R_AKNEXPOPFIELD_QUERY_PROMPT );
        // Set values into popup fields.
        popup[ index ]->SetQueryValueL( iTextValues[ index ] ); // Moved up 
                                                                // from below.
        }
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldTextValueDialog::SaveFormDataL()
// Save form data.
// ----------------------------------------------------------------------------
//
TBool CAknExPopFieldTextValueDialog::SaveFormDataL()
    {
    for ( TInt i = KAknExPopFieldIndexInitialize;
              i < KAknExPopFieldMaxTextValueField; i++ )
        {
        if ( ControlOrNull( EAknExPopFieldDlgCtrlIdPop01+i ) )
            {
            iIndexValue[ i ] = iTextValues[ i ]->CurrentValueIndex();
            }
        }

    return ETrue;
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldTextValueDialog::DoNotSaveFormDataL()
// Does not save form data.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldTextValueDialog::DoNotSaveFormDataL()
    {
    for ( TInt i = KAknExPopFieldIndexInitialize;
              i < KAknExPopFieldMaxTextValueField; i++ )
        {
        if( ControlOrNull( EAknExPopFieldDlgCtrlIdPop01+i ) )
            {
            iTextValues[ i ]->SetCurrentValueIndex( iIndexValue[ i ] );
            }
        }
    }

// End of File

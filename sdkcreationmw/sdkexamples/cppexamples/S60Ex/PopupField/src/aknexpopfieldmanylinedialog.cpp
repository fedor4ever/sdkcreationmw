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
#include "AknExPopFieldManyLineDialog.h"


// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CAknExPopFieldManyLineDialog* CAknExPopFieldManyLineDialog::NewLC()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CAknExPopFieldManyLineDialog* CAknExPopFieldManyLineDialog::NewLC()
    {
    CAknExPopFieldManyLineDialog* self =
        new ( ELeave ) CAknExPopFieldManyLineDialog;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldManyLineDialog::~CAknExPopFieldManyLineDialog()
// Destructor.
// ----------------------------------------------------------------------------
//
CAknExPopFieldManyLineDialog::~CAknExPopFieldManyLineDialog()
    {
    DiscardTextArray();
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldManyLineDialog::CAknExPopFieldManyLineDialog()
// C++ default constructor can NOT contain any code, that might leave.
// ----------------------------------------------------------------------------
//
CAknExPopFieldManyLineDialog::CAknExPopFieldManyLineDialog():  
                              iTextArray( NULL ),
                              iValueTextArray( NULL )
    {

    TInt index( KAknExPopFieldMLIndexInitialize );
    for ( index = KAknExPopFieldMLIndexInitialize;
          index < KAknExPopFieldMaxMLTextValueField; index++ )
        {
        iTextValues[ index ] = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldManyLineDialog::DiscardTextArray()
// Discard text array in popup field object.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldManyLineDialog::DiscardTextArray()
    {
    TInt index( KAknExPopFieldMLIndexInitialize );
    for ( index = KAknExPopFieldMLIndexInitialize;
          index < KAknExPopFieldMaxMLTextValueField; index++ )
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
// CAknExPopFieldManyLineDialog::ConfigurePopfieldValueL()
// Configuration values (text) into Popup Fields.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldManyLineDialog::ConfigurePopfieldValueL()
    {
    // Discard textarray.
    DiscardTextArray();

    // Calculate number of pop-up field.
    TInt index( KAknExPopFieldMLIndexInitialize );
    TInt count( KAknExPopFieldMLCountInitialize );
    TInt controlId( KAknExPopFieldMLControlIdInitialize );
    CAknPopupField* popup[ KAknExPopFieldMaxMLTextValueField ];
    for ( index = KAknExPopFieldMLIndexInitialize;
          index < KAknExPopFieldMaxMLTextValueField; index++ )
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
                            R_AKNEXPOPFIELD_TWELVE_TEXT_ARRAY );

    iValueTextArray = CAknQueryValueTextArray::NewL();
    iValueTextArray->SetArray( *iTextArray );

    // Set the same textarray into all the values, and initial value index.
    for ( index = KAknExPopFieldMLIndexInitialize; index < count; index++ )
        {
        iTextValues[ index ] = CAknQueryValueText::NewL();
        iTextValues[ index ]->SetArrayL( iValueTextArray );
        iTextValues[ index ]->SetCurrentValueIndex( NULL );
        iTextValues[ index ]->SetQueryCaption( R_AKNEXPOPFIELD_QUERY_PROMPT );
        }

    // Set values into popup fields.
    for ( index = KAknExPopFieldMLIndexInitialize; index < count; index++ )
        {
        popup[ index ]->SetQueryValueL( iTextValues[ index ] );
        }
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldManyLineDialog::SaveFormDataL()
// Save form data.
// ----------------------------------------------------------------------------
//
TBool CAknExPopFieldManyLineDialog::SaveFormDataL()
    {
    for ( TInt i = KAknExPopFieldMLIndexInitialize;
              i < KAknExPopFieldMaxMLTextValueField; i++ )
        {
        if ( ControlOrNull( EAknExPopFieldDlgCtrlIdPop01+i ) )
            {
            iIndexValue[ i ] = iTextValues[ i ]->CurrentValueIndex();
            }
        }

    return ETrue;
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldManyLineDialog::DoNotSaveFormDataL()
// Does not save form data.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldManyLineDialog::DoNotSaveFormDataL()
    {
    for ( TInt i = KAknExPopFieldMLIndexInitialize;
              i < KAknExPopFieldMaxMLTextValueField; i++ )
        {
        if ( ControlOrNull( EAknExPopFieldDlgCtrlIdPop01+i ) )
            {
            iTextValues[ i ]->SetCurrentValueIndex( iIndexValue[ i ] );
            }
        }
    }

// End of File

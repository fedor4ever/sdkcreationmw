/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include <sensrvchannelfinder.h>
#include <sensrvchannel.h>
#include <sensrvaccelerometersensor.h>
#include <sensrvtappingsensor.h>
#include <avkon.hrh>
#include <aknnotewrappers.h>
#include <sensorexample.rsg>
#include "sensorexample.pan"
#include "sensorexampleAppUi.h"
#include "sensorexampleAppView.h"
#include "sensorexample.hrh"
#include "sensorexampledatalistener.h"

const TInt KInfoNoteBufferSize( 150 );
const TInt KInfoNoteTitleBufferSize( 50 );
const TInt KRealFormatWidth( 10 );
const TInt KRealFormatDecimalPlaces( 2 );
_LIT( KNewLine, "\n" );
_LIT( KHexPrefix, "0x" );
_LIT( KColon, ":" );
_LIT( KSpace, " " );

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CSensorExampleAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSensorExampleAppUi::ConstructL()
    {
    TBool showWarningNote( EFalse );
    TBuf<KInfoNoteBufferSize> warningText;    

    // Initialise app UI with standard value.
    BaseConstructL( CAknAppUi::EAknEnableSkin );

    // Create view object
    iAppView = CSensorExampleAppView::NewL( ClientRect() );

    iDataListener = CSensorExampleDataListener::NewL( iAppView );
    
    warningText.Append( iCoeEnv->AllocReadResourceLC( R_SE_NOTFOUND )->Des() );
    CleanupStack::PopAndDestroy();//R_SE_NOTFOUND

    TRAPD( err , 
           OpenChannelL( iAccelerometerChannel, 
                         KSensrvChannelTypeIdAccelerometerXYZAxisData ) );
    if( KErrNone != err || !iAccelerometerChannel )
        {
        warningText.Append( KNewLine );
        warningText.Append( iCoeEnv->AllocReadResourceLC( R_SE_ACCELEROMETERCHANNEL )->Des() );
        CleanupStack::PopAndDestroy();//R_SE_ACCELEROMETERCHANNEL
        showWarningNote = ETrue;
        }

    TRAP( err , OpenChannelL( iDblTapChannel, 
                              KSensrvChannelTypeIdAccelerometerDoubleTappingData ) );
    if( KErrNone != err || !iDblTapChannel )
        {
        warningText.Append( KNewLine );
        warningText.Append( iCoeEnv->AllocReadResourceLC( R_SE_DOUBLETAPCHANNEL )->Des() );
        CleanupStack::PopAndDestroy();//R_SE_DOUBLETAPCHANNEL
        showWarningNote = ETrue;
        }

    TRAP( err , OpenChannelL( iOrientationChannel, KSensrvChannelTypeIdOrientationData ) );
    if( KErrNone != err || !iOrientationChannel )
        {
        warningText.Append( KNewLine );
        warningText.Append( iCoeEnv->AllocReadResourceLC( R_SE_ORIENTATIONCHANNEL )->Des() );
        CleanupStack::PopAndDestroy();//R_SE_ORIENTATIONCHANNEL
        showWarningNote = ETrue;
        }

    //In case of some channel opening fails, show error note
    if( showWarningNote )
        {
        CAknWarningNote* note = new ( ELeave ) CAknWarningNote( ETrue );
        note->ExecuteLD( warningText );
        }
    }

// -----------------------------------------------------------------------------
// CSensorExampleAppUi::OpenChannelL()
// -----------------------------------------------------------------------------
//
void CSensorExampleAppUi::OpenChannelL( CSensrvChannel*& aChannel,
                                        TSensrvChannelTypeId aChannelTypeId )
    {
    CSensrvChannelFinder* finder = CSensrvChannelFinder::NewLC();
    RSensrvChannelInfoList channelList;
    CleanupClosePushL( channelList );

    TSensrvChannelInfo info;
    info.iChannelType = aChannelTypeId;
    finder->FindChannelsL( channelList, info );

    if ( channelList.Count() > 0 )
        {
        info = channelList[0];

        aChannel = CSensrvChannel::NewL( info );
        aChannel->OpenChannelL();
        aChannel->StartDataListeningL( iDataListener, 1, 1, 0 );
        }

    channelList.Close();
    CleanupStack::Pop( &channelList );
    CleanupStack::PopAndDestroy( finder );
    }

// -----------------------------------------------------------------------------
// CSensorExampleAppUi::CSensorExampleAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CSensorExampleAppUi::CSensorExampleAppUi()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CSensorExampleAppUi::~CSensorExampleAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CSensorExampleAppUi::~CSensorExampleAppUi()
    {
    delete iDataListener;
    delete iAppView;

    if( iDblTapChannel )
        {
        iDblTapChannel->StopDataListening();
        iDblTapChannel->CloseChannel();
        delete iDblTapChannel;
        }

    if( iAccelerometerChannel )
        {
        iAccelerometerChannel->StopDataListening();
        iAccelerometerChannel->CloseChannel();
        delete iAccelerometerChannel;
        }

    if( iOrientationChannel )
        {
        iOrientationChannel->StopDataListening();
        iOrientationChannel->CloseChannel();
        delete iOrientationChannel;
        }

    }

// -----------------------------------------------------------------------------
// CSensorExampleAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CSensorExampleAppUi::HandleCommandL( TInt aCommand )
    {

    switch( aCommand )
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            {
            Exit();
            }
            break;

        case ESensorProperties:
        case EGetValue:
        case ESetValue:
            {
            HandleQueryL( aCommand );
            }
            break;

        default:
            Panic( ESensorExampleUi );
            break;
        }
    }

// -----------------------------------------------------------------------------
// CSensorExampleAppUi::HandleQuery()
// -----------------------------------------------------------------------------
//
void CSensorExampleAppUi::HandleQueryL( TInt aCommand )
    {
    CSensrvChannelFinder* finder = CSensrvChannelFinder::NewLC();
    RSensrvChannelInfoList channelList;
    CleanupClosePushL( channelList );

    TSensrvChannelInfo channelInfo;
    finder->FindChannelsL( channelList, channelInfo );

    TInt selectedChannelIndex = iAppView->ShowQueryDialogL( channelList );

    if( KErrCancel != selectedChannelIndex )
        {
        //List all properties of the selected channel
        RSensrvPropertyList channelPropertyList;
        CleanupClosePushL( channelPropertyList );
        CSensrvChannel* selectedChannel;
        selectedChannel = CSensrvChannel::NewL( channelList[selectedChannelIndex] );
        CleanupStack::PushL( selectedChannel );
        selectedChannel->OpenChannelL();

        selectedChannel->GetAllPropertiesL( channelPropertyList );
        selectedChannelIndex = iAppView->ShowQueryDialogL( channelPropertyList );

        if( KErrCancel != selectedChannelIndex )
            {
            switch( aCommand )
                {
                case ESensorProperties:
                    {
                    ShowPropertyNoteL( channelPropertyList[selectedChannelIndex] );
                    }
                    break;

                case EGetValue:
                    {
                    ShowPropertyValueNoteL( channelPropertyList[selectedChannelIndex] );
                    }
                    break;

                case ESetValue:
                    {
                    HandlePropertyValueQueryL( channelPropertyList[selectedChannelIndex],
                                               selectedChannel );
                    }
                    break;

                default:
                    Panic( ESensorExampleUi );
                    break;
                }
            }

        selectedChannel->CloseChannel();
        CleanupStack::PopAndDestroy( selectedChannel );
        channelPropertyList.Close();
        CleanupStack::Pop( &channelPropertyList );
        }

    channelList.Close();
    CleanupStack::Pop( &channelList );
    CleanupStack::PopAndDestroy( finder );
    }

// -----------------------------------------------------------------------------
// CSensorExampleAppUi::ShowPropertyNoteL()
// -----------------------------------------------------------------------------
//
void CSensorExampleAppUi::ShowPropertyNoteL( TSensrvProperty& aProperty )
   {
    TBuf<KInfoNoteBufferSize> buffer;

    //Property identifier
    buffer.Append( iCoeEnv->AllocReadResourceLC( R_SE_PROPERTYID )->Des() );
    CleanupStack::PopAndDestroy();//R_SE_PROPERTYID
    buffer.Append( KColon );
    buffer.Append( KHexPrefix );
    buffer.AppendNumUC( aProperty.GetPropertyId(), EHex );

    //Property type
    buffer.Append( KSpace );
    buffer.Append( iCoeEnv->AllocReadResourceLC( R_SE_PROPERTYTYPE )->Des() );
    CleanupStack::PopAndDestroy();//R_SE_PROPERTYTYPE
    buffer.Append( KColon );

    switch( aProperty.PropertyType() )
        {
        case ESensrvIntProperty:
            {
            buffer.Append( iCoeEnv->AllocReadResourceLC( R_SE_PROPERTYTYPEINT )->Des() );
            CleanupStack::PopAndDestroy();//R_SE_PROPERTYTYPEINT
            }
            break;

        case ESensrvRealProperty:
            {
            buffer.Append( iCoeEnv->AllocReadResourceLC( R_SE_PROPERTYTYPEREAL )->Des() );
            CleanupStack::PopAndDestroy();//R_SE_PROPERTYTYPEREAL
            }
            break;

        case ESensrvBufferProperty:
            {
            buffer.Append( iCoeEnv->AllocReadResourceLC( R_SE_PROPERTYTYPEBUFFER )->Des() );
            CleanupStack::PopAndDestroy();//R_SE_PROPERTYTYPEBUFFER
            }
            break;

        default:
            {
            buffer.Append( iCoeEnv->AllocReadResourceLC( R_SE_PROPERTYTYPENOTYPE )->Des() );
            CleanupStack::PopAndDestroy();//R_SE_PROPERTYTYPENOTYPE
            }
            break;
        }

    //Item index
    buffer.Append( KNewLine );
    buffer.Append( iCoeEnv->AllocReadResourceLC( R_SE_PROPERTYITEMINDEX )->Des() );
    CleanupStack::PopAndDestroy();//R_SE_PROPERTYITEMINDEX
    buffer.Append( KColon );
    buffer.AppendNum( aProperty.PropertyItemIndex() );

    //Array index
    buffer.Append( KSpace );
    buffer.Append( iCoeEnv->AllocReadResourceLC( R_SE_PROPERTYARRAYINDEX )->Des() );
    CleanupStack::PopAndDestroy();//R_SE_PROPERTYARRAYINDEX
    buffer.Append( KColon );
    buffer.AppendNum( aProperty.GetArrayIndex() );

    //Read only
    buffer.Append( KNewLine );
    if( aProperty.ReadOnly() )
        {
        buffer.Append( iCoeEnv->AllocReadResourceLC( R_SE_PROPERTYROETRUE )->Des() );
        CleanupStack::PopAndDestroy();//R_SE_PROPERTYROETRUE
        }
    else
        {
        buffer.Append( iCoeEnv->AllocReadResourceLC( R_SE_PROPERTYROEFALSE )->Des() );
        CleanupStack::PopAndDestroy();//R_SE_PROPERTYROEFALSE
        }

    //Property value
    buffer.Append( KNewLine );
    buffer.Append( iCoeEnv->AllocReadResourceLC( R_SE_PROPERTYVALUE )->Des() );
    CleanupStack::PopAndDestroy();//R_SE_PROPERTYVALUE
    buffer.Append( KColon );

    switch( aProperty.PropertyType() )
        {
        case ESensrvIntProperty:
            {
            //Value
            TInt value( 0 );
            aProperty.GetValue( value );
            buffer.AppendNum( value );

            //Max value
            buffer.Append( KNewLine );
            aProperty.GetMaxValue( value );
            buffer.Append( iCoeEnv->AllocReadResourceLC( R_SE_PROPERTYMAX )->Des() );
            CleanupStack::PopAndDestroy();//R_SE_PROPERTYMAX
            buffer.Append( KColon );
            buffer.AppendNum( value );

            //Min value
            buffer.Append( KSpace );
            aProperty.GetMinValue( value );
            buffer.Append( iCoeEnv->AllocReadResourceLC( R_SE_PROPERTYMIN )->Des() );
            CleanupStack::PopAndDestroy();//R_SE_PROPERTYMIN
            buffer.Append( KColon );
            buffer.AppendNum( value );
            }
            break;

        case ESensrvRealProperty:
            {
            //Value
            TReal value( 0 );
            aProperty.GetValue( value );
            TRealFormat realFormat( KRealFormatWidth, KRealFormatDecimalPlaces );
            buffer.AppendNum( value, realFormat );

            //Max value
            buffer.Append( KNewLine );
            aProperty.GetMaxValue( value );
            buffer.Append( iCoeEnv->AllocReadResourceLC( R_SE_PROPERTYMAX )->Des() );
            CleanupStack::PopAndDestroy();//R_SE_PROPERTYMAX
            buffer.Append( KColon );
            buffer.AppendNum( value, realFormat );

            //Min value
            buffer.Append( KSpace );
            aProperty.GetMinValue( value );
            buffer.Append( iCoeEnv->AllocReadResourceLC( R_SE_PROPERTYMIN )->Des() );
            CleanupStack::PopAndDestroy();//R_SE_PROPERTYMIN
            buffer.Append( KColon );
            buffer.AppendNum( value, realFormat );
            }
            break;

        case ESensrvBufferProperty:
            {
            TBuf8<KSensrvPropertyTextBufferSize> buf8;
            TBuf<KSensrvPropertyTextBufferSize> buf;
            aProperty.GetValue( buf8 );
            buf.Copy( buf8 );
            buffer.Append( buf );
            }
            break;

        default:
            {
            buffer.Append( iCoeEnv->AllocReadResourceLC( R_SE_PROPERTYUNINITIALIZED )->Des() );
            CleanupStack::PopAndDestroy();//R_SE_PROPERTYUNINITIALIZED
            }
            break;
        }

    ShowNoteL( buffer );
   }

// -----------------------------------------------------------------------------
// CSensorExampleAppUi::ShowPropertyValueNoteL()
// -----------------------------------------------------------------------------
//
void CSensorExampleAppUi::ShowPropertyValueNoteL( TSensrvProperty& aProperty )
    {
    TBuf<KInfoNoteBufferSize> buffer;

    buffer.Append( iCoeEnv->AllocReadResourceLC( R_SE_PROPERTYVALUE )->Des() );
    CleanupStack::PopAndDestroy();//R_SE_PROPERTYVALUE
    buffer.Append( KColon );

    switch( aProperty.PropertyType() )
        {
        case ESensrvIntProperty:
            {
            TInt value( 0 );
            aProperty.GetValue( value );
            buffer.AppendNum( value );
            }
            break;

        case ESensrvRealProperty:
            {
            TReal value( 0 );
            aProperty.GetValue( value );
            TRealFormat realFormat( KRealFormatWidth, KRealFormatDecimalPlaces );
            buffer.AppendNum( value, realFormat );
            }
            break;

        case ESensrvBufferProperty:
            {
            TBuf8<KSensrvPropertyTextBufferSize> bufValue;
            aProperty.GetValue( bufValue );
            buffer.Copy( bufValue );
            }
            break;

        default:
            {
            buffer.Append( iCoeEnv->AllocReadResourceLC( R_SE_PROPERTYUNINITIALIZED )->Des() );
            CleanupStack::PopAndDestroy();//R_SE_PROPERTYUNINITIALIZED
            }
            break;
        }

    ShowNoteL( buffer );
    }

// -----------------------------------------------------------------------------
// CSensorExampleAppUi::HandlePropertyValueQueryL()
// -----------------------------------------------------------------------------
//
void CSensorExampleAppUi::HandlePropertyValueQueryL( TSensrvProperty& aProperty,
                                                     CSensrvChannel* aChannel )
    {
    TBuf<KInfoNoteBufferSize> buffer;

    if( !aProperty.ReadOnly() )
        {
        if( ESensrvIntProperty == aProperty.PropertyType() )
            {
            TInt value(0);
            TInt min(0);
            TInt max(0);
            aProperty.GetValue( value );
            aProperty.GetMaxValue( max );
            aProperty.GetMinValue( min );
            CAknNumberQueryDialog* dlg;
            dlg = CAknNumberQueryDialog::NewL( value );
            TBuf<KInfoNoteTitleBufferSize> title;
            title.Format( iCoeEnv->AllocReadResourceLC( R_SE_VALUEQUERYTITLE )->Des(), min, max );
            CleanupStack::PopAndDestroy();//R_SE_VALUEQUERYTITLE

            if( EEikBidOk == dlg->ExecuteLD( R_SENSOREXAMPLE_DIALOG_VALUE_EDIT_QUERY, title ) )
                {
                aProperty.SetValue( value );
                TInt ret = aChannel->SetProperty( aProperty );

                if( KErrNone == ret )
                    {
                    TSensrvProperty property;
                    aChannel->GetPropertyL( aProperty.GetPropertyId(),
                                            aProperty.PropertyItemIndex(),
                                            aProperty.GetArrayIndex(),
                                            property );
                    property.GetValue( value );

                    buffer.Append( iCoeEnv->AllocReadResourceLC( R_SE_VALUESETTO )->Des() );
                    CleanupStack::PopAndDestroy();//R_SE_VALUESETTO
                    buffer.AppendNum( value );
                    }
                else
                    {
                    buffer.Append( iCoeEnv->AllocReadResourceLC( R_SE_VALUESETERROR )->Des() );
                    CleanupStack::PopAndDestroy();//R_SE_VALUESETERROR
                    buffer.AppendNum( ret );
                    }
                ShowNoteL( buffer );
                }
            else
                {
                //Cancel was pressed
                }
            }
        else
            {
            buffer.Append( iCoeEnv->AllocReadResourceLC( R_SE_VALUESETINTEGERONLY )->Des() );
            CleanupStack::PopAndDestroy();//R_SE_VALUESETINTEGERONLY
            ShowNoteL( buffer );
            }
        }
    else
        {
        buffer.Append( iCoeEnv->AllocReadResourceLC( R_SE_VALUESETREADONLY )->Des() );
        CleanupStack::PopAndDestroy();//R_SE_VALUESETREADONLY
        ShowNoteL( buffer );
        }
    }

// -----------------------------------------------------------------------------
//  CSensorExampleAppUi::ShowNoteL
// -----------------------------------------------------------------------------
//
void CSensorExampleAppUi::ShowNoteL( const TDesC& aNoteText )
    {
    CAknNoteDialog* note = NULL;

    note = new ( ELeave ) CAknNoteDialog( CAknNoteDialog::ENoTone, CAknNoteDialog::ENoTimeout );
    note->PrepareLC( R_SENSOREXAMPLE_INFORMATION_NOTE ); // Adds to CleanupStack
    note->SetTextWrapping( ETrue );
    note->SetTextL( aNoteText );
    note->RunLD();
    // No need to Pop 'note' from Cleanup Stack since it is deleted in RunLD
    }

// -----------------------------------------------------------------------------
//  Called by the framework when the application status pane
//  size is changed.  Passes the new client rectangle to the
//  AppView
// -----------------------------------------------------------------------------
//
void CSensorExampleAppUi::HandleStatusPaneSizeChange()
    {
    iAppView->SetRect( ClientRect() );
    }

// End of File

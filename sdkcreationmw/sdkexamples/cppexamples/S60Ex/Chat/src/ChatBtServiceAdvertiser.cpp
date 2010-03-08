/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include <bt_sock.h>
#include "ChatBtServiceAdvertiser.h"
#include "Chat.pan"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CChatBtServiceAdvertiser::CChatBtServiceAdvertiser()
// Constructor.
// ----------------------------------------------------------------------------
//
CChatBtServiceAdvertiser::CChatBtServiceAdvertiser() :
    iRecord( 0 ), 
    iIsConnected( EFalse )
    {
    // no implementation required
    }

// ----------------------------------------------------------------------------
// CChatBtServiceAdvertiser::~CChatBtServiceAdvertiser()
// Destructor.
// ----------------------------------------------------------------------------
//
CChatBtServiceAdvertiser::~CChatBtServiceAdvertiser()
    {
    if ( IsAdvertising() )
        {
        TRAPD( err,StopAdvertisingL() );
        if ( err != KErrNone )
            {
            User::Panic( KPanicBTServiceAdvertiser, err );
            }
        }

    iSdpDatabase.Close();
    iSdpSession.Close();
    }

// ----------------------------------------------------------------------------
// CChatBtServiceAdvertiser::ConnectL()
// Connect to the SDP database.
// ----------------------------------------------------------------------------
//
void CChatBtServiceAdvertiser::ConnectL()
    {
    if ( !iIsConnected )
        {
        User::LeaveIfError( iSdpSession.Connect() );
        User::LeaveIfError( iSdpDatabase.Open( iSdpSession ) );
        iIsConnected = ETrue;
        }
    }

// ----------------------------------------------------------------------------
// CChatBtServiceAdvertiser::StartAdvertisingL()
// Start the advertising of this service.
// ----------------------------------------------------------------------------
//
void CChatBtServiceAdvertiser::StartAdvertisingL( TInt aPort )
    {
    if ( IsAdvertising() )
        {
        // could be advertising on a different port
        StopAdvertisingL(); 
        }
    if ( ! iIsConnected )
        {
        ConnectL();
        }
    iSdpDatabase.CreateServiceRecordL( ServiceClass(), iRecord );

    // add a Protocol to the record
    CSdpAttrValueDES* vProtocolDescriptor = CSdpAttrValueDES::NewDESL( NULL );
    CleanupStack::PushL( vProtocolDescriptor );
 
    BuildProtocolDescriptionL( vProtocolDescriptor,aPort );

    iSdpDatabase.UpdateAttributeL( iRecord, KSdpAttrIdProtocolDescriptorList, 
        *vProtocolDescriptor );

    CleanupStack::PopAndDestroy( vProtocolDescriptor );

    // Add a name to the record
    iSdpDatabase.UpdateAttributeL( iRecord, 
        KSdpAttrIdBasePrimaryLanguage + 
        KSdpAttrIdOffsetServiceName, 
        ServiceName() );

    // Add a description to the record
    iSdpDatabase.UpdateAttributeL( iRecord, 
        KSdpAttrIdBasePrimaryLanguage + 
        KSdpAttrIdOffsetServiceDescription, 
        ServiceDescription() );

    }

// ----------------------------------------------------------------------------
// CChatBtServiceAdvertiser::UpdateAvailabilityL()
// Update the service availability field of the service record.
// ----------------------------------------------------------------------------
//
void CChatBtServiceAdvertiser::UpdateAvailabilityL( TBool aIsAvailable )
    {
    TUint state;
    if ( aIsAvailable )
        {
        state = 0xFF;  //  Fully unused
        }
    else
        {
        state = 0x00;  //  Fully used -> can't connect
        }

    //  Update the availibility attribute field
    iSdpDatabase.UpdateAttributeL( iRecord, 
        KSdpAttrIdServiceAvailability, 
        state );

    //  Mark the record as changed - by increasing its state number (version)
    iSdpDatabase.UpdateAttributeL( iRecord, 
        KSdpAttrIdServiceRecordState, 
        ++iRecordState );
    }

// ----------------------------------------------------------------------------
// CChatBtServiceAdvertiser::StopAdvertisingL()
// Stop advertising this service. Remove the record from the sdp database.
// ----------------------------------------------------------------------------
//
void CChatBtServiceAdvertiser::StopAdvertisingL()
    {
    if ( IsAdvertising() )
        {
        iSdpDatabase.DeleteRecordL( iRecord );
        iRecord = 0;
        }
    }

// ----------------------------------------------------------------------------
// CChatBtServiceAdvertiser::IsAdvertising()
// Does the SDP database contain a record for this service.
// ----------------------------------------------------------------------------
//
TBool CChatBtServiceAdvertiser::IsAdvertising()
    {
    return iRecord != 0;
    }

// End of File

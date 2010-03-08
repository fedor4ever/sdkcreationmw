/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Bluetooth RFCOMM bearer
 *
*/


#include <bt_sock.h>
#include "BTServiceAdvertiser.h"

_LIT(KServiceName, "EcmtAgent");
_LIT(KServiceDesc, "Bluetooth bearer for EcmtAgent");

// ----------------------------------------------------------------------------
// CBTServiceAdvertiser::CBTServiceAdvertiser()
// Constructor.
// ----------------------------------------------------------------------------
CBTServiceAdvertiser::CBTServiceAdvertiser():
iRecord(0),
iIsConnected(EFalse)
{
}

// ----------------------------------------------------------------------------
// CBTServiceAdvertiser::~CBTServiceAdvertiser()
// Destructor. Of course.
// ----------------------------------------------------------------------------
CBTServiceAdvertiser::~CBTServiceAdvertiser()
{
    if (IsAdvertising())
    {
        TRAP_IGNORE(StopAdvertisingL());
    }

    iSdpDb.Close();
    iSdpSession.Close();
}

// ----------------------------------------------------------------------------
// CBTServiceAdvertiser::ConnectL()
// Connect to the SDP database.
// ----------------------------------------------------------------------------
void CBTServiceAdvertiser::ConnectL()
{
    if ( !iIsConnected )
    {
        User::LeaveIfError(iSdpSession.Connect());
        User::LeaveIfError(iSdpDb.Open(iSdpSession));
        iIsConnected = ETrue;
    }
}

// ----------------------------------------------------------------------------
// CBTServiceAdvertiser::UpdateAttributeL()
// Updates the specified SDK attribute of the current record
// ----------------------------------------------------------------------------
void CBTServiceAdvertiser::UpdateAttributeL(TSdpAttributeID aAttrID, 
                                            CSdpAttrValue* aValue)
{
    iSdpDb.UpdateAttributeL(iRecord, aAttrID, *aValue);
}

// ----------------------------------------------------------------------------
// CBTServiceAdvertiser::StartAdvertisingL()
// Start the advertising of this service.
// ----------------------------------------------------------------------------
void CBTServiceAdvertiser::StartAdvertisingL( TInt aPort )
{
    // Reset the state
    if (!iIsConnected)
    {
        ConnectL();
    }
    else if (IsAdvertising())
    {
        // Could be advertising on a different port
        StopAdvertisingL();
    }

    iSdpDb.CreateServiceRecordL(KSerialPortUUID, iRecord);

    // KSdpAttrIdProtocolDescriptorList
    CSdpAttrValueDES* attVal = CSdpAttrValueDES::NewDESL(NULL);
    CleanupStack::PushL(attVal);
    attVal
    ->StartListL()
        ->BuildDESL()
        ->StartListL() 
            ->BuildUUIDL(KL2CAP)
            ->BuildUintL(TSdpIntBuf<TUint16>(KRFCOMM))
        ->EndListL()
        ->BuildDESL()
        ->StartListL()
            ->BuildUUIDL(KRFCOMM )
            ->BuildUintL(TSdpIntBuf<TUint8>(TUint8(aPort)))
        ->EndListL()
    ->EndListL();
    UpdateAttributeL(KSdpAttrIdProtocolDescriptorList,attVal);
    CleanupStack::PopAndDestroy(attVal);

    // KSdpAttrIdLanguageBaseAttributeIDList
    attVal = CSdpAttrValueDES::NewDESL(NULL);
    CleanupStack::PushL(attVal);
    attVal
    ->StartListL()
        ->BuildUintL(TSdpIntBuf<TUint16>(KLanguageEnglish))
        ->BuildUintL(TSdpIntBuf<TUint16>(0x006a)) // UTF-8, I think
        ->BuildUintL(TSdpIntBuf<TUint16>(KSdpAttrIdBasePrimaryLanguage))
    ->EndListL();
    UpdateAttributeL(KSdpAttrIdLanguageBaseAttributeIDList,attVal);
    CleanupStack::PopAndDestroy(attVal);

    // KSdpAttrIdBrowseGroupList
    attVal = CSdpAttrValueDES::NewDESL(NULL);
    CleanupStack::PushL(attVal);
    attVal
    ->StartListL()
        ->BuildUUIDL(KPublicBrowseGroupUUID)
    ->EndListL();
    UpdateAttributeL(KSdpAttrIdBrowseGroupList,attVal);
    CleanupStack::PopAndDestroy(attVal);

    // KSdpAttrIdServiceInfoTimeToLive
    iSdpDb.UpdateAttributeL(iRecord,KSdpAttrIdServiceInfoTimeToLive,30);

    // Name and description
    iSdpDb.UpdateAttributeL(iRecord,
                            KSdpAttrIdBasePrimaryLanguage + 
                            KSdpAttrIdOffsetServiceName, 
                            KServiceName());
    iSdpDb.UpdateAttributeL(iRecord, 
                            KSdpAttrIdBasePrimaryLanguage + 
                            KSdpAttrIdOffsetServiceDescription, 
                            KServiceDesc());

    // Finally, update the availibility attribute
    CSdpAttrValueUint* value;
    value = CSdpAttrValueUint::NewUintL(TSdpIntBuf<TUint8>(0xff));
    CleanupStack::PushL(value);
    UpdateAttributeL(KSdpAttrIdServiceAvailability, value);
    CleanupStack::PopAndDestroy(value);
}

// ----------------------------------------------------------------------------
// CBTServiceAdvertiser::StopAdvertisingL()
// Stop advertising this service. Remove the record from the sdp database.
// ----------------------------------------------------------------------------
void CBTServiceAdvertiser::StopAdvertisingL()
{
    if (IsAdvertising())
    {
        iSdpDb.DeleteRecordL( iRecord );
        iRecord = 0;
    }
}

// ----------------------------------------------------------------------------
// CBTServiceAdvertiser::IsAdvertising()
// Does the SDP database contain a record for this service.
// ----------------------------------------------------------------------------
TBool CBTServiceAdvertiser::IsAdvertising() const
{
    return iRecord != 0;
}

/**
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

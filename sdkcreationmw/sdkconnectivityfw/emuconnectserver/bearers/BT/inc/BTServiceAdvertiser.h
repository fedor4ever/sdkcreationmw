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



#ifndef __CBTSERVICE_ADVERTISER__
#define __CBTSERVICE_ADVERTISER__

// INCLUDES
#include <e32base.h>
#include <btsdp.h>
#include <btmanclient.h>

/**
* CBTServiceAdvertiser
* Advertises a service in the SDP database.
*/
class CBTServiceAdvertiser : public CBase
{
public:

    /**
     * CBTServiceAdvertiser
     * Construct this object 
     */
    CBTServiceAdvertiser();

    /**
     * ~CBTServiceAdvertiser
     * Destroy the object, close all open handles and remove 
     * the advertised service.
     */
    virtual ~CBTServiceAdvertiser();

public:
    /**
     * StartAdvertisingL
     * Start the advertising of this service
     * @param aPort the port being used by the service 
     */
    void StartAdvertisingL( TInt aPort );

    /**
     * StopAdvertisingL
     * Stop advertising this service. Remove the record 
     * from the sdp database
     */
    void StopAdvertisingL();

    /**
     * IsAdvertising
     * Does the SDP database contain a record for this service
     * @return ETrue whether the service is being advertised
     */
    TBool IsAdvertising() const;

private:

    /**
     * ConnectL
     * Connect to the SDP database
     */  
    void ConnectL();

    /**
     * UpdateAttributeL
     * Updates the specified SDP attribute of the currently active record
     */
    void UpdateAttributeL(TSdpAttributeID aAttrID, CSdpAttrValue* aValue);

private:

    /** iSdpSession a handle to the SDP session */
    RSdp iSdpSession;

    /** iSdpDatabase a connection to the SDP database */
    RSdpDatabase iSdpDb;

    /** iRecord the record handle of the service record for this server */
    TSdpServRecordHandle iRecord;

    /** iIsConnected has a connection been made to the SDP Database */
    TBool iIsConnected;
};

#endif // __CBTSERVICE_ADVERTISER__

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

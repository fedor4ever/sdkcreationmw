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


#ifndef CHATBTSERVICEADVERTISER_H
#define CHATBTSERVICEADVERTISER_H

// INCLUDES
#include <e32base.h>
#include <btsdp.h>
#include <StringLoader.h>

#include "ChatBtServiceAdvertiser.h"

// CLASS DECLARATIONS

/**
* CChatBtServiceAdvertiser
* Advertises a service in the SDP database.
*/
class CChatBtServiceAdvertiser : public CBase
    {
    public: // Destructor

        /**
        * ~CChatBtServiceAdvertiser
        * Destroy the object, close all open handles and remove 
        * the advertised service.
        */
        virtual ~CChatBtServiceAdvertiser();

    public: // New functions
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
        TBool IsAdvertising();

        /**
        * UpdateAvailabilityL
        * Update the service availability field of the service record
        * @param aIsAvailable ETrue is the service is not busy.
        */
        void UpdateAvailabilityL( TBool aIsAvailable );

    protected:  // New functions

        /**
        * CChatBtServiceAdvertiser
        * Construct this object 
        */
        CChatBtServiceAdvertiser();

        /**
        * BuildProtocolDescriptionL
        * Builds the protocol description
        * @param aProtocolDescriptor the protocol descriptor
        * @param aPort the service port
        */
        virtual void BuildProtocolDescriptionL( 
            CSdpAttrValueDES* aProtocolDescriptor, TInt aPort ) = 0;

        /**
        * ServiceClass
        * @return the service class
        */
        virtual TInt ServiceClass() = 0;

        /**
        * ServiceName
        * @return the service name
        */
        virtual const TDesC& ServiceName() = 0;

        /**
        * ServiceDescription
        * @return the service description
        */
        virtual const TDesC& ServiceDescription() = 0;
    
    private:    // Function from base classes

        /**
        * ConnectL
        * Connect to the SDP database
        */  
        void ConnectL();

    private:    // data

        /** 
        * iSdpSession
        * a handle to the SDP session 
        */
        RSdp iSdpSession;

        /** 
        * iSdpDatabase
        * a connection to the SDP database 
        */
        RSdpDatabase iSdpDatabase;

        /** 
        * iRecord
        * the record handle of the service record for this server 
        */
        TSdpServRecordHandle iRecord;

        /** 
        * iRecordState
        * the current record state - change number 
        */
        TInt iRecordState;

        /** 
        * iIsConnected
        * has a connection been made to the SDP Database 
        */
        TBool iIsConnected;
    };

#endif // CHATBTSERVICEADVERTISER_H

// End of File

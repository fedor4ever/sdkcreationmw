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


#ifndef CHATSERVICEADVERTISER_H
#define CHATSERVICEADVERTISER_H

// INCLUDES
#include <e32base.h>
#include <btsdp.h>
#include "ChatBtServiceAdvertiser.h"


// CLASS DECLARATIONS
/**
* CChatServiceAdvertiser
* Advertises the message service using the SDP database.
*/
class CChatServiceAdvertiser : public CChatBtServiceAdvertiser
    {
    public: // Constructors and destructor

        /**
        * NewL()
        * Create a CChatServiceAdvertiser object
        * @return a pointer to the created instance of
        * CChatServiceAdvertiser
        */
        static CChatServiceAdvertiser* NewL();

        /**
        * NewLC()
        * Create a CChatServiceAdvertiser object, which will
        * draw itself to aRect.
        * @return a pointer to the created instance of
        * CChatServiceAdvertiser
        */
        static CChatServiceAdvertiser* NewLC();

        /**
        * ~CChatServiceAdvertiser()
        * Destroy the object and release all memory objects.
        * Close any open sockets
        */
        virtual ~CChatServiceAdvertiser();

    protected: // from CChatBtServiceAdvertiser

        /**
        * BuildProtocolDescriptionL()
        * Builds the protocol description
        * @param aProtocolDescriptor the protocol descriptor
        * @param aPort the service port
        */
        void BuildProtocolDescriptionL(
            CSdpAttrValueDES* aProtocolDescriptor, TInt aPort );


        /**
        * ServiceClass()
        * @return the service class
        */
        TInt ServiceClass();

        /**
        * ServiceName()
        * @return the service name
        */
        const TDesC& ServiceName();

        /**
        * ServiceDescription()
        * @return the service description
        */
        const TDesC& ServiceDescription();

    private:    // Constructors

        /**
        * CChatServiceAdvertiser()
        * Constructs this object
        */
        CChatServiceAdvertiser();

        /**
        * ConstructL()
        * 2nd phase construction of this object
        */
        void ConstructL();

    private:    // data

        /**
        * iServiceName
        * Owned by CChatServiceAdvertiser
        */
        HBufC* iServiceName;

        /**
        * iServiceDescription
        * Owned by CChatServiceAdvertiser
        */
        HBufC* iServiceDescription;
    };

#endif // CHATSERVICEADVERTISER_H

// End of File

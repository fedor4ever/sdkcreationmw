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
* Description:  An interface class is notified when an async telephony command
*  execution is completed.
*
*/


#ifndef ISVTELINFOAPP_TELEPHONY_NOTIFY_H
#define ISVTELINFOAPP_TELEPHONY_NOTIFY_H

//INCLUDES
#include <etel3rdparty.h>

//CLASS DEFINITIONS
/**
* MIsvTelInfoAppTelephonyNotify: This class specifies an interface that should
* be implemented by a class if it wants to get notified on completion of async
* telephony commands
*/
class MIsvTelInfoAppTelephonyNotify
{
    public:
       /**
        * Pure virtual functions 
        */
       /** 
        * Gets invoked once command completion notification from 
        * GetPhoneId ETel API is received.
        * @param aStatus, specifies SUCCESS or Error code
        * @param aPhoneId, Contains Phone Id
        * @param aErrMsg, contains the error message returned by ETel API        
        */
        virtual void GetPhoneIdApiCompleteL(TInt aStatus, 
            CTelephony::TPhoneIdV1 aPhoneId, TDesC16& aErrMsg) = 0;
        
       /** 
        * Gets invoked once command completion notification from 
        * GetSubscriberId ETel API is received.
        * @param aStatus, specifies SUCCESS or Error code
        * @param aSubscriberId, contains Subscriber Id
        * @param aErrMsg, contains the error message returned by ETel API        
        */
        virtual void GetSubscriberIdApiCompleteL(TInt aStatus, 
            CTelephony::TSubscriberIdV1 aSubscriberId, TDesC16& aErrMsg) = 0;
        
       /** 
        * Gets invoked once command completion notification from 
        * GetNwRegistrationStatus ETel API is received.
        * @param aStatus, specifies SUCCESS or Error code
        * @param aNwRegistrationStatus Contains current network 
        * registration status.
        * @param aErrMsg, contains the error message returned by ETel API        
        */
        virtual void GetNwRegistrationStatusApiCompleteL(TInt aStatus, 
            CTelephony::TNetworkRegistrationV1 aNwRegistrationStatus, 
            TDesC16& aErrMsg) = 0;
        
       /** 
        * Gets invoked once command completion notification from 
        * GetBarringStatus ETel API is received.
        * @param aStatus, specifies SUCCESS or Error code
        * @param aBarringCondition,
        * contains barring condition 
        * @param aCallBarringServices Contains information about the 
        * call barring service.       
        * @param aErrMsg, contains the error message returned by ETel API        
        */
        virtual void GetBarringStatusApiCompleteL( 
            TInt aStatus,
            CTelephony::TCallBarringCondition aBarringCondition, 
            CTelephony::TCallBarringSupplServicesV1 aCallBarringServices,
            TDesC16& aErrMsg ) = 0;     
                                        
       /** 
        * Gets invoked once command completion notification from 
        * GetWaitingStatus ETel API is received.
        * @param aStatus, specifies SUCCESS or Error code
        * @param aCallWaitingServices Contains information about the 
        * call waiting service.
        * @param aErrMsg, contains the error message returned by ETel API        
        */
        virtual void GetWaitingStatusApiCompleteL(TInt aStatus, 
            CTelephony::TCallWaitingSupplServicesV1 aCallWaitingServices, 
            TDesC16& aErrMsg ) = 0;
        
       /** 
        * Gets invoked once command completion notification from 
        * GetIdentityServiceStatus ETel API is received.
        * @param aStatus, specifies SUCCESS or Error code
        * @param aIdentityServiceType,
        * contains identity serivce type        
        * @param aIdentityService Conatins call identity service status.
        * @param aErrMsg, contains the error message returned by ETel API        
        */
        virtual void GetIdentityServiceStatusApiCompleteL( 
            TInt aStatus, 
            CTelephony::TIdentityService aIdentityServiceType,
            CTelephony::TIdentityServiceV1 aIdentityService,
            TDesC16& aErrMsg ) = 0;
	};

#endif // ISVTELINFOAPP_TELEPHONY_NOTIFY_H

//End of file
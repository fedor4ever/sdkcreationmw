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
* Description:  A class that wraps the CTelephony class.
*
*/


#ifndef ISVTELINFOAPP_TELEPHONY_H
#define ISVTELINFOAPP_TELEPHONY_H

// INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <etel3rdparty.h>
#include <charconv.h>

// CONSTANTS
const TInt KMaxCmdSize = 100;
const TInt KMaxTelNumSize = 100;
const TInt KMaxErrMsgSize = 100;
const TInt KMaxDTMFLength = 80;

// FORWARD DECLARATIONS
class MIsvTelInfoAppTelephonyNotify;
class CTelephony;

// CLASS DEFINITIONS

/**
* CIsvTelInfoAppTelephony: This class derives from CActive and wraps the 
* CTelephony class. Provides a command table based execution for 
* telephony commands with command specific parsing capabilities.
*/
class CIsvTelInfoAppTelephony : public CActive
    {
    public:
         /**
        * Default constructor.
        * @param aNotify reference to the object that implements the 
        * MIsvTelInfoAppTelephonyNotify interface.
        */
        CIsvTelInfoAppTelephony(MIsvTelInfoAppTelephonyNotify& aNotify);
        
        /**
        * Second-phase constructor.
        */
        void ConstructL();
        
        /**
        * Two-phased constructor.
        * @param aNotify Reference to an object implementing the
        * MIsvTelInfoAppTelephonyNotify interface
        */
        static CIsvTelInfoAppTelephony* NewL(MIsvTelInfoAppTelephonyNotify& 
            aNotify);
        
        /**
        * Destructor.
        */
        ~CIsvTelInfoAppTelephony();

        //Wrapper functions for individual commands
        /**
        * Wrapper function that calls GetPhoneId of CTelephony class.
        */
        void TelApiWrapperGetPhoneIdL( );
        
        /**
        * Wrapper function that calls GetSubscriberId of CTelephony class.
        */
        void TelApiWrapperGetSubscriberIdL( );
        
        /**
        * Wrapper function that calls GetNetworkRegistrationStatus of 
        * CTelephony class.
        */
        void TelApiWrapperGetNwRegistrationStatusL( );

        /**
        * Wrapper function that calls GetCallBarringStatus of 
        * CTelephony class.
        * @param CTelephony::TCallBarringCondition aBarCondition 
        * Barring Condition
        */
        void TelApiWrapperGetBarringStatusL( 
            CTelephony::TCallBarringCondition aBarCondition );
            
        /**
        * Wrapper function that calls GetCallWaitingStatus of 
        * CTelephony class.
        */
        void TelApiWrapperGetWaitingStatusL( );
        /**
        * Wrapper function that calls GetIdentityServiceStatus of
        * CTelephony class.
        */
        void TelApiWrapperGetIdentityStatusL( 
            CTelephony::TIdentityService aIdentityServiceType );
         
        /**
        * Loads error strings from resource files
        */                                                        
        void LoadErrorStringsFromResource( );

       /**
        * Finds the system wide error.
        * @param aStatus The system wide error.
        * @param aErrMsgBuffer The buffer containing the error message.
        */                                                        
        void GeneralErrorMessage(
            TInt aStatus, 
            TBuf16<KMaxErrMsgSize> *aErrMsgBuffer);

        /**
        * Starts the active scheduler wait after testing if it 
        * is already started
        */                                                        
        void StartWait( );
        
    protected:
        
        /**
        * From CActive, RunL.
        * Handles the active object’s request completion event
        */
        void RunL();
        
        /**
        * From CActive, DoCancel.
        * Implements cancellation of an outstanding request.
        */
        void DoCancel();

    private:
    
          /**
           * Command Table.
           */
           enum TETelApiCmd 
            {
            /** 
            Get Phone Identity.
            */
            EGetPhoneId = 0,
            /** 
            Get SubscriberId.
            */
            EGetSubscriberId,
            /** 
            Get Network registration status.
            */
            ENwRegistrationStatus,
            /** 
            Get Call barring status.
            */
            EGetBarringStatus,
            /** 
            Get Call waiting status.
            */
            EGetWaitingStatus,
            /** 
            Get identity status.
            */
            EGetIdentityStatus,
            /** 
            Async commands.
            */
			EAsyncEvent,
            /** 
            Maximum number of commands.
            */
            EMaxCommands,
            /**
            Command None
            */
            ECommandNone
            };
            
    private:
      
        //Instance of command table.    
        TETelApiCmd iTelApiCmd; 

        // Call barring condition.
        CTelephony::TCallBarringCondition iBarringCondition; 

        // Phone ID services.
        CTelephony::TIdentityService iIdentityServiceType; 
        
        //Instance of scheduler wait class
        CActiveSchedulerWait iWait; 

       	//Notification event
       	CTelephony::TNotificationEvent iEvent;
       	
        //Reference to a class implementing MIsvTelInfoAppTelephonyNotify 
        //interface.
        MIsvTelInfoAppTelephonyNotify& iNotify;                                                  
        
        //Contains error strings loaded from resource
        TBuf16<KMaxErrMsgSize> iErrStrNotFound;
        TBuf16<KMaxErrMsgSize> iErrStrGeneral;
        TBuf16<KMaxErrMsgSize> iErrStrCancel;
        TBuf16<KMaxErrMsgSize> iErrStrNoMemory;
        TBuf16<KMaxErrMsgSize> iErrStrNotSupported;
        TBuf16<KMaxErrMsgSize> iErrStrArgument;
        TBuf16<KMaxErrMsgSize> iErrStrTotalLossOfPrecision;
        TBuf16<KMaxErrMsgSize> iErrStrBadHandle;
        TBuf16<KMaxErrMsgSize> iErrStrOverflow;
        TBuf16<KMaxErrMsgSize> iErrStrUnderflow;
        TBuf16<KMaxErrMsgSize> iErrStrAlreadyExists;
        TBuf16<KMaxErrMsgSize> iErrStrInUse;
        TBuf16<KMaxErrMsgSize> iErrStrServerTerminated;
        TBuf16<KMaxErrMsgSize> iErrStrServerBusy;
        TBuf16<KMaxErrMsgSize> iErrStrUnknown;
        TBuf16<KMaxErrMsgSize> iErrStrCorrupt;
        TBuf16<KMaxErrMsgSize> iErrStrAccessDenied;
        TBuf16<KMaxErrMsgSize> iErrStrCommsLineFail;
        TBuf16<KMaxErrMsgSize> iErrStrTimedOut;
        TBuf16<KMaxErrMsgSize> iErrStrCouldNotConnect;
        TBuf16<KMaxErrMsgSize> iErrStrCouldNotDisconnect;
        TBuf16<KMaxErrMsgSize> iErrStrHardwareNotAvailable;
        TBuf16<KMaxErrMsgSize> iErrStrSessionClosed;
        TBuf16<KMaxErrMsgSize> iErrStrPermissionDenied;
        TBuf16<KMaxErrMsgSize> iErrStrDefault;
        TBuf16<KMaxErrMsgSize> iEmuApiNotSupported;

        //Contains mobile phone identity.
        CTelephony::TPhoneIdV1 iPhoneId; 

    public:

        //Pointer to CTelephony class 
        //which contains the ETel ISV 3rd party APIs
        CTelephony* iTel;      

	private:
        CTelephony::TPhoneIdV1Pckg iPhoneIdPckg;
            
        CTelephony::TSubscriberIdV1 iSubscriberId; 
        CTelephony::TSubscriberIdV1Pckg iSubscriberIdPckg;
        
        CTelephony::TNetworkRegistrationV1 iNwRegistrationStatus;
        CTelephony::TNetworkRegistrationV1Pckg iNwRegistrationStatusPckg;
        
        CTelephony::TCallBarringSupplServicesV1 iCallBarringServices;
        CTelephony::TCallBarringSupplServicesV1Pckg iCallBarringServicesPckg;
        
        CTelephony::TCallWaitingSupplServicesV1 iCallWaitingServices;
        CTelephony::TCallWaitingSupplServicesV1Pckg iCallWaitingServicesPckg;
        
        CTelephony::TIdentityServiceV1 iIdentityService;
        CTelephony::TIdentityServiceV1Pckg iIdentityServicePckg;
    };

#endif // ISVTELINFOAPP_TELEPHONY_H

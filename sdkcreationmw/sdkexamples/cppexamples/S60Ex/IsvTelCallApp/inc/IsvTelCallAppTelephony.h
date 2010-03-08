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


#ifndef ISVTELCALLAPP_TELEPHONY_H
#define ISVTELCALLAPP_TELEPHONY_H

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
class MIsvTelCallAppTelephonyNotify;
class CTelephony;

// CLASS DEFINITIONS
/**
* CIsvTelCallAppTelephony: This class derives from CActive and wraps the 
* CTelephony class. Provides a command table based execution for 
* telephony commands with command specific parsing capabilities.
*/
class CIsvTelCallAppTelephony : public CActive
    {
    public:
        /**
        * Two-phased constructor.
        * @param aNotify Reference to an object implementing the
        * MIsvTelCallAppTelephonyNotify interface
        */
        static CIsvTelCallAppTelephony* NewL( MIsvTelCallAppTelephonyNotify& 
            aNotify );
        
        /**
        * Destructor.
        */
        ~CIsvTelCallAppTelephony();

        /**
        * Set the pointer to CTelephony class object
        * @param aTel Pointer to CTelephony class object
        */
        void SetTelephonyPtr( CTelephony* aTel );
        
        /**
        * Get the pointer to CTelephony class object
        */
        CTelephony* GetTelephonyPtr();
        /**
        * Create a CTelephony class object
        */
        void CreateTelephonyPtrL();

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
        
    public:
 
         /**
        * Default constructor.
        * @param aNotify reference to the object that implements the 
        * MIsvTelCallAppTelephonyNotify interface.
        */
        CIsvTelCallAppTelephony( MIsvTelCallAppTelephonyNotify& aNotify );
        
        /**
        * Second-phase constructor.
        */
        void ConstructL();
        
        /**
        * Register for notification change
        */
        void RegisterForAsyncNotification( TInt );

        //Wrapper functions for individual commands
        /**
        * Wrapper function that calls DialNewCall of CTelephony class.
        * @param CTelephony::TCallId aCallId either Call1 or Call2.
        * @param aTelNumber Called party's mobile number.
        */
        void TelApiWrapperMakeNewCallL( CTelephony::TCallId aCallId, 
            TBuf<CTelephony::KMaxTelNumberSize> aTelNumber );
   
        /**
        * Wrapper function that calls GetCallStatus of CTelephony class.
        * @param CTelephony::TCallId aCallId either Call1 or Call2.
        */
        void TelApiWrapperGetCallStatusL( CTelephony::TCallId aCallId );

        /**
        * Wrapper function that calls Hangup of CTelephony class.
        * @param CTelephony::TCallId aCallId either Call1 or Call2.
        */
        void TelApiWrapperHangupL( CTelephony::TCallId aCallId );
        
        /**
        * Wrapper function that calls Hold of CTelephony class.
        * @param CTelephony::TCallId aCallId either Call1 or Call2.
        */
        void TelApiWrapperHoldL( CTelephony::TCallId aCallId );
        /**
        * Wrapper function that calls Resume of CTelephony class.
        * @param CTelephony::TCallId aCallId either Call1 or Call2.
        */
        void TelApiWrapperResumeL( CTelephony::TCallId aCallId );
        
        /**
        * Wrapper function that calls Swap of CTelephony class.
        */
        void TelApiWrapperSwapL( );
        
        /**
        * Wrapper function that calls GetLineStatus of CTelephony class
        */
        void TelApiWrapperGetVoiceLineStatusL();
        
        /**
        * Wrapper function that calls GetLineStatus of CTelephony class
        */
        void TelApiWrapperGetDataLineStatusL();
        
        /**
        * Wrapper function that calls GetLineStatus of CTelephony class
        */
        void TelApiWrapperGetFaxLineStatusL();
        
        /**
        * Wrapper function that calls SendDTMFTones of CTelephony class.
        * @param CTelephony::TCallId aCallId either Call1 or Call2.
        * @param aDtmfSeq DTMF sequence to be sent.
        */
        void TelApiWrapperSendDTMFL(CTelephony::TCallId aCallId,
            TBuf<KMaxDTMFLength> aDtmfSeq);
        
       /**
        * Answers Incoming Call
        */
        void TelApiWrapperAnswerInCallL();
        
        /**
        * Loads error strings from resource files
        */                                                        
        void LoadErrorStringsFromResource( );
        
   public:
       /**
        * Finds the system wide error.
        * @param aStatus The system wide error.
        * @param aErrMsgBuffer The buffer containing the error message.
        */                                                        
        void GeneralErrorMessage(
            TInt aStatus, 
            TBuf16<KMaxErrMsgSize>* aErrMsgBuffer);

        /**
        * Starts the active scheduler wait after testing if it is already
        * started
        */                                                        
        void StartWait( );

    private:
    
            /**
           * Command Table.
           */
           enum TETelApiCmd 
            {
            /** 
            Dial new call.
            */
            EMakeNewCall = 0,
            /** 
            Terminate call.
            */
            EHangup,
            /** 
            Place call on hold.
            */
            EHold,
            /** 
            Moves call from Hold to Active state. 
            */
            EResume,
            /** 
            Moves the call under to hold to active and 
            * the call that is active to hold.
            */
            ESwap,
            /** 
            Get Call Status.
            */
            EGetCallStatus,
            /** 
            Get Voice line status.
            */
            EGetVoiceLineStatus,
            /** 
            Get Data line status.
            */
            EGetDataLineStatus,
            /** 
            Get Fax line status.
            */
            EGetFaxLineStatus,
            /** 
            Send DTMF.
            */
            ESendDTMF,
            /** 
            Answer Incoming Call.
            */
            EAnswerIncomingCall,
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

        //Call identifier.
        CTelephony::TCallId iCallId; 
        
        //Instance of scheduler wait class
        CActiveSchedulerWait iWait; 

       	//Notification event
       	CTelephony::TNotificationEvent iEvent;
       	
        //Reference to a class implementing MIsvTelCallAppTelephonyNotify 
        //interface.
        MIsvTelCallAppTelephonyNotify& iNotify;                                                  
        
        //Contains telephone number
        TBuf<CTelephony::KMaxTelNumberSize> iTelNum; 

        //Contains command languauge arguments
        TBuf8<KMaxCmdSize> iCallId1Str;
        TBuf8<KMaxCmdSize> iCallId2Str;

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

        //A typedef'd packaged TPhoneIdV1 for passing 
        //through a generic API method 
        CTelephony::TCallStatusV1 iCallStatus;
        CTelephony::TCallStatusV1Pckg iCallStatusPckg;
    };

#endif // ISVTELCALLAPP_TELEPHONY_H
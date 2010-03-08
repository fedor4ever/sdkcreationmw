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


#ifndef ISVTELCALLAPP_TELEPHONY_NOTIFY_H
#define ISVTELCALLAPP_TELEPHONY_NOTIFY_H

//INCLUDES
#include <etel3rdparty.h>

//CLASS DEFINITIONS
/**
* MIsvTelCallAppTelephonyNotify: This class specifies an interface that should
* be implemented by a class if it wants to get notified on completion of async
* telephony commands
*/
class MIsvTelCallAppTelephonyNotify
{
    public:
       /**
        * Pure virtual functions 
        */
        
       /** 
        * Gets invoked once command completion notification from DialNewCall 
        * ETel API is received.
        * @param aStatus, specifies SUCCESS or Error code
        * @param aCallId, refrence to Call ID. 
        * @param aTelNum, refrence to Tel Number.
        * @param aErrMsg, contains the error message returned by ETel API        
        */
        virtual void MakeNewCallApiCompleteL( TInt aStatus,
            const CTelephony::TCallId aCallId,
            CTelephony::TTelNumber &aTelNum,
            TDesC16& aErrMsg ) = 0;
        
       /** 
        * Gets invoked once command completion notification from HangUp 
        * ETel API is received.
        * @param aStatus, specifies SUCCESS or Error code
        * @param reference to Call ID. 
        * @param aErrMsg, contains the error message returned by ETel API        
        */
        virtual void HangupCompleteL( TInt aStatus, TDesC16& aErrMsg ) = 0;
        
       /** 
        * Gets invoked once command completion notification from Hold 
        * ETel API is received.
        * @param aStatus, specifies SUCCESS or Error code
        * @param aCallId, refrence to Call ID. 
        * @param aErrMsg, contains the error message returned by ETel API        
        */
        virtual void HoldCompleteL(TInt aStatus, TDesC16& aErrMsg ) = 0;
        
       /** 
        * Gets invoked once command completion notification from Resume 
        * ETel API is received.
        * @param aStatus, specifies SUCCESS or Error code
        * @param aCallId, refrence to Call ID. 
        * @param aErrMsg, contains the error message returned by ETel API        
        */
        virtual void ResumeCompleteL( TInt aStatus, TDesC16& aErrMsg ) = 0;
        
       /** 
        * Gets invoked once command completion notification from Swap 
        * ETel API is received.
        * @param aStatus, specifies SUCCESS or Error code
        * @param aErrMsg, contains the error message returned by ETel API        
        */
        virtual void SwapCompleteL( TInt aStatus, TDesC16& aErrMsg) = 0;
        
        
       /** 
        * Gets invoked once command completion notification from GetCallStatus 
        * ETel API is received.
        * @param aStatus, specifies SUCCESS or Error code
        * @param aCallId, refrence to Call ID. 
        * @param aCallStatus, contains status
        * @param aErrMsg, contains the error message returned by ETel API        
        */
        virtual void GetCallStatusCompleteL( TInt aStatus,
            const CTelephony::TCallId aCallId,
            CTelephony::TCallStatusV1& aCallStatus,
            TDesC16& aErrMsg ) = 0;

       /** 
        * Gets invoked once command completion notification from 
        * GetLineStatus ETel API is received.
        * @param aStatus, specifies SUCCESS or Error code
        * @param aErrMsg, contains the error message returned by ETel API        
        * @param aCallStatus, contains status.
        */
        virtual void GetVoiceLineStatusCompleteL( 
            TInt aStatus, 
            TDesC16& aErrMsg,
            CTelephony::TCallStatusV1& aCallStatus ) = 0;
                                        
       /** 
        * Gets invoked once command completion notification from 
        * GetLineStatus ETel API is received.
        * @param aStatus, specifies SUCCESS or Error code
        * @param aErrMsg, contains the error message returned by ETel API        
        * @param aCallStatus, contains status.
        */
        virtual void GetDataLineStatusCompleteL( 
            TInt aStatus, 
            TDesC16& aErrMsg,
            CTelephony::TCallStatusV1& aCallStatus ) = 0;
        
       /** 
        * Gets invoked once command completion notification from 
        * GetLineStatus ETel API is received.
        * @param aStatus, specifies SUCCESS or Error code
        * @param aErrMsg, contains the error message returned by ETel API        
        * @param aCallStatus, contains status.
        */
        virtual void GetFaxLineStatusCompleteL( 
            TInt aStatus, 
            TDesC16& aErrMsg,
            CTelephony::TCallStatusV1& aCallStatus ) = 0;
                                        
       /** 
        * Gets invoked once command completion notification from 
        * SendDTMFTones ETel API is received.
        * @param aStatus, specifies SUCCESS or Error code
        * @param aErrMsg, contains the error message returned by ETel API        
        */
        virtual void SendDTMFApiCompleteL( TInt aStatus, 
            TDesC16& aErrMsg ) = 0;   
        
       /** 
        * Gets invoked once command completion notification from 
        * AnswerIncomingCallApiCompleteL ETel ISV API is received.
        * @param aStatus, specifies SUCCESS or Error Code
        * @param aErrMsg, contains the error message returned by ETel ISV API
        */
        virtual void AnswerIncomingCallApiCompleteL( TInt aStatus, 
            const CTelephony::TCallId aCallId,
            TDesC16& aErrMsg ) = 0;  
};

#endif // ISVTELCALLAPP_TELEPHONY_NOTIFY_H

//End of file
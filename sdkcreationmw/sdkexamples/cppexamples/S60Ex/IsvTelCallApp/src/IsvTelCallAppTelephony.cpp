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
#include <e32std.h>
#include <avkon.hrh>
#include <aknnotewrappers.h> 
#include <f32fsys.h>
#include <IsvTelCallApp.rsg>
#include "IsvTelCallAppTelephonyNotify.h"
#include "IsvTelCallAppTelephony.h"

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CIsvTelCallAppTelephony* CIsvTelCallAppTelephony::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CIsvTelCallAppTelephony* CIsvTelCallAppTelephony::NewL
    ( MIsvTelCallAppTelephonyNotify& aNotify )
    {
    CIsvTelCallAppTelephony* selfWrapperClass = 
        new ( ELeave ) CIsvTelCallAppTelephony( aNotify );
    CleanupStack::PushL( selfWrapperClass );
    selfWrapperClass->ConstructL();
    CleanupStack::Pop();
    return selfWrapperClass;
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppTelephony::~CIsvTelCallAppTelephony()
// Destructor.
// ----------------------------------------------------------------------------
//
CIsvTelCallAppTelephony::~CIsvTelCallAppTelephony()
    {
    Cancel();
    }

// ----------------------------------------------------------------------------
// void CIsvTelCallAppTelephony::RunL()
// Handles the active object’s request completion event.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppTelephony::RunL()
    {
    if ( (iTelApiCmd == EAsyncEvent ) || 
       ( iTelApiCmd == EAnswerIncomingCall))
        {
        }
    else
        {
	    iWait.AsyncStop();
        }
        
    TBuf16<KMaxErrMsgSize> errMsgBuffer;
    
    //Find the type of error message and store in errMsgBuffer.
    GeneralErrorMessage( iStatus.Int(), &errMsgBuffer );
    
    switch( iTelApiCmd )
        {
        case EMakeNewCall :
            {
            iNotify.MakeNewCallApiCompleteL( iStatus.Int(), iCallId, 
               iTelNum, errMsgBuffer );
            }
            break;            
        case EHangup :
            {
            iNotify.HangupCompleteL( iStatus.Int(), errMsgBuffer );
            }
            break;            
        case EHold :
            {
            iNotify.HoldCompleteL( iStatus.Int(), errMsgBuffer );
            }
            break;
            
        case EResume :
            {
            iNotify.ResumeCompleteL( iStatus.Int(), errMsgBuffer );
            }
            break;
            
        case ESwap :
            {
            iNotify.SwapCompleteL( iStatus.Int(), errMsgBuffer );
            }
            break;

        case ESendDTMF :
            {
            iNotify.SendDTMFApiCompleteL( iStatus.Int(), errMsgBuffer );
            }
            break;
            
       case EAnswerIncomingCall :
            {
            iNotify.AnswerIncomingCallApiCompleteL( iStatus.Int(),
                iCallId,
                errMsgBuffer );

		    if ( iStatus.Int() != KErrNone )
		        {
		        }
		    else
		        {
		        iTelApiCmd = EAnswerIncomingCall;
    		    iTel->AnswerIncomingCall( iStatus, 
    		        iCallId, 
    		        CTelephony::EVoiceLine );
    		    SetActive();                
		        }
            }
            break;
            
		case EAsyncEvent :
		    {
		    switch (iEvent)
		        {
		        case CTelephony::EOwnedCall1StatusChange :
		            {
      		        iCallId = CTelephony::EISVCall1;
                    iNotify.GetCallStatusCompleteL(iStatus.Int(), 
                       iCallId, 
                       iCallStatus,
                       errMsgBuffer);
                       
                    if ( iStatus.Int() != KErrNone)   
                        {
                        }
                    else
                        {
                        iEvent = CTelephony::EOwnedCall1StatusChange;
        		        iTel->NotifyChange(iStatus, iEvent, iCallStatusPckg);
		        	    SetActive();  
                        }
		            }
		            break;
		        case CTelephony::EOwnedCall2StatusChange :
		            {
   		            iCallId = CTelephony::EISVCall2;
                    iNotify.GetCallStatusCompleteL( iStatus.Int(), iCallId, 
                        iCallStatus,
                        errMsgBuffer );
		            if ( iStatus.Int() != KErrNone )
		                {
		                }
		            else
		                {
			            iEvent = CTelephony::EOwnedCall2StatusChange;
        		        iTel->NotifyChange(iStatus, iEvent, iCallStatusPckg);
		        	    SetActive();                       
		                }
		            }
		            break;
		        
		        case CTelephony::EVoiceLineStatusChange :
		            {
                    iNotify.GetVoiceLineStatusCompleteL( iStatus.Int(), 
                       errMsgBuffer,
                       iCallStatus );
                    if ( iStatus.Int() != KErrNone )    
                        {
                        }
                    else
                        {
				        iEvent = CTelephony::EVoiceLineStatusChange;
			            iTel->NotifyChange(iStatus, iEvent, iCallStatusPckg);
				        SetActive();
                        }
		            }
		            break;
		        default:
                   {
                   _LIT(inforMsg, "Default Notify Complete");
                   CAknErrorNote* errorNote = new (ELeave) CAknErrorNote;
                   errorNote->ExecuteLD(inforMsg);
		           }
		           break;
		        }
		    }
		    break;
		    
        default :
            break;
        }
    return;
    }

// ----------------------------------------------------------------------------
// void CIsvTelCallAppTelephony::DoCancel()
// Implements cancellation of an outstanding request.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppTelephony::DoCancel()
    {
    //Based on the command, invoke the appropriate CancelAsync APIs
    switch( iTelApiCmd )
        {
        case EMakeNewCall :
            {
            iTel->CancelAsync( CTelephony::EDialNewCallCancel );
            }
            break;

        case EHangup :
            {
            iTel->CancelAsync( CTelephony::EHangupCancel );
            }    
            break;

        case EHold :
            {
            iTel->CancelAsync( CTelephony::EHoldCancel );
            }    
            break;

        case EResume :
            {
            iTel->CancelAsync( CTelephony::EResumeCancel );
            }    
            break;

        case EAnswerIncomingCall :
            {
            iTel->CancelAsync( CTelephony::EAnswerIncomingCallCancel );
            }
            break;

        case EAsyncEvent:
            {
        	switch( iEvent )
         	    {
		        case CTelephony::EOwnedCall1StatusChange :
		            {
		            iTel->CancelAsync( 
		                CTelephony::EOwnedCall1StatusChangeCancel );
		            }
		            break;
		        case CTelephony::EOwnedCall2StatusChange :
		            {
		            iTel->CancelAsync( 
		                 CTelephony::EOwnedCall2StatusChangeCancel );
		            }
		            break;
         	    
		        case CTelephony::EVoiceLineStatusChange :
		            {
		            iTel->CancelAsync( 
		                CTelephony::EVoiceLineStatusChangeCancel );
		            }
		            break;
        		default :
        			break;
        		}
            }
        default :
            break;
        }
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppTelephony::CIsvTelCallAppTelephony()
// Default constructor.
// ----------------------------------------------------------------------------
//
CIsvTelCallAppTelephony::CIsvTelCallAppTelephony( 
    MIsvTelCallAppTelephonyNotify& aNotify )
    :CActive( EPriorityStandard ),
    iNotify( aNotify ), 
    iCallStatusPckg( iCallStatus )
    { 
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// void CIsvTelCallAppTelephony::ConstructL()
// It creates a CTelephony object and initializes a locally 
// contained command table.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppTelephony::ConstructL()
    {

    //----------------------------------------------------------------------
    // Load Error strings from resource file
    //----------------------------------------------------------------------
    LoadErrorStringsFromResource();

    //Load call id strings from resource
    TBuf16<KMaxCmdSize> callIdStr;
    CCoeEnv::Static()->ReadResource(callIdStr, 
        R_ISVTELCALLAPP_TELSTR_CALLID1);
    iCallId1Str.Copy(callIdStr);

    CCoeEnv::Static()->ReadResource(callIdStr, 
        R_ISVTELCALLAPP_TELSTR_CALLID2);
    iCallId2Str.Copy(callIdStr);

    }

// ----------------------------------------------------------------------------
// void CIsvTelCallAppTelephony::RegisterForAsyncNotification()
// Wrapper function that register funtions to notify change.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppTelephony::RegisterForAsyncNotification( TInt aAsyncEvent )
    {
    if (IsActive())
    	{
    	return;
    	}
	iTelApiCmd = EAsyncEvent;    
	switch(aAsyncEvent)
	    {
		case CTelephony::EOwnedCall1StatusChange :
		    {
			iEvent = CTelephony::EOwnedCall1StatusChange;
            iCallStatus.iStatus = CTelephony::EStatusUnknown;
		    }
		    break;
		case CTelephony::EOwnedCall2StatusChange :
		    {
			iEvent = CTelephony::EOwnedCall2StatusChange;					
            iCallStatus.iStatus = CTelephony::EStatusUnknown;
		    }
		    break;
		case CTelephony::EVoiceLineStatusChange :
		    {
			iEvent = CTelephony::EVoiceLineStatusChange;
		    }
		    break;
		default :
		    break;	
	    }
		iTel->NotifyChange(iStatus, iEvent, iCallStatusPckg);
    	SetActive();
    }

// ----------------------------------------------------------------------------
// void CIsvTelCallAppTelephony::SetTelephonyPtr()
// Called by other classes to set the iTel Ptr
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppTelephony::SetTelephonyPtr(CTelephony* aTel)
    {
    iTel = aTel;
    }

// ----------------------------------------------------------------------------
// CTelephony* CIsvTelCallAppTelephony::GetTelephonyPtr()
// Called by other classes to get the iTel Ptr
// ----------------------------------------------------------------------------
//
CTelephony* CIsvTelCallAppTelephony::GetTelephonyPtr()
    {
    return(iTel);
    }

// ----------------------------------------------------------------------------
// void CIsvTelCallAppTelephony::CreateTelephonyPtrL()
// Called by other classes to get the iTel Ptr
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppTelephony::CreateTelephonyPtrL()
    {
   //Creating Telephony object. 
    iTel = CTelephony::NewL();
    }

// ----------------------------------------------------------------------------
// void CIsvTelCallAppTelephony::TelApiWrapperMakeNewCallL( )
// Wrapper function that calls DialNewCall of CTelephony class.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppTelephony::TelApiWrapperMakeNewCallL( 
    CTelephony::TCallId aCallId,
    TBuf<CTelephony::KMaxTelNumberSize> aTelNumber)
    {
    if ( IsActive() )
    	{
    	return;
    	}
    
    iTelApiCmd = EMakeNewCall;
    CTelephony::TCallParamsV1 callParams;
    CTelephony::TCallParamsV1Pckg callParamPckg( callParams );
    iCallId = aCallId;
    iTelNum = aTelNumber;
    
#ifdef __WINS__
    CAknErrorNote* errorNoteMsg = new (ELeave) CAknErrorNote;
    errorNoteMsg->ExecuteLD(iEmuApiNotSupported);
#else
    iTel->DialNewCall( iStatus, callParamPckg, iTelNum, 
        iCallId, CTelephony::EVoiceLine );
    SetActive();
    StartWait();
#endif
    }

// ----------------------------------------------------------------------------
// void CIsvTelCallAppTelephony::TelApiWrapperGetCallStatusL( )
// Wrapper function that calls GetCallStatus of CTelephony class.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppTelephony::TelApiWrapperGetCallStatusL( 
    CTelephony::TCallId aCallId )
    {
    if ( IsActive() )
    	{
    	return;
    	}
    
    if ( ( TInt )aCallId == KErrNotFound )
        {
        return;
        }
	iCallId = aCallId;
    iTelApiCmd = EGetCallStatus;
    
#ifdef __WINS__
    CAknErrorNote* errorNoteMsg = new (ELeave) CAknErrorNote;
    errorNoteMsg->ExecuteLD(iEmuApiNotSupported);
#else
    TInt retCode;
    iCallStatus.iStatus = CTelephony::EStatusUnknown;
    retCode = iTel->GetCallStatus( iCallId, iCallStatusPckg );

    TBuf16<KMaxErrMsgSize> errMsgBuffer;
    
    //Find the type of error message and store in errMsgBuffer.
    GeneralErrorMessage( retCode, &errMsgBuffer );

    iNotify.GetCallStatusCompleteL( retCode, iCallId, iCallStatus,
        errMsgBuffer);
#endif
    }

// ----------------------------------------------------------------------------
// void CIsvTelCallAppTelephony::TelApiWrapperHangupL( )
// Wrapper function that calls Hangup of CTelephony class.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppTelephony::TelApiWrapperHangupL( CTelephony::TCallId aCallId )
    {
    if ( IsActive() )
    	{
    	return;
    	}

    if ( ( TInt )aCallId == KErrNotFound )
        {
        return;
        }
	iCallId = aCallId;
    iTelApiCmd = EHangup;
    
#ifdef __WINS__
    CAknErrorNote* errorNoteMsg = new (ELeave) CAknErrorNote;
    errorNoteMsg->ExecuteLD(iEmuApiNotSupported);
#else
    iTel->Hangup( iStatus, iCallId );
    SetActive();
    StartWait();
#endif
    }

// ----------------------------------------------------------------------------
// void CIsvTelCallAppTelephony::TelApiWrapperHoldL( )
// Wrapper function that calls Hold of CTelephony class.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppTelephony::TelApiWrapperHoldL( CTelephony::TCallId aCallId )
    {
    if ( IsActive() )
    	{
    	return;
    	}
    
    if ( ( TInt )aCallId == KErrNotFound )
        {
        return;
        }
	iCallId = aCallId;
    iTelApiCmd = EHold;
    
#ifdef __WINS__
    CAknErrorNote* errorNoteMsg = new (ELeave) CAknErrorNote;
    errorNoteMsg->ExecuteLD(iEmuApiNotSupported);
#else
    iTel->Hold( iStatus, iCallId );
    SetActive();
    StartWait();
#endif
    }

// ----------------------------------------------------------------------------
// void CIsvTelCallAppTelephony::TelApiWrapperResumeL( )
// Wrapper function that calls Resume of CTelephony class.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppTelephony::TelApiWrapperResumeL( CTelephony::TCallId aCallId )
    {
    if ( IsActive() )
    	{
    	return;
    	}

    if ( ( TInt )aCallId == KErrNotFound )
        {
        return;
        }
	iCallId = aCallId;
    iTelApiCmd = EResume;
    
#ifdef __WINS__
    CAknErrorNote* errorNoteMsg = new (ELeave) CAknErrorNote;
    errorNoteMsg->ExecuteLD(iEmuApiNotSupported);
#else
    iTel->Resume( iStatus, iCallId );
    SetActive();
    StartWait();
#endif
    }

// ----------------------------------------------------------------------------
// void CIsvTelCallAppTelephony::TelApiWrapperSwapL( )
// Wrapper function that calls Swap of CTelephony class.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppTelephony::TelApiWrapperSwapL( )
    {
    if ( IsActive() )
    	{
    	return;
    	}

    //Set the call id
    iCallId = CTelephony::EISVCall1;
    CTelephony::TCallId swapCallId = CTelephony::EISVCall2;

    iTelApiCmd = ESwap;
    
#ifdef __WINS__
    CAknErrorNote* errorNoteMsg = new (ELeave) CAknErrorNote;
    errorNoteMsg->ExecuteLD(iEmuApiNotSupported);
#else
    iTel->Swap( iStatus, iCallId, swapCallId );
    SetActive();
    StartWait();
#endif
    }

// ----------------------------------------------------------------------------
// void CIsvTelCallAppTelephony::TelApiWrapperGetVoiceLineStatusL( TPtr8,TInt )
// Wrapper function that calls GetLineStatus of CTelephony class.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppTelephony::TelApiWrapperGetVoiceLineStatusL( )
    {
    if ( IsActive() )
    	{
    	return;
    	}
    
    iTelApiCmd = EGetVoiceLineStatus;

#ifdef __WINS__
    CAknErrorNote* errorNoteMsg = new (ELeave) CAknErrorNote;
    errorNoteMsg->ExecuteLD(iEmuApiNotSupported);
#else    
    //Call GetLineStatus with EVoiceLine to get the status of voice line.
    TInt aStatus = iTel->GetLineStatus(CTelephony::EVoiceLine, 
        iCallStatusPckg);
    TBuf16<KMaxErrMsgSize> errMsgBuffer;
    GeneralErrorMessage( aStatus, &errMsgBuffer );
    iNotify.GetVoiceLineStatusCompleteL( aStatus, errMsgBuffer, iCallStatus );
#endif        
    }

// ----------------------------------------------------------------------------
// void CIsvTelCallAppTelephony::TelApiWrapperGetDataLineStatusL( )
// Wrapper function that calls GetLineStatus of CTelephony class.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppTelephony::TelApiWrapperGetDataLineStatusL( ) 
    {
    if ( IsActive() )
    	{
    	return;
    	}
    
    iTelApiCmd = EGetDataLineStatus;
    
#ifdef __WINS__
    CAknErrorNote* errorNoteMsg = new (ELeave) CAknErrorNote;
    errorNoteMsg->ExecuteLD(iEmuApiNotSupported);
#else    
    //Call GetLineStatus with EDataLine to get the status of data line.
    TInt aStatus = iTel->GetLineStatus( 
        CTelephony::EDataLine, iCallStatusPckg );
    TBuf16<KMaxErrMsgSize> errMsgBuffer;
    GeneralErrorMessage( aStatus, &errMsgBuffer );
    iNotify.GetDataLineStatusCompleteL( aStatus, errMsgBuffer, iCallStatus );
#endif    
    }

// ----------------------------------------------------------------------------
// void CIsvTelCallAppTelephony::TelApiWrapperGetFaxLineStatusL( )
// Wrapper function that calls GetLineStatus of CTelephony class.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppTelephony::TelApiWrapperGetFaxLineStatusL( )
    {
    if ( IsActive() )
    	{
    	return;
    	}
    	
    iTelApiCmd = EGetFaxLineStatus;
    
#ifdef __WINS__
    CAknErrorNote* errorNoteMsg = new (ELeave) CAknErrorNote;
    errorNoteMsg->ExecuteLD(iEmuApiNotSupported);
#else    
    //Call GetLineStatus with EFaxLine to get the status of fax line.
    TInt aStatus = iTel->GetLineStatus(
        CTelephony::EFaxLine, iCallStatusPckg );
    
    TBuf16<KMaxErrMsgSize> errMsgBuffer;
    GeneralErrorMessage( aStatus, &errMsgBuffer );
    iNotify.GetFaxLineStatusCompleteL( aStatus, errMsgBuffer, iCallStatus );
#endif    
    }

// ----------------------------------------------------------------------------
// void CIsvTelCallAppTelephony::TelApiWrapperSendDTMFL( )
// Wrapper function that calls SendDTMFTones of CTelephony class.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppTelephony::TelApiWrapperSendDTMFL(
    CTelephony::TCallId aCallId, 
    TBuf<KMaxDTMFLength> aDtmfSeq )
    {
    if ( IsActive() )
    	{
    	return;
    	}

	CTelephony::TCallId callId = aCallId;
	TBuf<KMaxDTMFLength> dtmfSeq = aDtmfSeq;
	
    iTelApiCmd = ESendDTMF;

#ifdef __WINS__
    CAknErrorNote* errorNoteMsg = new (ELeave) CAknErrorNote;
    errorNoteMsg->ExecuteLD(iEmuApiNotSupported);
#else   
    iTel->GetCallStatus( callId, iCallStatusPckg );
    
    //If the call status, for which DTMF tones is to be passed 
    //is not active, then return error.
    if ( iCallStatus.iStatus != CTelephony::EStatusConnected )
        {
        TBuf16<KMaxErrMsgSize> KError;
        CCoeEnv::Static()->ReadResource(KError,
            R_ISVTELCALLAPP_TELSTR_CALLNOTACTIVE);
        iNotify.SendDTMFApiCompleteL( ( TInt )KErrNotFound, KError );
        return;
        }
    iTel->SendDTMFTones( iStatus, dtmfSeq );
    SetActive();
    StartWait();
#endif    
    }

// ----------------------------------------------------------------------------
// void CIsvTelCallAppTelephony::TelApiWrapperAnswerInCall( )
// Wrapper function that calls AnswerIncomingCall of CTelephony class.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppTelephony::TelApiWrapperAnswerInCallL()
    {
    iTelApiCmd = EAnswerIncomingCall;
#ifdef __WINS__
    CAknErrorNote* errorNote = new (ELeave) CAknErrorNote;
    errorNote->ExecuteLD(iEmuApiNotSupported);
#else    
    iTel->AnswerIncomingCall( iStatus, iCallId, CTelephony::EVoiceLine );
    SetActive();
#endif    
    }

// ----------------------------------------------------------------------------
// void CIsvTelCallAppTelephony::StartWait()
// Checks for the active scheduler wait class's start state 
// and then starts the wait
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppTelephony::StartWait( )
    {
    if ( iWait.IsStarted() == (TInt)ETrue )
        {
        return;
        }
    else
        {
        iWait.Start();
        }
    return;
    }
    
// ----------------------------------------------------------------------------
// void CIsvTelCallAppTelephony::LoadErrorStringsFromResource()
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppTelephony::LoadErrorStringsFromResource()
    {
    CCoeEnv::Static()->ReadResource(iErrStrNotFound,
        R_ISVTELCALLAPP_TELSTR_ERRNOTFOUND);
    CCoeEnv::Static()->ReadResource(iErrStrGeneral,
        R_ISVTELCALLAPP_TELSTR_ERRGENERAL);
    CCoeEnv::Static()->ReadResource(iErrStrCancel,
        R_ISVTELCALLAPP_TELSTR_ERRCANCEL);
    CCoeEnv::Static()->ReadResource(iErrStrNoMemory,
        R_ISVTELCALLAPP_TELSTR_ERRNOMEMORY);
    CCoeEnv::Static()->ReadResource(iErrStrNotSupported,
        R_ISVTELCALLAPP_TELSTR_ERRNOTSUPPORTED);
    CCoeEnv::Static()->ReadResource(iErrStrArgument,
        R_ISVTELCALLAPP_TELSTR_ERRARGUMENT);
    CCoeEnv::Static()->ReadResource(iErrStrTotalLossOfPrecision,
        R_ISVTELCALLAPP_TELSTR_ERRTOTALLOSSOFPRECISION);
    CCoeEnv::Static()->ReadResource(iErrStrBadHandle,
        R_ISVTELCALLAPP_TELSTR_ERRBADHANDLE);
    CCoeEnv::Static()->ReadResource(iErrStrOverflow,
        R_ISVTELCALLAPP_TELSTR_ERROVERFLOW);
    CCoeEnv::Static()->ReadResource(iErrStrUnderflow,
        R_ISVTELCALLAPP_TELSTR_ERRUNDERFLOW);
    CCoeEnv::Static()->ReadResource(iErrStrAlreadyExists,
        R_ISVTELCALLAPP_TELSTR_ERRALREADYEXISTS);
    CCoeEnv::Static()->ReadResource(iErrStrInUse,
        R_ISVTELCALLAPP_TELSTR_ERRINUSE);
    CCoeEnv::Static()->ReadResource(iErrStrServerTerminated,
        R_ISVTELCALLAPP_TELSTR_ERRSERVERTERMINATED);
    CCoeEnv::Static()->ReadResource(iErrStrServerBusy,
        R_ISVTELCALLAPP_TELSTR_ERRSERVERBUSY);
    CCoeEnv::Static()->ReadResource(iErrStrUnknown,
        R_ISVTELCALLAPP_TELSTR_ERRUNKNOWN);
    CCoeEnv::Static()->ReadResource(iErrStrCorrupt,
        R_ISVTELCALLAPP_TELSTR_ERRCORRUPT);
    CCoeEnv::Static()->ReadResource(iErrStrAccessDenied,
        R_ISVTELCALLAPP_TELSTR_ERRACCESSDENIED);
    CCoeEnv::Static()->ReadResource(iErrStrCommsLineFail,
        R_ISVTELCALLAPP_TELSTR_ERRCOMMSLINEFAIL);
    CCoeEnv::Static()->ReadResource(iErrStrTimedOut,
        R_ISVTELCALLAPP_TELSTR_ERRTIMEDOUT);
    CCoeEnv::Static()->ReadResource(iErrStrCouldNotConnect,
        R_ISVTELCALLAPP_TELSTR_ERRCOULDNOTCONNECT);
    CCoeEnv::Static()->ReadResource(iErrStrCouldNotDisconnect,
        R_ISVTELCALLAPP_TELSTR_ERRCOULDNOTDISCONNECT);
    CCoeEnv::Static()->ReadResource(iErrStrHardwareNotAvailable,
        R_ISVTELCALLAPP_TELSTR_ERRHARDWARENOTAVAILABLE);
    CCoeEnv::Static()->ReadResource(iErrStrSessionClosed,
        R_ISVTELCALLAPP_TELSTR_ERRSESSIONCLOSED);
    CCoeEnv::Static()->ReadResource(iErrStrPermissionDenied,
        R_ISVTELCALLAPP_TELSTR_ERRPERMISSIONDENIED);
    CCoeEnv::Static()->ReadResource(iErrStrDefault,
        R_ISVTELCALLAPP_TELSTR_ERRORDEFAULT);
	CCoeEnv::Static()->ReadResource(iEmuApiNotSupported,
	    R_ISVTELCALLAPP_ERR_EMUAPINOTSUPPORTED);        
    }

// ----------------------------------------------------------------------------
// void CIsvTelCallAppTelephony::GeneralErrorMessage()
// Finds the system wide error. On return aErrMsgBuffer contains the 
// error message.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppTelephony::GeneralErrorMessage( TInt aStatus,
    TBuf16<KMaxErrMsgSize> *aErrMsgBuffer )
    {
    switch( aStatus )
        {
        case KErrNotFound:
            {
            (*aErrMsgBuffer).Copy(iErrStrNotFound);
            }
            break;
        case KErrGeneral:
            {    
            (*aErrMsgBuffer).Copy(iErrStrGeneral);
            }
            break;
        case KErrCancel:
            {
            (*aErrMsgBuffer).Copy(iErrStrCancel);
            }
            break;
        case KErrNoMemory:
            {
            (*aErrMsgBuffer).Copy(iErrStrNoMemory);
            }
            break;
        case KErrNotSupported:
            {
            (*aErrMsgBuffer).Copy(iErrStrNotSupported);
            }
            break;
        case KErrArgument:
            {
            (*aErrMsgBuffer).Copy(iErrStrArgument);
            }
            break;
        case KErrTotalLossOfPrecision:
            {
            (*aErrMsgBuffer).Copy(iErrStrTotalLossOfPrecision);
            }
            break;
        case KErrBadHandle:
            {
            (*aErrMsgBuffer).Copy(iErrStrBadHandle);
            }
            break;
        case KErrOverflow:
            {
            (*aErrMsgBuffer).Copy(iErrStrOverflow);
            }
            break;
        case KErrUnderflow:
            {
            (*aErrMsgBuffer).Copy(iErrStrUnderflow);
            }
            break;
        case KErrAlreadyExists:
            {
            (*aErrMsgBuffer).Copy(iErrStrAlreadyExists);
            }
            break;
        case KErrInUse:
            {
            (*aErrMsgBuffer).Copy(iErrStrInUse);
            }
            break;
        case KErrServerTerminated:
            {
            (*aErrMsgBuffer).Copy(iErrStrServerTerminated);
            }
            break;
        case KErrServerBusy:
            {
            (*aErrMsgBuffer).Copy(iErrStrServerBusy);
            }
            break;
        case KErrUnknown:
            {
            (*aErrMsgBuffer).Copy(iErrStrUnknown);
            }
            break;
        case KErrCorrupt:
            {
            (*aErrMsgBuffer).Copy(iErrStrCorrupt);
            }
            break;
        case KErrAccessDenied:
            {
            (*aErrMsgBuffer).Copy(iErrStrAccessDenied);
            }
            break;
        case KErrCommsLineFail:
            {
            (*aErrMsgBuffer).Copy(iErrStrCommsLineFail);
            }
            break;
        case KErrTimedOut:
            {
            (*aErrMsgBuffer).Copy(iErrStrTimedOut);
            }
            break;
        case KErrCouldNotConnect:
            {
            (*aErrMsgBuffer).Copy(iErrStrCouldNotConnect);
            }
            break;
        case KErrCouldNotDisconnect:
            {
            (*aErrMsgBuffer).Copy(iErrStrCouldNotDisconnect);
            }
            break;
        case KErrHardwareNotAvailable:
            {
            (*aErrMsgBuffer).Copy(iErrStrHardwareNotAvailable);
            }
            break;
        case KErrSessionClosed:
            {
            (*aErrMsgBuffer).Copy(iErrStrSessionClosed);
            }
            break;
        case KErrPermissionDenied:
            {
            (*aErrMsgBuffer).Copy(iErrStrPermissionDenied);
            }
            break;
        default:
            {
            (*aErrMsgBuffer).Copy(iErrStrDefault);
            }
            break;
        }
    }

// End of File 


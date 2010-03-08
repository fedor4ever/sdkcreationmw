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
#include <IsvTelInfoApp.rsg>
#include "IsvTelInfoAppTelephonyNotify.h"
#include "IsvTelInfoAppTelephony.h"

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CIsvTelInfoAppTelephony* CIsvTelInfoAppTelephony::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CIsvTelInfoAppTelephony* CIsvTelInfoAppTelephony::NewL
    ( MIsvTelInfoAppTelephonyNotify& aNotify )
    {
    CIsvTelInfoAppTelephony* selfWrapperClass = 
        new ( ELeave ) CIsvTelInfoAppTelephony( aNotify );
    CleanupStack::PushL( selfWrapperClass );
    selfWrapperClass->ConstructL();
    CleanupStack::Pop();
    return selfWrapperClass;
    }

// ----------------------------------------------------------------------------
// CIsvTelInfoAppTelephony::~CIsvTelInfoAppTelephony()
// Destructor.
// ----------------------------------------------------------------------------
//
CIsvTelInfoAppTelephony::~CIsvTelInfoAppTelephony()
    {
    Cancel();
    }

// ----------------------------------------------------------------------------
// void CIsvTelInfoAppTelephony::RunL()
// Handles the active object’s request completion event.
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppTelephony::RunL()
    {
    if ( iTelApiCmd != EAsyncEvent ) 
        {
 	    iWait.AsyncStop();
        }

    TBuf16<KMaxErrMsgSize> errMsgBuffer;
    
    //Find the type of error message and store in errMsgBuffer.
 	GeneralErrorMessage( iStatus.Int(), &errMsgBuffer );
   
    //Based on the command, invoke the appropriate "command complete" routines
    //in the notify-object.
    switch( iTelApiCmd )
        {
        case EGetPhoneId :
            {
            iNotify.GetPhoneIdApiCompleteL( iStatus.Int(), iPhoneId, errMsgBuffer );
            }
            break;
        
        case EGetSubscriberId :
            {
            iNotify.GetSubscriberIdApiCompleteL( iStatus.Int(), 
                iSubscriberId, 
                errMsgBuffer );
            }
            break;

        case ENwRegistrationStatus :
            {
            iNotify.GetNwRegistrationStatusApiCompleteL( iStatus.Int(),
                iNwRegistrationStatus, errMsgBuffer );
            }
            break;

        case EGetBarringStatus :
            {
            iNotify.GetBarringStatusApiCompleteL( iStatus.Int(),
                iBarringCondition, 
                iCallBarringServices, 
                errMsgBuffer );
            }
            break;
        
       case EGetWaitingStatus :
            {
            iNotify.GetWaitingStatusApiCompleteL( iStatus.Int(), 
                iCallWaitingServices, 
                errMsgBuffer );
            }
            break;
                
       case EGetIdentityStatus :
            {
            iNotify.GetIdentityServiceStatusApiCompleteL( iStatus.Int(),
                iIdentityServiceType, 
                iIdentityService, 
                errMsgBuffer );
            }
            break;

      default :
            break;
        }
    return;
    }

// ----------------------------------------------------------------------------
// void CIsvTelInfoAppTelephony::DoCancel()
// Implements cancellation of an outstanding request.
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppTelephony::DoCancel()
    {
    //Based on the command, invoke the appropriate CancelAsync APIs
    switch( iTelApiCmd )
        {
        case EGetPhoneId : 
            {
            iTel->CancelAsync( CTelephony::EGetPhoneIdCancel );
            }    
            break;
            
        case EGetSubscriberId : 
            {
            iTel->CancelAsync( CTelephony::EGetSubscriberIdCancel );
            }    
            break;
            
        case EGetBarringStatus :
            {
            iTel->CancelAsync( CTelephony::EGetCallBarringStatusCancel );
            }
            break;
        
        case EGetWaitingStatus :
            {
            iTel->CancelAsync( CTelephony::EGetCallWaitingStatusCancel );
            }
            break;
        
        case EGetIdentityStatus :
            {
            iTel->CancelAsync( CTelephony::EGetIdentityServiceStatusCancel );
            }
            break;

        default :
            break;
        }
    }

// ----------------------------------------------------------------------------
// CIsvTelInfoAppTelephony::CIsvTelInfoAppTelephony()
// Default constructor.
// ----------------------------------------------------------------------------
//
CIsvTelInfoAppTelephony::CIsvTelInfoAppTelephony( 
    MIsvTelInfoAppTelephonyNotify& aNotify )
    :CActive( EPriorityStandard ),
    iNotify( aNotify ), 
    iPhoneIdPckg( iPhoneId ),
    iSubscriberIdPckg( iSubscriberId ),
    iNwRegistrationStatusPckg( iNwRegistrationStatus ),
    iCallBarringServicesPckg( iCallBarringServices ),
    iCallWaitingServicesPckg( iCallWaitingServices ),
    iIdentityServicePckg( iIdentityService )
    { 
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// void CIsvTelInfoAppTelephony::ConstructL()
// It creates a CTelephony object and initializes a locally 
// contained command table.
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppTelephony::ConstructL()
    {
    //----------------------------------------------------------------------
    // Load Error strings from resource file
    //----------------------------------------------------------------------
    LoadErrorStringsFromResource();
   
   //Creating Telephony object. 
#ifdef __WINS__

#else
    iTel = CTelephony::NewL();
#endif   
    }

// ----------------------------------------------------------------------------
// void CIsvTelInfoAppTelephony::TelApiWrapperGetPhoneId()
// Wrapper function that calls GetPhoneId of CTelephony class.
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppTelephony::TelApiWrapperGetPhoneIdL( )
    {
    if ( IsActive() )
    	{
    	return;
    	}
    
    iTelApiCmd = EGetPhoneId;
    
#ifdef __WINS__
    CAknErrorNote* errorNote = new (ELeave) CAknErrorNote;
    errorNote->ExecuteLD(iEmuApiNotSupported);
#else    
    iTel->GetPhoneId( iStatus, iPhoneIdPckg );
    SetActive();
    StartWait();
#endif    
    }

// ----------------------------------------------------------------------------
// void CIsvTelInfoAppTelephony::TelApiWrapperGetSubscriberId( )
// Wrapper function that calls GetSubscriberId of CTelephony class.
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppTelephony::TelApiWrapperGetSubscriberIdL( )
    {
    if ( IsActive() )
    	{
    	return;
    	}
    	
    iTelApiCmd = EGetSubscriberId;
    
#ifdef __WINS__
    CAknErrorNote* errorNote = new (ELeave) CAknErrorNote;
    errorNote->ExecuteLD(iEmuApiNotSupported);
#else    
    iTel->GetSubscriberId( iStatus, iSubscriberIdPckg );
    SetActive();
    StartWait();
#endif    
    }

// ----------------------------------------------------------------------------
// void CETelISVTestAppTelephony::TelApiWrapperGetBarringStatus( )
// Wrapper function that calls GetCallBarringStatus of CTelephony class.
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppTelephony::TelApiWrapperGetBarringStatusL( 
    CTelephony::TCallBarringCondition aBarCondition)
    {
    if ( IsActive() )
    	{
    	return;
    	}
    
    iTelApiCmd = EGetBarringStatus;
   
    //Find the barring condition for which the status is to be fetched.
    iBarringCondition = aBarCondition;
    if ( ( TInt )iBarringCondition == KErrNotFound )
        {
        return;
        }
#ifdef __WINS__
    CAknErrorNote* errorNote = new (ELeave) CAknErrorNote;
    errorNote->ExecuteLD(iEmuApiNotSupported);
#else    
    iTel->GetCallBarringStatus( iStatus, iBarringCondition, 
        iCallBarringServicesPckg );
    SetActive();
    StartWait();
#endif    
    }

// ----------------------------------------------------------------------------
// void CETelISVTestAppTelephony::TelApiWrapperGetWaitingStatus( )
// Wrapper function that calls GetCallWaitingStatus of CTelephony class.
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppTelephony::TelApiWrapperGetWaitingStatusL( )
    {
    if ( IsActive() )
    	{
    	return;
    	}
    
    iTelApiCmd = EGetWaitingStatus;
    
#ifdef __WINS__
    CAknErrorNote* errorNote = new (ELeave) CAknErrorNote;
    errorNote->ExecuteLD(iEmuApiNotSupported);
#else    
    iTel->GetCallWaitingStatus( iStatus, iCallWaitingServicesPckg );
    SetActive();
    StartWait();
#endif    
    }

// ----------------------------------------------------------------------------
// void CETelISVTestAppTelephony::TelApiWrapperGetNwRegistrationStatus()
// Wrapper function that calls GetNetworkRegistrationStatus of CTelephony class.
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppTelephony::TelApiWrapperGetNwRegistrationStatusL( )
    {
    if ( IsActive() )
    	{
    	return;
    	}
    
    iTelApiCmd = ENwRegistrationStatus;
    
#ifdef __WINS__
    CAknErrorNote* errorNote = new (ELeave) CAknErrorNote;
    errorNote->ExecuteLD(iEmuApiNotSupported);
#else    
    iTel->GetNetworkRegistrationStatus( iStatus, iNwRegistrationStatusPckg );
    SetActive();
    StartWait();
#endif    
    }
    
// ----------------------------------------------------------------------------
// void CETelISVTestAppTelephony::TelApiWrapperGetIdentityStatus( )
// Wrapper function that calls GetIdentityServiceStatus of CTelephony class.
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppTelephony::TelApiWrapperGetIdentityStatusL( 
    CTelephony::TIdentityService aIdentityServiceType )
    {
    if ( IsActive() )
    	{
    	return;
    	}
    
    iTelApiCmd = EGetIdentityStatus;
   
       //Find the identity service for which the status is to be fetched.
    iIdentityServiceType = aIdentityServiceType;
       
    if ( ( TInt )iIdentityServiceType == KErrNotFound )
        {
        return;
        }
#ifdef __WINS__
    CAknErrorNote* errorNote = new (ELeave) CAknErrorNote;
    errorNote->ExecuteLD(iEmuApiNotSupported);
#else    
    iTel->GetIdentityServiceStatus( iStatus, iIdentityServiceType, 
        iIdentityServicePckg );
    SetActive();
    StartWait();
#endif    
    }

// ----------------------------------------------------------------------------
// void CIsvTelInfoAppTelephony::StartWait()
// Checks for the active scheduler wait class's start state and then starts the
// wait
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppTelephony::StartWait( )
    {
    if ( iWait.IsStarted() != (TInt)ETrue )
        {
        iWait.Start();
        }
    }
    
// ----------------------------------------------------------------------------
// void CIsvTelInfoAppTelephony::LoadErrorStringsFromResource()
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppTelephony::LoadErrorStringsFromResource()
    {
    CCoeEnv::Static()->ReadResource(iErrStrNotFound,
        R_ISVTELINFOAPP_TELSTR_ERRNOTFOUND);
    CCoeEnv::Static()->ReadResource(iErrStrGeneral,
        R_ISVTELINFOAPP_TELSTR_ERRGENERAL);
    CCoeEnv::Static()->ReadResource(iErrStrCancel,
        R_ISVTELINFOAPP_TELSTR_ERRCANCEL);
    CCoeEnv::Static()->ReadResource(iErrStrNoMemory,
        R_ISVTELINFOAPP_TELSTR_ERRNOMEMORY);
    CCoeEnv::Static()->ReadResource(iErrStrNotSupported,
        R_ISVTELINFOAPP_TELSTR_ERRNOTSUPPORTED);
    CCoeEnv::Static()->ReadResource(iErrStrArgument,
        R_ISVTELINFOAPP_TELSTR_ERRARGUMENT);
    CCoeEnv::Static()->ReadResource(iErrStrTotalLossOfPrecision,
        R_ISVTELINFOAPP_TELSTR_ERRTOTALLOSSOFPRECISION);
    CCoeEnv::Static()->ReadResource(iErrStrBadHandle,
        R_ISVTELINFOAPP_TELSTR_ERRBADHANDLE);
    CCoeEnv::Static()->ReadResource(iErrStrOverflow,
        R_ISVTELINFOAPP_TELSTR_ERROVERFLOW);
    CCoeEnv::Static()->ReadResource(iErrStrUnderflow,
        R_ISVTELINFOAPP_TELSTR_ERRUNDERFLOW);
    CCoeEnv::Static()->ReadResource(iErrStrAlreadyExists,
        R_ISVTELINFOAPP_TELSTR_ERRALREADYEXISTS);
    CCoeEnv::Static()->ReadResource(iErrStrInUse,
        R_ISVTELINFOAPP_TELSTR_ERRINUSE);
    CCoeEnv::Static()->ReadResource(iErrStrServerTerminated,
        R_ISVTELINFOAPP_TELSTR_ERRSERVERTERMINATED);
    CCoeEnv::Static()->ReadResource(iErrStrServerBusy,
        R_ISVTELINFOAPP_TELSTR_ERRSERVERBUSY);
    CCoeEnv::Static()->ReadResource(iErrStrUnknown,
        R_ISVTELINFOAPP_TELSTR_ERRUNKNOWN);
    CCoeEnv::Static()->ReadResource(iErrStrCorrupt,
        R_ISVTELINFOAPP_TELSTR_ERRCORRUPT);
    CCoeEnv::Static()->ReadResource(iErrStrAccessDenied,
        R_ISVTELINFOAPP_TELSTR_ERRACCESSDENIED);
    CCoeEnv::Static()->ReadResource(iErrStrCommsLineFail,
        R_ISVTELINFOAPP_TELSTR_ERRCOMMSLINEFAIL);
    CCoeEnv::Static()->ReadResource(iErrStrTimedOut,
        R_ISVTELINFOAPP_TELSTR_ERRTIMEDOUT);
    CCoeEnv::Static()->ReadResource(iErrStrCouldNotConnect,
        R_ISVTELINFOAPP_TELSTR_ERRCOULDNOTCONNECT);
    CCoeEnv::Static()->ReadResource(iErrStrCouldNotDisconnect,
        R_ISVTELINFOAPP_TELSTR_ERRCOULDNOTDISCONNECT);
    CCoeEnv::Static()->ReadResource(iErrStrHardwareNotAvailable,
        R_ISVTELINFOAPP_TELSTR_ERRHARDWARENOTAVAILABLE);
    CCoeEnv::Static()->ReadResource(iErrStrSessionClosed,
        R_ISVTELINFOAPP_TELSTR_ERRSESSIONCLOSED);
    CCoeEnv::Static()->ReadResource(iErrStrPermissionDenied,
        R_ISVTELINFOAPP_TELSTR_ERRPERMISSIONDENIED);
    CCoeEnv::Static()->ReadResource(iErrStrDefault,
        R_ISVTELINFOAPP_TELSTR_ERRORDEFAULT);
	CCoeEnv::Static()->ReadResource(iEmuApiNotSupported,
	    R_ISVTELINFOAPP_ERR_EMUAPINOTSUPPORTED);        
    }

// ----------------------------------------------------------------------------
// void CIsvTelInfoAppTelephony::GeneralErrorMessage()
// Finds the system wide error. On return aErrMsgBuffer contains the 
// error message.
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppTelephony::GeneralErrorMessage( TInt aStatus,
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

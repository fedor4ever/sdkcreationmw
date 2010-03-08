/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class to handle Event simulation events
*
*/


// INCLUDE FILES
#include "EventWrapper.h"

#include <AlmConst.h> // for alarm server
#include <AgmAlarm.h>  // KUidAgendaModelAlarmCategory - the alarm category id for calendar alarms
#include <e32property.h> // subscribe&publish
#include <eikenv.h>
#include <eikon.hrh>
#include <HWRMPowerStateSDKPSKeys.h>

#include "doseventsender.h"
#include "f32file.h" // RFs
#include "EcmtEventSimPlugin.h" // for callback
#include "ecmtmmcevent.h" //CEcmtMMCEvent

const TUint64 KTTYDeviceId = 0x012345;
const TUint64 KHeadsetDeviceId = 0x010103;
const TUint64 KLoopsetDeviceId = 0x010201;

// ============================= LOCAL FUNCTIONS ===============================
// none

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CEventWrapper::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CEventWrapper* CEventWrapper::NewL( CEventSimPlugin* aPlugin )
    {
    CEventWrapper* self = new(ELeave) CEventWrapper ( );
    CleanupStack::PushL( self );
    self->ConstructL( aPlugin );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CEventWrapper::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEventWrapper::ConstructL( CEventSimPlugin* aPlugin )
    {
	iPlugin = aPlugin;
	iMMCPlugin = CEcmtMMCEvent::NewL();
    }

// -----------------------------------------------------------------------------
// CEventWrapper::CEventWrapper
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CEventWrapper::CEventWrapper( ) 
    {    
    // Initial state for accessory connections
	iAccConnStatus.HWDeviceID = 0;
	iAccConnStatus.AccConnStatus = ETFAsyDisc;
    }

// -----------------------------------------------------------------------------
// CEventWrapper::~CEventWrapper
// Destructor.
// -----------------------------------------------------------------------------
//
CEventWrapper::~CEventWrapper()
    {
    delete iMMCPlugin;
    }

// -----------------------------------------------------------------------------
// CEventWrapper::SetEventL
// Set Wrapper event
// -----------------------------------------------------------------------------
//
void CEventWrapper::SetEventL( TInt aEvent, TDesC& aParam )
  {
  switch ( aEvent )
    {
    case EWrapperBatteryLow:
        SetDOSEventL( aEvent );
        break;
      
    case EWrapperBatteryEmpty:
        SetDOSEventL( aEvent );
        break;
      
    case EWrapperGripOpen:
        SetGripEventL( aEvent );
        break;
      
    case EWrapperGripClosed:
        SetGripEventL( aEvent );
        break;
      
    case EWrapperCameraLensOpen:
        SetDOSEventL( aEvent );
        break;
      
    case EWrapperCameraLensClosed:
        SetDOSEventL( aEvent );
        break;
      
    case EWrapperAlarmNotification:
        SetClockAlarmL( aParam );
        break;
      
    case EWrapperCalendarNotification:
        SetCalendarAlarmL( aParam );
        break;
            
    case EWrapperDiskSpaceNotification:
    	SetDiskSpaceNotificationL( );
        break;
      
    case EWrapperSIMRemoved:
        SetDOSEventL( aEvent );
        break;
      
    case EWrapperSIMChanged:
        SetDOSEventL( aEvent );
        break;
      
    case EWrapperSIMLocked:
        SetDOSEventL( aEvent );
        break;
      
    case EWrapperSIMSMSMemoryFull:
        SetDOSEventL( aEvent );
        break;
      
    case EWrapperHeadSetConnected:
        SetDOSEventL( aEvent );
        break;
      
    case EWrapperHeadsetRemoved:
        SetDOSEventL( aEvent );
        break;
      
    case EWrapperTtyConnected:
        SetDOSEventL( aEvent );
        break;
      
    case EWrapperTtyRemoved:
        SetDOSEventL( aEvent );
        break;
      
    case EWrapperLoopsetConnected:
        SetDOSEventL( aEvent );
        break;
      
    case EWrapperLoopsetRemoved:
        SetDOSEventL( aEvent );
        break;
      
    case EWrapperHandsfreeModeOn:
        SetHandsFreeL( ETrue );
        break;
     
    case EWrapperHandsfreeModeOff:
        SetHandsFreeL( EFalse );
        break;
      
    case EWrapperIncomingVoicecall:
        // SetTSYSimulationEventL( aEvent, aParam );
        break;
      
    case EWrapperIncomingDatacall:
        // SetTSYSimulationEventL( aEvent, aParam );
        break;
      
    case EWrapperRemoteHangup:
        // SetTSYSimulationEventL( aEvent, aParam );
        break;
        
    case EWrapperConnectingExternalPowerCharger:
        SetDOSEventL( aEvent );
        break;
    case EWrapperRemovingExternalPowerCharger:
        SetDOSEventL( aEvent );
        break;    
    	
    case EWrapperMMC:
    	iMMCPlugin->SetEventL( aParam );
    	break;
    
    case EWrapperBatteryFull:
        SetDOSEventL( aEvent );
        break;	
    
    default:
      // bad event
      RDebug::Print(_L("CEventWrapper::SetEventL() - Unknown event"));
      User::Leave( KErrNotFound );
      break;    
    }

  }


// -----------------------------------------------------------------------------
// CEventWrapper::SetGripEventL
// Set grip event
// -----------------------------------------------------------------------------
//

void CEventWrapper::SetGripEventL( TInt aEvent )
  {
  TWsEvent aKey;
  TKeyEvent keyEvent;

  keyEvent.iRepeats = 0;

   switch ( aEvent )
    { 
    case EWrapperGripOpen:
        keyEvent.iCode = EKeyGripOpen;
        keyEvent.iScanCode = EStdKeyDevice4;
  		break;
  
	case EWrapperGripClosed:
	    keyEvent.iCode = EKeyGripClose;
        keyEvent.iScanCode = EStdKeyDevice5;
		break;
  
  	default:
      	// bad event
      	User::Leave( KErrNotFound );
      	break; 
    }

  RWsSession WsSession;
  User::LeaveIfError( WsSession.Connect() );
  
  WsSession.SimulateKeyEvent( keyEvent );
  aKey.SetType( EEventKey );
  User::LeaveIfError( WsSession.SendEventToAllWindowGroups( aKey ) );
  WsSession.Close();
  }
    
  
// -----------------------------------------------------------------------------
// CEventWrapper::SetClockAlarmL
// Set Clock alarm
// -----------------------------------------------------------------------------
//
void CEventWrapper::SetClockAlarmL( TDesC& aParam )
    {
    
    TLex lexer ( aParam );
    // get alarm time
    TPtrC minutes = lexer.NextToken();

    RASCliSession alarmSvr;
    User::LeaveIfError( alarmSvr.Connect() );
    CleanupClosePushL( alarmSvr );
	TASShdAlarm alarm;    
    
    // check if default values are used. EcmtManager-EventSim plugin 
	// sends null string instead of 0.
    if ( minutes.Compare( KNullDesC ) == KErrNone )
    	{
    	alarm.NextDueTime().HomeTime(); // show alarm note immediately
    	}
    else
    	{
    	// get id to int
    	TLex alarmLexer ( minutes );
    	TInt alarmMinutes;
    	User::LeaveIfError( alarmLexer.Val( alarmMinutes ) );
    	alarm.NextDueTime().HomeTime();
    	alarm.NextDueTime() += TTimeIntervalMinutes( alarmMinutes );
    	}
    
    // set clock related alarm
    alarm.Category() = KAlarmClockOne;
    alarm.RepeatDefinition() = EAlarmRepeatDefintionRepeatOnce;
    alarm.SoundName() = KSoundName;
    alarmSvr.AlarmAdd( alarm );
    // set enabled
    User::LeaveIfError( alarmSvr.SetAlarmStatus(alarm.Id(), EAlarmStatusEnabled));
    CleanupStack::PopAndDestroy(); // alarmSvr
  
    }    
    
    
// -----------------------------------------------------------------------------
// CEventWrapper::SetCalendarAlarmL
// Set calendar alarm
// -----------------------------------------------------------------------------
//    
void CEventWrapper::SetCalendarAlarmL( TDesC& aParam )
    {
    TLex lexer ( aParam );
    // get alarm time
    TPtrC minutes = lexer.NextToken();
    
    // skip space
    lexer.SkipSpace();
    // get alarm message
    TPtrC alarmMessage = lexer.Remainder();
    
    
    RASCliSession alarmSvr;
    User::LeaveIfError( alarmSvr.Connect() );
	CleanupClosePushL( alarmSvr );
	
    TASShdAlarm alarm;    
    // check if default values are used
    if ( minutes.Compare( KDefault ) == KErrNone )
    	{
    	alarm.NextDueTime().HomeTime(); // show alarm note immediately
    	}
    else
    	{
    	// get id to int
    	TLex alarmLexer ( minutes );
    	TInt alarmMinutes;
    	User::LeaveIfError( alarmLexer.Val( alarmMinutes ) );
    	alarm.NextDueTime().HomeTime();
    	alarm.NextDueTime() += TTimeIntervalMinutes( alarmMinutes );
    	}
    
    // set clock related alarm
    alarm.Category() = KUidAgendaModelAlarmCategory;
    alarm.RepeatDefinition() = EAlarmRepeatDefintionRepeatOnce;
    alarm.Message() = alarmMessage;
    alarm.SoundName() = KSoundName;
    alarmSvr.AlarmAdd( alarm );
    // set enabled
    User::LeaveIfError( alarmSvr.SetAlarmStatus( alarm.Id(), EAlarmStatusEnabled ));
    CleanupStack::PopAndDestroy(); // alarmSvr
  
    }    
 
    
// -----------------------------------------------------------------------------
// CEventWrapper::SetDiskSpaceNotificationL
// Set diskspace notification
// -----------------------------------------------------------------------------
//     
void CEventWrapper::SetDiskSpaceNotificationL( )
    {
    RFs fs;
    User::LeaveIfError( fs.Connect() );
    User::LeaveIfError( fs.SetErrorCondition( KErrDiskFull, 1 ));
    fs.Close();
    }    


// -----------------------------------------------------------------------------
// CEventWrapper::SetHandsFreeL
// Set handsfree mode
// -----------------------------------------------------------------------------
//    
void CEventWrapper::SetHandsFreeL( TBool /*aValue*/ )
    { 
	/*
	//Before Accessory framework:
    RPhCltServer phoneServer;
    RPhCltIhf hf;
    
    User::LeaveIfError( phoneServer.Connect() );
    CleanupClosePushL( phoneServer );
    
    User::LeaveIfError( hf.Open( phoneServer ) );
    if ( aValue )
    	{
        hf.SetIhfMode( ETrue, ETrue );	
    	}
    else 
    	{
        hf.SetIhfMode( EFalse, ETrue );	
    	}
    
    hf.Close();
    CleanupStack::PopAndDestroy(); // phoneServer
	
	//New way for implementing handsfree mode on/off. In addition to this it requires 
	//a plugin which simulates speaker. See also MTelephonyAudioRoutingObserver.
	CTelephonyAudioRouting* audioRouting = CTelephonyAudioRouting::NewL(*this);
	// get available outputs
	TArray<CTelephonyAudioRouting::TAudioOutput> availableOutputs = audioRouting->AvailableOutputs();

	TInt count = availableOutputs.Count();
	CTelephonyAudioRouting::TAudioOutput output;
	for(TInt i = 0; i < count; ++i)
		{
		output = availableOutputs[i];
		}
	output = audioRouting->Output();
	// set output
	audioRouting->SetOutputL(CTelephonyAudioRouting::ELoudspeaker);
	*/
    }


// -----------------------------------------------------------------------------
// CEventWrapper::SetDOSEventL
// Set DOS event
// -----------------------------------------------------------------------------
//    
void CEventWrapper::SetDOSEventL( TInt aEvent )
    {
    CDosEventSender* eventSender = CDosEventSender::NewL();

    TInt err = 0;
    switch ( aEvent )
        {
    
#ifdef	RD_VIBRA_AND_LIGHT_CHANGE   
 
    	case EWrapperBatteryLow:
        	eventSender->BatteryLowWarning( 0 );
        	break;

    	case EWrapperBatteryEmpty:
    		eventSender->BatteryStatus( EBatteryEmpty );
    		break;

    	case EWrapperConnectingExternalPowerCharger:
    		eventSender->ChargingState( EChargingStateOn );
    		break;

    	case EWrapperRemovingExternalPowerCharger:
    		eventSender->ChargingState( EChargingStateOff );
    		break;	

       	case EWrapperBatteryFull:
    		eventSender->ChargingState( EChargingStateFull );
    		break;

#else

	    case EWrapperBatteryLow:
	        // Set battery level
	        err = RProperty::Define(KPSUidHWRMPowerState, KHWRMBatteryLevel, RProperty::EInt);
        	err = RProperty::Set(KPSUidHWRMPowerState, KHWRMBatteryLevel, EBatteryLevelLevel1);        	
        	// Show note
			err = RProperty::Define(KPSUidHWRMPowerState, KHWRMBatteryStatus, RProperty::EInt);
        	err = RProperty::Set(KPSUidHWRMPowerState, KHWRMBatteryStatus, EBatteryStatusLow);
        	break;
        	
		case EWrapperBatteryEmpty:
            // Set battery level
		    err = RProperty::Define(KPSUidHWRMPowerState, KHWRMBatteryLevel, RProperty::EInt);
        	err = RProperty::Set(KPSUidHWRMPowerState, KHWRMBatteryLevel, EBatteryLevelLevel0);		
		    // Show note
			err = RProperty::Define(KPSUidHWRMPowerState, KHWRMBatteryStatus, RProperty::EInt);
        	err = RProperty::Set(KPSUidHWRMPowerState, KHWRMBatteryStatus, EBatteryStatusEmpty);
    		break;

    	case EWrapperConnectingExternalPowerCharger:
    		err = RProperty::Define(KPSUidHWRMPowerState, KHWRMChargingStatus, RProperty::EInt);
        	err = RProperty::Set(KPSUidHWRMPowerState, KHWRMChargingStatus, EChargingStatusCharging);
    		break;

    	case EWrapperRemovingExternalPowerCharger:    	
    		err = RProperty::Define(KPSUidHWRMPowerState, KHWRMChargingStatus, RProperty::EInt);
        	err = RProperty::Set(KPSUidHWRMPowerState, KHWRMChargingStatus, EChargingStatusNotConnected);
    		break;	

       	case EWrapperBatteryFull:
       	    // Set battery level
       	    err = RProperty::Define(KPSUidHWRMPowerState, KHWRMBatteryLevel, RProperty::EInt);
        	err = RProperty::Set(KPSUidHWRMPowerState, KHWRMBatteryLevel, EBatteryLevelLevel7);
       	    //Show note
       		err = RProperty::Define(KPSUidHWRMPowerState, KHWRMChargingStatus, RProperty::EInt);
        	err = RProperty::Set(KPSUidHWRMPowerState, KHWRMChargingStatus, EChargingStatusChargingComplete);
    		break;    
#endif

#ifndef RD_STARTUP_CHANGE

	    case EWrapperSIMSMSMemoryFull:
		    eventSender->SmsStorageStatusChanged( ESmsSimStorageFull );
		    break;
		    
#endif // RD_STARTUP_CHANGE
		    
	    case EWrapperHeadSetConnected:
			SetProperty( KHeadsetDeviceId, ETFAsyConn );
	    	break;
	    
	    case EWrapperHeadsetRemoved:
			SetProperty( KHeadsetDeviceId, ETFAsyDisc );
	    	break;
	    	
	    case EWrapperTtyConnected:
			SetProperty( KTTYDeviceId, ETFAsyConn );
			break;
	    	
	    case EWrapperTtyRemoved:
			SetProperty( KTTYDeviceId, ETFAsyDisc );
	    	break;

	    case EWrapperLoopsetConnected:
			SetProperty( KLoopsetDeviceId, ETFAsyConn );
	    	break;
	    	
	    case EWrapperLoopsetRemoved:
			SetProperty( KLoopsetDeviceId, ETFAsyDisc );
	    	break;
	    	
        default:
      		// bad event
      		RDebug::Print(_L("CEventWrapper::SetDOSEventL() - Unknown event!"));
	      	User::Leave( KErrNotFound );
      		break;  
    }
    
    delete eventSender;
    }

// -----------------------------------------------------------------------------
// CEventWrapper::SetProperty
// This function modifies Publish&Subscribe variable 
// KTFAccessoryTestProperty. By modifying the value of this variable accessory 
// connections and disconnections can be simulated. AsyStub plugin monitors and 
// responses to the changes of this variable by implementing accessory 
// simulations with the S60 Accessory Framework.
// -----------------------------------------------------------------------------
//   
void CEventWrapper::SetProperty( const TInt64 aHWDeviceID, 
                                 TTFASYReference aMethod )
	{	    	
    RProperty subscribe;
	TInt err(KErrNone);
	err = RProperty::Define( KTFAccessoryTestProperty, KTFAccessoryMethod, 
	                            RProperty::EByteArray );    
    if( err != KErrAlreadyExists )
    	User::LeaveIfError( err );       
    
    err =  subscribe.Attach(KTFAccessoryTestProperty, KTFAccessoryMethod);
    User::LeaveIfError( err );    	    

    TPckgBuf< TTFAccessoryPublishAndSubscribe > buf;
	TAccPolGenericID genericID;
	
	buf().iMethod     = aMethod;
    buf().iParam1     = aHWDeviceID;
    buf().iGenericID  = genericID;
	
    // If trying to connect without disconnecting first, disconnect the 
    // previous device first
    if(	aMethod == ETFAsyConn && iAccConnStatus.AccConnStatus == ETFAsyConn )
        {            
        //disconnect old accessory  
        TPckgBuf< TTFAccessoryPublishAndSubscribe > bufDisc;
	    TAccPolGenericID genericIDDisc;
	
        bufDisc().iMethod     = ETFAsyDisc;
	    bufDisc().iParam1     = iAccConnStatus.HWDeviceID;
        bufDisc().iGenericID  = genericIDDisc;
    
        // Send capability to stub.
	    err = subscribe.Set( KTFAccessoryTestProperty, 
	                         KTFAccessoryMethod, 
	                         bufDisc ); 
	    User::LeaveIfError( err );

		// Warning this is a MAGIC NUMBER!! We have to give accessory framework
		// some time completing its actions. Especially now when connecting 
		// new accessory (below). A suitable time seems to be the following. 
		// Without this new accessory do not get connected.
		User::After( TTimeIntervalMicroSeconds32(500000) );
		}
	
    // Send capability to stub.
    err = subscribe.Set(KTFAccessoryTestProperty, KTFAccessoryMethod, buf ); 
    User::LeaveIfError( err );
		
	iAccConnStatus.HWDeviceID = aHWDeviceID;
	iAccConnStatus.AccConnStatus = aMethod;
	}

// -----------------------------------------------------------------------------
// CEventWrapper::GetWrapperStatusL
// Reques MMC drive status from iMMCPlugin;
// -----------------------------------------------------------------------------
//    
void CEventWrapper::GetWrapperStatusL(CBufFlat* aMsg)
   {
    // get mmc status
   	HBufC8* msg = iMMCPlugin->SendMMCStatusLC();   	   	   	
	aMsg->InsertL( aMsg->Size(), msg->Des() );
   	CleanupStack::PopAndDestroy(); //msg   	
   }

// End of File  

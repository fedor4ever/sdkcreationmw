/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Phone and MMC events wrapper class.
*
*/


#ifndef __EVENTWRAPPER_H__
#define __EVENTWRAPPER_H__

//  INCLUDES
#include <e32base.h>
#include <AccPolGenericid.h>
#include "..\..\..\..\..\AsyStub\inc\TfAccessoryTestControlTypes.h"

// FORWARD DECLARATIONS
class CEventSimPlugin;
class CEcmtMMCEvent;

// CONSTANTS
_LIT(KSoundName, "a");    // for the alarm alert note.

_LIT(KRandom, "random"); // for memory notification
_LIT(KDeterministic, "deterministic"); // for memory notification
_LIT(KNone, "none"); // for memory notification
_LIT(KReset, "reset"); // for memory notification

_LIT(KDefault, "0"); // for indicating default settings


    
/**
* Enumeration for events. EventSim plugin at EcmtManager 
* side uses same values, so if this is modified, remember to 
* update values to Java side also.
*/
enum TWrapperEvent
    {
    EWrapperBatteryLow = 0,
    EWrapperBatteryEmpty = 1,
    EWrapperGripOpen = 2,
    EWrapperGripClosed = 3,
    EWrapperCameraLensOpen = 4,
    EWrapperCameraLensClosed = 5,
    EWrapperAlarmNotification = 6,
    EWrapperCalendarNotification = 7,
    EWrapperMemoryNotification = 8,
    EWrapperDiskSpaceNotification = 9,
    EWrapperSIMRemoved = 10,
    EWrapperSIMChanged = 11,
    EWrapperSIMLocked = 12,
    EWrapperSIMSMSMemoryFull = 13,
    EWrapperHeadSetConnected = 14,
    EWrapperHeadsetRemoved = 15,
    EWrapperTtyConnected = 16,
    EWrapperTtyRemoved = 17,
    EWrapperLoopsetConnected = 18,
    EWrapperLoopsetRemoved = 19,
    EWrapperHandsfreeModeOn = 20,
    EWrapperHandsfreeModeOff = 21,
    EWrapperIncomingVoicecall = 22,
    EWrapperIncomingDatacall = 23,
    EWrapperIncomingFaxcall = 24,
    EWrapperRemoteHangup = 25,
    EWrapperConnectingExternalPowerCharger = 26,
    EWrapperRemovingExternalPowerCharger = 27,
    EWrapperMMC = 28,
    EWrapperBatteryFull = 29
    };

/**
* Struct for defining the state of accessory connectivity
*/
struct AccConnectStatus 
    {
    TInt64    HWDeviceID;
    TTFASYReference AccConnStatus;          
    };

// CLASS DECLARATION

/**
*  Wrapper class for setting phone and MMC events 
*  
*  @lib EcmtEventSimPlugin.lib
*  @since 
*  @ingroup EcmtPlugins
*/
class CEventWrapper
{
public: // Constructors and destructor

    /**
    * Two-phased constructor.
    * @param aPlugin 
    */
    static CEventWrapper* NewL( CEventSimPlugin* aPlugin );
    
    /**
    * Destructor.
    */
    ~CEventWrapper();
     
public:
  /**
    * Set Simulation event.
    * @since
    * @param aEvent Event that is set on
    * @param aParam Parameters of the event 
    */
    void SetEventL( TInt aEvent, TDesC& aParam );
    
    /**
    * Get status from wrapper.
    * @since
    * @param aMsg includes the status of the wrapper
    */
    void GetWrapperStatusL( CBufFlat* aMsg );
    
    /**
    * This function modifies Publish&Subscribe variable 
    * KTFAccessoryTestProperty for simulating accessory connections and
    * disconnections. AsyStub plugin monitors this variable and as a 
    * plugin for Accessory Framework sets accessory simulations to happen.
    * @param aHWDeviceID ID for accessory. Possible devices are Defined in 
    *        epoc32/winscw/c/private/1020504A/AccPolGIDInt.dat.
    * @param aMethod Disconnect or connect (ETFAsyDisc or ETFAsyConn)
    */
	void SetProperty(const TInt64 aHWDeviceID, TTFASYReference aMethod);
 
private:
    /**
    * By default Symbian 2nd phase constructor is private.
    */
    void ConstructL( CEventSimPlugin* aPlugin );
    
    /**
    * C++ default constructor.
    */
    CEventWrapper( );
  
    /**
    * Set Clock alarm
    * @since
    * @param aParam Parameters of Clock Alarm 
    */
    void SetClockAlarmL( TDesC& aParam );
    
    /**
    * Set Grip Event
    * @since
    * @param aEvent Event enum value
    */
    void SetGripEventL( TInt aEvent );
    
    /**
    * Set Calendar alarm
    * @since
    * @param aParam Parameters of Calendar Alarm 
    */
    void SetCalendarAlarmL( TDesC& aParam );
           
    /**
    * Set diskspace notification.
    * @since
    */ 
    void SetDiskSpaceNotificationL( );
    
    /**
    * Set HandsFree (on/off)
    * @since
    * @param aValue Sets handsFree on or off. 
    */ 
    void SetHandsFreeL( TBool aValue );
    
    /**
    * Set DOS Event
    * @since
    * @param aEvent Event enum value
    */ 
    void SetDOSEventL( TInt aEvent );

private:
	// instance of the Plugin
  CEventSimPlugin*  iPlugin;
  
  // For handling MMC events
  CEcmtMMCEvent*    iMMCPlugin;
  
  // Status for accessory connections  
  AccConnectStatus iAccConnStatus;
};

#endif // __EVENTWRAPPER_H__

// End of File
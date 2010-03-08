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
* Description:  Interface for Event simulation plugin
*
*/

#ifndef ECMTEVENTSIMPLUGIN_H
#define ECMTEVENTSIMPLUGIN_H

//  INCLUDES
#include <e32base.h>
#include "EcmtPlugin.h"
#include "EcmtMessagingIf.h"

// CONSTANTS
const TMessageChannel KChannelId = 1;
const TInt KDefaultInterval = 1 * 5000 * 1000; 

// MACROS
//none

// DATA TYPES
//none

// FUNCTION PROTOTYPES
//none

// FORWARD DECLARATIONS
class CEventWrapper;

// CLASS DECLARATION

/**
*  Provides a message interceptor and handler for messages coming from the
*  EventSimPlugin in epdt
*  
*  @lib EcmtEventSimPlugin.lib
*  @since 
*  @ingroup EcmtPlugins
*/
class CEventSimPlugin : public CTimer, public MEcmtPlugin, public MEcmtMessageEventObserver
  {
  public:
      /**
      * Destructor.
      */
      ~CEventSimPlugin( );

      /**
      * Two-phased constructor.
      *
      */
      static MEcmtPlugin* NewL( );

      /**
      * Binds this object into a messaging interface.
      * @since 
      * @param MEcmtMessaging* object to bind to
      */
      void BindMessagingL( MEcmtMessaging* aMessaging );
    
      /**
      * Sends data to EPDT
      * @since 
      * @param aMsg Message to send 
      */
      void SendDataL( CBufFlat* aMsg );
    
      
  
  protected: // from CActive
      /**
      * From CTimer.
      * @since 
      */  
      void RunL();
    
      /**
      * From CTimer.
      * @since 
      * @param aError Error code 
      */  
      TInt RunError( TInt aError );
  
  private:
  
  	  /**
      * Handle Wrapper status change
      * @since 
      */        
      void HandleStatusChangeL();
      
      /**
      * Handles a notify message
      * @since 
      * @param const CEcmtMessageEvent& message event
      */  
      void HandleNotifyL( const CEcmtMessageEvent& aEvent );
    
      /**
      * C++ default constructor.
      */
      CEventSimPlugin();
    
      /**
      * By default Symbian 2nd phase constructor is private.
      */
      void ConstructL();

  private:
      // Cannot be a reference since it is given only after construction
      MEcmtMessaging* iMessaging;
    
      // Handles message sending
      MEcmtMessageSender* iMessageSender;
    
      // Uid of the plugin
      TUid iUid;
      // Uid of the java plugin
      TUid iTargetUid;
    
      // event wrapper
      CEventWrapper* iWrapper; 
    
      TTimeIntervalMicroSeconds32 iInterval;
  };
#endif // ECMTEVENTSIMPLUGIN_H

// End of File
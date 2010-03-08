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
* Description:  Server Session for Ecmt server
*
*/



#ifndef ECTMSERVERSESSION_H
#define ECTMSERVERSESSION_H

//  INCLUDES
#include <e32base.h>
#include "EcmtMessaging.h"

// CONSTANTS

// MACROS

// FORWARD DECLARATIONS
class CEcmtServer;

// CLASS DECLARATION

/**
*  Class handling client sessions with CEcmtServer
*  @ingroup EcmtCore
*/
class CEcmtServerSession : 
    public CSession2,
    public MEcmtMessageEventObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CEcmtServerSession* NewL( );
        
        /**
        * Destructor.
        */
        virtual ~CEcmtServerSession();

    public: // New functions
      /**
       * Sends a message to the client if it has requested a message.
       * If the client is not listening, the message is ignored.
       * @param aNotification the incoming message
       */
    	void NotifyClient( const TDesC8& aNotification );

      /**
       * Returns the UID of the client application.
       * Unless the application has set itself as a message
       * observer, this will return zero.
       */
      TUid GetClientUid();
        
    public: // From CSession
		/**
        * Service request from client
        * @param aMessage message from client (containing requested operation and any data)
        */
		void ServiceL( const RMessage2& aMessage );

    protected:  // New functions
        
    protected:  // Functions from base classes

    private:

        /**
        * C++ default constructor.
        */
        CEcmtServerSession( );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
        /**
        * Sends a binary message to any UID within Ecmt Server and/or Manager.
        * @param aMessage the message to be sent
        */
        void SendDataL( const RMessage2& aMessage );

        /**
        * Sends a text message to the Diagnostics log view.
        * @param aMessage the message to be sent
        */
        void SendTextL( const RMessage2& aMessage );

        /**
        * Handles incoming messages for the client application.
        * @param aEvent the incoming message
        */
        void HandleNotifyL( const CEcmtMessageEvent& aEvent );

    private:    // Data
        RMessage2	      iClientNotifyMessage;
    	  TBool		      iClientWaitingForNotification;
    	  TBool		      iInitialClientNotificationSent;
        TUid            iClientUid;
        MEcmtMessaging* iMessaging;
    };

#endif
            
// End of File

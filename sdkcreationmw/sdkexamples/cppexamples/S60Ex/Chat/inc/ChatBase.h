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


#ifndef CHATBASE_H
#define CHATBASE_H

#include "ChatProtocolConstants.h"

// FORWARD DECLARATIONS
class MLog;

       /**
        * TChatState
        * The state of the active object, determines behaviour within
        * the RunL method.
        * EWaitingToGetDevice waiting for the user to select a device
        * EGettingDevice searching for a device
        * EGettingService searching for a service
        * EGettingConnection connecting to a service on a remote machine
        * EConnected connected to a service on a remote machine
        * ESendingMessage sending a message to the remote machine
        * EWaitingForMessage wainting for message from remote machine
        * EConnecting connecting to remote machine
        * EDisconnecting disconnecting from remote machine
        */
 
        enum TChatState 
            {
            EWaitingToGetDevice,
            EGettingDevice,
            EGettingService,
            EGettingConnection,
            EConnected,
            EDisconnected,
            ESendingMessage,
            EWaitingForMessage,
            EConnecting,
            EDisconnecting
            };
            
            
/**
* CChatBase
* Connects and sends messages to a remote machine using bluetooth
*/
class CChatBase : public CActive 
    {
    public: // Constructors and destructor

        /**
        * NewL()
        * Construct a CChatClient
        * @param aLog the log to send output to
        * @return a pointer to the created instance of CChatClient
        */
        static CChatBase* NewL( MLog& aLog );

        /**
        * NewLC()
        * Construct a CChatClient
        * @param aLog the log to send output to
        * @return a pointer to the created instance of CChatClient
        */
        static CChatBase* NewLC( MLog& aLog );

        /**
        * CChatBase()
        * Constructs this object
        * @param aLog the log to send output to
        */
        CChatBase( MLog& aLog );
        
        /**
        * ~CChatBase()
        * Destroy the object and release all memory objects. 
        * Close any open sockets.
        */
        virtual ~CChatBase();

    public:     // New functions

        /**
        * SetServer()
        * Set servermode
        * @param aServerMode ETrue if servermode is used
        */
        void SetServer( TBool aServerMode );
    
        /**
        * Server()
        * Get Servermode
        * @return servermode
        */
        TBool Server();
    
        /**
        * SetState()
        * Set state
        * @param aState the state of the engine
        */
        void SetState( TChatState aState );
    
        /**
        * State()
        * Get State
        * @return state
        */
        TInt State();
    
        /**
        * IsConnected()
        * @return ETrue if the client is fully connected to the server.
        */
        TBool IsConnected();

        /**
        * IsConnecting()
        * @return ETrue if the client is establishing a 
        * connection to the server.
        */
        TBool IsConnecting();

        /**
        * IsConnected()
        * @return ETrue if the client is connected.
        */
        TBool IsSendingMessage();
        
    
        /**
        * IsReadyToSendMessage()
        * @return ETrue if the client can send a message.
        */
        TBool IsReadyToSendMessage();


    protected:    // from CActive

        /**
        * DoCancel()
        * Cancel any outstanding requests
        */
        void DoCancel();

        /**
        * RunL()
        * Respond to an event
        */
        void RunL();

    private:    // Functions from base classes

        /**
        * ConstructL()
        * Performs second phase construction of this object
        */
        void ConstructL();

    private:    // New functions
     
    private:    // data

        /** 
        * iState the current state of the client 
        */
        TChatState iState;

        /** 
        * iLog the log to send output to 
        */
        MLog& iLog;
        
        /** 
        * iServerMode indicates server mode 
        */
        TBool iServerMode;  
    };

#endif // CHATBASE_H

// End of File

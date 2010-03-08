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


#ifndef CHATBT_H
#define CHATBT_H


#include <es_sock.h>
#include <bt_sock.h>
#include <BTextNotifiers.h>
#include <BtSdp.h>
#include "ChatBase.h"

// FORWARD DECLARATIONS
class CChatServiceSearcher;
class CChatServiceAdvertiser;
class MLog;

/**
* CChatBt
* Connects and sends messages to a remote machine using bluetooth
*/
class CChatBt : public CChatBase
    {
    public: // Constructors and destructor

        /**
        * NewL()
        * Construct a CChatBt
        * @param aLog the log to send output to
        * @return a pointer to the created instance of CChatBt
        */
        static CChatBt* NewL( MLog& aLog );

        /**
        * NewLC()
        * Construct a CChatBt
        * @param aLog the log to send output to
        * @return a pointer to the created instance of CChatBt
        */
        static CChatBt* NewLC( MLog& aLog );

        /**
        * ~CChatBt()
        * Destroy the object and release all memory objects. 
        * Close any open sockets.
        */
        virtual ~CChatBt();

    public:     // New functions


        /**
        * ConnectL()
        * Connect to an available service on a remote machine
        */
        void ConnectL();

        /** 
        * DisconnectL()
        * Disconnect from remote machine
        */
        void DisconnectL();

        /**
        * SendMessageL()
        * Sends a message to a service on a remote machine.
        * @param aText Text to send
        */    
        void SendMessageL( TDes& aText );
        
        /**
        * StartL()
        * Start server and waiting for connection.
        */  
        void StartL();
        
        /**
        * StopL()
        * Stop server.
        */ 
        void StopL();

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
        * ConnectToServerL
        * Connects to the service
        */    
        void ConnectToServerL();

        /**
        * DisonnectFromServerL()
        * Disconnects from the service
        */    
        void DisconnectFromServerL();

        /**
        * CChatBt()
        * Constructs this object
        * @param aLog the log to send output to
        */
        CChatBt( MLog& aLog );

        /**
        * ConstructL()
        * Performs second phase construction of this object
        */
        void ConstructL();
        
    private:    // New functions

        /**
        * RequestData()
        * Request data from the client, this object is notified
        * when data becomes available
        */
        void RequestData();


        /**
        * SetSecurityWithChannelL
        * Sets the security on the channel port 
        * And returns the channel available.
        * @param aAuthentication is authentication used
        * @param aEncryption is encryption used
        * @param aAuthorisation is authorisation used
        * @param aDenied is denied used
        * @return an available channel
        */
        void SetSecurityWithChannelL( TBool aAuthentication,
            TBool aEncryption,
            TBool aAuthorisation,
            TBool aDenied,
            TInt& aChannel );

        /**
        * SetSecurityOnChannelL()
        * Sets the security on the channel port
        * @param aAuthentication is authentication used
        * @param aEncryption is encryption used
        * @param aAuthorisation is authorisation used
        * @param aChannel used channel ID
        */
        static void SetSecurityOnChannelL( TBool aAuthentication,
            TBool aEncryption,
            TBool aAuthorisation,
            TInt aChannel );
            
    private:    // data

        /** 
        * iServiceSearcher searches for service this 
        * client can connect to.
        * Owned by CChatBt
        */
        CChatServiceSearcher* iServiceSearcher;

        /** 
        * iLog 
        * the log to send output to 
        */
        MLog& iLog;

        /** 
        * iMessage a copy of the message to send 
        * Owned by CChatBt
        */
        HBufC8* iMessage;

        /** 
        * iSocketServer 
        * a connection to the socket server 
        */
        RSocketServ iSocketServer;

        /** 
        * iSocket 
        * a socket to connect with 
        */
        RSocket iSocket;

        /** 
        * iServiceClass 
        * the service class UUID to search for 
        */
        TUUID iServiceClass;
        
        /** 
        * iBuffer 
        * the buffer to read data to 
        */
        TBuf8<KChatTextBufLength> iBuffer;
        
        /**
        * iAdvertiser used to advertise this service
        * Owned by CChatServer
        */
        CChatServiceAdvertiser* iAdvertiser;
        
        /** 
        * iAcceptedSocket 
        * the socket that connections are accepted to 
        */
        RSocket iAcceptedSocket;
    
        /** 
        * iLen 
        * length of data read 
        */
        TSockXfrLength iLen;
        
        /** 
        * iActiveSocket 
        * the active socket 
        */
        RSocket* iActiveSocket;

    };

#endif // CHATBT_H

// End of File

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


#ifndef CHATINET_H
#define CHATINET_H

#include <es_sock.h>
#include <in_sock.h>
#include "ChatBase.h"

// CONSTANTS
const TInt KIpAddressLen = 15;

// FORWARD DECLARATIONS
class MLog;

/**
* CChatInet
* Connects and sends messages to a remote machine using bluetooth
*/
class CChatInet : public CChatBase
    {
    public: // Constructors and destructor

        /**
        * NewL
        * Construct a CChatInet
        * @param aLog the log to send output to
        * @return a pointer to the created instance of CChatInet
        */
        static CChatInet* NewL( MLog& aLog );

        /**
        * NewLC
        * Construct a CChatInet
        * @param aLog the log to send output to
        * @return a pointer to the created instance of CChatInet
        */
        static CChatInet* NewLC( MLog& aLog );

        /**
        * ~CChatInet
        * Destroy the object and release all memory objects. 
        * Close any open sockets.
        */
        virtual ~CChatInet();

    public:     // New functions

        /**
        * ConnectL
        * Connect to an available service on a remote machine
        */
        void ConnectL( const TDesC& aAddress );

        /** 
        * DisconnectL
        * Disconnect from remote machine
        */
        void DisconnectL();

        /**
        * SendMessageL
        * Sends a message to a service on a remote machine.
        */    
        void SendMessageL( const TDesC& aText );
        
        /**
        * Stop
        * Stop server.
        */  
        void Stop();
        
        /**
        * RequestData()
        * Read more data from socket.
        */  
        void RequestData();
    
        /**
        * StartL
        * Start server.
        */ 
        void StartL();
    
    private:  // New functions
        /**
        * ConnectL
        * Connect to server.
        * @param aAddr address to connect to
        */ 
        void ConnectL( TUint32 aAddr );
    
    protected:    // from CActive

        /**
        * DoCancel
        * Cancel any outstanding requests
        */
        void DoCancel();

        /**
        * RunL
        * Respond to an event
        */
        void RunL();

    private:    // Functions from base classes
    
        /**
        * ConnectToServerL
        * Connects to the service
        */    
        void ConnectToServerL( const TDesC& aAddress );

        /**
        * DisonnectFromServerL
        * Disconnects from the service
        */    
        void DisconnectFromServerL();

        /**
        * CChatInet
        * Constructs this object
        */
        CChatInet( MLog& aLog );

        /**
        * ConstructL
        * Performs second phase construction of this object
        */
        void ConstructL();

    private:    // data
    
        /** 
        * iLog 
        * the log to send output to 
        */
        MLog& iLog;

        /** 
        * iMessage a copy of the message to send 
        * Owned by CChatInet
        */
        HBufC8* iMessage;

        /** 
        * iConnection 
        * a management interface to network connection 
        */
        RConnection iConnection;
    
        /** 
        * iSocketServer
        * a connection to the socket server 
        */
        RSocketServ iSocketServer;

        /** 
        * iSendingSocket
        * a socket to connect with 
        */
        RSocket iSocket;

        /** 
        * iBuffer
        * the buffer to read data to 
        */
        TBuf8<KChatTextBufLength> iBuffer;
        
        /**
        * iAddress
        * the address to be used in the connection.
        */
        TInetAddr iAddress;

        /**
        * iPort
        * the port number to connect to.
        */
        TInt iPort;

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
        * iResolver
        * DNS name resolver.
        */
        RHostResolver iResolver;

        /**
        * iNameEntry
        * the result from the name resolver.
        */
        TNameEntry iNameEntry;

        /**
        * iNameRecord
        * the name record found by the resolver.
        */
        TNameRecord iNameRecord;
        
        /**
        * iActiveSocket
        * the active socket 
        */
        RSocket* iActiveSocket;

    };

#endif // CHATINET_H

// End of File

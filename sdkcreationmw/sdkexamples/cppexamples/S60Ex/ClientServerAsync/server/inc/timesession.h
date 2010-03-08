/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __CTIMESESSION_H__
#define __CTIMESESSION_H__

// INCLUDE FILES
#include <e32base.h>

// FORWARD DECLARATIONS
class CTimeServer;

// CLASS DECLARATION
/**
* CTimeServerSession.
*  An instance of class CTimeServerSession is created for each client.
*/
class CTimeServerSession : public CSession2
    {
    public: // Constructors and destructors

        /**
        * NewL.
        * Two-phased constructor.
        * @param aServer The server.
        * @return Pointer to created CTimeServerSession object.
        */
        static CTimeServerSession* NewL( CTimeServer& aServer );

        /**
        * NewLC.
        * Two-phased constructor.
        * @param aServer The server.
        * @return Pointer to created CTimeServerSession object.
        */
        static CTimeServerSession* NewLC( CTimeServer& aServer );

        /**
        * ~CTimeServerSession.
        * Destructor.
        */
        virtual ~CTimeServerSession();

    public: // New functions

        /**
        * SendTimeToClient.
        * Call from Beat() and Synchronize().
        * Sends current time back to requesting client.
        */
        void SendTimeToClient();

    public: // Functions from base classes

        /**
        * From CSession, ServiceL.
        * Service request from client.
        * @param aMessage Message from client
        *                 (containing requested operation and any data).
        */
        void ServiceL( const RMessage2& aMessage );

    private: // Constructors and destructors

        /**
        * CTimeServerSession.
        * C++ default constructor.
        * @param aServer The server.
        */
        CTimeServerSession( CTimeServer& aServer );

        /**
        * ConstructL.
        * 2nd phase constructor.
        */
        void ConstructL();

    private: // New methods

        /**
        * PanicClient.
        * Causes the client thread to panic.
        * @param aMessage Message from client.
        * @param aPanic Panic code.
        */
        void PanicClient( const RMessagePtr2& aMessage, 
                          TInt aPanic ) const;

        /**
        * RequestTimeL.
        * Called as a result of the client requesting the time.
        * Starts the 'get time' operation.
        * @param aMessage Message from client.
        */
        void RequestTimeL( const RMessage2& aMessage );

    private: // Data

        /**
        * iWaitingForTick, state of tick.
        */
        TBool iWaitingForTick;

        /**
        * iMessage, the message exchange data with the server.
        */
        RMessage2 iMessage;

        /**
        * iServer, reference to the server.
        */
        CTimeServer& iServer;
    };

#endif // __CTIMESESSION_H__


// End of File

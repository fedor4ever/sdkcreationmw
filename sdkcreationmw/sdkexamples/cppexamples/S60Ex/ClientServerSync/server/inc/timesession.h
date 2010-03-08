/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __TIMESESSION_H__
#define __TIMESESSION_H__

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
        void PanicClient( const RMessagePtr2& aMessage, TInt aPanic ) const;

        /**
        * RequestTimeL.
        * Handle the result of the client requesting the time.
        * Gets the time and writes it back to the client synchronously.
        * @param aMessage Message from client.
        */
        void RequestTimeL( const RMessage2& aMessage );

    private: // Data

        /**
        * iServer, reference to the server.
        */
        CTimeServer& iServer;
    };

#endif // __TIMESESSION_H__


// End of File

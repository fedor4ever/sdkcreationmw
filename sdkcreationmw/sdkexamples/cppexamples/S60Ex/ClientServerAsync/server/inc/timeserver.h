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



#ifndef __CTIMESERVER_H__
#define __CTIMESERVER_H__

// INCLUDE FILES
#include <e32base.h>
#include "AsyncServer.pan"

// CLASS DECLARATION
/**
* CTimeServer.
*  An instance of class CTimeServer is the main server class
*  for the ClientServerAsync example application.
*/
class CTimeServer : public CServer2, public MBeating
    {
    public : // Constructors and destructors

        /**
        * NewL.
        * Two-phased constructor.
        * @return Pointer to created CTimeServer object.
        */
        static CTimeServer* NewL();

        /**
        * NewLC.
        * Two-phased constructor.
        * @return Pointer to created CTimeServer object.
        */
        static CTimeServer* NewLC();

        /**
        * ~CTimeServer.
        * Destructor.
        */
        virtual ~CTimeServer();

    public: // New functions

        /**
        * ThreadFunction.
        * Main function for the server thread.
        * @param aStarted A semaphore to be flagged when server
        *                 has been started.
        * @return Error code.
        */
        static TInt ThreadFunction( TAny* aStarted );

        /**
        * IncrementSessions.
        * Increments the count of the active sessions for this server.
        */
        void IncrementSessions();

        /**
        * DecrementSessions.
        * Decrements the count of the active sessions for this server.
        * If no more sessions are in use the server terminates.
        */
        void DecrementSessions();

        /**
        * WaitForTickL.
        * Activates the heartbeat.
        */
        void WaitForTickL();

    public: // Functions from base classes

        /**
        * From MBeating, Beat.
        * A clock tick has occured.
        */
        void Beat();

        /**
        * From MBeating, Synchronize.
        * Several clock ticks have occured.
        */
        void Synchronize();

    protected: // Functions from base classes

        /**
        * From CActive, RunError.
        * Processes any errors.
        * @param aError The leave code reported.
        * @result return KErrNone if leave is handled.
        */
        TInt RunError( TInt aError );

    private: // Constructors and destructors

        /**
        * CTimeServer.
        * C++ default constructor.
        * @param aPriority priority for this thread.
        */
        CTimeServer( TInt aPriority );

        /**
        * ConstructL.
        * 2nd phase constructor.
        */
        void ConstructL();

    private: // New methods

        /**
        * PanicClient.
        * Panics the client.
        * @param aMessage The message channel to the client.
        * @param aReason The reason code for the panic.
        */
        static void PanicClient( const RMessage2& aMessage,
                                 TTimeServPanic aReason );

        /**
        * PanicServer.
        * Panics the server.
        * @param aPanic The panic code.
        */
        static void PanicServer( TTimeServPanic aPanic );

        /**
        * ThreadFunctionL.
        * Second stage startup for the server thread.
        */
        static void ThreadFunctionL();

        /**
        * SendTimeToSessions.
        * Informs all the clients that a time change has occured.
        */
        void SendTimeToSessions();

    private: // Functions from base classes

        /**
        * From CServer, NewSessionL.
        * Creates a time server session.
        * @param aVersion The client version.
        * @param aMessage Message from client.
        * @return Pointer to new session.
        */
        CSession2* NewSessionL( const TVersion& aVersion, 
                                const RMessage2& aMessage ) const;

    private: // Data

        /**
        * iSessionCount, the number of session owned by this server.
        */
        TInt iSessionCount;

        /**
        * iHeartbeat, a periodic timer used to update the client.
        * Owned by CTimeServer object.
        */
        CHeartbeat* iHeartbeat;
    };

#endif // __CTIMESERVER_H__

// End of File

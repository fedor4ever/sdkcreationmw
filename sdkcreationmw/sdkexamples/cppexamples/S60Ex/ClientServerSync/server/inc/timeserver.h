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



#ifndef __TIMESERVER_H__
#define __TIMESERVER_H__

// INCLUDE FILES
#include <e32base.h>
#include "SyncServer.pan"

// CLASS DECLARATION
/**
* CTimeServer.
*  An instance of class CTimeServer is the main server class
*  for the ClientServerSync example application
*/
class CTimeServer : public CServer2
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

    public: // New functions

        /**
        * ThreadFunction.
        * Main function for the server thread.
        * @param aNone Not used.
        * @return Error code.
        */
        static TInt ThreadFunction( TAny* aNone );

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
        * @param aReason the reason code for the panic.
        */
        static void PanicServer( TTimeServPanic aReason );

        /**
        * ThreadFunctionL.
        * Second stage startup for the server thread.
        */
        static void ThreadFunctionL();

    private: // Functions from base classes

        /**
        * From CServer, NewSessionL.
        * Create a time server session.
        * @param aVersion The client version.
        * @param aMessage Message from client.
        * @return Pointer to new session.
        */
        CSession2* NewSessionL( const TVersion& aVersion, 
                                const RMessage2& aMessage ) const;

    private: // Data

        /**
        * iSessionCount, the number of open sessions.
        */
        TInt iSessionCount;
    };


#endif // __TIMESERVER_H__


// End of File

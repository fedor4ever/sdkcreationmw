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



// INCLUDE FILES
#include <e32svr.h>
#include <e32math.h>

#include "TimeServer.h"
#include "ClientServerCommon.h"
#include "TimeSession.h"

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CTimeServer::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTimeServer* CTimeServer::NewL()
    {
    CTimeServer* timeServer = CTimeServer::NewLC();
    CleanupStack::Pop( timeServer );
    return timeServer;
    }

// -----------------------------------------------------------------------------
// CTimeServer::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTimeServer* CTimeServer::NewLC()
    {
    CTimeServer* timeServer = new ( ELeave ) CTimeServer( EPriorityNormal );
    CleanupStack::PushL( timeServer );
    timeServer->ConstructL();
    return timeServer;
    }

// -----------------------------------------------------------------------------
// CTimeServer::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTimeServer::ConstructL()
    {
    StartL( KTimeServerName );
    }

// -----------------------------------------------------------------------------
// CTimeServer::CTimeServer()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CTimeServer::CTimeServer( TInt aPriority )
: CServer2( aPriority )
    {
    // Implementation not required
    }

// -----------------------------------------------------------------------------
// CTimeServer::~CTimeServer()
// Destructor.
// -----------------------------------------------------------------------------
//
CTimeServer::~CTimeServer()
    {
    delete iHeartbeat;
    iHeartbeat = NULL;
    }

// -----------------------------------------------------------------------------
// CTimeServer::NewSessionL()
// Creates a time server session.
// -----------------------------------------------------------------------------
//
CSession2* CTimeServer::NewSessionL( const TVersion& aVersion,
                                     const RMessage2& /*aMessage*/ ) const
    {
    // Check we are the right version
    if ( !User::QueryVersionSupported( TVersion( KTimeServMajorVersionNumber,
                                                 KTimeServMinorVersionNumber,
                                                 KTimeServBuildVersionNumber ),
                                       aVersion ) )
        {
        User::Leave( KErrNotSupported );
        }

    // Make new session
    //RThread client = Message().Client();
    return CTimeServerSession::NewL( *const_cast<CTimeServer*> ( this ) );
    }

// -----------------------------------------------------------------------------
// CTimeServer::IncrementSessions()
// Increments the count of the active sessions for this server.
// -----------------------------------------------------------------------------
//
void CTimeServer::IncrementSessions()
    {
    iSessionCount++;
    }

// -----------------------------------------------------------------------------
// CTimeServer::DecrementSessions()
// Decrements the count of the active sessions for this server.
// -----------------------------------------------------------------------------
//
void CTimeServer::DecrementSessions()
    {
    iSessionCount--;
    if ( iSessionCount <= 0 )
        {
        CActiveScheduler::Stop();
        }
    }

// -----------------------------------------------------------------------------
// CTimeServer::RunError()
// Processes any errors.
// -----------------------------------------------------------------------------
//
TInt CTimeServer::RunError( TInt aError )
    {
    if ( aError == KErrBadDescriptor )
        {
        // A bad descriptor error implies a badly programmed client,
        // so panic it; otherwise report the error to the client
        PanicClient( Message(), EBadDescriptor );
        }
    else
        {
        Message().Complete( aError );
        }

    // The leave will result in an early return from CServer::RunL(), skipping
    // the call to request another message. So do that now in order to keep the
    // server running.
    ReStart();

    return KErrNone;    // Handled the error fully
    }

// -----------------------------------------------------------------------------
// CTimeServer::PanicClient()
// Panics the client.
// -----------------------------------------------------------------------------
//
void CTimeServer::PanicClient( const RMessage2& aMessage, TTimeServPanic aPanic )
    {
    aMessage.Panic( KCSAsyncServer, aPanic );
    }

// -----------------------------------------------------------------------------
// CTimeServer::PanicServer()
// Panics the server.
// -----------------------------------------------------------------------------
//
void CTimeServer::PanicServer( TTimeServPanic aPanic )
    {
    User::Panic( KCSAsyncServer, aPanic );
    }

// -----------------------------------------------------------------------------
// CTimeServer::WaitForTickL()
// Activates the heartbeat.
// -----------------------------------------------------------------------------
//
void CTimeServer::WaitForTickL()
    {
    if ( !iHeartbeat )
        {
        iHeartbeat = CHeartbeat::NewL( EPriorityHigh );
        iHeartbeat->Start( ETwelveOClock, this );
        }
    }

// -----------------------------------------------------------------------------
// CTimeServer::Beat()
// A clock tick has occured.
// -----------------------------------------------------------------------------
//
void CTimeServer::Beat()
    {
    SendTimeToSessions();
    }

// -----------------------------------------------------------------------------
// CTimeServer::Synchronize()
// Several clock ticks have occured.
// -----------------------------------------------------------------------------
//
void CTimeServer::Synchronize()
    {
    SendTimeToSessions();
    }

// -----------------------------------------------------------------------------
// CTimeServer::SendTimeToSessions()
// Informs all the clients that a time change has occured.
// -----------------------------------------------------------------------------
//
void CTimeServer::SendTimeToSessions()
    {
    iSessionIter.SetToFirst();
    CTimeServerSession* session;
    session = reinterpret_cast<CTimeServerSession*>( iSessionIter++ );
    while ( session )
        {
        session->SendTimeToClient();
        session = reinterpret_cast<CTimeServerSession*>( iSessionIter++ );
        }
    }

// -----------------------------------------------------------------------------
// CTimeServer::ThreadFunctionL()
// Second stage startup for the server thread.
// -----------------------------------------------------------------------------
//
void CTimeServer::ThreadFunctionL()
    {
    // Construct active scheduler
    CActiveScheduler* activeScheduler = new ( ELeave ) CActiveScheduler;
    CleanupStack::PushL( activeScheduler );

    // Install active scheduler
    // We don't need to check whether an active scheduler is already installed
    // as this is a new thread, so there won't be one
    CActiveScheduler::Install( activeScheduler );

    // Construct our server
    CTimeServer::NewLC();    // Anonymous

    RSemaphore semaphore;
    User::LeaveIfError( semaphore.OpenGlobal( KTimeServerSemaphoreName ) );

    // Semaphore opened ok
    semaphore.Signal();
    semaphore.Close();

    // Start handling requests
    CActiveScheduler::Start();

    CleanupStack::PopAndDestroy( 2, activeScheduler ); //Anonymous CTimeServer
    }

// -----------------------------------------------------------------------------
// CTimeServer::ThreadFunction()
// Main function for the server thread.
// -----------------------------------------------------------------------------
//
TInt CTimeServer::ThreadFunction( TAny* /*aNone*/ )
    {
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if ( !( cleanupStack ) )
        {
        PanicServer( ECreateTrapCleanup );
        }

    TRAPD( err, ThreadFunctionL() );
    if ( err != KErrNone )
        {
        PanicServer( ESrvCreateServer );
        }

    delete cleanupStack;
    cleanupStack = NULL;

    return KErrNone;
    }


// -----------------------------------------------------------------------------
// E32Main()
// Provides the API for the operating system to start the executable.
// Returns the address of the function to be called.
// -----------------------------------------------------------------------------
//
TInt E32Main()
    {
    return CTimeServer::ThreadFunction( NULL );
    }



// End of File

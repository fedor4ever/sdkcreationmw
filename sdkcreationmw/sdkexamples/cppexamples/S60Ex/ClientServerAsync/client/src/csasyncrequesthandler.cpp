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

#include "CSAsync.pan"
#include "CSAsyncRequestHandler.h"
#include "ClientServerCommon.h"
#include "AsyncTimeObserver.h"

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CCSAsyncRequestHandler::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
CCSAsyncRequestHandler* CCSAsyncRequestHandler::NewL(
                                               MAsyncTimeObserver& aObserver )
    {
    CCSAsyncRequestHandler* self = NewLC( aObserver );
    CleanupStack::Pop( self );
    return( self ) ;
    }

// -----------------------------------------------------------------------------
// CCSAsyncRequestHandler::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
CCSAsyncRequestHandler* CCSAsyncRequestHandler::NewLC(
                                               MAsyncTimeObserver& aObserver )
    {
    CCSAsyncRequestHandler* self =
        new ( ELeave ) CCSAsyncRequestHandler( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CCSAsyncRequestHandler::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CCSAsyncRequestHandler::ConstructL()
    {
    User::LeaveIfError( iSession.Connect() );
    }

// -----------------------------------------------------------------------------
// CCSAsyncRequestHandler::CCSAsyncRequestHandler()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
CCSAsyncRequestHandler::CCSAsyncRequestHandler( MAsyncTimeObserver& aObserver )
: CActive( EPriorityStandard ), iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CCSAsyncRequestHandler::~CCSAsyncRequestHandler()
// Destructor.
// -----------------------------------------------------------------------------
//
CCSAsyncRequestHandler::~CCSAsyncRequestHandler()
    {
    Cancel(); // Causes call to DoCancel()
    iSession.Close();
    }

// -----------------------------------------------------------------------------
// CCSAsyncRequestHandler::RequestTime()
// Sends a request to the server for an update to the time.
// -----------------------------------------------------------------------------
//
void CCSAsyncRequestHandler::RequestTime()
    {
    if ( !IsActive() )
        {
        iSession.RequestTime( iTime, iStatus );
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CCSAsyncRequestHandler::CancelRequest()
// Cancels an outstanding request.
// -----------------------------------------------------------------------------
//
void CCSAsyncRequestHandler::CancelRequest()
    {
    Cancel() ; // Causes call to DoCancel()
    }

// -----------------------------------------------------------------------------
// CCSAsyncRequestHandler::RunL()
// Invoked to handle responses from the server.
// -----------------------------------------------------------------------------
//
void CCSAsyncRequestHandler::RunL()
    {
    switch ( iStatus.Int() )
        {
        case ETimeServRequestTimeComplete:
            // The server has completed the request, signalled the client
            // thread and the clients active scheduler runs the active object.
            // Now do something with it
            iObserver.HandleTimeUpdate();
            RequestTime();   // Add this line to make the clock keep ticking
            break ;

        case KErrCancel:
            // The request was canceled
            break ;

        case KErrNotReady:
            // We requested a new time before completing the previous request
        default:
            User::Panic( KCSAsyncClient, ECSAsyncBadState ); // Unexpected error
            break;
        }
    }

// -----------------------------------------------------------------------------
// CCSAsyncRequestHandler::DoCancel()
// Cancels any outstanding operation.
// -----------------------------------------------------------------------------
//
void CCSAsyncRequestHandler::DoCancel()
    {
    iSession.CancelRequestTime();
    }

// -----------------------------------------------------------------------------
// CCSAsyncRequestHandler::Time() const
// Gets a copy of the last time received from the server.
// -----------------------------------------------------------------------------
//
TTime CCSAsyncRequestHandler::Time() const
    {
    return iTime;
    }

// End of File

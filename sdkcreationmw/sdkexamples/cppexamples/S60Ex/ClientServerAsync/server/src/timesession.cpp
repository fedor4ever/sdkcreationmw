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

#include "TimeSession.h"
#include "ClientServerCommon.h"
#include "TimeServer.h"

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CTimeServerSession::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTimeServerSession* CTimeServerSession::NewL( CTimeServer& aServer )
    {
    CTimeServerSession* self = CTimeServerSession::NewLC( aServer );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CTimeServerSession::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTimeServerSession* CTimeServerSession::NewLC( CTimeServer& aServer )
    {
    CTimeServerSession* self = new ( ELeave ) CTimeServerSession( aServer );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CTimeServerSession::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTimeServerSession::ConstructL()
    {
    iServer.IncrementSessions();
    }

// -----------------------------------------------------------------------------
// CTimeServerSession::CTimeServerSession()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CTimeServerSession::CTimeServerSession(  CTimeServer& aServer )
: iServer( aServer )
    {
    // Implementation not required
    }

// -----------------------------------------------------------------------------
// CTimeServerSession::~CTimeServerSession()
// Destructor.
// -----------------------------------------------------------------------------
//
CTimeServerSession::~CTimeServerSession()
    {
    iServer.DecrementSessions();
    }

// -----------------------------------------------------------------------------
// CTimeServerSession::ServiceL()
// Service request from client.
// -----------------------------------------------------------------------------
//
void CTimeServerSession::ServiceL( const RMessage2& aMessage )
    {
    switch ( aMessage.Function() )
        {
        case ETimeServRequestTime:
            RequestTimeL( aMessage );
            break;

        case ETimeServCancelRequestTime:
            if ( iWaitingForTick )
                {
                iMessage.Complete( KErrCancel );
                iWaitingForTick = EFalse;
                }
            aMessage.Complete( KErrNone );
            break;

        default:
            PanicClient( aMessage, EBadRequest );
        }
    }

// -----------------------------------------------------------------------------
// CTimeServerSession::RequestTimeL()
// Called as a result of the client requesting the time.
// -----------------------------------------------------------------------------
//
void CTimeServerSession::RequestTimeL( const RMessage2& aMessage )
    {
    if ( iWaitingForTick )
        {
        // We're already busy
        PanicClient( aMessage, EReqAlreadyPending );
        }
    else
        {
        // Keep a copy of message - for later use
        iMessage = aMessage ;

        // Start the work performed by the server
        // (just a wait for new second in this case)
        iServer.WaitForTickL();
        iWaitingForTick = ETrue;
        }
    }

// -----------------------------------------------------------------------------
// CTimeServerSession::SendTimeToClient()
// Sends current time back to requesting client.
// -----------------------------------------------------------------------------
//
void CTimeServerSession::SendTimeToClient()
    {
    if ( iWaitingForTick )
        {
        iWaitingForTick = EFalse;

        // Get current time to return to client
        TTime time;
        time.HomeTime();

        // Create a descriptor which points to the 'time' structure
        TPtr8 ptr( reinterpret_cast<TUint8*>( &time ), sizeof( time ),
                   sizeof( time ) );

        // Write the 'time' data back to the client        
        TRAPD( res, iMessage.WriteL( 0, ptr, 0 ) );
        
        if ( res != KErrNone )
            {
            PanicClient( iMessage, EBadDescriptor );
            }
        
        iMessage.Complete( ETimeServRequestTimeComplete );
        }
    }

// -----------------------------------------------------------------------------
// CTimeServerSession::PanicClient()
// Causes the client thread to panic.
// -----------------------------------------------------------------------------
//
void CTimeServerSession::PanicClient( const RMessagePtr2& aMessage, 
                                      TInt aPanic ) const
    {
    aMessage.Panic( KCSAsyncServer, aPanic ); // Note: this panics the client thread,
                                     // not server
    }

// End of File

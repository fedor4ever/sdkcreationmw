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
CTimeServerSession::CTimeServerSession( CTimeServer& aServer )
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
        case ETimeServRequestTime :
            RequestTimeL( aMessage );
            break;

        default:
            PanicClient( aMessage, EBadRequest );
            break;
        }
    aMessage.Complete( KErrNone );
    }

// -----------------------------------------------------------------------------
// CTimeServerSession::RequestTimeL()
// Called as a result of the client requesting the time.
// -----------------------------------------------------------------------------
//

void CTimeServerSession::RequestTimeL( const RMessage2& aMessage )
    {    
    TTime time;
    time.HomeTime();

    TPtr8 ptr( reinterpret_cast<TUint8*>( &time ), sizeof( time ),
               sizeof( time ) );
    
    // Write time data to the descriptor which is the first message argument
    aMessage.WriteL( 0, ptr, 0 );    
    }


// -----------------------------------------------------------------------------
// CTimeServerSession::PanicClient()
// Causes the client thread to panic.
// -----------------------------------------------------------------------------
//
void CTimeServerSession::PanicClient( const RMessagePtr2& aMessage, 
                                      TInt aPanic ) const
    {
    aMessage.Panic( KCSSyncServer,aPanic ); // Note: this panics the client thread,
                                   // not server
    }

// End of File

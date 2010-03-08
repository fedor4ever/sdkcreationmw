/*
* Copyright (c) 2004 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include <w32std.h>
#include <e32cons.h>
#include <in_sock.h>

#include "servertimeoutnotify.h"
#include "servertimeouttimer.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTimeOutTimer::CTimeOutTimer()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CTimeOutTimer::CTimeOutTimer( const TInt aPriority, MTimeoutNotify&
               aTimeOutNotify ) : CTimer( aPriority ), iNotify( aTimeOutNotify )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CTimeOutTimer::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTimeOutTimer* CTimeOutTimer::NewL(const TInt aPriority, MTimeoutNotify&
                                                                 aTimeOutNotify)
    {
    CTimeOutTimer* self = CTimeOutTimer::NewLC( aPriority, aTimeOutNotify );
    CleanupStack::Pop(); // self
    return self;
    }

// -----------------------------------------------------------------------------
// CTimeOutTimer::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CTimeOutTimer* CTimeOutTimer::NewLC( const TInt aPriority, MTimeoutNotify&
                                                                aTimeOutNotify )
    {
    CTimeOutTimer *self = new ( ELeave ) CTimeOutTimer( aPriority,
                                                               aTimeOutNotify );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CTimeOutTimer::~CTimeOutTimer()
// Destructor.
// -----------------------------------------------------------------------------
//
CTimeOutTimer::~CTimeOutTimer()
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CTimeOutTimer::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CTimeOutTimer::ConstructL()
    {
    // Call the base class ConstructL
    CTimer::ConstructL();

    // Add this timer to the active scheduler
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CTimeOutTimer::RunL()
// When the timer times out notify the object registered with this timer.
// -----------------------------------------------------------------------------
//
void CTimeOutTimer::RunL()
    {
    iNotify.TimedOut();
    }

// End of File

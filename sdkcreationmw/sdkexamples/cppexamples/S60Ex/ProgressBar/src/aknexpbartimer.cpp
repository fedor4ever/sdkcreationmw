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
#include "AknExPbarTimerModel.h"
#include "AknExPbarTimer.h"
#include "AknExPbarcontainer.h"


// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CAknExPbarTimer::CAknExPbarTimer()
// C++ default constructor can NOT contain any code, that might leave. 
// -----------------------------------------------------------------------------
//
CAknExPbarTimer::CAknExPbarTimer() : CTimer( EActivePriorityClockTimer )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CAknExPbarTimer::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAknExPbarTimer* CAknExPbarTimer::NewL( TAknExPbarTimerModel* aModel,
                                        CAknExPbarContainer *aContainer )
    {
    CAknExPbarTimer* self = NewLC( aModel, aContainer );
    CleanupStack::Pop( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// CAknExPbarTimer::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAknExPbarTimer* CAknExPbarTimer::NewLC( TAknExPbarTimerModel* aModel, 
                                         CAknExPbarContainer *aContainer )
    {
    CAknExPbarTimer* self = new ( ELeave ) CAknExPbarTimer();
    CleanupStack::PushL( self );
    self->ConstructL( aModel, aContainer );
    return self;
    }

// -----------------------------------------------------------------------------
// CAknExPbarTimer::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAknExPbarTimer::ConstructL( TAknExPbarTimerModel* aModel,
                                  CAknExPbarContainer *aContainer )
    {

    CTimer::ConstructL();

    // Add this Timer to  Active Scheduler
    CActiveScheduler::Add( this );

    iModel = aModel;
    iContainer = aContainer;
    if ( iModel->iRunning )
        {
        Queue();
        }
    }

// -----------------------------------------------------------------------------
// CAknExPbarTimer::~CAknExPbarTimer()
// Destructor.
// -----------------------------------------------------------------------------
//
CAknExPbarTimer::~CAknExPbarTimer()
    {
    // No implementation required
    }


// -----------------------------------------------------------------------------
// CAknExPbarTimer::RunL()
// What to do, when timer event occurred.
// -----------------------------------------------------------------------------
//
void CAknExPbarTimer::RunL()
    {
    iContainer->IncrementBarsAndDraw( iModel->iIncrement );
    Queue();
    }

// ---------------------------------------------------------
// CAknExPbarTimer::Queue()
// Set timer event interval
// ---------------------------------------------------------
//
void CAknExPbarTimer::Queue()
    {
    After( KIntervalBaseTime * iModel->iHundredths );
    }

// End of File

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


//  INCLUDES
#include <eikenv.h>
#include <e32base.h>
#include <coemain.h>
#include <AknGlobalNote.h>
#include <aknnotewrappers.h>
#include <AknExQuery.rsg>
#include "AknExQuery.hrh"
#include "AknExQueryContainer.h"

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CAknExQueryGlobalListObserver::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CAknExQueryGlobalListObserver* 
        CAknExQueryGlobalListObserver::NewL( CAknExQueryContainer* aContainer )
    {
    CAknExQueryGlobalListObserver* active =
        new ( ELeave ) CAknExQueryGlobalListObserver( aContainer );
    CleanupStack::PushL( active );
    active->ConstructL();
    CleanupStack::Pop();
    return active;
    }


// ----------------------------------------------------------------------------
// CAknExQueryGlobalListObserver::~CAknExQueryGlobalListObserver()
// Destructor.
// ----------------------------------------------------------------------------
//
CAknExQueryGlobalListObserver::~CAknExQueryGlobalListObserver()
    {
    Cancel();
    }

// ----------------------------------------------------------------------------
// CAknExQueryGlobalListObserver::Start()
// Start the activator.
// ( other items were commented in a header ).
// ----------------------------------------------------------------------------
//
void CAknExQueryGlobalListObserver::Start()
    {
    SetActive();
    }

// ----------------------------------------------------------------------------
// CAknExQueryGlobalListObserver::CAknExQueryGlobalListObserver()
// C++ default constructor.
// ----------------------------------------------------------------------------
//
CAknExQueryGlobalListObserver::CAknExQueryGlobalListObserver( 
                                            CAknExQueryContainer* aContainer ) 
: CActive( EActivePriorityDefault ), iContainer( aContainer )
    {
    }

// ----------------------------------------------------------------------------
// CAknExQueryGlobalListObserver::ConstructL()
// Symbian default constructor can leave.
// ----------------------------------------------------------------------------
//
void CAknExQueryGlobalListObserver::ConstructL()
    {
    CActiveScheduler::Add( this );
    }

// ----------------------------------------------------------------------------
// CAknExQueryGlobalListObserver::RunL()
// Handle an event.
// ----------------------------------------------------------------------------
//
void CAknExQueryGlobalListObserver::RunL()
    {
    if ( iSelectItemFlag )
        {
        TBuf<80> msg( NULL );
        CEikonEnv::Static()->ReadResource( msg, R_AKNEXQUERY_RECEIVED_MESSAGE );
        msg.AppendNum( iStatus.Int() );
        CAknQueryDialog* dlg = new ( ELeave ) CAknQueryDialog( 
                                                    msg, 
                                                    CAknQueryDialog::ENoTone );
        dlg->ExecuteLD( R_AKNEXQUERY_CONFIRMATION_QUERY2 );
        }
        
    Cancel();
    }

// ----------------------------------------------------------------------------
// CAknExQueryGlobalListObserver::DoCancel()
// Implement the Cancel protocol.
// ----------------------------------------------------------------------------
//
void CAknExQueryGlobalListObserver::DoCancel()
    {
    }

// ----------------------------------------------------------------------------
// CAknExQueryGlobalListObserver::SetSelectItemFlag()
// Set iSelectItemFlag.
// ----------------------------------------------------------------------------
//
void CAknExQueryGlobalListObserver::SetSelectItemFlag()
    {
    iSelectItemFlag = ETrue;
    }

// ----------------------------------------------------------------------------
// CAknExQueryGlobalListObserver::SetSelectItemFlag()
// Set iSelectItemFlag.
// ----------------------------------------------------------------------------
//
void CAknExQueryGlobalListObserver::UnsetSelectItemFlag()
    {
    iSelectItemFlag = EFalse;
    }

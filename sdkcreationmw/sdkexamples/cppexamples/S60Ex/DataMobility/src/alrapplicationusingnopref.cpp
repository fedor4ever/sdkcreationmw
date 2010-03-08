/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CALRApplicationUsingNoPref implementation.
*
*/


// INCLUDE FILES
#include <e32cons.h>
#include <stringloader.h>
#include <datamobility.rsg>
#include "alrapplicationusingnopref.h"
#include "datamobilityprinter.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CALRApplicationUsingNoPref::NewL
// -----------------------------------------------------------------------------
//
CALRApplicationUsingNoPref* CALRApplicationUsingNoPref::NewL( MDataMobilityPrinter& aObserver )
    {
    CALRApplicationUsingNoPref* self =
        new ( ELeave ) CALRApplicationUsingNoPref( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CALRApplicationUsingNoPref::ConstructL
// -----------------------------------------------------------------------------
//
void CALRApplicationUsingNoPref::ConstructL()
    {
    User::LeaveIfError( iSocketServer.Connect() );
    }

// -----------------------------------------------------------------------------
// CALRApplicationUsingNoPref::CALRApplicationUsingNoPref
// -----------------------------------------------------------------------------
//
CALRApplicationUsingNoPref::CALRApplicationUsingNoPref( MDataMobilityPrinter& aObserver )
    : CActive( CActive::EPriorityStandard ),
      iObserver( aObserver ),
      iMobility( NULL )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CALRApplicationUsingNoPref::~CALRApplicationUsingNoPref
// -----------------------------------------------------------------------------
//
CALRApplicationUsingNoPref::~CALRApplicationUsingNoPref()
    {
    delete iMobility;

    Cancel();

    iConnection.Close();

    iSocketServer.Close();
    }

// -----------------------------------------------------------------------------
// CALRApplicationUsingNoPref::StartL
// -----------------------------------------------------------------------------
//
void CALRApplicationUsingNoPref::StartL()
    {
    User::LeaveIfError( iConnection.Open( iSocketServer ) );

    Print( R_OUTPUT_START_CONNECTION_WITH_NO_PREFERENCES );

    iConnection.Start( iStatus );

    SetActive();
    }

// -----------------------------------------------------------------------------
// CALRApplicationUsingNoPref::PreferredCarrierAvailable
// -----------------------------------------------------------------------------
//
void CALRApplicationUsingNoPref::PreferredCarrierAvailable( TAccessPointInfo aOldAPInfo,
                                                 TAccessPointInfo aNewAPInfo,
                                                 TBool aIsUpgrade,
                                                 TBool aIsSeamless )
    {
    Print( R_OUTPUT_PREFERREDCARRIERAVAILABLE );

    Print( R_OUTPUT_OLD_IAP_ID, aOldAPInfo.AccessPoint() );
    
    Print( R_OUTPUT_NEW_IAP_ID, aNewAPInfo.AccessPoint() );

    if ( aIsUpgrade )
        {
        Print( R_OUTPUT_IT_IS_UPGRADE );
        }
    else
        {
        Print( R_OUTPUT_IT_IS_NOT_UPGRADE );
        }

    if ( aIsSeamless )
        {
        // in S60 3.2, this situation cannot occur.
        Print( R_OUTPUT_IT_IS_SEAMLESS );
        }
    else
        {
        Print( R_OUTPUT_IT_IS_NOT_SEAMLESS );
        // Sockets have to be closed at this point.
        Print( R_OUTPUT_WE_ASK_TO_MIGRATE );

        iMobility->MigrateToPreferredCarrier();
        }

    }

// -----------------------------------------------------------------------------
// CALRApplicationUsingNoPref::NewCarrierActive
// -----------------------------------------------------------------------------
//
void CALRApplicationUsingNoPref::NewCarrierActive( TAccessPointInfo aNewAPInfo,
                                                   TBool aIsSeamless )
    {
    Print( R_OUTPUT_NEWCARRIERACTIVE_IAP_ID, aNewAPInfo.AccessPoint() );

    if ( aIsSeamless )
        {
        // in S60 3.2, this situation cannot occur.
        Print( R_OUTPUT_IT_IS_SEAMLESS );
        }
    else
        {
        Print( R_OUTPUT_IT_IS_NOT_SEAMLESS );
        // Sockets have to be re-opened and check they can connect
        // to their server at this point.
        Print( R_OUTPUT_WE_ACCEPT_IT );

        iMobility->NewCarrierAccepted();
        }
    }

// -----------------------------------------------------------------------------
// CALRApplicationUsingNoPref::Error
// -----------------------------------------------------------------------------
//
void CALRApplicationUsingNoPref::Error( TInt aError )
    {
    Print( R_OUTPUT_MOBILITY_ERROR, aError );
    }

// -----------------------------------------------------------------------------
// CALRApplicationUsingNoPref::RunL
// -----------------------------------------------------------------------------
//
void CALRApplicationUsingNoPref::RunL()
    {
    if ( iStatus.Int() == KErrNone )
        {
        TUint32 iap( 0 );
        _LIT( KIAPId, "IAP\\Id" );
        iConnection.GetIntSetting( KIAPId, iap );

        Print( R_OUTPUT_CONNECTION_STARTED_WITH_IAP_ID, iap );

        iMobility = CActiveCommsMobilityApiExt::NewL( iConnection, *this );

        Print( R_OUTPUT_APPLICATION_REGISTERED_FOR_MOBILITY );

        Print( R_OUTPUT_MESSAGE_NOTIFICATIONS );
        }
    else
        {
        Print( R_OUTPUT_RUNL_STATUS_ERROR, iStatus.Int() );
        }
    }

// -----------------------------------------------------------------------------
// CALRApplicationUsingNoPref::DoCancel
// -----------------------------------------------------------------------------
//
void CALRApplicationUsingNoPref::DoCancel()
    {
    iConnection.Close();
    // iConnection.Stop() should be avoided:
    // other client applications' connection should not be terminated.
    }

// -----------------------------------------------------------------------------
// CALRApplicationUsingNoPref::RunError
// -----------------------------------------------------------------------------
//
TInt CALRApplicationUsingNoPref::RunError( TInt aError )
    {
    Print( R_OUTPUT_RUNERROR_ERROR, aError );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CALRApplicationUsingNoPref::Print
// -----------------------------------------------------------------------------
//
void CALRApplicationUsingNoPref::Print( TInt aResourceId )
    {
    // Load a string from the resource file.
    HBufC* message = NULL;
    TRAPD( err, message = StringLoader::LoadL( aResourceId ) );
    if ( err != KErrNone )
        {
        return;
        }

    iObserver.Print( *message );

    delete message;
    }

// -----------------------------------------------------------------------------
// CALRApplicationUsingNoPref::Print
// -----------------------------------------------------------------------------
//
void CALRApplicationUsingNoPref::Print( TInt aResourceId, TInt aInt )
    {
    // Load a string from the resource file.
    HBufC* message = NULL;
    TRAPD( err, message = StringLoader::LoadL( aResourceId, aInt ) );
    if ( err != KErrNone )
        {
        return;
        }

    iObserver.Print( *message );

    delete message;
    }

//  End of File

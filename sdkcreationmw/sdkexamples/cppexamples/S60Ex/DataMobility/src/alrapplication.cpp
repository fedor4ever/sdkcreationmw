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
* Description:  CALRApplication implementation.
*
*/


// INCLUDE FILES
#include <e32cons.h>
#include <stringloader.h>
#include <datamobility.rsg>
#include <connpref.h>
#include "alrapplication.h"
#include "datamobilityprinter.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CALRApplication::NewL
// -----------------------------------------------------------------------------
//
CALRApplication* CALRApplication::NewL( MDataMobilityPrinter& aObserver )
    {
    CALRApplication* self =
        new ( ELeave ) CALRApplication( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CALRApplication::ConstructL
// -----------------------------------------------------------------------------
//
void CALRApplication::ConstructL()
    {
    User::LeaveIfError( iSocketServer.Connect() );
    }

// -----------------------------------------------------------------------------
// CALRApplication::CALRApplication
// -----------------------------------------------------------------------------
//
CALRApplication::CALRApplication( MDataMobilityPrinter& aObserver )
    : CActive( CActive::EPriorityStandard ),
      iObserver( aObserver ),
      iMobility( NULL )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CALRApplication::~CALRApplication
// -----------------------------------------------------------------------------
//
CALRApplication::~CALRApplication()
    {
    delete iMobility;

    Cancel();

    iConnection.Close();

    iSocketServer.Close();
    }

// -----------------------------------------------------------------------------
// CALRApplication::StartL
// -----------------------------------------------------------------------------
//
void CALRApplication::StartL()
    {
    User::LeaveIfError( iConnection.Open( iSocketServer ) );

    TConnSnapPref prefs;
    prefs.SetSnap( 1 ); // assume 1 is a valid SNAP configured in CommsDat

    Print( R_OUTPUT_START_CONNECTION_WITH_PREFERENCES );

    Print( R_OUTPUT_SNAP_ID, prefs.Snap() );

    iConnection.Start( prefs, iStatus );

    SetActive();
    }

// -----------------------------------------------------------------------------
// CALRApplication::PreferredCarrierAvailable
// -----------------------------------------------------------------------------
//
void CALRApplication::PreferredCarrierAvailable( TAccessPointInfo aOldAPInfo,
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
// CALRApplication::NewCarrierActive
// -----------------------------------------------------------------------------
//
void CALRApplication::NewCarrierActive( TAccessPointInfo aNewAPInfo,
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
// CALRApplication::Error
// -----------------------------------------------------------------------------
//
void CALRApplication::Error( TInt aError )
    {
    Print( R_OUTPUT_MOBILITY_ERROR, aError );
    }

// -----------------------------------------------------------------------------
// CALRApplication::RunL
// -----------------------------------------------------------------------------
//
void CALRApplication::RunL()
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
// CALRApplication::DoCancel
// -----------------------------------------------------------------------------
//
void CALRApplication::DoCancel()
    {
    iConnection.Close();
    // iConnection.Stop() should be avoided:
    // other client applications' connection should not be terminated.
    }

// -----------------------------------------------------------------------------
// CALRApplication::RunError
// -----------------------------------------------------------------------------
//
TInt CALRApplication::RunError( TInt aError )
    {
    Print( R_OUTPUT_RUNERROR_ERROR, aError );

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CALRApplication::Print
// -----------------------------------------------------------------------------
//
void CALRApplication::Print( TInt aResourceId )
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
// CALRApplication::Print
// -----------------------------------------------------------------------------
//
void CALRApplication::Print( TInt aResourceId, TInt aInt )
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

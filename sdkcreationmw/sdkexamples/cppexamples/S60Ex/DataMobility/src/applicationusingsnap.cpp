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
* Description:  CApplicationUsingSNAP implementation.
*
*/


// INCLUDE FILES
#include <e32cons.h>
#include <stringloader.h>
#include <datamobility.rsg>
#include <connpref.h>
#include "applicationusingsnap.h"
#include "datamobilityprinter.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CApplicationUsingSNAP::NewL
// -----------------------------------------------------------------------------
//
CApplicationUsingSNAP* CApplicationUsingSNAP::NewL(
    MDataMobilityPrinter& aObserver )
    {
    CApplicationUsingSNAP* self =
        new ( ELeave ) CApplicationUsingSNAP( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CApplicationUsingSNAP::ConstructL
// -----------------------------------------------------------------------------
//
void CApplicationUsingSNAP::ConstructL()
    {
    User::LeaveIfError( iSocketServer.Connect() );
    }

// -----------------------------------------------------------------------------
// CApplicationUsingSNAP::CApplicationUsingSNAP
// -----------------------------------------------------------------------------
//
CApplicationUsingSNAP::CApplicationUsingSNAP( MDataMobilityPrinter& aObserver )
    : CActive( CActive::EPriorityStandard ),
      iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CApplicationUsingSNAP::~CApplicationUsingSNAP
// -----------------------------------------------------------------------------
//
CApplicationUsingSNAP::~CApplicationUsingSNAP()
    {
    Cancel();

    iConnection.Close();

    iSocketServer.Close();
    }

// -----------------------------------------------------------------------------
// CApplicationUsingSNAP::StartL
// -----------------------------------------------------------------------------
//
void CApplicationUsingSNAP::StartL()
    {
    User::LeaveIfError( iConnection.Open( iSocketServer ) );

    TConnSnapPref prefs;
    prefs.SetSnap( 1 ); // assume 1 is a valid SNAP configured in CommsDat

    PrintL( R_OUTPUT_START_CONNECTION_WITH_PREFERENCES );

    PrintL( R_OUTPUT_SNAP_ID, prefs.Snap() );

    iConnection.Start( prefs, iStatus );

    SetActive();
    }

// -----------------------------------------------------------------------------
// CApplicationUsingSNAP::RunL
// -----------------------------------------------------------------------------
//
void CApplicationUsingSNAP::RunL()
    {
    if ( iStatus.Int() == KErrNone )
        {
        TUint32 iap( 0 );
        _LIT( KIAPId, "IAP\\Id" );
        iConnection.GetIntSetting( KIAPId, iap );

        PrintL( R_OUTPUT_CONNECTION_STARTED_WITH_IAP_ID, iap );
        }
    else
        {
        PrintL( R_OUTPUT_RUNL_STATUS_ERROR, iStatus.Int() );
        }
    }

// -----------------------------------------------------------------------------
// CApplicationUsingSNAP::DoCancel
// -----------------------------------------------------------------------------
//
void CApplicationUsingSNAP::DoCancel()
    {
    iConnection.Close();
    // iConnection.Stop() should be avoided:
    // other client applications' connection should not be terminated.
    }

// -----------------------------------------------------------------------------
// CApplicationUsingSNAP::PrintL
// -----------------------------------------------------------------------------
//
void CApplicationUsingSNAP::PrintL( TInt aResourceId )
    {
    // Load a string from the resource file.
    HBufC* message = StringLoader::LoadL( aResourceId );

    iObserver.Print( *message );

    delete message;
    }

// -----------------------------------------------------------------------------
// CApplicationUsingSNAP::PrintL
// -----------------------------------------------------------------------------
//
void CApplicationUsingSNAP::PrintL( TInt aResourceId, TInt aInt )
    {
    // Load a string from the resource file.
    HBufC* message = StringLoader::LoadL( aResourceId, aInt );

    iObserver.Print( *message );

    delete message;
    }

//  End of File

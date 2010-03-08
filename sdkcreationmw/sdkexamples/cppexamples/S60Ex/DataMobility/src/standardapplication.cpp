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
* Description:  CStandardApplication implementation.
*
*/


// INCLUDE FILES
#include <e32cons.h>
#include <stringloader.h>
#include <datamobility.rsg>
#include <CommDbConnPref.h>
#include "standardapplication.h"
#include "datamobilityprinter.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CStandardApplication::NewL
// -----------------------------------------------------------------------------
//
CStandardApplication* CStandardApplication::NewL( MDataMobilityPrinter& aObserver )
    {
    CStandardApplication* self =
        new ( ELeave ) CStandardApplication( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CStandardApplication::ConstructL
// -----------------------------------------------------------------------------
//
void CStandardApplication::ConstructL()
    {
    User::LeaveIfError( iSocketServer.Connect() );
    }

// -----------------------------------------------------------------------------
// CStandardApplication::CStandardApplication
// -----------------------------------------------------------------------------
//
CStandardApplication::CStandardApplication( MDataMobilityPrinter& aObserver )
    : CActive( CActive::EPriorityStandard ),
      iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CStandardApplication::~CStandardApplication
// -----------------------------------------------------------------------------
//
CStandardApplication::~CStandardApplication()
    {
    Cancel();

    iConnection.Close();

    iSocketServer.Close();
    }

// -----------------------------------------------------------------------------
// CStandardApplication::StartL
// -----------------------------------------------------------------------------
//
void CStandardApplication::StartL()
    {
    User::LeaveIfError( iConnection.Open( iSocketServer ) );

    // Let's open a valid access point
    TCommDbConnPref prefs;
    prefs.SetIapId( 1 ); // assume 1 is a valid IAP configured in CommsDat
    prefs.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );

    PrintL( R_OUTPUT_START_CONNECTION_WITH_PREFERENCES );

    PrintL( R_OUTPUT_IAP_ID, prefs.IapId() );

    iConnection.Start( prefs, iStatus );

    SetActive();
    }

// -----------------------------------------------------------------------------
// CStandardApplication::RunL
// -----------------------------------------------------------------------------
//
void CStandardApplication::RunL()
    {
    if ( iStatus.Int() == KErrNone )
        {
        PrintL( R_OUTPUT_CONNECTION_STARTED );
        }
    else
        {
        PrintL( R_OUTPUT_RUNL_STATUS_ERROR, iStatus.Int() );
        }
    }

// -----------------------------------------------------------------------------
// CStandardApplication::DoCancel
// -----------------------------------------------------------------------------
//
void CStandardApplication::DoCancel()
    {
    iConnection.Close();
    // iConnection.Stop() should be avoided:
    // other client applications' connection should not be terminated.
    }

// -----------------------------------------------------------------------------
// CStandardApplication::PrintL
// -----------------------------------------------------------------------------
//
void CStandardApplication::PrintL( TInt aResourceId )
    {
    // Load a string from the resource file.
    HBufC* message = StringLoader::LoadL( aResourceId );

    iObserver.Print( *message );

    delete message;
    }

// -----------------------------------------------------------------------------
// CStandardApplication::PrintL
// -----------------------------------------------------------------------------
//
void CStandardApplication::PrintL( TInt aResourceId, TInt aInt )
    {
    // Load a string from the resource file.
    HBufC* message = StringLoader::LoadL( aResourceId, aInt );

    iObserver.Print( *message );

    delete message;
    }

//  End of File

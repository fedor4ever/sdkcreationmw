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
* Description:  CApplicationTriggeringConnDlg implementation.
*
*/


// INCLUDE FILES
#include <e32cons.h>
#include <stringloader.h>
#include <datamobility.rsg>
#include <CommDbConnPref.h>
#include "applicationtriggeringconndlg.h"
#include "datamobilityprinter.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CApplicationTriggeringConnDlg::NewL
// -----------------------------------------------------------------------------
//
CApplicationTriggeringConnDlg* CApplicationTriggeringConnDlg::NewL(
    MDataMobilityPrinter& aObserver )
    {
    CApplicationTriggeringConnDlg* self =
        new ( ELeave ) CApplicationTriggeringConnDlg( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CApplicationTriggeringConnDlg::ConstructL
// -----------------------------------------------------------------------------
//
void CApplicationTriggeringConnDlg::ConstructL()
    {
    User::LeaveIfError( iSocketServer.Connect() );
    }

// -----------------------------------------------------------------------------
// CApplicationTriggeringConnDlg::CApplicationTriggeringConnDlg
// -----------------------------------------------------------------------------
//
CApplicationTriggeringConnDlg::CApplicationTriggeringConnDlg( MDataMobilityPrinter& aObserver )
    : CActive( CActive::EPriorityStandard ),
      iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CApplicationTriggeringConnDlg::~CApplicationTriggeringConnDlg
// -----------------------------------------------------------------------------
//
CApplicationTriggeringConnDlg::~CApplicationTriggeringConnDlg()
    {
    Cancel();

    iConnection.Close();

    iSocketServer.Close();
    }

// -----------------------------------------------------------------------------
// CApplicationTriggeringConnDlg::StartL
// -----------------------------------------------------------------------------
//
void CApplicationTriggeringConnDlg::StartL()
    {
    User::LeaveIfError( iConnection.Open( iSocketServer ) );

    TCommDbConnPref prefs;
    prefs.SetDialogPreference( ECommDbDialogPrefPrompt );

    PrintL( R_OUTPUT_PROMPT_CONNECTION_DIALOG );

    iConnection.Start( prefs, iStatus );

    SetActive();
    }

// -----------------------------------------------------------------------------
// CApplicationTriggeringConnDlg::RunL
// -----------------------------------------------------------------------------
//
void CApplicationTriggeringConnDlg::RunL()
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
// CApplicationTriggeringConnDlg::DoCancel
// -----------------------------------------------------------------------------
//
void CApplicationTriggeringConnDlg::DoCancel()
    {
    iConnection.Close();
    // iConnection.Stop() should be avoided:
    // other client applications' connection should not be terminated.
    }

// -----------------------------------------------------------------------------
// CApplicationTriggeringConnDlg::PrintL
// -----------------------------------------------------------------------------
//
void CApplicationTriggeringConnDlg::PrintL( TInt aResourceId )
    {
    // Load a string from the resource file.
    HBufC* message = StringLoader::LoadL( aResourceId );

    iObserver.Print( *message );

    delete message;
    }

// -----------------------------------------------------------------------------
// CApplicationTriggeringConnDlg::PrintL
// -----------------------------------------------------------------------------
//
void CApplicationTriggeringConnDlg::PrintL( TInt aResourceId, TInt aInt )
    {
    // Load a string from the resource file.
    HBufC* message = StringLoader::LoadL( aResourceId, aInt );

    iObserver.Print( *message );

    delete message;
    }

//  End of File

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
* Description:  CApplicationUsingUiSettings implementation.
*
*/


// INCLUDE FILES
#include <e32cons.h>
#include <stringloader.h>
#include <datamobility.rsg>
#include <CommDbConnPref.h>
#include "applicationusinguisettings.h"
#include "datamobilityprinter.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CApplicationUsingUiSettings::NewL
// -----------------------------------------------------------------------------
//
CApplicationUsingUiSettings* CApplicationUsingUiSettings::NewL( MDataMobilityPrinter& aObserver )
    {
    CApplicationUsingUiSettings* self =
        new ( ELeave ) CApplicationUsingUiSettings( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CApplicationUsingUiSettings::ConstructL
// -----------------------------------------------------------------------------
//
void CApplicationUsingUiSettings::ConstructL()
    {
    User::LeaveIfError( iSocketServer.Connect() );
    }

// -----------------------------------------------------------------------------
// CApplicationUsingUiSettings::CApplicationUsingUiSettings
// -----------------------------------------------------------------------------
//
CApplicationUsingUiSettings::CApplicationUsingUiSettings( MDataMobilityPrinter& aObserver )
    : CActive( CActive::EPriorityStandard ),
      iObserver( aObserver )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CApplicationUsingUiSettings::~CApplicationUsingUiSettings
// -----------------------------------------------------------------------------
//
CApplicationUsingUiSettings::~CApplicationUsingUiSettings()
    {
    Cancel();

    iConnection.Close();

    iSocketServer.Close();
    }

// -----------------------------------------------------------------------------
// CApplicationUsingUiSettings::StartL
// -----------------------------------------------------------------------------
//
void CApplicationUsingUiSettings::StartL()
    {
    User::LeaveIfError( iConnection.Open( iSocketServer ) );

    TBool selected = ConnectionSettingL();

    if ( !selected )
        {
        PrintL( R_OUTPUT_USER_CANCELLED_THE_SELECTION );
        return;
        }

    switch ( iUserSelection.iResult )
        {
        case CMManager::EDestination:
            {
            TConnSnapPref prefs;
            prefs.SetSnap( iUserSelection.iId );

            PrintL( R_OUTPUT_START_CONNECTION_WITH_PREFERENCES );
        
            PrintL( R_OUTPUT_SNAP_ID, prefs.Snap() );

            iConnection.Start( prefs, iStatus );
            break;
            }
        case CMManager::EConnectionMethod:
            {
            TCommDbConnPref prefs;
            prefs.SetIapId( iUserSelection.iId );
            prefs.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );

            PrintL( R_OUTPUT_START_CONNECTION_WITH_PREFERENCES );
        
            PrintL( R_OUTPUT_IAP_ID, prefs.IapId() );

            iConnection.Start( prefs, iStatus );
            break;
            }
        case CMManager::EDefaultConnection:
            {
            PrintL( R_OUTPUT_ATTEMPT_TO_USE_THE_DFT_CONNECTION );
        
            iConnection.Start( iStatus );
            break;
            }
        default: // CMManager::EAlwaysAsk
            {
            TCommDbConnPref prefs;
            prefs.SetDialogPreference( ECommDbDialogPrefPrompt );

            PrintL( R_OUTPUT_PROMPT_CONNECTION_DIALOG );
        
            iConnection.Start( prefs, iStatus );
            }
        }

    SetActive();
    }

// -----------------------------------------------------------------------------
// CApplicationUsingUiSettings::ConnectionSettingL
// -----------------------------------------------------------------------------
//
TBool CApplicationUsingUiSettings::ConnectionSettingL()
    {
    TBool selected( EFalse );

    CCmApplicationSettingsUi* settings = CCmApplicationSettingsUi::NewL();
    CleanupStack::PushL( settings );

    TUint listedItems = CMManager::EShowAlwaysAsk |
                        CMManager::EShowDefaultConnection |
                        CMManager::EShowDestinations |
                        CMManager::EShowConnectionMethods;

    TBearerFilterArray filter;

    selected = settings->RunApplicationSettingsL( iUserSelection,
                                                  listedItems,
                                                  filter );

    CleanupStack::PopAndDestroy( settings );

    return selected;
    }

// -----------------------------------------------------------------------------
// CApplicationUsingUiSettings::RunL
// -----------------------------------------------------------------------------
//
void CApplicationUsingUiSettings::RunL()
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
// CApplicationUsingUiSettings::DoCancel
// -----------------------------------------------------------------------------
//
void CApplicationUsingUiSettings::DoCancel()
    {
    iConnection.Close();
    // iConnection.Stop() should be avoided:
    // other client applications' connection should not be terminated.
    }

// -----------------------------------------------------------------------------
// CApplicationUsingUiSettings::PrintL
// -----------------------------------------------------------------------------
//
void CApplicationUsingUiSettings::PrintL( TInt aResourceId )
    {
    // Load a string from the resource file.
    HBufC* message = StringLoader::LoadL( aResourceId );

    iObserver.Print( *message );

    delete message;
    }

// -----------------------------------------------------------------------------
// CApplicationUsingUiSettings::PrintL
// -----------------------------------------------------------------------------
//
void CApplicationUsingUiSettings::PrintL( TInt aResourceId, TInt aInt )
    {
    // Load a string from the resource file.
    HBufC* message = StringLoader::LoadL( aResourceId, aInt );

    iObserver.Print( *message );

    delete message;
    }

//  End of File

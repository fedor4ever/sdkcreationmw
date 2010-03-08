/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include <avkon.hrh>
#include <aknnotewrappers.h> 
#include <eikmenup.h>
#include <AknWaitDialog.h> 
#include <WebClient.rsg>
#include "WebClient.pan"
#include "WebClientAppUi.h"
#include "WebClientContainer.h"
#include "WebClient.hrh"

// ----------------------------------------------------------------------------
// CWebClientAppUi::ConstructL()
// Second phase construction.
// ----------------------------------------------------------------------------
//
void CWebClientAppUi::ConstructL()
    {
    BaseConstructL( CAknAppUi::EAknEnableSkin );

    iContainer = CWebClientContainer::NewL( ClientRect() );
    iContainer->SetMopParent( this );
    AddToStackL( iContainer );

    iClient = CWebClientEngine::NewL( *iContainer );
    iClient->SetCallBack( this );
    iWaitDialog = NULL;
    }

// ----------------------------------------------------------------------------
// CWebClientAppUi::CWebClientAppUi()
// First phase construction.
// ----------------------------------------------------------------------------
//
CWebClientAppUi::CWebClientAppUi()
    {
    // no implementation required
    }

// ----------------------------------------------------------------------------
// CWebClientAppUi::~CWebClientAppUi()
// Destructor.
// ----------------------------------------------------------------------------
//
CWebClientAppUi::~CWebClientAppUi()
    {
    if ( iContainer )
        {
        RemoveFromStack( iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    if ( iClient )
        {
        delete iClient;
        iClient = NULL;
        }
    if ( iWaitDialog )
        {
        iWaitDialog->ProcessFinishedL();    // deletes the dialog
        delete iWaitDialog;
        iWaitDialog = NULL;
        }
    }

// ----------------------------------------------------------------------------
// CWebClientAppUi::HandleCommandL()
// Handles user commands.
// ----------------------------------------------------------------------------
//
void CWebClientAppUi::HandleCommandL( TInt aCommand )
    {
    switch( aCommand )
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;
        case EWebClientCmdReqUrl:
            HandleUrlRequestL();
            break;
        case EWebClientCmdHeader:
        case EWebClientCmdSource:
        case EWebClientCmdStatus:
            iContainer->HandleCommandL( aCommand );
            break;
        default:
            break;
        }
    }

// -----------------------------------------------------------------------------
//  Handles user command Request URL.
//  Called from HandleCommandL()
// -----------------------------------------------------------------------------
//
void CWebClientAppUi::HandleUrlRequestL()
    {
    // Issue HTTP get to engine; first cancel possible existing transaction
    iClient->CancelTransactionL();

    // URL Query
    TBuf<KPrefixedUriNameLength> uri;
    CAknTextQueryDialog* dlg = new (ELeave) CAknTextQueryDialog( uri, 
        CAknQueryDialog::ENoTone );

    if ( dlg->ExecuteLD( R_WEBCLIENT_DIALOG_URL_QUERY )) 
        {
        // Insert prefix to URL (it must begin with "http://" or "https://")
        TBuf8<KPrefixedUriNameLength> uri8;
        uri.LowerCase();
        if(uri.Find( KHttpPrefix) == KErrNotFound
            && uri.Find( KHttpsPrefix ) == KErrNotFound )
            {
            // If the URL does not contain http or https, 
            // use the default, "http://"
            uri8.Append( KHttpPrefix8 );
            uri8.Append( uri );
            } else {
            uri8.Copy( uri );
            }

        // Set requested URL to label and clear application view
        uri.Copy(uri8);
        iContainer->ResetL( uri );

        // Start transaction
        iClient->IssueHTTPGetL( uri8 );

        // Create the waitdialog
        iWaitDialog = new ( ELeave )CAknWaitDialog(
            reinterpret_cast<CEikDialog**>( &iWaitDialog ), ETrue );
        // Set DialogDissmissed callback
        iWaitDialog->SetCallback( this ); 
        iWaitDialog->ExecuteLD( R_WEBCLIENT_DOWNLOAD_WAIT_NOTE );
        }
    } 

// ----------------------------------------------------------------------------
// CWebClientAppUi::HandleResourceChangeL(TInt AType)
// Called by framework when layout is changed.
// ----------------------------------------------------------------------------
//
void CWebClientAppUi::HandleResourceChangeL(TInt aType)
    {
    CAknAppUi::HandleResourceChangeL(aType);
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {		
        iContainer->SetRect( ClientRect() );
        }
    }

// -----------------------------------------------------------------------------
//  Called by the framework when the application status pane
//  size is changed.
// -----------------------------------------------------------------------------
//
void CWebClientAppUi::HandleStatusPaneSizeChange()
    {
    // no implementation required
    } 

// ----------------------------------------------------------------------------
// CWebClientAppUi::HandleKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
// Takes care of key event handling
// ----------------------------------------------------------------------------
//
TKeyResponse CWebClientAppUi::HandleKeyEventL( const TKeyEvent& /*aKeyEvent*/,
                                               TEventCode /*aType*/ )
    {
    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------------------------------
// CWebClientAppUi::DialogDismissedL( TInt aButtonId )
// Is called when a dialog is dismissed.
// ----------------------------------------------------------------------------
//
void CWebClientAppUi::DialogDismissedL( TInt aButtonId )
    {
    if ( aButtonId == EAknSoftkeyCancel )
        {
        // Cancel current transaction
        if ( iClient->IsRunning() )
            {
            iClient->CancelTransactionL();
            }
        }
    }

// ----------------------------------------------------------------------------
// CWebClientAppUi::RemoveWaitDialogL()
// Called by CWebClientEngine when Http transaction is finished.
// ----------------------------------------------------------------------------
//
void CWebClientAppUi::RemoveWaitDialogL()
    {
    if ( iWaitDialog )
        {
        iWaitDialog->ProcessFinishedL();    // deletes the dialog
        }
    }

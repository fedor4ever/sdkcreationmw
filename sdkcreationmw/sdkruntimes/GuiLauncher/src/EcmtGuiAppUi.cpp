/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#include <avkon.hrh>
#include <eikmenup.h>
#include <stringloader.h>
#include "EcmtGuiDocument.h"
#include "EcmtGui.pan"
#include "EcmtGuiAppUi.h"
#include "EcmtGuiLogView.h"
#include "EcmtWlanOddSettingView.h"


#if !defined(__WINS__) || defined(_ENABLE_SETINGS_ON_WINS_)
#include "EcmtGuiSettingsView.h"
#endif 

#include "EcmtGuiUid.h"
#include "EcmtGui.hrh"
#include <EcmtGui.rsg>
// ===========================================================================
void CEcmtGuiAppUi::ConstructL()
	{
    BaseConstructL(EAknEnableSkin);
        
    iLogView = CEcmtGuiLogView::NewL();
    AddViewL(iLogView);
    
    iWlanOdd =CWlanOddSettingView::NewL();
    AddViewL(iWlanOdd);
#if !defined(__WINS__) || defined(_ENABLE_SETINGS_ON_WINS_)
    iSettingsView = CEcmtGuiSettingsView::NewL();
    AddViewL(iSettingsView);
#endif        

    SetDefaultViewL(*iLogView);
	}

// ===========================================================================
CEcmtGuiAppUi::CEcmtGuiAppUi()
// ===========================================================================
    {
    }

// ===========================================================================
CEcmtGuiAppUi::~CEcmtGuiAppUi()
    {
    if (iLogView)   
        iLogView = NULL;
    if (iWlanOdd)   
        iWlanOdd = NULL;
#if !defined(__WINS__) || defined(_ENABLE_SETINGS_ON_WINS_)
    if ( iSettingsView )
        iSettingsView = NULL;
#endif    
    }

// ===========================================================================
void CEcmtGuiAppUi::HandleResourceChangeL(TInt aType)
// ===========================================================================
    {
    CAknAppUi::HandleResourceChangeL(aType);
    if (aType == KEikDynamicLayoutVariantSwitch)
        {
        iLogView->HandleSizeChange( aType );
        #if !defined(__WINS__) || defined(_ENABLE_SETINGS_ON_WINS_)
        iSettingsView->HandleSizeChange( aType );
        #endif
        }   
    }

// ===========================================================================
void CEcmtGuiAppUi::ConnectEcmtL()
// ===========================================================================
{
    TInt retval = iEcmt.Connect();
    if ( retval == KErrNone )
    {
        iLogView->SetConnected( ETrue );
        iLogView->LogMessageL( R_SERVER_RUNNING );
        iEcmt.SetServerObserverL( KUidEcmtGuiApp, this );
        
    }
    else
    {
        TBuf<KMaxLogMsgLen> status;
        StringLoader::Load( status, R_SERVER_START_ERROR );
        status.AppendNum( retval );
        iLogView->LogMessageL( status );
    }
}

// ===========================================================================
void CEcmtGuiAppUi::DisconnectEcmt()
// ===========================================================================
{
    iEcmt.Close();
    iLogView->SetConnected( EFalse );
    iLogView->LogMessageL( R_SERVER_STOPPED );
}

// ===========================================================================
void CEcmtGuiAppUi::HandleCommandL(TInt aCommand)
// ===========================================================================
{
    switch(aCommand)
    {
    case EEikCmdExit:
    case EAknSoftkeyExit:
        if ( iLogView->IsConnected() )
        {
            iEcmt.Close();
        }
        Exit();
        break;

    case EEcmtGuiCmdStart:
    case EEcmtGuiCmdStop:
        if ( !iLogView->IsConnected() )
        {
            ConnectEcmtL();
        }
        else
        {
            DisconnectEcmt();
        }
        break;
        
    case EEcmtGuiCmdWlanOdd:
    	{
    	    TVwsViewId aViewID = iWlanOdd->ViewId();
    	    ActivateViewL(aViewID);
        }
        break;
        
    case EEcmtGuiCmdClearLog:
	      iLogView->ClearLogL();
        break;

    default:
        Panic(EEcmtGuiUi);
        break;
    }
}



// ===========================================================================
void CEcmtGuiAppUi::HandleEcmtServerNotification(const TDesC& aMsg,TInt aError)
    {
    // Turn on the lights when something happens
    User::ResetInactivityTime();

    // Message logging to screen should never leave unless we're totally out
    // of memory, and in that case any attempt to show error notes will fail
    // too so might as well trap and ignore the leave.

    if (aError != KErrNone)
        {
        TBuf<KMaxLogMsgLen> status;
        StringLoader::Load(status, R_ERROR_GETTING_NOTIFICATION);
        status.AppendNum(aError);
        TRAP_IGNORE( iLogView->LogMessageL(status) );
        }
    else
        {
        TRAP_IGNORE( iLogView->LogMessageL( aMsg ) );
        }
    }




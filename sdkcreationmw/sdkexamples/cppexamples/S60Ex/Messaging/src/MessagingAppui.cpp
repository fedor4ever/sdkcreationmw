/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include <sendui.h>
#include <SendUiConsts.h>
#include <CMessageData.h> 
#include <Messaging.rsg>
#include "Messaging.hrh"
#include "MessagingAppui.h"
#include "MessagingContainer.h" 

// ================= MEMBER FUNCTIONS ==========================================

// -----------------------------------------------------------------------------
// CMessagingAppUi::ConstructL()
// Symbian two-phased constructor
// -----------------------------------------------------------------------------
//
void CMessagingAppUi::ConstructL()
    {
    BaseConstructL( CAknAppUi::EAknEnableSkin );
    iAppContainer = CMessagingContainer::NewL( ClientRect() );
    iAppContainer->SetMopParent( this );
    AddToStackL( iAppContainer );    
    iSendUi = CSendUi::NewL();    
    }

// -----------------------------------------------------------------------------
// CMessagingAppUi::~CMessagingAppUi()
// Destructor
// Frees reserved resources
// -----------------------------------------------------------------------------
//
CMessagingAppUi::~CMessagingAppUi()
    {
    if ( iAppContainer )
        {
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
		iAppContainer = NULL;
        }
    if ( iSendUi )
	    {
	    delete iSendUi;
	    iSendUi = NULL;
	    }
    }

// -----------------------------------------------------------------------------
// CMessagingAppUi::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// -----------------------------------------------------------------------------
//
void CMessagingAppUi::DynInitMenuPaneL( TInt, CEikMenuPane* )
    {
    // no implementation needed here
    }

// -----------------------------------------------------------------------------
// CMessagingAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
// takes care of key event handling
// -----------------------------------------------------------------------------
//
TKeyResponse CMessagingAppUi::HandleKeyEventL( const TKeyEvent&, TEventCode )
    {
    return EKeyWasNotConsumed;
    }

// -----------------------------------------------------------------------------
// CMessagingAppUi::HandleCommandL(TInt aCommand)
// takes care of command handling
// -----------------------------------------------------------------------------
//
void CMessagingAppUi::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EAknSoftkeyExit:
        case EEikCmdExit:
            {
            Exit();
            break;
            }        
        case EMessagingCmdCreateSMS:
        	{
        	// create and send SMS
        	CreateAndSendMessageL( KSenduiMtmSmsUid );
        	break;	
        	}
        case EMessagingCmdCreateMMS:
        	{
        	// create and send MMS
        	CreateAndSendMessageL( KSenduiMtmMmsUid );
        	break;	
        	}
        case EMessagingCmdCreateEmail:
        	{
        	// create and send email
        	CreateAndSendMessageL( KSenduiMtmSmtpUid );	
        	break;	
        	}       
        default:
            break;      
        }
    }


// -----------------------------------------------------------------------------
// CMessagingAppUi::CreateAndSendMessageL(const TUid aServiceUid)
// Starts message editor via CSendUi class. 
// Message type is defined in parameter aServiceUid.
// -----------------------------------------------------------------------------
//
void CMessagingAppUi::CreateAndSendMessageL( const TUid aServiceUid )
	{
	// create empty message
	CMessageData*  message = CMessageData::NewLC(); 
	// start message editor through SendUI
	iSendUi->CreateAndSendMessageL( aServiceUid, message, KNullUid );
	CleanupStack::PopAndDestroy( message );
	}

// -----------------------------------------------------------------------------
// CMessagingAppUi::HandleResourceChangeL( TInt aType )
// takes care of command handling
// -----------------------------------------------------------------------------
//
void CMessagingAppUi::HandleResourceChangeL( TInt aType )
    {
    CAknAppUi::HandleResourceChangeL( aType );
    if ( aType==KEikDynamicLayoutVariantSwitch )
        {
        iAppContainer->SetRect( ClientRect() );
        }
    }
// End of File  
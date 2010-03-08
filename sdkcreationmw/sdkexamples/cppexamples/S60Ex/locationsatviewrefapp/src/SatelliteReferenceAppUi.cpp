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
#include "SatelliteReferenceAppUi.h"
#include "SatelliteReferenceContainer.h" 
#include <SatelliteReference.rsg>
#include "SatelliteReference.hrh"
#include <StringLoader.h>

#include <avkon.hrh>

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CSatelliteReferenceAppUi::ConstructL()
// ----------------------------------------------------------
//
void CSatelliteReferenceAppUi::ConstructL()
    {
    //we have to enable MSK for 3.2
    BaseConstructL( EAknEnableSkin | EAknEnableMSK );
    iAppContainer = new (ELeave) CSatelliteReferenceContainer;
    iAppContainer->SetMopParent(this);
    iAppContainer->ConstructL( ClientRect(), this);
    AddToStackL( iAppContainer );
    }

// ----------------------------------------------------
// CSatelliteReferenceAppUi::~CSatelliteReferenceAppUi()
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CSatelliteReferenceAppUi::~CSatelliteReferenceAppUi()
    {
    if (iAppContainer)
        {
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
        }
   }

// ------------------------------------------------------------------------------
//  CSatelliteReferenceAppUi::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CSatelliteReferenceAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {
    }

// ----------------------------------------------------
// CSatelliteReferenceAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
// ----------------------------------------------------
//
TKeyResponse CSatelliteReferenceAppUi::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/,TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }
    
// ------------------------------------------------------------------
// CSatelliteReferenceAppUi::HandleForegroundEventL(TBool aForeground)
// ------------------------------------------------------------------
//
void CSatelliteReferenceAppUi::HandleForegroundEventL(TBool aForeground)
	{
	if(iSatelliteInfo)	
		{
		iSatelliteInfo->HandleForegroundEventL(aForeground);			
		}
	}

// ----------------------------------------------------
// CSatelliteReferenceAppUi::HandleCommandL(TInt aCommand)
// ----------------------------------------------------
//
void CSatelliteReferenceAppUi::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
        case EAknSoftkeyExit:
        case EEikCmdExit:
            {
            Exit();
            break;
            }
            
        case ESatelliteInfoCmd:
	        {
	        HBufC* srvName = StringLoader::LoadLC( 
	                              R_SATREF_SERVICE_RULE_NAME, 
	                              CEikonEnv::Static() );
	        // Creating instance of CSatelliteInfo
	    	iSatelliteInfo = CSatelliteInfoUI::NewL();
	    	
	    	//Setting the default satellite view as Firmament view 
	    	iSatelliteInfo->SetLaunchView(CSatelliteInfoUI::ESatelliteFirmamentView);
	    	
	    	// ExecuteLD displays the satellite dialog.
	    	// The dialog display satellite ID's along with 
	    	// their signal strengths depicted using bar's.
	    	iSatelliteInfo->ExecuteLD(*srvName);
	    	
	    	CleanupStack::PopAndDestroy(srvName); //srvName
	    	iSatelliteInfo = NULL;
	    	break;
	        }        
	        	
        default:
	        {
	        break;
	        }            
        }
    }

// End of File  

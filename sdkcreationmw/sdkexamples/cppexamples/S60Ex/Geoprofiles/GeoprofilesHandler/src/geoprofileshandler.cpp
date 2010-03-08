/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Framework Function definitions for Geo-Profiles.
*
*/


// System Includes
#include <e32base.h>        // Basic framework functions
#include <lbtcommon.h>
#include <lbtserver.h> 
#include <lbt.h>
#include <lbttriggerinfo.h>
#include <lbtstartuptrigger.h>
#include <s32mem.h>
#include <ProEngFactory.h>
#include <mproengengine.h>
#include <mproengprofilenamearray.h> 

// User Includes
#include "geoprofileshandler.h"

// Constant Definitions
_LIT( KTxtApplication, "EvtHandlerUI" );

// ----------------------------------------------------------------------------
// GLDEF_C	 TInt E32Main()
// ----------------------------------------------------------------------------
//
GLDEF_C	 TInt E32Main()
	{
	// __UHEAP_MARK;
	// Cretae a Cleanup stack
	CTrapCleanup* cleanup = CTrapCleanup::New();
	
	// Call the initialization implementation
    TRAPD( error, callImplementationL());
        
	__ASSERT_ALWAYS( !error, User::Panic( KTxtApplication, error ));
	
	// Destroy the cleanup stack handle
	delete cleanup;
	// __UHEAP_MARKEND;
	return KErrNone;
	}

// ----------------------------------------------------------------------------
// LOCAL_C void callImplementationL()
// ----------------------------------------------------------------------------
//
LOCAL_C void callImplementationL()
    {
    // Create and Install Active Scheduler
	CActiveScheduler* scheduler = new( ELeave ) CActiveScheduler;
	User::LeaveIfNull( scheduler );    	
	CleanupStack::PushL( scheduler );
	CActiveScheduler::Install( scheduler );
		 
	TRAP_IGNORE( HandleEventL());
		    	
	// Delete active scheduler
   	CleanupStack::PopAndDestroy( scheduler );	    
    }

// ----------------------------------------------------------------------------
// void HandleEventL()
// ----------------------------------------------------------------------------
//
void HandleEventL()
	{    		
	TInt profileId = GetProfileIdofFiredTriggerL();
	ActivateProfileL( profileId );	
    }

// ----------------------------------------------------------------------------
// TInt GetProfileIdofFiredTriggerL
// ----------------------------------------------------------------------------
//
TInt GetProfileIdofFiredTriggerL()
    {
    TInt profileId = KErrNotFound;
    
	// Open LBT server session
	RLbtServer      server;
    server.Connect();
    CleanupClosePushL( server );
    
    // Open LBT handle
    RLbt lbt;
    lbt.Open( server );
    CleanupClosePushL( lbt );
    
    // Obtain the list of fired triggers
    RArray < TLbtTriggerFireInfo >  triggerInfoList;    
    lbt.GetFiredTriggersL( triggerInfoList );
    
    if ( triggerInfoList.Count())
        {
        CLbtTriggerInfo* triggerInfo = lbt.GetTriggerLC( triggerInfoList[0].iTriggerId );
        CLbtTriggerEntry* triggerEntry = triggerInfo->TriggerEntry();
        CLbtStartupTrigger* startupTrigger = static_cast<CLbtStartupTrigger*>( triggerEntry );
        TPtrC cmdLine = startupTrigger->CommandLine();
        
        // Read only the Profile ID
        TLex lexer( cmdLine );
        User::LeaveIfError( lexer.Val( profileId ));
        
        CleanupStack::PopAndDestroy(triggerInfo );
        }
    triggerInfoList.Reset();
    triggerInfoList.Close();
    CleanupStack::PopAndDestroy( 2 );	 
    
    return profileId;    
    }

// ----------------------------------------------------------------------------
// void ActivateProfileL
// ----------------------------------------------------------------------------
//
void ActivateProfileL( TInt aProfileId )
    {    
    // Obtain the list of profiles
    MProEngEngine* profileEng = NULL;
    profileEng = ProEngFactory::NewEngineL();
    TRAP_IGNORE( profileEng->SetActiveProfileL( aProfileId ));    
    if ( profileEng )
        {
        profileEng->Release();
        }
    }

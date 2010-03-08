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
* Description:  Engine for Geoprofiles application
*
*/


// SYSTEM INCLUDES
#include <lbtgeocircle.h> 
#include <lbttriggerconditionarea.h> 
#include <lbttriggerfilterbyattribute.h> 
#include <lbtlisttriggeroptions.h>
#include <lbtstartuptrigger.h>
#include <lbttriggerinfo.h>
#include <lbttriggerentry.h>
#include <lbtgeocell.h>
#include <s32mem.h>
#include <LbsPosition.h>
#include <aknwaitdialog.h>
#include <geoprofilesui.rsg>

// USER INCLUDES
#include "geoprofilesengine.h"
#include "geoprofilesuid.hrh"
#include "geoprofile.h"
#include "geoprofilesdebug.h"

// CONSTANT DEFINTION
_LIT( KMyGeoProfile, "GeoProfiles" );

//Set executable file name of the trigger handling process.
_LIT( KTriggerHandlingProcessName, "geoprofileshandler.exe" );
_LIT_SECURE_ID( triggerHandlingProcessSID,  KGeoProfilesHandlerUid ); 
_LIT_SECURE_ID( managerProcessSID,  KGeoProfilesUiUid ); 

// ---------------------------------------------------------------------------
// CGeoProfilesEngine::CGeoProfilesEngine
// ---------------------------------------------------------------------------
//
CGeoProfilesEngine::CGeoProfilesEngine( MGeoProfilesEngineObserver&   aObserver )
    :CActive( EPriorityStandard ),
    iObserver( aObserver ),
    iEngineState( CGeoProfilesEngine::ENoAction ) 
    {    
    }
    
// ---------------------------------------------------------------------------
// CGeoProfilesEngine::~CGeoProfilesEngine
// ---------------------------------------------------------------------------
// 
CGeoProfilesEngine::~CGeoProfilesEngine()
    {
    GEOPROFILEDEBUG("+ CGeoProfilesEngine::~CGeoProfilesEngine" )
        
    Cancel();

    // Close the notifier handle
    iNotifier->Cancel();
    delete iNotifier;
     
    delete iStartupTrigger;       
    
    // Close the server handles
    iLbt.Close();
    iLbtNotifier.Close();
    iLbtServer.Close();
    
    GEOPROFILEDEBUG("- CGeoProfilesEngine::~CGeoProfilesEngine" )
    }
    
// ---------------------------------------------------------------------------
// CGeoProfilesEngine::NewL
// ---------------------------------------------------------------------------
// 
CGeoProfilesEngine* CGeoProfilesEngine::NewL( MGeoProfilesEngineObserver&   aObserver )
    {
    GEOPROFILEDEBUG("+ CGeoProfilesEngine::NewL" )
    
    CGeoProfilesEngine* self = new ( ELeave ) CGeoProfilesEngine( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    GEOPROFILEDEBUG("- CGeoProfilesEngine::NewL" )
    return self;                                                                  
    }
    
// ---------------------------------------------------------------------------
// CGeoProfilesEngine::ConstructL
// ---------------------------------------------------------------------------
//
void CGeoProfilesEngine::ConstructL()
    {
    GEOPROFILEDEBUG("- CGeoProfilesEngine::ConstructL" )
    
    // Open the LBT server handle
    iLbtServer.Connect();
    
    // Open the session handle
    iLbt.Open( iLbtServer );
      
    // Open the session for notifiers
    iLbtNotifier.Open( iLbtServer );
        
    // Open the Trigger Notifier handle
    iNotifier = CLbtTriggerChangeEventNotifier::NewL( iLbtNotifier, *this );
    iNotifier->Start();
    
    CActiveScheduler::Add( this );
    
    GEOPROFILEDEBUG("- CGeoProfilesEngine::ConstructL" )  
    }

// ---------------------------------------------------------------------------
// CGeoProfilesEngine::RunL
// ---------------------------------------------------------------------------
//
void CGeoProfilesEngine::RunL()
    {
    GEOPROFILEDEBUG1("+ CGeoProfilesEngine::RunL %d", iStatus.Int())
    
    if ( iStatus.Int() != KErrNone )
        {
        iEngineState = ENoAction;
        
        // Dismiss the Wait dialog if it is Running
        DismissWaitDialogL();
        return;
        }
                
    switch ( iEngineState )
        {
        case ECreateProfile:
            {
            // The profile has been successfully created.
            delete iStartupTrigger;
            iStartupTrigger = NULL;            
            iEngineState = ENoAction;
            
            // Dismiss the Wait Dialog
            DismissWaitDialogL();
            
            break;    
            }
        case EDeleteProfile:
            {
            // Delet the Trigger
            iLbt.DeleteTriggerL( iTriggerId );
            iEngineState = ENoAction;
           
            // Dismiss the Wait Dialog
            DismissWaitDialogL();
            break;    
            }
        case EUpdateProfile:
            {
            // Update the Profile
            DoUpdateProfileL( *iProfile );
            iProfile = NULL;
            
            iEngineState = ENoAction;
            // Dismiss the Wait Dialog
            DismissWaitDialogL();            
                
            }
        default:
            {
            iEngineState = ENoAction;
            // Dismiss the Wait Dialog
            DismissWaitDialogL();  
            break;                
            }
        }    
    GEOPROFILEDEBUG("- CGeoProfilesEngine::RunL")
    }
    

// ---------------------------------------------------------------------------
// CGeoProfilesEngine::DoCancel
// ---------------------------------------------------------------------------
//
void CGeoProfilesEngine::DoCancel()
    {
    GEOPROFILEDEBUG("+ CGeoProfilesEngine::DoCancel")
    
    iLbt.CancelAll();        
    
    DismissWaitDialogL();
    
    GEOPROFILEDEBUG("- CGeoProfilesEngine::DoCancel")
    }

// ---------------------------------------------------------------------------
// void CGeoProfilesEngine::AddProfileL
// ---------------------------------------------------------------------------
//    
void CGeoProfilesEngine::AddProfileL( CGeoProfile&      aProfile )
    {    
    GEOPROFILEDEBUG("+ CGeoProfilesEngine::AddProfileL")
    
    if ( aProfile.GeoPositionType() == CGeoProfile::EUndefined )
        {
        User::Leave( KErrArgument );
        }
        
    if ( IsActive())
        {
        User::Leave( KErrInUse );
        }
        
    CLbtTriggerConditionArea* condition = NULL;
    
    // Currently only co-ordinates are supported
    if( Math::IsNaN( aProfile.GeoPositionInfo().Latitude() ) || 
        Math::IsNaN( aProfile.GeoPositionInfo().Longitude()))
        {
        User::Leave( KErrArgument );
        }
    TInt radius = aProfile.GeoProfileRadius(); 
    if ( !Math::IsNaN( aProfile.GeoPositionInfo().HorizontalAccuracy()))
        {
        radius += aProfile.GeoPositionInfo().HorizontalAccuracy();
        }
        
    //Define the triggering area   
    CLbtGeoCircle * trigArea = CLbtGeoCircle ::NewLC( aProfile.GeoPositionInfo(),
                                                      radius );
    //Construct trigger condition.
    
    CLbtTriggerConditionArea::TDirection triggerType = CLbtTriggerConditionArea::EFireOnEnter;
    if (aProfile.TriggerType() == CGeoProfile::EExitTrigger )
        {
        triggerType = CLbtTriggerConditionArea::EFireOnExit;
        }
    // Currently only Entry type Triggers are suppored
    condition = CLbtTriggerConditionArea::NewL( trigArea, 
                                                triggerType );
    
    //ownership of trigArea is transferred.                                                 
    CleanupStack::Pop( trigArea );
    
    // Load Command line arguments
    HBufC* cmdLineArg = aProfile.PartialExternalizeLC();
    
    // Create the startup trigger
    iStartupTrigger = CLbtStartupTrigger::NewL( aProfile.GeoPlaceName(),
                                                CLbtTriggerEntry::EStateEnabled,
                                                CRequestorBase::ERequestorService,
                                                CRequestorBase::EFormatApplication,
                                                KMyGeoProfile,
                                                managerProcessSID,
                                                condition,
                                                KTriggerHandlingProcessName,
                                                triggerHandlingProcessSID,
                                                *cmdLineArg );    
    CleanupStack::PopAndDestroy( cmdLineArg ); 
    
                                                    
    iTriggerId = 0xFFFFFFFF;
    iEngineState = ECreateProfile;
    
    // Create the trigger 
    iLbt.CreateTrigger( *iStartupTrigger, iTriggerId, ETrue, iStatus );
    SetActive();
    
    // Launch the Wait Dialog
    LaunchWaitDialogL( R_LBL_GEOPROFILES_WAITDLG_CREATE );
    
    GEOPROFILEDEBUG("- CGeoProfilesEngine::AddProfileL")
    }

// ---------------------------------------------------------------------------
// void CGeoProfilesEngine::RemoveProfileL
// ---------------------------------------------------------------------------
//
void CGeoProfilesEngine::UpdateProfileL( CGeoProfile&      aProfile )
    { 
    GEOPROFILEDEBUG("+ CGeoProfilesEngine::UpdateProfileL")

    if ( IsActive())
        {
        User::Leave( KErrInUse );
        }
                    
    iEngineState = EUpdateProfile;    
    iProfile = &aProfile;
    
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    
    // Launch the Wait Dialog
    LaunchWaitDialogL( R_LBL_GEOPROFILES_WAITDLG_UPDATE );    
    
    GEOPROFILEDEBUG("- CGeoProfilesEngine::UpdateProfileL")             
    }
    
// ---------------------------------------------------------------------------
// void CGeoProfilesEngine::RemoveProfileL
// ---------------------------------------------------------------------------
//
void CGeoProfilesEngine::RemoveProfileL( CGeoProfile&      aProfile )
    {
    GEOPROFILEDEBUG("+ CGeoProfilesEngine::RemoveProfileL")

    if ( IsActive())
        {
        User::Leave( KErrInUse );
        }
            
    iEngineState = EDeleteProfile;    
    iTriggerId = aProfile.TriggerId();
    
    SetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    
    // Launch the Wait Dialog
    LaunchWaitDialogL( R_LBL_GEOPROFILES_WAITDLG_DELETE );
    
    GEOPROFILEDEBUG("- CGeoProfilesEngine::RemoveProfileL")
    }
    
// ---------------------------------------------------------------------------
// void CGeoProfilesEngine::ListProfilesL
// ---------------------------------------------------------------------------
//
void CGeoProfilesEngine::ListProfilesL( RPointerArray< CGeoProfile>&   aGeoProfiles )
    {
    GEOPROFILEDEBUG("+ CGeoProfilesEngine::ListProfilesL")
    
    // Clean up the existing profiles
    aGeoProfiles.ResetAndDestroy();
    
    // Fetch the individual list of triggers
    RArray < TLbtTriggerId > aTriggerIdList;    
    iLbt.ListTriggerIdsL( aTriggerIdList );
    
    // Load the individual profiles
    for ( TInt i = 0; i < aTriggerIdList.Count(); i++ )
        {
        CGeoProfile* profile = CGeoProfile::NewL();
        CleanupStack::PushL( profile );
        
        // Fetch the trigger associated with the Id.
        CLbtTriggerInfo* trigger = iLbt.GetTriggerLC( aTriggerIdList[i]);
        CLbtTriggerEntry* triggerEntry = trigger->TriggerEntry();
        
        // Set the Location Information
        CLbtTriggerConditionArea* triggArea =  
                    static_cast< CLbtTriggerConditionArea*>( triggerEntry->GetCondition());
                    
        // Set the Trigger Type
        if ( triggArea->Direction() == CLbtTriggerConditionArea::EFireOnEnter )
            {
            profile->SetTriggerType( CGeoProfile::EEntryTrigger );
            }
        else
            {
            profile->SetTriggerType( CGeoProfile::EExitTrigger );   
            }
            
        CLbtGeoAreaBase* triggGeoArea = 
                    static_cast< CLbtGeoAreaBase*>( triggArea->TriggerArea());
        
        // Set Coordinate Information            
        if ( triggGeoArea->Type() == CLbtGeoAreaBase::ECircle )
            {
            CLbtGeoCircle* loc = static_cast<CLbtGeoCircle*>( triggArea->TriggerArea() );
            TLocality locality;
            locality.SetCoordinate( loc->Center().Latitude(),
                                    loc->Center().Longitude());
            profile->SetGeoPositionInfoL( triggerEntry->Name(), locality );           
            }
        // Copy the Trigger Id
        profile->SetTriggerId( triggerEntry->Id());
        
        // Load all other information
        CLbtStartupTrigger* startupTrigger = static_cast<CLbtStartupTrigger*>( triggerEntry );
        TPtrC cmdLine = startupTrigger->CommandLine();           
        // Internalize the contents
        profile->PartialInternalizeL( cmdLine );
        CleanupStack::PopAndDestroy( trigger );
        
        // Append the profile structure to the array
        if ( aGeoProfiles.Append( profile ))
            {
            CleanupStack::PopAndDestroy( profile );    
            }
        else
            {
            CleanupStack::Pop( profile );    
            }
        }
    aTriggerIdList.Reset();
    aTriggerIdList.Close();
    
    GEOPROFILEDEBUG("- CGeoProfilesEngine::ListProfilesL") 
    }

// ---------------------------------------------------------------------------
// void CGeoProfilesEngine::TriggerChangedL
// ---------------------------------------------------------------------------
//    
void CGeoProfilesEngine::TriggerChangedL( const TLbtTriggerChangeEvent& /* aEvent*/ )
    {
    iObserver.ProfileStatusChangedL();   
    }

// ---------------------------------------------------------------------------
// void CGeoProfilesEngine::LaunchWaitDialogL
// ---------------------------------------------------------------------------
//     
void CGeoProfilesEngine::LaunchWaitDialogL(  TInt aResource )
    {
    GEOPROFILEDEBUG("+ CGeoProfilesEngine::LaunchWaitDialogL") 
    
    iWaitDialog = new ( ELeave ) CAknWaitDialog (( REINTERPRET_CAST( CEikDialog**, &iWaitDialog )), ETrue );          
    iWaitDialog->ExecuteLD( aResource );
    iWaitDialog = NULL;
    
    GEOPROFILEDEBUG("+ CGeoProfilesEngine::LaunchWaitDialogL") 
    }

// ---------------------------------------------------------------------------
// void CGeoProfilesEngine::DismissWaitDialogL
// ---------------------------------------------------------------------------
// 
void CGeoProfilesEngine::DismissWaitDialogL()
    {
    GEOPROFILEDEBUG("+ CGeoProfilesEngine::DismissWaitDialogL") 
    
    if ( iWaitDialog )
        {        	
        TRAPD( err, iWaitDialog->ProcessFinishedL() );
        if ( err )
    		{
		    delete iWaitDialog;
            iWaitDialog = NULL;          
	    	}    	    	
        }
    GEOPROFILEDEBUG("+ CGeoProfilesEngine::DismissWaitDialogL")  
    }
    
// ---------------------------------------------------------------------------
// void CGeoProfilesEngine::DoUpdateProfileL
// ---------------------------------------------------------------------------
//     
void CGeoProfilesEngine::DoUpdateProfileL(  CGeoProfile&      aProfile )
    {
    GEOPROFILEDEBUG("+ CGeoProfilesEngine::DoUpdateProfileL") 
      
    if ( aProfile.GeoPositionType() == CGeoProfile::EUndefined )
        {
        User::Leave( KErrArgument );
        }
            
    CLbtTriggerInfo* trigger = iLbt.GetTriggerLC( aProfile.TriggerId());
    
    CLbtStartupTrigger* startupTrigger = 
        static_cast< CLbtStartupTrigger* >( trigger->TriggerEntry() );
        
    // Copy the Profile Name
    startupTrigger->SetNameL( aProfile.GeoPlaceName());
    
    // Copy the co-ordinates and radius  
    if( Math::IsNaN( aProfile.GeoPositionInfo().Latitude() ) || 
        Math::IsNaN( aProfile.GeoPositionInfo().Longitude()))
        {
        User::Leave( KErrArgument );
        }
    TInt radius = aProfile.GeoProfileRadius(); 
    if ( !Math::IsNaN( aProfile.GeoPositionInfo().HorizontalAccuracy()))
        {
        radius += aProfile.GeoPositionInfo().HorizontalAccuracy();
        }
     
    // Remove the Initial condition area
    startupTrigger->SetCondition( NULL );
                    
    // Define the triggering area   
    CLbtGeoCircle * trigArea = CLbtGeoCircle ::NewLC( aProfile.GeoPositionInfo(), radius );
    
    CLbtTriggerConditionArea::TDirection triggerType = CLbtTriggerConditionArea::EFireOnEnter;
    if (aProfile.TriggerType() == CGeoProfile::EExitTrigger )
        {
        triggerType = CLbtTriggerConditionArea::EFireOnExit;
        }
    CLbtTriggerConditionArea* condition = CLbtTriggerConditionArea::NewL( trigArea, 
                                                                          triggerType );
    
    //ownership of trigArea is transferred.                                                 
    CleanupStack::Pop( trigArea );
    
    // Set the New condition    
    startupTrigger->SetCondition( condition );
    
    
    // Set the Command line arguments
    HBufC* cmdLineArg = aProfile.PartialExternalizeLC();        
    startupTrigger->SetCommandLineL( *cmdLineArg );
        
    // Update the Trigger
    TLbtTriggerAttributeFieldsMask mask = 
            CLbtTriggerEntry::EAttributeName | 
            CLbtTriggerEntry::EAttributeCondition | 
            CLbtTriggerEntry::EAttributeStartUpCommandLine;
         
    iLbt.UpdateTriggerL( *startupTrigger, mask, ELbtTrue );     
    CleanupStack::PopAndDestroy( cmdLineArg );        
    CleanupStack::PopAndDestroy( trigger );
    
    GEOPROFILEDEBUG("- CGeoProfilesEngine::DoUpdateProfileL")        
    }

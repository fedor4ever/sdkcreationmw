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
* Description:  Geo Profile Place selector
*
*/


// SYSTEM INCLUDES
#include <BADESCA.H>
#include <stringloader.h>
#include <aknlistquerydialog.h>
#include <aknpopupheadingpane.h> 
#include <geoprofilesui.rsg>
#include <clmklandmarkselectordlg.h> 
#include <tlmkitemiddbcombiinfo.h>
#include <aknwaitdialog.h>
#include <AknQueryDialog.h>
#include <aknnotewrappers.h> 

// USER INCLUDES
#include "geoplaceselector.h"
#include "geoprofile.h"
#include "geoprofilesdebug.h"
#include "geoprofiles.hrh"

// CONSTANT DEFINTION
_LIT( KTab, "\t" );

// ---------------------------------------------------------------------------
// CGeoPlaceSelector::CGeoPlaceSelector
// ---------------------------------------------------------------------------
//
CGeoPlaceSelector::CGeoPlaceSelector()
    :CActive( EPriorityStandard )
    {    
    }
    
// ---------------------------------------------------------------------------
// CGeoPlaceSelector::~CGeoPlaceSelector
// ---------------------------------------------------------------------------
// 
CGeoPlaceSelector::~CGeoPlaceSelector()
    {
    // Cancel any outstanding request
    Cancel();
    
    iPositioner.Close();
    iServer.Close();
    
    delete iPlacePopupTitle;
    }
    
// ---------------------------------------------------------------------------
// CGeoPlaceSelector::NewL
// ---------------------------------------------------------------------------
// 
CGeoPlaceSelector* CGeoPlaceSelector::NewLC()
    {
    CGeoPlaceSelector* self = new ( ELeave ) CGeoPlaceSelector();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;                                                                  
    }
    
// ---------------------------------------------------------------------------
// CGeoPlaceSelector::ConstructL
// ---------------------------------------------------------------------------
//
void CGeoPlaceSelector::ConstructL()
    {
    // Allocate the Title text
    iPlacePopupTitle = NULL;
    
    User::LeaveIfError( iServer.Connect());
    User::LeaveIfError( iPositioner.Open( iServer ));
    
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// void CGeoPlaceSelector::SelectGeoPlaceL
// ---------------------------------------------------------------------------
//
TInt CGeoPlaceSelector::SelectGeoPlaceL( TInt&            aPlaceType,
                                         TLocality&       aLocality,
                                         TPtr&            aPlaceName )
    {
    GEOPROFILEDEBUG("+ CGeoPlaceSelector::SelectGeoPlaceL" )
    TInt retVal = KErrCancel;
    retVal = DisplayPlacePopUpL();
    switch ( retVal )
        {
        case EPlaceCurrentLocation:
            {
            retVal = SelectPlaceFromCurrentLocationL( aLocality );
            GEOPROFILEDEBUG1("Selectplace from Current Location %d", retVal )
            aPlaceType = CGeoProfile::ECoordinate;
            break;    
            }
        case EPlaceLandmark:
            {
            retVal = SelectPlaceFromLandmarkL( aLocality, aPlaceName );
            GEOPROFILEDEBUG1("Selectplace from Landmark %d", retVal )
            aPlaceType = CGeoProfile::ECoordinate;
            }
        default:
            {
            break;
            }
        }
    
    if ( !retVal )
        {
        GEOPROFILEDEBUG("Selectplace Error" )
        HBufC* title = StringLoader::LoadLC( R_LBL_GEOPROFILES_LOCATIONNAME );
        
        // Ask the User to enter a suitable name
        CAknTextQueryDialog* nameQuery = CAknTextQueryDialog::NewL( aPlaceName );
        retVal = !nameQuery->ExecuteLD( R_GEOPROFILES_NAME_QUERYDIALOG, title->Des());
        CleanupStack::PopAndDestroy( title );     
        }
    GEOPROFILEDEBUG1("- CGeoPlaceSelector::SelectGeoPlaceL %d", retVal )
    
    return retVal;
    }

// ---------------------------------------------------------------------------
// void CGeoPlaceSelector::RunL
// ---------------------------------------------------------------------------
//
void CGeoPlaceSelector::RunL()
    {
    iLocAcqState = iStatus.Int();
    DismissWaitDialog();
    }

// ---------------------------------------------------------------------------
// void CGeoPlaceSelector::DoCancel
// ---------------------------------------------------------------------------
//
void CGeoPlaceSelector::DoCancel()
    {
    iPositioner.CancelRequest( EPositionerNotifyPositionUpdate );
    iLocAcqState = KErrNotFound;
    DismissWaitDialog();       
    }
// ---------------------------------------------------------------------------
// TInt CGeoPlaceSelector::DisplayPlacePopUp
// ---------------------------------------------------------------------------
//
TInt CGeoPlaceSelector::DisplayPlacePopUpL()
    {
    GEOPROFILEDEBUG("+ CGeoPlaceSelector::DisplayPlacePopUpL" )
    TInt retVal = KErrCancel;
       
    // Index for profile selection
    TInt index = 0;
        
    // Load the Title text
    HBufC* title = StringLoader::LoadLC( R_LBL_GEOPROFILES_PLACE );
        
    // Create the list Query Dialog
    CAknListQueryDialog* placeQuery = new( ELeave ) CAknListQueryDialog( &index );
    placeQuery->PrepareLC( R_GEOPROFILES_LIST_QUERY );
          
    // Assign Title Text
    CAknPopupHeadingPane *  heading = placeQuery->QueryHeading();
    if( heading )
        {
        heading->SetTextL( *title );
        }
       
    // Descriptor array
    CDesCArrayFlat* array = new ( ELeave ) CDesCArrayFlat( 4 );
    CleanupStack::PushL( array );
    
    HBufC* placeName = HBufC::NewLC( 256 );
    TPtr placeNamePtr = placeName->Des();
   
    // Append the Current Place Label
    HBufC* currentplace = StringLoader::LoadLC( R_LBL_GEOPROFILES_CURRENTPLACE );
    placeNamePtr.Zero();
    placeNamePtr.Append( KTab );
    placeNamePtr.Append( currentplace->Des());
    array->AppendL( *placeName );    
    CleanupStack::PopAndDestroy( currentplace );   
    
    // Append the Landmark Label
    HBufC* landmark = StringLoader::LoadLC( R_LBL_GEOPROFILES_LANDMARKS );
    placeNamePtr.Zero();
    placeNamePtr.Append( KTab );
    placeNamePtr.Append( landmark->Des());
    array->AppendL( *placeName );
    CleanupStack::PopAndDestroy( landmark );
                
    // Cleanup the profile Name buffer
    CleanupStack::PopAndDestroy( placeName );
    
    // Assign the Item text array
    placeQuery->SetItemTextArray( array );
    placeQuery->SetOwnershipType(  ELbmOwnsItemArray );
    CleanupStack::Pop( array );
      
    if ( placeQuery->RunLD())
        {
        retVal = index;        
        }
        
    CleanupStack::PopAndDestroy( title );
    
    GEOPROFILEDEBUG1("- CGeoPlaceSelector::DisplayPlacePopUpL %d", retVal )
    
    return retVal;    
    }

// ---------------------------------------------------------------------------
// void CGeoPlaceSelector::SelectPlaceFromLandmarkL
// ---------------------------------------------------------------------------
//
TInt CGeoPlaceSelector::SelectPlaceFromLandmarkL( TLocality&    aLocality,
                                                  TPtr&         aPlaceName )
    {
    GEOPROFILEDEBUG("+ CGeoPlaceSelector::SelectPlaceFromLandmarkL" )
    TInt retVal = KErrCancel;
    
    TLmkItemIdDbCombiInfo lmkInfo;
    
    
    // Create a Landmarks object
    CLmkLandmarkSelectorDlg* dlg =  CLmkLandmarkSelectorDlg::NewL();
    
    
    if ( dlg->ExecuteLD( lmkInfo ) )
        {
        GEOPROFILEDEBUG("Item Selected" )
                
        CPosLandmarkDatabase* lmkDb = lmkInfo.GetLmDb();
        CleanupStack::PushL( lmkDb );
        CPosLandmark* lmk = lmkDb->ReadLandmarkLC( lmkInfo.GetItemId() );
        
        //retrieve lat long info
        TLocality locality;
        lmk->GetPosition( locality );
        if( !Math::IsNaN( locality.Latitude() ) && 
            !Math::IsNaN( locality.Longitude()))
            {
            aLocality = locality;
            
            // Store the name if it exists
            TPtrC landmarkName;
            lmk->GetLandmarkName( landmarkName );
            
            if ( landmarkName.Length() && landmarkName.Length() < KGeoPlaceNameMaxLength )
                {
                aPlaceName.Copy( landmarkName );
                }
            retVal = KErrNone;
            }
        else
            {
            User::Leave( KErrNotFound );    
            }
        GEOPROFILEDEBUG("Item Selection complete" )
        CleanupStack::PopAndDestroy( 2, lmkDb );       
        }
    GEOPROFILEDEBUG1("+ CGeoPlaceSelector::SelectPlaceFromLandmarkL %d", retVal )
    return retVal;
    }

// ---------------------------------------------------------------------------
// void CGeoPlaceSelector::SelectPlaceFromCurrentLocationL
// ---------------------------------------------------------------------------
//
TInt CGeoPlaceSelector::SelectPlaceFromCurrentLocationL( TLocality&     aLocality )
    {
    if ( IsActive())
        {
        User::Leave( KErrInUse );
        }
        
    // Initialize the server options
    _LIT(KRequestorName, "Geo Profiles ");    
	User::LeaveIfError( iPositioner.SetRequestor( 
	                    CRequestor::ERequestorService,
	                    CRequestor::EFormatApplication,
	                    KRequestorName ));
	                    
    // Issue a Location Request
    TPositionInfo posInfo;
    iPositioner.NotifyPositionUpdate( posInfo, iStatus );
    SetActive();    
    
    iLocAcqState = KErrNotFound;
    iDialogState = KErrNone;
    
    // Launch the Wait Dialog which will block the thread
    LaunchWaitDialogL();
    
    if( iLocAcqState )
        {
        // If Location Acquistion Failed, leave with the corresponding error code.
        User::Leave( iLocAcqState );
        }
        
    if ( !iDialogState )
        {
        // Now Copy the Location Information
        TPosition position;
        posInfo.GetPosition( position );
        
        if( !Math::IsNaN( position.Latitude() ) && 
            !Math::IsNaN( position.Longitude()))
            {
            aLocality = position;
            }
        else
            {
            User::Leave( KErrNotFound );   
            }                
        }	                     
    return iDialogState;
    }
    
// ---------------------------------------------------------------------------
// void CGeoPlaceSelector::LaunchWaitDialogL
// ---------------------------------------------------------------------------
//     
void CGeoPlaceSelector::LaunchWaitDialogL()
    {
    iWaitDialog = new ( ELeave ) CAknWaitDialog (( REINTERPRET_CAST( CEikDialog**, &iWaitDialog )), ETrue );          
    if( !( iWaitDialog->ExecuteLD( R_LBL_GEOPROFILES_WAITDLG_CURRENTLOCATION )))
        {
        iWaitDialog = NULL;
        iDialogState = KErrAbort;
        Cancel();    
        }
    iWaitDialog = NULL;
    }

// ---------------------------------------------------------------------------
// void CGeoPlaceSelector::DismissWaitDialogL
// ---------------------------------------------------------------------------
// 
void CGeoPlaceSelector::DismissWaitDialog()
    {
    if ( iWaitDialog )
        {        	
        TRAPD( err, iWaitDialog->ProcessFinishedL());
        if ( err )
    		{
		    delete iWaitDialog;                      
	    	}
	    iWaitDialog = NULL;
        } 
    }

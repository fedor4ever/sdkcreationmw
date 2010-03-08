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
* Description:  Geo Profile selector
*
*/


// SYSTEM INCLUDES
#include <BADESCA.H>
#include <stringloader.h>
#include <aknlistquerydialog.h>
#include <aknpopupheadingpane.h> 
#include <geoprofilesui.rsg>
#include <ProEngFactory.h>
#include <mproengengine.h>
#include <mproengprofilenamearray.h> 


// USER INCLUDES
#include "geoprofileselector.h"

// CONSTANT DEFINTION
_LIT( KTab, "\t" );

// ---------------------------------------------------------------------------
// CGeoProfileSelector::CGeoProfileSelector
// ---------------------------------------------------------------------------
//
CGeoProfileSelector::CGeoProfileSelector()
    {    
    }
    
// ---------------------------------------------------------------------------
// CGeoProfileSelector::~CGeoProfileSelector
// ---------------------------------------------------------------------------
// 
CGeoProfileSelector::~CGeoProfileSelector()
    {
    delete iProfilePopupTitle;
    }
    
// ---------------------------------------------------------------------------
// CGeoProfileSelector::NewL
// ---------------------------------------------------------------------------
// 
CGeoProfileSelector* CGeoProfileSelector::NewLC()
    {
    CGeoProfileSelector* self = new ( ELeave ) CGeoProfileSelector();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;                                                                  
    }
    
// ---------------------------------------------------------------------------
// CGeoProfileSelector::ConstructL
// ---------------------------------------------------------------------------
//
void CGeoProfileSelector::ConstructL()
    {
    // Allocate the Title text
    iProfilePopupTitle = NULL;
    }

// ---------------------------------------------------------------------------
// void CGeoProfileSelector::SelectGeoProfileL
// ---------------------------------------------------------------------------
//
TInt CGeoProfileSelector::SelectGeoProfileL( TInt&    aProfileId )
    {
    TInt retVal = KErrCancel;
    
    // Index for profile selection
    TInt index = 0;
        
    // Load the Title text
    HBufC* title = StringLoader::LoadLC( R_LBL_GEOPROFILES_PROFILE );
        
    // Obtain the list of profiles
    MProEngEngine* profileEng = ProEngFactory::NewEngineL();
    CleanupReleasePushL( *profileEng );
   
    MProEngProfileNameArray* profileNames = profileEng->ProfileNameArrayLC();
            
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
    
    HBufC* profileName = HBufC::NewLC( 256 );
    TPtr profileNamePtr = profileName->Des();
        
    for ( TInt i = 0; i < profileNames->MdcaCount(); i++ )
        {
        profileNamePtr.Zero();
        profileNamePtr.Append( KTab );
        profileNamePtr.Append( profileNames->MdcaPoint( i ));
        
        array->AppendL( *profileName );
        }
    
    // Cleanup the profile Name buffer
    CleanupStack::PopAndDestroy( profileName );
    
    // Assign the Item text array
    placeQuery->SetItemTextArray( array );
    placeQuery->SetOwnershipType(  ELbmOwnsItemArray );
    CleanupStack::Pop( array );
      
    if ( placeQuery->RunLD())
        {
        retVal = KErrNone;
        
        // Obtain the Profile Name and Profile ID at index
        aProfileId = profileNames->ProfileId( index );
        }
        
    CleanupStack::PopAndDestroy( 3, title );    
    return retVal;    
    }

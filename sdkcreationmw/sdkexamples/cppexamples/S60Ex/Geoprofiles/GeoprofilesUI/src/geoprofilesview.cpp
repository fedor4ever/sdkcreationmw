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
* Description:  View class for Geo Profiles Application
*
*/


// SYSTEM INCLUDES
#include <geoprofilesui.rsg>
#include <eikmenup.h>
#include <eikbtgpc.h>
#include <Lbs.h>
#include <s32mem.h>
#include <aknnotewrappers.h>
#include <stringloader.h>

// USER INCLUDES
#include "geoprofilesview.h"
#include "geoprofilescontainer.h"
#include "geoprofilesappui.h"
#include "geoprofiles.hrh"
#include "geoprofileslbmodel.h"
#include "geoprofile.h"
#include "geoprofileeditor.h"
  
// CONSTANT DEFINTIONS
const TUid	KGeoProfilesViewId = TUid::Uid( 1 );

// ----------------- Member funtions for CGeoProfilesView class -----------------------

// ---------------------------------------------------------------------------
// CGeoProfilesView::CGeoProfilesView
// ---------------------------------------------------------------------------
//  
CGeoProfilesView::CGeoProfilesView()
    {
    }

// ---------------------------------------------------------------------------
// CGeoProfilesView::~CGeoProfilesView
// ---------------------------------------------------------------------------
//  
CGeoProfilesView::~CGeoProfilesView()
    {
    // Delete the container resource
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }
    delete iContainer;
    
    // Delete the list box model
    delete iLBModel;
    
    delete iEngine;
    
    // Reset And Destroy the buffer
    iGeoProfiles.ResetAndDestroy();
    iGeoProfiles.Close();
    
    }

// ---------------------------------------------------------------------------
// CGeoProfilesView* CGeoProfilesView::NewL
// ---------------------------------------------------------------------------
//        
CGeoProfilesView* CGeoProfilesView::NewL()
    {   
    CGeoProfilesView* self = CGeoProfilesView::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CGeoProfilesView* CGeoProfilesView::NewLC
// ---------------------------------------------------------------------------
//   
CGeoProfilesView* CGeoProfilesView::NewLC()
    {   
    CGeoProfilesView* self = new ( ELeave ) CGeoProfilesView();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// void CGeoProfilesView::ConstructL
// ---------------------------------------------------------------------------
//  
void CGeoProfilesView::ConstructL()
    {
    BaseConstructL( R_GEOPROFILES_VIEW );
  
    iEngine = CGeoProfilesEngine::NewL( *this );
    
    // Fetch the list of currently available profiles
    iEngine->ListProfilesL( iGeoProfiles );
    
    // Allocate the List box view
    iLBModel = CGeoProfilesLBModel::NewL( &iGeoProfiles );
    
    // Set the Correct CBA
    SetCbaL();   
    }

// ---------------------------------------------------------------------------
// void CGeoProfilesView::ContainerCommandL
// ---------------------------------------------------------------------------
//
void CGeoProfilesView::ContainerCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case KEikDynamicLayoutVariantSwitch:
            {
            ScreenSizeChanged();
            break;
            }
    	case EGeoProfilesOpen:
    	    {
    	    // Get the focussed item
    	    TInt index = iContainer->GetFocussedItem();
    	    
    	    if ( index >= 0 && index < iGeoProfiles.Count())
    	        {
                CGeoProfile* profile = iGeoProfiles[ index ];
                if ( profile )
                    {
                    // Launch the editor
                    CGeoProfileEditor* editor = CGeoProfileEditor::NewL( *profile );
            	    if ( editor->ExecuteLD())
            	        {
            	        TRAPD( error, iEngine->UpdateProfileL( *profile ));
                        if ( error )
                            {
                            // Display error note
                            HBufC* errorString = StringLoader::LoadLC( R_LBL_GEOPROFILES_OPERATION_FAIL );
                            CAknErrorNote* note = new( ELeave ) CAknErrorNote;
                            note->ExecuteLD( errorString->Des());
                            CleanupStack::PopAndDestroy( errorString );
                            }            	        
                	    }    	                            
                    }        	   
    	        }    	        	        	    
    	    break;    
    	    }            
        default:
            {
            break;    
            }
        }
    }

// ---------------------------------------------------------------------------
// void CGeoProfilesView::ScreenSizeChanged
// ---------------------------------------------------------------------------
//  
void CGeoProfilesView::ScreenSizeChanged()
	{
	// Request the Client container to change its rect to a new Rect
	if( iContainer )
        {
        iContainer->SetRect( ClientRect());     
        }	
	}
			
// ---------------------------------------------------------------------------
// void CGeoProfilesView::DoActivateL
// ---------------------------------------------------------------------------
//  	
void CGeoProfilesView::DoActivateL( const TVwsViewId&  /* PrevViewId*/,
                                 TUid         /* aCustomMessageId*/,
                           const TDesC8&      /* aCustomMessage */)
    {
    // Destroy the existing container if it exists
    if( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer=NULL;
        }                       
    
    // Create new Container 
    iContainer = CGeoProfilesContainer::NewL( ClientRect(), *iLBModel, *this );
    iContainer->SetMopParent( this );
    AppUi()->AddToViewStackL( *this, iContainer );
    SetCbaL();   	
    }
    
// ---------------------------------------------------------------------------
// void CGeoProfilesView::DoDeactivate
// ---------------------------------------------------------------------------
//           
void CGeoProfilesView::DoDeactivate()
    {
    // Destroy Container
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    }
    
// ---------------------------------------------------------------------------
// TUid CGeoProfilesView::Id
// ---------------------------------------------------------------------------
//   
TUid CGeoProfilesView::Id() const
    {
    return KGeoProfilesViewId;
    }

// ---------------------------------------------------------------------------
// void CGeoProfilesView::HandleCommandL
// ---------------------------------------------------------------------------
//       
void CGeoProfilesView::HandleCommandL( TInt aCommand )   
    {
    // This Class only handles the Help feature. All the other events are handled
    // by the App UI class.
    switch( aCommand )
    	{
    	case EGeoProfilesAdd:
    	    {
    	    // Launch an empty Geo-Profile structure
    	    CGeoProfile* geoProfile = CGeoProfile::NewL();
    	    CleanupStack::PushL( geoProfile );
    	    
    	    // Launch the Editor
    	    CGeoProfileEditor* editor = CGeoProfileEditor::NewL( *geoProfile );
    	    if ( editor->ExecuteLD())
    	        {
    	        TRAPD( error, iEngine->AddProfileL( *geoProfile ));
                if ( error )
                    {
                    // Display error note
                    HBufC* errorString = StringLoader::LoadLC( R_LBL_GEOPROFILES_OPERATION_FAIL );
                    CAknErrorNote* note = new( ELeave ) CAknErrorNote;
                    note->ExecuteLD( errorString->Des());
                    CleanupStack::PopAndDestroy( errorString );
                    }     	        
    	        }    	    
    	    // Delete the Geo-Profile object
    	    CleanupStack::PopAndDestroy( geoProfile );
    	    
    	    break;    
    	    }
    	case EGeoProfilesOpen:
    	    {
    	    // Get the focussed item
    	    TInt index = iContainer->GetFocussedItem();
    	    
    	    if ( index >= 0 && index < iGeoProfiles.Count())
    	        {
                CGeoProfile* profile = iGeoProfiles[ index ];
                if ( profile )
                    {
                    // Launch the editor
                    CGeoProfileEditor* editor = CGeoProfileEditor::NewL( *profile );
            	    if ( editor->ExecuteLD())
            	        {
            	        TRAPD( error, iEngine->UpdateProfileL( *profile ));
                        if ( error )
                            {
                            // Display error note
                            HBufC* errorString = StringLoader::LoadLC( R_LBL_GEOPROFILES_OPERATION_FAIL );
                            CAknErrorNote* note = new( ELeave ) CAknErrorNote;
                            note->ExecuteLD( errorString->Des());
                            CleanupStack::PopAndDestroy( errorString );
                            } 
                	    }    	                            
                    }        	   
    	        }    	        	        	    
    	    break;    
    	    }
    	case EGeoProfilesDelete:
    	    {
    	    // Removing a profile from the list
    	    TInt index = iContainer->GetFocussedItem();
    	    if ( index >= 0 && index < iGeoProfiles.Count())
    	        {
                CGeoProfile* profile = iGeoProfiles[ index ];
                if ( profile )
                    {
                    // Remove it from the Engine
    	            TRAPD( error, iEngine->RemoveProfileL( *profile ));
                    if ( error )
                        {
                        // Display error note
                        HBufC* errorString = StringLoader::LoadLC( R_LBL_GEOPROFILES_OPERATION_FAIL );
                        CAknErrorNote* note = new( ELeave ) CAknErrorNote;
                        note->ExecuteLD( errorString->Des());
                        CleanupStack::PopAndDestroy( errorString );
                        }     	               	                            
                    }    	    
    	        }    	    
    	    break;    
    	    }   	        	        	        	    
        default:
        	{
			AppUi()->HandleCommandL( aCommand );
			break;        	
        	}
    	}
    }

// ---------------------------------------------------------------------------
// void CGeoProfilesView::DynInitMenuPaneL
// ---------------------------------------------------------------------------
//  
void CGeoProfilesView::DynInitMenuPaneL( 
                                TInt          aResourceId, 
                                CEikMenuPane* aMenuPane )
    {
    switch ( aResourceId )
        {
        case R_GEOPROFILES_MENU:
            {
            if ( !iGeoProfiles.Count())
                {
                aMenuPane->SetItemDimmed( EGeoProfilesOpen, ETrue );
                aMenuPane->SetItemDimmed( EGeoProfilesDelete, ETrue );
                }
            break;
            }
        default:
            {
            break;    
            }
        }
    }

// ---------------------------------------------------------------------------
// void CGeoProfilesView::ProfileStatusChangedL
// ---------------------------------------------------------------------------
//     
void CGeoProfilesView::ProfileStatusChangedL()
    {
    // Reload the profiles array
    iEngine->ListProfilesL( iGeoProfiles );
    iLBModel->UpdateModel( &iGeoProfiles );
    iContainer->UpdateL( *iLBModel );
    SetCbaL();
    }

// ---------------------------------------------------------------------------
// void CGeoProfilesView::SetCbaL
// ---------------------------------------------------------------------------
// 
void CGeoProfilesView::SetCbaL()
    {
	if( Cba() )
	    {
	    if ( iGeoProfiles.Count())
	        {
	        Cba()->SetCommandSetL( R_GEOPROFILES_CBA );    
	        }
	   	else
	   	    {
	   	    Cba()->SetCommandSetL( R_GEOPROFILES_CBA_EMPTY );        
	   	    }
	   	Cba()->DrawDeferred();
	    }	    
    }

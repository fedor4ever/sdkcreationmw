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
* Description:  Geo Profile Editor
*
*/


// SYSTEM INCLUDES 
#include <geoprofilesui.rsg>
#include <avkon.rsg>
#include <avkon.hrh>
#include <eikedwin.h>
#include <eikmenup.h>
#include <eikenv.h>
#include <stringloader.h>
#include <eikaufty.h>
#include <aknuniteditor.h> 
#include <e32math.h>
#include <barsread.h>       // For TResourceReader
#include <bautils.h> 
#include <eikbtgpc.h>
#include <akndialog.h>
#include <aknlistquerydialog.h>
#include <AknLocationEd.h>
#include <aknnumedwin.h>
#include <Lbs.h>
#include <LbsPosition.h>
#include <e32math.h>
#include <aknnotewrappers.h>
#include <aknpopupfieldtext.h>
#include <aknnotewrappers.h>

// USER INCLUDES
#include "geoprofileeditor.h"
#include "geoprofile.h"
#include "geoprofiles.hrh"
#include "geoprofileselector.h"
#include "geoplaceselector.h"
#include "geoprofilesdebug.h"

// CONSTANT DEFINTION

// ---------------------------------------------------------------------------
// CGeoProfileEditor::CGeoProfileEditor
// ---------------------------------------------------------------------------
//
CGeoProfileEditor::CGeoProfileEditor( CGeoProfile&      aGeoProfile )
    :iGeoProfile( aGeoProfile )
    {
    }

// ---------------------------------------------------------------------------
// ~CGeoProfileEditor::CGeoProfileEditor
// ---------------------------------------------------------------------------
//
CGeoProfileEditor::~CGeoProfileEditor()
    {
    delete iNewGeoProfile;
    }

// ---------------------------------------------------------------------------
// CGeoProfileEditor* CGeoProfileEditor::NewL
// ---------------------------------------------------------------------------
//
CGeoProfileEditor* CGeoProfileEditor::NewL( CGeoProfile&      aGeoProfile )
    {
    GEOPROFILEDEBUG("+ CGeoProfilesEngine::NewL" )
    
    CGeoProfileEditor* self = new ( ELeave ) CGeoProfileEditor( aGeoProfile );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    GEOPROFILEDEBUG("- CGeoProfilesEngine::NewL" )
    
    return self;
    }

// ---------------------------------------------------------------------------
// void CGeoProfileEditor::ConstructL
// ---------------------------------------------------------------------------
//
void CGeoProfileEditor::ConstructL()
    {
    GEOPROFILEDEBUG("+ CGeoProfilesEngine::ConstructL" )
    
    CAknForm::ConstructL( R_GEOPROFILES_EDITOR_FORM ); 
        
    // Copy the contents of Geo Profile with the orignial contents
    iNewGeoProfile = CGeoProfile::NewL( iGeoProfile.GeoPlaceName(),
                                        iGeoProfile.GeoPositionInfo(),
                                        iGeoProfile.GeoProfileId(),
                                        iGeoProfile.GeoProfileRadius(),
                                        iGeoProfile.TriggerId(),
                                        iGeoProfile.TriggerType()
                                        );
    
    GEOPROFILEDEBUG("- CGeoProfilesEngine::ConstructL" )    
    }

// ---------------------------------------------------------------------------
// TInt CGeoProfileEditor::ExecuteLD
// ---------------------------------------------------------------------------
//
TInt CGeoProfileEditor::ExecuteLD()
    {
    GEOPROFILEDEBUG("CGeoProfilesEngine::ExecuteLD" )
    return CAknForm::ExecuteLD( R_GEOPROFILES_EDITOR_DIALOG );         
    }

// ---------------------------------------------------------------------------
// TBool CGeoProfileEditor::OkToExitL
// ---------------------------------------------------------------------------
//    
TBool CGeoProfileEditor::OkToExitL( TInt aButtonId )
    {
    GEOPROFILEDEBUG("+ CGeoProfilesEngine::OkToExitL" )
    
    TInt retValue = EFalse;
    // Check for the Options Menu.
    switch ( aButtonId )
        {
        case EAknSoftkeyOk:
            {          
            retValue = ETrue;
                        
            if ( IsEditable())
                {
                
                if ( !CheckIfSavableL())
                    {
                    retValue = EFalse;
                    }
                else
                    {
                    iGeoProfile.SetGeoProfileIdL( iNewGeoProfile->GeoProfileId());
                    iGeoProfile.SetGeoPositionInfoL( iNewGeoProfile->GeoPlaceName(), 
                                                     iNewGeoProfile->GeoPositionInfo());
                    iGeoProfile.SetGeoProfileRadius( iNewGeoProfile->GeoProfileRadius());
                    iGeoProfile.SetTriggerType( iNewGeoProfile->TriggerType());                          
                    }                
                }
            break;
            }
        case EAknSoftkeyCancel:
            {                        
            retValue = ETrue;
            break;
            }
        case EAknSoftkeyEdit:
            {
            SetEditableL( ETrue );
            
            // Set the CBA to Editable
            CEikButtonGroupContainer* cba   = CEikButtonGroupContainer::Current();
            CleanupStack::PushL( cba );
            
            TInt focusId = IdOfFocusControl();                        
            if ( focusId == EGeoRadius )
                {
                cba->SetCommandSetL ( R_GEOPROFILES_CBA_OK_CANCEL );
                }
            else
                {
                cba->SetCommandSetL ( R_GEOPROFILES_CBA_OK_CANCEL_CHANGE );     
                }
                      
            cba->DrawDeferred();    
            CleanupStack::Pop( cba );  
                
            break;
            } 
        case EAknSoftkeyChange:
            {
            if ( IdOfFocusControl() == EGeoProfileName )
                {
                HandleProfileSelectionL();
                }
            else if ( IdOfFocusControl() == EGeoPlace )
                {
                HandlePlaceSelectionL();    
                }
            else if ( IdOfFocusControl() == EGeoProfileType )
                {
                
                CAknPopupFieldText* triggerType = static_cast < CAknPopupFieldText* > ( Control( EGeoProfileType ));
                if( triggerType )
                    {
                    if ( triggerType->CurrentValueIndex() == 0)
                        {
                        triggerType->SetCurrentValueIndex( 1 );
                        }
                    else
                        {
                        triggerType->SetCurrentValueIndex( 0 );    
                        }    
                    }                                 
                HandleControlStateChangeL( EGeoProfileType );                
                UpdatePageL( ETrue );               
                }
            break;
            }
        case EGeoProfilesNone:
        default:
            {
            break;    
            }
        }
        
    GEOPROFILEDEBUG("- CGeoProfilesEngine::OkToExitL" )
    return retValue;
    }

// ---------------------------------------------------------------------------
// void CGeoProfileEditor::PreLayoutDynInitL
// ---------------------------------------------------------------------------
// 
void CGeoProfileEditor::PreLayoutDynInitL()
    {
    GEOPROFILEDEBUG("+ CGeoProfilesEngine::PreLayoutDynInitL" )
    
    CAknForm::PreLayoutDynInitL();
    
    // Load the Form values 
    LoadFormValuesL();
    
    GEOPROFILEDEBUG("- CGeoProfilesEngine::PreLayoutDynInitL" )
    }
    
// ---------------------------------------------------------------------------
// void CGeoProfileEditor::LineChangedL
// ---------------------------------------------------------------------------
//    
void CGeoProfileEditor::LineChangedL( TInt  /* aControlId */)
    {
    GEOPROFILEDEBUG("+ CGeoProfilesEngine::LineChangedL" )
    
    if ( IsEditable() )
        {
        // Set the CBA to Editable
        CEikButtonGroupContainer* cba   = CEikButtonGroupContainer::Current();
        CleanupStack::PushL( cba );
                    
        TInt focusId = IdOfFocusControl();                        
        if ( focusId == EGeoRadius )
            {
            cba->SetCommandSetL ( R_GEOPROFILES_CBA_OK_CANCEL );
            }
        else
            {
            cba->SetCommandSetL ( R_GEOPROFILES_CBA_OK_CANCEL_CHANGE );     
            }
                  
        cba->DrawDeferred();    
        CleanupStack::Pop( cba );         
        }
    GEOPROFILEDEBUG("- CGeoProfilesEngine::LineChangedL" )    
    }

// ---------------------------------------------------------------------------
// void CGeoProfileEditor::HandleControlStateChangeL
// ---------------------------------------------------------------------------
// 
void CGeoProfileEditor::HandleControlStateChangeL( TInt aControlId )
    {
    GEOPROFILEDEBUG("+ CGeoProfilesEngine::HandleControlStateChangeL" )
    
    switch ( aControlId )
        {
        case EGeoRadius:
            {
            GEOPROFILEDEBUG("+ Radius" )
            
            CAknIntegerEdwin* integerEd = static_cast < CAknIntegerEdwin* > ( ControlOrNull( EGeoRadius ));
            if ( integerEd )
                {
                TInt radius = 0;
                TInt error = integerEd->GetTextAsInteger( radius );
                if ( CAknIntegerEdwin::EValueValid  == error )
                    {
                    iNewGeoProfile->SetGeoProfileRadius( radius );    
                    }
                }
                
            GEOPROFILEDEBUG("- Radius" )
            break;    
            }
        case EGeoProfileType:
            {
            GEOPROFILEDEBUG("+ TriggerType" )
            
            CAknPopupFieldText* triggerType = static_cast < CAknPopupFieldText* > ( Control( EGeoProfileType ));
            if( triggerType )
                {
                if ( triggerType->CurrentValueIndex() == 0)
                    {
                    iNewGeoProfile->SetTriggerType( CGeoProfile::EEntryTrigger );
                    }
                else
                    {
                    iNewGeoProfile->SetTriggerType( CGeoProfile::EExitTrigger );    
                    }    
                }            
            GEOPROFILEDEBUG("- Trigger Type" )
            break;    
            }
        default:
            {
            break;    
            }
        }
    GEOPROFILEDEBUG("- CGeoProfilesEngine::HandleControlStateChangeL" ) 
    }

// ---------------------------------------------------------------------------
// void CGeoProfileEditor::HandlePointerEventL
// ---------------------------------------------------------------------------
// 
void CGeoProfileEditor::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    GEOPROFILEDEBUG("+ CGeoProfilesEngine::HandlePointerEventL" )
       
    if ( IsEditable())
        {
        CAknForm::HandlePointerEventL( aPointerEvent );         
        // Handle the Pointer Event  
        if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
            {
            switch( IdOfFocusControl())
                {
                case EGeoProfileName:
                    {
                    CAknForm::HandlePointerEventL( aPointerEvent );
                    
                    CEikEdwin* profile = static_cast < CEikEdwin* > ( ControlOrNull( EGeoProfileName ));
                    if ( profile->Rect().Contains( aPointerEvent.iPosition ) )
                        {
                        HandleProfileSelectionL();
                        }                
                    break;    
                    }
                case EGeoPlace:
                    {
                    CAknForm::HandlePointerEventL( aPointerEvent );
                    CEikEdwin* place = static_cast < CEikEdwin* > ( ControlOrNull( EGeoPlace ));
                    if ( place->Rect().Contains( aPointerEvent.iPosition ) )
                        {
                        HandlePlaceSelectionL();
                        }                 
                    break;    
                    }     
                default:
                    {
                    break;                    
                    }
                }
            }            
        }
    else
        {
        CAknForm::HandlePointerEventL( aPointerEvent );     
        }        
    GEOPROFILEDEBUG("- CGeoProfilesEngine::HandlePointerEventL" )
    }

// ---------------------------------------------------------------------------
// void CGeoProfileEditor::LoadFormValuesL
// ---------------------------------------------------------------------------
//    
void CGeoProfileEditor::LoadFormValuesL()
    {
    GEOPROFILEDEBUG("+ CGeoProfilesEngine::LoadFormValuesL" )
    
    // Assign the Initial Values
    LoadProfileL();
    LoadPlaceL();
    LoadTriggerTypeL();
    GEOPROFILEDEBUG("- CGeoProfilesEngine::LoadFormValuesL" )    
    }

// ---------------------------------------------------------------------------
// void CGeoProfileEditor::LoadProfileL
// ---------------------------------------------------------------------------
//    
void CGeoProfileEditor::LoadProfileL()
    {
    GEOPROFILEDEBUG("+ CGeoProfilesEngine::LoadProfileL" )
    
    CEikEdwin* profile = static_cast < CEikEdwin* > ( ControlOrNull( EGeoProfileName ));
    if ( profile )
        {
        TPtrC profileName = iNewGeoProfile->ProfileName();
        profile->SetTextL( &profileName ); 
        profile->DrawDeferred();
        }
        
    GEOPROFILEDEBUG("- CGeoProfilesEngine::LoadProfileL" )         
    }

// ---------------------------------------------------------------------------
// void CGeoProfileEditor::LoadPlaceL
// ---------------------------------------------------------------------------
//    
void CGeoProfileEditor::LoadPlaceL()
    { 
    GEOPROFILEDEBUG("+ CGeoProfilesEngine::LoadPlaceL" )
    
    // Load the Place field name   
    CEikEdwin* geoProfile = static_cast < CEikEdwin* > ( ControlOrNull( EGeoPlace ));
     if ( geoProfile )
         {
         TPtrC geoProfileName = iNewGeoProfile->GeoPlaceName();
         geoProfile->SetTextL( &geoProfileName );
         geoProfile->DrawDeferred();
         } 
 
    // Load the Radius
    CAknIntegerEdwin* integerEd = static_cast < CAknIntegerEdwin* > ( ControlOrNull( EGeoRadius ));
    if ( integerEd )
        {
        integerEd->SetValueL( iNewGeoProfile->GeoProfileRadius() );
        integerEd->DrawDeferred();
        }
          
    // Check the status of the lines and make them visible or invisible based on the Place
    DrawDeferred();
    
    GEOPROFILEDEBUG("- CGeoProfilesEngine::LoadPlaceL" )
    }

// ---------------------------------------------------------------------------
// void CGeoProfileEditor::LoadTriggerTypeL
// ---------------------------------------------------------------------------
// 
void CGeoProfileEditor::LoadTriggerTypeL()
    {
    CAknPopupFieldText* triggerType = static_cast < CAknPopupFieldText* > ( Control( EGeoProfileType ));
    if( triggerType )
        {
        if ( iNewGeoProfile->TriggerType() == CGeoProfile::EEntryTrigger )
            {
            triggerType->SetCurrentValueIndex( 0 );
            }
        else
            {
            triggerType->SetCurrentValueIndex( 1 );    
            }    
        }
    DrawDeferred();   
    }
       
// ---------------------------------------------------------------------------
// void CGeoProfileEditor::HandleProfileSelectionL
// ---------------------------------------------------------------------------
// 
void CGeoProfileEditor::HandleProfileSelectionL()
    {
    GEOPROFILEDEBUG("+ CGeoProfilesEngine::HandleProfileSelectionL" )
    
    // Allocate the Profile Selector class
    CGeoProfileSelector* profileSelector = CGeoProfileSelector::NewLC();
    
    // Launch the profile selector
    TInt profileId = 0;    
    TInt retVal = profileSelector->SelectGeoProfileL( profileId );
                                                      
    if ( !retVal )
        {
        // Profile Selection was successful. Update the Profile
        iNewGeoProfile->SetGeoProfileIdL( profileId );        
        LoadProfileL();
        }
    
    CleanupStack::PopAndDestroy( profileSelector );
    
    GEOPROFILEDEBUG("- CGeoProfilesEngine::HandleProfileSelectionL" )
    }
    
// ---------------------------------------------------------------------------
// void CGeoProfileEditor::HandlePlaceSelectionL
// ---------------------------------------------------------------------------
// 
void CGeoProfileEditor::HandlePlaceSelectionL()
    {
    GEOPROFILEDEBUG("+ CGeoProfilesEngine::HandlePlaceSelectionL" )
    
    // Allocate the Place Selector class
    CGeoPlaceSelector* placeSelector = CGeoPlaceSelector::NewLC();
            
    // Launch the profile selector
    TInt placeType = CGeoProfile::ECoordinate;
    TLocality locality;
    
    // Take a temporary buffer and copy the location   
    HBufC* place = HBufC::NewLC( KGeoPlaceNameMaxLength );
    place->Des().Zero();
    
    TPtr name( place->Des());
    TInt retVal = KErrCancel;   
    TRAPD( error, retVal = placeSelector->SelectGeoPlaceL( placeType,
                                                  locality,
                                                  name ));
    if ( error )
        {
        // Display error note
        HBufC* errorString = StringLoader::LoadLC( R_LBL_GEOPROFILES_LOCATION_FAIL );
        CAknErrorNote* note = new( ELeave ) CAknErrorNote;
        note->ExecuteLD( errorString->Des());
        CleanupStack::PopAndDestroy( errorString );
        }
    else if ( !retVal )
        {
        // Profile Selection was successful. Update the Profile
        iNewGeoProfile->SetGeoPositionInfoL( place->Des(), locality );               
        LoadPlaceL();
        }
    CleanupStack::PopAndDestroy( place ); 
    CleanupStack::PopAndDestroy( placeSelector );    
    
    LoadPlaceL();
     
    GEOPROFILEDEBUG("- CGeoProfilesEngine::HandlePlaceSelectionL" )   
    }

// ---------------------------------------------------------------------------
// TInt CGeoProfileEditor::CheckIfSavableL
// ---------------------------------------------------------------------------
//     
TInt CGeoProfileEditor::CheckIfSavableL()
    {
    GEOPROFILEDEBUG("+ CGeoProfilesEngine::CheckIfSavableL" )
    
    TInt ret = ETrue;
    
    // Check if the Name field has been filled and the Location set
    if ( iNewGeoProfile->GeoPositionType() == CGeoProfile::EUndefined )
        {
        HBufC* info = StringLoader::LoadLC( R_LBL_GEOPROFILES_INCOMPLETE );
        
        iInfoNote = new (ELeave) CAknInformationNote( &iInfoNote );
        iInfoNote->ExecuteLD( info->Des());
        
        CleanupStack::PopAndDestroy( info );
        ret = EFalse;
        }
        
    GEOPROFILEDEBUG("- CGeoProfilesEngine::CheckIfSavableL" )
    return ret;
    }

/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of UI class for the Haptics Example
*
*/


#include <aknnotewrappers.h>
#include <hapticsexample.rsg>
#include <eikmenup.h>
#include <aknlistquerydialog.h>
#include <hwrmhaptics.h>
#include "hapticsexampleappui.h"
#include "hapticsexampleappview.h"
#include "hapticsexample.hrh"
#include "hapticsexampleconstants.h"

// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CHapticsExampleAppUi::CHapticsExampleAppUi()
    {
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CHapticsExampleAppUi::ConstructL()
    { 
    // Initialize application UI with standard value.
    BaseConstructL( CAknAppUi::EAknEnableSkin );

    // Create view object
    iAppView = CHapticsExampleAppView::NewL( ClientRect() );
    AddToStackL( iAppView );
    
    // disable screen rotation, use portrait only
    SetOrientationL( EAppUiOrientationPortrait );
    }


// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CHapticsExampleAppUi::~CHapticsExampleAppUi()
    {
    RemoveFromStack( iAppView );
    delete iAppView;
    iAppView = NULL;
    }


// ---------------------------------------------------------------------------
// From class CEikAppUi.
// Takes care of command handling.
// ---------------------------------------------------------------------------
//
void CHapticsExampleAppUi::HandleCommandL( TInt aCommand )
    {
    switch( aCommand )
        {
        case EAknSoftkeyBack:
            //flow through
        case EAknSoftkeyExit:
            //flow through
        case EEikCmdExit:
            Exit();
            break;

        case EHapticsExampleStrength:
            QueryStrengthSettingL();
            break;

        case EHapticsExampleEnableHaptics:
            //flow through
        case EHapticsExampleDisableHaptics:
            iAppView->SetHapticsEnabled( 
                    aCommand == EHapticsExampleEnableHaptics );
            break;
        case EHapticsExampleSensorMode:
            //flow through
        case EHapticsExampleDragMode:
            iAppView->SetSensorModeL( aCommand == EHapticsExampleSensorMode );
            break;

        default:
            break;
        }
    }


// ---------------------------------------------------------------------------
// From class CEikAppUi.
// Called by framework when layout is changed.
// Passes the new client rectangle to the AppView
// ---------------------------------------------------------------------------
//
void CHapticsExampleAppUi::HandleResourceChangeL( TInt aType )
    {
    // call base class first 
    CAknAppUi::HandleResourceChangeL( aType );
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        iAppView->SetRect( ClientRect() );
        }
    }

// ---------------------------------------------------------------------------
// From MEikMenuObserver
// Dynamically manages the contents of menu
// ---------------------------------------------------------------------------
//
void CHapticsExampleAppUi::DynInitMenuPaneL( TInt aResourceId, 
                                             CEikMenuPane* aMenuPane )
    {
    CAknAppUi::DynInitMenuPaneL( aResourceId, aMenuPane );
    
    if ( iAppView && aResourceId == R_HAPTICSEXAMPLE_MENU )
        {
        aMenuPane->SetItemDimmed( EHapticsExampleEnableHaptics, 
                                  iAppView->HapticsEnabled() );
        aMenuPane->SetItemDimmed( EHapticsExampleDisableHaptics, 
                                  !iAppView->HapticsEnabled() );
        aMenuPane->SetItemDimmed( EHapticsExampleSensorMode, 
                                  iAppView->SensorMode() );
        aMenuPane->SetItemDimmed( EHapticsExampleDragMode, 
                                  !iAppView->SensorMode() );
        }
    }
    
// ---------------------------------------------------------------------------
// Queries user for strength setting
// ---------------------------------------------------------------------------
//
void CHapticsExampleAppUi::QueryStrengthSettingL()
    {
    TInt index;
    TInt strengthPerc = KMaxStrength; // if query fails, the default is  100%
    const TInt KStrengthStep = K25Percent; // interval from 0 to 100%, step 25%
    
    CAknListQueryDialog* query = new ( ELeave )CAknListQueryDialog( &index );
    if ( query )
        {
        query->PrepareLC( R_HAPTICSEXAMPLE_STRENGTH_QUERY );
        if ( query->RunLD() && index >= 0 )
            {
            strengthPerc -= ( index * KStrengthStep );
            }
        }
    iAppView->SetHapticsStrength( strengthPerc );
    }

// End of File


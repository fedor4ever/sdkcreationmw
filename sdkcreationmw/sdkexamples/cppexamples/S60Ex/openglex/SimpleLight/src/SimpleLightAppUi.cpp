/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include "SimpleLightAppUi.h"
#include "SimpleLightContainer.h"
#include <SimpleLight.rsg>
#include "simplelight.hrh"

#include <eikmenup.h>
#include <avkon.hrh>

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CSimpleLightAppUi::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------
//
void CSimpleLightAppUi::ConstructL()
    {
    BaseConstructL();
    iAppContainer = new (ELeave) CSimpleLightContainer;
    iAppContainer->SetMopParent(this);
    iAppContainer->ConstructL( ClientRect(), this );
    AddToStackL( iAppContainer );
    }

// Destructor
CSimpleLightAppUi::~CSimpleLightAppUi()
    {
    if ( iAppContainer )
        {
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
        }
    }

// ------------------------------------------------------------------------------
//  CSimpleLightAppUi::DynInitMenuPaneL
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CSimpleLightAppUi::DynInitMenuPaneL(
    TInt aResourceId, CEikMenuPane* aMenuPane )
    {
    // Item texts for the lighting menu.
    if ( aResourceId == R_SIMPLELIGHT_LIGHTING_MENU )
        {
        // Text for switching the lights on/off.
        if ( iAppContainer->iSimpleLight->iLightingEnabled )
            {
            aMenuPane->SetItemTextL( ESimpleLightLights, R_LIGHTS_OFF );
            }
        else
            {
            aMenuPane->SetItemTextL( ESimpleLightLights, R_LIGHTS_ON  );
            }

        // Text for switching the lamp on/off.
        if ( iAppContainer->iSimpleLight->iLampEnabled )
            {
            aMenuPane->SetItemTextL( ESimpleLightLamp, R_LAMP_OFF );
            }
        else
            {
            aMenuPane->SetItemTextL( ESimpleLightLamp, R_LAMP_ON );
            }

        // Text for switching the spot on/off.
        if ( iAppContainer->iSimpleLight->iSpotEnabled )
            {
            aMenuPane->SetItemTextL( ESimpleLightSpot, R_SPOT_OFF );
            }
        else
            {
            aMenuPane->SetItemTextL( ESimpleLightSpot, R_SPOT_ON );
            }
        }
    }

// ----------------------------------------------------
// CSimpleLightAppUi::HandleKeyEventL
// Key event handler
// ----------------------------------------------------
//
TKeyResponse CSimpleLightAppUi::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/, TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------
// CSimpleLightAppUi::HandleCommandL
// Command handler
// ----------------------------------------------------
//
void CSimpleLightAppUi::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
        case EEikCmdExit:
            {
            Exit();
            break;
            }
        case ESimpleLightLights:
            iAppContainer->iSimpleLight->ToggleLighting();
            break;
        case ESimpleLightLamp:
            iAppContainer->iSimpleLight->ToggleLamp();
            break;
        case ESimpleLightSpot:
            iAppContainer->iSimpleLight->ToggleSpot();
            break;
        case ESimpleLightDuck:
            iAppContainer->iSimpleLight->DuckModel();
            break;
        case ESimpleLightHead:
            iAppContainer->iSimpleLight->HeadModel();
            break;
        default:
            break;
        }
    }

// End of File

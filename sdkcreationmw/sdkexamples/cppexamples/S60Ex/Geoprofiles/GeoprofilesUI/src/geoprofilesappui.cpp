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
* Description:  Application class for Geo Profiles Application UI.
*
*/


// SYSTEM FILES
#include <avkon.hrh>

// USER INCLUDES
#include "geoprofilesappui.h"
#include "geoprofiles.hrh"
#include "geoprofilesview.h"

// ----------------- Member funtions for CGeoProfilesApp ------------------------------

// ---------------------------------------------------------------------------
// void CGeoProfilesAppUi::ConstructL
// ---------------------------------------------------------------------------
//
void CGeoProfilesAppUi::ConstructL()
    {
    BaseConstructL( EAknEnableSkin | EAknEnableMSK );
        
    // Create the View object and add it to the View stack
    CGeoProfilesView* view = CGeoProfilesView::NewL();
    CleanupStack::PushL( view );  	
    AddViewL( view );
    CleanupStack::Pop( view );
    iGeoProfilesView = view;
    }

// ---------------------------------------------------------------------------
// CGeoProfilesAppUi::~CGeoProfilesAppUi
// ---------------------------------------------------------------------------
//
CGeoProfilesAppUi::~CGeoProfilesAppUi()
    {        
    }

// ---------------------------------------------------------------------------
// TKeyResponse CGeoProfilesAppUi::HandleKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CGeoProfilesAppUi::HandleKeyEventL( 
                                    const TKeyEvent& /*aKeyEvent*/,
                                          TEventCode /*aType*/ )
    {
    return EKeyWasNotConsumed;
    }

// ---------------------------------------------------------------------------
// void CGeoProfilesAppUi::HandleCommandL
// ---------------------------------------------------------------------------
//
void CGeoProfilesAppUi::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EAknSoftkeyExit: 
        case EEikCmdExit:
            {
            Exit();
            break;
            }
       default: 
       	    { 	          	           	    
       	    break; 
       	    }
        }
    }

/*
* Copyright (c) 2004 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include <eiktbar.h>
#include <eikenv.h>
#include <w32adll.h>
#include <avkon.hrh>

#include "animation.pan"
#include "animationdocument.h"
#include "AnimationAppUi.h"
#include "AnimationAppView.h"
#include "animation.hrh"
#include "clientdll.h"
#include "clientimagecommander.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAnimationAppUi::CAnimationAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CAnimationAppUi::CAnimationAppUi( RWsSession& aSession )
    : CAknAppUi(),                                // Base class constructor
      iClientDll( aSession ),                     // Animation client dll
      iClientCommander( iClientDll )              // Animation client object
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CAnimationAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAnimationAppUi::ConstructL()
    {
    // Perform the base class construction
    BaseConstructL(EAknEnableSkin);

    // Create an application view
    iAppView = CAnimationAppView::NewL( ClientRect() );

    // Allow the appview to receive keyboard input
    AddToStackL( iAppView );

    // Initialise the RClientDll class
    SetupClientDllL();

    // Initialise the RImageCommander class
    SetupImageCommanderL();
    }

// -----------------------------------------------------------------------------
// CAnimationAppUi::SetupClientDllL()
// Setup the animation client Dll.
// -----------------------------------------------------------------------------
//
void CAnimationAppUi::SetupClientDllL()
    {
    // Create the server dll filename
    TFileName serverDllFilename( KServerDllFilename );

    // Load the animation server, if an error occurs then
    // let higher level handle the problem
    User::LeaveIfError( iClientDll.Load( serverDllFilename ) );
    }

// -----------------------------------------------------------------------------
// CAnimationAppUi::SetupImageCommanderL()
// Setup the animation image commander.
// -----------------------------------------------------------------------------
//
void CAnimationAppUi::SetupImageCommanderL()
    {
    // Tell client to construct a server side object
    TBuf8<1> params( TInt8( 0 ) );
    RWindowBase* windowBase = iAppView->DrawableWindow();
    iClientCommander.ImageConstruct( *windowBase, 
                                     RImageCommander::KAnimationSquare, 
                                     params );
    }

// -----------------------------------------------------------------------------
// CAnimationAppUi::~CAnimationAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CAnimationAppUi::~CAnimationAppUi()
    {
    // remove the application view from the stack
    RemoveFromStack( iAppView );

    // Close the animation object
    iClientCommander.Close ();

    // Close the animation server
    iClientDll.Close();

    // Delete and NULL the application view
    delete iAppView;
    iAppView = NULL;
    }

// -----------------------------------------------------------------------------
// CAnimationAppUi::HandleCommandL()
// Takes care of key event handling.
// -----------------------------------------------------------------------------
//
void CAnimationAppUi::HandleCommandL( TInt aCommand )
    {
    switch( aCommand )
        {
    // Exit the application
    case EEikCmdExit:
    case EAknSoftkeyExit:
        Exit();
        break;

    // Ask the animation to do something, in this case, reset
    case EEikCmdReset:
        iClientCommander.ImageCommand( RImageCommander::KAnimationReset );
        break;

    default:
        break;
        }
    }
    
// ----------------------------------------------------------------------------
// void CAnimationAppUi::HandleResourceChangeL()
// Handles a resource change
// ----------------------------------------------------------------------------
//

void CAnimationAppUi::HandleResourceChangeL( TInt aType )
    {
    CAknAppUi::HandleResourceChangeL( aType );
    if  ( aType == KEikDynamicLayoutVariantSwitch )
    	{
        iAppView->HandleResolutionChange( ClientRect() );
        iClientCommander.ImageCommand( RImageCommander::KResolutionChange );
        }
    }

// End of File

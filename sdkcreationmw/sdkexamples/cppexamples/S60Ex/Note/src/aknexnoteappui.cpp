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
#include "AknExNoteAppUi.h"
#include "AknExNoteView.h"
#include "AknExNoteConstants.h"


// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CAknExNoteAppUi::CAknExNoteAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CAknExNoteAppUi::CAknExNoteAppUi()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CAknExNoteAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAknExNoteAppUi::ConstructL()
    {

    // Initialise app UI with standard value.
    BaseConstructL( EAknEnableSkin );

    // Creates CAknExNoteView class object.
    CAknExNoteView* view = new( ELeave ) CAknExNoteView;

    // Push CAknExNoteView's pointer into CleanUpStack for safety object construction.
    CleanupStack::PushL( view );

    //Construct View
    view->ConstructL();
    
     // View is constructed correctly , Pop it's pointer from CleanUpStack
    CleanupStack::Pop();

    // Transfer ownership to CAknViewAppUi.
    AddViewL( view ); 

    ActivateLocalViewL( view->Id() );
    }

// -----------------------------------------------------------------------------
// CAknExNoteAppUi::~CAknExNoteAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CAknExNoteAppUi::~CAknExNoteAppUi()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CAknExNoteAppUi::HandleCommandL()
// Handles the commands.
// -----------------------------------------------------------------------------
//
void CAknExNoteAppUi::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EEikCmdExit:
            Exit();
            break;
        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
// void CAknExNoteAppUi::HandleResourceChangeL( TInt aType )
// Handles resource changes.
// ----------------------------------------------------------------------------
//
void CAknExNoteAppUi::HandleResourceChangeL( TInt aType )
    {
    CAknAppUi::HandleResourceChangeL( aType );
    if  ( aType == KEikDynamicLayoutVariantSwitch )
    	{
    	CAknView* view = View( KViewId );
        view->HandleViewRectChange();
        }
    }

// End of File

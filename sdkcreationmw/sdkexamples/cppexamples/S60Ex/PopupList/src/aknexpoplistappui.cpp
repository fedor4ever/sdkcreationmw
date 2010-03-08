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
#include <avkon.hrh>
#include <barsread.h>
#include <akntitle.h>
#include <aknnavi.h>
#include <akncontext.h>
#include <AknExPopList.rsg>

#include "aknexpoplist.hrh"
#include "AknExPopListAppUi.h"
#include "AknExPopListView.h"

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CAknExPopListAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAknExPopListAppUi::ConstructL()
    {
    // Initialise app UI with standard value.
    BaseConstructL( EAknEnableSkin );

    // Create CAknExPopListView instance and name it view1
    CAknExPopListView* view = new( ELeave ) CAknExPopListView;

    // Push view1's pointer to CleanupStack
    CleanupStack::PushL( view );

    // Construct View1 
    view->ConstructL();
    
    // Contruct of View1 ok: Pop view1's pointer from CleanUpStack.
    CleanupStack::Pop();

    // Transfer view1's ownership to CAknViewAppUi
    AddViewL( view );
    
    ActivateLocalViewL( view->Id() );
    }

// -----------------------------------------------------------------------------
// CAknExPopListAppUi::~CAknExPopListAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CAknExPopListAppUi::~CAknExPopListAppUi()
    {
    }

// -----------------------------------------------------------------------------
// CAknExPopListAppUi::HandleCommandL()
// Handles the commands.
// -----------------------------------------------------------------------------
//
void CAknExPopListAppUi::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EEikCmdExit:
        case EAknCmdExit:
            Exit();
            break;

        default:
            break;      
        }
    }

// ----------------------------------------------------------------------------
// void CAknExPopListAppUi::HandleResourceChangeL( TInt aType )
// Handles resource changes.
// ----------------------------------------------------------------------------
//
void CAknExPopListAppUi::HandleResourceChangeL( TInt aType )
    {
    CAknAppUi::HandleResourceChangeL( aType );
    if  ( aType == KEikDynamicLayoutVariantSwitch )
    	{
    	CAknView* view = View( KViewId );
    	view->HandleViewRectChange();
        }
    }

// End of File  

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
#include <Avkon.hrh>
#include "AknExQueryAppUi.h"
#include "AknExQueryView.h"

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CAknExQueryAppUi::CAknExQueryAppUi()
// Default constructor.
// ----------------------------------------------------------------------------
//
CAknExQueryAppUi::CAknExQueryAppUi()
    {
    }

// ----------------------------------------------------------------------------
// CAknExQueryAppUi::ConstructL()
// Constructor.
// ----------------------------------------------------------------------------
//
void CAknExQueryAppUi::ConstructL()
    {
    BaseConstructL( EAknEnableSkin );

    CAknExQueryView* view = new ( ELeave ) CAknExQueryView;
    CleanupStack::PushL( view );
    view->ConstructL();
    AddViewL( view ); // transfer ownership to CAknViewAppUi
    CleanupStack::Pop();

    ActivateLocalViewL( view->Id() );
    }

// ----------------------------------------------------------------------------
// CAknExQueryAppUi::~CAknExQueryAppUi()
// Destructor
// Frees reserved resources
// ----------------------------------------------------------------------------
//
CAknExQueryAppUi::~CAknExQueryAppUi()
    {
    }

// ----------------------------------------------------------------------------
// CAknExQueryAppUi::HandleCommandL( TInt aCommand )
// Handles the commands.
// ----------------------------------------------------------------------------
//
void CAknExQueryAppUi::HandleCommandL( TInt aCommand )
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
// void CAknExQueryAppUi::HandleResourceChangeL( TInt aType )
// Handles resource changes.
// ----------------------------------------------------------------------------
//
void CAknExQueryAppUi::HandleResourceChangeL( TInt aType )
    {
    CAknAppUi::HandleResourceChangeL( aType );
    if  ( aType == KEikDynamicLayoutVariantSwitch )
    	{
    	CAknView* view = View( KViewId );
        view->HandleViewRectChange();
        }
    }

// End of File

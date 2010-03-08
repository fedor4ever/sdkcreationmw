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
#include "AknExPopFieldAppUi.h"
#include "AknExPopFieldView.h"

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CAknExPopFieldAppUi::CAknExPopFieldAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// ----------------------------------------------------------------------------
//
CAknExPopFieldAppUi::CAknExPopFieldAppUi()
    {
    // No implementation required
    }

// ----------------------------------------------------------------------------
// void CAknExPopFieldAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldAppUi::ConstructL()
    {
    BaseConstructL(EAknEnableSkin);

    // Creates CAknExPopFieldView class object.
    iView = new ( ELeave ) CAknExPopFieldView;
    CleanupStack::PushL( iView );
    iView->ConstructL();
    AddViewL( iView ); // transfer ownership to CAknViewAppUi.
    CleanupStack::Pop(); // view

    ActivateLocalViewL( iView->Id() );
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldAppUi::~CAknExPopFieldAppUi()
// Destructor.
// ----------------------------------------------------------------------------
//
CAknExPopFieldAppUi::~CAknExPopFieldAppUi()
    {
    // No implementation required
    }

// ----------------------------------------------------------------------------
// void CAknExPopFieldAppUi::HandleCommandL()
// Handles the commands.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldAppUi::HandleCommandL( TInt aCommand )
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
// void CAknExPopFieldAppUi::HandleResourceChangeL( TInt aType )
// Handles resolution change.
// ----------------------------------------------------------------------------
//
void CAknExPopFieldAppUi::HandleResourceChangeL( TInt aType )
    {
    CAknAppUi::HandleResourceChangeL( aType );
    if  ( aType == KEikDynamicLayoutVariantSwitch )
    	{
        iView->HandleSizeChange();
        }
    }

// End of File

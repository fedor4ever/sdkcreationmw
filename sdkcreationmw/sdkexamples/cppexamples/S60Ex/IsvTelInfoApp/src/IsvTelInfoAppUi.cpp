/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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


//INCLUDES
#include <avkon.hrh>
#include "IsvTelInfoAppUi.h"
#include "IsvTelInfoAppView.h"

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CIsvTelInfoAppUi::CIsvTelInfoAppUi()
// Default constructor.
// ----------------------------------------------------------------------------
//
CIsvTelInfoAppUi::CIsvTelInfoAppUi()
    {
    }

// ----------------------------------------------------------------------------
// void CIsvTelInfoAppUi::ConstructL()
// A Second phase constructor
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppUi::ConstructL()
    {
    BaseConstructL(CAknAppUi::EAknEnableSkin);

    // Creates CIsvTelInfoAppView class object.
    CIsvTelInfoAppView* view = new( ELeave ) CIsvTelInfoAppView;
    CleanupStack::PushL( view );
    view->ConstructL();
    AddViewL( view ); // transfer ownership to CAknViewAppUi.
    CleanupStack::Pop(); // view

    iViewId = view->Id();
    ActivateLocalViewL( iViewId );
    }

// ----------------------------------------------------------------------------
// CIsvTelInfoAppUi::~CIsvTelInfoAppUi()
// Destructor.
// ----------------------------------------------------------------------------
//
CIsvTelInfoAppUi::~CIsvTelInfoAppUi()
    {
    }

// ----------------------------------------------------------------------------
// void CIsvTelInfoAppUi::HandleCommandL( TInt )
// Handles the commands.
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppUi::ProcessCommandL( TInt aCommand )
    {
    CAknViewAppUi::ProcessCommandL(aCommand);
    }

// ----------------------------------------------------------------------------
// void CIsvTelInfoAppUi::HandleCommandL( TInt )
// Handles the commands.
// ----------------------------------------------------------------------------
//
void CIsvTelInfoAppUi::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
        case EEikCmdExit:
            Exit();
            break;
        default:
            break;
        }
    }

// End of File


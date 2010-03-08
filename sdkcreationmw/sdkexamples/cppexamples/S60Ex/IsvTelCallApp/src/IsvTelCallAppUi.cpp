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


#include <avkon.hrh>
#include "IsvTelCallAppUi.h"
#include "IsvTelCallAppView.h"

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CIsvTelCallAppUi::CIsvTelCallAppUi()
// Default constructor.
// ----------------------------------------------------------------------------
//
CIsvTelCallAppUi::CIsvTelCallAppUi()
    {
    }

// ----------------------------------------------------------------------------
// void CIsvTelCallAppUi::ConstructL()
// A Second phase constructor
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppUi::ConstructL()
    {
    BaseConstructL(CAknAppUi::EAknEnableSkin);

    // Creates CIsvTelCallAppView class object.
    CIsvTelCallAppView* view = new( ELeave ) CIsvTelCallAppView;
    CleanupStack::PushL( view );
    view->ConstructL();
    AddViewL( view ); // transfer ownership to CAknViewAppUi.
    CleanupStack::Pop(); // view

    iViewId = view->Id();
    ActivateLocalViewL( iViewId );
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppUi::~CIsvTelCallAppUi()
// Destructor.
// ----------------------------------------------------------------------------
//
CIsvTelCallAppUi::~CIsvTelCallAppUi()
    {
    }

// ----------------------------------------------------------------------------
// void CIsvTelCallAppUi::HandleCommandL( TInt )
// Handles the commands.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppUi::ProcessCommandL( TInt aCommand )
    {
    CAknViewAppUi::ProcessCommandL(aCommand);
    }

// ----------------------------------------------------------------------------
// void CIsvTelCallAppUi::HandleCommandL( TInt )
// Handles the commands.
// ----------------------------------------------------------------------------
//
void CIsvTelCallAppUi::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
        case EEikCmdExit:
        	{
        	PrepareToExit();
        	Exit();
            break;
        	}
        default:
            break;
        }
    }

// End of File

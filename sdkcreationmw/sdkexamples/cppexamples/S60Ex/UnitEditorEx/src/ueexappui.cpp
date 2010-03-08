/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include <aknform.h>

#include "UEEx.pan"
#include "UEExAppUi.h"
#include "ueexform.h"
#include "UEEx.hrh"
#include "UEEx.rsg"


// ============================ MEMBER FUNCTIONS ===============================
    
// -----------------------------------------------------------------------------
// CUEExAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUEExAppUi::ConstructL()
    {
    BaseConstructL( EAknEnableSkin );
    
    // Show the form and get out of here
    CUEExForm* form = new ( ELeave ) CUEExForm;
    CleanupStack::PushL( form );
    form->ConstructL( R_FORM_MENUBAR );
    CleanupStack::Pop();
    form->ExecuteLD( R_DIALOG1 );
    Exit();
    }

// -----------------------------------------------------------------------------
// CUEExAppUi::CUEExAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CUEExAppUi::CUEExAppUi()                              
    {
	// no implementation required
    }

// -----------------------------------------------------------------------------
// CUEExAppUi::~CUEExAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CUEExAppUi::~CUEExAppUi()
    {
    }

// -----------------------------------------------------------------------------
// CUEExAppUi::HandleCommandL()
// Takes care of command handling.
// -----------------------------------------------------------------------------
//
void CUEExAppUi::HandleCommandL( TInt aCommand )
    {
    switch( aCommand )
        {
        case EEikCmdExit:
        case EAknSoftkeyExit:
            Exit();
            break;

        default:
            Panic( EUEExBasicUi );
            break;
        }
    }




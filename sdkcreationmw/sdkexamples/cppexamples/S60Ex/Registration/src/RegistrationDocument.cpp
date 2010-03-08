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


// INCLUDE FILES
#include "RegistrationDocument.h"
#include "RegistrationAppui.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// constructor
// ----------------------------------------------------------------------------
//
CRegistrationDocument::CRegistrationDocument( CEikApplication& aApp )
: CAknDocument( aApp )    
    {
    // No implementation required
    }

// ----------------------------------------------------------------------------
// destructor
// ----------------------------------------------------------------------------
//
CRegistrationDocument::~CRegistrationDocument()
    {
    // No implementation required
    }

// ----------------------------------------------------------------------------
// Symbian default constructor can leave.
// ----------------------------------------------------------------------------
//
void CRegistrationDocument::ConstructL()
    {
    // No implementation required
    }

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CRegistrationDocument* CRegistrationDocument::NewL( 
        CEikApplication& aApp )     // CRegistrationApp reference
    {
    CRegistrationDocument* self = new ( ELeave ) CRegistrationDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
// ----------------------------------------------------------------------------
// CRegistrationDocument::CreateAppUiL()
// constructs CRegistrationAppUi
// ----------------------------------------------------------------------------
//
CEikAppUi* CRegistrationDocument::CreateAppUiL()
    {
    return new ( ELeave ) CRegistrationAppUi;
    }

// End of File  

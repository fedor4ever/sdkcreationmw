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
#include "MessagingDocument.h"
#include "MessagingAppui.h"

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CMessagingDocument::CMessagingDocument()
// constructor
// ----------------------------------------------------------------------------
//
CMessagingDocument::CMessagingDocument( CEikApplication& aApp )
: CAknDocument( aApp )
    {
    // no implementation required 
    }

// ----------------------------------------------------------------------------
// CMessagingDocument::~CMessagingDocument()
// destructor
// ----------------------------------------------------------------------------
//
CMessagingDocument::~CMessagingDocument()
    {
    // no implementation required 
    }

// ----------------------------------------------------------------------------
// CMessagingDocument::ConstructL()
// Symbian two phased constructor
// ----------------------------------------------------------------------------
//
void CMessagingDocument::ConstructL()
    {
    // no implementation required 
    }

// ----------------------------------------------------------------------------
// CMessagingDocument::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CMessagingDocument* CMessagingDocument::NewL(
        CEikApplication& aApp )     // CMessagingApp reference
    {
    CMessagingDocument* self = new (ELeave) CMessagingDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
// -----------------------------------------------------------------------------
// CMessagingDocument::CreateAppUiL()
// constructs CMessagingAppUi
// -----------------------------------------------------------------------------
//
CEikAppUi* CMessagingDocument::CreateAppUiL()
    {
    return new ( ELeave ) CMessagingAppUi;
    }

// End of File
/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include "OcrExampleDocument.h"
#include "OcrExampleAppui.h"

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// COcrExampleDocument::COcrExampleDocument()
// constructor
// ----------------------------------------------------------------------------
//
COcrExampleDocument::COcrExampleDocument( CEikApplication& aApp )
: CAknDocument( aApp )
    {
    // no implementation required
    }

// ----------------------------------------------------------------------------
// COcrExampleDocument::~COcrExampleDocument()
// destructor
// ----------------------------------------------------------------------------
//
COcrExampleDocument::~COcrExampleDocument()
    {
    // no implementation required
    }

// ----------------------------------------------------------------------------
// COcrExampleDocument::ConstructL()
// Symbian two phased constructor
// ----------------------------------------------------------------------------
//
void COcrExampleDocument::ConstructL()
    {
    // no implementation required
    }

// ----------------------------------------------------------------------------
// COcrExampleDocument::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
COcrExampleDocument* COcrExampleDocument::NewL(
        CEikApplication& aApp )     // COcrExampleApp reference
    {
    COcrExampleDocument* self = new (ELeave) COcrExampleDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// COcrExampleDocument::CreateAppUiL()
// constructs COcrExampleAppUi
// -----------------------------------------------------------------------------
//
CEikAppUi* COcrExampleDocument::CreateAppUiL()
    {
    return new (ELeave) COcrExampleAppUi;
    }

// End of File

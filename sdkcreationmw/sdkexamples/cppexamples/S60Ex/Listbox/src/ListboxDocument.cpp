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
#include "ListboxAppUi.h"
#include "ListboxDocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CListboxDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CListboxDocument* CListboxDocument::NewL( CEikApplication& aApp )
    {
    CListboxDocument* self = NewLC( aApp );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CListboxDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CListboxDocument* CListboxDocument::NewLC( CEikApplication& aApp )
    {
    CListboxDocument* self =
        new ( ELeave ) CListboxDocument( aApp );

    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CListboxDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CListboxDocument::ConstructL()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CListboxDocument::CListboxDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CListboxDocument::CListboxDocument( CEikApplication& aApp )
    : CAknDocument( aApp )
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CListboxDocument::~CListboxDocument()
// Destructor.
// ---------------------------------------------------------------------------
//
CListboxDocument::~CListboxDocument()
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CListboxDocument::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CListboxDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it;
    // the framework takes ownership of this object
    return ( new ( ELeave ) CListboxAppUi );
    }

// End of File

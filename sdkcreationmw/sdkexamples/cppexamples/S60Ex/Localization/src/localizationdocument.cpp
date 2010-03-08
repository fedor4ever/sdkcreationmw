/*
* Copyright (c) 2005 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include "LocalizationAppUi.h"
#include "LocalizationDocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLocalizationDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLocalizationDocument* CLocalizationDocument::NewL( CEikApplication&
                                                          aApp )
    {
    CLocalizationDocument* self = NewLC( aApp );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CLocalizationDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLocalizationDocument* CLocalizationDocument::NewLC( CEikApplication&
                                                           aApp )
    {
    CLocalizationDocument* self =
        new ( ELeave ) CLocalizationDocument( aApp );

    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CLocalizationDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLocalizationDocument::ConstructL()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CLocalizationDocument::CLocalizationDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CLocalizationDocument::CLocalizationDocument( CEikApplication& aApp )
    : CAknDocument( aApp )
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CLocalizationDocument::~CLocalizationDocument()
// Destructor.
// ---------------------------------------------------------------------------
//
CLocalizationDocument::~CLocalizationDocument()
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CLocalizationDocument::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CLocalizationDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it;
    // the framework takes ownership of this object
    return ( static_cast <CEikAppUi*> ( new ( ELeave )
                                        CLocalizationAppUi ) );
    }

// End of File

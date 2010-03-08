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
* Description:  CDataMobilityDocument implementation
*
*/



// INCLUDE FILES
#include "datamobilityappui.h"
#include "datamobilitydocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDataMobilityDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDataMobilityDocument* CDataMobilityDocument::NewL( CEikApplication& aApp )
    {
    CDataMobilityDocument* self = NewLC( aApp );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CDataMobilityDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CDataMobilityDocument* CDataMobilityDocument::NewLC( CEikApplication& aApp )
    {
    CDataMobilityDocument* self =
        new ( ELeave ) CDataMobilityDocument( aApp );

    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CDataMobilityDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CDataMobilityDocument::ConstructL()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CDataMobilityDocument::CDataMobilityDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CDataMobilityDocument::CDataMobilityDocument( CEikApplication& aApp )
    : CAknDocument( aApp )
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CDataMobilityDocument::~CDataMobilityDocument()
// Destructor.
// ---------------------------------------------------------------------------
//
CDataMobilityDocument::~CDataMobilityDocument()
    {
    // No implementation required
    }

// ---------------------------------------------------------------------------
// CDataMobilityDocument::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CDataMobilityDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it
    // the framework takes ownership of this object
    return ( static_cast <CEikAppUi*> ( new ( ELeave )
                                        CDataMobilityAppUi ) );
    }

// End of File

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
#include "GraphicsAppUi.h"
#include "GraphicsDocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CGraphicsDocument::CGraphicsDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CGraphicsDocument::CGraphicsDocument( CEikApplication& aApp ) : CEikDocument
                                                                    ( aApp )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CGraphicsDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//

CGraphicsDocument* CGraphicsDocument::NewL( CEikApplication& aApp )
    {
    CGraphicsDocument* self = NewLC( aApp );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CGraphicsDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CGraphicsDocument* CGraphicsDocument::NewLC( CEikApplication& aApp )
    {
    CGraphicsDocument* self = new ( ELeave ) CGraphicsDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CGraphicsDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CGraphicsDocument::ConstructL()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CGraphicsDocument::~CGraphicsDocument()
// Destructor.
// -----------------------------------------------------------------------------
//
CGraphicsDocument::~CGraphicsDocument()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CGraphicsDocument::CreateAppUiL()
// Constructs CreateAppUi.
// -----------------------------------------------------------------------------
//
CEikAppUi* CGraphicsDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it;
    // the framework takes ownership of this object
    return ( static_cast<CEikAppUi*>( new ( ELeave ) CGraphicsAppUi) );
    }

// End of File

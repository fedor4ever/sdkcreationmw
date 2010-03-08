/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include "SimpleCubeDocument.h"
#include "SimpleCubeAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CSimpleCubeDocument::CSimpleCubeDocument
//
// Just calls the base class constructor.
// -----------------------------------------------------------------------------
//

CSimpleCubeDocument::CSimpleCubeDocument(CEikApplication& aApp)
: CAknDocument(aApp)
    {
    }

// -----------------------------------------------------------------------------
// CSimpleCubeDocument::~CSimpleCubeDocument
//
// Empty destructor.
// -----------------------------------------------------------------------------
//

CSimpleCubeDocument::~CSimpleCubeDocument()
    {
    }

// -----------------------------------------------------------------------------
// CSimpleCubeDocument::ConstructL
//
// Empty 2nd phase constructor.
// -----------------------------------------------------------------------------
//

void CSimpleCubeDocument::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CSimpleCubeDocument::NewL
//
// -----------------------------------------------------------------------------
//

CSimpleCubeDocument* CSimpleCubeDocument::NewL(
        CEikApplication& aApp)     // CSimpleCubeApp reference
    {
    CSimpleCubeDocument* self = new (ELeave) CSimpleCubeDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------
// CSimpleCubeDocument::CreateAppUiL()
// Constructs and returns a CSimpleCubeAppUi object.
// ----------------------------------------------------
//

CEikAppUi* CSimpleCubeDocument::CreateAppUiL()
    {
    return new (ELeave) CSimpleCubeAppUi;
    }

// End of File

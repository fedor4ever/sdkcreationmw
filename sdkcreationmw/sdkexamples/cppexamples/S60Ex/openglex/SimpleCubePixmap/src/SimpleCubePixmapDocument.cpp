/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include "SimpleCubePixmapDocument.h"
#include "SimpleCubePixmapAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CSimpleCubePixmapDocument::CSimpleCubePixmapDocument
// 
// Just calls the base class constructor.
// -----------------------------------------------------------------------------
//
CSimpleCubePixmapDocument::CSimpleCubePixmapDocument(CEikApplication& aApp)
: CAknDocument(aApp)    
    {
    }

// destructor
CSimpleCubePixmapDocument::~CSimpleCubePixmapDocument()
    {
    }

// -----------------------------------------------------------------------------
// CSimpleCubePixmapDocument::ConstructL
// 
// Empty 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CSimpleCubePixmapDocument::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CSimpleCubeDocument::NewL
// 
// -----------------------------------------------------------------------------
//
CSimpleCubePixmapDocument* CSimpleCubePixmapDocument::NewL(
        CEikApplication& aApp)     // CSimpleCubePixmapApp reference
    {
    CSimpleCubePixmapDocument* self = new (ELeave) CSimpleCubePixmapDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }
    
// ----------------------------------------------------
// CSimpleCubePixmapDocument::CreateAppUiL()
// Constructs and returns a CSimpleCubePixmapAppUi object.
// ----------------------------------------------------
//
CEikAppUi* CSimpleCubePixmapDocument::CreateAppUiL()
    {
    return new (ELeave) CSimpleCubePixmapAppUi;
    }

// End of File  

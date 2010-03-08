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
#include "SimpleCubePbufferDocument.h"
#include "SimpleCubePbufferAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CSimpleCubePbufferDocument::CSimpleCubePbufferDocument
//
// Just calls the base class constructor.
// -----------------------------------------------------------------------------
//
CSimpleCubePbufferDocument::CSimpleCubePbufferDocument(CEikApplication& aApp)
: CAknDocument(aApp)
    {
    }

// destructor
CSimpleCubePbufferDocument::~CSimpleCubePbufferDocument()
    {
    }

// -----------------------------------------------------------------------------
// CSimpleCubePbufferDocument::ConstructL
//
// Empty 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CSimpleCubePbufferDocument::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CSimpleCubeDocument::NewL
//
// -----------------------------------------------------------------------------
//
CSimpleCubePbufferDocument* CSimpleCubePbufferDocument::NewL(
        CEikApplication& aApp)     // CSimpleCubePbufferApp reference
    {
    CSimpleCubePbufferDocument* self = new (ELeave) CSimpleCubePbufferDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------
// CSimpleCubePbufferDocument::CreateAppUiL()
// constructs CSimpleCubePbufferAppUi
// ----------------------------------------------------
//
CEikAppUi* CSimpleCubePbufferDocument::CreateAppUiL()
    {
    return new (ELeave) CSimpleCubePbufferAppUi;
    }

// End of File

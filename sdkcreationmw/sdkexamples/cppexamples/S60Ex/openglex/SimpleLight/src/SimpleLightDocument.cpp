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
#include "SimpleLightDocument.h"
#include "SimpleLightAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CSimpleLightDocument::CSimpleLightDocument
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSimpleLightDocument::CSimpleLightDocument(CEikApplication& aApp)
: CAknDocument(aApp)
    {
    }

// -----------------------------------------------------------------------------
// ?classname::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSimpleLightDocument::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CSimpleLightDocument::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSimpleLightDocument* CSimpleLightDocument::NewL(
        CEikApplication& aApp)     // CSimpleLightApp reference
    {
    CSimpleLightDocument* self = new (ELeave) CSimpleLightDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// destructor
CSimpleLightDocument::~CSimpleLightDocument()
    {
    }

// ----------------------------------------------------
// CSimpleLightDocument::CreateAppUiL()
// constructs CSimpleLightAppUi
// ----------------------------------------------------
//
CEikAppUi* CSimpleLightDocument::CreateAppUiL()
    {
    return new (ELeave) CSimpleLightAppUi;
    }

// End of File

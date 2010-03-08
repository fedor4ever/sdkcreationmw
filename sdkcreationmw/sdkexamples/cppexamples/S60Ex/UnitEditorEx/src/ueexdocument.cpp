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
#include "UEExAppUi.h"
#include "UEExDocument.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUEExDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUEExDocument* CUEExDocument::NewL( CEikApplication& aApp )
    {
    CUEExDocument* self = NewLC(aApp);
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// CUEExDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUEExDocument* CUEExDocument::NewLC( CEikApplication& aApp )
    {
    CUEExDocument* self = new ( ELeave ) CUEExDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CUEExDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUEExDocument::ConstructL()
    {
	// no implementation required
    }    

// -----------------------------------------------------------------------------
// CUEExDocument::CUEExDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CUEExDocument::CUEExDocument( CEikApplication& aApp ) : CAknDocument( aApp ) 
    {
	// no implementation required
    }

// ---------------------------------------------------------------------------
// CUEExDocument::~CUEExDocument()
// Destructor.
// ---------------------------------------------------------------------------
//
CUEExDocument::~CUEExDocument()
    {
	// no implementation required
    }

// ---------------------------------------------------------------------------
// CUEExDocument::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CUEExDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it,
    // the framework takes ownership of this object
    CEikAppUi* appUi = new ( ELeave ) CUEExAppUi;
    return appUi;
    }


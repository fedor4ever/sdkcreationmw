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
#include "longtapdetectorexappui.h"
#include "longtapdetectorexdoc.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLongTapDetectorExDoc::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLongTapDetectorExDoc* CLongTapDetectorExDoc::NewL( CEikApplication& aApp )
    {
    CLongTapDetectorExDoc* self = NewLC( aApp );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CLongTapDetectorExDoc::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLongTapDetectorExDoc* CLongTapDetectorExDoc::NewLC( CEikApplication& aApp )
    {
    CLongTapDetectorExDoc* self = new ( ELeave ) CLongTapDetectorExDoc( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CLongTapDetectorExDoc::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLongTapDetectorExDoc::ConstructL()
    {
	// no implementation required
    }    

// -----------------------------------------------------------------------------
// CLongTapDetectorExDoc::CLongTapDetectorExDoc()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CLongTapDetectorExDoc::CLongTapDetectorExDoc( CEikApplication& aApp ) : CAknDocument( aApp ) 
    {
	// no implementation required
    }

// ---------------------------------------------------------------------------
// CLongTapDetectorExDoc::~CLongTapDetectorExDoc()
// Destructor.
// ---------------------------------------------------------------------------
//
CLongTapDetectorExDoc::~CLongTapDetectorExDoc()
    {
	// no implementation required
    }

// ---------------------------------------------------------------------------
// CLongTapDetectorExDoc::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CLongTapDetectorExDoc::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it,
    // the framework takes ownership of this object
    CEikAppUi* appUi = new ( ELeave ) CLongTapDetectorExAppUi;
    return appUi;
    }


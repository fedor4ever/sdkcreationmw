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
#include "styluspopupmenuexappui.h"
#include "styluspopupmenuexdoc.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CStylusPopupMenuExDoc::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CStylusPopupMenuExDoc* CStylusPopupMenuExDoc::NewL( CEikApplication& aApp )
    {
    CStylusPopupMenuExDoc* self = NewLC( aApp );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CStylusPopupMenuExDoc::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CStylusPopupMenuExDoc* CStylusPopupMenuExDoc::NewLC( CEikApplication& aApp )
    {
    CStylusPopupMenuExDoc* self = new ( ELeave ) CStylusPopupMenuExDoc( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CStylusPopupMenuExDoc::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CStylusPopupMenuExDoc::ConstructL()
    {
	// no implementation required
    }    

// -----------------------------------------------------------------------------
// CStylusPopupMenuExDoc::CStylusPopupMenuExDoc()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CStylusPopupMenuExDoc::CStylusPopupMenuExDoc( CEikApplication& aApp ) : CAknDocument( aApp ) 
    {
	// no implementation required
    }

// ---------------------------------------------------------------------------
// CStylusPopupMenuExDoc::~CStylusPopupMenuExDoc()
// Destructor.
// ---------------------------------------------------------------------------
//
CStylusPopupMenuExDoc::~CStylusPopupMenuExDoc()
    {
	// no implementation required
    }

// ---------------------------------------------------------------------------
// CStylusPopupMenuExDoc::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CStylusPopupMenuExDoc::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it,
    // the framework takes ownership of this object
    CEikAppUi* appUi = new ( ELeave ) CStylusPopupMenuExAppUi;
    return appUi;
    }


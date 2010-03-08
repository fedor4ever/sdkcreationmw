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
#include "ChoiceListExAppUi.h"
#include "ChoiceListExDocument.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CChoiceListExDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CChoiceListExDocument* CChoiceListExDocument::NewL( CEikApplication& aApp )
    {
    CChoiceListExDocument* self = NewLC( aApp );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CChoiceListExDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CChoiceListExDocument* CChoiceListExDocument::NewLC( CEikApplication& aApp )
    {
    CChoiceListExDocument* self = new ( ELeave ) CChoiceListExDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CChoiceListExDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CChoiceListExDocument::ConstructL()
    {
	// no implementation required
    }    

// -----------------------------------------------------------------------------
// CChoiceListExDocument::CChoiceListExDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CChoiceListExDocument::CChoiceListExDocument( CEikApplication& aApp ) : CAknDocument( aApp ) 
    {
	// no implementation required
    }

// ---------------------------------------------------------------------------
// CChoiceListExDocument::~CChoiceListExDocument()
// Destructor.
// ---------------------------------------------------------------------------
//
CChoiceListExDocument::~CChoiceListExDocument()
    {
	// no implementation required
    }

// ---------------------------------------------------------------------------
// CChoiceListExDocument::CreateAppUiL()
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CChoiceListExDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it,
    // the framework takes ownership of this object
    CEikAppUi* appUi = new ( ELeave ) CChoiceListExAppUi;
    return appUi;
    }


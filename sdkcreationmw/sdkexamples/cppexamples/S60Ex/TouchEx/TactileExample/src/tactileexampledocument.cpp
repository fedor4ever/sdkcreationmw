/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of application document class
*
*/


// INCLUDE FILES
#include "tactileexampleappui.h"
#include "tactileexampledocument.h"

// ============================ MEMBER FUNCTIONS =============================


// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
//
CTactileExampleDocument::CTactileExampleDocument( CEikApplication& aApp )
    : CAknDocument( aApp )
    {
    // No implementation required
    }


// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CTactileExampleDocument::~CTactileExampleDocument()
    {
    // No implementation required
    }


// ---------------------------------------------------------------------------
// From class CEikDocument.
// Constructs CreateAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CTactileExampleDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it;
    // the framework takes ownership of this object
    return  new ( ELeave ) CTactileExampleAppUi;
    }
// End of File


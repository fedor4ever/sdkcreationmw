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
#include "animationappui.h"
#include "animationdocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAnimationDocument::CAnimationDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CAnimationDocument::CAnimationDocument( CEikApplication& aApp )
    : CEikDocument( aApp )
    {
    // No implementation required
    }


// -----------------------------------------------------------------------------
// CAnimationDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAnimationDocument* CAnimationDocument::NewL( CEikApplication& aApp )
    {
    CAnimationDocument* self = NewLC( aApp );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CAnimationDocument::NewLC()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAnimationDocument* CAnimationDocument::NewLC( CEikApplication& aApp )
    {
    CAnimationDocument* self = new ( ELeave ) CAnimationDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CAnimationDocument::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAnimationDocument::ConstructL()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CAnimationDocument::~CAnimationDocument()
// Destructor.
// -----------------------------------------------------------------------------
//
CAnimationDocument::~CAnimationDocument()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CAnimationDocument::CreateAppUiL()
// Constructs CreateAppUi.
// -----------------------------------------------------------------------------
//
CEikAppUi* CAnimationDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it
    CEikAppUi* appUi = new ( ELeave ) CAnimationAppUi( CCoeEnv::Static()->
                                                                WsSession() );
    return appUi;
    }

// End of File

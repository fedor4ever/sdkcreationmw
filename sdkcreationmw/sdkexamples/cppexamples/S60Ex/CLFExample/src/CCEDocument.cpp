/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include "CCEDocument.h"

#include "CCEAppUi.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CCEDocument::CCEDocument
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CCEDocument::CCEDocument( CEikApplication& aApp )
    : CAknDocument( aApp )
    {
    }

// -----------------------------------------------------------------------------
// CCEDocument::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CCEDocument* CCEDocument::NewL( CEikApplication& aApp )
    {
    CCEDocument* self = new( ELeave ) CCEDocument( aApp );
    return self;
    }

// Destructor
CCEDocument::~CCEDocument()
    {
    }

// -----------------------------------------------------------------------------
// CCEDocument::CreateAppUiL
// Creates the Application UI.
// -----------------------------------------------------------------------------
//
CEikAppUi* CCEDocument::CreateAppUiL()
    {
    return new( ELeave ) CCEAppUi();
    }

// End of File

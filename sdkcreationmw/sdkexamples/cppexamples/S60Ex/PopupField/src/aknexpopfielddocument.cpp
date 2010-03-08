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
#include "AknExPopFieldDocument.h"
#include "AknExPopFieldAppUi.h"

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CAknExPopFieldDocument::CAknExPopFieldDocument()
// C++ default constructor can NOT contain any code, that might leave.
// ----------------------------------------------------------------------------
//
CAknExPopFieldDocument::CAknExPopFieldDocument( CEikApplication& aApp ):   
                                                CEikDocument( aApp )
    {
    // No implementation required
    }


// ----------------------------------------------------------------------------
// CAknExPopFieldDocument* CAknExPopFieldDocument::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CAknExPopFieldDocument* CAknExPopFieldDocument::NewL( CEikApplication& aApp )
    {
    CAknExPopFieldDocument* self = new ( ELeave ) CAknExPopFieldDocument( aApp );
    return self;
    }

// ----------------------------------------------------------------------------
// CAknExPopFieldDocument::~CAknExPopFieldDocument()
// Destructor.
// ----------------------------------------------------------------------------
//
CAknExPopFieldDocument::~CAknExPopFieldDocument()
    {
    // No implementation required
    }


// ----------------------------------------------------------------------------
// CEikAppUi* CAknExPopFieldDocument::CreateAppUiL()
// Constructs CAknExPopFieldAppUi.
// ----------------------------------------------------------------------------
//
CEikAppUi* CAknExPopFieldDocument::CreateAppUiL()
    {
    return new ( ELeave ) CAknExPopFieldAppUi;
    }

// End of File

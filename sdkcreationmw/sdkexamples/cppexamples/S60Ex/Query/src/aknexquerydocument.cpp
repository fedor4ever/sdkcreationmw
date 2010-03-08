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
#include "AknExQueryDocument.h"
#include "AknExQueryAppUi.h"

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CAknExQueryDocument::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CAknExQueryDocument* CAknExQueryDocument::NewL( CEikApplication& aApp )
    {
    CAknExQueryDocument* self = new ( ELeave ) CAknExQueryDocument( aApp );
    return self;
    }

//-----------------------------------------------------------------------------
// CAknExQueryDocument::~CAknExQueryDocument()
// Destructor.
//-----------------------------------------------------------------------------
//
CAknExQueryDocument::~CAknExQueryDocument()
    {
    }

// ----------------------------------------------------------------------------
// CAknExQueryDocument::CAknExQueryDocument( CEikApplication& )
// Overload constructor.
// ----------------------------------------------------------------------------
//
CAknExQueryDocument::CAknExQueryDocument( CEikApplication& aApp )
: CEikDocument( aApp )
    {
    }

// ----------------------------------------------------------------------------
// CAknExQueryDocument::CreateAppUiL()
// constructs CAknExQueryAppUi
// ----------------------------------------------------------------------------
//
CEikAppUi* CAknExQueryDocument::CreateAppUiL()
    {
    return new ( ELeave ) CAknExQueryAppUi;
    }

// End of File

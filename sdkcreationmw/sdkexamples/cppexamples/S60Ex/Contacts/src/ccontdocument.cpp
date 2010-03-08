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
#include "CContDocument.h"
#include "CContAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CContDocument::CContDocument()
// constructor
// ---------------------------------------------------------
//
CContDocument::CContDocument( CEikApplication& aApp )
: CAknDocument( aApp )
    {
    }

// ---------------------------------------------------------
// CContDocument::~CContDocument ()
// destructor
// ---------------------------------------------------------
//
CContDocument::~CContDocument()
    {
    }

// ---------------------------------------------------------
// CContDocument::ConstructL()
// Two phased constructor
// ---------------------------------------------------------
//
void CContDocument::ConstructL()
    {
    }


// ---------------------------------------------------------
// CContDocument::NewL()
// Two phased constructor
// ---------------------------------------------------------
//
CContDocument* CContDocument::NewL(
        CEikApplication& aApp )     // CContApp reference
    {
    CContDocument* self = new (ELeave) CContDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------
// CContDocument::CreateAppUiL()
// constructs CContAppUi
// ----------------------------------------------------
//
CEikAppUi* CContDocument::CreateAppUiL()
    {
    return new (ELeave) CContAppUi;
    }

// End of File

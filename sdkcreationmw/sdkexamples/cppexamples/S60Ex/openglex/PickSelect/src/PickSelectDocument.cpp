/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include "PickSelectDocument.h"
#include "PickSelectAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CPickSelectDocument::CPickSelectDocument
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPickSelectDocument::CPickSelectDocument(CEikApplication& aApp)
: CAknDocument(aApp)
    {
    }

// -----------------------------------------------------------------------------
// CPickSelectDocument::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPickSelectDocument::ConstructL()
    {
    }


// -----------------------------------------------------------------------------
// CPickSelectDocument::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPickSelectDocument* CPickSelectDocument::NewL(
        CEikApplication& aApp)     // CPickSelectApp reference
    {
    CPickSelectDocument* self = new (ELeave) CPickSelectDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// destructor
CPickSelectDocument::~CPickSelectDocument()
    {
    }

// ----------------------------------------------------
// CPickSelectDocument::CreateAppUiL()
// constructs CPickSelectAppUi
// ----------------------------------------------------
//
CEikAppUi* CPickSelectDocument::CreateAppUiL()
    {
    return new (ELeave) CPickSelectAppUi;
    }

// End of File

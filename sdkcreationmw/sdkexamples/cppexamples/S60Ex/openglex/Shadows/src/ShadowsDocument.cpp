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
#include "ShadowsDocument.h"
#include "ShadowsAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CShadowsDocument::CShadowsDocument
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CShadowsDocument::CShadowsDocument(CEikApplication& aApp)
: CAknDocument(aApp)
    {
    }

// -----------------------------------------------------------------------------
// CShadowsDocument::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CShadowsDocument::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CShadowsDocument::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CShadowsDocument* CShadowsDocument::NewL(
        CEikApplication& aApp)     // CShadowsApp reference
    {
    CShadowsDocument* self = new (ELeave) CShadowsDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// destructor
CShadowsDocument::~CShadowsDocument()
    {
    }

// ----------------------------------------------------
// CShadowsDocument::CreateAppUiL()
// constructs CShadowsAppUi
// ----------------------------------------------------
//
CEikAppUi* CShadowsDocument::CreateAppUiL()
    {
    return new (ELeave) CShadowsAppUi;
    }

// End of File

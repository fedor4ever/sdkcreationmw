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
#include "EnvMappingDocument.h"
#include "EnvMappingAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CEnvMappingDocument::CEnvMappingDocument
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//

CEnvMappingDocument::CEnvMappingDocument(CEikApplication& aApp)
: CAknDocument(aApp)
{
}

// -----------------------------------------------------------------------------
// CMultiTexDocument::~CMultiTexDocument
//
// Empty destructor.
// -----------------------------------------------------------------------------
//

CEnvMappingDocument::~CEnvMappingDocument()
{
}

// -----------------------------------------------------------------------------
// CEnvMappingDocumente::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEnvMappingDocument::ConstructL()
{
}

// -----------------------------------------------------------------------------
// CEnvMappingDocument::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CEnvMappingDocument* CEnvMappingDocument::NewL(
        CEikApplication& aApp)     // CEnvMappingApp reference
    {
    CEnvMappingDocument* self = new (ELeave) CEnvMappingDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------
// CEnvMappingDocument::CreateAppUiL()
// constructs CEnvMappingAppUi
// ----------------------------------------------------
//
CEikAppUi* CEnvMappingDocument::CreateAppUiL()
{
    return new (ELeave) CEnvMappingAppUi;
}


// End of File

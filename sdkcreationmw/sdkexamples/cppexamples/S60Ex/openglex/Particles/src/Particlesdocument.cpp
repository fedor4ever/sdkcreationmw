/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include "ParticlesDocument.h"
#include "ParticlesAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
// CParticlesDocument::CParticlesDocument()
// Default constructor that must never leave.
// ----------------------------------------------------
//
CParticlesDocument::CParticlesDocument( CEikApplication& aApp )
: CAknDocument( aApp )
    {
    }

// ----------------------------------------------------
// CParticlesDocument::~CParticlesDocument()
// Destructor.
// ----------------------------------------------------
//
CParticlesDocument::~CParticlesDocument()
    {
    }

// ----------------------------------------------------
// CParticlesDocument::ConstructL()
// EPOC default constructor can leave.
// ----------------------------------------------------
//
void CParticlesDocument::ConstructL()
    {
    }

// ----------------------------------------------------
// CParticlesDocument::NewL()
// Second phase constructor.
// ----------------------------------------------------
//
CParticlesDocument* CParticlesDocument::NewL( CEikApplication& aApp )
    {
    CParticlesDocument* self = new (ELeave) CParticlesDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------
// CParticlesDocument::CreateAppUiL()
// constructs CParticlesAppUi
// ----------------------------------------------------
//
CEikAppUi* CParticlesDocument::CreateAppUiL()
    {
    return new (ELeave) CParticlesAppUi;
    }

// End of File

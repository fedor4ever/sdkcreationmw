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
#include "FloatingDuckDocument.h"
#include "FloatingDuckAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
// CFloatingDuckDocument::CFloatingDuckDocument()
// Default constructor that must never leave.
// ----------------------------------------------------
//
CFloatingDuckDocument::CFloatingDuckDocument( CEikApplication& aApp )
: CAknDocument( aApp )
    {
    }

// ----------------------------------------------------
// CFloatingDuckDocument::~CFloatingDuckDocument()
// Destructor.
// ----------------------------------------------------
//
CFloatingDuckDocument::~CFloatingDuckDocument()
    {
    }

// ----------------------------------------------------
// CFloatingDuckDocument::ConstructL()
// EPOC default constructor can leave.
// ----------------------------------------------------
//
void CFloatingDuckDocument::ConstructL()
    {
    }

// ----------------------------------------------------
// CFloatingDuckDocument::NewL()
// Second phase constructor.
// ----------------------------------------------------
//
CFloatingDuckDocument* CFloatingDuckDocument::NewL( CEikApplication& aApp )
    {
    CFloatingDuckDocument* self = new (ELeave) CFloatingDuckDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------
// CFloatingDuckDocument::CreateAppUiL()
// constructs CFloatingDuckAppUi
// ----------------------------------------------------
//
CEikAppUi* CFloatingDuckDocument::CreateAppUiL()
    {
    return new (ELeave) CFloatingDuckAppUi;
    }

// End of File

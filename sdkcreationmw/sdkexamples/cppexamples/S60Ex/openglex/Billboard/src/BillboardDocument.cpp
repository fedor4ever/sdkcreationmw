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
#include "BillboardDocument.h"
#include "BillboardAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
// CBillboardDocument::CBillboardDocument()
// Default constructor that must never leave.
// ----------------------------------------------------
//
CBillboardDocument::CBillboardDocument( CEikApplication& aApp )
: CAknDocument( aApp )
    {
    }

// ----------------------------------------------------
// CBillboardDocument::~CBillboardDocument()
// Destructor.
// ----------------------------------------------------
//
CBillboardDocument::~CBillboardDocument()
    {
    }

// ----------------------------------------------------
// CBillboardDocument::ConstructL()
// EPOC default constructor can leave.
// ----------------------------------------------------
//
void CBillboardDocument::ConstructL()
    {
    }

// ----------------------------------------------------
// CBillboardDocument::NewL()
// Second phase constructor.
// ----------------------------------------------------
//
CBillboardDocument* CBillboardDocument::NewL( CEikApplication& aApp )
    {
    CBillboardDocument* self = new (ELeave) CBillboardDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------
// CBillboardDocument::CreateAppUiL()
// constructs CBillboardAppUi
// ----------------------------------------------------
//
CEikAppUi* CBillboardDocument::CreateAppUiL()
    {
    return new (ELeave) CBillboardAppUi;
    }

// End of File

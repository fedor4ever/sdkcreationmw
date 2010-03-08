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
#include "LensFlareDocument.h"
#include "LensFlareAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
// CLensFlareDocument::CLensFlareDocument()
// Default constructor that must never leave.
// ----------------------------------------------------
//
CLensFlareDocument::CLensFlareDocument( CEikApplication& aApp )
: CAknDocument( aApp )
    {
    }

// ----------------------------------------------------
// CLensFlareDocument::~CLensFlareDocument()
// Destructor.
// ----------------------------------------------------
//
CLensFlareDocument::~CLensFlareDocument()
    {
    }

// ----------------------------------------------------
// CLensFlareDocument::ConstructL()
// EPOC default constructor can leave.
// ----------------------------------------------------
//
void CLensFlareDocument::ConstructL()
    {
    }

// ----------------------------------------------------
// CLensFlareDocument::NewL()
// Second phase constructor.
// ----------------------------------------------------
//
CLensFlareDocument* CLensFlareDocument::NewL( CEikApplication& aApp )
    {
    CLensFlareDocument* self = new (ELeave) CLensFlareDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------
// CLensFlareDocument::CreateAppUiL()
// constructs CLensFlareAppUi
// ----------------------------------------------------
//
CEikAppUi* CLensFlareDocument::CreateAppUiL()
    {
    return new (ELeave) CLensFlareAppUi;
    }

// End of File

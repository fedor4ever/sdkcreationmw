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
#include "SnowDocument.h"
#include "SnowAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
// CSnowDocument::CSnowDocument()
// Default constructor that must never leave.
// ----------------------------------------------------
//
CSnowDocument::CSnowDocument( CEikApplication& aApp )
: CAknDocument( aApp )
    {
    }

// ----------------------------------------------------
// CSnowDocument::~CSnowDocument()
// Destructor.
// ----------------------------------------------------
//
CSnowDocument::~CSnowDocument()
    {
    }

// ----------------------------------------------------
// CSnowDocument::ConstructL()
// EPOC default constructor can leave.
// ----------------------------------------------------
//
void CSnowDocument::ConstructL()
    {
    }

// ----------------------------------------------------
// CSnowDocument::NewL()
// Second phase constructor.
// ----------------------------------------------------
//
CSnowDocument* CSnowDocument::NewL( CEikApplication& aApp )
    {
    CSnowDocument* self = new (ELeave) CSnowDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------
// CSnowDocument::CreateAppUiL()
// constructs CSnowAppUi
// ----------------------------------------------------
//
CEikAppUi* CSnowDocument::CreateAppUiL()
    {
    return new (ELeave) CSnowAppUi;
    }

// End of File

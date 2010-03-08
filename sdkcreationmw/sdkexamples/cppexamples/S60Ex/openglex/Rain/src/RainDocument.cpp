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
#include "RainDocument.h"
#include "RainAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
// CRainDocument::CRainDocument()
// Default constructor that must never leave.
// ----------------------------------------------------
//
CRainDocument::CRainDocument( CEikApplication& aApp )
: CAknDocument( aApp )
    {
    }

// ----------------------------------------------------
// CRainDocument::~CRainDocument()
// Destructor.
// ----------------------------------------------------
//
CRainDocument::~CRainDocument()
    {
    }

// ----------------------------------------------------
// CRainDocument::ConstructL()
// EPOC default constructor can leave.
// ----------------------------------------------------
//
void CRainDocument::ConstructL()
    {
    }

// ----------------------------------------------------
// CRainDocument::NewL()
// Second phase constructor.
// ----------------------------------------------------
//
CRainDocument* CRainDocument::NewL( CEikApplication& aApp )
    {
    CRainDocument* self = new (ELeave) CRainDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------
// CRainDocument::CreateAppUiL()
// constructs CRainAppUi
// ----------------------------------------------------
//
CEikAppUi* CRainDocument::CreateAppUiL()
    {
    return new (ELeave) CRainAppUi;
    }

// End of File

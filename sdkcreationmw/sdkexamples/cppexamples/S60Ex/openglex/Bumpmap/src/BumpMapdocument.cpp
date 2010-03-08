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
#include "BumpMapDocument.h"
#include "BumpMapAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
// CBumpMapDocument::CBumpMapDocument()
// Default constructor that must never leave.
// ----------------------------------------------------
//
CBumpMapDocument::CBumpMapDocument( CEikApplication& aApp )
: CAknDocument( aApp )
    {
    }

// ----------------------------------------------------
// CBumpMapDocument::~CBumpMapDocument()
// Destructor.
// ----------------------------------------------------
//
CBumpMapDocument::~CBumpMapDocument()
    {
    }

// ----------------------------------------------------
// CBumpMapDocument::ConstructL()
// EPOC default constructor can leave.
// ----------------------------------------------------
//
void CBumpMapDocument::ConstructL()
    {
    }

// ----------------------------------------------------
// CBumpMapDocument::NewL()
// Second phase constructor.
// ----------------------------------------------------
//
CBumpMapDocument* CBumpMapDocument::NewL( CEikApplication& aApp )
    {
    CBumpMapDocument* self = new (ELeave) CBumpMapDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------
// CBumpMapDocument::CreateAppUiL()
// constructs CBumpMapAppUi
// ----------------------------------------------------
//
CEikAppUi* CBumpMapDocument::CreateAppUiL()
    {
    return new (ELeave) CBumpMapAppUi;
    }

// End of File

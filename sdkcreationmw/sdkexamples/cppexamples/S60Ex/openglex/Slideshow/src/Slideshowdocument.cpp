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
#include "SlideshowDocument.h"
#include "SlideshowAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
// CSlideshowDocument::CSlideshowDocument()
// Default constructor that must never leave.
// ----------------------------------------------------
//
CSlideshowDocument::CSlideshowDocument( CEikApplication& aApp )
: CAknDocument( aApp )
    {
    }

// ----------------------------------------------------
// CSlideshowDocument::~CSlideshowDocument()
// Destructor.
// ----------------------------------------------------
//
CSlideshowDocument::~CSlideshowDocument()
    {
    }

// ----------------------------------------------------
// CSlideshowDocument::ConstructL()
// EPOC default constructor can leave.
// ----------------------------------------------------
//
void CSlideshowDocument::ConstructL()
    {
    }

// ----------------------------------------------------
// CSlideshowDocument::NewL()
// Second phase constructor.
// ----------------------------------------------------
//
CSlideshowDocument* CSlideshowDocument::NewL( CEikApplication& aApp )
    {
    CSlideshowDocument* self = new (ELeave) CSlideshowDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------
// CSlideshowDocument::CreateAppUiL()
// constructs CSlideshowAppUi
// ----------------------------------------------------
//
CEikAppUi* CSlideshowDocument::CreateAppUiL()
    {
    return new (ELeave) CSlideshowAppUi;
    }

// End of File

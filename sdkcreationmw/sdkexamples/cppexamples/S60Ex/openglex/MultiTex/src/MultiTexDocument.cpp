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
#include "MultiTexDocument.h"
#include "MultiTexAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CMultiTexDocument::CMultiTexDocument
//
// Just calls the base class constructor.
// -----------------------------------------------------------------------------
//

CMultiTexDocument::CMultiTexDocument(CEikApplication& aApp)
: CAknDocument(aApp)
{
}

// -----------------------------------------------------------------------------
// CMultiTexDocument::~CMultiTexDocument
//
// Empty destructor.
// -----------------------------------------------------------------------------
//

CMultiTexDocument::~CMultiTexDocument()
{
}

// -----------------------------------------------------------------------------
// CMultiTexDocument::ConstructL
//
// Empty 2nd phase constructor.
// -----------------------------------------------------------------------------
//

void CMultiTexDocument::ConstructL()
{
}

// -----------------------------------------------------------------------------
// CMultiTexDocument::NewL
//
// -----------------------------------------------------------------------------
//

CMultiTexDocument* CMultiTexDocument::NewL(
    CEikApplication& aApp)     // CMultiTexApp reference
{
    CMultiTexDocument* self = new (ELeave) CMultiTexDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
}

// ----------------------------------------------------
// CMultiTexDocument::CreateAppUiL()
// Constructs and returns a CMultiTexAppUi object.
// ----------------------------------------------------
//

CEikAppUi* CMultiTexDocument::CreateAppUiL()
{
    return new (ELeave) CMultiTexAppUi;
}

// End of File

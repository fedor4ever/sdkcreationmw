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
#include "HierarchyDocument.h"
#include "HierarchyAppUi.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CHierarchyDocument::CHierarchyDocument
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHierarchyDocument::CHierarchyDocument(CEikApplication& aApp)
: CAknDocument(aApp)
    {
    }

// destructor
CHierarchyDocument::~CHierarchyDocument()
    {
    }

// -----------------------------------------------------------------------------
// CHierarchyDocument::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHierarchyDocument::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CHierarchyDocument::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHierarchyDocument* CHierarchyDocument::NewL(
        CEikApplication& aApp)     // CHierarchyApp reference
    {
    CHierarchyDocument* self = new (ELeave) CHierarchyDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------
// CHierarchyDocument::CreateAppUiL()
// constructs CHierarchyAppUi
// ----------------------------------------------------
//
CEikAppUi* CHierarchyDocument::CreateAppUiL()
    {
    return new (ELeave) CHierarchyAppUi;
    }

// End of File

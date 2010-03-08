/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation for HierarchicalColumnListDocument class.
*
*/


#include "HierarchicalColumnListAppUi.h"
#include "HierarchicalColumnListDocument.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Standard Symbian OS construction sequence.
// ---------------------------------------------------------------------------
//
CHierarchicalColumnListDocument* CHierarchicalColumnListDocument::NewL(
    CEikApplication& aApp )
    {
    CHierarchicalColumnListDocument* self = NewLC( aApp );
    CleanupStack::Pop( self );
    return self;
    }


CHierarchicalColumnListDocument* CHierarchicalColumnListDocument::NewLC(
    CEikApplication& aApp )
    {
    CHierarchicalColumnListDocument* self =
        new ( ELeave ) CHierarchicalColumnListDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


void CHierarchicalColumnListDocument::ConstructL()
    {
    // no implementation required
    }


CHierarchicalColumnListDocument::CHierarchicalColumnListDocument(
    CEikApplication& aApp )
    : CAknDocument( aApp ) 
    {
    // no implementation required
    }


CHierarchicalColumnListDocument::~CHierarchicalColumnListDocument()
    {
    // no implementation required
    }


CEikAppUi* CHierarchicalColumnListDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it,
    // the framework takes ownership of this object
    CEikAppUi* appUi = new ( ELeave ) CHierarchicalColumnListAppUi;
    return appUi;
    }


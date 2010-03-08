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
#include "MipmappingDocument.h"
#include "MipmappingAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
// CMipmappingDocument::CMipmappingDocument()
// Default constructor that must never leave.
// ----------------------------------------------------
//
CMipmappingDocument::CMipmappingDocument( CEikApplication& aApp )
: CAknDocument( aApp )
    {
    }

// ----------------------------------------------------
// CMipmappingDocument::~CMipmappingDocument()
// Destructor.
// ----------------------------------------------------
//
CMipmappingDocument::~CMipmappingDocument()
    {
    }

// ----------------------------------------------------
// CMipmappingDocument::ConstructL()
// EPOC default constructor can leave.
// ----------------------------------------------------
//
void CMipmappingDocument::ConstructL()
    {
    }

// ----------------------------------------------------
// CMipmappingDocument::NewL()
// Second phase constructor.
// ----------------------------------------------------
//
CMipmappingDocument* CMipmappingDocument::NewL( CEikApplication& aApp )
    {
    CMipmappingDocument* self = new (ELeave) CMipmappingDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------
// CMipmappingDocument::CreateAppUiL()
// constructs CMipmappingAppUi
// ----------------------------------------------------
//
CEikAppUi* CMipmappingDocument::CreateAppUiL()
    {
    return new (ELeave) CMipmappingAppUi;
    }

// End of File

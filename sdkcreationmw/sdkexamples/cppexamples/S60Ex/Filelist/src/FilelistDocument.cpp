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
#include "FilelistDocument.h"
#include "FilelistAppui.h"

// ================= MEMBER FUNCTIONS ==========================================

// -----------------------------------------------------------------------------
// CFilelistDocument::CFilelistDocument()
// constructor
// -----------------------------------------------------------------------------
//
CFilelistDocument::CFilelistDocument( CEikApplication& aApp )
    : CAknDocument( aApp )    
    {
    // no implementation required 
    }

// -----------------------------------------------------------------------------
// CFilelistDocument::~CFilelistContainer()
// Destructor
// -----------------------------------------------------------------------------
//
CFilelistDocument::~CFilelistDocument()
    {
    // no implementation required 
    }

// -----------------------------------------------------------------------------
// CFilelistDocument::ConstructL()
// Symbian two phased constructor
// -----------------------------------------------------------------------------
//
void CFilelistDocument::ConstructL()
    {
    // no implementation required 
    }

// -----------------------------------------------------------------------------
// CFilelistDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFilelistDocument* CFilelistDocument::NewL(
        CEikApplication& aApp )     // CFilelistApp reference
    {
    CFilelistDocument* self = new (ELeave) CFilelistDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
// -----------------------------------------------------------------------------
// CFilelistDocument::CreateAppUiL()
// constructs CFilelistAppUi
// -----------------------------------------------------------------------------
//
CEikAppUi* CFilelistDocument::CreateAppUiL()
    {
    return new (ELeave) CFilelistAppUi;
    }

// End of File  

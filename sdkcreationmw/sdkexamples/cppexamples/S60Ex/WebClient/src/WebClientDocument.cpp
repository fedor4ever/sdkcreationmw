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
#include "WebClientAppUi.h"
#include "WebClientDocument.h"

// ----------------------------------------------------------------------------
// CWebClientDocument::NewL()
// Creates instance of CWebClientDocument.
// ----------------------------------------------------------------------------
//
CWebClientDocument* CWebClientDocument::NewL( CEikApplication& aApp )
    {
    CWebClientDocument* self = NewLC( aApp );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CWebClientDocument::NewLC()
// Creates instance of CWebClientDocument.
// ----------------------------------------------------------------------------
//
CWebClientDocument* CWebClientDocument::NewLC( CEikApplication& aApp )
    {
    CWebClientDocument* self = new (ELeave) CWebClientDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CWebClientDocument::ConstructL()
// Second phase construction.
// ----------------------------------------------------------------------------
//
void CWebClientDocument::ConstructL()
    {
    // no implementation required
    }

// ----------------------------------------------------------------------------
// CWebClientDocument::CWebClientDocument()
// First phase construction.
// ----------------------------------------------------------------------------
//
CWebClientDocument::CWebClientDocument( CEikApplication& aApp ) 
: CAknDocument( aApp ) 
    {
    // no implementation required
    }

// ----------------------------------------------------------------------------
// CWebClientDocument::~CWebClientDocument()
// Destructor.
// ----------------------------------------------------------------------------
//
CWebClientDocument::~CWebClientDocument()
    {
    // no implementation required
    }

// ----------------------------------------------------------------------------
// CWebClientDocument::CreateAppUiL()
// Creates the application user interface, and returns a pointer to it,
// the framework takes ownership of this object
// ----------------------------------------------------------------------------
//
CEikAppUi* CWebClientDocument::CreateAppUiL()
    {
    return ( static_cast <CEikAppUi*> ( new (ELeave) CWebClientAppUi )); 
    }


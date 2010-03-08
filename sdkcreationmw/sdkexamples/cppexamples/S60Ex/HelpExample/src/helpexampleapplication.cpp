/*
* Copyright (c) 2004 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include "HelpExampleDocument.h"
#include "HelpExampleApplication.h"

// ============================ MEMBER FUNCTIONS ===============================

// KUidHelpExampleApp is defined in HelpExampleApplication.h
// because CHelpfulForm needs it.

// -----------------------------------------------------------------------------
// CHelpExampleApplication::CreateDocumentL()
// Creates CHelpExampleDocument object.
// -----------------------------------------------------------------------------
//
CApaDocument* CHelpExampleApplication::CreateDocumentL()
    {  
    // Create an HelpExample document, and return a pointer to it
    return ( static_cast<CApaDocument*>( CHelpExampleDocument::NewL ( *this ) ) );
    }

// -----------------------------------------------------------------------------
// CHelpExampleApplication::AppDllUid()
// Returns application UID.
// -----------------------------------------------------------------------------
//
TUid CHelpExampleApplication::AppDllUid() const
    {
    return KUidHelpExampleApp;
    }

// End of File
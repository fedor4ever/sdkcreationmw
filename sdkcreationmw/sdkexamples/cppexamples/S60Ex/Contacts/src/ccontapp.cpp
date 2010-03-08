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
#include    <eikstart.h>

#include    "CContApp.h"
#include    "CContDocument.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CContApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CContApp::AppDllUid() const
    {
    return KUidContacts;
    }


// ---------------------------------------------------------
// CContApp::CreateDocumentL()
// Creates CCContDocument object
// ---------------------------------------------------------
//
CApaDocument* CContApp::CreateDocumentL()
    {
    return CContDocument::NewL( *this );
    }

// ================= OTHER EXPORTED FUNCTIONS ==============
//
// ---------------------------------------------------------
// NewApplication()
// Constructs CCContApp
// Returns: created application object
// ---------------------------------------------------------
//
EXPORT_C CApaApplication* NewApplication()
    {
    return new CContApp;
    }

// ---------------------------------------------------------
// E32Dll(TDllReason)
// Entry point function
// Returns: KErrNone: No error
// ---------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }

// End of File


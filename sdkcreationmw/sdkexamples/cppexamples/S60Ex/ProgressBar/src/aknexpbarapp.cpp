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
#include <eikstart.h>

#include "AknExPbarApp.h"
#include "AknExPbarDocument.h"


// ========================= MEMBER FUNCTIONS ===================================

// ------------------------------------------------------------------------------
// CAknExPbarApp::AppDllUid()
// Returns application UID
// ------------------------------------------------------------------------------
//
TUid CAknExPbarApp::AppDllUid() const
    {
    return KUidAknExPbar;
    }

// ------------------------------------------------------------------------------
// CAknExPbarApp::CreateDocumentL()
// Creates CAknExPbarDocument object
// ------------------------------------------------------------------------------
//
CApaDocument* CAknExPbarApp::CreateDocumentL()
    {
    return CAknExPbarDocument::NewL( *this );
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================

// ------------------------------------------------------------------------------
// NewApplication() 
// Constructs CAknExPbarApp.
// Create an application, and return a pointer to it
// ------------------------------------------------------------------------------
//
EXPORT_C CApaApplication* NewApplication()
    {
    return new CAknExPbarApp;
    }

// -----------------------------------------------------------------------------
// E32Main()
// Entry point function for Symbian Apps, returns that everything is ok
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }

// End of File  


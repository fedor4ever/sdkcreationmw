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

#include "AknExPopListApp.h"
#include "AknExPopListDocument.h"


// ========================= MEMBER FUNCTIONS ===================================

// ------------------------------------------------------------------------------
// CAknExPopListApp::CreateDocumentL()
// Creates CAknExPopListDocument object
// ------------------------------------------------------------------------------
//
CApaDocument* CAknExPopListApp::CreateDocumentL()
    {
    return CAknExPopListDocument::NewL( *this );
    } 

// ----------------------------------------------------------------------------
// CAknExPopListApp::AppDllUid()
// Returns application UID
// ----------------------------------------------------------------------------
//
TUid CAknExPopListApp::AppDllUid() const
    {
    return KUidAknExPopList;
    }


// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// NewApplication()
// Constructs CAknExPopListApplication.
// Create an application, and return a pointer to it
// -----------------------------------------------------------------------------
//
EXPORT_C CApaApplication* NewApplication()
    {
    return new CAknExPopListApp;
    }

// ----------------------------------------------------------------------------
// TInt E32Main()
// Entry point function for EPOC Apps.
// Returns: TInt:   Error if starting the app through framework wasn't succesful
// ----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }

// End of File  


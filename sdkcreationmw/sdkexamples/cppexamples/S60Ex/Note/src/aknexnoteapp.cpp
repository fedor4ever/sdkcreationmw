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

#include "AknExNoteApp.h"
#include "AknExNoteDocument.h"


// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CAknExNoteApp::CreateDocumentL()
// Creates CAknExNoteDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CAknExNoteApp::CreateDocumentL()
    {
    return CAknExNoteDocument::NewL( *this );
    }

// ----------------------------------------------------------------------------
// TUid CAknExNoteApp::AppDllUid()
// Returns application UID
// ----------------------------------------------------------------------------
//
TUid CAknExNoteApp::AppDllUid() const
    {
    return KUidAknExNote;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// NewApplication()
// Constructs CAknExNoteApplication.
// Create an application, and return a pointer to it
// -----------------------------------------------------------------------------
//
EXPORT_C CApaApplication* NewApplication()
    {
    return new CAknExNoteApp;
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

/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
#include <eikenv.h>
#include "noughtsandcrossesapp.h"
#include "noughtsandcrossesdocument.h"

#include <eikstart.h>

// ============================ MEMBER FUNCTIONS ===============================

// UID for the application;
// this should correspond to the uid defined in the mmp file
const TUid KUidNac = { 0xE6F18BAE };


// -----------------------------------------------------------------------------
// CNoughtsAndCrossesApp::OpenIniFileLC(RFs& aFs)
// Returns the .ini file associated with the application.
// -----------------------------------------------------------------------------
//
CDictionaryStore* CNoughtsAndCrossesApp::OpenIniFileLC(RFs& aFs) const
    {
    return CEikApplication::OpenIniFileLC(aFs);
    }

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesApp::AppDllUid()
// Returns application UID.
// -----------------------------------------------------------------------------
//
TUid CNoughtsAndCrossesApp::AppDllUid() const
    {
    return KUidNac;
    }

// -----------------------------------------------------------------------------
// CNoughtsAndCrossesApp::CreateDocumentL()
// Creates CApaDocument object.
// -----------------------------------------------------------------------------
//
CApaDocument* CNoughtsAndCrossesApp::CreateDocumentL()
    {
    return CNoughtsAndCrossesDocument::NewL (*this);
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// NewApplication()
// Constructs CNoughtsAndCrossesApp.
// -----------------------------------------------------------------------------
//
// Create an application, and return a pointer to it
EXPORT_C CApaApplication* NewApplication()
    {
    return new CNoughtsAndCrossesApp;
    }


// ---------------------------------------------------------
// E32Main()
// Entry point function for new (>= 9.0) EPOC Apps (exe)
// Returns: Sistem Wide error codes or KErrNone if all goes well
// ---------------------------------------------------------
//
GLDEF_C TInt E32Main()
{
	return EikStart::RunApplication( NewApplication );
}

// End of File

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
#include    <eikstart.h>
#include    "RainApp.h"
#include    "RainDocument.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CRainApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CRainApp::AppDllUid() const
    {
    return KUidRain;
    }


// ---------------------------------------------------------
// CRainApp::CreateDocumentL()
// Creates CRainDocument object
// ---------------------------------------------------------
//
CApaDocument* CRainApp::CreateDocumentL()
    {
    return CRainDocument::NewL( *this );
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

// ---------------------------------------------------------
// NewApplication()
// Creates and returns the CRainApp object
// ---------------------------------------------------------
//
CApaApplication* NewApplication()
    {
    return new CRainApp;
    }

// ---------------------------------------------------------
// E32Main()
// Main entry point for exe application. Creates and starts
// the rest of the application.
// ---------------------------------------------------------
//
TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }

// End of File

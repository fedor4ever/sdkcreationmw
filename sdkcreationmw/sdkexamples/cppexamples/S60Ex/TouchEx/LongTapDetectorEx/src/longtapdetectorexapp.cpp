/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include "longtapdetectorexapp.h"
#include "longtapdetectorexdoc.h"


// ============================ MEMBER FUNCTIONS ===============================

// UID for the application;
// this should correspond to the uid defined in the mmp file
static const TUid KUidLongTapDetectorExApp = {0xA0000283};

// -----------------------------------------------------------------------------
// CLongTapDetectorExApp::CreateDocumentL()
// Creates CApaDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CLongTapDetectorExApp::CreateDocumentL()
    {  
    // Create an LongTapDetectorEx document, and return a pointer to it
    CApaDocument* document = CLongTapDetectorExDoc::NewL( *this );
    return document;
    }

// -----------------------------------------------------------------------------
// CLongTapDetectorExApp::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CLongTapDetectorExApp::AppDllUid() const
    {
    // Return the UID for the LongTapDetectorEx application
    return KUidLongTapDetectorExApp;
    }


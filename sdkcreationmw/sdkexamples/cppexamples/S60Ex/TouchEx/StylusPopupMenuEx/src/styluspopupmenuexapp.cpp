/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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
#include "styluspopupmenuexapp.h"
#include "styluspopupmenuexdoc.h"


// ============================ MEMBER FUNCTIONS ===============================

// UID for the application;
// this should correspond to the uid defined in the mmp file
static const TUid KUidStylusPopupMenuExApp = {0xE0000284};

// -----------------------------------------------------------------------------
// CStylusPopupMenuExApp::CreateDocumentL()
// Creates CApaDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CStylusPopupMenuExApp::CreateDocumentL()
    {  
    // Create an StylusPopupMenuEx document, and return a pointer to it
    CApaDocument* document = CStylusPopupMenuExDoc::NewL( *this );
    return document;
    }

// -----------------------------------------------------------------------------
// CStylusPopupMenuExApp::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CStylusPopupMenuExApp::AppDllUid() const
    {
    // Return the UID for the StylusPopupMenuEx application
    return KUidStylusPopupMenuExApp;
    }


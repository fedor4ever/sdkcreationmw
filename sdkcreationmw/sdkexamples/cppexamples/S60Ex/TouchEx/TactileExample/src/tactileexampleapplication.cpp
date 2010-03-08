/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of main application class
*
*/


// INCLUDE FILES
#include "tactileexampledocument.h"
#include "tactileexampleapplication.h"

// ============================ MEMBER FUNCTIONS =============================

// UID for the application;
// this should correspond to the uid defined in the mmp file
const TUid KUidTactileExampleApp = { 0xA0000282 };


// ---------------------------------------------------------------------------
// From class CApaApplication.
// Returns application UID
// ---------------------------------------------------------------------------
//
TUid CTactileExampleApplication::AppDllUid() const
    {
    // Return the UID for the TactileExample application
    return KUidTactileExampleApp;
    }


// ---------------------------------------------------------------------------
// From class CApaApplication.
// Creates CApaDocument object
// ---------------------------------------------------------------------------
//
CApaDocument* CTactileExampleApplication::CreateDocumentL()
    {
    // Create an TactileExample document, and return a pointer to it
    return new ( ELeave ) CTactileExampleDocument( *this );
    }
// End of File

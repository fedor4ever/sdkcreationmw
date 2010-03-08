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
* Description:  This class implements CEikApplication and is used in the 
*                launching of the application
*
*/


// INCLUDE FILES
#include "LBSReferenceApp.h"
#include "LBSReferenceDocument.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CLbsReferenceApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CLbsReferenceApp::AppDllUid() const
    {
    // Return the UID for the Animation application
    return KUidLBSReference;
    }

// ---------------------------------------------------------
// CLbsReferenceApp::CreateDocumentL()
// Creates CLBSReferenceDocument object
// ---------------------------------------------------------
//
CApaDocument* CLbsReferenceApp::CreateDocumentL()
    {
    // Create an Reference application document, and return a pointer to it
    return CLbsReferenceDocument::NewL( *this );
    }


// End of File


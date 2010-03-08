/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of CLandmarksApplication
*
*/


// INCLUDE FILES
#include "LandmarksDocument.h"
#include "LandmarksApplication.h"

// CONSTANTS
// UID for the application, this should match the uid defined in the mmp file.
static const TUid KUidLandmarksApp = {0xA0000187};

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLandmarksApplication::CreateDocumentL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CApaDocument* CLandmarksApplication::CreateDocumentL()
    {  
    // Create an Landmarks document, and return a pointer to it
    CApaDocument* document = CLandmarksDocument::NewL(*this);
    return document;
    }

// -----------------------------------------------------------------------------
// CLandmarksApplication::AppDllUid
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUid CLandmarksApplication::AppDllUid() const
    {
    // Return the UID for the Landmarks application
    return KUidLandmarksApp;
    }

//  End of File  


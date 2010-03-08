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
#include "UEExDocument.h"
#include "UEExApplication.h"

// ============================ MEMBER FUNCTIONS ===============================

// UID for the application;
// this should correspond to the uid defined in the mmp file
static const TUid KUidUEExApp = {0xA000027A};

// -----------------------------------------------------------------------------
// CUEExApplication::CreateDocumentL()
// Creates CApaDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CUEExApplication::CreateDocumentL()
    {  
    // Create an UEEx document, and return a pointer to it
    CApaDocument* document = CUEExDocument::NewL( *this );
    return document;
    }

// -----------------------------------------------------------------------------
// CUEExApplication::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CUEExApplication::AppDllUid() const
    {
    // Return the UID for the UEEx application
    return KUidUEExApp;
    }


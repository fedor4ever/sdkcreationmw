/*
* Copyright (c) 2005 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include "DynamicSettingListDocument.h"
#include "DynamicSettingListApplication.h"

// UID for the application, this should correspond to the uid defined in the mmp file
static const TUid KUidDynamicSettingListApp = {0x0D6F698F};


// ================= MEMBER FUNCTIONS =======================


// ---------------------------------------------------------
// CDynamicSettingListApplication::CreateDocumentL()
// Creates CDynamicSettingListApplication object
// ---------------------------------------------------------
//
CApaDocument* CDynamicSettingListApplication::CreateDocumentL()
    {  
    // Create an DynamicSettingList document, and return a pointer to it
    CApaDocument* document = CDynamicSettingListDocument::NewL(*this);
    return document;
    }

// ---------------------------------------------------------
// CDynamicSettingListApplication::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CDynamicSettingListApplication::AppDllUid() const
    {
    // Return the UID for the DynamicSettingList application
    return KUidDynamicSettingListApp;
    }

// End of file
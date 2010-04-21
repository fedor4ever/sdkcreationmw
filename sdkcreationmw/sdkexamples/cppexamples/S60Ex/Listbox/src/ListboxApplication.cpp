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


#include "ListboxDocument.h"
#include "ListboxApplication.h"

// ============================ MEMBER FUNCTIONS ===============================

// UID for the application;
// this should correspond to the uid defined in the mmp file
const TUid KUidListboxApp = { 0xE0000228 };

// -----------------------------------------------------------------------------
// CListboxApplication::CreateDocumentL()
// Creates CApaDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CListboxApplication::CreateDocumentL()
    {
    // Create an Listbox document, and return a pointer to it
    return (static_cast<CApaDocument*>
                    ( CListboxDocument::NewL( *this ) ) );
    }

// -----------------------------------------------------------------------------
// CListboxApplication::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CListboxApplication::AppDllUid() const
    {
    // Return the UID for the Listbox application
    return KUidListboxApp;
    }

// End of File


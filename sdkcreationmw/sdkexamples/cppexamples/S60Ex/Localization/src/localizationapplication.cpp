/*
* Copyright (c) 2005 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include "LocalizationDocument.h"
#include "LocalizationApplication.h"

// ============================ MEMBER FUNCTIONS ===============================

// UID for the application;
// this should correspond to the uid defined in the mmp file

const TUid KUidLocalizationApp = { 0xE0005B00 };



// -----------------------------------------------------------------------------
// CLocalizationApplication::CreateDocumentL()
// Creates CApaDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CLocalizationApplication::CreateDocumentL()
    {
    // Create an Localization document, and return a pointer to it
    return (static_cast<CApaDocument*>
                    ( CLocalizationDocument::NewL( *this ) ) );
    }

// -----------------------------------------------------------------------------
// CLocalizationApplication::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CLocalizationApplication::AppDllUid() const
    {
    // Return the UID for the Localization application
    return KUidLocalizationApp;
    }

// End of File

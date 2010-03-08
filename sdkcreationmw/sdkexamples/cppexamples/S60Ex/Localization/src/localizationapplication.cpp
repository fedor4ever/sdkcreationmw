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
#include "LocalizationDocument.h"
#include "LocalizationApplication.h"

// ============================ MEMBER FUNCTIONS ===============================

// UID for the application;
// this should correspond to the uid defined in the mmp file
#ifdef __SERIES60_3X__

const TUid KUidLocalizationApp = { 0xE0005B00 };

#else // on 2nd Ed, makesis doesn't like app UIDs above 0x80000000

const TUid KUidLocalizationApp = { 0x00005B00 };
    
#endif

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

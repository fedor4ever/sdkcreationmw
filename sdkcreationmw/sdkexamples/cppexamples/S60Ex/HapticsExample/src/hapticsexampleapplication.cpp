/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the main application class.
*
*/


#include "hapticsexampledocument.h"
#include "hapticsexampleapplication.h"


/* UID for the application;
 * this should correspond to the uid defined in the mmp file
 */
const TUid KUidHapticsExampleApp = { 0xE0007DFE };


// ---------------------------------------------------------------------------
// From class CApaApplication.
// Returns application UID
// ---------------------------------------------------------------------------
//
TUid CHapticsExampleApplication::AppDllUid() const
    {
    // Return the UID for the Haptics Example application
    return KUidHapticsExampleApp;
    }


// ---------------------------------------------------------------------------
// From class CApaApplication.
// Creates CApaDocument object
// ---------------------------------------------------------------------------
//
CApaDocument* CHapticsExampleApplication::CreateDocumentL()
    {
    // Create an HapticsExample document, and return a pointer to it
    return new ( ELeave ) CHapticsExampleDocument( *this );
    }

// End of File

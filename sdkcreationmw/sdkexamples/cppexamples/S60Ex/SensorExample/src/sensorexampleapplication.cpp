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
#include "sensorexampledocument.h"
#include "sensorexampleapplication.h"

// ============================ MEMBER FUNCTIONS ===============================

// UID for the application,
// this should correspond to the uid defined in the mmp file
const TUid KUidSensorExampleApp = { 0x2000B175 };

// -----------------------------------------------------------------------------
// CSensorExampleApplication::CreateDocumentL()
// Creates CApaDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CSensorExampleApplication::CreateDocumentL()
    {
    // Create an sensorexample document, and return a pointer to it
    return ( static_cast<CApaDocument*>( CSensorExampleDocument::NewL( *this ) ) );
    }

// -----------------------------------------------------------------------------
// CSensorExampleApplication::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CSensorExampleApplication::AppDllUid() const
    {
    // Return the UID for the sensorexample application
    return KUidSensorExampleApp;
    }

// End of File


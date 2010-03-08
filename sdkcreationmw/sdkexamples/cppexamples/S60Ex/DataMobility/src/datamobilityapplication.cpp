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
* Description:  Main application class
*
*/


// INCLUDE FILES
#include "datamobility.hrh"
#include "datamobilitydocument.h"
#include "datamobilityapplication.h"

// ============================ MEMBER FUNCTIONS ===============================

// UID for the application
// this should correspond to the uid defined in the mmp file
const TUid KUidDataMobilityApp = { _UID3 };

// -----------------------------------------------------------------------------
// CDataMobilityApplication::CreateDocumentL()
// Creates CApaDocument object
// -----------------------------------------------------------------------------
//
CApaDocument* CDataMobilityApplication::CreateDocumentL()
    {
    // Create an datamobility document, and return a pointer to it
    return ( static_cast<CApaDocument*>
                    ( CDataMobilityDocument::NewL( *this ) ) );
    }

// -----------------------------------------------------------------------------
// CDataMobilityApplication::AppDllUid()
// Returns application UID
// -----------------------------------------------------------------------------
//
TUid CDataMobilityApplication::AppDllUid() const
    {
    // Return the UID for the datamobility application
    return KUidDataMobilityApp;
    }

// End of File

/*
* Copyright (c) 2004 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include <eikstart.h>

#include "AknExQueryApp.h"
#include "AknExQueryDocument.h"

// ----------------------------------------------------------------------------
// CAknExQueryApp::CreateDocumentL()
// Creates CAknExQueryDocument object
// ----------------------------------------------------------------------------
//
CApaDocument* CAknExQueryApp::CreateDocumentL()
    {
    return CAknExQueryDocument::NewL( *this );
    }

// ----------------------------------------------------------------------------
// CAknExQueryApp::AppDllUid()
// Returns application UID
// ----------------------------------------------------------------------------
//
TUid CAknExQueryApp::AppDllUid() const
    {
    return KUidAknExQuery;
    }

// ================= OTHER EXPORTED FUNCTIONS =================================
//
// ----------------------------------------------------------------------------
// NewApplication()
// Constructs CAknExQueryApp
// Returns: CApaDocument*: created application object
// ----------------------------------------------------------------------------
//
EXPORT_C CApaApplication* NewApplication()
    {
    return new CAknExQueryApp;
    }

// ----------------------------------------------------------------------------
// TInt E32Main()
// Entry point function for EPOC Apps.
// Returns: TInt:   Error if starting the app through framework wasn't succesful
// ----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }

// End of File

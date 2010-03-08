/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include    "SatelliteReferenceApp.h"
#include    "SatelliteReferenceDocument.h"
#include 	<eikstart.h>

// ================= OTHER EXPORTED FUNCTIONS ==============
//
// ---------------------------------------------------------
// NewApplication() 
// Constructs CSatelliteReferenceApp
// Returns: created application object
// ---------------------------------------------------------
//
EXPORT_C CApaApplication* NewApplication()
    {
    return new CSatelliteReferenceApp;
    }

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CSatelliteReferenceApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CSatelliteReferenceApp::AppDllUid() const
    {
    return KUidSatelliteReferenceApp;
    }
  

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }
   
// ---------------------------------------------------------
// CSatelliteReferenceApp::CreateDocumentL()
// Creates SatelliteReferenceDocument object
// ---------------------------------------------------------
//
CApaDocument* CSatelliteReferenceApp::CreateDocumentL()
    {
    return CSatelliteReferenceDocument::NewL( *this );
    }



// End of File  


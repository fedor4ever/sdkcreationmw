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
* Description:  Implements the main application class.
*
*/


// INCLUDE FILES
#include "BrCtlSampleApp.h"
#include "BrCtlSampleAppDocument.h"
#include <eikstart.h>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CBrCtlSampleApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CBrCtlSampleApp::AppDllUid() const
    {
    return KUidBrCtlSampleApp;
    }

// ---------------------------------------------------------
// CDictionaryStore* CBrCtlSampleApp::OpenIniFileLC(RFs& aFs) const
// overrides CAknApplication::OpenIniFileLC to enable INI file support
// ---------------------------------------------------------
//
CDictionaryStore* CBrCtlSampleApp::OpenIniFileLC(RFs& aFs) const
{
    return CEikApplication::OpenIniFileLC(aFs);
}
   
// ---------------------------------------------------------
// CBrCtlSampleApp::CreateDocumentL()
// Creates CBrCtlSampleAppDocument object
// ---------------------------------------------------------
//
CApaDocument* CBrCtlSampleApp::CreateDocumentL()
    {
    return CBrCtlSampleAppDocument::NewL( *this );
    }

// ================= OTHER EXPORTED FUNCTIONS ==============
//
// ---------------------------------------------------------
// NewApplication() 
// Constructs CBrCtlSampleApp
// Returns: created application object
// ---------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication( )
	{
    return new CBrCtlSampleApp;
    }
    

// ---------------------------------------------------------
//    E32Main
//    Main function for Symbian OS v9 EXE application
//    Returns: TInt
// ---------------------------------------------------------
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }

// End of File  


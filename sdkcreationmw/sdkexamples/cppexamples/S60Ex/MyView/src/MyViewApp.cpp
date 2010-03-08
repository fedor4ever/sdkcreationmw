/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Application class launches the application, DLL entry point
*                is included here
*
*/



// INCLUDE FILES
#include    "MyViewApp.h"
#include    "MyViewDocument.h"

#ifdef __SERIES60_3X__
    #include <eikstart.h>
#endif

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CMyViewApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CMyViewApp::AppDllUid() const
    {
    return KUidMyView;
    }

   
// ---------------------------------------------------------
// CMyViewApp::CreateDocumentL()
// Creates CMyViewDocument object
// ---------------------------------------------------------
//
CApaDocument* CMyViewApp::CreateDocumentL()
    {
    return CMyViewDocument::NewL( *this );
    }

// ================= OTHER EXPORTED FUNCTIONS ==============
//
// ---------------------------------------------------------
// NewApplication() 
// Constructs CMyViewApp
// Returns: created application object
// ---------------------------------------------------------
//
EXPORT_C CApaApplication* NewApplication()
    {
    return new CMyViewApp;
    }


#ifdef __SERIES60_3X__  //////////// 3rd Ed

// -----------------------------------------------------------------------------
//	Entry point function for Symbian Apps (3rd Edition)
//  Returns: System Wide error codes or KErrNone if all goes well
// -----------------------------------------------------------------------------
GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication( NewApplication );
	}

#else /////////// 2nd Ed


// ---------------------------------------------------------
// E32Dll(TDllReason) 
// Entry point function for EPOC Apps
// Returns: KErrNone: No error
// ---------------------------------------------------------
//
GLDEF_C TInt E32Dll( TDllReason )
    {
    return KErrNone;
    }
    
#endif
    

// End of File  


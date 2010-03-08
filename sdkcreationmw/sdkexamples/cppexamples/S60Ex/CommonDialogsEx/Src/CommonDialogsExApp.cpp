/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements main application class.
*
*/


// INCLUDE FILES
#include    <eikstart.h>
#include    "CommonDialogsExApp.h"
#include    "CommonDialogsExDocument.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCommonDialogsExApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CCommonDialogsExApp::AppDllUid() const
    {
    return KUidDLLTest;
    }

// ---------------------------------------------------------
// CCommonDialogsExApp::CreateDocumentL()
// Creates CDLLTestDocument object
// ---------------------------------------------------------
//
CApaDocument* CCommonDialogsExApp::CreateDocumentL()
    {
    return CCommonDialogsExDocument::NewL( *this );
    }

// ================= OTHER EXPORTED FUNCTIONS ==============
//
// ---------------------------------------------------------
// NewApplication() 
// Constructs CCommonDialogsExApp
// Returns: CApaDocument*: created application object
// ---------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
    {
    return new CCommonDialogsExApp;
    }
    
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }
    
// End of File  


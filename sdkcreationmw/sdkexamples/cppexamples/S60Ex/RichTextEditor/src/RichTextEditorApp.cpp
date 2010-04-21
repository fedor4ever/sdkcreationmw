/*
* Copyright (c) 2006 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include    "RichTextEditorApp.h"
#include    "RichTextEditorDocument.h"

#include <eikstart.h>

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CRTEApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CRTEApp::AppDllUid() const
    {
    return KUidRTE;
    }

// ---------------------------------------------------------
// CRTEApp::CreateDocumentL()
// Creates CRTEDocument object
// ---------------------------------------------------------
//
CApaDocument* CRTEApp::CreateDocumentL()
    {
    return CRTEDocument::NewL( *this );
    }

// ================= OTHER EXPORTED FUNCTIONS ==============
//
// ---------------------------------------------------------
// NewApplication() 
// Constructs CRTEApp
// Returns: created application object
// ---------------------------------------------------------
//
EXPORT_C CApaApplication* NewApplication()
    {
    return new CRTEApp;
    }


// ---------------------------------------------------------
// E32Main()
// Entry point function for new (>= 9.0) EPOC Apps (exe)
// Returns: Sistem Wide error codes or KErrNone if all goes well
// ---------------------------------------------------------
//
GLDEF_C TInt E32Main()
{
	return EikStart::RunApplication( NewApplication );
}

// End of File  


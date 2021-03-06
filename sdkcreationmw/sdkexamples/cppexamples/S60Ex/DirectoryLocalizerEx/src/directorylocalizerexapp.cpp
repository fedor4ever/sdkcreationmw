/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  An application class, CDirectoryLocalizerExApp 
*				 - starting point of the program
*
*/


// INCLUDE FILES
#include "DirectoryLocalizerExApp.h"
#include "DirectoryLocalizerExDocument.h"
#include <eikstart.h>

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// TUid CDirectoryLocalizerEx::AppDllUid()
// Returns application UID.
// ----------------------------------------------------------------------------
//
TUid CDirectoryLocalizerExApp::AppDllUid() const
    {
    return KUidDirectoryLocalizerEx;
    }

// ----------------------------------------------------------------------------
// CApaDocument* CDirectoryLocalizerEx::CreateDocumentL()
// Creates CBCTestTemplateDocument object.
// ----------------------------------------------------------------------------
//
CApaDocument* CDirectoryLocalizerExApp::CreateDocumentL()
    {
    return CDirectoryLocalizerExDocument::NewL( *this );
    }

// ================= OTHER EXPORTED FUNCTIONS =================================
//
// ----------------------------------------------------------------------------
// CApaApplication* NewApplication()
// Constructs CDirectoryLocalizerEx.
// Returns: CApaDocument*: created application object
// ----------------------------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
    {
    return new CDirectoryLocalizerExApp();
    }

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }

// End of File

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
* Description:  An application class, CAddressBookApp 
*				 - starting point of the program
*
*/


// INCLUDES
#include "AddressBookApp.h"
#include "AddressBookDocument.h"
#include <eikstart.h>

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CAddressBookApp::CreateDocumentL()
// Creates CAddressBookDocument object and return a pointer to it 
// ----------------------------------------------------------------------------
//
CApaDocument* CAddressBookApp::CreateDocumentL()
    {  
    	return (static_cast<CApaDocument*>(CAddressBookDocument::NewL(*this))); 
    }

// ----------------------------------------------------------------------------
// AddressBookApp::AppDllUid()
// Returns application UID
// ----------------------------------------------------------------------------
//
TUid CAddressBookApp::AppDllUid() const
    {
        return KUidAddressBookApp;
    }

// ----------------------------------------------------------------------------
// NewApplication()
// Constructs CAddressBookApp
// Returns: created application object
// ----------------------------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication() 
{
    return (static_cast<CApaApplication*>(new CAddressBookApp));
}

// ----------------------------------------------------------------------------
// E32Main 
// Executable entry point
// ----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
{
    return EikStart::RunApplication(NewApplication);
}

// End of File


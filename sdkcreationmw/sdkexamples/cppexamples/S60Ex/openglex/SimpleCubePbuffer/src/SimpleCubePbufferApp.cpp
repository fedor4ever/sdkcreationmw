/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include    <eikstart.h>
#include    "SimpleCubePbufferApp.h"
#include    "SimpleCubePbufferDocument.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CSimpleCubePbufferApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CSimpleCubePbufferApp::AppDllUid() const
    {
    return KUidSimpleCubePbuffer;
    }


// ---------------------------------------------------------
// CSimpleCubePbufferApp::CreateDocumentL()
// Creates CSimpleCubePbufferDocument object
// ---------------------------------------------------------
//
CApaDocument* CSimpleCubePbufferApp::CreateDocumentL()
    {
    return CSimpleCubePbufferDocument::NewL( *this );
    }


// ================= OTHER EXPORTED FUNCTIONS ==============


CApaApplication* NewApplication()
    {
    return new CSimpleCubePbufferApp;
    }

TInt E32Main()
    {
        return EikStart::RunApplication(NewApplication);
    }

// End of File


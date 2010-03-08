/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include    "SimpleCubePixmapApp.h"
#include    "SimpleCubePixmapDocument.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CSimpleCubePixmapApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CSimpleCubePixmapApp::AppDllUid() const
    {
    return KUidSimpleCubePixmap;
    }

   
// ---------------------------------------------------------
// CSimpleCubePixmapApp::CreateDocumentL()
// Creates CSimpleCubePixmapDocument object
// ---------------------------------------------------------
//
CApaDocument* CSimpleCubePixmapApp::CreateDocumentL()
    {
    return CSimpleCubePixmapDocument::NewL( *this );
    }


// ================= OTHER EXPORTED FUNCTIONS ==============


CApaApplication* NewApplication()
    {
    return new CSimpleCubePixmapApp;
    }

TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }

// End of File  


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
#include    "TextureApp.h"
#include    "TextureDocument.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CTextureApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CTextureApp::AppDllUid() const
    {
    return KUidTexture;
    }


// ---------------------------------------------------------
// CTextureApp::CreateDocumentL()
// Creates CTextureDocument object
// ---------------------------------------------------------
//
CApaDocument* CTextureApp::CreateDocumentL()
    {
    return CTextureDocument::NewL( *this );
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

LOCAL_C CApaApplication* NewApplication()
    {
    return new CTextureApp;
    }

GLDEF_C TInt E32Main()
    {
        return EikStart::RunApplication(NewApplication);
    }

// End of File


/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Application class for Geo Profiles  Application.
*
*/


// SYSTEM INCLUDES
#include <eikstart.h>

// USER INCLUDES
#include "geoprofilesapp.h"
#include "geoprofilesdocument.h"
#include "geoprofilesuid.hrh"

// ------------------------ Local Functions ----------------------------------

// ---------------------------------------------------------------------------
// CApaApplication* NewApplication
// ---------------------------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
    {
    return new CGeoProfilesApp;
    }

// ----------------- Member funtions for CGeoProfilesApp ------------------------------
	
// ---------------------------------------------------------------------------
// TUid CGeoProfilesApp::AppDllUid
// ---------------------------------------------------------------------------
//
TUid CGeoProfilesApp::AppDllUid() const
    {
    return TUid::Uid( KGeoProfilesUiUid );
    }

// ---------------------------------------------------------------------------
// CApaDocument* CGeoProfilesApp::CreateDocumentL
// ---------------------------------------------------------------------------
//
CApaDocument* CGeoProfilesApp::CreateDocumentL()
    {
    return CGeoProfilesDocument::NewL( *this );
    }

// ------------------------ Global Functions ----------------------------------

// ---------------------------------------------------------------------------
// TInt E32Main
// ---------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
   }

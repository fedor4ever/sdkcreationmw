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
* Description:  Document class for Geo Profiles Application
*
*/


// INCLUDE FILES
#include "geoprofilesdocument.h"
#include "geoprofilesappui.h"

// ----------------- Member funtions for CGeoProfilesDocument class ------------------

// ---------------------------------------------------------------------------
// CGeoProfilesDocument::CGeoProfilesDocument
// ---------------------------------------------------------------------------
//
CGeoProfilesDocument::CGeoProfilesDocument( CEikApplication&  aApp )
	:CAknDocument( aApp )    
    {
    }

// ---------------------------------------------------------------------------
// CGeoProfilesDocument::~CGeoProfilesDocument
// ---------------------------------------------------------------------------
//
CGeoProfilesDocument::~CGeoProfilesDocument()
    { 
    }

// ---------------------------------------------------------------------------
// CGeoProfilesDocument* CGeoProfilesDocument::NewL
// ---------------------------------------------------------------------------
//
CGeoProfilesDocument* CGeoProfilesDocument::NewL( CEikApplication&    aApp )     
    {
    CGeoProfilesDocument* self = new (ELeave) CGeoProfilesDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    
    return self;
    }
    
// ---------------------------------------------------------------------------
// CGeoProfilesDocument::ConstructL
// ---------------------------------------------------------------------------
//
void CGeoProfilesDocument::ConstructL()
    {
    }
    
// ---------------------------------------------------------------------------
// CGeoProfilesDocument::CGeoProfilesDocument
// ---------------------------------------------------------------------------
//    
CEikAppUi* CGeoProfilesDocument::CreateAppUiL()
    {
    return new (ELeave) CGeoProfilesAppUi;
    }

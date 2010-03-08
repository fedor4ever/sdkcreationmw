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
#include "TextureDocument.h"
#include "TextureAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// constructor
CTextureDocument::CTextureDocument(CEikApplication& aApp)
: CAknDocument(aApp)
{
}

// destructor
CTextureDocument::~CTextureDocument()
{
}

// EPOC default constructor can leave.
void CTextureDocument::ConstructL()
{
}

// Two-phased constructor.
CTextureDocument* CTextureDocument::NewL(
        CEikApplication& aApp)     // CTextureApp reference
    {
    CTextureDocument* self = new (ELeave) CTextureDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------
// CTextureDocument::CreateAppUiL()
// constructs CTextureAppUi
// ----------------------------------------------------
//
CEikAppUi* CTextureDocument::CreateAppUiL()
{
    return new (ELeave) CTextureAppUi;
}


// End of File

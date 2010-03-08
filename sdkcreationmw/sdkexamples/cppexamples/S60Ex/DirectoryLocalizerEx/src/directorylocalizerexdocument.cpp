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
* Description:  Application Document class, CDirectoryLocalizerExDocument - 
*				 stores and restores the state of the application
*
*/


// INCLUDE FILES
#include "directorylocalizerexdocument.h"
#include "directorylocalizerexappui.h"

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CDirectoryLocalizerExDocument* CDirectoryLocalizerExDocument::NewL( CEikApplication& )
// Symbian OS two-phased constructor.
// ----------------------------------------------------------------------------
//
CDirectoryLocalizerExDocument* CDirectoryLocalizerExDocument::NewL( CEikApplication& aApp )
    {
    CDirectoryLocalizerExDocument* self = new( ELeave ) CDirectoryLocalizerExDocument( aApp );
    return self;
    }

// ----------------------------------------------------------------------------
// CDirectoryLocalizerExDocument::~CDirectoryLocalizerExDocument()
// Destructor.
// ----------------------------------------------------------------------------
//
CDirectoryLocalizerExDocument::~CDirectoryLocalizerExDocument()
    {
    }

// ----------------------------------------------------------------------------
// CDirectoryLocalizerExDocument::CDirectoryLocalizerExDocument( CEikApplication& )
// Overload constructor.
// ----------------------------------------------------------------------------
//
CDirectoryLocalizerExDocument::CDirectoryLocalizerExDocument( CEikApplication& aApp )
    :   CEikDocument( aApp )
    {
    }

// ----------------------------------------------------------------------------
// CEikAppUi* CDirectoryLocalizerExDocument::CreateAppUiL()
// Constructs CBCTestVolumeAppUi.
// ----------------------------------------------------------------------------
//
CEikAppUi* CDirectoryLocalizerExDocument::CreateAppUiL()
    {
    return new( ELeave ) CDirectoryLocalizerExAppUi;
    }

// End of File

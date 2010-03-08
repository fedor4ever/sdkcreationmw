/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include "IsvTelInfoAppApplication.h"
#include "IsvTelInfoAppUi.h"
#include "IsvTelInfoAppDocument.h"

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CIsvTelInfoAppDocument* CIsvTelInfoAppDocument::NewL( CAknApplication& )
// EPOC two-phased constructor.
// ----------------------------------------------------------------------------
//
CIsvTelInfoAppDocument* CIsvTelInfoAppDocument::NewL( CAknApplication& aApp )
    {
    CIsvTelInfoAppDocument* self = new( ELeave ) CIsvTelInfoAppDocument(aApp);
    return self;
    }

// ----------------------------------------------------------------------------
// CIsvTelInfoAppDocument::~CIsvTelInfoAppDocument()
// Destructor.
// ----------------------------------------------------------------------------
//
CIsvTelInfoAppDocument::~CIsvTelInfoAppDocument()
    {
    }

// ----------------------------------------------------------------------------
// CIsvTelInfoAppDocument::CIsvTelInfoAppDocument( CAknApplication& )
// Overload constructor.
// ----------------------------------------------------------------------------
//
CIsvTelInfoAppDocument::CIsvTelInfoAppDocument( CAknApplication& aApp )
    :   CAknDocument( aApp )
    {
    }

// ----------------------------------------------------------------------------
// CEikAppUi* CIsvTelInfoAppDocument::CreateAppUiL()
// Constructs CIsvTelInfoAppUi.
// ----------------------------------------------------------------------------
//
CEikAppUi* CIsvTelInfoAppDocument::CreateAppUiL()
    {
    iIsvTelInfoAppUiPtr = new( ELeave ) CIsvTelInfoAppUi;
    return (iIsvTelInfoAppUiPtr);
    }
    
// End of File

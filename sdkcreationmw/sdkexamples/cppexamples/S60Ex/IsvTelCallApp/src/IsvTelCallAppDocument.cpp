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
#include "IsvTelCallAppApplication.h"
#include "IsvTelCallAppUi.h"
#include "IsvTelCallAppDocument.h"

// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CIsvTelCallAppDocument* CIsvTelCallAppDocument::NewL( CAknApplication& )
// EPOC two-phased constructor.
// ----------------------------------------------------------------------------
//
CIsvTelCallAppDocument* CIsvTelCallAppDocument::NewL( CAknApplication& aApp )
    {
    CIsvTelCallAppDocument* self = new( ELeave ) CIsvTelCallAppDocument(aApp);
    return self;
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppDocument::~CIsvTelCallAppDocument()
// Destructor.
// ----------------------------------------------------------------------------
//
CIsvTelCallAppDocument::~CIsvTelCallAppDocument()
    {
    }

// ----------------------------------------------------------------------------
// CIsvTelCallAppDocument::CIsvTelCallAppDocument( CAknApplication& )
// Overload constructor.
// ----------------------------------------------------------------------------
//
CIsvTelCallAppDocument::CIsvTelCallAppDocument( CAknApplication& aApp )
:   CAknDocument( aApp )
    {
    }

// ----------------------------------------------------------------------------
// CEikAppUi* CIsvTelCallAppDocument::CreateAppUiL()
// Constructs CIsvTelCallAppUi.
// ----------------------------------------------------------------------------
//
CEikAppUi* CIsvTelCallAppDocument::CreateAppUiL()
    {
    iIsvTelCallAppUiPtr = new( ELeave ) CIsvTelCallAppUi;
    return (iIsvTelCallAppUiPtr);
    }
    
// End of File

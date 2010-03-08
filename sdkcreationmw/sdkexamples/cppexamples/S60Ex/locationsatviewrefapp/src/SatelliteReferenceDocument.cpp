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
#include "SatelliteReferenceDocument.h"
#include "SatelliteReferenceAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CSatelliteReferenceDocument::CSatelliteReferenceDocument()
// constructor
// ---------------------------------------------------------
//
CSatelliteReferenceDocument::CSatelliteReferenceDocument(CEikApplication& aApp)
: CAknDocument(aApp)    
    {
    }

// ---------------------------------------------------------
// CSatelliteReferenceDocument::~CSatelliteReferenceDocument()
// destructor
// ---------------------------------------------------------
//
CSatelliteReferenceDocument::~CSatelliteReferenceDocument()
    {
    }


// ---------------------------------------------------------
// CSatelliteReferenceDocument::ConstructL()
// EPOC default constructor can leave.
// ---------------------------------------------------------
//
void CSatelliteReferenceDocument::ConstructL()
    {
    }


// ---------------------------------------------------------
// CSatelliteReferenceDocument::NewL()
// Two-phased constructor.
// ---------------------------------------------------------
//
CSatelliteReferenceDocument* CSatelliteReferenceDocument::NewL(
        CEikApplication& aApp)     // SatelliteReferenceApp reference
    {
    CSatelliteReferenceDocument* self = new (ELeave) CSatelliteReferenceDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }
    
// ----------------------------------------------------
// CSatelliteReferenceDocument::CreateAppUiL()
// constructs SatelliteReference
// ----------------------------------------------------
//
CEikAppUi* CSatelliteReferenceDocument::CreateAppUiL()
    {
    return new (ELeave) CSatelliteReferenceAppUi;
    }

// End of File  

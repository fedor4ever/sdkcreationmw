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
#include "LmUiRefAppDocument.h"
#include "LmUiRefAppUi.h"
#include <EPos_CPosLandmarkDatabase.h>
#include <Epos_Landmarks.h>
#include <EPos_CPosLmOperation.h>



// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------------------------
// CLmUiRefAppDocument::CLmUiRefAppDocument
// constructor
// ---------------------------------------------------------------------------
//
CLmUiRefAppDocument::CLmUiRefAppDocument(CEikApplication& aApp)
: CAknDocument(aApp)    
    {
    }

// ---------------------------------------------------------------------------
// CLmUiRefAppDocument::~CLmUiRefAppDocument
// ---------------------------------------------------------------------------
//
CLmUiRefAppDocument::~CLmUiRefAppDocument()
    {
    delete iDb;
    }

// ---------------------------------------------------------------------------
// CLmUiRefAppDocument::ConstructL()
// ---------------------------------------------------------------------------
//
void CLmUiRefAppDocument::ConstructL()
    {
    //Open database and initilize it if needed
    iDb = CPosLandmarkDatabase::OpenL();
    CPosLmOperation* operation = NULL;
    if ( iDb->IsInitializingNeeded() )
        {
        operation = iDb->InitializeL();
        operation->ExecuteL();
        }
    }

// ---------------------------------------------------------------------------
// CLmUiRefAppDocument::NewL()
// ---------------------------------------------------------------------------
//
CLmUiRefAppDocument* CLmUiRefAppDocument::NewL(
        CEikApplication& aApp)     
    {
    CLmUiRefAppDocument* self = new (ELeave) CLmUiRefAppDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }
    
// ----------------------------------------------------
// CLmUiRefAppDocument::CreateAppUiL()
// constructs CLmUiRefAppUi
// ----------------------------------------------------
//
CEikAppUi* CLmUiRefAppDocument::CreateAppUiL()
    {
    return new (ELeave) CLmUiRefAppUi;
    }

// ---------------------------------------------------------------------------
// CLmUiRefAppDocument::LmDb
// ---------------------------------------------------------------------------
//
CPosLandmarkDatabase& CLmUiRefAppDocument::LmDb() const
    {
    return *iDb;
    }

// End of File  


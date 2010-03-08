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
* Description:  Implementation of CLandmarksDocument
*
*/


// INCLUDE FILES
#include "LandmarksAppUi.h"
#include "LandmarksDocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLandmarksDocument::CLandmarksDocument
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLandmarksDocument::CLandmarksDocument(CEikApplication& aApp) : CAknDocument(aApp) 
    {
    // No implementation required
    }   

// -----------------------------------------------------------------------------
// CLandmarksDocument::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLandmarksDocument::ConstructL()
    {
    // No implementation required
    }    

// -----------------------------------------------------------------------------
// CLandmarksDocument::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLandmarksDocument* CLandmarksDocument::NewL(CEikApplication& aApp)
    {
    CLandmarksDocument* self = new (ELeave) CLandmarksDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CLandmarksDocument::~CLandmarksDocument()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CLandmarksDocument::CreateAppUiL
// 
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
CEikAppUi* CLandmarksDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it,
    // the framework takes ownership of this object
    CEikAppUi* appUi = new (ELeave) CLandmarksAppUi;
    return appUi;
    }

// End of File


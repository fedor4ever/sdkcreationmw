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
* Description:  This class creates the user interface of the application
*
*/


// INCLUDE FILES
#include "LBSReferenceDocument.h"
#include "LBSReferenceAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// constructor
CLbsReferenceDocument::CLbsReferenceDocument(CEikApplication& aApp)
: CAknDocument(aApp)
    {
    // No implementation required
    }

// Symbian OS default constructor can leave.
void CLbsReferenceDocument::ConstructL()
    {
    // No implementation required
    }

// Two-phased constructor.
CLbsReferenceDocument* CLbsReferenceDocument::NewL(CEikApplication& aApp)
    {
    //Create the object
    CLbsReferenceDocument* self = new (ELeave) CLbsReferenceDocument( aApp );

    //Push the object to the cleanup stack
    CleanupStack::PushL( self );

    //Construct the object
    self->ConstructL();
    
    //Pop the object from the cleanup stack
    CleanupStack::Pop( self );
 
    //Return pointer to the created object
    return self;
    }

// destructor
CLbsReferenceDocument::~CLbsReferenceDocument()
    {
    // No implementation required
    }

// ----------------------------------------------------
// CLbsReferenceDocument::CreateAppUiL()
// constructs CLBSReferenceAppUi
// ----------------------------------------------------
//
CEikAppUi* CLbsReferenceDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it
    return new (ELeave) CLbsReferenceAppUi;
    }

// End of File

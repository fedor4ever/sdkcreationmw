/*
* Copyright (c) 2004 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include "AknExNoteDocument.h"
#include "AknExNoteAppUi.h"



// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CAknExNoteDocument::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAknExNoteDocument* CAknExNoteDocument::NewL( CEikApplication& aApp )
    {
    CAknExNoteDocument* self = new( ELeave ) CAknExNoteDocument( aApp );
    return self;
    }

// -----------------------------------------------------------------------------
// CAknExNoteDocument::~CAknExNoteDocument()
// Destructor.
// -----------------------------------------------------------------------------
//
CAknExNoteDocument::~CAknExNoteDocument()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CAknExNoteDocument::CAknExNoteDocument()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CAknExNoteDocument::CAknExNoteDocument( CEikApplication& aApp ):
                                        CEikDocument( aApp )
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// CAknExNoteDocument::CreateAppUiL()
// constructs CAknExNoteAppUi
// -----------------------------------------------------------------------------
//
CEikAppUi* CAknExNoteDocument::CreateAppUiL()
    {
    return new( ELeave ) CAknExNoteAppUi;
    }

// End of File

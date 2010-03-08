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
* Description:  Implements the document for application.
*
*/


// INCLUDE FILES
#include "BrCtlSampleAppDocument.h"
#include "BrCtlSampleAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// constructor
CBrCtlSampleAppDocument::CBrCtlSampleAppDocument(CEikApplication& aApp)
: CAknDocument(aApp)    
    {
    }

// destructor
CBrCtlSampleAppDocument::~CBrCtlSampleAppDocument()
    {
    }

// EPOC default constructor can leave.
void CBrCtlSampleAppDocument::ConstructL()
    {
    }

// Two-phased constructor.
CBrCtlSampleAppDocument* CBrCtlSampleAppDocument::NewL(
        CEikApplication& aApp)     // CBrCtlSampleApp reference
    {
    CBrCtlSampleAppDocument* self = new (ELeave) CBrCtlSampleAppDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }
    
// ----------------------------------------------------
// CBrCtlSampleAppDocument::CreateAppUiL()
// constructs CBrCtlSampleAppUi
// ----------------------------------------------------
//
CEikAppUi* CBrCtlSampleAppDocument::CreateAppUiL()
    {
    return new (ELeave) CBrCtlSampleAppUi;
    }

// End of File  


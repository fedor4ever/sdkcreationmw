/*
* Copyright (c) 2006 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include "RichTextEditorDocument.h"
#include "RichTextEditorAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// constructor
CRTEDocument::CRTEDocument(CEikApplication& aApp)
: CAknDocument(aApp)    
    {
    }

// destructor
CRTEDocument::~CRTEDocument()
    {
    }

// EPOC default constructor can leave.
void CRTEDocument::ConstructL()
    {
    }

// Two-phased constructor.
CRTEDocument* CRTEDocument::NewL(
        CEikApplication& aApp)     // CRTEApp reference
    {
    CRTEDocument* self = new (ELeave) CRTEDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();//Self

    return self;
    }
    
// ----------------------------------------------------
// CRTEDocument::CreateAppUiL()
// constructs CRTEAppUi
// ----------------------------------------------------
//
CEikAppUi* CRTEDocument::CreateAppUiL()
    {
    return new (ELeave) CRTEAppUi;
    }

// End of File  

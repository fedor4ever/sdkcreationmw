/*
* Copyright (c) 2002 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Document class creates AppUi class
*
*/


// INCLUDE FILES
#include "MyViewDocument.h"
#include "MyViewAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// constructor
CMyViewDocument::CMyViewDocument(CEikApplication& aApp)
: CAknDocument(aApp)    
    {
    }

// destructor
CMyViewDocument::~CMyViewDocument()
    {
    }

// EPOC default constructor can leave.
void CMyViewDocument::ConstructL()
    {
    }

// Two-phased constructor.
CMyViewDocument* CMyViewDocument::NewL(
        CEikApplication& aApp)     // CMyViewApp reference
    {
    CMyViewDocument* self = new (ELeave) CMyViewDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
// ----------------------------------------------------
// CMyViewDocument::CreateAppUiL()
// constructs CMyViewAppUi
// ----------------------------------------------------
//
CEikAppUi* CMyViewDocument::CreateAppUiL()
    {
    return new (ELeave) CMyViewAppUi;
    }

// End of File  

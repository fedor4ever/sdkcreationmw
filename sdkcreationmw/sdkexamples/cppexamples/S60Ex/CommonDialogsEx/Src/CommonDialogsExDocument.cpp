/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements CommonDialogsEx document.
*
*/


// INCLUDE FILES
#include "CommonDialogsExDocument.h"
#include "CommonDialogsExAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// destructor
CCommonDialogsExDocument::~CCommonDialogsExDocument()
    {
    }

// EPOC default constructor can leave.
void CCommonDialogsExDocument::ConstructL()
    {
    }

// Two-phased constructor.
CCommonDialogsExDocument* CCommonDialogsExDocument::NewL(
        CEikApplication& aApp)     
    {
    CCommonDialogsExDocument* self = new (ELeave) CCommonDialogsExDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }
    
// ----------------------------------------------------
// CCommonDialogsExDocument::CreateAppUiL()
// constructs CDLLTestAppUi
// ----------------------------------------------------
//
CEikAppUi* CCommonDialogsExDocument::CreateAppUiL()
    {
    return new (ELeave) CCommonDialogsExAppUi;
    }

// End of File  

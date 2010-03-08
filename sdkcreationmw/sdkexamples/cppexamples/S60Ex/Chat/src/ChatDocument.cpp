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
#include "ChatDocument.h"
#include "ChatAppui.h"

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CChatDocument::CChatDocument( CEikApplication& aApp )
// constructor
// ----------------------------------------------------------------------------
//
CChatDocument::CChatDocument( CEikApplication& aApp )
: CAknDocument( aApp )    
    {
    }

// ----------------------------------------------------------------------------
// CChatDocument::~CChatDocument()
// destructor
// ----------------------------------------------------------------------------
//
CChatDocument::~CChatDocument()
    {
    }

// ----------------------------------------------------------------------------
// CChatDocument::ConstructL()
// default constructor can leave.
// ----------------------------------------------------------------------------
//
void CChatDocument::ConstructL()
    {
    }

// ----------------------------------------------------------------------------
// CChatDocument* CChatDocument::NewL( CEikApplication& aApp )
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CChatDocument* CChatDocument::NewL( CEikApplication& aApp )   
    {
    CChatDocument* self = new (ELeave) CChatDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }
    
// ----------------------------------------------------
// CChatDocument::CreateAppUiL()
// constructs CChatAppUi
// ----------------------------------------------------
//
CEikAppUi* CChatDocument::CreateAppUiL()
    {
    return new (ELeave) CChatAppUi;
    }

// End of File  

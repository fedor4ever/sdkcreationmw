/*
* Copyright (c) 2005 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include "DynamicSettingListAppUi.h"
#include "DynamicSettingListDocument.h"

// ================= MEMBER FUNCTIONS =======================

// Constructor
CDynamicSettingListDocument::CDynamicSettingListDocument(CEikApplication& aApp) : CAknDocument(aApp) 
    {
	// no implementation required
    }

// Destructor
CDynamicSettingListDocument::~CDynamicSettingListDocument()
    {
	// no implementation required
    }

// ----------------------------------------------------
// CDynamicSettingListDocument::NewL()
// Two-phased constructor.
// ----------------------------------------------------
// 
CDynamicSettingListDocument* CDynamicSettingListDocument::NewL(CEikApplication& aApp)
    {
    CDynamicSettingListDocument* self = NewLC(aApp);
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------
// CDynamicSettingListDocument::NewLC()
// Two-phased constructor.
// ----------------------------------------------------
// 
CDynamicSettingListDocument* CDynamicSettingListDocument::NewLC(CEikApplication& aApp)
    {
    CDynamicSettingListDocument* self = new (ELeave) CDynamicSettingListDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------
// CDynamicSettingListDocument::ConstructL()
// Symbian OS default constructor can leave.
// ----------------------------------------------------
// 
void CDynamicSettingListDocument::ConstructL()
    {
	// no implementation required
    }    

// ----------------------------------------------------
// CDynamicSettingListDocument::CreateAppUiL()
// constructs CDynamicSettingListAppUi
// ----------------------------------------------------
//
CEikAppUi* CDynamicSettingListDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it,
    // the framework takes ownership of this object
    CEikAppUi* appUi = new (ELeave) CDynamicSettingListAppUi;
    return appUi;
    }

// End of file

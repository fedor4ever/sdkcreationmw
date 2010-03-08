/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Application Document class, CAddressBookDocument - 
*				 stores and restores the state of the application
*
*/


// INCLUDES
#include "AddressBookDocument.h"
#include "AddressBookAppUi.h"

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
// CAddressBookDocument::CAddressBookDocument(CEikApplication& aApp)
// constructor
// ----------------------------------------------------
//
CAddressBookDocument::CAddressBookDocument(CEikApplication& aApp)
: CAknDocument(aApp)    
    {
    }

// ----------------------------------------------------
// CAddressBookDocument::~CAddressBookDocument()
// destructor
// ----------------------------------------------------
//
CAddressBookDocument::~CAddressBookDocument()
    {
    }

// ----------------------------------------------------
// CAddressBookDocument::NewL(CEikApplication& aApp) 
// Two-phased constructor
// ----------------------------------------------------
//
CAddressBookDocument* CAddressBookDocument::NewL(
        CEikApplication& aApp)     // CAddressBookApp reference
    {
    CAddressBookDocument* self = new (ELeave) CAddressBookDocument( aApp );
    return self;
    }
    
// ----------------------------------------------------
// CAddressBookDocument::CreateAppUiL()
// constructs CAddressBookAppUi
// ----------------------------------------------------
//
CEikAppUi* CAddressBookDocument::CreateAppUiL()
    {
    return new (ELeave) CAddressBookAppUi;
    }

// End of File  


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
#include "IMOpenAPIExampleDocument.h"
#include "IMOpenAPIExampleAppUi.h"
#include "IMOpenAPIExamplemodel.h"

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
// CIMOpenAPIExampleDocument::CIMOpenAPIExampleDocument
// C++ Constructor
// ----------------------------------------------------
//
CIMOpenAPIExampleDocument::CIMOpenAPIExampleDocument(CEikApplication& aApp)
: CAknDocument(aApp)    
    {
    }

// ----------------------------------------------------
// CIMOpenAPIExampleDocument::~CIMOpenAPIExampleDocument
// Destructor
// ----------------------------------------------------
//
CIMOpenAPIExampleDocument::~CIMOpenAPIExampleDocument()
    {
    if(iModel)
	    delete iModel;
    iModel = NULL;
    }

// ----------------------------------------------------
// CIMOpenAPIExamplemodel& CIMOpenAPIExampleDocument::Model()
// ----------------------------------------------------
//
CIMOpenAPIExamplemodel& CIMOpenAPIExampleDocument::Model()
    {
    return *iModel;
    }

// ----------------------------------------------------
// CIMOpenAPIExampleDocument::ConstructL()
// Epoc constructor, can leave
// ----------------------------------------------------
//
void CIMOpenAPIExampleDocument::ConstructL()
    {
	iModel = CIMOpenAPIExamplemodel::NewL(this);
    }

// ----------------------------------------------------
// CIMOpenAPIExampleDocument* CIMOpenAPIExampleDocument::NewL
// Two phase constructor
// ----------------------------------------------------
//
CIMOpenAPIExampleDocument* CIMOpenAPIExampleDocument::NewL(
        CEikApplication& aApp)
    {
    CIMOpenAPIExampleDocument* self = new (ELeave) CIMOpenAPIExampleDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }
    
// ----------------------------------------------------
// CIMOpenAPIExampleDocument::CreateAppUiL()
// constructs CIMOpenAPIExampleAppUi
// ----------------------------------------------------
//
CEikAppUi* CIMOpenAPIExampleDocument::CreateAppUiL()
    {
    return new (ELeave) CIMOpenAPIExampleAppUi;
    }

// ----------------------------------------------------
// CIMOpenAPIExampleDocument::UI()
// return pointer to UI
// ----------------------------------------------------
//
CIMOpenAPIExampleAppUi* CIMOpenAPIExampleDocument::UI()
	{
	return iUI; 
	}

// ----------------------------------------------------
// CIMOpenAPIExampleDocument::Document()
// return pointer to document for use in model
// ----------------------------------------------------
//
CIMOpenAPIExampleDocument& CIMOpenAPIExampleDocument::Document()
    {
    return *this;
    }

// ----------------------------------------------------
// CIMOpenAPIExampleDocument::SetPointerToUI()
// sets the pointer to app ui, called by appui
// ----------------------------------------------------
//
void CIMOpenAPIExampleDocument::SetPointerToUI(CIMOpenAPIExampleAppUi* aUIPointer)
	{
	iUI = aUIPointer;
	}

// End of File  


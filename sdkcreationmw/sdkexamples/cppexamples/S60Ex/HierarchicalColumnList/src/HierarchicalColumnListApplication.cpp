/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Implementation for HierarchicalColumnListApplication class.
*
*/


#include "HierarchicalColumnListDocument.h"
#include "HierarchicalColumnListApplication.h"

// UID for the application, this should correspond to the uid defined
// in the mmp file
static const TUid KUidHierarchicalColumnListApp = { 0xE0007E00 };


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Create an HierarchicalColumnList document, and return a pointer to it.
// ---------------------------------------------------------------------------
//
CApaDocument* CHierarchicalColumnListApplication::CreateDocumentL()
    {  
    CApaDocument* document = CHierarchicalColumnListDocument::NewL( *this );
    return document;
    }


// ---------------------------------------------------------------------------
// Return the UID for the HierarchicalColumnList application.
// ---------------------------------------------------------------------------
//
TUid CHierarchicalColumnListApplication::AppDllUid() const
    {
    return KUidHierarchicalColumnListApp;
    }


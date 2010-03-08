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
* Description: 
*
*/



// INCLUDE FILES
#include "CSSyncDocument.h"
#include "CSSyncApplication.h"

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CCSSyncApplication::CreateDocumentL()
// Creates a CSSync document, and return a pointer to it
// -----------------------------------------------------------------------------
//
CApaDocument* CCSSyncApplication::CreateDocumentL()
    {
    return( static_cast<CApaDocument*>( CCSSyncDocument::NewL( *this ) ) );
    }

// -----------------------------------------------------------------------------
// CCSSyncApplication::AppDllUid()
// Return the UID for the CSSync application
// -----------------------------------------------------------------------------
//
TUid CCSSyncApplication::AppDllUid() const
    {
    return KUidCSSyncApp;
    }


// End of File

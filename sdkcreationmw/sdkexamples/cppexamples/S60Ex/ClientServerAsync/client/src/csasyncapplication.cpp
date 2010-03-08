/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include "CSAsyncDocument.h"
#include "CSAsyncApplication.h"

// ========================= MEMBER FUNCTIONS ==================================

// -----------------------------------------------------------------------------
// CCSAsyncApplication::CreateDocumentL()
// Creates a CSAsync document, and return a pointer to it
// -----------------------------------------------------------------------------
//
CApaDocument* CCSAsyncApplication::CreateDocumentL()
    {
    return ( static_cast<CApaDocument*>( CCSAsyncDocument::NewL( *this ) ) );
    }

// ----------------------------------------------------------------------------
// TUid CCSAsyncApplication::AppDllUid()
// Returns the UID for the CSAsync application
// ----------------------------------------------------------------------------
//
TUid CCSAsyncApplication::AppDllUid() const
    {
    return KUidCSAsyncApp;
    }


// End of File

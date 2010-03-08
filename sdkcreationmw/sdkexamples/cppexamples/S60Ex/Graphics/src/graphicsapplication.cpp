/*
* Copyright (c) 2004 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include "GraphicsDocument.h"
#include "GraphicsApplication.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CGraphicsApplication::CreateDocumentL()
// Creates CApaDocument object and returns a pointer to it
// -----------------------------------------------------------------------------
//
CApaDocument* CGraphicsApplication::CreateDocumentL()
    {
    //return ( static_cast<CApaDocument*> ( CGraphicsDocument::NewL( *this ) ) );
    return CGraphicsDocument::NewL( *this );
    }

// -----------------------------------------------------------------------------
// CGraphicsApplication::AppDllUid()
// Returns the UID for the FocusEvent application
// -----------------------------------------------------------------------------
//
TUid CGraphicsApplication::AppDllUid() const
    {
    return KUidGraphicsApp;
    }

// End of File

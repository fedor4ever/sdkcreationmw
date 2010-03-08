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
#include    "WebClientApp.h"
#include    "WebClientDocument.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CWebClientApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CWebClientApp::AppDllUid() const
    {
    return KUidWebClient;
    }

   
// ---------------------------------------------------------
// CWebClientApp::CreateDocumentL()
// Creates CWebClientDocument object
// ---------------------------------------------------------
//
CApaDocument* CWebClientApp::CreateDocumentL()
    {
    return CWebClientDocument::NewL( *this );
    }

// End of File  


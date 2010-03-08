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
#include    "ChatApp.h"
#include    "ChatDocument.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CChatApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CChatApp::AppDllUid() const
    {
    return KUidChat;
    }

   
// ---------------------------------------------------------
// CChatApp::CreateDocumentL()
// Creates CChatDocument object
// ---------------------------------------------------------
//
CApaDocument* CChatApp::CreateDocumentL()
    {
    return CChatDocument::NewL( *this );
    }

// End of File  


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
#include "animationdocument.h"
#include "animationapplication.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAnimationApplication::CreateDocumentL()
// Creates an Animation document and returns a pointer to it.
// -----------------------------------------------------------------------------
//
CApaDocument* CAnimationApplication::CreateDocumentL()
    {
    CApaDocument* document = CAnimationDocument::NewL( *this );
    return document;
    }

// -----------------------------------------------------------------------------
// CAnimationApplication::AppDllUid()
// Returns the UID for the Animation application.
// -----------------------------------------------------------------------------
//
TUid CAnimationApplication::AppDllUid() const
    {
    return KUidAnimationApp;
    }

// End of File

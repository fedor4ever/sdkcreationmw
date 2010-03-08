/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of the document class for the Haptics Example.
*
*/


#include "hapticsexampleappui.h"
#include "hapticsexampledocument.h"

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CHapticsExampleDocument::CHapticsExampleDocument( CEikApplication& aApp )
    : CAknDocument( aApp )
    {
    
    }


// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CHapticsExampleDocument::~CHapticsExampleDocument()
    {
    
    }


// ---------------------------------------------------------------------------
// Constructs CEikAppUi.
// ---------------------------------------------------------------------------
//
CEikAppUi* CHapticsExampleDocument::CreateAppUiL()
    {
    return  new ( ELeave ) CHapticsExampleAppUi;
    }

// End of File


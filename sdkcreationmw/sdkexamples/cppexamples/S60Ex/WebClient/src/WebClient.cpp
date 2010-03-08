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
#include <eikstart.h>
#include "WebClientApp.h"

// ---------------------------------------------------------
// NewApplication() 
// Constructs CWebClientApp
// Returns: created application object
// ---------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
    {
    return new CWebClientApp;
    }

// ---------------------------------------------------------
// E32Main()
// Main startup entry point
// ---------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }

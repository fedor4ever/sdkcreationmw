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
* Description:  Contains E32Main entry point, that launches the application
*
*/



#include "LbsReferenceApp.h"
#include <eikstart.h>

// ---------------------------------------------------------
// NewApplication()
// Constructs CLbsReferenceApp
// Returns: created application object
// ---------------------------------------------------------
//

EXPORT_C CApaApplication* NewApplication()
    {
    // Create a Reference application , and return a pointer to it
    return new CLbsReferenceApp;
    }


// ---------------------------------------------------------
// E32Main()
// The entry point for the application code. It creates, via a
// call to NewApplication() an instance of the CApaApplication
// derived class, CLbsReferenceApp.
// ---------------------------------------------------------

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }



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
#include <eikstart.h>

#include "CSSyncApplication.h"

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// NewApplication()
// Create a CSSync application, and return a pointer to it
// -----------------------------------------------------------------------------
//
EXPORT_C CApaApplication* NewApplication()
    {
    return ( static_cast<CApaApplication*>( new CCSSyncApplication ) );
    }


// -----------------------------------------------------------------------------
// E32Dll()
// DLL entry point, return that everything is ok
// -----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }

// End of File

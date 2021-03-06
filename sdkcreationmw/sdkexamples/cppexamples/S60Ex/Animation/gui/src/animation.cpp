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
#include "Animation.hrh"
#include "AnimationDocument.h"
#include "AnimationApplication.h"
#include <eikstart.h>


// ================= OTHER EXPORTED FUNCTIONS =================================

// ----------------------------------------------------------------------------
// CApaApplication* NewApplication()
// Constructs CAnimationApplication.
// Returns: CApaDocument*: created application object
// ----------------------------------------------------------------------------
//
EXPORT_C CApaApplication* NewApplication()
    {
    return new CAnimationApplication;
    }

// ----------------------------------------------------------------------------
// TInt E32Main()
// Entry point function for EPOC Apps.
// Returns: TInt:   Error if starting the app through framework wasn't succesful
// ----------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }

// End of File

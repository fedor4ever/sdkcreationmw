/*
* Copyright (c) 2005 - 2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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
#include "DynamicSettingListApplication.h"
#include <eikstart.h>


// Create an application, and return a pointer to it
EXPORT_C CApaApplication* NewApplication() 
    {
    return (new CDynamicSettingListApplication);
    }

// ---------------------------------------------------------
// E32Main 
// main function for Symbian OS v9 EXE application.
// ---------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication ( NewApplication );
    }


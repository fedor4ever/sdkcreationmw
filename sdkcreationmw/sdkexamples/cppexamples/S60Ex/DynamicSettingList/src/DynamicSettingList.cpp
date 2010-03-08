/*
* Copyright (c) 2005 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include "DynamicSettingListApplication.h"
#if defined __SERIES60_3X__
#include <eikstart.h>
#endif

// Create an application, and return a pointer to it
EXPORT_C CApaApplication* NewApplication() 
    {
    return (new CDynamicSettingListApplication);
    }

#if defined __SERIES60_3X__
// ---------------------------------------------------------
// E32Main 
// main function for Symbian OS v9 EXE application.
// ---------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication ( NewApplication );
    }

#else // !(__SERIES60_30__)

// ---------------------------------------------------------
// E32Dll(TDllReason) 
// Entry point function for Symbian Apps
// Returns: KErrNone: No error
// ---------------------------------------------------------
//
GLDEF_C TInt E32Dll( TDllReason )
    {
    return KErrNone;
    }
    
#endif // (__SERIES60_30__)    

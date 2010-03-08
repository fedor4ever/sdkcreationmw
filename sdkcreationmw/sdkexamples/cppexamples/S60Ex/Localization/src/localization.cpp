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
#ifdef __SERIES60_3X__
#include <eikstart.h>
#endif

#include "LocalizationApplication.h"

EXPORT_C CApaApplication* NewApplication()
	{
	return static_cast<CApaApplication*>( new CLocalizationApplication );
	}

#ifdef __SERIES60_3X__

GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication( NewApplication );
	}
	
#else

GLDEF_C TInt E32Dll( TDllReason )
    {
    return KErrNone;
    }

#endif


/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include "longtapdetectorexapp.h"
#include <eikstart.h>

/**
 * This example application shows some ways to use CAknButton
 * component. 
 */
LOCAL_C CApaApplication* NewApplication()
	{
	return new CLongTapDetectorExApp(); 
	}


GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication( NewApplication );
	}

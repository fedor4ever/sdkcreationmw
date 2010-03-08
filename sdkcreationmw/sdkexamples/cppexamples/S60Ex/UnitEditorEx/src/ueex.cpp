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
#include "UEExApplication.h"
#include <eikstart.h>

/**
 * This example application shows some ways to use CAknUnitEditor
 * component. CAknUnitEditor is a multi field numeric editor, MFNE. It supports
 * a floating point value and a unit label, which can either be a
 * localized pre-defined unit or custom string. In this example a form is
 * created to contain the unit editor, but a unit editor can also be used like
 * any basic CCoeControl derived control. For more infomation about the
 * control itself, see the documentation for CAknUnitEditor. For a more
 * detailed explanation about using the unit editor in this application
 * see the documentation for CUEExForm in file ueexform.h.
 */
LOCAL_C CApaApplication* NewApplication()
	{
	return new CUEExApplication;
	}


GLDEF_C TInt E32Main()
	{
	return EikStart::RunApplication( NewApplication );
	}

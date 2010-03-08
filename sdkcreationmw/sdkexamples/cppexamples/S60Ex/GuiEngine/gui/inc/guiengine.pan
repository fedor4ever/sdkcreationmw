/*
* Copyright (c) 2004, 2006 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __GUIENGINE_PAN__
#define __GUIENGINE_PAN__

// GuiEngine application panic codes

_LIT( KGuiEnginePanicName, "GuiEngine" );
enum TGuiEnginePanics
{
    EGuiEngineUnknownCommand = 1,               // Command unknown
    EGuiEngineInvalidBrushType = 2,     // Brush type is not correct
    EGuiEngineBasicUi = 100
};

#endif // __GUIENGINE_PAN__


// End of File

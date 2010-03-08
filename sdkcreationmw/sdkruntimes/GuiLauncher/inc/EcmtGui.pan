/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __ECMTGUI_PAN__
#define __ECMTGUI_PAN__

/** 
*  EcmtGui application panic codes
*  @ingroup EcmtGui
*/
enum TEcmtGuiPanics 
    {
    EEcmtGuiUi = 1
    // add further panics here
    };

inline void Panic(TEcmtGuiPanics aReason)
    {
		_LIT(applicationName,"EcmtGui");
    User::Panic(applicationName, aReason);
    }

#endif // __ECMTGUI_PAN__

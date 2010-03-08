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


#ifndef __FOCUSEVENT_PAN__
#define __FOCUSEVENT_PAN__

/** FocusEvent application panic codes */
enum TFocusEventPanics
    {
    EFocusEventBasicUi = 1
    // add further panics here
    };

inline void Panic(TFocusEventPanics aReason)
    {

    _LIT(applicationName,"FocusEvent");
    User::Panic(applicationName, aReason);
    }

#endif // __FOCUSEVENT_PAN__

// End of File

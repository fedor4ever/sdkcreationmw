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



#ifndef __UEEX_PAN__
#define __UEEX_PAN__

/** UEEx application panic codes */
enum TUEExPanics 
    {
    EUEExBasicUi = 1
    // add further panics here
    };

inline void Panic(TUEExPanics aReason)
    {
	_LIT(applicationName,"UEEx");
    User::Panic(applicationName, aReason);
    }

#endif // __UEEX_PAN__

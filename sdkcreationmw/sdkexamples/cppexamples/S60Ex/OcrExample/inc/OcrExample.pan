/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __OCREXAMPLE_PAN__
#define __OCREXAMPLE_PAN__

#include <e32std.h>

/** application panic codes */
enum TOcrExamplePanics
    {
    EOcrExampleUi = 1
    // add further panics here
    };

inline void Panic(TOcrExamplePanics aReason)
    {
    _LIT(applicationName,"OcrExample");
    User::Panic(applicationName, aReason);
    }

#endif // __OCREXAMPLE_PAN__

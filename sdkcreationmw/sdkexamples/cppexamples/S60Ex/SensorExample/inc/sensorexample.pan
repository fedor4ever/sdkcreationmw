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


#ifndef __SENSOREXAMPLE_PAN__
#define __SENSOREXAMPLE_PAN__

/** Sensorexample application panic codes */
enum TSensorExamplePanics
    {
    ESensorExampleUi = 1
    // add further panics here
    };

inline void Panic(TSensorExamplePanics aReason)
    {
    _LIT(applicationName, "sensorexample");
    User::Panic(applicationName, aReason);
    }

#endif // __SENSOREXAMPLE_PAN__

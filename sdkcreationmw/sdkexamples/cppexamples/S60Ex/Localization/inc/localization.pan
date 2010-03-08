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




#ifndef __LOCALIZATION_PAN__
#define __LOCALIZATION_PAN__

/** Localization application panic codes */
enum TLocalizationPanics
    {
    ELocalizationUi = 1
    // add further panics here
    };

inline void Panic(TLocalizationPanics aReason)
    {
    _LIT(applicationName,"Localization");
    User::Panic(applicationName, aReason);
    }

#endif // __LOCALIZATION_PAN__

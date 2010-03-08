/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Contains data and definitions used by several classes
*
*/


#ifndef __LANDMARKS_COMMON_DATA_H__
#define __LANDMARKS_COMMON_DATA_H__

// INCLUDES
#include <gulicon.h>
#include <AknIconUtils.h>

// DATA TYPES
typedef CArrayPtr<CGulIcon> CIconList;

// CONSTANTS
_LIT(KTab, "\t");
_LIT(KWildCard, "*");
_LIT(KLandmarksMbmFileName, "\\resource\\apps\\LmRefApp.mbm");

_LIT(KLatLongFormat, "%g");
_LIT(KListBoxLatLongFormat, "%S%d%c %d' %.3f\"");
_LIT(KPositiveSign, "+");
_LIT(KNegativeSign, "-");

const TInt KMinutesPerDegree = 60;
const TInt KSecondsPerMinute = 60;
const TInt KDegreeSign = 0x00B0;

const TInt KNrOfDecimals = 2;
const TInt KNrOfItemsDisplayed = 5;
const TInt KGranularity = 8;

const TReal KDbUsageLimit = 0.8;

const TInt KDefaultGlobalIconIndex = 0;     // see CreateIconListL()
const TInt KDefaultUserIconIndex = 1;

#endif // __LANDMARKS_COMMON_DATA_H__

// End of File


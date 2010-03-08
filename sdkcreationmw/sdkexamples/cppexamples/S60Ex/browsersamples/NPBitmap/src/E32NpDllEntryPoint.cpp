/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  test netscape plugins implementation
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <implementationproxy.h>
#include "CEcomBrowserPluginInterface.h"
#include "bitmapecommain.h"

// CONSTANTS
const TInt KNpBitMapUidValue = 0xA000018D;
const TImplementationProxy KImplementationTable[] =
    {
        {{KNpBitMapUidValue}, (TProxyNewLPtr) CBitmapEcomMain::NewL}
    };

// -----------------------------------------------------------------------------
// ImplementationGroupProxy
// Returns the filters implemented in this DLL
// Returns: The filters implemented in this DLL
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
{
    aTableCount = sizeof(KImplementationTable) / sizeof(TImplementationProxy);
    return KImplementationTable;
}


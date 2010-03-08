/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Main component of EcmtCommonPlugin DLL
*
*/



// INCLUDE FILES
#include "EcmtPanPlugin.h"
#include "EcmtEmuConfigPlugin.h"
#include <ecom.h>
#include <ImplementationProxy.h>
#include <bautils.h>
#include <barsc.h>


// ============================= LOCAL FUNCTIONS ===============================
/**
*    Bulk code needed by ECom and Symbian OS frameworks
*/

const TImplementationProxy ImplementationTable[] = 
    {
        { { KPANPLUGIN_IMPL_UID }, CEcmtPanPlugin::NewL },
        { { KEMUCONFIGPLUGIN_IMPL_UID }, CEcmtEmuConfigPlugin::NewL }
    };

// Exported proxy for instantiation method resolution
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }

//  End of File  

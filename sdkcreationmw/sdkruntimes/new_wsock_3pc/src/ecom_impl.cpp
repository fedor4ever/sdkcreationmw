// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// ECOM Implementation for the Winsock Provider Factories
// 
//

/**
 @file
 @internalComponent
*/


#include <ecom/implementationproxy.h>
#include <ecom/ecom.h>

//#include "winsockcprfactory.h"
//#include "winsockscprfactory.h"
//#include "winsocktiermanagerfactory.h"

#include "winsockflowfactory.h"
#include "winsockmcprfactory.h"
#include "../inc/winsockuid.h"


//#include "tunnelagentcprfactory.h"
// ---------------- ECOM Implementation ----------------

const TImplementationProxy ImplementationTable[] =
    {
   IMPLEMENTATION_PROXY_ENTRY(KWinsockFlowUid, CWinsockFlowFactory::NewL),
   IMPLEMENTATION_PROXY_ENTRY(KWinsockMcprUid, CWinsockMetaConnectionProviderFactory::NewL)
    };


EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }



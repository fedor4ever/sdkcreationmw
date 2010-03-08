/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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


#define TRACE_PREFIX "WSOCK: InterfaceFactory: "
#include "wsock.h"
#include "WinsockInterface.h"
#include "WinsockInterfaceFactory.h"

CWinsockInterfaceFactory::CWinsockInterfaceFactory()
{
}

CWinsockInterfaceFactory::~CWinsockInterfaceFactory()
{
}

// CNifIfFactory
CNifIfBase* CWinsockInterfaceFactory::NewInterfaceL(const TDesC& aName)
{
    return NewInterfaceL(aName, NULL);
}

CNifIfBase* CWinsockInterfaceFactory::NewInterfaceL(const TDesC& DEBUG_ONLY(aName), 
                                                    MNifIfNotify* aNotify)
{
    TRACE2("NewInterfaceL(%S,%08X)",&aName,aNotify);
    return new(ELeave)CWinsockInterface(this, aNotify);
}

TInt CWinsockInterfaceFactory::Info(TNifIfInfo& /*aInfo*/, 
                                    TInt DEBUG_ONLY(aIndex)) const
{
    TRACE1("Info(%d)",aIndex);
    return KErrNotFound;
}

// CNifFactory
void CWinsockInterfaceFactory::InstallL()
{
    TRACE("Install()");
}

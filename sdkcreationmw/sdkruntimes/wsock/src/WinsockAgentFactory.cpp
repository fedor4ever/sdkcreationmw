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


#define TRACE_PREFIX "WSOCK: AgentFactory: "
#include "wsock.h"
#include "WinsockAgent.h"
#include "WinsockAgentFactory.h"

CWinsockAgentFactory::CWinsockAgentFactory()
{
}

CWinsockAgentFactory::~CWinsockAgentFactory()
{
}

// CNifAgentFactory
CNifAgentBase * CWinsockAgentFactory::NewAgentL(const TDesC& /*aName*/)
{
    return CWinsockAgent::NewL();
}

TInt CWinsockAgentFactory::Info(TNifAgentInfo& /*aInfo*/,
                                TInt DEBUG_ONLY(aIndex)) const
{
    TRACE1("Info(%d)",aIndex);
    return KErrNotFound;
}

// CNifFactory
void CWinsockAgentFactory::InstallL()
{
    TRACE("Install()");
}

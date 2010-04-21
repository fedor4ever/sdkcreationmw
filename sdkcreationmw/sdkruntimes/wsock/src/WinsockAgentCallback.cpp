/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#define TRACE_PREFIX "WSOCK: AgentCallback: "
#include "wsock.h"
#include "WinsockAgentCallback.h"

CWinsockAgentCallback::CWinsockAgentCallback(CWinsockAgent* aAgent,
CWinsockAgent::CallbackMethod aMethod) : 
CActive(EPriorityStandard), 
iAgent(aAgent), 
iMethod(aMethod)
{
    CActiveScheduler::Add(this);
}

void CWinsockAgentCallback::DoCancel()
{
    TRACE("callback cancelled");
    TRequestStatus *status = (&iStatus);
    User::RequestComplete(status, KErrCancel);
}

void CWinsockAgentCallback::RunL()
{
    (iAgent->*iMethod)();
}

void CWinsockAgentCallback::InvokeLater()
{
    if (!IsActive())
    {
        SetActive();
        TRequestStatus *status = (&iStatus);
        User::RequestComplete(status, KErrNone);
    }
}

CWinsockAgent* CWinsockAgentCallback::Agent()
{
    return iAgent;
}

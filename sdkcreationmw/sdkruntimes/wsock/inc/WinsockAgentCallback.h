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


#ifndef _WSOCK_AGENT_CONNECTOR_H_
#define _WSOCK_AGENT_CONNECTOR_H_

#include "WinsockAgent.h"

// Fires delayed connection notification
class CWinsockAgentCallback : public CActive
{
private:
    CWinsockAgent* iAgent;
    CWinsockAgent::CallbackMethod iMethod;
public:
    CWinsockAgentCallback(CWinsockAgent* aAgent, 
        CWinsockAgent::CallbackMethod aMethod);

    void DoCancel();
    void RunL();

    void InvokeLater();
    CWinsockAgent* Agent();
};

#endif // _WSOCK_AGENT_CONNECTOR_H_

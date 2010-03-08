/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __JAVA_DEBUG_AGENT_CONNECTOR_H_
#define __JAVA_DEBUG_AGENT_CONNECTOR_H_

#include <in_sock.h>
#include <CommDbConnPref.h>
#include "JavaDebugAgentDriver.h"

class CJavaDebugAgentConnector : public CActive
{
private:
    enum TState
    {
        EIdle,
        EStarting,
        EResolving,
        EConnecting,
        EConnected,
        EFailed
    };

    TState iState;
    CJavaDebugAgentDriver* iDriver;
    TCommDbConnPref iConnPref;
    TInt iApId;
    RConnection iConnection;
    RSocket* iSocket;
    CTcpServerHandler* iHandler;
    RHostResolver iResolver;
    TNameEntry iNameEntry;
    TInetAddr iAddress;

public:
    CJavaDebugAgentConnector(CJavaDebugAgentDriver* aDriver, TInt aApId);
    ~CJavaDebugAgentConnector();

    // Initiates the connection
    void ConnectL();

    // CActive
    void RunL();
    TInt RunError(TInt aError);
    void DoCancel();

private:
    RSocketServ& SocketServ();
    TBool AlreadyConnected(TUint32 aIapId);
    void DoResolveL();
    void DoConnectL();
};

// INLINE METHODS

inline RSocketServ& CJavaDebugAgentConnector::SocketServ()
    { return iDriver->SocketServ(); }

#endif // __JAVA_DEBUG_AGENT_CONNECTOR_H_

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

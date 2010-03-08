/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  MIDP debug session context
 *
*/

#ifndef _ECMT_MIDP_DEBUG_SESSION_H_
#define _ECMT_MIDP_DEBUG_SESSION_H_

#include <es_sock.h>
#include <in_sock.h>

/**
 * MIDP Debug session
 */
class MEcmtMessaging;
class MEcmtMidpDebugSessionObserver;
class CEcmtMidpDebugConnection;
class CEcmtMidpDebugSession : public CActive
{
private:
    enum TFlags {
        FLAG_AGENT_STARTED = 0x0001,
        FLAG_SESSION_CLOSED = 0x0002
    };

    TUint iSessionId;
    MEcmtMidpDebugSessionObserver* iListener;
    MEcmtMessaging* iMessaging;
    RSocketServ iSocketServ;
    RProcess iDebugAgentProcess;
    TInt iFlags;
    RPointerArray<CEcmtMidpDebugConnection> iConnections;
    RPointerArray<CEcmtMidpDebugConnection> iDeadConnections;

public:

    // CActive
    virtual void DoCancel();
    virtual void RunL();

    static CEcmtMidpDebugSession* NewL(TInt aSessionId,
                                       MEcmtMidpDebugSessionObserver* aListnr,
                                       MEcmtMessaging* aMessaging);
    virtual ~CEcmtMidpDebugSession();

    void ConnectL(TUint aConnId, TUint aPort);
    void Send(TUint aConnId, TUint aSeq, const TDesC8& aData);
    void Reset(TUint aConnId);

    inline TUint SessionId() const;
    inline MEcmtMessaging* Messaging();
    inline RSocketServ* SocketServ();
    void ConnectionClosed(CEcmtMidpDebugConnection* aConnection);
    void FillHeader(TDes8& aHeader, TUint8 aOpCode);
    TBool DataReceivedFromDebugAgent(const TDesC8& aData);

private:
    CEcmtMidpDebugSession(TInt aSessionId,
                          MEcmtMidpDebugSessionObserver* aListener,
                          MEcmtMessaging* aMessaging);
    void ConstructL();
    void StartDebugAgentL(TInt aPort);
    void KillDebugAgent();
    TInt FindFreePortL();
    void CloseSession();
    void FlushDeadConnections();
    CEcmtMidpDebugConnection* FindConnection(TUint aConnId, 
                                             TInt& aIndex) const;
};

// Inline functions
inline TUint CEcmtMidpDebugSession::SessionId() const 
    { return iSessionId; }
inline MEcmtMessaging* CEcmtMidpDebugSession::Messaging()
    { return iMessaging; }
inline RSocketServ* CEcmtMidpDebugSession::SocketServ()
    { return &iSocketServ; }

#endif // _ECMT_MIDP_DEBUG_SESSION_H_

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

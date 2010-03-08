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
* Description:  MIDP connection context
 *
*/

#ifndef _ECMT_MIDP_DEBUG_CONNECTION_H_
#define _ECMT_MIDP_DEBUG_CONNECTION_H_

#include <es_sock.h>
#include <in_sock.h>

/**
 * MIDP connection context.
 */
class MEcmtMessaging;
class CEcmtMidpDebugSession;
class CEcmtMidpDebugConnection : public CActive
{
private:
    enum TState {
        STATE_NOT_CONNECTED,
        STATE_CONNECTING,
        STATE_SLEEPING,
        STATE_READING
    };

    static const TInt MAX_CONNECT_ATTEMPTS = 5000;
    static const TUint KMaxSocketReadBuffer = 2048;

    TUint iConnId;
    TUint iGatewaySeq;
    TUint iDebugAgentSeq;

    CEcmtMidpDebugSession* iSession;
    TInetAddr iSeiAddr;
    TInt iConnectAttempt;
    TInt iConnectRetryDelay;
    RSocket iSeiSocket;
    RTimer iTimer;

    TBool iClosed;
    TState iConnState;
    TUint8* iReadBuffer;
    TPtr8 iReadPtr;
    TSockXfrLength iReadXfrLength;

    HBufC8* iWriteBuffer;

public:

    // CActive
    virtual void DoCancel();
    virtual void RunL();

    static CEcmtMidpDebugConnection* NewLC(CEcmtMidpDebugSession* aSession,
                                           TInt aConnId, TInt aPort,
                                           TInt aConnectRetryDelay);

    virtual ~CEcmtMidpDebugConnection();
    TBool Send(TUint aSeq, const TDesC8& aData);
    inline TUint ConnId() const;

private:
    CEcmtMidpDebugConnection(CEcmtMidpDebugSession* aSession, TInt aConnId,
                             TInt aConnectRetryDelay);

    void ConstructL(TInt aPort);
    void PutConnId(TDes8& aPacket, TInt aOffset) const;
    TBool Receive(const TDesC8& aData);
    void IssueRead();
    void Close();
};

// Inline functions
inline TUint CEcmtMidpDebugConnection::ConnId() const 
    { return iConnId; }

#endif // _ECMT_MIDP_DEBUG_CONNECTION_H_

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

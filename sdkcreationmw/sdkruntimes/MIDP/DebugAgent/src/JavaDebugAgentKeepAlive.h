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


#ifndef __JAVA_DEBUG_AGENT_KEEP_ALIVE_H_
#define __JAVA_DEBUG_AGENT_KEEP_ALIVE_H_

#include <in_sock.h>
#include "JavaDebugAgentLog.h"

class CJavaDebugAgentKeepAlive : public CActive
{
private:
    class CReceiver;
    enum TState
    {
        EIdle,
        EConnecting,
        ESending,
        EWaiting,
        EFailed
    };

    TState iState;
    RSocket iSocket;
    RTimer iTimer;
    TTimeIntervalMicroSeconds32 iPeriod;
    RSocketServ* iSocketServ;
    CReceiver* iReceiver;
    TInetAddr iAddress;
    MJavaDebugAgentLog* iLog;
    TBuf8<12> iMsg;
    TUint16 iSeq;

public:
    static CJavaDebugAgentKeepAlive* NewL(RSocketServ* aSocketServ, 
        const TSockAddr* aAddr, MJavaDebugAgentLog* aLog, 
        TInt aPeriodInMilliseconds);
    static CJavaDebugAgentKeepAlive* NewLC(RSocketServ* aSocketServ, 
        const TSockAddr* aAddr, MJavaDebugAgentLog* aLog, 
        TInt aPeriodInMilliseconds);
    ~CJavaDebugAgentKeepAlive();
    const TInetAddr& Address() const;

protected:
    // CActive
    void RunL();
    TInt RunError(TInt aError);
    void DoCancel();

private:
    CJavaDebugAgentKeepAlive(RSocketServ* aSocketServ, 
        const TSockAddr* aAddr, MJavaDebugAgentLog* aLog,
        TInt aPeriodInMilliseconds);
    void ConstructL();
    void Wait();
    void Ping();
};

// INLINE METHODS

inline const TInetAddr& CJavaDebugAgentKeepAlive::Address() const
    { return iAddress; }

#endif // __JAVA_DEBUG_AGENT_KEEP_ALIVE_H_

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

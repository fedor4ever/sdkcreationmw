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


#ifndef __JAVA_DEBUG_AGENT_DRIVER_H_
#define __JAVA_DEBUG_AGENT_DRIVER_H_

#include <es_sock.h>
#include "Driver.h"
#include "JavaDebugAgentKeepAlive.h"

class CTcpServer;
class CTcpServerHandler;
class MJavaDebugAgentLog;
class MJavaDebugAgentObserver;
class CJavaDebugAgentConnector;
class CJavaDebugAgentDriver : public CDriver
{
public:
    enum TState {
        EIdle,
        EConnecting,
        EListening,
        EConnected,
        EDisconnected,
        EDeleted
    };

private:
    RFs iFs;
    RSocketServ iSocketServ;
    CAgentEngine* iEngine;
    CTcpServer* iServer;
    CAsyncCallBack* iAsyncCleanup;
    RPointerArray<CTcpServerHandler> iHandlers;
    RPointerArray<CTcpServerHandler> iDeadHandlers;
    RPointerArray<CJavaDebugAgentKeepAlive> iKeepAliveList;
    MJavaDebugAgentLog* iLog;
    MJavaDebugAgentObserver* iObserver;
    CJavaDebugAgentConnector* iConnector;
    TInt iApId;
    TInt iKeepAlivePeriod; // in milliseconds
    TState iState;

public:
    static CJavaDebugAgentDriver* NewLC(MJavaDebugAgentLog* aLog);
    static CJavaDebugAgentDriver* NewL(MJavaDebugAgentLog* aLog);
    ~CJavaDebugAgentDriver();

    void StartListeningL();
    void ConnectL();
    void SetObserver(MJavaDebugAgentObserver* aObserver);
    MJavaDebugAgentLog* Log();
    RSocketServ& SocketServ();
    TState State() const;

    // Access to parameters not defined in the base class
    TInt ApId() const;
    TInt KeepAlivePeriod() const;
    void SetApId(TInt aApId);
    void SetKeepAlivePeriod(TInt aMilliSeconds);

    // Notification from CJavaDebugAgentConnector
    void ConnectionFailed();

    // CDriver
    virtual void InitL();
    virtual void Log(const TDesC& aString);
    virtual void Message(const TDesC& aString);
    virtual RFs& Fs();
    virtual CAgentEngine* Engine();
    virtual void Shutdown();
    virtual void SettingsChange();

    // Notifications from CTcpServerHandler
    virtual void ClientConnected(CTcpServerHandler* aHandler);
	virtual void ClientDisconnected(CTcpServerHandler* aHandler);

private:
    void ConstructL();
    CJavaDebugAgentDriver(MJavaDebugAgentLog* aLog);
    TBool CheckIfIdle() const;
    void SetState(TState aState);
    void InitKeepAliveL(const TSockAddr* aRemoteAddr);
    void DeleteConnector();
    static TInt AsyncCleanup(TAny* aThis);
};

// Observer interface
class MJavaDebugAgentObserver
{
public:
    virtual void StateChanged(CJavaDebugAgentDriver::TState aState) = 0;
};

// INLINE METHODS

inline MJavaDebugAgentLog* CJavaDebugAgentDriver::Log()
    { return iLog; }
inline RSocketServ& CJavaDebugAgentDriver::SocketServ()
    { return iSocketServ; }
inline TInt CJavaDebugAgentDriver::ApId() const
    { return iApId; }
inline TInt CJavaDebugAgentDriver::KeepAlivePeriod() const
    { return iKeepAlivePeriod; }
inline void CJavaDebugAgentDriver::SetApId(TInt aApId)
    { iApId = aApId; }
inline void CJavaDebugAgentDriver::SetKeepAlivePeriod(TInt aMilliSeconds)
    { iKeepAlivePeriod = aMilliSeconds; }
inline void CJavaDebugAgentDriver::SetObserver(MJavaDebugAgentObserver* aObserver)
    { iObserver = aObserver; }
inline CJavaDebugAgentDriver::TState CJavaDebugAgentDriver::State() const
    { return iState; }

#endif // __JAVA_DEBUG_AGENT_DRIVER_H_

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

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


#include <e32math.h>

#include "TcpServer.h"
#ifdef DEBUG_AGENT_ENGINE_AMS
#include "AmsEngine.h"
#else
#include "SwiEngine.h"
#endif

#include "JavaDebugAgentLog.h"
#include "JavaDebugAgentDriver.h"
#include "JavaDebugAgentConnector.h"

#define SUPER CDriver

const TInt KPinLength = 4;

CJavaDebugAgentDriver* CJavaDebugAgentDriver::NewLC(MJavaDebugAgentLog* aLog)
{
    CJavaDebugAgentDriver* self = new(ELeave)CJavaDebugAgentDriver(aLog);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

CJavaDebugAgentDriver* CJavaDebugAgentDriver::NewL(MJavaDebugAgentLog* aLog)
{
    CJavaDebugAgentDriver* self = NewLC(aLog);
    CleanupStack::Pop(self);
    return self;
}

CJavaDebugAgentDriver::CJavaDebugAgentDriver(MJavaDebugAgentLog* aLog) : 
iLog(aLog)
{
}

CJavaDebugAgentDriver::~CJavaDebugAgentDriver()
{
    DeleteConnector();
    SetState(EDeleted);

    iDeadHandlers.ResetAndDestroy();
    iHandlers.ResetAndDestroy();
    iKeepAliveList.ResetAndDestroy();
    delete iAsyncCleanup;
    delete iServer;
    delete iEngine;
    iFs.Close();
    iSocketServ.Close();
}

void CJavaDebugAgentDriver::ConstructL()
{
    TCallBack cb(AsyncCleanup,this);
    iAsyncCleanup = new(ELeave)CAsyncCallBack(cb,CActive::EPriorityStandard);
    User::LeaveIfError(iSocketServ.Connect());
    User::LeaveIfError(iFs.Connect());
    iFs.ShareProtected();
}

void CJavaDebugAgentDriver::InitL()
{
#ifdef DEBUG_AGENT_ENGINE_AMS
    iEngine = new (ELeave)CAmsEngine(this);
#else
    iEngine = new (ELeave)CSwiEngine(this);
#endif

    // setup pin code
    if (IsUsePin())
    {
        // generate random KPinLength digit pin
        TInt rand = (TInt)(0x7fffffff & (Math::Random()));
        TBuf<32> buf;
        buf.AppendNum(rand);
        SetPinCodeL(buf.Left(KPinLength));
        iLog->LogFormat(_S("Pin code for this session: %S"),GetPinCode());
    }
}

TInt CJavaDebugAgentDriver::AsyncCleanup(TAny* aThis)
{
    ((CJavaDebugAgentDriver*)aThis)->iDeadHandlers.ResetAndDestroy();
    return KErrNone;
}

TBool CJavaDebugAgentDriver::CheckIfIdle() const
{
    if (iServer)
    {
        iLog->Log(_S("Already listening!"));
        return EFalse;
    }
    if (iHandlers.Count() > 0)
    {
        iLog->Log(_S("Already connected!"));
        return EFalse;
    }
    return ETrue;
}

void CJavaDebugAgentDriver::StartListeningL()
{
    if (!CheckIfIdle()) return;
    CTcpServer* server = CTcpServer::NewLC(this);
    TInetAddr address(GetAgentPort());
    TRAPD(err,server->StartListeningL(_L("tcp"), address, 10));
    if (err == KErrNone)
    {
        CleanupStack::Pop();
        iServer = server;
        SetState(EListening);
    }
    else
    {
        iLog->LogFormat(_S("Server failed, error %d"),err);
        User::Leave(err);
    }
}

void CJavaDebugAgentDriver::DeleteConnector()
{
    // Note: CJavaDebugAgentConnector destructor may invoke ConnectionFailed
    // and we want iConnector pointer be NULL to avoid double delete.
    CJavaDebugAgentConnector* connector = iConnector;
    iConnector = NULL;
    delete connector;
}

void CJavaDebugAgentDriver::ConnectL()
{
    if (!CheckIfIdle()) return;
    DeleteConnector();
    iConnector = new(ELeave)CJavaDebugAgentConnector(this, iApId);
    iConnector->ConnectL();
    SetState(EConnecting);
}

void CJavaDebugAgentDriver::SetState(TState aState)
{
    if (iState != aState)
    {
        iState = aState;
        if (iObserver)
        {
            iObserver->StateChanged(aState);
        }
    }
}

void CJavaDebugAgentDriver::Log(const TDesC& aString)
{
    iLog->LogFormat(_S("%S"),&aString);
}

RFs& CJavaDebugAgentDriver::Fs()
{
    return iFs;
}

void CJavaDebugAgentDriver::Shutdown()
{
    DeleteConnector();
    delete iServer;
    iServer = NULL;
}

CAgentEngine* CJavaDebugAgentDriver::Engine() 
{
    return iEngine;
}

void CJavaDebugAgentDriver::Message(const TDesC& aString)
{
    Log(aString);
}

void CJavaDebugAgentDriver::SettingsChange()
{
    // ????
}

void CJavaDebugAgentDriver::InitKeepAliveL(const TSockAddr* aRemoteAddr)
{
    CJavaDebugAgentKeepAlive* keepAlive = 
        CJavaDebugAgentKeepAlive::NewLC(&iSocketServ, aRemoteAddr, iLog,
                                        iKeepAlivePeriod);
    User::LeaveIfError(iKeepAliveList.Append(keepAlive));
    CleanupStack::Pop(keepAlive);
}

// Notifications from CTcpServerHandler
void CJavaDebugAgentDriver::ClientConnected(CTcpServerHandler* aHandler)
{
    if (iHandlers.Append(aHandler) == KErrNone)
    {
        if (iKeepAlivePeriod > 0)
        {
            TInetAddr local;
            TInetAddr remote;
            aHandler->Socket()->LocalName(local);
            aHandler->Socket()->RemoteName(remote);

            // No ping for local connections (as in case of debugging over
            // Bluetooth)
            if (!local.Match(remote))
            {
                TRAPD(err, InitKeepAliveL(&remote));
                if (err == KErrNone)
                {
                    iLog->Log(_S("Starting keep-alive timer"));
                }
                else
                {
                    // Ignore the error, it shoudn't be fatal
                    iLog->LogFormat(_S("Keep-alive error %d"),err);
                }
            }
        }

        if (iHandlers.Count() == 1) 
        {
            // First client
            SetState(EConnected);
        }
    }
    else
    {
        // This connection will work but we will leak some memory.
        // Suggest the user to restart the application
        iLog->Log(_S("Internal error. Please restart debug agent."));
    }
}

void CJavaDebugAgentDriver::ClientDisconnected(CTcpServerHandler* aHandler)
{
    TInt handlerPos = iHandlers.Find(aHandler);
    if (handlerPos >= 0) {
        TInetAddr remote;
        RSocket* socket = aHandler->Socket();
        socket->LocalName(remote);

        // Delete a matching KeepAlive object. It doesn't matter which one.
        // Ideally we shouldn't create more than one KeepAlive per address
        // but we normally have no more than one client, so it's doesn't
        // really matter.
        for (TInt i = 0; i<iKeepAliveList.Count(); i++)
        {
            if (remote.Match(iKeepAliveList[i]->Address()))
            {
                delete iKeepAliveList[i];
                iKeepAliveList.Remove(i);
                break;
            }
        }

        // It's not obvious how we can recover from the situation when
        // we can't this handler to iDeadHandlers array. It's not safe
        // to delete the handler here because this callback is invoked 
        // by the handler, and it may access its this pointer after this
        // callback has returned. It that case, let's leave it in iHandlers
        // array. It screws things up but at least we don't leak memory.
        // Hopefully, this won't happen too often...
        if (iDeadHandlers.Append(aHandler) == KErrNone)
        {
            iHandlers.Remove(handlerPos);
            if (iHandlers.Count() == 0) {
                // It was our last client
                SetState(iServer ? EListening : EDisconnected);
            }
            // Schedule callback on a clean stack where we can safely
            // delete the handler
            iAsyncCleanup->CallBack();
        }
    }
    
    //Dima: Cleanup on start in InitL conflicts with installation
    //seemingly due to errors in Installer (ECOM framework panics
    //on loading midp2installerplugin.dll on installation after cleanup)
    //so we clean up on client disconnect.
    if (IsAutoCleanEnabled())
    {
        Log(_L("Cleaning up old data..."));
        iEngine->Cleanup();
        Log(_L("Done cleaning..."));
    }
}

// Notification from CJavaDebugAgentConnector
void CJavaDebugAgentDriver::ConnectionFailed()
{
    DeleteConnector();
    SetState(EDisconnected);
}

/**
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

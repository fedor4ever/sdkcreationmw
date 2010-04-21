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


#define TRACE_PREFIX "WSOCK: Agent: "
#include "wsock.h"
#include "WinsockAgent.h"
#include "WinsockAgentCallback.h"
#include "WinsockProtocolFamily.h"

#ifdef SYMBIAN_NETWORKING_PLATSEC
#define ACCESS_METHOD1(_ret,_what,_p1) \
    _ret CWinsockAgent::Do##_what(_p1, const RMessagePtr2* /*aMessage*/ )
#define ACCESS_METHOD2(_ret,_what,_p1,_p2) \
    _ret CWinsockAgent::Do##_what(_p1, _p2, const RMessagePtr2* /*aMessage*/ )
#else /* SYMBIAN_NETWORKING_PLATSEC */
#define ACCESS_METHOD1(_ret,_what,_p1) \
    _ret CWinsockAgent::_what(_p1)
#define ACCESS_METHOD2(_ret,_what,_p1,_p2) \
    _ret CWinsockAgent::_what(_p1,_p2)
#endif /* SYMBIAN_NETWORKING_PLATSEC */

// Settings
_LIT(KIfName,"IfName");                 // IF_NAME
_LIT(KIfNetworks,"IfNetworks");         // SERVICE_IF_NETWORKS
_LIT(KIAPBearerType,"IAP\\IAPBearerType");
_LIT(KIAPServiceType,"IAP\\IAPServiceType");
_LIT(KIAPName,"IAP\\Name");
_LIT(KIAPId,"IAP\\Id");
_LIT(KIAPService,"IAP\\IAPService");
_LIT(KIAPNetwork,"IAP\\IAPNetwork");
_LIT(KLANBearerName,"LANBearer\\Name");
_LIT(KLANBearerLastSocketActivityTimeout,"LANBearer\\LastSocketActivityTimeout");
_LIT(KLANBearerLastSocketActivityTimeout2,"LastSocketActivityTimeout");
_LIT(KLANBearerLastSessionClosedTimeout,"LANBearer\\LastSessionClosedTimeout");
_LIT(KLANBearerLastSessionClosedTimeout2,"LastSessionClosedTimeout");
_LIT(KLANBearerLastSocketClosedTimeout,"LANBearer\\LastSocketClosedTimeout");
_LIT(KLANBearerLastSocketClosedTimeout2,"LastSocketClosedTimeout");

CWinsockAgent* CWinsockAgent::NewL()
{
    CWinsockAgent* self = new(ELeave)CWinsockAgent;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

CWinsockAgent::CWinsockAgent()
{
}

CWinsockAgent::~CWinsockAgent()
{
    if (iConnector)
    {
        iConnector->Cancel();
        delete iConnector;
    }
    if (iDisconnector)
    {
        iDisconnector->Cancel();
        delete iDisconnector;
    }
    
  
}

void CWinsockAgent::ConstructL()
{
    WsockCheckCommDbL(iIapId, iIapNetwork, iIapService, iIapBearer);
}

void CWinsockAgent::Info(TNifAgentInfo& aInfo) const
{
    aInfo.iVersion = TVersion(1,0,0);
    aInfo.iName = KWinsockAgent;
}

void CWinsockAgent::Connect(TAgentConnectType aType)
{
    Connect(aType, NULL);
}

void CWinsockAgent::Connect(TAgentConnectType DEBUG_ONLY(aType), 
                            CStoreableOverrideSettings* /*aOverride*/)
{
    TRACE1("Connect(%d)",aType);
    ASSERT(iNotify);
    ScheduleConnectNotification();
    iNotify->ServiceStarted();
}

void CWinsockAgent::CancelConnect()
{
    TRACE("CancelConnect()");
    CancelConnectNotification();
}

void CWinsockAgent::Reconnect()
{
    TRACE("Reconnect()");
    ScheduleConnectNotification();
}

void CWinsockAgent::CancelReconnect()
{
    TRACE("CancelReconnect()");
    CancelConnectNotification();
}

void CWinsockAgent::Authenticate(TDes& /*aUsername*/, TDes& /*aPassword*/)
{
    TRACE("Authenticate()");
    ASSERT(iNotify);
    iNotify->AuthenticateComplete(KErrNone);
}

void CWinsockAgent::CancelAuthenticate()
{
    TRACE("CancelAuthenticate()");
}

void CWinsockAgent::Disconnect(TInt aReason)
{
    TRACE1("Disconnect(%d)",aReason);
    iDisconnectReason = aReason;
    if (!iDisconnector)
    {
        iDisconnector = new CWinsockAgentCallback(this,DisconnectComplete);
    }
    if (iDisconnector)
    {
        // Schedule asynchronous disconnect notification
        ASSERT(iDisconnector->Agent() == this);
        iDisconnector->InvokeLater();
    }
}

TInt CWinsockAgent::GetExcessData(TDes8& aBuffer)
{
    TRACE("GetExcessData()");
    aBuffer.SetLength(0);
    return KErrNone;
}

ACCESS_METHOD2(TInt, ReadInt, const TDesC& aField, TUint32& aValue)
{
    if (aField == KIAPId)
    {
        aValue = iIapId;
    }
    else if (aField == KIAPNetwork)
    {
        aValue = iIapNetwork;
    }
    else if (aField == KIAPService)
    {
        aValue = iIapService;
    }
    else if (aField == KLANBearerLastSocketActivityTimeout ||
             aField == KLANBearerLastSocketActivityTimeout2)
    {
        aValue = KWinsockSocketActivityTimeout;
    }
    else if (aField == KLANBearerLastSessionClosedTimeout ||
             aField == KLANBearerLastSessionClosedTimeout2)
    {
        aValue = KWinsockSessionClosedTimeout;
    }
    else if (aField == KLANBearerLastSocketClosedTimeout ||
             aField == KLANBearerLastSocketClosedTimeout2)
    {
        aValue = KWinsockSocketClosedTimeout;
    }
    else
    {
        TRACE1("ReadInt(%S)  -> KErrNotFound",&aField);
        return KErrNotFound;
    }
    TRACE2("ReadInt(%S) = %d",&aField,aValue);
    return KErrNone;
}

ACCESS_METHOD2(TInt, WriteInt, const TDesC& DEBUG_ONLY(aField), 
               TUint32 DEBUG_ONLY(aValue))
{
    TRACE2("WriteInt(%S,%d)",&aField,aValue);
    return KErrNotSupported;
}

ACCESS_METHOD2(TInt, ReadDes, const TDesC& DEBUG_ONLY(aField), 
               TDes8& /*aValue*/)
{
    TRACE1("ReadDes8(%S)",&aField);
    return KErrNotFound;
}

ACCESS_METHOD2(TInt, WriteDes, const TDesC& DEBUG_ONLY(aField),
               const TDesC8& /*aValue*/)
{
    TRACE1("WriteDes8(%S)",&aField);
    return KErrNotSupported;
}

ACCESS_METHOD2(TInt, ReadDes, const TDesC& aField, TDes16& aValue)
{
    if (aField == KIfName)
    {
        aValue = KWinsockInterface;
    }
    else if (aField == KIfNetworks)
    {
        CWinsockProtocolFamily::ProtocolNames(aValue);
    }
    else if (aField == KIAPBearerType)
    {
        aValue = KWinsockBearerType;
    }
    else if (aField == KIAPServiceType)
    {
        aValue = KWinsockServiceType;
    }
    else if (aField == KIAPName)
    {
        aValue = KWinsockAPName;
    }
    else if (aField == KLANBearerName)
    {
        aValue = KWinsockBearerName;
    }
    else 
    {
        TRACE1("ReadDes16(%S) -> KErrNotFound",&aField);
        return KErrNotFound;
    }
    TRACE2("ReadDes16(%S) = %S",&aField,&aValue);
    return KErrNone;
}

ACCESS_METHOD2(TInt, WriteDes, const TDesC& DEBUG_ONLY(aField),
               const TDesC16& /*aValue*/)
{
    TRACE1("WriteDes16(%S)",&aField);
    return KErrNotSupported;
}

ACCESS_METHOD2(TInt, ReadBool, const TDesC& DEBUG_ONLY(aField),
               TBool& /*aValue*/)
{
    TRACE1("ReadBool(%S)",&aField);
    return KErrNotFound;
}

ACCESS_METHOD2(TInt, WriteBool, const TDesC& DEBUG_ONLY(aField),
               TBool DEBUG_ONLY(aValue))
{
    TRACE2("WriteBool(%S,%d)",&aField,aValue);
    return KErrNotSupported;
}

ACCESS_METHOD1(HBufC*, ReadLongDesLC, const TDesC& DEBUG_ONLY(aField))
{
    TRACE1("ReadLongDesLC(%S)",&aField);
    return NULL;
}

TInt CWinsockAgent::Notification(TNifToAgentEventType DEBUG_ONLY(aEvent),
                                 TAny* /*aInfo*/)
{
    TRACE1("Notification(%d)",aEvent);
    return KErrNotSupported;
}

TInt CWinsockAgent::IncomingConnectionReceived()
{
    TRACE("IncomingConnectionReceived()");
    return KErrNotSupported;
}

void CWinsockAgent::GetLastError(TInt& aError)
{
    TRACE("GetLastError()");
    aError = KErrNone;
}

TBool CWinsockAgent::IsActive() const
{
    return ETrue;
}

TBool CWinsockAgent::IsReconnect() const
{
    return EFalse;
}

void CWinsockAgent::SetConnectionSettingsL(const TConnectionSettings& aSettings)
{
    TRACE("SetConnectionSettingsL()");
    iSettings = aSettings;
}

TConnectionSettings& CWinsockAgent::ConnectionSettingsL()
{
    TRACE("ConnectionSettingsL()");
    return iSettings;
}

void CWinsockAgent::SetOverridesL(CStoreableOverrideSettings* /*aOverride*/)
{
    TRACE("SetOverridesL()");
}

CStoreableOverrideSettings* CWinsockAgent::OverridesL()
{
    TRACE("OverridesL()");
    User::Leave(KErrNotSupported);
    return NULL;
}

void CWinsockAgent::RequestNotificationOfServiceChangeL(
    MAgentSessionNotify* /*aSession*/)
{
    User::Leave(KErrNotSupported);
}

void CWinsockAgent::CancelRequestNotificationOfServiceChange(
    MAgentSessionNotify* /*aSession*/)
{
}

// Private methods

void CWinsockAgent::ScheduleConnectNotification()
{
    ASSERT(iNotify);
    if (!iConnector && iNotify)
    {
        iConnector = new CWinsockAgentCallback(this,ConnectComplete);
    }
    if (iConnector)
    {
        // Schedule asynchronous connect notification
        ASSERT(iConnector->Agent() == this);
        iConnector->InvokeLater();
    }
}

void CWinsockAgent::CancelConnectNotification()
{
    if (iConnector)
    {
        iConnector->Cancel();
    }
}

void CWinsockAgent::ConnectComplete()
{
    TRACE("firing connection notification");
    iNotify->AgentProgress(KConnectionOpen, KErrNone);
    iNotify->ConnectComplete(KErrNone);
}

void CWinsockAgent::DisconnectComplete()
{
    TRACE("firing disconnect notification");
    iNotify->AgentProgress(KConnectionClosed, iDisconnectReason);
    iNotify->DisconnectComplete();
}

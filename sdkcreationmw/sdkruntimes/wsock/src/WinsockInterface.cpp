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


#define TRACE_PREFIX "WSOCK: Interface: "
#include "wsock.h"
#include "WinsockInterface.h"
#include "WinsockProtocolFamily.h"
#include <es_enum_partner.h>

const TInt KWinsockSubConnectionId = 1;
const TConnectionType EWinsockConnectionType = EConnectionEthernet;
#define super CNifIfLink 

// CWinsockInterface::CallbackQueueEntry
class CWinsockInterface::CallbackQueueEntry
{
private:
    CWinsockInterface* iInterface;
    CallbackMethod iMethod;
    TInt iParam1;
    TInt iParam2;
public:
    TSglQueLink iLink;
    CallbackQueueEntry(CWinsockInterface* aInterface, CallbackMethod aMethod,
        TInt aParam1, TInt aParam2) : iInterface(aInterface), 
        iMethod(aMethod), iParam1(aParam1), iParam2(aParam2) {}
    void Invoke() { (iInterface->*iMethod)(iParam1, iParam2); }
};

// CWinsockInterface
CWinsockInterface::CWinsockInterface(CNifIfFactory* aFactory, 
                                     MNifIfNotify* aNotify) : 
super(*aFactory),
iSignature(KWinsockInterfaceSignature),
iCallbackQueue(_FOFF(CallbackQueueEntry,iLink)),
iCallback(TCallBack(AsyncCallbackProc,this),CActive::EPriorityStandard)
{
    iNotify = aNotify;
    iTimeStarted.HomeTime();
    TRACE1("created [%08X]",this);
}

CWinsockInterface::~CWinsockInterface()
{
    CancelOutstandingCallbacks();
    TRACE1("destroyed [%08X]",this);
}

TInt CWinsockInterface::Signature() const
{
    return KWinsockInterfaceSignature;
}

TInt CWinsockInterface::AsyncCallbackProc(TAny* aPtr)
{
    CWinsockInterface* self = (CWinsockInterface*)aPtr;
    self->HandleAsyncCallback();
    return KErrNone;
}

void CWinsockInterface::HandleAsyncCallback()
{
    // Invoke the first callback in the queue
    if (!iCallbackQueue.IsEmpty()) {
        CallbackQueueEntry* entry = iCallbackQueue.First();
        iCallbackQueue.Remove(*entry);
        entry->Invoke();
        delete entry;

        // Schedule the next one
        if (!iCallbackQueue.IsEmpty()) {
            iCallback.CallBack();
        }
    }
}

void CWinsockInterface::ScheduleAsyncCallback(CallbackMethod aMethod,
                                              TInt aP1, TInt aP2)
{
    CallbackQueueEntry* entry = new CallbackQueueEntry(this,aMethod,aP1,aP2);
    if (entry) {
        iCallbackQueue.AddLast(*entry);
        iCallback.Cancel();
        iCallback.CallBack();
    }
}

void CWinsockInterface::CancelOutstandingCallbacks()
{
    iCallback.Cancel();
    while (!iCallbackQueue.IsEmpty()) {
        CallbackQueueEntry* entry = iCallbackQueue.First();
        iCallbackQueue.Remove(*entry);
        delete entry;
    }
}

void CWinsockInterface::NotifyLinkLayerUp(TInt, TInt)
{
    ScheduleAsyncCallback(NotifySubConnectionOpened);
    TRACE("firing LinkLayerUp event");
    iNotify->LinkLayerUp();
}

void CWinsockInterface::NotifyLinkLayerOpen(TInt, TInt)
{
    TRACE("firing LinkLayerOpen event");
    iNotify->IfProgress(KLinkLayerOpen, KErrNone);
}

// Sends a subconnection opened event for the entire connection
void CWinsockInterface::NotifySubConnectionOpened(TInt, TInt)
{
    ScheduleAsyncCallback(NotifyLinkLayerOpen);

    TRACE("firing TSubConnectionOpenedEvent");
    TSubConnectionOpenedEvent event;
    event.iSubConnectionUniqueId = KNifEntireConnectionSubConnectionId;
    TPckg<TSubConnectionOpenedEvent> eventPtr(event);
    iNotify->NifEvent(ESubConnectionEvent, ESubConnectionOpened, eventPtr); 
}

// Sends a subconnection Closed event for the entire connection
void CWinsockInterface::NotifySubConnectionClosed(TInt aReason, TInt)
{
    ScheduleAsyncCallback(NotifyLinkLayerClosed, aReason);

    TRACE("firing TSubConnectionClosedEvent");
    TSubConnectionClosedEvent event;
    event.iSubConnectionUniqueId = KNifEntireConnectionSubConnectionId;
    event.iTotalUplinkDataVolume = iBytesReceived;
    event.iTotalDownlinkDataVolume = iBytesSent;
    event.iTimeClosed.UniversalTime();
    TPckg<TSubConnectionClosedEvent> eventPtr(event);
    iNotify->NifEvent(ESubConnectionEvent, ESubConnectionClosed, eventPtr);
}

void CWinsockInterface::NotifyLinkLayerDown(TInt aReason, TInt aAction)
{
    ScheduleAsyncCallback(NotifySubConnectionClosed, aReason);
    TRACE("firing LinkLayerDown event");
    iNotify->LinkLayerDown(aReason, (MNifIfNotify::TAction)aAction);
}

void CWinsockInterface::NotifyLinkLayerClosed(TInt aReason, TInt)
{
    TRACE("firing LinkLayerClosed event");
    iNotify->IfProgress(KLinkLayerClosed, aReason);
}

void CWinsockInterface::DataSent(TUint aBytes)
{
    // Handle rollover?
    iBytesSent += aBytes;
    if (iBytesSentGranularity)
    {
        TUint delta = iBytesSent - iBytesSentMark;
        if (delta >= iBytesSentGranularity)
        {
            int chunks = delta/iBytesSentGranularity;
            iBytesSentMark += chunks*iBytesSentGranularity;
            iNotify->NotifyDataSent(KWinsockSubConnectionId,
                delta*iBytesSentGranularity);
        }
    }
}

void CWinsockInterface::DataReceived(TUint aBytes)
{
    // Handle rollover?
    iBytesReceived += aBytes;
    if (iBytesReceivedGranularity)
    {
        TUint delta = iBytesReceived - iBytesReceivedMark;
        if (delta >= iBytesReceivedGranularity)
        {
            int chunks = delta/iBytesReceivedGranularity;
            iBytesReceivedMark += chunks*iBytesReceivedGranularity;
            iNotify->NotifyDataReceived(KWinsockSubConnectionId,
                delta*iBytesReceivedGranularity);
        }
    }
}

// CNifIfBase
void CWinsockInterface::Info(TNifIfInfo& aInfo) const
{
    aInfo.iName = KWinsockInterface;
    aInfo.iVersion = TVersion(1,0,0);
    aInfo.iFlags = KNifIfIsBase | KNifIfIsLink | KNifIfUsesNotify | KNifIfCreatedByFactory;
    aInfo.iProtocolSupported = 0;
    for (TInt i=0; i<CWinsockProtocolFamily::ProtocolCount(); i++)
    {
        const TServerProtocolDesc* p = CWinsockProtocolFamily::ProtocolDesc(i);
        aInfo.iProtocolSupported |= p->iProtocol;
    }
}

TInt CWinsockInterface::Send(RMBufChain& /*aPdu*/, TAny* /*aSource*/)
{
    TRACE("Send()");
    return KErrNotSupported;
}

TInt CWinsockInterface::Notification(TAgentToNifEventType DEBUG_ONLY(aEvent),
                                     TAny* /*aInfo*/)
{
    TRACE1("Notification(%d)",aEvent);
    return KErrNotSupported;
}

// CNifIfLink
TInt CWinsockInterface::Start()
{
    TRACE("Start()");
    ScheduleAsyncCallback(NotifyLinkLayerUp);
    return KErrNone;
}

void CWinsockInterface::Stop(TInt aReason, MNifIfNotify::TAction aAction)
{
    TRACE2("Stop(%d,%d)",aReason, aAction);
    CancelOutstandingCallbacks();
    ScheduleAsyncCallback(NotifyLinkLayerDown, aReason, aAction);
}

CNifIfBase* CWinsockInterface::GetBinderL(const TDesC& DEBUG_ONLY(aName))
{
    TRACE1("GetBinderL(%S)",&aName);
    return this;
}

TInt CWinsockInterface::Control(TUint aLevel,TUint aName,TDes8& aOption, 
                                TAny* aSource)
{
    TRACE3("Control(%d,%08X), %d bytes",aLevel,aName,aOption.Length());
	switch (aLevel)
    {
  /*  case KCOLInterface:
        switch (aName)
        {
        case KCOGetNifEMIPtr:
        default:
            {
                MNifIfExtendedManagementInterface** that = 
                    (MNifIfExtendedManagementInterface**)aOption.Ptr();
                if (aOption.Length() == sizeof(*that))
                {
                    MNifIfExtendedManagementInterface* This = this;
                    TRACE1("MNifIfExtendedManagementInterface = %08X",This);
                    *that = This;
                    return KErrNone;
                }
                else
                {
                    return KErrArgument;
                }
            }
            break;
        }
        break;
*/
    case KCOLAgent:
        switch (aName)
        {
        case KCOGetAgentEMIPtr:
            {
                MNifAgentExtendedManagementInterface** that = 
                    (MNifAgentExtendedManagementInterface**)aOption.Ptr();
                if (aOption.Length() == sizeof(*that))
                {
                    MNifAgentExtendedManagementInterface* This = this;
                    TRACE1("MNifAgentExtendedManagementInterface = %08X",This);
                    *that = This;
                    return KErrNone;
                }
                else
                {
                    return KErrArgument;
                }
            }
        default:
            break;
        }
        break;

    default:
        break;
    }
    return super::Control(aLevel, aName, aOption, aSource);
}

// MNifIfExtendedManagementInterface
TInt CWinsockInterface::Stop(TSubConnectionUniqueId aId, 
                             TInt DEBUG_ONLY(aReason))
{
    TRACE2("Stop(%d,%d)", aId, aReason);
    switch (aId)
    {
    case KNifEntireConnectionSubConnectionId:
    case KWinsockSubConnectionId:
        return KErrNone;
    default:
        return KErrNotFound;
    }
}

TInt CWinsockInterface::GetDataTransferred(TSubConnectionUniqueId aId, 
                                           TUint& aSentBytes, 
                                           TUint& aReceivedBytes)
{
    TRACE1("GetDataTransferred(%d)", aId);
    switch (aId)
    {
    case KNifEntireConnectionSubConnectionId:
    case KWinsockSubConnectionId:
        aSentBytes = iBytesSent;
        aReceivedBytes = iBytesReceived;
        return KErrNone;
    default:
        return KErrNotFound;
    }
}

TInt CWinsockInterface::SetDataSentNotificationGranularity(TSubConnectionUniqueId aId, 
                                                           TUint aGranularity)
{
    TRACE2("SetDataSentNotificationGranularity(%d,%d)", aId, aGranularity);
    switch (aId)
    {
    case KNifEntireConnectionSubConnectionId:
    case KWinsockSubConnectionId:
        iBytesSentGranularity = aGranularity;
        return KErrNone;
    default:
        return KErrNotFound;
    }
}

TInt CWinsockInterface::CancelDataSentNotification(TSubConnectionUniqueId aId)
{
    TRACE1("CancelDataSentNotification(%d)", aId);
    switch (aId)
    {
    case KNifEntireConnectionSubConnectionId:
    case KWinsockSubConnectionId:
        iBytesSentGranularity = 0;
        return KErrNone;
    default:
        return KErrNotFound;
    }
}

TInt CWinsockInterface::SetDataReceivedNotificationGranularity(TSubConnectionUniqueId aId, 
                                                               TUint aGranularity)
{
    TRACE2("SetDataReceivedNotificationGranularity(%d,%d)", aId, aGranularity);
    switch (aId)
    {
    case KNifEntireConnectionSubConnectionId:
    case KWinsockSubConnectionId:
        iBytesReceivedGranularity = aGranularity;
        return KErrNone;
    default:
        return KErrNotFound;
    }
}

TInt CWinsockInterface::CancelDataReceivedNotification(TSubConnectionUniqueId aId)
{
    TRACE1("CancelDataReceivedNotification(%d)", aId);
    switch (aId)
    {
    case KNifEntireConnectionSubConnectionId:
    case KWinsockSubConnectionId:
        iBytesReceivedGranularity = 0;
        return KErrNone;
    default:
        return KErrNotFound;
    }
}

// MNifAgentExtendedManagementInterface
TInt CWinsockInterface::GetInterfaceType(TConnectionType& aConnectionType)
{
    aConnectionType = EWinsockConnectionType;
    return KErrNone;
}

TInt CWinsockInterface::EnumerateSubConnections(TUint& aCount)
{
    aCount = 1;
    return KErrNone;
}

TInt CWinsockInterface::GetSubConnectionInfo(TUint aIndex, 
                                             TDes8& aInfo)
{
    if (aInfo.Length() != sizeof(TSubConnectionInfo))
    {
        return KErrArgument;
    }

    TSubConnectionInfo* info = (TSubConnectionInfo*)aInfo.Ptr();
    switch (aIndex)
    {
    case KNifEntireConnectionSubConnectionId:
    case KWinsockSubConnectionId:
        info->iConnectionType = EWinsockConnectionType;
        info->iSubConnectionUniqueId = aIndex;
        info->iTimeStarted = iTimeStarted;
        return KErrNone;
    default:
        return KErrNotFound;
    }
}

TInt CWinsockInterface::GetSubConnectionInfo(TDes8& aInfo)
{
    if (aInfo.Length() != sizeof(TSubConnectionInfo))
    {
        return KErrArgument;
    }

    TSubConnectionInfo* info = (TSubConnectionInfo*)aInfo.Ptr();
    return GetSubConnectionInfo(info->iSubConnectionUniqueId, aInfo);
}

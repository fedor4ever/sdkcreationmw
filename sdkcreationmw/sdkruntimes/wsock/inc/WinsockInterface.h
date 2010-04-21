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


#ifndef _WSOCK_INTERFACE_H_
#define _WSOCK_INTERFACE_H_

#include <nifif.h>
#include <nifagt.h>
#include "WinsockUid.h"

const TInt KWinsockInterfaceSignature = KWinsockInterfaceUid;

class CWinsockInterface : public CNifIfLink, 
                         // public MNifIfExtendedManagementInterface,
                          public MNifAgentExtendedManagementInterface
{
private:
    class CallbackQueueEntry;
    typedef void (CWinsockInterface::*CallbackMethod)(TInt aArg1, TInt aArg2);

private:
    TInt iSignature;
    TUint iBytesSent;
    TUint iBytesReceived;
    TUint iBytesSentMark;
    TUint iBytesReceivedMark;
    TUint iBytesSentGranularity;
    TUint iBytesReceivedGranularity;
    TTime iTimeStarted;
    TSglQue<CallbackQueueEntry> iCallbackQueue;
    CAsyncCallBack iCallback;

public:
    CWinsockInterface(CNifIfFactory* aFactory, MNifIfNotify* aNotify);
    ~CWinsockInterface();

    TInt Signature() const;
    void DataSent(TUint aBytes);
    void DataReceived(TUint aBytes);

    // CNifIfLink
    virtual TInt Start();
    virtual void Stop(TInt aReason, MNifIfNotify::TAction aAction);
    virtual CNifIfBase* GetBinderL(const TDesC& aName);

    // CNifIfBase
    virtual void Info(TNifIfInfo& aInfo) const;
    virtual TInt Send(RMBufChain& aPdu, TAny* aSource);
    virtual TInt Notification(TAgentToNifEventType aEvent, void * aInfo);
    virtual TInt Control(TUint aLevel,TUint aName,TDes8& aOption,TAny* aSrc);

    // MNifIfExtendedManagementInterface
    virtual TInt Stop(TSubConnectionUniqueId aSubConnectionId, TInt aReason);
    virtual TInt GetDataTransferred(TSubConnectionUniqueId aSubConnectionId, TUint& aSentBytes, TUint& aReceivedBytes);
    virtual TInt SetDataSentNotificationGranularity(TSubConnectionUniqueId aSubConnectionId, TUint aGranularity);
    virtual TInt CancelDataSentNotification(TSubConnectionUniqueId aSubConnectionId);
    virtual TInt SetDataReceivedNotificationGranularity(TSubConnectionUniqueId aSubConnectionId, TUint aGranularity);
    virtual TInt CancelDataReceivedNotification(TSubConnectionUniqueId aSubConnectionId);

    // MNifAgentExtendedManagementInterface
    virtual TInt GetInterfaceType(TConnectionType& aConnectionType);
    virtual TInt EnumerateSubConnections(TUint& aCount);
    virtual TInt GetSubConnectionInfo(TUint aIndex, TDes8& aSubConnectionInfo);
    virtual TInt GetSubConnectionInfo(TDes8& aSubConnectionInfo);

private:
    // Operations on asynchronous event queue
    static TInt AsyncCallbackProc(TAny* aPtr);
    void HandleAsyncCallback();
    void ScheduleAsyncCallback(CallbackMethod aMethod, TInt aP1=0, TInt aP2=0);
    void CancelOutstandingCallbacks();

    // Asynchronous callbacks that fire various interface-related events
    void NotifyLinkLayerUp(TInt aParam1, TInt aParam2);
    void NotifyLinkLayerOpen(TInt aParam1, TInt aParam2);
    void NotifySubConnectionOpened(TInt aParam1, TInt aParam2);
    void NotifySubConnectionClosed(TInt aParam1, TInt aParam2);
    void NotifyLinkLayerDown(TInt aParam1, TInt aParam2);
    void NotifyLinkLayerClosed(TInt aParam1, TInt aParam2);
};

#endif // _WSOCK_INTERFACE_H_

/*
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

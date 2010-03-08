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


#include <in_sock.h>
#include <ip4_hdr.h>
#include "JavaDebugAgentKeepAlive.h"

// CJavaDebugAgentKeepAlive::CReceiver
class CJavaDebugAgentKeepAlive::CReceiver : public CActive
{
private:
    RSocket* iSocket;
    TSockXfrLength iLength;
    TBuf8<1> iBuf;
public:
    CReceiver(RSocket* aSocket);
    void Recv();
    virtual void RunL();
    virtual void DoCancel();
};

CJavaDebugAgentKeepAlive::CReceiver::CReceiver(RSocket* aSocket) :
CActive(EPriorityStandard),
iSocket(aSocket)
{ 
    CActiveScheduler::Add(this);
}

void CJavaDebugAgentKeepAlive::CReceiver::RunL()
{
    if (iStatus.Int() == KErrNone) Recv();
}

void CJavaDebugAgentKeepAlive::CReceiver::DoCancel()
{
    iSocket->CancelRead();
}

void CJavaDebugAgentKeepAlive::CReceiver::Recv()
{
    SetActive();
    iStatus = KRequestPending;
    iSocket->RecvOneOrMore(iBuf, 0, iStatus, iLength);
}

// CJavaDebugAgentKeepAlive
CJavaDebugAgentKeepAlive* 
CJavaDebugAgentKeepAlive::NewL(RSocketServ* aSocketServ, 
                               const TSockAddr* aAddr,
                               MJavaDebugAgentLog* aLog,
                               TInt aPeriod)
{
    CJavaDebugAgentKeepAlive* self = NewLC(aSocketServ, aAddr, aLog, aPeriod);
    CleanupStack::Pop(self);
    return self;
}

CJavaDebugAgentKeepAlive* 
CJavaDebugAgentKeepAlive::NewLC(RSocketServ* aSocketServ, 
                                const TSockAddr* aAddr,
                                MJavaDebugAgentLog* aLog,
                               TInt aPeriod)
{
    CJavaDebugAgentKeepAlive* self = new(ELeave)
        CJavaDebugAgentKeepAlive(aSocketServ, aAddr, aLog, aPeriod);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

CJavaDebugAgentKeepAlive::~CJavaDebugAgentKeepAlive()
{
    Cancel();
    delete iReceiver;
    iSocket.Close();
    iTimer.Close();
}

CJavaDebugAgentKeepAlive::
CJavaDebugAgentKeepAlive(RSocketServ* aSocketServ,
                         const TSockAddr* aAddr,
                         MJavaDebugAgentLog* aLog,
                         TInt aPeriodInMilliseconds) :
CActive(EPriorityStandard),
iPeriod(aPeriodInMilliseconds*1000),
iSocketServ(aSocketServ),
iAddress(*aAddr),
iLog(aLog)
{
    CActiveScheduler::Add(this);
    iMsg.FillZ(iMsg.MaxLength());
    iMsg[0] = KInet4ICMP_Echo;
    iMsg[8] = 'P';
    iMsg[9] = 'I';
    iMsg[10] = 'N';
    iMsg[11] = 'G';
}

void CJavaDebugAgentKeepAlive::ConstructL()
{
    iReceiver = new(ELeave)CReceiver(&iSocket);
    User::LeaveIfError(iTimer.CreateLocal());
    User::LeaveIfError(iSocket.Open(*iSocketServ, KAfInet, 
        KSockDatagram, KProtocolInetIcmp));

    SetActive();
    iStatus = KRequestPending;
    iState = EConnecting;
    iSocket.Connect(iAddress, iStatus);
}

// CActive
void CJavaDebugAgentKeepAlive::RunL()
{
    TInt err = iStatus.Int();
    switch (iState)
    {
    case EConnecting:
        if (err == KErrNone)
        {
            iReceiver->Recv();
            Ping();
        }
        else
        {
            iState = EFailed;
            iLog->LogFormat(_S("KeepAlive connect error %d"),err);
        }
        break;

    case ESending:
        if (err == KErrNone)
        {
            Wait();
        }
        else
        {
            iState = EFailed;
            iLog->LogFormat(_S("KeepAlive send error %d"),err);
        }
        break;

    case EWaiting:
        if (err == KErrNone)
        {
            Ping();
        }
        else
        {
            iState = EFailed;
            iLog->LogFormat(_S("KeepAlive wait error %d"),err);
        }
        break;
    }
}

void CJavaDebugAgentKeepAlive::Wait()
{
    SetActive();
    iStatus = KRequestPending;
    iState = EWaiting;
    iTimer.After(iStatus,iPeriod);
}

void CJavaDebugAgentKeepAlive::Ping()
{
    TUint16* ptr16 = ((TUint16*)&iMsg[0]);
    ptr16[1] = 0;  // checksum
    iSeq++;
    iMsg[6] = (TUint8)(iSeq >> 8); // convert to network byte order
    iMsg[7] = (TUint8)iSeq;

    // Checksum is calculated in native byte order, and stored natively too
    TUint16 sum = 0;
    for (TInt i=iMsg.Length()/2-1; i>=0; i--) sum += ptr16[i];
    ptr16[1] = ~sum;

    // Send the message
    SetActive();
    iStatus = KRequestPending;
    iState = ESending;
    iSocket.SendTo(iMsg, iAddress, 0, iStatus);
}

TInt CJavaDebugAgentKeepAlive::RunError(TInt /*aError*/)
{
    iState = EFailed;
    return KErrNone;
}

void CJavaDebugAgentKeepAlive::DoCancel()
{
    TRequestStatus *status;
    switch (iState)
    {
    case EConnecting:
        iSocket.CancelConnect();
        break;
    case ESending:
        iSocket.CancelSend();
        break;
    case EWaiting:
        iTimer.Cancel();
        break;
    default:
		status = (&iStatus);
		User::RequestComplete(status, KErrCancel);
        break;
    }
}

/**
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

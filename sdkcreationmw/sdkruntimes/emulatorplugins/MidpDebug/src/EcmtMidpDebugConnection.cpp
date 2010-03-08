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
* Description:  MIDP debug connection context
 *
*/


#include <e32base.h>

#define TRACE_PREFIX "MidpDebugConnection: "

#include "EcmtMessageEvent.h"
#include "EcmtMidpDebugSession.h"
#include "EcmtMidpDebugConnection.h"
#include "EcmtMidpDebugPluginUids.h"
#include "EcmtMidpDebugProtocol.h"
#include "EcmtMidpDebugTrace.h"
#include "EcmtMessagingIf.h"

_LIT(KLocalHost, "127.0.0.1");

// ---------------------------------------------------------------------------
// CEcmtMidpDebugConnection::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
CEcmtMidpDebugConnection*
CEcmtMidpDebugConnection::NewLC(CEcmtMidpDebugSession* aSession,
                                TInt aConnId, TInt aPort, TInt aRetryDelay)
{
    CEcmtMidpDebugConnection* self;
    self = new(ELeave)CEcmtMidpDebugConnection(aSession, aConnId, aRetryDelay);
    CleanupStack::PushL(self);
    self->ConstructL(aPort);
    return self;
}

// NOTE: classes derived from CBase are zero-initialized
CEcmtMidpDebugConnection::
CEcmtMidpDebugConnection(CEcmtMidpDebugSession* aSession, 
                         TInt aConnId,
                         TInt aConnectRetryDelay):
    CActive(EPriorityStandard),
    iConnId(aConnId),
    iSession(aSession),
    iConnectRetryDelay(aConnectRetryDelay),
    iReadPtr(0, 0)
{
    // This one doesn't leave, we can do it in the constructor
    CActiveScheduler::Add(this);
    TRACE2("[%08X] (%u) created",this,iConnId);
}

// ---------------------------------------------------------------------------
// CEcmtMidpDebugConnection::ConstructL
// ---------------------------------------------------------------------------
void CEcmtMidpDebugConnection::ConstructL(TInt aPort)
{
    iReadBuffer = (TUint8*)User::AllocL(KMaxSocketReadBuffer);
    iReadPtr.Set(iReadBuffer, 0, KMaxSocketReadBuffer);
    LEAVE_IF_ERROR(iTimer.CreateLocal());
    LEAVE_IF_ERROR(iSeiSocket.Open(*iSession->SocketServ(),
                                   KAfInet, KSockStream,
                                   KProtocolInetTcp));

    LEAVE_IF_ERROR(iSeiAddr.Input(KLocalHost));
    iSeiAddr.SetPort(aPort);
    iSeiAddr.SetFamily(KAfInet);
    iSeiSocket.Connect(iSeiAddr, iStatus);
    iConnState = STATE_CONNECTING;
    TRACE3("[%08X] (%u) connecting to port %d",this,iConnId,aPort);
    SetActive();
}

CEcmtMidpDebugConnection::~CEcmtMidpDebugConnection()
{
    // Cancel pending read, connect or whatever
    Cancel();

    iTimer.Close();
    iSeiSocket.Close();
    delete iReadBuffer;
    delete iWriteBuffer;

    // Send RESET event to the gateway. It's OK if we send it more than once.
    MEcmtMessaging* messaging = iSession->Messaging();
    if (messaging)
    {
        CEcmtMessageEvent* event = messaging->NewMessageEvent(
            TUid::Uid(KECMT_MIDP_DEBUG_GATEWAY_UID),
            ECMT_MIDP_DEBUG_RESET_SIZE);
        if (event)
        {
            TPtr8 ptr(messaging->Message(*event));
            ptr.FillZ(ECMT_MIDP_DEBUG_RESET_SIZE);
            iSession->FillHeader(ptr, ECMT_MIDP_DEBUG_OPCODE_RESET);
            PutConnId(ptr, ECMT_MIDP_DEBUG_RESET_CID_OFFSET);
            messaging->GetMessageSender()->SendMessage(event);
        }
    }

    TRACE2("[%08X] (%u) destroyed",this,iConnId);
}

// Handles data received from the gateway. Returns True if sequence number
// matched, False if it didn't match or any other error has occured. If this
// method returns False, the session will be deallocated by the caller.
TBool CEcmtMidpDebugConnection::Send(TUint aSeq, const TDesC8& aData)
{
    if (aSeq == iGatewaySeq++)
    {
        TInt len = aData.Length();
        if (iConnState == STATE_READING)
        {
            // Note that while we are writing data to the socket, we are not
            // receving anything. Perhaps, we should write asynchronously...
            TRACE3("[%08X] (%u) sending %u bytes",this,iConnId,len);
            TRequestStatus status(KRequestPending);
            iSeiSocket.Write(aData, status);
            User::WaitForRequest(status);
        }
        else if (len > 0)
        {
            // Store the data in the buffer, we will send it when we
            // finally connect to the debug agent.
            if (iWriteBuffer == NULL)
            {
                // First write, allocate the buffer
                TRACE3("[%08X] (%u) queuing %u bytes",this,iConnId,len);
                iWriteBuffer = aData.Alloc();
                if (!iWriteBuffer)
                {
                    TRACE2("[%08X] (%u) out of memory!",this,iConnId);
                    return EFalse;
                }
            }
            else
            {
                // Subsequent writes are appended to the data collected
                // from the previous writes
                TRACE3("[%08X] (%u) queuing %u bytes more",this,iConnId,len);
                TInt newLength = iWriteBuffer->Length() + len;
                HBufC8* newBuffer = iWriteBuffer->ReAlloc(newLength);
                if (!newBuffer)
                {
                    TRACE2("[%08X] (%u) out of memory!",this,iConnId);
                    return EFalse;
                }
                newBuffer->Des().Append(aData);
                iWriteBuffer = newBuffer;
            }
        }
        return ETrue;
    }
    else
    {
        // Sequence number mismatch
        TRACE4("[%08X] (%u) sequence number mismatch %d/%d",this,iConnId,
               aSeq,iGatewaySeq-1);
        Close();
        return EFalse;
    }
}

// Puts connection id into the packet at given offset
void CEcmtMidpDebugConnection::PutConnId(TDes8& aPacket, TInt aOffset) const
{
    TUint cid = iConnId;
    aPacket[aOffset+3] = (TUint8)cid; cid >>= 8;
    aPacket[aOffset+2] = (TUint8)cid; cid >>= 8;
    aPacket[aOffset+1] = (TUint8)cid; cid >>= 8;
    aPacket[aOffset+0] = (TUint8)cid;
}

// Handles data received from the debug agent. Returns False if memory 
// allocation fails
TBool CEcmtMidpDebugConnection::Receive(const TDesC8& aData)
{
    TRACE3("[%08X] (%u) received %u bytes",this,iConnId,aData.Length());
    MEcmtMessaging* messaging = iSession->Messaging();
    if (messaging)
    {
        CEcmtMessageEvent* event = messaging->NewMessageEvent(
            TUid::Uid(KECMT_MIDP_DEBUG_GATEWAY_UID),
            aData.Length() + ECMT_MIDP_DEBUG_SEND_DATA_OFFSET);
        if (event)
        {
            TPtr8 ptr(messaging->Message(*event));
            ptr.FillZ(ECMT_MIDP_DEBUG_SEND_DATA_OFFSET);
            iSession->FillHeader(ptr, ECMT_MIDP_DEBUG_OPCODE_SEND);
            PutConnId(ptr, ECMT_MIDP_DEBUG_SEND_CID_OFFSET);
            TUint seq = iDebugAgentSeq++;
            ptr[ECMT_MIDP_DEBUG_SEND_SEQ_OFFSET+3] = (TUint8)seq; seq >>= 8;
            ptr[ECMT_MIDP_DEBUG_SEND_SEQ_OFFSET+2] = (TUint8)seq; seq >>= 8;
            ptr[ECMT_MIDP_DEBUG_SEND_SEQ_OFFSET+1] = (TUint8)seq; seq >>= 8;
            ptr[ECMT_MIDP_DEBUG_SEND_SEQ_OFFSET+0] = (TUint8)seq;
            ptr.Append(aData);
            messaging->GetMessageSender()->SendMessage(event);
            return ETrue;
        }
    }
    return EFalse;
}

// ---------------------------------------------------------------------------
// CEcmtMidpDebugConnection::RunL
// ---------------------------------------------------------------------------
void CEcmtMidpDebugConnection::RunL()
{
    switch(iConnState)
    {
    case STATE_SLEEPING:
        if (iStatus == KErrNone)
        {
            TInt err = iSeiSocket.Open(*iSession->SocketServ(),
                                       KAfInet,
                                       KSockStream,
                                       KProtocolInetTcp);
            if (err == KErrNone)
            {
                iConnState = STATE_CONNECTING;
                iSeiSocket.Connect(iSeiAddr, iStatus);
                SetActive();
                return;
            }
            
        }
        Close();
        break;

    case STATE_CONNECTING:
        if (iStatus == KErrCouldNotConnect)
        {
            // Symbian sockets must be re-opened after each unsuccessful
            // connection attempt.
            if (iConnectAttempt++ < MAX_CONNECT_ATTEMPTS)
            {
                iSeiSocket.Close();
                iConnState = STATE_SLEEPING;
                iTimer.After(iStatus, iConnectRetryDelay);
                SetActive();
                return;
            }
        }
        else if (iStatus == KErrNone)
        {
            TRACE2("[%08X] (%u) connected",this,iConnId);
            if (iWriteBuffer != NULL && iWriteBuffer->Length()>0)
            {
                TRequestStatus status(KRequestPending);
                iSeiSocket.Write(iWriteBuffer->Des(), status);
                User::WaitForRequest(status);
                delete iWriteBuffer;
                iWriteBuffer = NULL;
            }
            IssueRead();
            return;
        }
        Close();
        break;				

    case STATE_READING:
        if (iStatus == KErrNone)
        {
            // Forward received data to GW:
            if (Receive(iReadPtr))
            {
                IssueRead();
                return;
            }
        }
        Close();
        break;
    }
}	
	
// ---------------------------------------------------------------------------
// CEcmtMidpDebugConnection::DoCancel
// ---------------------------------------------------------------------------
void CEcmtMidpDebugConnection::DoCancel()
{
   switch (iConnState)
   {
   case STATE_SLEEPING:
       iTimer.Cancel();
       break;
   case STATE_CONNECTING:
       iSeiSocket.CancelConnect();
       break;
   case STATE_READING:
       iSeiSocket.CancelRead();
       break;
   default:
       break;
   }
}

void CEcmtMidpDebugConnection::IssueRead()
{
    iConnState = STATE_READING;
    iReadPtr.SetLength(0);
    iReadXfrLength = 0;
    iSeiSocket.RecvOneOrMore(iReadPtr, 0, iStatus, iReadXfrLength);
    SetActive();
}

void CEcmtMidpDebugConnection::Close()
{
    if (!iClosed)
    {
        iClosed = ETrue;
        TRACE2("[%08X] (%u) closed",this,iConnId);
        iSession->ConnectionClosed(this);
    }
}

/**
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

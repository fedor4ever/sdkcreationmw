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

#include <e32base.h>

#define TRACE_PREFIX "MidpDebugSession: "

#include "EcmtMessageEvent.h"
#include "EcmtMidpDebugSession.h"
#include "EcmtMidpDebugPluginUids.h"
#include "EcmtMidpDebugProtocol.h"
#include "EcmtMidpDebugConnection.h"
#include "EcmtMidpDebugSessionObserver.h"
#include "EcmtMidpDebugTrace.h"
#include "EcmtMessagingIf.h"

//===========================================================================
// CEcmtMidpDebugPlugin
//===========================================================================

// ---------------------------------------------------------------------------
// CEcmtMidpDebugSession::NewL
// Two-phased constructor.
// ---------------------------------------------------------------------------
CEcmtMidpDebugSession*
CEcmtMidpDebugSession::NewL(TInt aSessionId,
                            MEcmtMidpDebugSessionObserver* aListener,
                            MEcmtMessaging* aMessaging)
{
    CEcmtMidpDebugSession* self = new(ELeave)CEcmtMidpDebugSession(aSessionId,
                                                                   aListener, 
                                                                   aMessaging);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

// ---------------------------------------------------------------------------
// CEcmtMidpDebugSession::ConstructL
// ---------------------------------------------------------------------------
void CEcmtMidpDebugSession::ConstructL()
{
    LEAVE_IF_ERROR(iSocketServ.Connect());
    StartDebugAgentL(FindFreePortL());
}

// NOTE: classes derived from CBase are zero-initialized
CEcmtMidpDebugSession::
CEcmtMidpDebugSession(TInt aSessionId, 
                      MEcmtMidpDebugSessionObserver* aListener,
                      MEcmtMessaging* aMessaging) :
    CActive(EPriorityStandard),
    iSessionId(aSessionId),
    iListener(aListener),
    iMessaging(aMessaging)
{
    iStatus = KRequestPending;
    CActiveScheduler::Add(this);
    SetActive();
    TRACE2("[%u] created (%08X)",iSessionId,this);
}

CEcmtMidpDebugSession::~CEcmtMidpDebugSession()
{
    // Cancel pending read, connect or whatever
    Cancel();

    iConnections.ResetAndDestroy();
    iDeadConnections.Reset();

    KillDebugAgent();

    // Send CLOSE event to the gateway. It's OK if we send it twice.
    if (iMessaging)
    {
        CEcmtMessageEvent* event = iMessaging->NewMessageEvent(
            TUid::Uid(KECMT_MIDP_DEBUG_GATEWAY_UID),
            ECMT_MIDP_DEBUG_CLOSE_SIZE);
        if (event)
        {
            TPtr8 ptr(iMessaging->Message(*event));
            ptr.FillZ(ECMT_MIDP_DEBUG_CLOSE_SIZE);
            FillHeader(ptr, ECMT_MIDP_DEBUG_OPCODE_CLOSE);
            iMessaging->GetMessageSender()->SendMessage(event);
        }
    }

    TRACE2("[%u] destroyed (%08X)",iSessionId,this);
}

// ---------------------------------------------------------------------------
// Protocol message handlers
// ---------------------------------------------------------------------------
void CEcmtMidpDebugSession::ConnectL(TUint aConnId, TUint aPort)
{
    TInt index;
    CEcmtMidpDebugConnection* conn = FindConnection(aConnId, index);
    if (conn)
    {
        TRACE3("[%u] CONNECT: destroying connection %08X (%u)",
            iSessionId, conn, aConnId);
        iConnections.Remove(index);
        delete conn;
    }

    conn = CEcmtMidpDebugConnection::NewLC(this, aConnId, aPort, 500000);
    LEAVE_IF_ERROR(iConnections.Append(conn));
    CleanupStack::Pop(conn);
    TRACE3("[%u] CONNECT: created new connection %08X (%u)",
        iSessionId, conn, aConnId);
}

void CEcmtMidpDebugSession::Send(TUint aConnId, TUint aSeq, 
                                  const TDesC8& aData)
{
    TInt index;
    CEcmtMidpDebugConnection* conn = FindConnection(aConnId, index);
    if (conn)
    {
        conn->Send(aSeq, aData);
    }
    else
    {
        TRACE2("[%u] SEND: invalid conn id %u",iSessionId, aConnId);
    }
}

void CEcmtMidpDebugSession::Reset(TUint aConnId)
{
    TInt index;
    CEcmtMidpDebugConnection* conn = FindConnection(aConnId, index);
    if (conn)
    {
        TRACE3("[%u] RESET: destroying connection %08X (%u)",
            iSessionId, conn, aConnId);
        iConnections.Remove(index);
        delete conn;
    }
    else
    {
        TRACE2("[%u] RESET: invalid conn id %u",iSessionId, aConnId);
    }
}

// Fill header for the next ECMT message from debug agent to the gateway.
// Increments the packet sequence number (iDebugAgentSeq)
void CEcmtMidpDebugSession::FillHeader(TDes8& aHeader, TUint8 aOpCode)
{
    TUint sid = iSessionId;
    aHeader[ECMT_MIDP_DEBUG_HEADER_SID_OFFSET+3] = (TUint8)sid; sid >>= 8;
    aHeader[ECMT_MIDP_DEBUG_HEADER_SID_OFFSET+2] = (TUint8)sid; sid >>= 8;
    aHeader[ECMT_MIDP_DEBUG_HEADER_SID_OFFSET+1] = (TUint8)sid; sid >>= 8;
    aHeader[ECMT_MIDP_DEBUG_HEADER_SID_OFFSET+0] = (TUint8)sid;
    aHeader[ECMT_MIDP_DEBUG_HEADER_OPCODE_OFFSET] = aOpCode;
}

// Starts debug agent with the specified port as a parameter
void CEcmtMidpDebugSession::StartDebugAgentL(TInt aPort)
{
    TBuf<20> arg;
    arg.Num(aPort);

    // Start the debug agent process
    _LIT(KDebugAgentExe, "\\sys\\bin\\DebugAgent.exe");
    const TDesC& exe = KDebugAgentExe;
    TRACE2("[%u] starting %S",iSessionId, &exe);
    LEAVE_IF_ERROR(iDebugAgentProcess.Create(exe, arg));
    iFlags = FLAG_AGENT_STARTED;
    iDebugAgentProcess.Resume();

    // Create control connection
    CEcmtMidpDebugConnection* conn;
    conn = CEcmtMidpDebugConnection::NewLC(this, 0, aPort, 250000);
    LEAVE_IF_ERROR(iConnections.Append(conn));
    CleanupStack::Pop(conn);
    TRACE2("[%u] created control connection %08X",iSessionId, conn);
}

// ---------------------------------------------------------------------------
// CEcmtMidpDebugSession::KillDebugAgent
// ---------------------------------------------------------------------------
void CEcmtMidpDebugSession::KillDebugAgent()
{
    if (iFlags & FLAG_AGENT_STARTED)
    {
        TRACE1("[%u] killing debug agent",iSessionId);
        iFlags &= ~FLAG_AGENT_STARTED;
        iDebugAgentProcess.Kill(0);
        iDebugAgentProcess.Close();
    }
}
	
// ---------------------------------------------------------------------------
// CEcmtMidpDebugSession::FindFreePortL
// ---------------------------------------------------------------------------
TInt CEcmtMidpDebugSession::FindFreePortL()
{
    TInt port = 0;
    RSocket sock;
    TInt err = sock.Open(iSocketServ, KAfInet, KSockStream, KProtocolInetTcp);
    if (err == KErrNone)
    {
        // Ask TCP/IP stack to allocate a port number for us
        TInetAddr addr(0,0);
        err = sock.Bind(addr);
        if (err == KErrNone)
        {
            // Read the port number and close the socket
            sock.LocalName(addr);
            port = addr.Port();
            TRACE2("[%u] port %d", iSessionId, port);
        }
        sock.Close();
    }
    LEAVE_IF_ERROR(err);
    return port;
}
	
// ---------------------------------------------------------------------------
// Finds connection by connection id	
// ---------------------------------------------------------------------------
CEcmtMidpDebugConnection* 
CEcmtMidpDebugSession::FindConnection(TUint aConnId, 
                                      TInt& aIndex) const
{
    for (TInt i=0; i<iConnections.Count(); i++)
    {
        if (iConnections[i]->ConnId() == aConnId)
        {
            aIndex = i;
            return iConnections[i];
        }
    }
    aIndex = -1;
    return NULL;
}

// ---------------------------------------------------------------------------
// CEcmtMidpDebugSession::RunL
// ---------------------------------------------------------------------------
void CEcmtMidpDebugSession::RunL()
{
    iStatus = KRequestPending;
    FlushDeadConnections();
    SetActive();
}	

// ---------------------------------------------------------------------------
// CEcmtMidpDebugSession::DoCancel
// ---------------------------------------------------------------------------
void CEcmtMidpDebugSession::DoCancel()
{
    TRequestStatus *status = (&iStatus);
    User::RequestComplete(status, KErrCancel);
}

void CEcmtMidpDebugSession::CloseSession()
{
    if (!(iFlags & FLAG_SESSION_CLOSED))
    {
        iFlags |= FLAG_SESSION_CLOSED;
        iListener->SessionClosed(this);
    }
}

void CEcmtMidpDebugSession::ConnectionClosed(CEcmtMidpDebugConnection* aConn)
{
    if (aConn->ConnId() == 0)
    {
        // Control connection has been closed, terminate the session
        CloseSession();
    }
    else
    {
        if (iDeadConnections.Find(aConn) < 0 &&
            iConnections.Find(aConn) >= 0)
        {
            iDeadConnections.Append(aConn);
            if (iStatus == KRequestPending)
            {
                TRequestStatus *status = (&iStatus);
                User::RequestComplete(status, KErrNone);
            }
        }
    }
}

void CEcmtMidpDebugSession::FlushDeadConnections()
{
    TInt count;
    while ((count = iDeadConnections.Count()) > 0) {
        CEcmtMidpDebugConnection* dead = iDeadConnections[count-1];
        iDeadConnections.Remove(count-1);
        TInt index = iConnections.Find(dead);
        if (index >= 0)
        {
            iConnections.Remove(index);
            delete dead;
        }
    }
}

/**
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

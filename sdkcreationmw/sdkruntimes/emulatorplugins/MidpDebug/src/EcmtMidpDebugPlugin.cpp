/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ECMT plugin for connecting Java debug agent with IDE
 *
*/


#define TRACE_PREFIX "MidpDebugPlugin: "

#include "EcmtMidpDebugPlugin.h"
#include "EcmtMidpDebugPluginUids.h"
#include "EcmtMidpDebugProtocol.h"
#include "EcmtMidpDebugSession.h"
#include "EcmtMidpDebugTrace.h"
#include "EcmtMessageEvent.h"

#include <ecom.h>
#include <ImplementationProxy.h>

//===========================================================================
// CEcmtMidpDebugPlugin::CScavenger
//===========================================================================

class CEcmtMidpDebugPlugin::CScavenger : public CActive
{
public:
    CScavenger(CEcmtMidpDebugPlugin* aPlugin);
    void DoCancel();
    void RunL();
private:
    CEcmtMidpDebugPlugin* iPlugin;
};

CEcmtMidpDebugPlugin::CScavenger::CScavenger(
    CEcmtMidpDebugPlugin* aPlugin) :
    CActive(EPriorityStandard), 
    iPlugin(aPlugin)
{
    iStatus = KRequestPending;
    CActiveScheduler::Add(this);
    SetActive();
}

void CEcmtMidpDebugPlugin::CScavenger::DoCancel()
{
    TRequestStatus *status = (&iStatus);
    User::RequestComplete(status, KErrCancel);
}

void CEcmtMidpDebugPlugin::CScavenger::RunL()
{
    iStatus = KRequestPending;
    iPlugin->FlushDeadSessions();
    SetActive();
}

//===========================================================================
// CEcmtMidpDebugPlugin
//===========================================================================

CEcmtMidpDebugPlugin::CEcmtMidpDebugPlugin() :
    iMessaging(NULL)
{
}

MEcmtPlugin* CEcmtMidpDebugPlugin::NewL()
{
    CEcmtMidpDebugPlugin* self = new(ELeave)CEcmtMidpDebugPlugin;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

void CEcmtMidpDebugPlugin::ConstructL()
{
    iScavenger = new(ELeave)CScavenger(this);
}

CEcmtMidpDebugPlugin::~CEcmtMidpDebugPlugin()
{
    iSessions.ResetAndDestroy();
    iDeadSessions.Reset();
    if (iScavenger)
    {
        iScavenger->Cancel();
        delete iScavenger;
    }
    TRACE_DEINIT();
}

void CEcmtMidpDebugPlugin::FlushDeadSessions()
{
    TInt count;
    while ((count = iDeadSessions.Count()) > 0) {
        CEcmtMidpDebugSession* dead = iDeadSessions[count-1];
        iDeadSessions.Remove(count-1);
        TInt index = iSessions.Find(dead);
        if (index >= 0)
        {
            iSessions.Remove(index);
            delete dead;
        }
    }
}

void CEcmtMidpDebugPlugin::BindMessagingL(MEcmtMessaging* aMessaging)
{
    TRACE_INIT(aMessaging);
    iMessaging = aMessaging;
    iMessaging->AddMessageEventObserverL(*this, TUid::Uid(
        KECMT_MIDP_DEBUG_PLUGIN_UID));
}

// Parses Ecmt packets
void CEcmtMidpDebugPlugin::HandleNotifyL(const CEcmtMessageEvent& aEvent)
{
    static const TInt KMinMsgSize[] = {
        0,
        ECMT_MIDP_DEBUG_OPEN_SIZE,
        ECMT_MIDP_DEBUG_CLOSE_SIZE,
        ECMT_MIDP_DEBUG_SEND_MIN_SIZE,
        ECMT_MIDP_DEBUG_CONNECT_SIZE,
        ECMT_MIDP_DEBUG_RESET_SIZE
    };

    static const TInt KMaxMsgSize[] = {
        0,
        ECMT_MIDP_DEBUG_OPEN_SIZE,
        ECMT_MIDP_DEBUG_CLOSE_SIZE,
        ECMT_MIDP_DEBUG_SEND_MAX_SIZE,
        ECMT_MIDP_DEBUG_CONNECT_SIZE,
        ECMT_MIDP_DEBUG_RESET_SIZE
    };

    const TPtrC8 msg = iMessaging->Message(aEvent);
    TInt msglen = msg.Length();
    if (msglen >= ECMT_MIDP_DEBUG_HEADER_SIZE)
    {
        // Decode the header
        TUint opcode = msg[ECMT_MIDP_DEBUG_HEADER_OPCODE_OFFSET];
        TUint sid = (msg[ECMT_MIDP_DEBUG_HEADER_SID_OFFSET] << 24) | 
            (msg[ECMT_MIDP_DEBUG_HEADER_SID_OFFSET+1] << 16) |
            (msg[ECMT_MIDP_DEBUG_HEADER_SID_OFFSET+2] << 8) |
            (msg[ECMT_MIDP_DEBUG_HEADER_SID_OFFSET+3]);

        // Check opcode
        if (opcode < ECMT_MIDP_DEBUG_OPCODE_MIN ||
            opcode > ECMT_MIDP_DEBUG_OPCODE_MAX)
        {
            TRACE1("invalid opcode: %u",opcode);
            return;
        }

        // Check message size
        if (msglen < KMinMsgSize[opcode] || 
            msglen > KMaxMsgSize[opcode])
        {
            TRACE2("opcode %u, invalid message size %u",opcode,msglen);
            return;
        }

        // Try to find existing session
        TInt index = -1;
        CEcmtMidpDebugSession* session = FindSession(sid, index);
        switch (opcode) {
        case ECMT_MIDP_DEBUG_OPCODE_OPEN:
            if (session)
            {
                TRACE2("OPEN: destroying session %08X (%d)",session,sid);
                delete session;
                iSessions.Remove(index);
            }
            session = CEcmtMidpDebugSession::NewL(sid,this,iMessaging);
            CleanupStack::PushL(session);
            LEAVE_IF_ERROR(iSessions.Append(session));
            CleanupStack::Pop(session);
            TRACE2("OPEN: created new session %08X (%u)",session,sid);
            break;
                
        case ECMT_MIDP_DEBUG_OPCODE_CLOSE:
            if (session)
            {
                TRACE2("CLOSE: destroying session %08X (%u)",session,sid);
                delete session;
                iSessions.Remove(index);
            }
            else
            {
                TRACE1("CLOSE: no such session: %u",sid); 
            }
            break;

        case ECMT_MIDP_DEBUG_OPCODE_SEND:
            if (session)
            {
                TUint cid = (msg[ECMT_MIDP_DEBUG_SEND_CID_OFFSET] << 24) | 
                    (msg[ECMT_MIDP_DEBUG_SEND_CID_OFFSET+1] << 16) |
                    (msg[ECMT_MIDP_DEBUG_SEND_CID_OFFSET+2] << 8) |
                    (msg[ECMT_MIDP_DEBUG_SEND_CID_OFFSET+3]);
                TUint seq = (msg[ECMT_MIDP_DEBUG_SEND_SEQ_OFFSET] << 24) | 
                    (msg[ECMT_MIDP_DEBUG_SEND_SEQ_OFFSET+1] << 16) |
                    (msg[ECMT_MIDP_DEBUG_SEND_SEQ_OFFSET+2] << 8) |
                    (msg[ECMT_MIDP_DEBUG_SEND_SEQ_OFFSET+3]);
                session->Send(cid, seq,
                    msg.Right(msglen - ECMT_MIDP_DEBUG_SEND_DATA_OFFSET));
            }
            else
            {
                // Tell the other side that this session id is invalid
                TRACE1("SEND: invalid sid %u",sid);
                SendSessionCloseMessage(sid);
            }
            break;

        case ECMT_MIDP_DEBUG_OPCODE_CONNECT:
            if (session)
            {
                TUint cid = (msg[ECMT_MIDP_DEBUG_CONNECT_CID_OFFSET] << 24) | 
                    (msg[ECMT_MIDP_DEBUG_CONNECT_CID_OFFSET+1] << 16) |
                    (msg[ECMT_MIDP_DEBUG_CONNECT_CID_OFFSET+2] << 8) |
                    (msg[ECMT_MIDP_DEBUG_CONNECT_CID_OFFSET+3]);
                TUint port = (msg[ECMT_MIDP_DEBUG_CONNECT_PORT_OFFSET] << 8) | 
                    (msg[ECMT_MIDP_DEBUG_CONNECT_PORT_OFFSET+1]);
                session->ConnectL(cid, port);
            }
            else
            {
                // Tell the other side that this session id is invalid
                TRACE1("CONNECT: invalid sid %u",sid);
                SendSessionCloseMessage(sid);
            }
            break;


        case ECMT_MIDP_DEBUG_OPCODE_RESET:
            if (session)
            {
                TUint cid = (msg[ECMT_MIDP_DEBUG_RESET_CID_OFFSET] << 24) | 
                    (msg[ECMT_MIDP_DEBUG_RESET_CID_OFFSET+1] << 16) |
                    (msg[ECMT_MIDP_DEBUG_RESET_CID_OFFSET+2] << 8) |
                    (msg[ECMT_MIDP_DEBUG_RESET_CID_OFFSET+3]);
                session->Reset(cid);
            }
            else
            {
                // Tell the other side that this session id is invalid
                TRACE1("RESET: invalid sid %u",sid);
                SendSessionCloseMessage(sid);
            }
            break;

        }
    }
}

// Notifies the other side of ECMT connection that the session is closed
void CEcmtMidpDebugPlugin::SendSessionCloseMessage(TUint aSessionId)
{
    TUint sid = aSessionId;
    CEcmtMessageEvent* event = iMessaging->NewMessageEvent(
        TUid::Uid(KECMT_MIDP_DEBUG_GATEWAY_UID),
        ECMT_MIDP_DEBUG_CLOSE_SIZE);
    if (event)
    {
        TPtr8 ptr(iMessaging->Message(*event));
        ptr.FillZ(ECMT_MIDP_DEBUG_CLOSE_SIZE);
        ptr[ECMT_MIDP_DEBUG_HEADER_SID_OFFSET+3] = sid; sid >>= 8;
        ptr[ECMT_MIDP_DEBUG_HEADER_SID_OFFSET+2] = sid; sid >>= 8;
        ptr[ECMT_MIDP_DEBUG_HEADER_SID_OFFSET+1] = sid; sid >>= 8;
        ptr[ECMT_MIDP_DEBUG_HEADER_SID_OFFSET+0] = sid;
        ptr[ECMT_MIDP_DEBUG_HEADER_OPCODE_OFFSET] = 
            ECMT_MIDP_DEBUG_OPCODE_CLOSE;

        // Send the message
        iMessaging->GetMessageSender()->SendMessage(event);
    }
}

// This callback is invoked by CEcmtMidpDebugSession when debug agent exits.
// It may not be safe to delete the session here, instead we schedule
// FlushDeadSessions call to occur later.
void CEcmtMidpDebugPlugin::SessionClosed(CEcmtMidpDebugSession* aSession)
{
    // The session should not call this method twice, but just in case...
    if (iDeadSessions.Find(aSession) < 0 &&
        iSessions.Find(aSession) >= 0)
    {
        iDeadSessions.Append(aSession);
        if (iScavenger->iStatus == KRequestPending)
        {
            TRequestStatus *status = (&iScavenger->iStatus);
            User::RequestComplete(status, KErrNone);
        }
    }
}

// Finds an active session in the list, given its session id. Returns pointer
// to the session object and its index in the session list. Returns NULL
// if session is not found.
CEcmtMidpDebugSession* CEcmtMidpDebugPlugin::FindSession(TUint aSessionId,
                                                         TInt& aIndex) const
{
    for (TInt i=0; i<iSessions.Count(); i++)
    {
        if (iSessions[i]->SessionId() == aSessionId)
        {
            aIndex = i;
            return iSessions[i];
        }
    }
    aIndex = -1;
    return NULL;
}

//===========================================================================
// ECom stuff
//===========================================================================

static TAny* NewEcmtMidpDebugPluginL()
{
    return CEcmtMidpDebugPlugin::NewL();
}

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aCount)
{
    static const TImplementationProxy ImplementationTable[] = {
        {{KECMT_MIDP_DEBUG_PLUGIN_IMPL_UID}, NewEcmtMidpDebugPluginL}
    };

    aCount = sizeof(ImplementationTable)/sizeof(TImplementationProxy);
    return ImplementationTable;
}

/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

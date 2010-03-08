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
* Description:  MIDP Debug plugin
 *
*/

#ifndef _ECMT_MIDP_DEBUG_PLUGIN_H_
#define _ECMT_MIDP_DEBUG_PLUGIN_H_

#include "EcmtPlugin.h"
#include "EcmtMessageEvent.h"
#include "EcmtMidpDebugSessionObserver.h"

/**
 * MIDP Debug plugin
 */
class CEcmtMidpDebugPlugin : public MEcmtPlugin,
                             public MEcmtMessageEventObserver,
                             public MEcmtMidpDebugSessionObserver
{
    class CScavenger;

private:
    CScavenger* iScavenger;
    MEcmtMessaging* iMessaging;
    RPointerArray<CEcmtMidpDebugSession> iSessions;
    RPointerArray<CEcmtMidpDebugSession> iDeadSessions;

public:
    static MEcmtPlugin* NewL();
    virtual ~CEcmtMidpDebugPlugin();
    void FlushDeadSessions();

    // MEcmtPlugin
    virtual void BindMessagingL(MEcmtMessaging* aMessaging);
    
    // MecmtMessageEventObserver
    virtual void HandleNotifyL(const CEcmtMessageEvent& aEvent);

    // MEcmtMidpDebugSessionObserver
    virtual void SessionClosed(CEcmtMidpDebugSession* aSession);

private:
    void ConstructL();
    CEcmtMidpDebugPlugin();
    CEcmtMidpDebugSession* FindSession(TUint aSessionId, TInt& aIndex) const;
    void SendSessionCloseMessage(TUint aSessionId);
};

#endif // _ECMT_MIDP_DEBUG_PLUGIN_H_

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

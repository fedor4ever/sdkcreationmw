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
* Description:  Notification interface between session and plugin objects
 *
*/

#ifndef _ECMT_MIDP_DEBUG_SESSION_OBSERVER_H_
#define _ECMT_MIDP_DEBUG_SESSION_OBSERVER_H_

class CEcmtMidpDebugSession;

/**
 * Defines Session -> Plugin notification mechanism. The session should
 * not call this method twice.
 */
class MEcmtMidpDebugSessionObserver
{
public:
    virtual void SessionClosed(CEcmtMidpDebugSession* aSession) = 0;
};

#endif // _ECMT_MIDP_DEBUG_SESSION_OBSERVER_H_

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

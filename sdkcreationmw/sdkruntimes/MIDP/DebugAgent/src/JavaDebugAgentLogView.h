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


#ifndef __JAVA_DEBUG_AGENT_LOG_VIEW_H_
#define __JAVA_DEBUG_AGENT_LOG_VIEW_H_

#include "JavaDebugAgentBasicLogView.h"

class CJavaDebugAgentLogView : public CJavaDebugAgentBasicLogView
{
public:
    static CJavaDebugAgentLogView* NewLC();

private:
    CJavaDebugAgentLogView();
    void ConstructL();

public:
    // CJavaDebugAgentBasicLogView
    virtual TRect LogScreenRect();

protected:
    // CAknView
    virtual void DoActivateL(const TVwsViewId& aPrevViewId, TUid aMsgId,
                             const TDesC8& aMsg);
};

#endif // __JAVA_DEBUG_AGENT_LOG_VIEW_H_

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

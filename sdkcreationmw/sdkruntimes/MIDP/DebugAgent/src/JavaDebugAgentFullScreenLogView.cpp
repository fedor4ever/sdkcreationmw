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


#include <aknviewappui.h>

#include "JavaDebugAgent.hrh"
#include "JavaDebugAgentRes.h"
#include "JavaDebugAgentFullScreenLogView.h"

#define SUPER CJavaDebugAgentBasicLogView

CJavaDebugAgentFullScreenLogView* CJavaDebugAgentFullScreenLogView::NewLC()
{
    CJavaDebugAgentFullScreenLogView* self = 
        new(ELeave)CJavaDebugAgentFullScreenLogView;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

CJavaDebugAgentFullScreenLogView::CJavaDebugAgentFullScreenLogView() :
SUPER(TUid::Uid(KJavaDebugAgentFullScreenLogViewId))
{
}

void CJavaDebugAgentFullScreenLogView::ConstructL()
{
    BaseConstructL(R_JAVA_DEBUG_AGENT_FULL_SCREEN_LOG_VIEW, 
        R_JAVA_DEBUG_AGENT_FULL_SCREEN_LOG_POPUP_MENUBAR);
}

void CJavaDebugAgentFullScreenLogView::DoActivateL(const TVwsViewId&,
                                                   TUid,const TDesC8&)
{
    Cba()->MakeVisible(EFalse);
    StatusPane()->MakeVisible(EFalse);
    ShowLogScreenL();
}

// CJavaDebugAgentBasicLogView
TRect CJavaDebugAgentFullScreenLogView::LogScreenRect()
{
    return AppUi()->ApplicationRect();
}

/**
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

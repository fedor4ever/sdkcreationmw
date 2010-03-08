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


#include <akntitle.h>
#include <eikbtgpc.h>
#include <barsread.h>
#include <aknviewappui.h>

#include "JavaDebugAgent.hrh"
#include "JavaDebugAgentRes.h"
#include "JavaDebugAgentLogView.h"

#define SUPER CJavaDebugAgentBasicLogView

CJavaDebugAgentLogView* CJavaDebugAgentLogView::NewLC()
{
    CJavaDebugAgentLogView* self = new(ELeave)CJavaDebugAgentLogView;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

CJavaDebugAgentLogView::CJavaDebugAgentLogView() :
SUPER(TUid::Uid(KJavaDebugAgentLogViewId))
{
}

void CJavaDebugAgentLogView::ConstructL()
{
    BaseConstructL(R_JAVA_DEBUG_AGENT_LOG_VIEW,
        R_JAVA_DEBUG_AGENT_LOG_POPUP_MENUBAR);
}

void CJavaDebugAgentLogView::DoActivateL(const TVwsViewId&,TUid,const TDesC8&)
{
    Cba()->MakeVisible(ETrue);
    StatusPane()->MakeVisible(ETrue);

    // Get title pane
    CAknTitlePane* title = (CAknTitlePane*)AppUi()->StatusPane()->
        ControlL(TUid::Uid(EEikStatusPaneUidTitle));
    
    // Configure the title
    TResourceReader reader;
    iCoeEnv->CreateResourceReaderLC(reader, R_JAVA_DEBUG_AGENT_LOG_TITLE);
    title->SetFromResourceL(reader);
    CleanupStack::PopAndDestroy();

    // Add control to stack
    ShowLogScreenL();
}

// CJavaDebugAgentBasicLogView
TRect CJavaDebugAgentLogView::LogScreenRect()
{
    return AppUi()->ClientRect();
}

/**
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

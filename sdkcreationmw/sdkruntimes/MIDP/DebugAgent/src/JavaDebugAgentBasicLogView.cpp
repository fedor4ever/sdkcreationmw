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
#include "JavaDebugAgentBasicLogView.h"

#define SUPER CAknView

CJavaDebugAgentBasicLogView::CJavaDebugAgentBasicLogView(TUid aViewId) :
iViewId(aViewId)
{
}

CJavaDebugAgentBasicLogView::~CJavaDebugAgentBasicLogView()
{
    AppUi()->RemoveFromStack(&iPopupMenuBar);
    if (iScreen) {
        AppUi()->RemoveFromStack(iScreen);
        delete iScreen;
    }
}

void CJavaDebugAgentBasicLogView::BaseConstructL(TInt aViewResId, 
                                                 TInt aMenuResId)
{
    SUPER::BaseConstructL(aViewResId);
    iPopupMenuBar.ConstructL(this, 0, aMenuResId);
    AppUi()->AddToStackL(*this, &iPopupMenuBar);
    iScreen = CJavaDebugAgentLogScreen::NewL(this, LogScreenRect());
    iScreen->SetMopParent(this);
}

void CJavaDebugAgentBasicLogView::HandleClientRectChange()
{
    iScreen->SetRect(LogScreenRect());
}

void CJavaDebugAgentBasicLogView::DynInitMenuPaneL(TInt aResId,
                                                   CEikMenuPane* aMenu)
{
    AppUi()->DynInitMenuPaneL(aResId, aMenu);
}

TUid CJavaDebugAgentBasicLogView::Id() const
{
    return iViewId;
}

void CJavaDebugAgentBasicLogView::ProcessCommandL(TInt aCommand)
{
    SUPER::ProcessCommandL(aCommand);
}

void CJavaDebugAgentBasicLogView::HandleCommandL(TInt aCommand)
{
    iPopupMenuBar.StopDisplayingMenuBar();
    AppUi()->HandleCommandL(aCommand);
}

void CJavaDebugAgentBasicLogView::ShowLogScreenL()
{
    AppUi()->AddToStackL(*this, iScreen);
    iScreen->MakeVisible(ETrue);
    iScreen->DrawNow();
}

void CJavaDebugAgentBasicLogView::DoDeactivate()
{
    if (iScreen) {
        AppUi()->RemoveFromStack(iScreen);
        iScreen->MakeVisible(EFalse);
    }
}

/**
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

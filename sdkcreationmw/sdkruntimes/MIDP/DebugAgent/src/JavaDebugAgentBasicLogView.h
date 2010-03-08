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


#ifndef __JAVA_DEBUG_AGENT_BASIC_LOG_VIEW_H_
#define __JAVA_DEBUG_AGENT_BASIC_LOG_VIEW_H_

#include <aknview.h>
#include <eikmenub.h>
#include "JavaDebugAgentLogScreen.h"

class CJavaDebugAgentBasicLogView : public CAknView
{
private:
    TUid iViewId;
    CJavaDebugAgentLogScreen* iScreen;
    CEikMenuBar iPopupMenuBar;

public:
    virtual ~CJavaDebugAgentBasicLogView();
    inline CJavaDebugAgentLogScreen* Screen();

protected:
    CJavaDebugAgentBasicLogView(TUid aViewId);
    void BaseConstructL(TInt aViewResId, TInt aMenuResId);
    void ShowLogScreenL();

public:
    inline void TryDisplayMenuBarL();
    inline CEikButtonGroupContainer* Cba() const;
    void HandleClientRectChange();

    virtual TRect LogScreenRect() = 0;

    // CAknView
    virtual TUid Id() const;
    virtual void HandleCommandL(TInt aCommand);
    virtual void ProcessCommandL(TInt aCommand);
    virtual void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

protected:
    // CAknView
    virtual void DoDeactivate();
};

// INLINE METHODS

inline CJavaDebugAgentLogScreen* CJavaDebugAgentBasicLogView::Screen() 
    { return iScreen; }
inline void CJavaDebugAgentBasicLogView::TryDisplayMenuBarL()
    { iPopupMenuBar.TryDisplayMenuBarL(); }
inline CEikButtonGroupContainer* CJavaDebugAgentBasicLogView::Cba() const
    { return CAknView::Cba(); }

#endif // __JAVA_DEBUG_AGENT_BASIC_LOG_VIEW_H_

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

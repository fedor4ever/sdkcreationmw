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


#ifndef __JAVA_DEBUG_AGENT_SETTINGS_VIEW_H_
#define __JAVA_DEBUG_AGENT_SETTINGS_VIEW_H_

#include <aknview.h>

class CJavaDebugAgentLogScreen;
class CJavaDebugAgentSettings;
class CJavaDebugAgentSettingsScreen;
class CJavaDebugAgentSettingsView : public CAknView
{
private:
    CJavaDebugAgentLogScreen* iLog;
    CJavaDebugAgentSettingsScreen* iScreen;
    CJavaDebugAgentSettings* iCurrentSettings;
    CJavaDebugAgentSettings* iBackupSettings;
    TUid iPreviousViewId;
    TInt iListResId;

public:
    static CJavaDebugAgentSettingsView* NewLC(CJavaDebugAgentLogScreen* aLog,
                                              TInt aListResId);
    ~CJavaDebugAgentSettingsView();
    inline const CJavaDebugAgentSettings* Settings() const;
    void HandleClientRectChange();

private:
    CJavaDebugAgentSettingsView(CJavaDebugAgentLogScreen* aLog, TInt aResId);
    void ConstructL();
    void ApplySettingsL();

public:
    // CAknView
    virtual TUid Id() const;
    virtual void HandleCommandL(TInt aCommand);

protected:
    // CAknView
    virtual void DoDeactivate();
    virtual void DoActivateL(const TVwsViewId& aPrevViewId,TUid aMsgId,
                             const TDesC8& aMsg);
};

// INLINE METHODS

inline const CJavaDebugAgentSettings*
CJavaDebugAgentSettingsView::Settings() const
    { return iCurrentSettings; }

#endif // __JAVA_DEBUG_AGENT_SETTINGS_VIEW_H_

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

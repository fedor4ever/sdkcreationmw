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


#ifndef __JAVA_DEBUG_AGENT_APP_UI_H__
#define __JAVA_DEBUG_AGENT_APP_UI_H__

#include <aknviewappui.h>
#include "JavaDebugAgentDriver.h"
#include "JavaDebugAgentBasicLogView.h"

class CJavaDebugAgentSettings;
class CJavaDebugAgentSettingsView;
class CJavaDebugAgentAppUi : public CAknViewAppUi, MJavaDebugAgentObserver
{
private:
    CAsyncCallBack iAsyncDeleteDriver;
    CJavaDebugAgentBasicLogView* iLogView;
    CJavaDebugAgentSettingsView* iSettingsView;
    CJavaDebugAgentDriver* iDriver;
    TInt iDefaultSoftKeys;
    TInt iCurrentSoftKeys;
    TBool iSlaveMode;

public:
    CJavaDebugAgentAppUi();
    virtual ~CJavaDebugAgentAppUi();
    virtual void ConstructL();
    virtual void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);

	// CEikAppUi
    virtual void HandleResourceChangeL(TInt aType);

    // MJavaDebugAgentObserver
    virtual void StateChanged(CJavaDebugAgentDriver::TState aState);

private:
    void HandleCommandL(TInt aCommand);
    CJavaDebugAgentDriver* NewDriverLC();
    void DeleteDriver();
    void HandleListenL();
    void HandleConnectL();
    static TInt AsyncDeleteDriver(TAny* aThis);
};

#endif // __JAVA_DEBUG_AGENT_APP_UI_H__

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

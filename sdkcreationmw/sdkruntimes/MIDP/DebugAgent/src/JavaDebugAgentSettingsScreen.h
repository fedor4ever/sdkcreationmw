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


#ifndef __JAVA_DEBUG_AGENT_SETTINGS_SCREEN_H_
#define __JAVA_DEBUG_AGENT_SETTINGS_SCREEN_H_

#include <AknSettingItemList.h>

class CJavaDebugAgentSettings;
class CJavaDebugAgentSettingsBooleanItem;
class CJavaDebugAgentSettingsScreen : public CAknSettingItemList
{
private:
    CJavaDebugAgentSettings* iSettings;
    CJavaDebugAgentSettingsBooleanItem* iAutoCleanItem;
    CJavaDebugAgentSettingsBooleanItem* iFileLogItem;

public:
    static CJavaDebugAgentSettingsScreen* NewL(CJavaDebugAgentSettings* aSet,
                                               TInt aListResId);
    ~CJavaDebugAgentSettingsScreen();

private:
    CJavaDebugAgentSettingsScreen(CJavaDebugAgentSettings* aSettings); 
    void ConstructL(TInt aListResId);
    TBool ChangeValueL(TInt aSettingId);

    // CAknSettingItemList
    CAknSettingItem* CreateSettingItemL(TInt aSettingId);
    void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);

	// CCoeControl
    virtual void SizeChanged();
};

#endif // __JAVA_DEBUG_AGENT_SETTINGS_SCREEN_H_

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

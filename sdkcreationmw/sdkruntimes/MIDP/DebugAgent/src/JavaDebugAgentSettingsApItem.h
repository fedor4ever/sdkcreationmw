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


#ifndef __JAVA_DEBUG_AGENT_SETTINGS_ACCESS_POINT_ITEM_H_
#define __JAVA_DEBUG_AGENT_SETTINGS_ACCESS_POINT_ITEM_H_

#include <commdb.h>
#include <cdblen.h>
#include <AknSettingItemList.h>

class CJavaDebugAgentSettingsApItem : public CAknSettingItem
{
private:
    TInt* iApId;
    HBufC* iAlwaysAskText;
    HBufC* iUserDefinedText;
    CCommsDatabase* iCommsDb;
    TBuf<KCommsDbSvrMaxFieldLength> iApName;
    TBool iApNameValid;

public:
    CJavaDebugAgentSettingsApItem(TInt aId, TInt* aApId);
    ~CJavaDebugAgentSettingsApItem();
    virtual void EditItemL(TBool aCalledFromMenu);
    virtual const TDesC& SettingTextL();

private:
    TBool AlwaysAskOrSelectApL(TInt* aUserDefined);
    TInt UpdateApName();
    void UpdateApNameL();
    CCommsDatabase* CommsDbL();
    const TDesC* AlwaysAskTextL();
    const TDesC* UserDefinedTextL();
};

#endif // __JAVA_DEBUG_AGENT_SETTINGS_ACCESS_POINT_ITEM_H_

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

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


#include <stringloader.h>
#include <aknsettingitemlist.h>
#include <aknradiobuttonsettingpage.h>
#include <ApSettingsHandlerUI.h>
#include <ApUtils.h>

#include "JavaDebugAgentRes.h"
#include "JavaDebugAgentSettings.h"
#include "JavaDebugAgentSettingsApItem.h"

#define SUPER CAknSettingItem
CJavaDebugAgentSettingsApItem::
CJavaDebugAgentSettingsApItem(TInt aId, TInt* aApId) : SUPER(aId), iApId(aApId)
{
}

CJavaDebugAgentSettingsApItem::~CJavaDebugAgentSettingsApItem()
{
    delete iCommsDb;
    delete iAlwaysAskText;
    delete iUserDefinedText;
}

const TDesC& CJavaDebugAgentSettingsApItem::SettingTextL()
{
    if (*iApId != KNoAccessPoint && !iApNameValid)
    {
        if (UpdateApName() == KErrNone)
        {
            iApNameValid = ETrue;
        }
        else
        {
            *iApId = KNoAccessPoint;
        }
    }
    if (*iApId == KNoAccessPoint) 
    {
        return *AlwaysAskTextL();
    } 
    else 
    {
        return iApName;
    }
}

void CJavaDebugAgentSettingsApItem::EditItemL(TBool /*aCalledFromMenu*/)
{
    UpdateApName();
    TBool aUserDefinedAp = ((*iApId) != KNoAccessPoint);
    while (AlwaysAskOrSelectApL(&aUserDefinedAp))
    {
        if (aUserDefinedAp)
        {
            // The primary purpose of this tool is to support
            // on-device-debugging over WLAN. We could filter out WLAN
            // access points here. The way it's done now, this tool can
            // be used for remote on-device debugging over GPRS. Why not?
            CApSettingsHandler *apUi = CApSettingsHandler::NewLC(
                ETrue, 
                EApSettingsSelListIsPopUp,
                EApSettingsSelMenuSelectNormal,
                KEApIspTypeAll,
                EApBearerTypeAllBearers,
                KEApSortNameAscending,
                EIPv4 | EIPv6);
        
            // Override prompt
            apUi->SetTextOverrideL(EPopupPromptText, SettingName());

            // Let the user select the access point
            TUint32 id = *iApId;
            TInt ret = apUi->RunSettingsL(id, id);
            CleanupStack::PopAndDestroy(apUi);

            // Access point selection menu has "Exit" menu item
            if (ret & KApUiEventExitRequested)
            {
                CEikonEnv::Static()->EikAppUi()->HandleCommandL(EEikCmdExit);
                break;
            }

            // Store the settings
            if (ret & KApUiEventSelected)
            {
                *iApId = id;
                iApNameValid = EFalse;
                UpdateListBoxTextL();
                break;
            }
        }
        else 
        {
            if (*iApId != KNoAccessPoint)
            {
                *iApId = KNoAccessPoint;
                UpdateListBoxTextL();
            }
            break;
        }
    }
}

TBool CJavaDebugAgentSettingsApItem::AlwaysAskOrSelectApL(TInt* aUserDefinedAp)
{
    // options array
    CDesCArrayFlat* values = new(ELeave)CDesCArrayFlat(1);
    CleanupStack::PushL(values);
    values->AppendL(*AlwaysAskTextL());
    values->AppendL(*UserDefinedTextL());

    // Create dialog
    CAknRadioButtonSettingPage* dlg = new(ELeave)CAknRadioButtonSettingPage(
        R_JAVA_DEBUG_AGENT_RADIO_BUTTON_SETTING_PAGE, *aUserDefinedAp, values);

    // Show the dialog
    CleanupStack::PushL(dlg);
    dlg->SetSettingTextL(SettingName()); 
    CleanupStack::Pop(dlg);
    TBool ok = dlg->ExecuteLD(CAknSettingPage::EUpdateWhenChanged);
    CleanupStack::PopAndDestroy(values);
    return ok;
}

TInt CJavaDebugAgentSettingsApItem::UpdateApName()
{
    TRAPD(err, UpdateApNameL());
    return err;
}

void CJavaDebugAgentSettingsApItem::UpdateApNameL()
{
    CApUtils* apUtils = CApUtils::NewLC(*CommsDbL());
    apUtils->NameL(*iApId, iApName);
    CleanupStack::PopAndDestroy(apUtils);
}

CCommsDatabase* CJavaDebugAgentSettingsApItem::CommsDbL()
{
    if (!iCommsDb)
    {
        iCommsDb = CCommsDatabase::NewL();
    }
    return iCommsDb;
}

const TDesC* CJavaDebugAgentSettingsApItem::AlwaysAskTextL()
{
    if (!iAlwaysAskText)
    {
        iAlwaysAskText = StringLoader::LoadL(
            R_JAVA_DEBUG_AGENT_ACCESS_POINT_ALWAYS_ASK);
    }
    return iAlwaysAskText;
}

const TDesC* CJavaDebugAgentSettingsApItem::UserDefinedTextL()
{
    if (!iUserDefinedText)
    {
        iUserDefinedText = StringLoader::LoadL(
            R_JAVA_DEBUG_AGENT_ACCESS_POINT_USER_DEFINED);
    }
    return iUserDefinedText;
}

/**
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

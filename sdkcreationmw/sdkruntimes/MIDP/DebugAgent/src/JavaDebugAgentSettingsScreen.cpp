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


#include "JavaDebugAgent.hrh"
#include "JavaDebugAgentRes.h"
#include "JavaDebugAgentSettings.h"
#include "JavaDebugAgentSettingsScreen.h"

// ---------------------------------------------------------------------------
//
//  List box
//
// ---------------------------------------------------------------------------

#define SUPER CAknSettingItemList

CJavaDebugAgentSettingsScreen* 
CJavaDebugAgentSettingsScreen::NewL(CJavaDebugAgentSettings* aSettings, 
                                    TInt aListResId)
{
    CJavaDebugAgentSettingsScreen* self;
    self = new(ELeave)CJavaDebugAgentSettingsScreen(aSettings);
    CleanupStack::PushL(self);
    self->ConstructL(aListResId);
    CleanupStack::Pop(self);
    return self;
}

CJavaDebugAgentSettingsScreen::CJavaDebugAgentSettingsScreen(
    CJavaDebugAgentSettings* aSettings) :
    iSettings(aSettings)
{
}

CJavaDebugAgentSettingsScreen::~CJavaDebugAgentSettingsScreen()
{
}

void CJavaDebugAgentSettingsScreen::ConstructL(TInt aListResId)
{
    ConstructFromResourceL(aListResId);
}

CAknSettingItem* CJavaDebugAgentSettingsScreen::CreateSettingItemL(TInt aSetId)
{
    CJavaDebugAgentSettings::TParam* param = iSettings->FindParam(aSetId);
    if (!param) User::Leave(KErrArgument);
    CAknSettingItem* item = param->CreateSettingItemL();
    switch (aSetId)
    {
    case EJavaDebugAgentAutoCleanItem:
        delete iAutoCleanItem;
        iAutoCleanItem = (CJavaDebugAgentSettingsBooleanItem*)item;
        item = iAutoCleanItem;
        break;

    case EJavaDebugAgentFileLogItem:
        delete iFileLogItem;
        iFileLogItem = (CJavaDebugAgentSettingsBooleanItem*)item;
        break;
    }
    return item;
}

TBool CJavaDebugAgentSettingsScreen::ChangeValueL(TInt aSettingId)
{
    CJavaDebugAgentSettingsBooleanItem* changeItem = NULL;
    switch (aSettingId)
    {
    case EJavaDebugAgentAutoCleanItem:
        changeItem = iAutoCleanItem;
        break;

    case EJavaDebugAgentFileLogItem:
        changeItem = iFileLogItem;
        break;

    default:
        return EFalse;
    }

    changeItem->ChangeValueL();
    changeItem->StoreL();
    changeItem->LoadL();
    changeItem->UpdateListBoxTextL();
    ListBox()->DrawDeferred();
    return ETrue;
}

void CJavaDebugAgentSettingsScreen::HandleListBoxEventL(
    CEikListBox* aListBox, TListBoxEvent aEventType)
{
    TInt index;
    switch(aEventType) {
    case EEventEnterKeyPressed:
    case EEventItemDoubleClicked:
        index = aListBox->CurrentItemIndex();
        if (index >= 0 && index < ListBox()->Model()->NumberOfItems())
        {
            CAknSettingItem* item = SettingItemArray()->At(index);
            if (ChangeValueL(item->Identifier()))
            {
                // Event has been handled
                return;
            }
        }
        // NO BREAK
    default:
        SUPER::HandleListBoxEventL(aListBox, aEventType);
        break;
    }
}

// CCoeControl
void CJavaDebugAgentSettingsScreen::SizeChanged()
{
    ListBox()->SetRect(Rect());
    SUPER::SizeChanged();
}

/**
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

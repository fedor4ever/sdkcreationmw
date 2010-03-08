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


#include "SettingsList.h"
#include "OcrExampleSettingsData.h"
#include "OcrExample.hrh"

using namespace NOCRUtils;

CSettingsList::CSettingsList(TOcrExampleSettingsData &aData)
    : iSettings(aData)
    {
    }

CAknSettingItem* CSettingsList::CreateSettingItemL( TInt aId )
    {
    CAknSettingItem* item = NULL;

    switch (aId)
        {
         case EOCRExampleSettingHighlightColor:
            item = new (ELeave) CAknEnumeratedTextPopupSettingItem(aId,
                                                        iSettings.iHighlightColor);
            break;

        case EOCRExampleSettingEnglish:
            item = new (ELeave) CAknBinaryPopupSettingItem(aId, iSettings.iUseEnglish);
            break;

        case EOCRExampleSettingChinese:
            item = new (ELeave) CAknBinaryPopupSettingItem(aId, iSettings.iUseChinese);
            break;

        case EOCRExampleSettingJapanese:
            item = new (ELeave) CAknBinaryPopupSettingItem(aId, iSettings.iUseJapanese);
            break;

        case EOCRExampleSettingCrosshairHeigth:
            item = new (ELeave) CAknSliderSettingItem(aId, iSettings.iCrossHairHeight );
            break;

        case EOCRExampleSettingCrosshairWidth:
            item = new (ELeave) CAknSliderSettingItem(aId, iSettings.iCrossHairWidth );
            break;
        }

    return item;
    }

void CSettingsList::EditItemL( TInt aIndex, TBool aCalledFromMenu )
    {
    CAknSettingItemList::EditItemL( aIndex, aCalledFromMenu );
    ( *SettingItemArray() )[aIndex]->StoreL();
    }

void CSettingsList::SizeChanged()
    {
    if (ListBox())
        {
        ListBox()->SetRect(Rect());
        }
    }


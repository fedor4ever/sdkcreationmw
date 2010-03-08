/*
* Copyright (c) 2005 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDE FILES
#include <coemain.h>
#include <DynamicSettingList.rsg>
#include "DynamicSettingList.hrh"
#include "DynamicSettingListAppView.h"
#include "DynamicSettingListSlider.h"
#include <eikfrlbd.h>

// CONSTANTS
_LIT(KEmptyText, "None");
_LIT(KName, "Text");
_LIT(KName2, "Integer");
_LIT(KName3, "Password");
_LIT(KName4, "Volume");
_LIT(KName5, "Slider");
_LIT(KName6, "Date");
_LIT(KName7, "Time offset");
_LIT(KName8, "IP");
_LIT(KName9, "Enumerated text");
_LIT(KName10, "Binary");
_LIT(KEnumText1, "Enum text 1");
_LIT(KEnumText2, "Enum text 2");
const TUint32 KDefaultIp = 0;
const TUint KDefaultIpPort = 80;

// ================= MEMBER FUNCTIONS =======================

// Constructor
CDynamicSettingListAppView::CDynamicSettingListAppView()
    : iVolume(KDefaultVolume), iIp(KDefaultIp, KDefaultIpPort)
    {
    iDate.HomeTime();
    }

// Destructor
CDynamicSettingListAppView::~CDynamicSettingListAppView()
    {
    delete iItemList;
    }

// ----------------------------------------------------
// CDynamicSettingListAppView::ConstructL()
// Symbian OS default constructor can leave.
// ----------------------------------------------------
//
void CDynamicSettingListAppView::ConstructL(const TRect& aRect)
    {
    // Create a window for this application view
    CreateWindowL();

    iItemList = new (ELeave) CAknSettingItemList;
    iItemList->SetMopParent(this);
    iItemList->ConstructFromResourceL(R_ENTRY_SETTINGS_LIST);

    LoadListL();

    iItemList->MakeVisible(ETrue);
    iItemList->SetRect(aRect);
    iItemList->ActivateL();
    iItemList->ListBox()->UpdateScrollBarsL();
    iItemList->DrawNow();

    // Set the windows size
    SetRect(aRect);

    // Activate the window, which makes it ready to be drawn
    ActivateL();
    }

// ----------------------------------------------------
// CDynamicSettingListAppView::Draw()
// This function is used for window server-initiated
// redrawing of controls, and for some
// application-initiated drawing.
// ----------------------------------------------------
//
void CDynamicSettingListAppView::Draw(const TRect& /*aRect*/) const
    {
    // Get the standard graphics context
    CWindowGc& gc = SystemGc();

    // Gets the control's extent
    TRect rect = Rect();

    // Clears the screen
    gc.Clear(rect);
    }

// ----------------------------------------------------
// CDynamicSettingListAppView::CountComponentControls()
// Gets the number of controls contained in a compound
// control.
// ----------------------------------------------------
//
TInt CDynamicSettingListAppView::CountComponentControls() const
    {
    TInt count = 0;
    if (iItemList)
        count++;
    return count;
    }

// ----------------------------------------------------
// CDynamicSettingListAppView::ComponentControl()
// Gets the specified component of a compound control.
// ----------------------------------------------------
//
CCoeControl* CDynamicSettingListAppView::ComponentControl(TInt /*aIndex*/) const
    {
    return iItemList;
    }

// ----------------------------------------------------
// CDynamicSettingListAppView::OfferKeyEventL()
// When a key event occurs, the control framework calls
// this function for each control on the control stack,
// until one of them can process the key event
// (and returns EKeyWasConsumed).
// ----------------------------------------------------
//
TKeyResponse CDynamicSettingListAppView::OfferKeyEventL(    const TKeyEvent& aKeyEvent,
                                                                TEventCode aType )
    {
    if(aType != EEventKey)
        {
        return EKeyWasNotConsumed;
        }
    else if(iItemList)
        {
        return iItemList->OfferKeyEventL( aKeyEvent, aType );
        }
    else
        {
        return EKeyWasNotConsumed;
        }

    }

// ----------------------------------------------------
// CDynamicSettingListAppView::LoadListL()
// Loads the setting item list dynamically.
// ----------------------------------------------------
//
void CDynamicSettingListAppView::LoadListL()
    {
    TBool isNumberedStyle = iItemList->IsNumberedStyle();
    CArrayPtr<CGulIcon>* icons = iItemList->ListBox()->ItemDrawer()->FormattedCellData()->IconArray();


    /* Text setting item */
    CAknTextSettingItem* item = new (ELeave) CAknTextSettingItem(1, iText);
    CleanupStack::PushL(item);
    item->SetEmptyItemTextL( KEmptyText );
    // The same resource id can be used for multiple text setting pages.
    item->ConstructL(isNumberedStyle, 1, KName, icons, R_TEXT_SETTING_PAGE, -1);
    iItemList->SettingItemArray()->AppendL(item);
    CleanupStack::Pop(item);


    /* Integer setting item */
    CAknIntegerEdwinSettingItem* item2 = new (ELeave) CAknIntegerEdwinSettingItem(2, iNumber);
    CleanupStack::PushL(item2);
    // The same resource id can be used for multiple integer setting pages.
    item2->ConstructL(isNumberedStyle, 2, KName2, icons, R_INTEGER_SETTING_PAGE, -1);
    iItemList->SettingItemArray()->AppendL(item2);
    CleanupStack::Pop(item2);


    /* Password setting item */
    CAknPasswordSettingItem* item3 = new (ELeave) CAknPasswordSettingItem(  3,
                                                                            CAknPasswordSettingItem::EAlpha,
                                                                            iPassword);
    CleanupStack::PushL(item3);
    item3->SetEmptyItemTextL( KEmptyText );
    // The same resource id can be used for multiple password setting pages.
    item3->ConstructL(isNumberedStyle, 3, KName3, icons, R_PASSWORD_SETTING_PAGE, -1);
    iItemList->SettingItemArray()->AppendL(item3);
    CleanupStack::Pop(item3);


    /* Volume setting item */
    CAknVolumeSettingItem* item4 = new (ELeave) CAknVolumeSettingItem(4, iVolume);
    CleanupStack::PushL(item4);
    // The same resource id can be used for multiple volume setting pages.
    item4->ConstructL(isNumberedStyle, 4, KName4, icons, R_VOLUME_SETTING_PAGE, -1);
    iItemList->SettingItemArray()->AppendL(item4);
    CleanupStack::Pop(item4);


   /* Slider setting item. Read comments in CDynamicSettingListSlider, why
    * a derived class is used instead of CAknSliderSettingItem
    */
    CDynamicSettingListSlider* item5 = new (ELeave) CDynamicSettingListSlider(5, iSlider);
    CleanupStack::PushL(item5);
    // The same resource id can be used for multiple slider setting pages.
    item5->ConstructL(isNumberedStyle, 5, KName5, icons, R_SLIDER_SETTING_PAGE, -1);
    iItemList->SettingItemArray()->AppendL(item5);
    CleanupStack::Pop(item5);


    /* Date setting item */
    CAknTimeOrDateSettingItem* item6 = new (ELeave) CAknTimeOrDateSettingItem(  6,
                                                                                CAknTimeOrDateSettingItem::EDate,
                                                                                iDate);
    CleanupStack::PushL(item6);
    // The same resource id can be used for multiple date setting pages.
    item6->ConstructL(isNumberedStyle, 6, KName6, icons, R_DATE_SETTING_PAGE, -1);
    iItemList->SettingItemArray()->AppendL(item6);
    CleanupStack::Pop(item6);


    /* Time offset setting item */
    CAknTimeOffsetSettingItem* item7 = new (ELeave) CAknTimeOffsetSettingItem(7, iTime);
    CleanupStack::PushL(item7);
    // The same resource id can be used for multiple time offset setting pages.
    item7->ConstructL(isNumberedStyle, 7, KName7, icons, R_TIMEOFFSET_SETTING_PAGE, -1);
    iItemList->SettingItemArray()->AppendL(item7);
    CleanupStack::Pop(item7);


    /* Ip field setting item */
    CAknIpFieldSettingItem* item8 = new (ELeave) CAknIpFieldSettingItem(8, iIp);
    CleanupStack::PushL(item8);
    // The same resource id can be used for multiple ip field setting pages.
    item8->ConstructL(isNumberedStyle, 8, KName8, icons, R_IPFIELD_SETTING_PAGE, -1);
    iItemList->SettingItemArray()->AppendL(item8);
    CleanupStack::Pop(item8);


    /* Enumerated text setting item */
    CAknEnumeratedTextPopupSettingItem* item9 = new (ELeave) CAknEnumeratedTextPopupSettingItem(9, iEnumText);
    CleanupStack::PushL(item9);
    // The same resource id can be used for multiple enumerated text setting pages.
    item9->ConstructL(isNumberedStyle, 9, KName9, icons, R_ENUMERATEDTEXT_SETTING_PAGE, -1, 0, R_POPUP_SETTING_TEXTS);

    // Load texts dynamically.
    CArrayPtr<CAknEnumeratedText>* texts = item9->EnumeratedTextArray();
    texts->ResetAndDestroy();
    CAknEnumeratedText* enumText;
    // Text 1
    HBufC* text = KEnumText1().AllocLC();
    enumText = new (ELeave) CAknEnumeratedText(0, text);
    CleanupStack::Pop(text);
    CleanupStack::PushL(enumText);
    texts->AppendL(enumText);
    CleanupStack::Pop(enumText);
    // Text 2
    text = KEnumText2().AllocLC();
    enumText = new (ELeave) CAknEnumeratedText(1, text);
    CleanupStack::Pop(text);
    CleanupStack::PushL(enumText);
    texts->AppendL(enumText);
    CleanupStack::Pop(enumText);

    iItemList->SettingItemArray()->AppendL(item9);
    CleanupStack::Pop(item9);


    /* Binary popup setting item */
    CAknBinaryPopupSettingItem* item10 = new (ELeave) CAknBinaryPopupSettingItem(10, iBinary);
    CleanupStack::PushL(item10);
    // The same resource id can be used for multiple binary setting pages.
    item10->ConstructL(isNumberedStyle, 10, KName10, icons, R_BINARY_SETTING_PAGE, -1, 0, R_BINARY_TEXTS);

    // Load texts dynamically.
    texts = item10->EnumeratedTextArray();
    texts->ResetAndDestroy();
    // Text 1
    text = KEnumText1().AllocLC();
    enumText = new (ELeave) CAknEnumeratedText(0, text);
    CleanupStack::Pop(text);
    CleanupStack::PushL(enumText);
    texts->AppendL(enumText);
    CleanupStack::Pop(enumText);
    // Text 2
    text = KEnumText2().AllocLC();
    enumText = new (ELeave) CAknEnumeratedText(1, text);
    CleanupStack::Pop(text);
    CleanupStack::PushL(enumText);
    texts->AppendL(enumText);
    CleanupStack::Pop(enumText);

    // Sets the correct text visible
    item10->LoadL();

    iItemList->SettingItemArray()->AppendL(item10);
    CleanupStack::Pop(item10);

    // Required when there is only one setting item.
    iItemList->SettingItemArray()->RecalculateVisibleIndicesL();

    iItemList->HandleChangeInItemArrayOrVisibilityL();
    }

// ----------------------------------------------------
// CDynamicSettingListAppView::StoreSettingsL()
// Stores the settings of the setting list.
// ----------------------------------------------------
//
void CDynamicSettingListAppView::StoreSettingsL()
    {
    iItemList->StoreSettingsL();
    }

// End of file
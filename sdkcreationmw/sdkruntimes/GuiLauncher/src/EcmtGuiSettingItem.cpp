/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is for accessing the user selected item texts from
                 settingpage.
*
*/


#include <aknpopupsettingpage.h> 
#include "ecmtguisettingitem.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ default constructor
// ---------------------------------------------------------------------------
//
CEcmtGuiSettingItem::CEcmtGuiSettingItem( CEcmtGuiSettingList& aSettingList,
                                          TInt aIdentifier,
                                          TInt& aValue )
: CAknEnumeratedTextPopupSettingItem(aIdentifier, aValue)
, iSettingList( aSettingList )
    {
    
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CEcmtGuiSettingItem::~CEcmtGuiSettingItem()
    {
    
    }

// ---------------------------------------------------------------------------
// From class CAknSettingItem.
// Handles setting page events.
// ---------------------------------------------------------------------------
//
void CEcmtGuiSettingItem::HandleSettingPageEventL( CAknSettingPage* aSettingPage,  
                                    TAknSettingPageEvent aEventType )
    {
    CEikFormattedCellListBox* listbox = (CEikFormattedCellListBox *) 
                                                aSettingPage->EditorControl();
    TInt selectedIndex = listbox->CurrentItemIndex();
            
    switch( aEventType )
        {
        case EEventSettingChanged:
            break;
        case EEventSettingCancelled:
            break;
        case EEventSettingOked:
                                    
            if( selectedIndex != KErrNotFound )
                {
                TPtrC selTextPtr = listbox->Model()->ItemText( selectedIndex );
                HBufC8* selText8 = HBufC8::NewLC( selTextPtr.Length() );
                selText8->Des().Copy( selTextPtr );
                //Inform EcmtGuiSetting list about the selected item.                
                iSettingList.SettingPageOkSelectedL( selText8->Des() );
                CleanupStack::PopAndDestroy(); // selText8
                }
            
            break;
        default:
            break;
        }
    }


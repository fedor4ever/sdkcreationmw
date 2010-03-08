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
* Description: 
*
*/


#include "listbox.hrh"
#include "ListboxSettingList.h"
#include "NaviScrollTimer.h"

// Listbox class name is shown on navigation pane
_LIT(KNaviPaneText, "CAknSettingItemList");

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------------------------
// CListboxSettingList::CListboxSettingList()
// C++ default constructor
// ---------------------------------------------------------------------------
//
CListboxSettingList::CListboxSettingList()
    {  	    
  	}

// ---------------------------------------------------------------------------
// CListboxSettingList::~CListboxSettingList()
// Destructor
// ---------------------------------------------------------------------------
// 	
CListboxSettingList::~CListboxSettingList()
  	{  	    
  	if( iTimer ) delete iTimer;
  	}

// ---------------------------------------------------------------------------
// CListboxSettingList::NewL()
// Two-phased constructor.
// ---------------------------------------------------------------------------
//  	
CListboxSettingList* CListboxSettingList::NewL()
  	{
  	CListboxSettingList* self = new ( ELeave ) CListboxSettingList;
  	CleanupStack::PushL( self );
  	self->ConstructL();
  	CleanupStack::Pop( self );
  	return self;
  	}
  
// ---------------------------------------------------------------------------
// CListboxSettingList::ConstructL()
// 2nd phase constructor
// ---------------------------------------------------------------------------
//	
void CListboxSettingList::ConstructL()
    {
    iTimer = CNaviScrollTimer::NewL( KNaviPaneText );
    iTimer->StartScroll();
    }

// ---------------------------------------------------------------------------
// CListboxSettingList::CreateSettingItemL()
// Creates setting item
// ---------------------------------------------------------------------------
//  	
CAknSettingItem* CListboxSettingList::CreateSettingItemL( TInt aSettingId )
    {
    CAknSettingItem* settingItem = NULL;
    
    switch( aSettingId )
        {
        case EListboxSettingUsername:
            settingItem = new ( ELeave ) 
                        CAknTextSettingItem( aSettingId, iText );
            break;
        case EListboxSettingConnectMode:
            settingItem = new ( ELeave ) 
                        CAknEnumeratedTextPopupSettingItem( aSettingId, iSelected );
            break;
        default:
            break;    
        }    
     
    return settingItem;
    }   

// ---------------------------------------------------------------------------
// CListboxSettingList::EditItemL()
// Edits setting item data.
// ---------------------------------------------------------------------------
//    
void CListboxSettingList::EditItemL( TInt aIndex, TBool aCalledFromMenu )
    {
    TInt currentItem = ((CAknSettingItemList*)this)->ListBox()->View()->CurrentItemIndex();
    CAknSettingItemList::EditItemL( currentItem, aCalledFromMenu );
  ( *SettingItemArray() )[aIndex]->UpdateListBoxTextL();
  ( *SettingItemArray() )[aIndex]->StoreL();    
    
    }


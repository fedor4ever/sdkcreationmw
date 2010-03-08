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


#ifndef __LISTBOXSETTINGLIST_H__
#define __LISTBOXSETTINGLIST_H__


#include <AknSettingItemList.h>
#include <aknsettingpage.h> 

class CNaviScrollTimer;
      
/**
*  CListboxSettingList class
*  This class is a container for CListboxSettingView. It implements functionality
*  for setting list. 
*/                             
class CListboxSettingList : public CAknSettingItemList 
    {
    public:
        
        /**
        * C++ default constructor.
        */
        CListboxSettingList();
        
        /**
        * Destructor.
        */
      	~CListboxSettingList();
      	
      	/**
        * Two-phased constructor.
        */
      	static CListboxSettingList* NewL();
      	
    public: // From CAknSettingItemList
            
        /**
        * From CAknSettingItemList.
        * Creates setting item.
        *
        * @param aSettingId Type of the setting item.
        * @return  Pointer to CAknSettingItem object.
        */
        CAknSettingItem* CreateSettingItemL( TInt aSettingId );
            
        /**
        * From CAknSettingItemList.
        * Edits setting item data.
        *
        * @param aIndex Setting item index.
        * @param aCalledFromMenu is called from menu.
        */
        void EditItemL( TInt aIndex, TBool aCalledFromMenu );
    
    private:
    
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    
    private: // Data
        
        /**
        * Descriptor for user name. 
        */
        TBuf<70> iText;
        
        /**
        * Integer representing the selected index from "Connection mode" 
        * setting page.
        */
        TInt iSelected;
        
        /**
        * Timer for scrolling listbox class name on the navigation pane.
        *
        * Owned
        */
        CNaviScrollTimer* iTimer;
    
    };
    
#endif // __LISTBOXSETTINGLIST_H__    
    
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
* Description:  This is a container class for the EcmtGuiSettingsView. It has
*                one control, CAknEnumeratedTextPopupSettingItem.
*
*/


#ifndef __ECMTGUI_SETTINGLIST_H__
#define __ECMTGUI_SETTINGLIST_H__


#include <AknSettingItemList.h>
#include <aknsettingpage.h> 

#include "EcmtGuiSettingDataHandler.h"


/**
 *  This is a container class for the EcmtGuiSettingsView. It has
 *  one control, CAknEnumeratedTextPopupSettingItem. 
 *
 *  @since S60 v3.1
 */
class CEcmtGuiSettingList : public CAknSettingItemList
    {

public: // Functions from base classes

    /**
     * C++ default constructor.
     */
    CEcmtGuiSettingList();
    
    /**
     * Destructor.
     */
  	~CEcmtGuiSettingList();
  	
  	/**
     * Two-phased constructor.
     */
  	static CEcmtGuiSettingList* NewL();
   	    
public: // New functions

    /**
     * Sets data items to the iSettingItem member. It uses 
     * CEcmtGuiSettingDataHandler to retrieve the data.
     */
	void SetDataL();
	
	/**
     * Stores user selection to file for later use. It uses 
     * CEcmtGuiSettingDataHandler to store the selection.
     */
	void StoreSelectionL();
	
	/**
     * This function is called from CEcmtGuiSettingItem when user presses OK
     * softkey from the setting page. If WLAN is selected it displays a list query
     * of WLAN access points.
     *
     * @param aSelItemText Selected item text.
     */
	void SettingPageOkSelectedL( const TDesC8& aSelItemText );
    
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
        
        
private: // From CAknSettingItemList
        
    /**
     * From CAknSettingItemList.
     * Handles the key events.
     *
     * @param aKeyEvent Key event
     * @param aType Type of key event
     * @return if key-event is consumed, EKeyWasConsumed. 
     *          Else EKeyWasNotConsumed
     */
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                     TEventCode aType );

public: // From CCoeControl.
    
    /**
     * From CCoeControl.
     * Draws the control.
     *
     * @param aRect The region of the control to be redrawn.
     */
    void Draw(const TRect& /*aRect*/) const;
           
    /**
     * From CCoeControl.
     * The function is called whenever SetExtent(), SetSize(), SetRect(), 
     * SetCornerAndSize(), or SetExtentToWholeScreen() are called on the 
     * control.
     */
    void SizeChanged();

private: 

    /**
     * By default Symbian 2nd phase constructor is private.
     */
     void ConstructL(const TRect& aRect );

private: // Data

    /**
     * CAknEnumeratedTextPopupSettingItem uses this to store user selected 
     * value. 
     */
    TInt iValue;
    
    /**
     * The setting item for the bearer selection.
     * Not own.
     */
    CAknEnumeratedTextPopupSettingItem* iSettingItem;
       
    /**
     * Data handler for the Setting list.
     * Own.
     */
    CEcmtGuiSettingDataHandler* iData;
    
    };

    

#endif // __ECMTGUI_SETTINGLIST_H__

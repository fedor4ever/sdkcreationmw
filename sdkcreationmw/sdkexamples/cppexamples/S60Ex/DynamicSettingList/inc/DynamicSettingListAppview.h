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


#ifndef __DYNAMICSETTINGLIST_APPVIEW_H__
#define __DYNAMICSETTINGLIST_APPVIEW_H__

// INCLUDE FILES
#include "dynamicsettinglist.hrh"
#include <coecntrl.h>
#include <aknsettingitemlist.h> 

// CLASS DECLARATION

/**
* A view class for a dynamic setting list.
*/
class CDynamicSettingListAppView : public CCoeControl
    {
public: // Constructor and destructor
	
    /**
    * Symbian OS default constructor.
    */      
	void ConstructL(const TRect& aRect);
	
	/**
	* Constructor
	*/
	CDynamicSettingListAppView();
	
	/**
	* Destructor
	*/
     ~CDynamicSettingListAppView();
          
private: // Functions from base classes

	/**
    * From CCoeControl,Draw.
    */
    void Draw(const TRect& aRect) const;

	/**
    * From CoeControl,CountComponentControls.
    */        
    TInt CountComponentControls() const;

	/**
    * From CCoeControl,ComponentControl.
    */
    CCoeControl* ComponentControl(TInt aIndex) const;

	/**
    * From CCoeControl,OfferKeyEventL.
    */
	TKeyResponse OfferKeyEventL(	const TKeyEvent& aKeyEvent, TEventCode aType );
	

private: // New Functions

	/**
	* Loads the setting list dynamically.
	*/	
	void LoadListL();

	/**
	* Stores the settings of the setting list.
	*/	
    void StoreSettingsL();


private: // Data members

	CAknSettingItemList* iItemList;
	
	TBuf<KMaxTextLength>		iText;
	TBuf<KMaxPasswordLength>	iPassword;
	TInt						iNumber;
	TInt						iVolume;
	TInt						iSlider;
	TTime						iDate;
	TTimeIntervalSeconds		iTime;
	TInetAddr					iIp;
	TInt						iEnumText;
	TBool						iBinary;
	
    };


#endif // __DYNAMICSETTINGLIST_APPVIEW_H__

// End of file
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


#ifndef ECMTGUISETTINGITEM_H
#define ECMTGUISETTINGITEM_H

#include <aknsettingitemlist.h> 
#include "ecmtguisettinglist.h"

/**
 *  This class is for accessing the user selected item texts from
 *  settingpage.
 *
 *  @since S60 v3.1
 */
class CEcmtGuiSettingItem : public CAknEnumeratedTextPopupSettingItem
    {
        
public:
    
    /**
     * C++ default constructor.
     */
    CEcmtGuiSettingItem( CEcmtGuiSettingList& aSettingList,
                         TInt aIdentifier, TInt& aValue );
    
    /**
     * Destructor.
     */
    ~CEcmtGuiSettingItem();        
    
public: // from CAknSettingItem
    
    /**
     * From CAknSettingItem.
     * Handles setting page events. From here it is possible to access
     * to selected item texts after OK softkey pressing.
     *
     * @param aSettingPage The setting page.
     * @param aEventType The event.
     */
    void HandleSettingPageEventL( CAknSettingPage* aSettingPage,  
                                    TAknSettingPageEvent aEventType );
    
private: //Data

    /**
     * Reference to CEcmtGuiSettingList object. Needed from 
     * HandleSettingPageEventL
     */
    CEcmtGuiSettingList& iSettingList;
    
    };
    
#endif //ECMTGUISETTINGITEM_H

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


#ifndef OCRSETTINGLIST_H
#define OCRSETTINGLIST_H

#include <AknSettingItemList.h>

class TOcrExampleSettingsData;

/**
 * namespace contains various utility classes
 */
namespace NOCRUtils
{

/**
 * CSettingsList
 */
class CSettingsList : public CAknSettingItemList
    {
    public:
        /** constructor */
        CSettingsList( TOcrExampleSettingsData& aData );

    public: // From CAknSettingItemList
        /**
        * CreateSettingItemL
        * Creates SettingItem
        * @param identifier
        * @return pointer to CAknSettingItem object
        */
        CAknSettingItem* CreateSettingItemL( TInt identifier );

        /**
        * EditItemL
        * Edits data.
        * @param aIndex index of setting item
        * @param aCalledFromMenu is called from menu
        */
        void EditItemL( TInt aIndex, TBool aCalledFromMenu );

    private:
        /** from CAknSettingItemList */
	    void SizeChanged();

    private:

        /** settings data */
        TOcrExampleSettingsData &iSettings;
    };

} // namespace

#endif

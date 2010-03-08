/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef REGISTRATIONLISTBOX_H
#define REGISTRATIONLISTBOX_H

// INCLUDE FILES
#include <AknSettingItemList.h>

#include "Registration.hrh"
#include <Registration.rsg>
#include "RegistrationItemdata.h"

// FORWARD DECLARATIONS
class CRegistrationView;

// CLASS DECLARATION

/**
* CRegistrationListbox
*/
class CRegistrationListbox : public CAknSettingItemList 
    {
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
                
    public: // New functions
        /**
        * SetData
        * Sets List's item data.
        * @param aData data to set
        */
        void SetData( TRegistrationItemdata* aItemData );

    private: // From CAknSettingItemList
        /**
        * OfferKeyEventL.
        * Handles the key events.
        * @param aKeyEvent Key event
        * @param aType Type of key event 
        *  (EEventKey, EEventKeyUp or EEventKeyDown)
        * @return if key-event is consumed, EKeyWasConsumed. 
        *  Else EKeyWasNotConsumed
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, 
                                     TEventCode aType );
                               
    private: // New functions
        /**
        * SizeChanged
        * Called by framework when the size is changed.
        */
        void SizeChanged();

    private: //Data

        TRegistrationItemdata*      iItemData;
        CRegistrationView*          iView;
    };

#endif // REGISTRATIONLISTBOX_H

// End of File

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


#ifndef OCREXAMPLESETTINGSCONTAINER_H
#define OCREXAMPLESETTINGSCONTAINER_H

// INCLUDES
#include <coecntrl.h>

class TOcrExampleSettingsData;
namespace NOCRUtils
{
    class CSettingsList;
}

/**
 * container for settings-view
 */
class COcrExampleSettingsContainer : public CCoeControl
    {
    public:
		/**
		* NewL.
		* Two-phased constructor.
		* Construct a COcrExampleSettingsContainer using two phase construction,
		* and return a pointer to the created object
		* @return A pointer to the created instance of COcrExampleSettingsContainer
		*/
		static COcrExampleSettingsContainer* NewL( const TRect& aRect,
		                                           TOcrExampleSettingsData& aData );
        /** Destructor. */
        virtual ~COcrExampleSettingsContainer();

    private:
        /** C++ constructor. */
        COcrExampleSettingsContainer(TOcrExampleSettingsData& aSettingsData);

        /** Symbian OS default constructor. */
        void ConstructL(const TRect& aRect);

    public: // Functions from base classes
        /**
        * From CCoeControl set the size and position of its component controls.
        */
        void SizeChanged();

    protected: // Functions from base classes

        /**
        * From CCoeControl return the number of controls owned
        * @return TInt number of controls
        */
        TInt CountComponentControls() const;

        /**
        * From CCoeControl returns a control
        * @param aIndex index of a control
        * @return CCoeControl* pointer on a control
        */
        CCoeControl* ComponentControl(TInt aIndex) const;

        /**
        * From CCoeControl event handling section
        * @param aKeyEvent the key event
        * @param aType the type of the event
        * @return TKeyResponse key event was used by this control or not
        */
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );
    private: //data

        /** settings list */
        NOCRUtils::CSettingsList    *iSettingsList;

        /** settings data */
        TOcrExampleSettingsData     &iSettingsData;
    };

#endif  //OCREXAMPLESETTINGSCONTAINER_H

// End of File

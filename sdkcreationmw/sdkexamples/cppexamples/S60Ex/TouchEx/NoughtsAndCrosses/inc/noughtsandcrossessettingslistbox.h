/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __CNOUGHTSANDCROSSESSETTINGSLISTBOX_H__
#define __CNOUGHTSANDCROSSESSETTINGSLISTBOX_H__

// INCLUDES
#include <AknSettingItemList.h>


// FORWARD DECLARATIONS
class CNoughtsAndCrossesInformationAndSettings;


// CLASS DECLARATION

/**
*  CNoughtsAndCrossesSettingListbox.
*  
*/
class CNoughtsAndCrossesSettingListbox : public CAknSettingItemList 
    {
    public: // Constructors and destructor
	
        /**
        * CNoughtsAndCrossesSettingListbox
        * C++ default constructor.
        */
        CNoughtsAndCrossesSettingListbox(
            CNoughtsAndCrossesInformationAndSettings& aSettings);

    public: // Functions from base classes

        /**
        * From CAknSettingItemList, CreateSettingItemL.
        */
        virtual CAknSettingItem* CreateSettingItemL(TInt identifier);

    private: // Functions from base classes

        /**
        * From CoeControl, HandleResourceChange.
        */
        virtual void HandleResourceChange(TInt aType);

        /**
        * From CoeControl, SizeChanged.
        */
        virtual void SizeChanged();

    private: // Data

        CNoughtsAndCrossesInformationAndSettings& iSettings;

        TBool iPlayerType;
    };

#endif // __CNOUGHTSANDCROSSESSETTINGSLISTBOX_H__

// End of File

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


// INCLUDE FILES
#include <aknglobalnote.h>
#include <OcrExample.rsg>

#include "OcrExampleAppUi.h"
#include "OcrExampleSettingsData.h"
#include "OcrExampleSettingsContainer.h"
#include "SettingsList.h"


// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// COcrExampleSettingsContainer::COcrExampleSettingsContainer
// C++ constructor
// ---------------------------------------------------------------------------
//
COcrExampleSettingsContainer::COcrExampleSettingsContainer(TOcrExampleSettingsData& aSettingsData)
    : iSettingsData(aSettingsData)
    {
    }

// -----------------------------------------------------------------------------
// COcrExampleSettingsContainer::NewLC()
// Symbian two-phased constructor.
// -----------------------------------------------------------------------------
//
COcrExampleSettingsContainer* COcrExampleSettingsContainer::NewL(
                                                const TRect& aRect,
                                                TOcrExampleSettingsData& aData )
    {
    COcrExampleSettingsContainer* self =
        new ( ELeave ) COcrExampleSettingsContainer(aData);
    self->ConstructL( aRect );
    return self;
    }



// ---------------------------------------------------------------------------
// COcrExampleSettingsContainer::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------------------------
//
void COcrExampleSettingsContainer::ConstructL(const TRect& /* aRect */)
    {
    iSettingsList = new (ELeave) NOCRUtils::CSettingsList(iSettingsData);
    iSettingsList->SetContainerWindowL(*this);
    iSettingsList->ConstructFromResourceL(R_OCR_SETTING_ITEM_LIST);
    }


// Destructor
COcrExampleSettingsContainer::~COcrExampleSettingsContainer()
    {
    delete iSettingsList;
    }


// ---------------------------------------------------------------------------
// COcrExampleSettingsContainer::CountComponentControls
// From CCoeControl return the number of controls owned
// ---------------------------------------------------------------------------
//
TInt COcrExampleSettingsContainer::CountComponentControls() const
    {
    return 1;
    }


// ---------------------------------------------------------------------------
// COcrExampleSettingsContainer::ComponentControl
// From CCoeControl returns a control
// ---------------------------------------------------------------------------
//
CCoeControl* COcrExampleSettingsContainer::ComponentControl(TInt /* aIndex */ ) const
    {
    return iSettingsList;
    }

// ---------------------------------------------------------------------------
// COcrExampleSettingsContainer::OfferKeyEventL
// Key event handling
// ---------------------------------------------------------------------------
//
TKeyResponse COcrExampleSettingsContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    if ( aKeyEvent.iCode == EKeyLeftArrow ||
         aKeyEvent.iCode == EKeyRightArrow )
	    {
        // Listbox takes all events even if it doesn't use them
        return EKeyWasNotConsumed;
	    }

    return iSettingsList->OfferKeyEventL( aKeyEvent, aType );
    }




// ---------------------------------------------------------------------------
// COcrExampleSettingsContainer::SizeChanged
// Set the size and position of component controls.
// ---------------------------------------------------------------------------
//
void COcrExampleSettingsContainer::SizeChanged()
    {
    if (iSettingsList)
	    {
        iSettingsList->SetRect(Rect());
	    }
    }
// End of File

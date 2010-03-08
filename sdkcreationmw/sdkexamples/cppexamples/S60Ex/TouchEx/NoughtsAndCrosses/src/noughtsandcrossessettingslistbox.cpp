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



// INCLUDE FILES
#include "noughtsandcrossesinformationandsettings.h"
#include "noughtsandcrossessettingslistbox.h"
#include "nac.hrh"


// ============================ MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CNoughtsAndCrossesSettingListbox::CNoughtsAndCrossesSettingListbox()
// Default constructor.
// ----------------------------------------------------------------------------
//
CNoughtsAndCrossesSettingListbox::CNoughtsAndCrossesSettingListbox(
    CNoughtsAndCrossesInformationAndSettings& aSettings) : iSettings (aSettings)
    {
    }

// ----------------------------------------------------------------------------
// CNoughtsAndCrossesSettingListbox::CreateSettingItemL()
// Framework method to create a setting item based upon the user id aSettingId.
// ----------------------------------------------------------------------------
//
CAknSettingItem* CNoughtsAndCrossesSettingListbox::CreateSettingItemL(TInt aIdentifier)
    {
    switch (aIdentifier)
        {
        case ENoughtsAndCrossesPlayerNameText:
            return new (ELeave) CAknTextSettingItem(
                aIdentifier, iSettings.PlayerName());

        case ENoughtsAndCrossesPlayerFirstBinary:
            return new (ELeave) CAknBinaryPopupSettingItem(
                aIdentifier, iSettings.HumanPlayFirst());

        case ENoughtsAndCrossesPlayerTypeBinary:
            return new (ELeave) CAknBinaryPopupSettingItem(
                aIdentifier, iSettings.HumanTypeCross());

    default:
        return NULL;
        }
    }

// ---------------------------------------------------------
// CNoughtsAndCrossesSettingListbox::HandleResourceChange()
// Called by framework when layout is changed.
// ---------------------------------------------------------
//
void CNoughtsAndCrossesSettingListbox::HandleResourceChange(TInt aType)
    {
    CCoeControl::HandleResourceChange(aType);

    // ADDED FOR SCALABLE UI SUPPORT
    // *****************************
    if ( aType==KEikDynamicLayoutVariantSwitch )
        {
        TRect rect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
        SetRect(rect);
        }
    }

// ---------------------------------------------------------
// CNoughtsAndCrossesSettingListbox::SizeChanged()
// Called by framework when the view size is changed.
// ---------------------------------------------------------
//
void CNoughtsAndCrossesSettingListbox::SizeChanged()
    {
    if (ListBox()) 
        {
        ListBox()->SetRect(Rect());
        }
    }
// End of File


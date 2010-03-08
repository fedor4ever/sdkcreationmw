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


// INCLUDE FILES
#include "DynamicSettingListSlider.h"
#include <aknslidersettingpage.h>

// ================= MEMBER FUNCTIONS =======================

// Constructor
CDynamicSettingListSlider::CDynamicSettingListSlider( TInt aIdentifier, TInt& aSliderValue )
	: CAknSliderSettingItem(aIdentifier, aSliderValue)
	{

	}

// ----------------------------------------------------
// CDynamicSettingListAppView::CreateAndExecuteSettingPageL()
// Creates and executes a slider setting page.
// ----------------------------------------------------
//
void CDynamicSettingListSlider::CreateAndExecuteSettingPageL()
	{
	CAknSettingPage* dlg = CreateSettingPageL();

	SetSettingPage( dlg );
	SettingPage()->SetSettingPageObserver(this);

	// This has to be called so that slider setting page will work correctly in
	// all environments (WINS UDEB, GCCE UREL, etc):
    // CAknSliderSettingItem has a  fault that occurs at least in S60 2nd
    // Edition emulator debug builds and in 3rd Edition gcce builds. This fault
    // prevents creating CAknSliderSettingItem dynamically. A workaround is
    // to create a proprietary slider setting item and inherit it from
    // CAknSliderSettingItem (see CDynamicSettingListSlider.h). Then override
    // CAknSliderSettingItem::CreateAndExecuteSettingPageL
    // and use the solution pointed out in
    // CDynamicSettingListSlider::CreateAndExecuteSettingPageL.

	SettingPage()->SetSettingTextL(SettingName());

  	SettingPage()->ExecuteLD(CAknSettingPage::EUpdateWhenChanged);
	SetSettingPage(0);
	}

// End of file

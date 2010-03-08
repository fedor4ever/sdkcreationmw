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

#include <eikmenup.h>
#include <OcrExample.rsg>

#include "OcrExample.hrh"
#include "OcrExample.pan"
#include "OcrExampleAppui.h"
#include "OcrExampleImageView.h"
#include "OcrExampleSettingsContainer.h"
#include "OcrExampleUids.h"

// ================= MEMBER FUNCTIONS ==========================================

// -----------------------------------------------------------------------------
// COcrExampleAppUi::ConstructL()
// Symbian two-phased constructor
// -----------------------------------------------------------------------------
//
void COcrExampleAppUi::ConstructL()
    {
    BaseConstructL(EAknEnableSkin);

    iImageView = COcrExampleImageView::NewL(iSettingsData);
    AddViewL(iImageView);

    iSettingsView =
        NOCRUtils::CSimpleView< COcrExampleSettingsContainer, TOcrExampleSettingsData >::NewL(
                R_OCR_EXAMPLE_SETTINGS_VIEW,
                KUidSettingsView,
                iSettingsData);
    AddViewL(iSettingsView);

    iTextView =
        NOCRUtils::CSimpleView< COcrExampleTextContainer, TTextContainerData >::NewL(
                R_OCR_EXAMPLE_TEXT_VIEW,
                KUidTextView,
                iTextData);
    AddViewL(iTextView);

    //set the main app view as the default view
    SetDefaultViewL(*iImageView);
    }

// -----------------------------------------------------------------------------
// COcrExampleAppUi::~COcrExampleAppUi()
// Destructor
// Frees reserved resources
// -----------------------------------------------------------------------------
//
COcrExampleAppUi::~COcrExampleAppUi()
    {
    iSettingsView = NULL;
    iImageView = NULL;
    iTextView = NULL;
    }

// -----------------------------------------------------------------------------
// COcrExampleAppUi::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// -----------------------------------------------------------------------------
//
void COcrExampleAppUi::DynInitMenuPaneL(TInt /*aResource*/, CEikMenuPane* /*aMenuPane*/)
    {
    }

// -----------------------------------------------------------------------------
// COcrExampleAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
// takes care of key event handling
// -----------------------------------------------------------------------------
//
TKeyResponse COcrExampleAppUi::HandleKeyEventL( const TKeyEvent&, TEventCode )
    {
    return EKeyWasNotConsumed;
    }

// -----------------------------------------------------------------------------
// COcrExampleAppUi::HandleCommandL(TInt aCommand)
// takes care of command handling
// -----------------------------------------------------------------------------
//
void COcrExampleAppUi::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EAknSoftkeyExit:
        case EEikCmdExit:
            Exit();
            break;
        }
    }


// -----------------------------------------------------------------------------
// COcrExampleAppUi::HandleResourceChangeL( TInt aType )
// takes care of command handling
// -----------------------------------------------------------------------------
//
void COcrExampleAppUi::HandleResourceChangeL( TInt aType )
    {
    // base-class call also
    CAknViewAppUi::HandleResourceChangeL(aType);

    if (aType == KEikDynamicLayoutVariantSwitch)
        {
        iImageView->HandleSizeChange();
        iSettingsView->HandleSizeChange();
        iTextView->HandleSizeChange();
        }
    }

// End of File

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


// INCLUDE FILES
#include "SlideshowAppUi.h"
#include "SlideshowContainer.h"
#include <Slideshow.rsg>
#include "slideshow.hrh"

#include <avkon.hrh>

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CSlideshowAppUi::ConstructL()
// ----------------------------------------------------------
//
void CSlideshowAppUi::ConstructL()
    {
    BaseConstructL();
    iAppContainer = new (ELeave) CSlideshowContainer;
    iAppContainer->SetMopParent(this);
    iAppContainer->ConstructL( ClientRect(), this );
    AddToStackL( iAppContainer );
    }

// ----------------------------------------------------
// CSlideshowAppUi::~CSlideshowAppUi()
// Destructor. Frees any reserved resources.
// ----------------------------------------------------
//
CSlideshowAppUi::~CSlideshowAppUi()
	{
    if ( iAppContainer )
		{
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
		}
	}

// ------------------------------------------------------------------------------
// CSlideshowAppUi::::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CSlideshowAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {
    }

// ----------------------------------------------------
// CSlideshowAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
// ----------------------------------------------------
//
TKeyResponse CSlideshowAppUi::HandleKeyEventL(
    const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    if (aType == EEventKey)
        {
        switch (aKeyEvent.iScanCode)
            {
            case EStdKeyLeftArrow:
                iAppContainer->iSlideshow->ShowPreviousPicture();
                break;
            case EStdKeyRightArrow:
                iAppContainer->iSlideshow->ShowNextPicture();
                break;
            }
        }
    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------
// CSlideshowAppUi::HandleCommandL(TInt aCommand)
// ----------------------------------------------------
//
void CSlideshowAppUi::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
        case EEikCmdExit:
            {
            Exit();
            break;
            }
        default:
            break;
        }
    }

// End of File

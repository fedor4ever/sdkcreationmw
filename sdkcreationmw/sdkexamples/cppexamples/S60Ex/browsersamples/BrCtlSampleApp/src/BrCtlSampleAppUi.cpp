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
* Description:  The Ui class for the Browser Control Sample Application
*
*/


// INCLUDE FILES
#include "BrCtlSampleAppUi.h"
#include "BrCtlSampleAppContainer.h" 
#include <BrCtlSampleApp.rsg>
#include "brctlsampleapp.hrh"

#include <avkon.hrh>


// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CBrCtlSampleAppUi::ConstructL()
// ?implementation_description
// ----------------------------------------------------------
//
void CBrCtlSampleAppUi::ConstructL()
    {
    BaseConstructL(CAknAppUi::EAknEnableSkin);
    iAppContainer = new (ELeave) CBrCtlSampleAppContainer;
    iAppContainer->SetMopParent(this);
    iAppContainer->ConstructL( ClientRect() );
    AddToStackL( iAppContainer );
    }

// ----------------------------------------------------
// CBrCtlSampleAppUi::~CBrCtlSampleAppUi()
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CBrCtlSampleAppUi::~CBrCtlSampleAppUi()
    {
    if (iAppContainer)
        {
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
        }

   }

// ------------------------------------------------------------------------------
// CBrCtlSampleAppUi::::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CBrCtlSampleAppUi::DynInitMenuPaneL(
    TInt aResourceId,CEikMenuPane* aMenuPane)
    {
    iAppContainer->DynInitMenuPaneL(aResourceId, aMenuPane);
    }

// ----------------------------------------------------
// CBrCtlSampleAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
// ?implementation_description
// ----------------------------------------------------
//
TKeyResponse CBrCtlSampleAppUi::HandleKeyEventL(
    const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    return iAppContainer->HandleKeyEventL(aKeyEvent, aType);;
    }

// ----------------------------------------------------
// CBrCtlSampleAppUi::HandleCommandL(TInt aCommand)
// ?implementation_description
// ----------------------------------------------------
//
void CBrCtlSampleAppUi::HandleCommandL(TInt aCommand)
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
            iAppContainer->HandleCommandL(aCommand);
            break;      
        }
    }

// End of File  


/*
* Copyright (c) 2006 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
#include <avkon.hrh>
#include <eikmenup.h>

#include "AudioStreamAppUi.h"
#include "AudioStreamView.h" 
#include <AudioStream.rsg>
#include "AudioStream.hrh"

#include "AudioStreamEngine.h"


// ----------------------------------------------------------------------------
// CAudioStreamAppUi::ConstructL()
//
// standard EPOC 2nd phase constructor
// ----------------------------------------------------------------------------
void CAudioStreamAppUi::ConstructL()
    {
	BaseConstructL(EAknEnableSkin);
	iEngine = CAudioStreamEngine::NewL( this );	
	// pass the handle of CAudioStreamEngine to CAudioStreamView so the 
	// view can use the engine
	iView = CAudioStreamView::NewL(ClientRect(), iEngine);
	iView->SetMopParent( this );
	// add view to control stack
	AddToStackL( iView );	
    }

// ----------------------------------------------------------------------------
// CAudioStreamAppUi::~CAudioStreamAppUi()
//
// destructor
// ----------------------------------------------------------------------------
CAudioStreamAppUi::~CAudioStreamAppUi()
	{
    // remove view from control stack
    RemoveFromStack( iView );
    delete iView;
	iView=NULL;
	
	delete iEngine;
	iEngine=NULL;
	}

// ----------------------------------------------------------------------------
// CAudioStreamAppUi::DynInitMenuPaneL(TInt aResourceId,
//	   CEikMenuPane* aMenuPane)
//
// this function is called by the EIKON framework just before it displays
// a menu pane. Its default implementation is empty, and by overriding it,
// the application can set the state of menu items dynamically according
// to the state of application data.
// ----------------------------------------------------------------------------
void CAudioStreamAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/, CEikMenuPane* /*aMenuPane*/)
    {
    }

// ----------------------------------------------------------------------------
// CAudioStreamAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
//
// takes care of key event handling
// ----------------------------------------------------------------------------
TKeyResponse CAudioStreamAppUi::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/,TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------------------------------
// CAudioStreamAppUi::HandleCommandL(TInt aCommand)
//
// takes care of command handling
// ----------------------------------------------------------------------------
void CAudioStreamAppUi::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        case EAknSoftkeyBack:
        case EEikCmdExit:
            {
            Exit();
            break;
            }
        case EAudioStreamCmdPlay:
            {
            iEngine->Play();
            break;
            }
        case EAudioStreamCmdRecord:
            {
            iEngine->Record();
            break;
            }
        case EAudioStreamCmdStop:
            {
            iEngine->Stop();
            break;
            }
        case EAudioStreamCmdLoad:
            {
            iEngine->LoadAudioFileL();
            break;
            }
        case EAudioStreamCmdSave:
            {
            iEngine->SaveAudioFileL();
            break;
            }
        case EAudioStreamCmdPcm:
            {
            iEngine->SetEncodingL(EFalse);
            break;
            }
        case EAudioStreamCmdAmr:
            {
            iEngine->SetEncodingL(ETrue);
            break;
            }
        default:
            break;      
        }
    }


// ----------------------------------------------------------------------------
// CAudioStreamView* CAudioStreamAppUi::GetView()
//
// returns a reference to application view
// ----------------------------------------------------------------------------
CAudioStreamView* CAudioStreamAppUi::GetView() const
{
	return iView;
}


// End of File  


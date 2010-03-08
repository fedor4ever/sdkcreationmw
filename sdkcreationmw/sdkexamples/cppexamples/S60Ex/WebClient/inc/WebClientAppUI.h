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


#ifndef WEBCLIENTAPPUI_H
#define WEBCLIENTAPPUI_H

// INCLUDES
#include <aknappui.h>
#include <aknprogressdialog.h>

// CONSTANTS
// Schemes for given uris
_LIT(KHttpPrefix, "http://");
_LIT8(KHttpPrefix8, "http://");

// HTTPS schemes
_LIT(KHttpsPrefix, "https://");
_LIT8(KHttpsPrefix8, "https://");

// FORWARD DECLARATIONS
class CWebClientContainer;
class CWebClientEngine;
class CAknWaitDialog;

// CLASS DECLARATION

/**
* Application UI class.
* Provides support for the following features:
* - EIKON control architecture
*/
class CWebClientAppUi : public CAknAppUi,
                        public MProgressDialogCallback
    {
    public:
        /**
        * ConstructL()
        * Perform the second phase construction of a CWebClientAppUi object.
        */
        void ConstructL();

        /**
        * CWebClientAppUi()
        * First phase construction of CWebClientAppUi.
        */
        CWebClientAppUi();

        /**
        * ~CWebClientAppUi()
        * Destructor for CWebClientAppUi.
        */
        ~CWebClientAppUi();

        /**
        * RemoveWaiDialogL()
        * Called by CWebClientEngine when Http transaction is finished.
        */
        void RemoveWaitDialogL();

    private:
        /**
        * From CEikAppUi
        * HandleCommandL()
        * Handles user commands.
        * @param aCommand: Command ID.
        */
        void HandleCommandL( TInt aCommand );

        /**
        *  HandleUrlRequestL()
        *  Handles user command Request URL.
        *  Called from HandleCommandL()
        */
		void HandleUrlRequestL();

        /**
        * HandleResourceChangeL()
        * Called by framework when layout is changed.
        */
        virtual void HandleResourceChangeL(TInt aType);

        /**
        *  HandleStatusPaneSizeChange()
        *  Called by the framework when the application status pane
        *  size is changed.
        */
        void HandleStatusPaneSizeChange();

        /**
        * From CEikAppUi, handles key events.
        * @param aKeyEvent Event to handled.
        * @param aType Type of the key event. 
        * @return Response code (EKeyWasConsumed, EKeyWasNotConsumed). 
        */
        virtual TKeyResponse HandleKeyEventL(
            const TKeyEvent& aKeyEvent,TEventCode aType );

        /**
        * From MProgressDialogCallBack
        * Is called when a dialog is dismissed.
        * @param aButtonId: ButtunId when a dialog is dismissed.
        */
        void DialogDismissedL( TInt aButtonId );

    private: // Data
        CWebClientContainer*    iContainer;
        CWebClientEngine*       iClient;
        CAknWaitDialog*         iWaitDialog;
        };

#endif // WEBCLIENTAPPUI_H


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
* Description:  Declares CommonDialogsEx appui.
*
*/


#ifndef CCOMMONDIALOGSEXAPPUI_H
#define CCOMMONDIALOGSEXAPPUI_H

// INCLUDES
#include <aknappui.h>
#include <CAknMemorySelectionDialog.h>

// FORWARD DECLARATIONS
class CCommonDialogsExContainer;

// CONSTANTS

// CLASS DECLARATION

/**
* Application UI class.
* Provides support for the following features:
* - EIKON control architecture
* 
*/
class CCommonDialogsExAppUi : public CAknAppUi
    {
    public: // // Constructors and destructor

        /**
        * EPOC default constructor.
        */      
        void ConstructL();

        /**
        * Destructor.
        */      
        ~CCommonDialogsExAppUi();
        
    private:
        // From MEikMenuObserver
        void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);

    private:
        /**
        * From CEikAppUi, takes care of command handling.
        * @param aCommand command to be handled
        */
        void HandleCommandL(TInt aCommand);

        /**
        * From CEikAppUi, handles key events.
        * @param aKeyEvent Event to handled.
        * @param aType Type of the key event. 
        * @return Reponse code (EKeyWasConsumed, EKeyWasNotConsumed). 
        */
        virtual TKeyResponse HandleKeyEventL(
            const TKeyEvent& aKeyEvent,TEventCode aType);
        
    private: //Data
        // This is NOT iAppView since this is REALLY a container ptr.
        CCommonDialogsExContainer* iAppContainer; 

		CAknMemorySelectionDialog::TMemory iMemory;
		     
        TDriveNumber iDrive;
    };

#endif

// End of File

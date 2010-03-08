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


#ifndef SATELLITE_REFERENCE_APPUI_H
#define SATELLITE_REFERENCE_APPUI_H

// INCLUDES
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknappui.h>
#include <aknapp.h>
#include <CSatelliteInfoUI.h>

// FORWARD DECLARATIONS
class CSatelliteReferenceContainer;
class CSatelliteInfoUI;


// CLASS DECLARATION

/**
* Application UI class.
* Provides support for the following features:
* - EIKON control architecture
*/
class CSatelliteReferenceAppUi : public CAknAppUi
    {
    public: // // Constructors and destructor

        void ConstructL();

        /**
        * Destructor.
        */      
        ~CSatelliteReferenceAppUi();
        
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
        * @return Response code (EKeyWasConsumed, EKeyWasNotConsumed). 
        */
        virtual TKeyResponse HandleKeyEventL(
            const TKeyEvent& aKeyEvent,TEventCode aType);
            
        /**
        * From CEikAppUi, handles the event when
        * the application is switch between foreground and
        * background
        */
        void HandleForegroundEventL(TBool aForeground);

    private: //Data
        CSatelliteReferenceContainer* iAppContainer; 
        
        /// Pointer to CSatelliteUI
        CSatelliteInfoUI* iSatelliteInfo;
    };

#endif

// End of File

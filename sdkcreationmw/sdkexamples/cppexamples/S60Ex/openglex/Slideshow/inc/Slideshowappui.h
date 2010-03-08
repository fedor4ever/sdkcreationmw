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


#ifndef SLIDESHOWAPPUI_H
#define SLIDESHOWAPPUI_H

// INCLUDES
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknappui.h>

// FORWARD DECLARATIONS
class CSlideshowContainer;

// CLASS DECLARATION

/**
 * Application UI class that contains the CSlideshowContainer
 * (as required by the Symbian UI application architecture).
 */
class CSlideshowAppUi : public CAknAppUi
    {
    public: // Constructors and destructor

        /**
         * Second phase constructor that can call methods that may leave.
         * Creates the CSlideshowContainer and adds it to the control stack.
         */
        void ConstructL();

        /**
         * Destructor. Removes CSlideshowContainer from the control
         * stack and destroys it.
         */
        virtual ~CSlideshowAppUi();

    private: // Functions from base classes

        /**
         * This method is called by the EIKON framework just before it displays
         * a menu pane. Builds the dynamic menuitems for toggling the various options
         * in the example.
         */
        void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);

        /**
         * CEikAppUi method that is used to handle user commands.
         * Handles menu commands and application exit request.
         * @param aCommand Command to be handled.
         */
        void HandleCommandL(TInt aCommand);

        /**
         * CEikAppUi method that is used to handle key events.
         * Does nothing in this implemenation.
         * @param aKeyEvent Event to handled (ignored by this implementation).
         * @param aType Type of the key event (ignored by this implementation).
         * @return Response code. Always EKeyWasNotConsumed in this implementation.
         */
        virtual TKeyResponse HandleKeyEventL(
            const TKeyEvent& aKeyEvent,TEventCode aType);

    private: //Data

        /** GUI container that resides in this application UI. */
        CSlideshowContainer* iAppContainer;
    };

#endif

// End of File

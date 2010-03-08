/*
* Copyright (c) 2004 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CCONTAPPUI_H__
#define __CCONTAPPUI_H__

// INCLUDES
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknappui.h>

// FORWARD DECLARATIONS
class CContContainer;

// CLASS DECLARATION

/**
* Application UI class.
* Provides support for the following features:
* - EIKON control architecture
*
*/
class CContAppUi : public CAknAppUi
    {
    public: // // Constructors and destructor

        /**
        * Default constructor
        */
        void ConstructL();

        /**
        * Destructor
        * Destructor is always virtual
        */
        virtual ~CContAppUi();

    private:
        /**
        * From MEikMenuObserver
        * @param aResourceId Resource ID identifying the menu pane to initialise
        * @param aMenuPane The in-memory representation of the menu pane.
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    private:
        /**
        * From CEikAppUi, takes care of command handling
        * @param aCommand command to be handled
        */
        void HandleCommandL( TInt aCommand );
        
        /**
        * From CEikAppUi, HandleResourceChangeL.
        * Handles resource changes.
        * @param aType Resource change type to be handled
        */
        void HandleResourceChangeL( TInt aType );

        /**
        * From CEikAppUi, handles key events
        * @param aKeyEvent Event to handled.
        * @param aType Type of the key event
        * @return Response code (EKeyWasConsumed, EKeyWasNotConsumed)
        */
        virtual TKeyResponse HandleKeyEventL(
            const TKeyEvent& aKeyEvent, TEventCode aType);

    private: // local methods
        /**
        * UpdateTitleL
        * The method updates title text
        * @param aResourceId Resource ID for text
        */
        void UpdateTitleL( TInt aResourceId );

    private: //Data
        /**
        * iAppContainer the container class
        * Owned by CContAppUi object, instantiated in
        * ConstructL, deleted in destructor
        */
        CContContainer* iAppContainer;
    };

#endif  // __CCONTAPPUI_H__

// End of File

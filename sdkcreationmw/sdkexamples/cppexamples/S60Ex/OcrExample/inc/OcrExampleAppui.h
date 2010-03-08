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


#ifndef OCREXAMPLEAPPUI_H
#define OCREXAMPLEAPPUI_H

// INCLUDES
#include <aknviewappui.h>
#include "SimpleView.h"
#include "OcrExampleTextContainer.h" // COcrExampleTextContainer & TTextContainerData
#include "OcrExampleSettingsdata.h"

// FORWARD DECLARATIONS
class COcrExampleImageView;
class COcrExampleSettingsView;
class COcrExampleSettingsContainer;

/**
 * Application UI class.
 * This is the main application UI for the OCR Example.
 * It manages the three different views in the app:
 * - the image-view (i.e. the main-view),
 * - settings view, and
 * - text view displaying the analyze and recognition results
 */
class COcrExampleAppUi : public CAknViewAppUi
    {
    public: // Constructors and destructor

        /** Symbian default constructor. */
        void ConstructL();

        /** Destructor. */
        ~COcrExampleAppUi();

    public: // New functions

    public: // Functions from base classes

        /**
        * HandleResourceChangeL()
        * Called by framework when layout is changed.
        * @param aType the type of resources that have changed
        */
        void HandleResourceChangeL( TInt aType );

    private:
       /**
        * From MEikMenuObserver
        * COcrExampleAppUi::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
        * This function is called by the EIKON framework just before it displays
        * a menu pane. Its default implementation is empty, and by overriding it,
        * the application can set the state of menu items dynamically according
        * to the state of application data.
        */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    private:
        /**
        * From CEikAppUi, takes care of command handling.
        * @param aCommand command to be handled
        */
        void HandleCommandL( TInt aCommand );

        /**
        * From CEikAppUi, handles key events.
        * @param aKeyEvent Event to handled.
        * @param aType Type of the key event.
        * @return Response code (EKeyWasConsumed, EKeyWasNotConsumed).
        */
        virtual TKeyResponse HandleKeyEventL( const TKeyEvent& aKeyEvent,
                                              TEventCode aType );

    private: //Data

        /** data-object for text-view */
        TTextContainerData          iTextData;

        /** data-object for settings- and image-view  */
        TOcrExampleSettingsData     iSettingsData;

        /** image view */
        COcrExampleImageView        *iImageView;

        /** settings view */
        NOCRUtils::CSimpleView<COcrExampleSettingsContainer,
                              TOcrExampleSettingsData>      *iSettingsView;
        /** text view */
        NOCRUtils::CSimpleView<COcrExampleTextContainer,
                              TTextContainerData>           *iTextView;
    };

#endif

// End of File

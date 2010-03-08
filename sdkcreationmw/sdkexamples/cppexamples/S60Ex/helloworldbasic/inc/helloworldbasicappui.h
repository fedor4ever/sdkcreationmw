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


#ifndef __HELLOWORLDBASICAPPUI_H__
#define __HELLOWORLDBASICAPPUI_H__

// INCLUDES
#include <aknappui.h>

// FORWARD DECLARATIONS
class CHelloWorldBasicAppView;


// CLASS DECLARATION
/**
* CHelloWorldBasicAppUi application UI class.
* Interacts with the user through the UI and request message processing
* from the handler class
*/
class CHelloWorldBasicAppUi : public CAknAppUi
    {
    public: // Constructors and destructor

        /**
        * ConstructL.
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * CHelloWorldBasicAppUi.
        * C++ default constructor. This needs to be public due to
        * the way the framework constructs the AppUi
        */
        CHelloWorldBasicAppUi();

        /**
        * ~CHelloWorldBasicAppUi.
        * Virtual Destructor.
        */
        virtual ~CHelloWorldBasicAppUi();

    private:  // Functions from base classes

        /**
        * From CEikAppUi, HandleCommandL.
        * Takes care of command handling.
        * @param aCommand Command to be handled.
        */
        void HandleCommandL( TInt aCommand );

        /**
        * HandleResourceChangeL()
        * Called by framework when layout is changed.
        * @param aType the type of resources that have changed
        */
        void HandleResourceChangeL( TInt aType );

    private: // Data

        /**
        * The application view
        * Owned by CHelloWorldBasicAppUi
        */
        CHelloWorldBasicAppView* iAppView;

    };

#endif // __HELLOWORLDBASICAPPUI_H__

// End of File


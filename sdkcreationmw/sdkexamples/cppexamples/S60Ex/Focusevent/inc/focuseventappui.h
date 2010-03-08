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


#ifndef __FOCUSEVENT_APPUI_H__
#define __FOCUSEVENT_APPUI_H__

// INCLUDES
#include <aknappui.h>

// FORWARD DECLARATIONS
class CFocusEventAppView;

// CLASS DECLARATION
/**
* CFocusEventAppUi application UI class.
* Interacts with the user through the UI and request message processing
* from the handler class
*/
class CFocusEventAppUi : public CAknAppUi
    {
    public: // Constructors and destructors

        /**
        * CFocusEventAppUi.
        * C++ default constructor. This needs to be public due to
        * the way the framework constructs the AppUi
        */
        CFocusEventAppUi();

        /**
        * ~CFocusEventAppUi.
        * Virtual Destructor.
        */
        virtual ~CFocusEventAppUi();

    public:  // Functions from base classes

        /**
        * From CAknAppUi, HandleForegroundEventL.
        * Handles a change of focus.
        * @param aForeground is ETrue is this application has focus.
        */
        void HandleForegroundEventL( TBool aForeground );

        /**
        * From CEikAppUi, HandleCommandL.
        * Takes care of command handling.
        * @param aCommand Command to be handled.
        */
        void HandleCommandL( TInt aCommand );

    private: // Constructors

        /**
        * ConstructL.
        * 2nd phase constructor.
        */
        void ConstructL();

    private: // Data

        /**
        * The application view
        * Owned by CFocusEventAppUi
        */
        CFocusEventAppView* iAppView;
    };

#endif // __FOCUSEVENT_APPUI_H__

// End of File

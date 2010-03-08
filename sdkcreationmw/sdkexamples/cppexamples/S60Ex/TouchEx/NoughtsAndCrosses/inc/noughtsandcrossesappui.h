/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef __NOUGHTSANDCROSSESAPPUI_H__
#define __NOUGHTSANDCROSSESAPPUI_H__

// INCLUDES
#include <aknviewappui.h>

// FORWARD DECLARATIONS
class CNoughtsAndCrossesView;
class CNoughtsAndCrossesSettingsView;


// CLASS DECLARATION
/**
* CNoughtsAndCrossesAppUi application UI class.
* Interacts with the user through the UI and request message processing
* from the handler class
*/
class CNoughtsAndCrossesAppUi : public CAknViewAppUi
    {
    public: // Constructors and destructor
    
        /**
        * ConstructL.
        * 2nd phase constructor.
        */
        void ConstructL();
        
    public: // Functions from base classes

        /**
        * From CEikAppUi, HandleCommandL.
        * Takes care of command handling.
        * @param aCommand Command to be handled.
        */
        virtual void HandleCommandL(TInt aCommand);

    private: // Data

        /**
        * iGameView, The application game view
        * Not owned by CNoughtsAndCrossesAppUi object.
        */
        CNoughtsAndCrossesView* iGameView;

        /**
        * iSettingsView, The application settings view
        * Not owned by CNoughtsAndCrossesAppUi object.
        */
        CNoughtsAndCrossesSettingsView* iSettingsView;
    };

#endif // __NOUGHTSANDCROSSESAPPUI_H__

// End of File

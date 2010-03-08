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
* Description:  This class functions as controller of the application
*
*/


#ifndef LBSREFERENCEAPPUI_H
#define LBSREFERENCEAPPUI_H

// INCLUDES
#include <aknappui.h>

// FORWARD DECLARATIONS
class CLbsReferenceContainer;

/**
* Application UI class.
* Provides support for the following features:
* - UIKON control architecture
*
*/
class CLbsReferenceAppUi :  public CAknAppUi
    {
    public: // Constructors and destructor

        /**
        * Symbian OS default constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        ~CLbsReferenceAppUi();

    private: // Functions from base classes
        
        // -----------------------------------------------------------------------------
        // From CEikAppUi, handles key events.
        // @param aKeyEvent Event to handled.
        // @param aType Type of the key event.
        // @return Response code (EKeyWasConsumed, EKeyWasNotConsumed).
        // -----------------------------------------------------------------------------
        //
        virtual TKeyResponse HandleKeyEventL(
            const TKeyEvent& aKeyEvent,TEventCode aType);

        // -----------------------------------------------------------------------------
        // From CEikAppUi, takes care of command handling.
        // @param aCommand command to be handled
        // -----------------------------------------------------------------------------
        //
        void HandleCommandL(TInt aCommand);


    private: //Data

        CLbsReferenceContainer* iAppContainer;      // App container

      
    };

#endif //LBSREFERENCEAPPUI_H

// End of File

/*
* Copyright (c) 2006 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares UI class for AudioStream application
*
*/


#ifndef AUDIOSTREAMAPPUI_H
#define AUDIOSTREAMAPPUI_H

// INCLUDES
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknappui.h>

// FORWARD DECLARATIONS
class CAudioStreamEngine;
class CAudioStreamView;


// CLASS DECLARATION

/**
* CAudioStreamAppUi Application UI class.
* 
*/
class CAudioStreamAppUi : public CAknAppUi
    {
    public: // // Constructors and destructor

        /**
        * EPOC AudioStreamault constructor.
        */      
        void ConstructL();

        /**
        * Destructor.
        */      
        ~CAudioStreamAppUi();
        
    public: // New functions

		CAudioStreamView* GetView() const;

    public: // Functions from base classes

    private:
        // From MEikMenuObserver
        void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);

    private:
        /**
        * From CEikAppUi, takes care of command handling.
        * param aCommand command to be handled
        */
        void HandleCommandL(TInt aCommand);

        /**
        * From CEikAppUi, handles key events.
        * param aKeyEvent Event to handled.
        * param aType Type of the key event. 
        * return Response code (EKeyWasConsumed, EKeyWasNotConsumed). 
        */
        virtual TKeyResponse HandleKeyEventL(
            const TKeyEvent& aKeyEvent,TEventCode aType);

    private: // data
        CAudioStreamView* iView; 
		CAudioStreamEngine* iEngine;

    };

#endif

// End of File

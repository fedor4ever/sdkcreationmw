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


#ifndef FILELISTAPPUI_H
#define FILELISTAPPUI_H

// INCLUDES
#include <aknappui.h>

// FORWARD DECLARATIONS
class CFilelistContainer;


// CLASS DECLARATION

/**
* Application UI class.
* Provides support for the following features:
* - EIKON control architecture
* 
*/
class CFilelistAppUi : public CAknAppUi
    {
    public: // // Constructors and destructor

        /**
        * Symbian default constructor.
        */      
        void ConstructL();

        /**
        * Destructor.
        */      
        ~CFilelistAppUi();
        
    public: // New functions

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
        * From CEikAppUi, handles key events.
        * @param aType The type of resources that have changed
        */    
        virtual void HandleResourceChangeL( TInt aType );            

    private: //Data
        CFilelistContainer* iAppContainer; 
    };

#endif

// End of File

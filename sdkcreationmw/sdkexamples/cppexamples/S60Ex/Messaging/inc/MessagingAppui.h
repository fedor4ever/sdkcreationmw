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


#ifndef MESSAGINGAPPUI_H
#define MESSAGINGAPPUI_H

// INCLUDES
#include <aknappui.h>

// FORWARD DECLARATIONS
class CMessagingContainer;
class CSendUi;


// CLASS DECLARATION

/**
* Application UI class.
* Provides support for the following features:
* - EIKON control architecture
* 
*/
class CMessagingAppUi : public CAknAppUi
    {
    public: // Constructors and destructor
        /**
        * Symbian default constructor.
        */      
        void ConstructL();

        /**
        * Destructor.
        */      
        ~CMessagingAppUi();
        
    public: // New functions
    
    public: // Functions from base classes
    
    	/**
        * HandleResourceChangeL()
        * Called by framework when layout is changed.
        * @param aType the type of resources that have changed
        */
		void HandleResourceChangeL( TInt aType );

    private:
        // From MEikMenuObserver
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
        
        /**
        * CreateAndSendMessageL()
        * Launches a message editor using CSendUi 
        * @param aServiceUid the type of message that will be created
        */
		void CreateAndSendMessageL( const TUid aServiceUid );
				
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
    
    	/**
    	* Container object
    	*/
       CMessagingContainer* iAppContainer; 
       	
       	/**
    	* SendUI object
    	*/
       CSendUi* iSendUi;
       
    };

#endif

// End of File

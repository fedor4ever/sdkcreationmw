/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Application UI class, CAddressBookAppUi - 
*				 responsible for handling user commands
*
*/


#ifndef ADDRESSBOOKAPPUI_H
#define ADDRESSBOOKAPPUI_H

// INCLUDES
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknappui.h>
#include <aknwaitnotewrapper.h>
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include "MAddressBookEngineObserver.h"

// FORWARD DECLARATIONS
class CAddressBookView;
class CAddressBookContact;
class CAddressBookEngine;

// CLASS DECLARATION

/**
* Application UI class.
* Provides support for the following features:
* - EIKON control architecture
*/
class CAddressBookAppUi : public CAknAppUi, public MAknBackgroundProcess,
						  public MAddressBookEngineObserver
    {
    public: // Constructors and destructor

        /**
        * Destructor.
        */      
        ~CAddressBookAppUi();
        
        // Functions from base classes
    
    	/**
    	* From MAddressBookEngineObserver, checks errors that can happen
    	* @param aCode Code of Error
    	*/
		virtual void ErrorL(TInt aCode);
		
		/**
    	* From MAddressBookEngineObserver, is called 
    	* by the CAddressBookEngine::SetStatus(const TInt aStatus)
		* to set conection's status as ready
    	*/
		virtual void ConnectionReady();
		
		/**
    	* From MAddressBookEngineObserver, updates bufer 
    	* containing the contact information and sets status as READY
    	* @param aContactArray - contact information that was found
    	*/
		virtual void SearchFinishedL(RPointerArray<CAddressBookContact>& aContactArray);

    private: // Constructors
    
    	/**
        * By default Symbian 2nd phase constructor is private.
        */      
        void ConstructL();
		
     	// New functions
    
        /**
        * Updates field of CPbkContactItem when user select 
        * the point of menu "Add to contacts". The change doesn't reflect to database.
        * @param modifiableItem - Item that need to modify
        * @param fieldId - Field of modifiableItem
        * @param newValue - Value that will be added instead an old one
		*/
		void UpdateFieldL(CPbkContactItem* aModifiableItem,
					TPbkFieldId aFieldId, const TDesC8& aNewValue);	
    
    	// Functions from base classes 
    	
        /** From MEikMenuObserver, is called by EIKON framework just before 
        * it displays a menu pane, 
        * sets the state of menu items dynamically according
		* to the state of application data.
        * @param aResourceId specifies dialog-resource ID.
        * @param aMenuPane specifies menu pane
        */
        void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);

		/** From MAknBackgroundProcess,
		* is called by the active scheduler to dismiss the dialog
		*@param aButtonId specifies button that was pressed
		*/
		void DialogDismissedL(TInt aButtonId);
		
		/** From MAknBackgroundProcess, 
		* is called by the active scheduler to check if is process already done 
		*/
		TBool IsProcessDone() const;
		
		/** From MAknBackgroundProcess
		* is called by the active scheduler to finish the process 
		*/
		void ProcessFinished();
		
		/** From MAknBackgroundProcess,
		* and is called by the active scheduler to do one step of processing
 		* Purpose this function in AddressBook to run AddressBookEngine::ConnectL() 
        * if connection is not established yet
		*/
		void StepL();

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
            
	private: //Data
        CAddressBookView* iAppContainer; 
		CAddressBookEngine* iEngine;
		CPbkContactEngine* iContactEngine;
		CAknWaitNoteWrapper* iWaitNote;
		TBool iReady;
		TBool iFirstTime;
		TBool iConnecting;
		TBool iAbout;
    };

#endif  // ADDRESSBOOKAPPUI_H

// End of File


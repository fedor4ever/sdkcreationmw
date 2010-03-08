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


#ifndef IMOPENAPIEXAMPLEAPPUI_H
#define IMOPENAPIEXAMPLEAPPUI_H

// INCLUDES
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknviewappui.h>
#include <AgentDialog.h>

#include "IMOpenAPIExamplemsg.h"
#include "IMOpenAPIExample.hrh"


// FORWARD DECLARATIONS
class CIMOpenAPIExampleContainer;
class CIMOpenAPIExampleDocument;
class CAknWaitDialog;

// CLASS DECLARATION

/**
* Application UI class.
* Provides support for the following features:
* - EIKON control architecture
* 
*/
class CIMOpenAPIExampleAppUi : public CAknViewAppUi
    {
	public: // Constructor and Destructor

        /**
        * EPOC default constructor.
        */      
        void ConstructL();

        /**
        * Destructor.
        */      
        ~CIMOpenAPIExampleAppUi();
        
	public: // Own functions

		/**
        * SetGameState.
        * Set the UI according to current game state
        * @param aState current game state
        */
		void SetGameState(TGameState const aState);
		
		/**
        * GetGameState.
        * Get the current game state
        * @return Current game state
        */
		TGameState GetGameState() const;

		/**
        * HandleMessageErrorFromModel.
        * handle error messages from Model. Model calls this function
        * @param aIntegerInfo Error information as string
        * aStringInfo Error information in string
        */
		void HandleMessageErrorFromModel(	const TInt aIntegerInfo,
											const TDesC& aStringInfo );

		/**
        * HandleMessageFromModel.
        * handle messages from Model. Model calls this function
        * @param aMsgID internal message id, used inside application
        * aIntegerInfo information as string
        * aStringInfo Error information in string
        */
		void HandleMessageFromModel(const TInternalMsgID aMsgID, 
											const TInt aIntegerInfo,
											const TDesC& aStringInfo );
											
		/**
        * HandleChallengeRec.
        * handle challenge from Model. Model calls this function when some
        * challenge comes
        * @param aUserID reference to the challenger User Id
        */
        void HandleChallengeRec(const TDesC& aUserID );
        
		/**
        * HandleMessageIMConnectionFromModel.
        * handle connection related messages from Model. Model calls this 
        * function
        * @param aMsgID Internal message id, used inside application
        * aIntegerInfo Information as string
        */
		void HandleMessageIMConnectionFromModel(const TInternalMsgID aMsgID, 
											const TInt aIntegerInfo	);
											
		/**
        * DisplayInfoMessage.
        * Displays the information message to user with given resource id
        * @param aResourceID resource id of the message
        */
        void DisplayInfoMessage(TInt aResourceID);

		/**
        * PlayedIndex.
        * This functions gets the index position in the matrix where opponent
        * has marked cross or circle.
        * @param aIntegerInfo position in the 3*3 matrix, 0 to 8
        */
        void PlayedIndex(const TInt aIntegerInfo);
        

    private: // Functions from base classes
        /**
        * PlayedIndex.
        * From MEikMenuObserver
        */
        void DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane);

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
            
            
    private: // Own functions

		/**
        * Challenge.
        * Display a query dialog for User ID of the opponent to be challenged
        * @return ETrue if user accepts to send challenge
        */
        TBool Challenge();
        
		/**
        * LoginSettings.
        * Do the login settings. Displays query dialogs for user id, password
        * server address and access points and saves to internal variables
        */
        void LoginSettings();

		/**
        * Document.
        * Returns a reference to the application document
        * @return reference to CIMOpenAPIExampleDocument
        */
        CIMOpenAPIExampleDocument& Document() const;
        
        /**
        * SelectAccessPoint
        * Display a selection list of access points to user
        * and stores selected access point to iAPNumber
        * @return none
        */
        void SelectAccessPoint(); 

    private: // data
    
		/**
        * iAppContainer.
        * Pointer to application container
        */
        CIMOpenAPIExampleContainer* iAppContainer;
        
		/**
        * iWaitDialog.
        * Pointer to wait dialog
        */
		CAknWaitDialog*     iWaitDialog;
		
		/**
        * iDisplayMessage.
        * Buffer for holding message to be displayed to the user
        */
        TBuf<KMaxInfoMsgLength> iDisplayMessage;
        
		/**
        * iUserID.
        * Buffer for holding user id for login
        */
        TBuf<KMaxChallengeIDLength> 	iUserID;
        
		/**
        * iUserID.
        * Buffer for holding server address for login
        */
        TBuf<KMaxServerAddressLength> 	iServerAddress;
        
		/**
        * iPassword.
        * Buffer for holding password for login
        */
		TBuf<KMaxPasswordLength> 		iPassword;
		
		/**
        * iAPNumber.
        * Integer for holding access point for login
        */
		TUint32 				 		iAPNumber;
		
        /**
        * flag indicating, whether login settings are specified.
        */
        TBool                           iLoginSettingsDone;

		/**
		* iDlgSrv
		* Dialog server for access point selection
		*/
		RGenConAgentDialogServer iDlgSrv;
		
		/**
		* iStatus
		* Request status for requests made to RGenConAgentDialogServer
		*/	
		TRequestStatus iStatus;
    };

#endif //IMOPENAPIEXAMPLEAPPUI_H

// End of File


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


#ifndef __IMOPENAPIEXAMPLEMODEL_H__
#define __IMOPENAPIEXAMPLEMODEL_H__

// INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <imconnection.h>
#include <imclient.h>
#include "IMOpenAPIExampleDocument.h"
#include "IMOpenAPIExamplemsg.h"


// CONSTANTS
// Client ID of this messaging application
_LIT(KClientID,"IMOPENAPIEXAMPLE1");

// FORWARD DECLARATIONS
class CIMOpenAPIExampleDocument;
class CDesC16Array;
class TTimeIntervalMicroSeconds32;


// CLASS DECLARATION
/**
 * This class is like an engine to the application. It holds the connection
 * and communication to the IM server. It also holds most of the game logic and
 * game data. The protocol allows only one outstanding opponent.
 */
class CIMOpenAPIExamplemodel :	public CBase,
								public MImObserver,
								public MImConnectionObserver

    {
	public: // methods from base classes
	
        /**
        * HandleLoginL.
        * From MImConnectionObserver
        */
		void HandleLoginL(const TInt aErrorCode);
		
        /**
        * HandleCancelLoginL.
        * From MImConnectionObserver
        */
		void HandleCancelLoginL(const TInt aErrorCode);
		
        /**
        * HandleLogoutL.
        * From MImConnectionObserver
        */
        void HandleLogoutL(const TInt aErrorCode);
        
        /**
        * HandleMessageSentL.
        * From MImObserver
        */
        void HandleMessageSentL(const TInt aOpCode, const TInt aErrorCode );
        
        /**
        * HandleSendErrorL.
        * From MImObserver
        */
        void HandleSendErrorL(const TInt aOpCode, const TInt aErrorCode, 
            						MImClientDetailedError* aDetailedError);
            						
        /**
        * HandleNewPToPMessageL.
        * From MImObserver
        */
        void HandleNewPToPMessageL(	const TInt aErrorCode, 
						        	const TContactItemId  aContactId,
						            const TDesC& aUserId,
						            const TDesC& aMessageType,
						            const TDesC8& aContent );
		
        /**
        * HandleNewPToPMessageL.
        * From MImObserver
        */
        void HandleNewPToPMessageL( const TInt aErrorCode, 
						            const TContactItemId  aContactId,
						            const TDesC& aUserId,
						            const TDesC& aMessageType,
						            const TDesC16& aContent );
						            
		
	public: // own methods
	
        /**
        * LoginL.
        * Login request to IM server. This method is Asynchronous. called from
        * UI
        * @param aServerAddress IM server address
        * aUserId user id
        * aPassword password
        * aAccessPoint access point number as integer
        */
        void LoginL(const TDesC& aServerAddress, const TDesC& aUserId,
							const TDesC& aPassword, const TInt aAccessPoint);
								
        /**
        * LogoutL.
        * Logout request from IM server. Called from UI
        */
        void LogoutL();
        
        /**
        * SendChallenge.
        * Send the challenge to the oponent. Called from UI
        */
		void SendChallenge();
		
        /**
        * SendChallengeAccepted.
        * Send that the challenge has been accepted by the UI. Called from UI
        */
		void SendChallengeAccepted();
		
        /**
        * SendChallengeRejected.
        * Send that the challenge has been rejected by the UI. Called from UI
        */
		void SendChallengeRejected();
		
        /**
        * SendAlreadyInGame.
        * Send that user is already in a game so he/she cant accept messages
        * from users other than opponent.
        */
        void SendAlreadyInGame();
        
        /**
        * SendStartGame.
        * Send to the opponent that user has started the game.
        */
		void SendStartGame();
		
        /**
        * SendExitGame.
        * Send to the opponent that user has exited the game.
        */
		void SendExitGame();
		
        /**
        * SendPlayedIndex.
        * Send to the opponent that user has played on this index in the game
        * matrix, so that opponent mark that position
        * @param aIndex the location where user mark. 0 to 8
        */
		void SendPlayedIndex(TInt aIndex);
		
        /**
        * SendEndChallenge.
        * Send to the opponent that user has ended the challenge. This means
        * that user is now free to play with any other users
        */
		void SendEndChallenge();
		
        /**
        * SetOppID.
        * Set the opponent's user's id locally
        * @param aOppID Opponent's user id
        */
        void SetOppID(const TDesC16& aOppID);
	

	public: // constructor and destructor
	
        /**
        * NewLC.
        * Factory method for creating the class instance
        * @param aDocument Pointer to the application's document
        */
        static CIMOpenAPIExamplemodel* NewLC(CIMOpenAPIExampleDocument* aDocument );
        
        /**
        * NewL.
        * Factory method for creating the class instance
        * @param aDocument Pointer to the application's document
        */
        static CIMOpenAPIExamplemodel* NewL(CIMOpenAPIExampleDocument* aDocument );
        
        /**
        * ~CIMOpenAPIExamplemodel.
        * Destructor
        */
        ~CIMOpenAPIExamplemodel();
        
        /**
        * GetGameState.
        * Get current game state from model.
        * @return TGameState current game state
        */
		TGameState GetGameState() const;
		
        /**
        * SetGameState.
        * Set current game state
        * @param aState current game state
        */
		void SetGameState(TGameState const aState);
		
        /**
        * GetOppSign.
        * Get opponent's sign
        * @return TGameSign opponent's sign
        */
        TGameSign GetOppSign() const;
        
        /**
        * IsIndexPositionEmpty.
        * Find whether a position on game's grid is empty
        * @param aIndex location of position in the grid
        * @return TBool ETrue if position is empty
        */
        TBool IsIndexPositionEmpty(TInt aIndex);
        
        /**
        * PrepareToExit.
        * Close connections and prepare to exit application
        */
        void PrepareToExit();

    private: // own private methods
    
        /**
        * CIMOpenAPIExamplemodel.
        * C++ constructor
        */
        CIMOpenAPIExamplemodel();
        
        /**
        * ConstructL.
        * 2nd phase constructor
        * @param aDocument Pointer to the application's document
        */
        void ConstructL(CIMOpenAPIExampleDocument* aDocument);
        
        /**
        * CheckForSeries.
        * Check for a series of 3 consective values on the grid. The functions
        * checks whether the three positions starting from an starting index 
        * and increased by given increament has the same values.
        * @param aValue The value which needs to be continuous
        * aStartIndex Starting index on the grid
        * aIndexIncrement Increament for the next position
        * @return ETrue if series found
        */
		TBool CheckForSeries(TGameSign aValue, TInt aStartIndex, TInt aIndexIncrement);
		
        /**
        * MatchOppID.
        * Match whether the given id is same as opponent's id, Each message
        * goes through this check in a game.
        * @param aOppID id to be matched with opponent's id
        * @return ETrue if matched
        */
		TBool MatchOppID(TDesC16& aOppID);
		
        /**
        * SendMsg.
        * Send given message
        * @param aContent The message needs to be send
        * @return TInt Message id
        */
		TInt SendMsg(TDesC16& aContent);
		
        /**
        * ResetGameData.
        * Resets all game data, usually done before starting a game
        */
        void ResetGameData();
        
        /**
        * IsGameFinnished.
        * Find whether the game is finnished.
        * @param aIsDraw ETrue if game is a draw
        * @return ETrue if game is finnished
        */
        TBool IsGameFinnished(TBool& aIsDraw);
        
        /**
        * IsGameDraw.
        * Find whether the game has draw.
        * @return ETrue if game is draw
        */
        TBool IsGameDraw();

    private: // data
    
        /**
        * iIMConnection.
        * Pointer to CImConnection. Used for IM connection handling
        */
		CImConnection*					iIMConnection;
		
        /**
        * iIMClient.
        * Pointer to MImClient. Used for IM communication
        */
        MImClient*						iIMClient;
        
        /**
        * iDocument.
        * A local copy to the document pointer
        */
		CIMOpenAPIExampleDocument*			iDocument;

        /**
        * iGameData.
        * This is a 9 member array, represents game data during a game. It can
        * have three posible values. 0, 1 and -1. 0 is equivalent to circle
        * sign, 1 is to cross and -1 represents an empty location in game grid
        */
		TFixedArray<TGameSign,9> iGameData;
		
        /**
        * iState.
        * This is Game state, all othe classes will get the game state from
        * here. The TGameState is defined in IMOpenAPIExamplemsg.h
        */
		TGameState iState;
		
        /**
        * iMsgID.
        * This is last sent message id
        */
		TInt iMsgID;

        /**
        * iArrayOfIDs.
        * Array of oponent ids, to match with returning messages
        */
		CDesC16Array* iArrayOfIDs;

        /**
        * iOppID.
        * Opponent ID
        */
        HBufC* iOppID;

        /**
        * iOppID.
        * ETrue while some message is being send and response didnt yet come
        */
		TBool iSending;

        /**
        * iOppID.
        * Stores last message sent. TMsg is defined in IMOpenAPIExamplemsg.h
        */
        TMsg iLastMsg;

        /**
        * iMySign, iOppSign.
        * User's own and opponent's signs. TGameSign is defined in IMOpenAPIExamplemsg.h
        */
        TGameSign iMySign, iOppSign;
        
        /**
        * iMessageToSend
        * Buffer containing message to be sent
        */
        TBuf<KMaxProtocolStrLength> iMessageToSend;

        /**
        * iSeed
        * This seed is used to randomize who's turn is first.
        */
        TInt64 iSeed;
        
        /**
        * iNotSupported
        * Whether the IM functionality is supported or not
        */
        TBool iNotSupported;

    };

#endif // __IMOPENAPIEXAMPLEMODEL_H__

// end of file


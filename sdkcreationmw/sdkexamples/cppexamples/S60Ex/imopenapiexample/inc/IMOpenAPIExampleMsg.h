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


#ifndef __IMOPENAPIEXAMPLEMSG_H__
#define __IMOPENAPIEXAMPLEMSG_H__

// CONSTANTS
const TInt KMaxProtocolStrLength = 25;
const TInt KMaxUserIDLength = 50;
const TInt KMaxInfoMsgLength = 100;

/**
* TGameState.
* Used to define the game state. Using game state engine and UI take actions.
*/
enum TGameState
			{
			EStateNotLogin,
			EStateLoginIdle,
			EStateSending,
			EStateChallenged,
			EStateReadytoStart,
			EStateWaitingForOppMove,
			EStateWaitingForOwnMove
			};
			
/**
* TGameSign.
* Defines signs, 0 means cross sign, 1 means zero sign, 2 means empty.
*/
enum TGameSign
			{
			ECrossSign = 0,
			EZeroSign,
			EEmptySign
			};
			
/**
* TInternalMsgID.
* Internal message ids which model sends to UI
*/
enum TInternalMsgID
			{
			Connected,
			DisConnected,
			ChallengeRec,
			ChallengeSent,
			MsgConfirmation,
			Error,
			PlayedIndexRec,
			StartGameRec,
			EndGameRec,
			ChallengeAcceptedRec,
			ChallengeRejectedRec,
            EndChallengeRec,
			AlreadyInGameRec,
            YouWon,
            YouLost,
            GameDraw,
            StartGameWarning
			};

/**
* TMsg.
* Used to save messages in model
*/
enum TMsg
			{
			MsgChallenge,
			MsgChallengeAccepted,
			MsgChallengeRejected,
			MsgStartGameYourTurn,
            MsgStartGameOppTurn,
			MsgExitGame,
			MsgPlayedIndex,
			MsgEndChallenge,
			MsgAlreadyInGame
			};



/**
* Constant strings used in message protocols.
*/
_LIT(KChallenge, "Challenge");
_LIT(KChallengeAccepted, "ChallengeAccepted");
_LIT(KChallengeRejected, "ChallengeRejected");
_LIT(KStartGameYourTurn, "StartGameYourTurn");
_LIT(KStartGameOppTurn, "StartGameOppTurn");
_LIT(KExitGame, "ExitGame");
_LIT(KPlayedIndex, "PlayedIndex");
_LIT(KEndChallenge, "EndChallenge");
_LIT(KAlreadyInGame, "AlreadyInGame");

#endif // __IMOPENAPIEXAMPLEMSG_H__

// end of file


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

// INCLUDE FILES
#include <imconnection.h>
#include <imclient.h>
#include <bamdesca.h>
#include <badesca.h>
#include <imerrors.h>
#include <e32math.h>
#include "IMOpenAPIExamplemodel.h"
#include "IMOpenAPIExampleDocument.h"
#include "IMOpenAPIExampleAppUi.h"
#include "IMOpenAPIExamplemsg.h"

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CIMOpenAPIExamplemodel::ConstructL()
// takes a pointer to document
// ----------------------------------------------------------
//
void CIMOpenAPIExamplemodel::ConstructL(CIMOpenAPIExampleDocument* aDocument)
    {
    TRAPD(err,iIMConnection = CImConnection::NewL(KNullDesC));
    if (err!=KErrNone)
        {
        iNotSupported = ETrue;
        iState = EStateNotLogin;
        }
    else
        {
    	iIMConnection->RegisterObserverL(this); // registering the connection
    	iIMClient = iIMConnection->CreateImClientL();// creating the client
    	iIMClient->RegisterObserverL(this); // registering the client
            
        // Check the IM status and set the application accordingly
    	if (iIMConnection->ImConnectionStatus() == ELoggedIn)
    		iState = EStateLoginIdle;
    	else
            
    	iNotSupported=EFalse;        
        }

    iDocument = aDocument;
    ResetGameData(); // reset game data grid
	

	iArrayOfIDs = new (ELeave) CDesCArrayFlat(1);
   
	// msg sent ids are zero in the begining
	iMsgID = 0;
    iSending = EFalse;
    iLastMsg = MsgEndChallenge;
    SetOppID(KNullDesC);
	}

// ----------------------------------------------------------
// CIMOpenAPIExamplemodel::NewLC()
// 
// ----------------------------------------------------------
//
CIMOpenAPIExamplemodel* CIMOpenAPIExamplemodel::NewLC(CIMOpenAPIExampleDocument* aDocument)
    {
    CIMOpenAPIExamplemodel* self = new (ELeave) CIMOpenAPIExamplemodel;
    CleanupStack::PushL(self);
    self->ConstructL(aDocument);
    return self;
    }

// ----------------------------------------------------------
// CIMOpenAPIExamplemodel::NewL()
// 
// ----------------------------------------------------------
//
CIMOpenAPIExamplemodel* CIMOpenAPIExamplemodel::NewL(CIMOpenAPIExampleDocument* aDocument)
    {
    CIMOpenAPIExamplemodel* self = NewLC(aDocument);
    CleanupStack::Pop(self);
    return self;
	}

// ----------------------------------------------------
// CIMOpenAPIExamplemodel::~CIMOpenAPIExamplemodel()
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CIMOpenAPIExamplemodel::~CIMOpenAPIExamplemodel()
	{
	if( iIMClient )
		{
        iIMClient->UnregisterObserver();
        delete iIMClient;
        iIMClient = NULL;
        }
    
    if (iIMConnection)
        {
        iIMConnection->UnregisterObserver();
        delete iIMConnection;
        iIMConnection = NULL;
        }
    
    if(iOppID)                    
        delete iOppID;
    iOppID = NULL;
    
    if (iArrayOfIDs)
        {
        iArrayOfIDs->Reset();
        delete iArrayOfIDs;
        }
	}


// ----------------------------------------------------
// CIMOpenAPIExamplemodel::CIMOpenAPIExamplemodel()
// 
// ----------------------------------------------------
//
CIMOpenAPIExamplemodel::CIMOpenAPIExamplemodel()
	{
	}
	
// FUNCTIONS FROM THE BASE CLASSES
// ----------------------------------------------------
// CIMOpenAPIExamplemodel::HandleLoginL(const TInt aErrorCode)
// 
// ----------------------------------------------------
//
void CIMOpenAPIExamplemodel::HandleLoginL(const TInt aErrorCode)
    {
	iDocument->UI()->HandleMessageIMConnectionFromModel(Connected, aErrorCode);
	if (aErrorCode == KErrNone)
		{
		SetGameState(EStateLoginIdle);
		}
    }

// ----------------------------------------------------
// CIMOpenAPIExamplemodel::HandleLogoutL()
// 
// ----------------------------------------------------
//
void CIMOpenAPIExamplemodel::HandleLogoutL(const TInt aErrorCode)
    {
	iDocument->UI()->HandleMessageIMConnectionFromModel(DisConnected, aErrorCode);
	SetGameState(EStateNotLogin);											
    }
    
// ----------------------------------------------------
// CIMOpenAPIExamplemodel::HandleLogoutL()
// 
// ----------------------------------------------------
//    
void CIMOpenAPIExamplemodel::HandleCancelLoginL(const TInt /*aErrorCode*/)
	{
	
	
	}

// ----------------------------------------------------
// CIMOpenAPIExamplemodel::HandleMessageSentL()
// 
// ----------------------------------------------------
//
void CIMOpenAPIExamplemodel::HandleMessageSentL( const TInt /*aOpCode*/, const TInt aErrorCode )
    {
    iSending = EFalse;
    switch (iLastMsg)
        {
        case MsgChallenge:
            SetGameState(EStateChallenged);
            iDocument->UI()->HandleMessageFromModel(ChallengeSent, aErrorCode, KNullDesC);
            break;
        
        case MsgChallengeAccepted:
            SetGameState(EStateReadytoStart);
            break;

        case MsgChallengeRejected:
            SetGameState(EStateLoginIdle);
            break;

        case MsgStartGameYourTurn:
            if (GetGameState() == EStateReadytoStart)
                SetGameState(EStateWaitingForOppMove);
            break;

        case MsgStartGameOppTurn:
            if (GetGameState() == EStateReadytoStart)
                SetGameState(EStateWaitingForOwnMove);
            break;

        case MsgExitGame:
            iDocument->UI()->HandleMessageFromModel(EndGameRec, aErrorCode, KNullDesC);
            SetGameState(EStateReadytoStart);
            break;

        case MsgPlayedIndex:
            break;

        case MsgEndChallenge:
            iDocument->UI()->HandleMessageFromModel(EndChallengeRec, aErrorCode, KNullDesC);
            SetGameState(EStateLoginIdle);
            break;

        case MsgAlreadyInGame:
            break;

        default:
            break;
        }
	}

// ----------------------------------------------------
// CIMOpenAPIExamplemodel::HandleSendErrorL()
// 
// ----------------------------------------------------
//
void CIMOpenAPIExamplemodel::HandleSendErrorL(const TInt /*aOpCode*/, 
                                      const TInt aErrorCode, 
                                      MImClientDetailedError* /*aDetailedError*/)
    {
    iSending = EFalse;


    switch (aErrorCode)
        {
        case KImApiErrNotLogged:
            SetGameState(EStateNotLogin);
            iDocument->UI()->HandleMessageErrorFromModel(aErrorCode,KNullDesC );
            break;

        default:
            iDocument->UI()->HandleMessageErrorFromModel(aErrorCode,KNullDesC );
            break;
        }
    }


// ----------------------------------------------------
// CIMOpenAPIExamplemodel::HandleNewPToPMessageL()
// 
// ----------------------------------------------------
//
void CIMOpenAPIExamplemodel::HandleNewPToPMessageL(
            const TInt aErrorCode, 
            const TContactItemId  /*aContactId*/,
            const TDesC& aUserId,
            const TDesC& /*aMessageType*/,
            const TDesC16& aContent )
    {
    TInt integerInfo = 0;
    TLex16 receivedMsgLex;
    TPtrC16 receivedMsg;
    TBool isDraw = EFalse;


    // checking for played index, which is different from other messages
    if (aContent.Find(KPlayedIndex) != KErrNotFound)
        {
        receivedMsgLex.Assign(aContent.Right(1));
        receivedMsgLex.Val(integerInfo);
        receivedMsg.Set( aContent.Left(11) );
        }
    else
        receivedMsg.Set(aContent);

    switch (iState)
        {
        // In login idle state we only expect a Challenge from any other user
        case EStateLoginIdle:
            if ( receivedMsg.Compare(KChallenge) == 0)
                {
                SetOppID(aUserId);
                iDocument->UI()->HandleChallengeRec(aUserId);
                }
            break;

        // In Challenged state we only expects challenge accepted or rejected from our opp
        case EStateChallenged:
            if( aUserId.Find(*iOppID) == KErrNotFound )
                break;
            else if ( receivedMsg.Compare(KChallengeAccepted) == 0)
                {
                SetGameState(EStateReadytoStart);
                iDocument->UI()->HandleMessageFromModel(ChallengeAcceptedRec, aErrorCode, aContent);
                }
            else if ( receivedMsg.Compare(KChallengeRejected) == 0)
                {
                SetGameState(EStateLoginIdle);
                iDocument->UI()->HandleMessageFromModel(ChallengeRejectedRec, aErrorCode, aContent);
                }
            else if ( receivedMsg.Compare(KAlreadyInGame) == 0)
                {
                SetGameState(EStateLoginIdle);
                iDocument->UI()->HandleMessageFromModel(AlreadyInGameRec, aErrorCode, aContent);
                }
            // if opp also sent the challenge at the same time, we will reply already in game
            else if ( receivedMsg.Compare(KChallenge) == 0)
                {
                SetGameState(EStateLoginIdle);
                iDocument->UI()->HandleMessageFromModel(AlreadyInGameRec, aErrorCode, aContent);
                }
            // in all othe requests we will not take any action
            break;

        
        case EStateReadytoStart:
            if( aUserId.Find(*iOppID) == KErrNotFound )
                break;
            else if ( receivedMsg.Compare(KStartGameYourTurn) == 0)
                {
                SetGameState(EStateWaitingForOwnMove);
                iDocument->UI()->HandleMessageFromModel(StartGameRec, aErrorCode, aContent);
                }
            else if ( receivedMsg.Compare(KStartGameOppTurn) == 0)
                {
                SetGameState(EStateWaitingForOppMove);
                iDocument->UI()->HandleMessageFromModel(StartGameRec, aErrorCode, aContent);
                }
            else if ( receivedMsg.Compare(KExitGame) == 0)
                {
                iDocument->UI()->HandleMessageFromModel(EndGameRec, aErrorCode, aContent);
                SetGameState(EStateReadytoStart);
                }
            else if ( receivedMsg.Compare(KEndChallenge) == 0)
                {
                iDocument->UI()->HandleMessageFromModel(EndChallengeRec, aErrorCode, KNullDesC);
                SetGameState(EStateLoginIdle);
                }
            else
                SendEndChallenge();
            break;


        case EStateWaitingForOppMove:
            if( aUserId.Find(*iOppID) == KErrNotFound )
                break;
            else if ( receivedMsg.Compare(KPlayedIndex) == 0)
                {
                iDocument->UI()->HandleMessageFromModel(PlayedIndexRec, integerInfo, aContent);
                iGameData.At(integerInfo) = iOppSign;
                if ( IsGameFinnished(isDraw) )
                    {
                    if (isDraw)
                        iDocument->UI()->HandleMessageFromModel(GameDraw, aErrorCode, aContent);
                    else
                        iDocument->UI()->HandleMessageFromModel(YouLost, aErrorCode, aContent);
                    ResetGameData();
                    SetGameState(EStateReadytoStart);
                    break;
                    }
                SetGameState(EStateWaitingForOwnMove);
                }
            else if ( receivedMsg.Compare(KExitGame) == 0)
                {
                iDocument->UI()->HandleMessageFromModel(EndGameRec, aErrorCode, aContent);
                SetGameState(EStateReadytoStart);
                }
            else
                SendEndChallenge();
            break;


        case EStateWaitingForOwnMove:
            if( aUserId.Find(*iOppID) == KErrNotFound )
                break;
            if ( receivedMsg.Compare(KExitGame) == 0)
                {
                iDocument->UI()->HandleMessageFromModel(EndGameRec, aErrorCode, aContent);
                SetGameState(EStateReadytoStart);
                }
            else
                SendEndChallenge();
            break;


        default:
            break;

        }

	}

// ----------------------------------------------------
// CIMOpenAPIExamplemodel::HandleNewPToPMessageL
// Sends messages to IM
// ----------------------------------------------------
//
void CIMOpenAPIExamplemodel::HandleNewPToPMessageL(
                                           const TInt /*aErrorCode*/, 
                                           const TContactItemId /*aContactId*/,
                                           const TDesC& /*aUserId*/,
                                           const TDesC& /*aMessageType*/,
                                           const TDesC8& /*aContent*/ )
    {
    }

// ----------------------------------------------------
// CIMOpenAPIExamplemodel::LoginL
// 
// ----------------------------------------------------
//
void CIMOpenAPIExamplemodel::LoginL(const TDesC& aServerAddress, const TDesC& aUserId,
								const TDesC& aPassword, const TInt aAccessPoint)
    {
	// We are using TRAPD at the UI level, because it has GUI to display
	if (iNotSupported)
	    User::Leave(KErrNotSupported);	
    iIMConnection->LoginL(aServerAddress, aUserId, aPassword, aAccessPoint);
    }
    
// ----------------------------------------------------
// CIMOpenAPIExamplemodel::LogoutL
// 
// ----------------------------------------------------
//
void CIMOpenAPIExamplemodel::LogoutL()
	{
	// We are using TRAPD at the UI level, because it has GUI to display
	iIMConnection->LogoutL();
	}

// ----------------------------------------------------
// CIMOpenAPIExamplemodel::SendMsg
// Sends messages to IM
// ----------------------------------------------------
//
TInt CIMOpenAPIExamplemodel::SendMsg(TDesC16& aContent)
	{
	iSending = ETrue;
    //SetGameState (EStateSending);
	return iIMClient->SendPToPMessageL(*iArrayOfIDs, aContent);
	}
	
// ----------------------------------------------------
// CIMOpenAPIExamplemodel::SendChallenge
// ----------------------------------------------------
//
void CIMOpenAPIExamplemodel::SendChallenge()
	{
	iMessageToSend.Copy(KChallenge);
	iMsgID = SendMsg(iMessageToSend);
    iLastMsg = MsgChallenge;
    iMySign = ECrossSign;
    iOppSign = EZeroSign;
	}
	
// ----------------------------------------------------
// CIMOpenAPIExamplemodel::SendChallengeAccepted
// ----------------------------------------------------
//
void CIMOpenAPIExamplemodel::SendChallengeAccepted()
	{
	iMessageToSend.Copy(KChallengeAccepted);
    iMsgID = SendMsg(iMessageToSend);
    iLastMsg = MsgChallengeAccepted;
    iMySign = EZeroSign;
    iOppSign = ECrossSign;
	}
	
// ----------------------------------------------------
// CIMOpenAPIExamplemodel::SendChallengeRejected
// ----------------------------------------------------
//
void CIMOpenAPIExamplemodel::SendChallengeRejected()
	{
    iMessageToSend.Copy(KChallengeRejected);	
    iMsgID = SendMsg(iMessageToSend);
    iLastMsg = MsgChallengeRejected;
    SetOppID(KNullDesC);
	}
	
// ----------------------------------------------------
// CIMOpenAPIExamplemodel::SendAlreadyInGame
// ----------------------------------------------------
//
void CIMOpenAPIExamplemodel::SendAlreadyInGame()
	{
    iMessageToSend.Copy(KAlreadyInGame);	
    iMsgID = SendMsg(iMessageToSend);
    iLastMsg = MsgAlreadyInGame;
	}
	
// ----------------------------------------------------
// CIMOpenAPIExamplemodel::SendStartGame
// ----------------------------------------------------
//
void CIMOpenAPIExamplemodel::SendStartGame()
	{
    TBool turn;
    
    // Find a randomize turn
    turn = ( Math::Rand(iSeed) ) % 2;

    if (turn)
        {
        iMessageToSend.Copy(KStartGameOppTurn);
	    iMsgID = SendMsg(iMessageToSend);
        iLastMsg = MsgStartGameOppTurn;
        }
    else
        {
        iMessageToSend.Copy(KStartGameYourTurn);
	    iMsgID = SendMsg(iMessageToSend);
        iLastMsg = MsgStartGameYourTurn;
        }
	}
	
// ----------------------------------------------------
// CIMOpenAPIExamplemodel::SendExitGame
// ----------------------------------------------------
//
void CIMOpenAPIExamplemodel::SendExitGame()
	{
    iMessageToSend.Copy(KExitGame);
	iMsgID = SendMsg(iMessageToSend);
    iLastMsg = MsgExitGame;
	}

// ----------------------------------------------------
// CIMOpenAPIExamplemodel::SendPlayedIndex
// ----------------------------------------------------
//
void CIMOpenAPIExamplemodel::SendPlayedIndex(TInt aIndex)
	{
    TBool isDraw = EFalse;

    iMessageToSend.Copy(KPlayedIndex);
	iMessageToSend.AppendNumUC(aIndex,EDecimal);
	iMsgID = SendMsg(iMessageToSend);
    iLastMsg = MsgPlayedIndex;

    iGameData.At(aIndex) = iMySign;
    if ( IsGameFinnished(isDraw) )
        {
        if (isDraw)
            iDocument->UI()->HandleMessageFromModel(GameDraw, KErrNone, KNullDesC);
        else
            iDocument->UI()->HandleMessageFromModel(YouWon, KErrNone, KNullDesC);
        ResetGameData();
        SetGameState(EStateReadytoStart);
        return;
        }
    SetGameState(EStateWaitingForOppMove);
	}
	
// ----------------------------------------------------
// CIMOpenAPIExamplemodel::SendEndChallenge
// ----------------------------------------------------
//
void CIMOpenAPIExamplemodel::SendEndChallenge()
	{
    iMessageToSend.Copy(KEndChallenge);
	iMsgID = SendMsg(iMessageToSend);
    iLastMsg = MsgEndChallenge;
	}
// ----------------------------------------------------
// CIMOpenAPIExamplemodel::IsGameFinnished
// ----------------------------------------------------
//
TBool CIMOpenAPIExamplemodel::IsGameFinnished(TBool& aIsDraw)
	{
	// checking for sign ECrossSign (*) win in the matrix
	if ( CheckForSeries(ECrossSign,ECrossSign,EZeroSign) || CheckForSeries(ECrossSign,3,EZeroSign) || 
        CheckForSeries(ECrossSign,6,EZeroSign) || CheckForSeries(ECrossSign,ECrossSign,3) || 
        CheckForSeries(ECrossSign,EZeroSign,3) || CheckForSeries(ECrossSign,EEmptySign,3) ||
		CheckForSeries(ECrossSign,ECrossSign,4) || CheckForSeries(ECrossSign,EEmptySign,EEmptySign) )
		{
        aIsDraw = EFalse;
		return ETrue;
		}

	// checking for sign EZeroSign (o) win in the matrix
	else if ( CheckForSeries(EZeroSign,ECrossSign,EZeroSign) || CheckForSeries(EZeroSign,3,EZeroSign) || CheckForSeries(EZeroSign,6,EZeroSign) ||
		CheckForSeries(EZeroSign,ECrossSign,3) || CheckForSeries(EZeroSign,EZeroSign,3) || CheckForSeries(EZeroSign,EEmptySign,3) ||
		CheckForSeries(EZeroSign,ECrossSign,4) || CheckForSeries(EZeroSign,EEmptySign,EEmptySign) )
		{
        aIsDraw = EFalse;
		return ETrue;
		}

	else if (IsGameDraw())
        {
        aIsDraw = ETrue;
        return ETrue;
        }

	return EFalse;
	}

// ----------------------------------------------------
// CIMOpenAPIExamplemodel::IsGameDraw()
// checks if the game has draw, should be called after checking for winner
// ----------------------------------------------------
//
TBool CIMOpenAPIExamplemodel::IsGameDraw()
    {
    for (TInt i=0;i<9;i++)
        {
        if (iGameData.At(i) == EEmptySign)
            return false;
        }
    return true;
    }

// ----------------------------------------------------
// CIMOpenAPIExamplemodel::CheckForSeries(TInt8 value, TInt8 startIndex, TInt8 indexIncrement)
// Check for a series of 3 consective 'value', give start index and increment by number
// ----------------------------------------------------
//
TBool CIMOpenAPIExamplemodel::CheckForSeries(TGameSign aValue, TInt aStartIndex, TInt aIndexIncrement)
	{
	return ( (iGameData.At(aStartIndex) == aValue) && 
				(iGameData.At(aStartIndex+aIndexIncrement) == aValue) && 
				(iGameData.At(aStartIndex+aIndexIncrement+aIndexIncrement) == aValue) );

	}

// ----------------------------------------------------
// CIMOpenAPIExamplemodel::GetGameState()
// returns Game current state, only call from outside this class
// ----------------------------------------------------
//
TGameState CIMOpenAPIExamplemodel::GetGameState() const
	{
    if(iSending)
        return EStateSending;
	return iState;
	}

// ----------------------------------------------------
// CIMOpenAPIExamplemodel::SetGameState()
// Sets game status, in all the game
// ----------------------------------------------------
//
void CIMOpenAPIExamplemodel::SetGameState(TGameState const aState)
	{
	iState = aState;
	iDocument->UI()->SetGameState(aState);
    if (iState == EStateReadytoStart)
        ResetGameData();
    else if (iState == EStateLoginIdle)
        SetOppID(KNullDesC);
	}

// ----------------------------------------------------
// CIMOpenAPIExamplemodel::SetOppID(TDesC16& aOppID)
// Sets the oponent id and pushed it into an array
// ----------------------------------------------------
//
void CIMOpenAPIExamplemodel::SetOppID(const TDesC16& aOppID)
	{
    TBuf<KMaxUserIDLength> oppID(aOppID);
    oppID.LowerCase();    
    delete iOppID;
    iOppID = NULL;
    iOppID = oppID.AllocL();
    iArrayOfIDs->Reset();
	iArrayOfIDs->AppendL(*iOppID);
	}

// ----------------------------------------------------
// CIMOpenAPIExamplemodel::MatchOppID(TDesC16& aOppID)
// matches incoming opp id to existing opp id, returns true if success
// ----------------------------------------------------
//
TBool CIMOpenAPIExamplemodel::MatchOppID(TDesC16& aOppID)
	{
	if ( iOppID->Compare(aOppID) == 0 )
		return ETrue;
	return EFalse;
	}

// ----------------------------------------------------
// CIMOpenAPIExamplemodel::GetOppSign()
// Gets opponent sign
// ----------------------------------------------------
//
TGameSign CIMOpenAPIExamplemodel::GetOppSign() const
    {
    return iOppSign;
    }

// ----------------------------------------------------
// CIMOpenAPIExamplemodel::ResetGameData()
// ----------------------------------------------------
//
void CIMOpenAPIExamplemodel::ResetGameData()
    {
    for (TInt8 i=0; i<9; i++) // set all game matrix data to 'empty'
		iGameData.At(i) = EEmptySign;
    }
    
// ----------------------------------------------------
// CIMOpenAPIExamplemodel::IsIndexPositionEmpty
// ----------------------------------------------------
//
TBool CIMOpenAPIExamplemodel::IsIndexPositionEmpty(TInt aIndex)
    {
    if ( iGameData.At(aIndex) == EEmptySign )
        return ETrue;
    return EFalse;
    }

// ----------------------------------------------------
// CIMOpenAPIExamplemodel::PrepareToExit()
// ----------------------------------------------------
//
void CIMOpenAPIExamplemodel::PrepareToExit()
    {
    if ( (iState != EStateNotLogin) && (iState != EStateLoginIdle) )
        SendEndChallenge();
    }
    
// End of File


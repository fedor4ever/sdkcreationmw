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
#include <eikmenub.h>
#include <aknwaitdialog.h>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <badesca.h>
#include <imerrors.h>
#include <aknquerydialog.h>
#include <aknlistquerydialog.h>
#include <akndialog.h>
#include <avkon.hrh>
#include <IMOpenAPIExample.rsg>

#include "IMOpenAPIExampleAppUi.h"
#include "IMOpenAPIExampleContainer.h"
#include "IMOpenAPIExampleDocument.h"
#include "IMOpenAPIExamplemodel.h" 
#include "IMOpenAPIExample.hrh"
#include "IMOpenAPIExampleGrid.h"

// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CIMOpenAPIExampleAppUi::ConstructL()
// 
// ----------------------------------------------------------
//
void CIMOpenAPIExampleAppUi::ConstructL()
    {
    BaseConstructL(CAknAppUi::EAknEnableSkin);
    SetLayoutAwareApp(EFalse);
    
	Document().SetPointerToUI(this);
    iAppContainer = new (ELeave) CIMOpenAPIExampleContainer;
    iAppContainer->SetMopParent( this );
    iAppContainer->ConstructL( ClientRect() );
    AddToStackL( iAppContainer );
    
	
    iAppContainer->SetGameView( Document().Model().GetGameState() );
    iAppContainer->SetModel( &(Document().Model()) );
    
    iAPNumber = 0;
    iLoginSettingsDone = EFalse;
    
    iDisplayMessage.Copy(KNullDesC);
    }

// ----------------------------------------------------
// CIMOpenAPIExampleAppUi::~CIMOpenAPIExampleAppUi()
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CIMOpenAPIExampleAppUi::~CIMOpenAPIExampleAppUi()
    {
    if (iAppContainer)
        {
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
        }
    if (iWaitDialog)
        {
        delete iWaitDialog;
        iWaitDialog = NULL;
        }
        
    iDlgSrv.Close();
        
   }

// ------------------------------------------------------------------------------
// CIMOpenAPIExampleAppUi::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CIMOpenAPIExampleAppUi::DynInitMenuPaneL(
    TInt aResourceId,CEikMenuPane* aMenuPane)
    {
	// Here we select the game options menu according to game state
		switch (aResourceId)
			{
			case R_IMOPENAPIEXAMPLE_MENU:
				switch ( Document().Model().GetGameState() )
				{
				case EStateNotLogin:
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdExitGame, ETrue);
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdChallenge, ETrue);
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdSelect, ETrue);
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdLogin, (!iLoginSettingsDone));
                    aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdStartGame, ETrue);
                    aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdEndChallenge, ETrue);
                    aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdLogout, ETrue);
                    aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdIMSettings, EFalse);
                    break;

				case EStateSending:
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdExitGame, ETrue);
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdChallenge, ETrue);
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdSelect, ETrue);
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdLogin, ETrue);
                    aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdStartGame, ETrue);
                    aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdEndChallenge, ETrue);
                    aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdLogout, ETrue);
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdIMSettings, ETrue);
					break;

				case EStateLoginIdle:
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdExitGame, ETrue);
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdLogin, ETrue);
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdSelect, ETrue);
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdChallenge, EFalse);
                    aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdStartGame, ETrue);
                    aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdEndChallenge, ETrue);
                    aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdLogout, EFalse);
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdIMSettings, ETrue);
					break;

				case EStateChallenged:
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdExitGame, ETrue);
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdSelect, ETrue);
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdChallenge, ETrue);
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdLogin, ETrue);
                    aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdStartGame, ETrue);
                    aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdEndChallenge, EFalse);
                    aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdLogout, ETrue);
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdIMSettings, ETrue);
					break;
				
				case EStateReadytoStart:
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdLogin, ETrue);
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdSelect, ETrue);
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdChallenge, ETrue);
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdExitGame, ETrue);
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdLogout, ETrue);
                    if (Document().Model().GetOppSign() == ECrossSign )
                        aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdStartGame, ETrue);
                    else
                        aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdStartGame, EFalse);
                    aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdEndChallenge, EFalse);
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdIMSettings, ETrue);
					break;

				case EStateWaitingForOppMove:
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdLogin, ETrue);
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdSelect, ETrue);
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdChallenge, ETrue);
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdExitGame, EFalse);
                    aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdStartGame, ETrue);
                    aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdEndChallenge, ETrue);
                    aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdLogout, ETrue);
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdIMSettings, ETrue);
					break;

				case EStateWaitingForOwnMove:
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdLogin, ETrue);
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdChallenge, ETrue);
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdSelect, EFalse);
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdExitGame, EFalse);
                    aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdStartGame, ETrue);
                    aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdEndChallenge, ETrue);
                    aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdLogout, ETrue);
					aMenuPane->SetItemDimmed(EIMOpenAPIExampleCmdIMSettings, ETrue);
					break;

				default:
					break;
				} // iState switch ends
			default:
				break;
			} // aResourceId switch ends  
    }

// ----------------------------------------------------
// CIMOpenAPIExampleAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
// takes care of key event handling
// ----------------------------------------------------
//
TKeyResponse CIMOpenAPIExampleAppUi::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/,TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------
// CIMOpenAPIExampleAppUi::HandleCommandL(TInt aCommand)
// takes care of command handling
// ----------------------------------------------------
//
void CIMOpenAPIExampleAppUi::HandleCommandL(TInt aCommand)
    {
	TInt err = KErrNone;
	_LIT(errString, "Error:( %i )");
	TBuf<KMaxChallengeIDLength> OppnentID;
	TBuf<KMaxChallengeIDLength> OppnentI;

	switch ( aCommand )
		{
		case EAknSoftkeyBack:
            TRAP( err , Document().Model().PrepareToExit() );
            Exit();
			break;
		case EEikCmdExit:
            TRAP( err , Document().Model().PrepareToExit() );
            Exit();
			break;

		case EIMOpenAPIExampleCmdLogin:
			TRAP(err ,Document().Model().LoginL(iServerAddress, iUserID, iPassword, iAPNumber));
			if (err!=KErrNone)
				{
	            break;			    
	    		}
			iWaitDialog = new(ELeave)CAknWaitDialog((REINTERPRET_CAST(CEikDialog**,&iWaitDialog)), EFalse );
			StringLoader::Load ( iDisplayMessage, R_INFO_MSG_LOGGING );
			iWaitDialog->SetTextL  ( iDisplayMessage );  
			iWaitDialog->ExecuteDlgLD(CAknNoteDialog::ENoTone,R_SINGULAR_WAIT_NOTE);
			break;
			
		case EIMOpenAPIExampleCmdLogout:
			TRAP(err ,Document().Model().LogoutL());
			iWaitDialog = new(ELeave)CAknWaitDialog((REINTERPRET_CAST(CEikDialog**,&iWaitDialog)), EFalse );
			StringLoader::Load ( iDisplayMessage, R_INFO_MSG_LOGGING_OUT );
			iWaitDialog->SetTextL  ( iDisplayMessage );  
			iWaitDialog->ExecuteDlgLD(CAknNoteDialog::ENoTone,R_SINGULAR_WAIT_NOTE);
			break;
			
		case EIMOpenAPIExampleCmdIMSettings:
			LoginSettings();
			break;
			
		case EIMOpenAPIExampleCmdExitGame:
            TRAP(err ,Document().Model().SendExitGame());
			break;

        case EIMOpenAPIExampleCmdStartGame:
            TRAP(err ,Document().Model().SendStartGame());
			break;

		case EIMOpenAPIExampleCmdChallenge:
            if (Challenge())
                {
                TRAP( err , Document().Model().SendChallenge() );
                if(err!=KErrNone)
                    break;
                iAppContainer->SetMySign(ECrossSign);
                }
			break;

        case EIMOpenAPIExampleCmdEndChallenge:
            TRAP( err , Document().Model().SendEndChallenge() );
            break;

        case EIMOpenAPIExampleCmdSelect:
            iAppContainer->PlayMove();
            break;
		
		default:
			break;      
		}
	
	if (err != KErrNone )
			{
			if(err==-5)
					{
					CAknInformationNote* note = new (ELeave) CAknInformationNote( EFalse );
					note->SetTimeout( CAknNoteDialog::ELongTimeout  );
					StringLoader::Load ( iDisplayMessage, R_INFO_MSG_NOT_SUPPORTED );
					note->ExecuteLD( iDisplayMessage );
 			  	iDisplayMessage.Copy(KNullDesC); 
					}	
			else 
					{
					CAknErrorNote* note = new(ELeave) CAknErrorNote();
					TBuf<0x100> buf;
					buf.Format(errString, err );
					note->ExecuteLD( buf );			     
					}
			}
			iDisplayMessage.Copy(KNullDesC);
	}


// ----------------------------------------------------
// CIMOpenAPIExampleAppUi::HandleMessageErrorFromModel( const TInt aErrorCode,
//									                const TDesC& aStringInfo)
// Model calls this function for error messages
// ----------------------------------------------------
//
void CIMOpenAPIExampleAppUi::HandleMessageErrorFromModel(
									const TInt aErrorCode,
									const TDesC& aStringInfo )
	{
	CAknInformationNote* note = new (ELeave) CAknInformationNote( EFalse );
	note->SetTimeout( CAknNoteDialog::ELongTimeout  );
	_LIT(errString, "in, %S with Error code: %d");

	switch (aErrorCode)
        {
        case KImApiErrNotLogged:
            StringLoader::Load ( iDisplayMessage, R_INFO_MSG_NOT_LOGGEDIN );
            break;

		default:
            StringLoader::Load ( iDisplayMessage, R_INFO_MSG_ERROR );
            iDisplayMessage.AppendFormat(errString, &aStringInfo, aErrorCode);
		    break;
		}
    note->ExecuteLD( iDisplayMessage );
    iDisplayMessage.Copy(KNullDesC); 
	}

// ----------------------------------------------------
// CIMOpenAPIExampleAppUi::HandleChallengeRec(const TDesC& aUserID )
// Model calls this function when it receives a challenge
// ----------------------------------------------------
//

void CIMOpenAPIExampleAppUi::HandleChallengeRec(const TDesC& aUserID )
    {
    TInt err = KErrNone;
    _LIT(errorString,"Error:( %i )");
    CAknQueryDialog* query = CAknQueryDialog::NewL(CAknQueryDialog::EConfirmationTone);
    StringLoader::Load ( iDisplayMessage, R_INFO_MSG_CHALLENGE_RECEIVED );
    iDisplayMessage.Append(aUserID);
    if ( query->ExecuteLD( R_CHALLENGE_QUERY, iDisplayMessage ) )
        {
        TRAP( err , Document().Model().SendChallengeAccepted() );
        iAppContainer->SetMySign(EZeroSign);
        }
    else
        TRAP( err , Document().Model().SendChallengeRejected() );
    	
    if (err != KErrNone )
		{
		CAknErrorNote* note = new(ELeave) CAknErrorNote();
		TBuf<0x100> buf;
		buf.Format(errorString, err );
		note->ExecuteLD( buf );
		}
    iDisplayMessage.Copy(KNullDesC);
    }

// ----------------------------------------------------
// CIMOpenAPIExampleAppUi::HandleMessageNewMsgFromModel(const TInternalMsgID aMsgID, 
//									            const TInt aIntegerInfo,
//									            const TDesC& /*aStringInfo*/ )
// Handles messages from model, except challenge, connection and error messages
// ----------------------------------------------------
//
void CIMOpenAPIExampleAppUi::HandleMessageFromModel(
									const TInternalMsgID aMsgID, 
									const TInt aIntegerInfo,
									const TDesC& /*aStringInfo*/ )
	{
	CAknInformationNote* note = new (ELeave) CAknInformationNote( EFalse );
	note->SetTimeout( CAknNoteDialog::ELongTimeout  );

	switch (aMsgID)
		{
        case ChallengeSent:
            StringLoader::Load ( iDisplayMessage, R_INFO_MSG_CHALLENGE_SENT );
            break;

		// handled by a seperate function
        case ChallengeRec:
 			break;

    	case PlayedIndexRec:
            iAppContainer->SetCellSign(Document().Model().GetOppSign(), aIntegerInfo);
            break;

		case StartGameRec:
            StringLoader::Load ( iDisplayMessage, R_INFO_MSG_GAME_STARTED );		    
			break;


		case EndGameRec:
            StringLoader::Load ( iDisplayMessage, R_INFO_MSG_GAME_ENDED );
            break;


		case ChallengeAcceptedRec:
            StringLoader::Load ( iDisplayMessage, R_INFO_MSG_CHALLENGE_ACCEPTED );
    		break;


		case ChallengeRejectedRec:
            StringLoader::Load ( iDisplayMessage, R_INFO_MSG_CHALLENGE_REJECTED );
			break;

		case AlreadyInGameRec:
            StringLoader::Load ( iDisplayMessage, R_INFO_MSG_ALREADY_IN_GAME );
    		break;

        case YouWon:
            StringLoader::Load ( iDisplayMessage, R_INFO_MSG_YOU_WON );
            break;

        case YouLost:
            StringLoader::Load ( iDisplayMessage, R_INFO_MSG_YOU_LOST );
            break;

        case GameDraw:
            StringLoader::Load ( iDisplayMessage, R_INFO_MSG_GAME_DRAW );
            break;

        case StartGameWarning:
            StringLoader::Load ( iDisplayMessage, R_INFO_MSG_START_GAME_WARNING );
            break;

        case EndChallengeRec:
            StringLoader::Load ( iDisplayMessage, R_INFO_MSG_CHALLENGE_ENDED );
            break;

		default:
            break;
		}
    if ( iDisplayMessage.Compare(KNullDesC) != 0)
        note->ExecuteLD( iDisplayMessage );
    else
    	delete note;

    iDisplayMessage.Copy(KNullDesC);
	}

// ----------------------------------------------------
// CIMOpenAPIExampleAppUi::HandleMessageIMConnectionFromModel(
//                                          const TInternalMsgID aMsgID, 
//										    const TInt aErrorCode)
// Model calls this function for connection and disconnection 
// ----------------------------------------------------
//
void CIMOpenAPIExampleAppUi::HandleMessageIMConnectionFromModel(
										const TInternalMsgID aMsgID, 
										const TInt aErrorCode	)

	{
	CAknInformationNote* note = new (ELeave) CAknInformationNote( EFalse );
	note->SetTimeout( CAknNoteDialog::ELongTimeout  );

	switch (aMsgID)
		{
		case Connected:
			iWaitDialog->ProcessFinishedL();
			if (aErrorCode != KErrNone)
				{
				StringLoader::Load ( iDisplayMessage, R_INFO_MSG_LOGIN_FAILED );
				StringLoader::Load ( iDisplayMessage, R_INFO_MSG_ERROR );
				iDisplayMessage.AppendNum(aErrorCode);
				}
			else 
				{
				StringLoader::Load ( iDisplayMessage, R_INFO_MSG_LOGIN_SUCCESS );
				}
			note->ExecuteLD( iDisplayMessage );
			break;

		case DisConnected:
			if (iWaitDialog)
				iWaitDialog->ProcessFinishedL();
			StringLoader::Load ( iDisplayMessage, R_INFO_MSG_DISCONNECTED );
			note->ExecuteLD( iDisplayMessage );
			break;

        default:
            break;
		}
	if(note)
		{
		delete note;
		note = NULL;
		}
    iDisplayMessage.Copy(KNullDesC);
	}

// ----------------------------------------------------
// CIMOpenAPIExampleDocument& CIMOpenAPIExampleAppUi::Document() const
// Provides a  pointer to document to be used by other functions
// ----------------------------------------------------
//
CIMOpenAPIExampleDocument& CIMOpenAPIExampleAppUi::Document() const
    {
    return *static_cast<CIMOpenAPIExampleDocument*>(CEikAppUi::Document());
    }


// ----------------------------------------------------
// CIMOpenAPIExampleAppUi::SetGameState(TGameState const aState)
// Sets game status, actually to the container
// ----------------------------------------------------
//
void CIMOpenAPIExampleAppUi::SetGameState(TGameState const aState)
	{
	iAppContainer->SetGameView(aState);
	}

// ----------------------------------------------------
// CIMOpenAPIExampleAppUi::GetGameState() const
// returns current Game state, it calls model for this
// ----------------------------------------------------
//
TGameState CIMOpenAPIExampleAppUi::GetGameState() const
	{
	return Document().Model().GetGameState();
	}

// ----------------------------------------------------
// CIMOpenAPIExampleAppUi::DisplayInfoMessage(TInt aResourceID)
// can display any info message to the user, by resource id
// ----------------------------------------------------
//
void CIMOpenAPIExampleAppUi::DisplayInfoMessage(TInt aResourceID)
    {
    CAknInformationNote* note = new (ELeave) CAknInformationNote( EFalse );
	note->SetTimeout( CAknNoteDialog::ELongTimeout  );
	StringLoader::Load ( iDisplayMessage, aResourceID );
	note->ExecuteLD( iDisplayMessage );
    iDisplayMessage.Copy(KNullDesC);
    }

// ----------------------------------------------------
// CIMOpenAPIExampleAppUi::Challenge()
// Handles Challenge, generates a text input for writing opponent id
// ----------------------------------------------------
//
TBool CIMOpenAPIExampleAppUi::Challenge()
    {
    TBuf<KMaxChallengeIDLength> oppnentID;
	CAknTextQueryDialog* OppIDQuery = CAknTextQueryDialog::NewL(oppnentID);
	CleanupStack::PushL(OppIDQuery);
	HBufC* prompt;
	prompt = StringLoader::LoadLC(R_INFO_MSG_OPP_ID_PROMPT);// Pushes prompt onto the Cleanup Stack.
	OppIDQuery->SetPromptL(*prompt);
	CleanupStack::PopAndDestroy(prompt);
	CleanupStack::Pop(OppIDQuery);
	if (OppIDQuery->ExecuteLD(R_DATA_QUERY))
	    {
		Document().Model().SetOppID(oppnentID);
		return ETrue;
		}
    return EFalse;
    }
    
// ----------------------------------------------------
// CIMOpenAPIExampleAppUi::LoginSettings()
// Display and accepts data from settings query dialogs
// ----------------------------------------------------
//
void CIMOpenAPIExampleAppUi::LoginSettings()
	{
	HBufC* prompt;
	
	// For User ID
	CAknTextQueryDialog* userIDQuery = CAknTextQueryDialog::NewL(iUserID);
	CleanupStack::PushL(userIDQuery);
	prompt = StringLoader::LoadLC(R_INFO_MSG_USER_ID);// Pushes prompt to Cleanup Stack.
	userIDQuery->SetPromptL(*prompt);
	CleanupStack::PopAndDestroy(prompt);
	CleanupStack::Pop(userIDQuery);
	if (!(userIDQuery->ExecuteLD(R_USER_ID_QUERY))) 
	    return; // return if user cancels
	
	//For Password
	CAknTextQueryDialog* passwordQuery = CAknTextQueryDialog::NewL(iPassword);
	CleanupStack::PushL(passwordQuery);
	prompt = StringLoader::LoadLC(R_INFO_MSG_PASSWORD);// Pushes prompt to Cleanup Stack.
	passwordQuery->SetPromptL(*prompt);
	CleanupStack::PopAndDestroy(prompt);
	CleanupStack::Pop(passwordQuery);
	if(!(passwordQuery->ExecuteLD(R_PASSWORD_QUERY))) 
	    return; // return if user cancels
	
	//For Server Address
	CAknTextQueryDialog* serverAddressQuery = CAknTextQueryDialog::NewL(iServerAddress);
	CleanupStack::PushL(serverAddressQuery);
	prompt = StringLoader::LoadLC(R_INFO_MSG_SERVER_ADDRESS);// Pushes prompt to Cleanup Stack.
	serverAddressQuery->SetPromptL(*prompt);
	CleanupStack::PopAndDestroy(prompt);
	CleanupStack::Pop(serverAddressQuery);
	if(!(serverAddressQuery->ExecuteLD(R_SERVER_ADDRESS_QUERY)))
	    return; // return if user cancels
	
	SelectAccessPoint();
    iLoginSettingsDone = ETrue;
	}

// ----------------------------------------------------
// CIMOpenAPIExampleAppUi::SelectAccessPoint()
// Display an available access points selection list box
// ----------------------------------------------------
//
void CIMOpenAPIExampleAppUi::SelectAccessPoint()
    {
	TConnectionPrefs prefs;

    iDlgSrv.Connect();

    prefs.iRank = 1;
    prefs.iDirection = ECommDbConnectionDirectionOutgoing;
    prefs.iBearerSet = ECommDbBearerCSD | ECommDbBearerWcdma | ECommDbBearerVirtual;

	TRAPD( err, iDlgSrv.IapConnection( iAPNumber, prefs, iStatus ) );
    User::LeaveIfError( err );
	
    User::WaitForRequest( iStatus );
    iDlgSrv.Close();
    }

// End of File  


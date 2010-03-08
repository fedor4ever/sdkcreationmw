/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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

#include "SIPExAppUi.h"
#include "SIPExGameView.h"
#include "SIPExLogView.h"
#include "SIPEx.hrh"
#include "SIPExAddrQueryDlg.h"
#include "SIPExIMDialog.h"
#include "SIPExDoc.h"
#include "SIPExGameContainer.h"
#include "SIPExLogContainer.h"
#include <SIPEx.rsg>
#include <eikenv.h>
#include <aknquerydialog.h>
#include <akntitle.h>
#include <stringloader.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIPExAppUi::CSIPExAppUi
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIPExAppUi::CSIPExAppUi()
: iState( CSIPExEngine::EIdle )
    {
    }

// -----------------------------------------------------------------------------
// CSIPExAppUi::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSIPExAppUi::ConstructL()
    {
    BaseConstructL( EAknEnableSkin );
    
    // Show tabs for main views from resources
    CEikStatusPane* sp = StatusPane();

    // Fetch pointer to the default navi pane control
    iNaviPane = static_cast< CAknNavigationControlContainer*>( 
    	sp->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );

    // Tabgroup has been read from resource and it was pushed to the navi 
    // pane. Get pointer to the navigation decorator with the 
    // ResourceDecorator() function. 
    // Application owns the decorator and it has responsibility to 
    // delete the object.
    iDecoratedTabGroup = iNaviPane->ResourceDecorator();
    if (iDecoratedTabGroup)
        {
        iTabGroup = static_cast< CAknTabGroup* >( 
      		iDecoratedTabGroup->DecoratedControl() );
        }

    iEngine = 
        ( static_cast< CSIPExDoc* >( Document() ) )->CreateEngineL( *this );
    
    // Receive view notifications from engine. Depending on currently 
    // active view, request (show message etc.) is either executed or 
    // stored for later usage
    iEngine->SetViewNotifier( *this );
    
    
    CSIPExGameView* gameview = CSIPExGameView::NewLC( *iEngine );
    AddViewL( gameview ); //transfer ownership to CAknViewAppUi
    CleanupStack::Pop( gameview );
    
    CSIPExLogView* logview = CSIPExLogView::NewLC( *iEngine );
    AddViewL( logview ); //transfer ownership to CAknViewAppUi
    CleanupStack::Pop( logview );

    SetDefaultViewL( *gameview );
    }

// -----------------------------------------------------------------------------
// CSIPExAppUi::~CSIPExAppUi
// Destructor
// -----------------------------------------------------------------------------
//
CSIPExAppUi::~CSIPExAppUi()
    {
    delete iDecoratedTabGroup;
    }
    
// -----------------------------------------------------------------------------
// CSIPExAppUi::HandleKeyEventL
// Handle certain key events (for changing views etc.)
// -----------------------------------------------------------------------------
//
TKeyResponse CSIPExAppUi::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/, 
    TEventCode /*aType*/)
    {
    // This would be good place for changing views according to left and right
    // key presses but those are reserved for game view for moving the piece
    return EKeyWasConsumed;
    }

// -----------------------------------------------------------------------------
// CSIPExAppUi::HandleCommandL
// User interaction handling. The views capture most of the events, but some
// might get pushed here
// -----------------------------------------------------------------------------
//
void CSIPExAppUi::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EEikCmdExit:
            {
            // If game is ongoing, notify opponent, ignore errors
            TRAPD( err, iEngine->EndGameL() );
            Exit();
            break;
            }

        case ECmdInviteForGame:
            {
            CSIPExAddrQueryDlg* dlg = CSIPExAddrQueryDlg::NewL( iAddress );
            if( dlg->ExecuteLD( R_INVITE_ADDRESS_DLG ) )
                {
                iEngine->InviteL( iAddress );
                }
            break;
            }     

        case ECmdEnableProfile:
            {
            iEngine->EnableProfileL();
            break;
            }

        case ECmdDisableProfile:
            {
            iEngine->DisableProfileL();
            break;
            }

        // Send instant message
        case ECmdSendIM:
            {
            TBuf< ESIPMaxMsgLen > msg;
            CSIPExIMDialog* dlg = CSIPExIMDialog::NewL( iAddress, msg );
            if( dlg->ExecuteLD( R_INSTANT_MESSAGE_DLG ) )
                {
                iEngine->SendInstantMsgL( iAddress, msg );
                }
            break;
            }
            
        // Change to Game View
        case ECmdChangeViewToGame:
            {
            if ( iTabGroup )
                {
                TInt active = iTabGroup->ActiveTabIndex();
                TInt count = iTabGroup->TabCount();
                if ( active > 0 )
                    {
                    active--;
                    iTabGroup->SetActiveTabByIndex( active );
                    // ActivateLocalViewL() is used to change the view. 
                    ActivateLocalViewL( 
                        TUid::Uid( iTabGroup->TabIdFromIndex( active ) ) );
                    }
                }
            break;
            }
            
        // Change to Log View
        case ECmdChangeViewToLog:
            {
            if ( iTabGroup )
                {
                TInt active = iTabGroup->ActiveTabIndex();
                TInt count = iTabGroup->TabCount();
                if ( (active + 1) < count )
                    {
                    active++;
                    iTabGroup->SetActiveTabByIndex( active );
                    // ActivateLocalViewL() is used to change the view. 
                    ActivateLocalViewL( 
                        TUid::Uid( iTabGroup->TabIdFromIndex( active ) ) );
                    }
                }    
            break;
            }
            

        // End game
        case ECmdEndGame:
            {
            iEngine->EndGameL();
            break;
            }

        default:
            {
            
            // Do nothing, no-one will handle it
            // No use of leaving though, because this isn't an error - only
            // an unhandled command
            break;
            }
        }
    }
	
// -----------------------------------------------------------------------------
// CSIPExAppUi::GameStateChanged
// From MSIPExGameObserver.
// Game state changed. Then the game should be redraw.
// -----------------------------------------------------------------------------
//
void CSIPExAppUi::GameStateChanged( CSIPExEngine::TEngineState aState )
    {
    iState = aState;
    
    if ( GameViewActive() )
        {
        GameContainer()->DrawDeferred();
        }
    }
    

// -----------------------------------------------------------------------------
// CSIPExAppUi::AcceptInvitationL
// From MSIPExGameObserver.
// Show confirmation dialog with summoner's address.
// If accepted return ETrue else EFalse.
// -----------------------------------------------------------------------------
//
TBool CSIPExAppUi::AcceptInvitationL( const TDesC& aFrom )
    {
    HBufC* buf = 
        StringLoader::LoadLC( R_SIP_EX_ACCEPT_INVITATION, aFrom, iEikonEnv );
    TBool retVal( EFalse );

    CAknQueryDialog* query = CAknQueryDialog::NewL();
    if( query->ExecuteLD( R_ACCEPT_INVITATION_DLG, *buf ) )
        {
        retVal = ETrue;
        }

    CleanupStack::PopAndDestroy( buf );
    return retVal;
    }

// -----------------------------------------------------------------------------
// CSIPExAppUi::IMReceivedL
// From MSIPExGameObserver.
// Show the received instant message in info editor.
// -----------------------------------------------------------------------------
//
void CSIPExAppUi::IMReceivedL( const TDesC8& aFrom, const TDesC8& aMsg )
    {
    HBufC* from = HBufC::NewLC( aFrom.Length() );
    from->Des().Copy( aFrom );

    HBufC* msg = HBufC::NewLC( aMsg.Length() );
    msg->Des().Copy( aMsg );

    CDesCArrayFlat* strings = new (ELeave) CDesCArrayFlat( 2 );
    CleanupStack::PushL( strings );
    strings->AppendL( *from );
    strings->AppendL( *msg );
    HBufC* buf = 
            StringLoader::LoadL( R_SIP_EX_IM_RECEIVED, *strings, iEikonEnv );
            
    CleanupStack::PopAndDestroy( strings );
    CleanupStack::PushL( buf );
    ShowInfoL( *buf );
    CleanupStack::PopAndDestroy( 3 );
    }

// -----------------------------------------------------------------------------
// CSIPExAppUi::ShowStatusInfoL
// -----------------------------------------------------------------------------
//
void CSIPExAppUi::ShowStatusInfoL( const TDesC& aText )
    {
    // Show status info in title pane (could be additionally 
    // shown in some popup note)
    //
    TUid titlePaneUid = { EEikStatusPaneUidTitle };
    
    CEikStatusPane* statusPane = StatusPane();
    
    CEikStatusPaneBase::TPaneCapabilities subPane = 
                                statusPane->PaneCapabilities( titlePaneUid );
    
    
    HBufC* text = StringLoader::LoadLC( R_SIP_EX_TITLE_TXT, aText, iEikonEnv );
    
    if ( subPane.IsPresent() && subPane.IsAppOwned() )
        {
        CAknTitlePane* titlePane = 
            static_cast< CAknTitlePane* >( statusPane->ControlL( titlePaneUid ) );
        
        titlePane->SetText( text );
        
        CleanupStack::Pop( text );
        }
    else
        {
        CleanupStack::PopAndDestroy( text );
        }
    }
    
// -----------------------------------------------------------------------------
// CSIPExAppUi::ShowInfoL
// -----------------------------------------------------------------------------
//
void CSIPExAppUi::ShowInfoL( const TDesC& aText )
    {
    static_cast< CSIPExDoc* >( Document() )->StoreInfoTextL( aText );
    if( !GameViewActive() )
        {
        LogContainer()->ShowInfoL();
        } 
    }
    
// -----------------------------------------------------------------------------
// CSIPExAppUi::ShowInfoL
// -----------------------------------------------------------------------------
//
void CSIPExAppUi::ShowInfoL( const TDesC8& aText )
    {
    HBufC* buf = HBufC::NewLC( aText.Length() );
    buf->Des().Copy( aText );
    static_cast< CSIPExDoc* >( Document() )->StoreInfoTextL( *buf );
    CleanupStack::PopAndDestroy( buf );
    
    if( !GameViewActive() )
        {
        LogContainer()->ShowInfoL();
        } 
    }
        
// -----------------------------------------------------------------------------
// CSIPExAppUi::EngineState
// -----------------------------------------------------------------------------
//
const CSIPExEngine::TEngineState CSIPExAppUi::EngineState()
    {
    return iState;
    }

// -----------------------------------------------------------------------------
// CSIPExAppUi::GameViewActive
// -----------------------------------------------------------------------------
//    
TBool CSIPExAppUi::GameViewActive()
    {
    return IsViewActive( KGameViewId );
    }
    
// -----------------------------------------------------------------------------
// CSIPExAppUi::LogViewActive
// -----------------------------------------------------------------------------
//    
TBool CSIPExAppUi::LogViewActive()
    {
    return IsViewActive( KLogViewId );
    }
   
// -----------------------------------------------------------------------------
// CSIPExAppUi::IsViewActive
// -----------------------------------------------------------------------------
// 
TBool CSIPExAppUi::IsViewActive( TUid aViewUid )
	{
	TInt active = iTabGroup->ActiveTabIndex();
    return ( TUid::Uid( iTabGroup->TabIdFromIndex( active ) ) == aViewUid );
	}
    
// -----------------------------------------------------------------------------
// CSIPExAppUi::GameContainer
// -----------------------------------------------------------------------------
//   
CSIPExGameContainer* CSIPExAppUi::GameContainer()
    {
    return static_cast< CSIPExGameView* >( View( KGameViewId ) )->Container();
    }
    
// -----------------------------------------------------------------------------
// CSIPExAppUi::LogContainer
// -----------------------------------------------------------------------------
//   
CSIPExLogContainer* CSIPExAppUi::LogContainer()
    {
    return static_cast< CSIPExLogView* >( View( KLogViewId ) )->Container();
    }
    
//  End of File

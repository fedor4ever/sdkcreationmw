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
#include "ChatAppui.h"
#include "ChatContainer.h" 
#include <ChatEx.rsg>
#include "Chat.hrh"

#include <avkon.hrh>
#include <eikmenup.h>
#include <aknnotewrappers.h>
#include <stringloader.h>

#include "Chat.pan"

#include "ChatContainer.h"
#include "ChatInet.h"
#include "ChatBt.h"

#include "ChatProtocolConstants.h"


// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------
// CChatAppUi::ConstructL()
// 
// ----------------------------------------------------------
//
void CChatAppUi::ConstructL()
    {
    BaseConstructL( CAknAppUi::EAknEnableSkin );

    iAppContainer = CChatContainer::NewL( ClientRect() );    
    
    // so view can update scroll bars
    iAppContainer->SetMopParent( this ); 

    AddToStackL( iAppContainer );

    iChatBt = CChatBt::NewL( *iAppContainer );
    iChatInet = CChatInet::NewL( *iAppContainer );

    // check whether BT is available or not

    RSdp sdpSession;

    if ( sdpSession.Connect() == KErrNone )
        {
        sdpSession.Close();
    
        iBtAvailable = ETrue;
        }
    else
        {
        iBtAvailable = EFalse;
        }
    }

// ----------------------------------------------------
// CChatAppUi::~CChatAppUi()
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CChatAppUi::~CChatAppUi()
    {
    delete iChatBt;
    iChatBt = NULL;
    
    delete iChatInet;
    iChatInet= NULL;
    
    if ( iAppContainer )
        {
        RemoveFromStack( iAppContainer );
        delete iAppContainer;
        iAppContainer = NULL;
        }
   }

// ------------------------------------------------------------------------------
// CChatAppUi::DynInitMenuPaneL(TInt aResourceId,CEikMenuPane* aMenuPane)
//  This function is called by the EIKON framework just before it displays
//  a menu pane. Its default implementation is empty, and by overriding it,
//  the application can set the state of menu items dynamically according
//  to the state of application data.
// ------------------------------------------------------------------------------
//
void CChatAppUi::DynInitMenuPaneL(
    TInt aResourceId,CEikMenuPane* aMenuPane)
    {
    if ( aResourceId == R_CHAT_MENU )
        {
        if ( iChatBt->IsConnected() )
            {
            aMenuPane->SetItemDimmed( EChatBtChat, ETrue );
            aMenuPane->SetItemDimmed( EChatIpChat, ETrue );
            aMenuPane->SetItemDimmed( EChatStopBtChat, !iChatBt->Server() );
            aMenuPane->SetItemDimmed( EChatStopIpChat, ETrue );
            aMenuPane->SetItemDimmed( EChatDisconnect, iChatBt->Server() );
            aMenuPane->SetItemDimmed( EChatSendMessage, 
                !iChatBt->IsReadyToSendMessage() );
            }  
              
        else if ( iChatInet->IsConnected() )
            {
            aMenuPane->SetItemDimmed( EChatBtChat, ETrue );
            aMenuPane->SetItemDimmed( EChatIpChat, ETrue );
            aMenuPane->SetItemDimmed( EChatStopBtChat, ETrue );
            aMenuPane->SetItemDimmed( EChatStopIpChat, !iChatInet->Server() );
            aMenuPane->SetItemDimmed( EChatDisconnect, iChatInet->Server() );
            aMenuPane->SetItemDimmed( EChatSendMessage,
                !iChatInet->IsReadyToSendMessage() );
            }    

        else if ( iChatBt->IsConnecting() )
            {
            aMenuPane->SetItemDimmed( EChatBtChat, ETrue );
            aMenuPane->SetItemDimmed( EChatIpChat, ETrue );
            aMenuPane->SetItemDimmed( EChatStopIpChat, ETrue );
            aMenuPane->SetItemDimmed( EChatStopBtChat, EFalse );
            aMenuPane->SetItemDimmed( EChatDisconnect, ETrue );
            aMenuPane->SetItemDimmed( EChatSendMessage, ETrue );
            }
            
       else if ( iChatInet->IsConnecting() )
            {
            aMenuPane->SetItemDimmed( EChatBtChat, ETrue );
            aMenuPane->SetItemDimmed( EChatIpChat, ETrue );
            aMenuPane->SetItemDimmed( EChatStopBtChat, ETrue );
            aMenuPane->SetItemDimmed( EChatStopIpChat, EFalse );
            aMenuPane->SetItemDimmed( EChatDisconnect, ETrue );
            aMenuPane->SetItemDimmed( EChatSendMessage, ETrue );
            }     
            
        else if ( iChatBt->IsSendingMessage() )
            {
            aMenuPane->SetItemDimmed( EChatBtChat, ETrue );
            aMenuPane->SetItemDimmed( EChatIpChat, ETrue );
            aMenuPane->SetItemDimmed( EChatStopBtChat, ETrue );
            aMenuPane->SetItemDimmed( EChatStopIpChat, ETrue );
            aMenuPane->SetItemDimmed( EChatDisconnect, EFalse );
            aMenuPane->SetItemDimmed( EChatSendMessage, ETrue );
            }
            
      else if ( iChatInet->IsSendingMessage() )
            {
            aMenuPane->SetItemDimmed( EChatBtChat, ETrue );
            aMenuPane->SetItemDimmed( EChatIpChat, ETrue );
            aMenuPane->SetItemDimmed( EChatStopBtChat, ETrue );
            aMenuPane->SetItemDimmed( EChatStopIpChat, ETrue );
            aMenuPane->SetItemDimmed( EChatDisconnect, iChatInet->Server() );
            aMenuPane->SetItemDimmed( EChatSendMessage, ETrue );
            }      
            
        else // both the server and the client are not connected
            {
            aMenuPane->SetItemDimmed( EChatBtChat, EFalse );
            aMenuPane->SetItemDimmed( EChatIpChat, EFalse );
            aMenuPane->SetItemDimmed( EChatStopBtChat, ETrue );
            aMenuPane->SetItemDimmed( EChatStopIpChat, ETrue );
            aMenuPane->SetItemDimmed( EChatDisconnect, ETrue );
            aMenuPane->SetItemDimmed( EChatSendMessage, ETrue );
            }
        aMenuPane->SetItemDimmed( EChatClearList, 
            !iAppContainer->ContainsEntries() );
        }
    }
    
// ----------------------------------------------------
// CChatAppUi::HandleKeyEventL(
//     const TKeyEvent& aKeyEvent,TEventCode /*aType*/)
// takes care of key event handling
// ----------------------------------------------------
//
TKeyResponse CChatAppUi::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/,TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }

// ----------------------------------------------------
// CChatAppUi::HandleCommandL(TInt aCommand)
// takes care of command handling
// ----------------------------------------------------
//
void CChatAppUi::HandleCommandL(TInt aCommand)
    {
    TBuf<KChatTextBufLength> text;

      switch ( aCommand )
          {
          
          case EAknSoftkeyExit:
          case EEikCmdExit:          
              {
              if ( iChatInet->IsConnected() || iChatInet->IsConnecting() )
                  {
                  if ( iChatInet->Server() )
                      {
                  	  iChatInet->Stop();
                      }
                  else
                      {
                      iChatInet->DisconnectL();
                      }           
                  }
              if ( iChatBt->IsConnected() || iChatBt->IsConnecting()  )
                  {
                  if ( iChatBt->Server() )
                      {
                  	  iChatBt->StopL();
                      }
                  else
                      {
                      iChatBt->DisconnectL();
                      }
                  }
 
              Exit();
              break;
              }
              
          case EChatBtChat:
              {
              break;
              }
              
          case EChatIpChat:
              {
              break;
              }
              
          case EChatStartChat:
              {
              if ( !iBtAvailable  )
                  {
                  ShowBTNotAvailableNoteL();
                  }
              else 
                  {
                  iChatBt->StartL();
                  }
              break;
              }
              
          case EChatConnect:
              {
              if ( !iBtAvailable  )
                  {
                  ShowBTNotAvailableNoteL();
                  }
              else 
                  {
                  iChatBt->ConnectL();  
                  }
              break;
              }
              
          case EChatStartIpChat:
              {
              iChatInet->StartL();
              break;
              }
              
          case EChatIpConnect:
              {
              iAppContainer->ShowDataQueryL( 
                  R_CHAT_SERVERDATA_QUERY, 
                  0, 
                  0, 
                  KChatTextBufLength, 
                  EFalse, 
                  text );

              if ( text.Length() != 0 )
                  {
                  iChatInet->ConnectL( text );
                  }
              break;
              }
              
          case EChatDisconnect:
              {
              if ( iChatInet->IsConnected() )
                  {
                  iChatInet->DisconnectL(); 
                  }
              if ( iChatBt->IsConnected() )
                  {
                  iChatBt->DisconnectL(); 
                  }
                break;
              }
              
          case EChatStopBtChat:
              {
              if ( !iBtAvailable  )
                  {
                  ShowBTNotAvailableNoteL();
                  }
              else 
                  {
                  iChatBt->StopL(); 
                  }
              break;
              }
              
          case EChatStopIpChat:
              {
              iChatInet->Stop();
              break;
              } 
                 
          case EChatSendMessage:
              {
              iAppContainer->ShowDataQueryL( 
                  R_CHAT_DATA_QUERY, 
                  0, 
                  0, 
                  KChatTextBufLength, 
                  ETrue, 
                  text );
              
              if ( iChatInet->IsConnected() )
                  {
                  iChatInet->SendMessageL( text );
                  }
              if( iChatBt->IsConnected() )
                  {
                  iChatBt->SendMessageL( text );
                  }
              break;
              }
              
           case EChatClearList:
               {
               iAppContainer->ClearMessageListL();
               break;
               } 
              
          default:
              break;      
          }
    }

// -----------------------------------------------------------------------------
// CChatAppUi::ShowBTNotAvailableNoteL()
// Show note if BT is not available 
// -----------------------------------------------------------------------------
//
void CChatAppUi::ShowBTNotAvailableNoteL()
    {
    // Load a string from the resource file and display it
    HBufC* textResource = StringLoader::LoadLC( R_CHAT_NO_BT );
    CAknErrorNote* errorNote;

    errorNote = new ( ELeave ) CAknErrorNote;

    // Show the information Note with
    // textResource loaded with StringLoader.
    errorNote->ExecuteLD( *textResource);

    // Pop HBuf from CleanUpStack and Destroy it.
    CleanupStack::PopAndDestroy( textResource );
    }


// -----------------------------------------------------------------------------
// CChatAppUi::HandleResourceChangeL( TInt aType )
// Called by framework when layout is changed.
// -----------------------------------------------------------------------------
//
void CChatAppUi::HandleResourceChangeL( TInt aType )
    {
    CAknAppUi::HandleResourceChangeL( aType );

    // ADDED FOR SCALABLE UI SUPPORT
    // *****************************
    if ( aType==KEikDynamicLayoutVariantSwitch )
        {
        iAppContainer->SetRect( ClientRect() );
        }
    //Controls derived from CCoeControl, handled in container class
    iAppContainer->HandleResourceChange( aType );
    }


// End of File  

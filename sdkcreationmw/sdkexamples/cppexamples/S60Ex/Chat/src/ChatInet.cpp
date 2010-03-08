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
#include <StringLoader.h>
#include <ChatEx.rsg>
#include <coemain.h>

#include "ChatInet.h"
#include "Chat.pan"
#include "Log.h"
#include "es_enum.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CChatInet::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CChatInet* CChatInet::NewL( MLog& aLog )
    {
    CChatInet* self = NewLC( aLog );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CChatInet::NewLC()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CChatInet* CChatInet::NewLC( MLog& aLog )
    {
    CChatInet* self = new ( ELeave ) CChatInet( aLog );
    CleanupStack::PushL( self );
    self->ConstructL ();
    return self;

    }

// ----------------------------------------------------------------------------
// CChatInet::CChatInet()
// Constructor.
// ----------------------------------------------------------------------------
//
CChatInet::CChatInet( MLog& aLog ) : 
    CChatBase( aLog ),
    iLog( aLog ),
    iPort( KDefaultPortNumber )
    {
    SetState( EDisconnected );
    }

// ----------------------------------------------------------------------------
// CChatInet::~CChatInet()
// Destructor.
// ----------------------------------------------------------------------------
//
CChatInet::~CChatInet()
    {
    // Close() will wait forever for Read to complete
    if ( State() == EConnected )
        {
        if ( iActiveSocket )
            {
            iActiveSocket->CancelRead();
            }
        }
    Cancel();
    iAcceptedSocket.Close();
    iSocket.Close();
    iSocketServer.Close();

    delete iMessage;
    iMessage = NULL;
    }

// ----------------------------------------------------------------------------
// CChatInet::ConstructL()
// Perform second phase construction of this object.
// ----------------------------------------------------------------------------
//
void CChatInet::ConstructL()
    {
    User::LeaveIfError( iSocketServer.Connect() );
    }

// ----------------------------------------------------------------------------
// CChatInet::DoCancel()
// Cancel any outstanding requests.
// ----------------------------------------------------------------------------
//
void CChatInet::DoCancel()
    {
    if ( State() == EConnected )
        {
        if ( iActiveSocket )
            {
            iActiveSocket->CancelRead();
            }
        }
        SetState( EDisconnected );
    }

// ----------------------------------------------------------------------------
// CChatInet::RunL()
// Respond to an event.
// ----------------------------------------------------------------------------
//
void CChatInet::RunL()
    {
    HBufC* textResource = NULL;
    TBuf<KMaxMarkLen> mark;
  
    if ( ( iStatus == KErrDisconnected ) || ( iStatus == KErrEof ) )
        {
        // Disconnected so go back to listening
        HBufC* strDisconnected = StringLoader
            ::LoadLC ( R_CHAT_STR_DISCONNECTED );
        iLog.LogL( *strDisconnected );
        CleanupStack::PopAndDestroy( strDisconnected );
        Stop();
        return;
        }
    else if ( iStatus == KErrAbort )
        {
        HBufC* strDisconnected = StringLoader
            ::LoadLC ( R_CHAT_STR_DISCONNECTED );
        iLog.LogL( *strDisconnected );
        CleanupStack::PopAndDestroy( strDisconnected );
        Stop();
        return;
        }
        
    else if ( iStatus != KErrNone )
        {
        switch ( State() )
            {
            case EGettingConnection:
                textResource = StringLoader
                    ::LoadLC( R_CHAT_ERR_CONNECTION_ERROR );
                iLog.LogL( *textResource, iStatus.Int() );
                DisconnectFromServerL();
                CleanupStack::PopAndDestroy( textResource );
                SetState( EDisconnecting );
                break;
              
            case EConnected:
                textResource = StringLoader
                    ::LoadLC( R_CHAT_ERR_LOST_CONNECTION );
                iLog.LogL( *textResource, iStatus.Int() );
                DisconnectFromServerL();
                CleanupStack::PopAndDestroy( textResource );
                SetState( EDisconnecting );
                break;
                
            case ESendingMessage:
                textResource = StringLoader
                    ::LoadLC( R_CHAT_ERR_MESSAGE_FAILED );
                iLog.LogL( *textResource, iStatus.Int() );
                CleanupStack::PopAndDestroy( textResource );
                DisconnectFromServerL();
                SetState( EDisconnecting );
                break;
                
            case EDisconnecting:
                if ( iStatus == KErrDisconnected )
                    {
                    textResource = StringLoader
                        ::LoadLC( R_CHAT_DISCONNECT_COMPLETE );
                    iLog.LogL( *textResource, iStatus.Int() );
                    CleanupStack::PopAndDestroy( textResource );
                    Stop();
                    SetState( EDisconnected );
                    }
                else
                    {
                    textResource = StringLoader
                        ::LoadLC( R_CHAT_ERR_FAILED_TO_DISCONNECT );
                    iLog.LogL( *textResource, iStatus.Int() );
                    CleanupStack::PopAndDestroy( textResource );
                    }
                    break;
                    
            case EDisconnected:
                    textResource = StringLoader
                        ::LoadLC( R_CHAT_DISCONNECT_COMPLETE );
                    iLog.LogL( *textResource, iStatus.Int() );
                    CleanupStack::PopAndDestroy( textResource );
                break;
                        
            default:
                break;
            }
        }
    else 
        {
        switch ( State() )
            {
   
            case EGettingConnection:
                textResource = StringLoader
                    ::LoadLC( R_CHAT_CONNECTED );
                iLog.LogL( *textResource );
                CleanupStack::PopAndDestroy( textResource );

                SetState( EConnected );
                RequestData();
                break;
              
            case EConnected:
              
              mark.Append( '>' );
                textResource = HBufC::NewLC( iBuffer.Length() );
              textResource->Des().Copy( iBuffer );
              iLog.LogL( *textResource, mark );
              iBuffer.Zero();
              CleanupStack::PopAndDestroy( textResource );
                RequestData();
                break;
                
            case ESendingMessage:
                SetState( EConnected );
                RequestData();
                break;
                
            case EDisconnecting:
                textResource = StringLoader
                    ::LoadLC( R_CHAT_DISCONNECT_COMPLETE );
                iLog.LogL( *textResource );
                CleanupStack::PopAndDestroy ( textResource );

                Stop();
                SetState( EDisconnected );
                break;
            
            case EDisconnected:
                break;
                
            default:
                Panic( EChatInvalidLogicState );
                break;
            };
        }
    }

// ----------------------------------------------------------------------------
// CChatInet::ConnectL()
// Connect to a service.
// ----------------------------------------------------------------------------
//
void CChatInet::ConnectL( const TDesC& aAddress )
    {
    if ( State() == EDisconnected && !IsActive() )
        {
        SetState( EGettingConnection );
        ConnectToServerL( aAddress );
        }
    else
        {
        HBufC* errClientBusy = StringLoader::LoadLC ( R_CHAT_CLIENT_BUSY );
        iLog.LogL( *errClientBusy );
        CleanupStack::PopAndDestroy( errClientBusy );

        User::Leave( KErrInUse );
        }
    }

// ----------------------------------------------------------------------------
// CChatInet::DisconnectL()
// Disconnects from the remote machine.
// ----------------------------------------------------------------------------
//
void CChatInet::DisconnectL()
    {
    if ( ( State() == EConnected )||( State() == ESendingMessage ) )
        {
        DisconnectFromServerL();
        SetState( EDisconnecting );
        }
    else
        {
        HBufC* errNoConn = StringLoader::LoadLC ( R_CHAT_ERR_NO_CONN );
        iLog.LogL( *errNoConn );
        CleanupStack::PopAndDestroy( errNoConn );
        User::Leave( KErrDisconnected );
        }
    }

// ----------------------------------------------------------------------------
// CChatInet::DisconnectFromServerL()
// Disconnects from the service
// ----------------------------------------------------------------------------
//
void CChatInet::DisconnectFromServerL()
    {
    // Terminate all operations
    iSocket.CancelAll();
    Cancel();

    HBufC* strReleasingConn = StringLoader
        ::LoadLC ( R_CHAT_STR_RELEASING_CONN );
    iLog.LogL( *strReleasingConn );
    CleanupStack::PopAndDestroy( strReleasingConn );
    iSocket.Shutdown( RSocket::ENormal, iStatus );
    SetActive();
    }

// ----------------------------------------------------------------------------
// CChatInet::ConnectToServerL()
// Connect to the server.
// ----------------------------------------------------------------------------
//
void CChatInet::ConnectToServerL( const TDesC& aAddress )
    {
    HBufC* strConnecting = StringLoader
        ::LoadLC ( R_CHAT_STR_CONNECTING );
    iLog.LogL( *strConnecting );
    CleanupStack::PopAndDestroy( strConnecting );
    
    TInetAddr addr;
  if ( addr.Input( aAddress ) == KErrNone )
        {
        // server name is already a valid ip address
        ConnectL( addr.Address() );
        }
    else 
        {
        // Initiate DNS
        User::LeaveIfError( iResolver.Open( iSocketServer,
                                            KAfInet,
                                            KProtocolInetTcp ) );
        // DNS request for name resolution
        iResolver.GetByName( aAddress, iNameEntry, iStatus );
        
        TRequestStatus waitStatus = KRequestPending;
        RTimer timer;
        timer.CreateLocal();
    
        timer.After( waitStatus, KChatTimeOut );
     
        User::WaitForRequest( iStatus, waitStatus );
        
        if ( iStatus == KRequestPending )
            {
            iResolver.Cancel();
            
            User::WaitForRequest( iStatus );
            iResolver.Close();
            // Operation did not complete in time.
            HBufC* strConnecting = StringLoader
                ::LoadLC ( R_CHAT_ERR_TIMEOUT );
            iLog.LogL( *strConnecting );
            CleanupStack::PopAndDestroy( strConnecting );
            SetState( EDisconnected );
            timer.Close();
            User::Leave( KErrTimedOut );

            }
        else
            {
            // DNS look up successful
        	iNameRecord = iNameEntry();
        	TBuf<KIpAddressLen> ipAddr;
        	TInetAddr::Cast( iNameRecord.iAddr ).Output( ipAddr );
        	ConnectL( TInetAddr::Cast( iNameRecord.iAddr ).Address() );
        	
            timer.Cancel();
            User::WaitForRequest( waitStatus );
            timer.Close();
            iResolver.Close();
            }

      }

  #ifdef __WINS__
      User::After( 1 );     // Fix to allow emulator client to connect to server
  #endif
  
    SetState( EGettingConnection );

    SetActive();
    }


// -----------------------------------------------------------------------------
// CChatInet::ConnectL()
// Initiates a connect operation on a socket.
// -----------------------------------------------------------------------------
//
void CChatInet::ConnectL( TUint32 aAddr )
    {
    iActiveSocket = &iSocket;
    iAddress.SetPort( iPort );
    iAddress.SetAddress( aAddr );
    iAddress.SetFamily( KAfInet );
        
    // Open a TCP socket
    User::LeaveIfError( iSocket.Open( iSocketServer,
        KAfInet,
        KSockStream,
        KProtocolInetTcp ) );

    // Initiate socket connection
    iSocket.Connect( iAddress, iStatus );
    }


// ----------------------------------------------------------------------------
// CChatInet::SendMessageL()
// Send a message to a service on a remote machine.
// ----------------------------------------------------------------------------
//
void CChatInet::SendMessageL( const TDesC& aText )
    {
    if ( State() != EConnected )
        {
        User::Leave( KErrDisconnected );
        }

    // stop reading socket
    if ( iActiveSocket )
        {
        iActiveSocket->CancelRead();  
        }

    if ( IsActive() )
        {
        Cancel();
        }
      
    TBufC<KChatTextBufLength> message ( aText );
    SetState( ESendingMessage );
    HBufC* tempString = HBufC::NewL( message.Length() );
    tempString->Des().Copy( message );
    
    iMessage = HBufC8::NewL(tempString->Length());
    iMessage->Des().Copy( *tempString );
    
    // write message to socket
    if ( iActiveSocket )
        {
        iActiveSocket->Write( *iMessage, iStatus );
        }
    delete tempString;
    SetActive();
    }


// ----------------------------------------------------------------------------
// CChatInet::StartL()
// Starts the server.
// ----------------------------------------------------------------------------
//
void CChatInet::StartL()
    {
    if ( State() != EDisconnected )
        {
        User::Leave( KErrInUse );
        }
    
    User::LeaveIfError( iConnection.Open( iSocketServer, KAfInet) );
    
    User::LeaveIfError( iConnection.Start() );
      
    TInt result( 0 );
    
    result = iSocket.Open( iSocketServer, 
        KAfInet, 
        KSockStream, 
        KProtocolInetTcp );
      
    if ( result != KErrNone )
        {
        iSocketServer.Close();
        User::Leave( result ); 
        }

    TInetAddr addr;
    iAddress.SetPort( iPort );
    iAddress.SetFamily( KAfInet );

    // bind
    User::LeaveIfError( iSocket.Bind( iAddress ) );
    
    // listen
    User::LeaveIfError( iSocket.Listen( KListeningQueSize ) );

    // close old connection - if any
    iAcceptedSocket.Close();

    // Open abstract socket
    User::LeaveIfError( iAcceptedSocket.Open( iSocketServer ) );  
  
    iActiveSocket = &iAcceptedSocket;
  
    // Set the Active Object's State to Connecting indicated.
    SetState( EGettingConnection );

    iSocket.Accept( iAcceptedSocket, iStatus );

    SetServer( ETrue );

    // Set the Active Object Active again,
    SetActive();
   
    }


// ----------------------------------------------------------------------------
// CChatInet::Stop()
// Stops the server.
// ----------------------------------------------------------------------------
//
void CChatInet::Stop()
    {
    if ( State() != EDisconnected )
        {
        // Close() will wait forever for Read to complete
        if ( State() == EConnected )
            {
            if ( iActiveSocket )
                {
                iActiveSocket->CancelRead();
                }
            }
        iAcceptedSocket.Close();
        iSocket.Close();
        }
        SetState( EDisconnected );
    }

// ----------------------------------------------------------------------------
// CChatInet::RequestData()
// Request data from the client.
// ----------------------------------------------------------------------------
//
void CChatInet::RequestData()
    {
    if ( iActiveSocket )
        {
        iActiveSocket->RecvOneOrMore( iBuffer, 0, iStatus, iLen );
        }
    SetActive();
    }


// End of File

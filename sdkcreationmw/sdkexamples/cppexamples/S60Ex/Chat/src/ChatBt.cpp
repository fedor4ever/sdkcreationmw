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
#include <coemain.h>
#include <ChatEx.rsg>
#include "ChatServiceAdvertiser.h"
#include "ChatBt.h"
#include "ChatServiceSearcher.h"
#include "Chat.pan"
#include "Log.h"

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CChatBt::NewL()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CChatBt* CChatBt::NewL( MLog& aLog )
    {
    CChatBt* self = NewLC( aLog );
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CChatBt::NewLC()
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CChatBt* CChatBt::NewLC( MLog& aLog )
    {
    CChatBt* self = new ( ELeave ) CChatBt( aLog );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CChatBt::CChatBt()
// Constructor.
// ----------------------------------------------------------------------------
//
CChatBt::CChatBt( MLog& aLog ) : 
    CChatBase( aLog ),
    iLog( aLog )
    {
    SetState( EWaitingToGetDevice );
    }

// ----------------------------------------------------------------------------
// CChatBt::~CChatBt()
// Destructor.
// ----------------------------------------------------------------------------
//
CChatBt::~CChatBt()
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

    iSocket.Close();
    iAcceptedSocket.Close();
    iSocketServer.Close();

    delete iMessage;
    iMessage = NULL;

    delete iServiceSearcher;
    iServiceSearcher = NULL;
    
    delete iAdvertiser;
    iAdvertiser = NULL;
    }

// ----------------------------------------------------------------------------
// CChatBt::ConstructL()
// Perform second phase construction of this object.
// ----------------------------------------------------------------------------
//
void CChatBt::ConstructL()
    {
    iServiceSearcher = CChatServiceSearcher::NewL( iLog );
    iAdvertiser = CChatServiceAdvertiser::NewL();
    User::LeaveIfError( iSocketServer.Connect() );
    }

// ----------------------------------------------------------------------------
// CChatBt::DoCancel()
// Cancel any outstanding requests.
// ----------------------------------------------------------------------------
//
void CChatBt::DoCancel()
    {
    // no implementation required
    }

// ----------------------------------------------------------------------------
// CChatBt::RunL()
// Respond to an event.
// ----------------------------------------------------------------------------
//
void CChatBt::RunL()
    {

    HBufC* textResource = NULL;
    TBuf<KMaxMarkLen> mark;
  
    if ( iStatus == KErrDisconnected )
        {
        // Disconnected
        HBufC* strDisconnected = StringLoader
            ::LoadLC ( R_CHAT_STR_DISCONNECTED );
        iLog.LogL( *strDisconnected );
        CleanupStack::PopAndDestroy( strDisconnected );
        StopL();
        return;
        }
        
    else if ( iStatus == KErrAbort )
        {
        HBufC* strDisconnected = StringLoader
            ::LoadLC ( R_CHAT_STR_DISCONNECTED );
        iLog.LogL( *strDisconnected );
        CleanupStack::PopAndDestroy( strDisconnected );
        StopL();
        return;
        }
  
    else if ( iStatus != KErrNone )
        {
        switch ( State() )
            {
            case EGettingDevice:
                if ( iStatus == KErrCancel )
                    {
                    textResource = StringLoader
                        ::LoadLC( R_CHAT_ERR_NO_DEVICE_SELECTED );
                    iLog.LogL( *textResource );
                    CleanupStack::PopAndDestroy( textResource );
                    }
                SetState( EWaitingToGetDevice );
                break;
                
            case EGettingService:
            
            case EGettingConnection:
                textResource = StringLoader
                    ::LoadLC( R_CHAT_ERR_CONNECTION_ERROR );
                iLog.LogL( *textResource, iStatus.Int() );
                CleanupStack::PopAndDestroy( textResource );
                SetState( EWaitingToGetDevice );
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

                    StopL();
                    SetState( EWaitingToGetDevice );
                    }
                else
                    {
                    textResource = StringLoader
                        ::LoadLC( R_CHAT_ERR_FAILED_TO_DISCONNECT );
                    iLog.LogL( *textResource, iStatus.Int() );
                    CleanupStack::PopAndDestroy( textResource );

                    Panic( EChatUnableToDisconnect );
                    }
                break;
            
            case EWaitingToGetDevice:
                textResource = StringLoader
                    ::LoadLC( R_CHAT_STR_DISCONNECTED );
                iLog.LogL( *textResource );
                CleanupStack::PopAndDestroy( textResource );
                break;
                
            default:
                Panic( EChatInvalidLogicState );
                break;
            }
        }
        
    else 
        {
        switch ( State() )
            {
            case EGettingDevice:
                // found a device now search for a suitable service
                iLog.LogL( iServiceSearcher->ResponseParams().DeviceName() );
                SetState( EGettingService );
                iStatus = KRequestPending;  // this means that the RunL 
                                            // can not be called until
                                            // this program does something 
                                            // to iStatus
                iServiceSearcher->FindServiceL( iStatus );
                SetActive();
                break;
                
            case EConnecting:
                textResource = StringLoader::LoadLC ( R_CHAT_CONNECTED );
                iLog.LogL( *textResource );
                CleanupStack::PopAndDestroy( textResource );
              
                // do not accept any more connections
                iAdvertiser->UpdateAvailabilityL( EFalse );
                RequestData();
                SetState( EConnected );
                break;
                
            case EGettingService:
                textResource = StringLoader
                    ::LoadLC( R_CHAT_FOUND_SERVICE );
                iLog.LogL( *textResource );
                CleanupStack::PopAndDestroy( textResource );
                SetState( EGettingConnection );
                ConnectToServerL();
                break;
                
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
                iSocket.Close();
                SetState( EWaitingToGetDevice );
                break;
            
            case EWaitingToGetDevice:
                
                break;
                
            default:
                Panic( EChatInvalidLogicState );
                break;
            };
        }
    }

// ----------------------------------------------------------------------------
// CChatBt::ConnectL()
// Connect to a service.
// ----------------------------------------------------------------------------
//
void CChatBt::ConnectL()
    {
    if ( State() == EWaitingToGetDevice && !IsActive() )
        {
        SetState( EGettingDevice );
        iServiceSearcher->SelectDeviceByDiscoveryL( iStatus );
        SetActive();
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
// CChatBt::DisconnectL()
// Disconnects from the remote machine.
// ----------------------------------------------------------------------------
//
void CChatBt::DisconnectL()
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
// CChatBt::DisconnectFromServerL()
// Disconnects from the service
// ----------------------------------------------------------------------------
//
void CChatBt::DisconnectFromServerL()
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
// CChatBt::ConnectToServerL()
// Connect to the server.
// ----------------------------------------------------------------------------
//
void CChatBt::ConnectToServerL()
    {
    HBufC* strConnecting = StringLoader
        ::LoadLC ( R_CHAT_STR_CONNECTING );
    iLog.LogL( *strConnecting );
    CleanupStack::PopAndDestroy( strConnecting );

    User::LeaveIfError( iSocket.Open( iSocketServer, KStrRFCOMM ) );

    TBTSockAddr address;
    address.SetBTAddr( iServiceSearcher->BTDevAddr() );
    address.SetPort( iServiceSearcher->Port() );

    iSocket.Connect( address, iStatus );
  
    iActiveSocket = &iSocket;
  
    #ifdef __WINS__
        // Fix to allow emulator client to connect to server
        User::After( 1 );     
    #endif
  
    SetActive();
    }


// ----------------------------------------------------------------------------
// CChatBt::SendMessageL()
// Send a message to a service on a remote machine.
// ----------------------------------------------------------------------------
//
void CChatBt::SendMessageL( TDes& aText )
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
    CleanupStack::PushL( tempString );
    tempString->Des().Copy( message );
    
    iMessage = HBufC8::NewL(tempString->Length());
    CleanupStack::Pop( tempString );
    iMessage->Des().Copy( *tempString );
    
    if ( iActiveSocket )
        {
        iActiveSocket->Write( *iMessage, iStatus );
        }

    delete tempString;
    
    SetActive();
    }



// ----------------------------------------------------------------------------
// CChatBt::StartL()
// Starts the server.
// ----------------------------------------------------------------------------
//
void CChatBt::StartL()
    {
    if ( State() != EWaitingToGetDevice )
        {
        User::Leave( KErrInUse );
        }

    TInt result( 0 );
    result = iSocket.Open( iSocketServer, KStrRFCOMM );
    if ( result != KErrNone )
        {
        iSocketServer.Close();
        User::Leave( result ); 
        }

   
    // 
    // Set the Socket's security with parameters, 
    // Authentication, Encryption, Authorisation and Denied
    // Method also return the channel available to listen to. 
    TInt channel;
    SetSecurityWithChannelL( EFalse, EFalse, ETrue, EFalse, channel );

    iAdvertiser->StartAdvertisingL( channel );
    iAdvertiser->UpdateAvailabilityL( ETrue );
    
    SetServer( ETrue );

    }

// ----------------------------------------------------------------------------
// CChatBt::SetSecurityWithChannelL( )
// Sets the security on the channel port and returns the available port.
// ----------------------------------------------------------------------------
//
void CChatBt::SetSecurityWithChannelL( 
    TBool aAuthentication,
    TBool aEncryption,
    TBool aAuthorisation,
    TBool aDenied,
    TInt& aChannel )

    {

    // Local variable to channel to listen to.
    TInt channel;


    User::LeaveIfError( iSocket.GetOpt( 
        KRFCOMMGetAvailableServerChannel, 
        KSolBtRFCOMM, 
        channel ) );

    TBTSockAddr listeningAddress;
    // Set the Port to listen to.
    listeningAddress.SetPort( channel );

    // Write Log events
    HBufC* strGetPort = StringLoader::LoadLC ( R_CHAT_STR_GET_PORT );

    iLog.LogL( *strGetPort, channel );
    CleanupStack::PopAndDestroy( strGetPort );


    User::LeaveIfError( iSocket.Bind( listeningAddress ) );
    User::LeaveIfError( iSocket.Listen( KListeningQueSize ) );

     // close old connection - if any
    iAcceptedSocket.Close();

    // Open abstract socket
    User::LeaveIfError( iAcceptedSocket.Open( iSocketServer ) );  

    // Set the Active Object's State to Connecting indicated.
    SetState( EConnecting );

    iSocket.Accept( iAcceptedSocket, iStatus );
  
    iActiveSocket = &iAcceptedSocket;
  
    // Set the Active Object Active again,
    SetActive();

    // Write Log events
    HBufC* acceptNextConn = StringLoader::LoadLC ( 
        R_CHAT_ACCEPT_NEXT_CONN );
    iLog.LogL( *acceptNextConn );
    CleanupStack::PopAndDestroy( acceptNextConn );

 
    // Set the security according to.
    TBTServiceSecurity serviceSecurity;
    serviceSecurity.SetUid ( KUidChatApp );
    serviceSecurity.SetAuthentication ( aAuthentication );
    serviceSecurity.SetEncryption ( aEncryption );
    serviceSecurity.SetAuthorisation ( aAuthorisation );
    serviceSecurity.SetDenied( aDenied );

    // Attach the security settings.
    listeningAddress.SetSecurity(serviceSecurity);

    // Return the port to listen to.
    aChannel = channel;
  
    }
                      

// ----------------------------------------------------------------------------
// CChatBt::StopL()
// Stops the server.
// ----------------------------------------------------------------------------
//
void CChatBt::StopL()
    {
    if ( State() != EDisconnected )
        {
        if ( iAdvertiser->IsAdvertising() )
            {
            iAdvertiser->StopAdvertisingL();
            }
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
        SetState( EWaitingToGetDevice );
    }

// ----------------------------------------------------------------------------
// CChatBt::RequestData()
// Request data from the client.
// ----------------------------------------------------------------------------
//
void CChatBt::RequestData()
    {
    if ( iActiveSocket )
        {
        iActiveSocket->RecvOneOrMore( iBuffer, 0, iStatus, iLen );
        }
    SetActive();
    }

// End of File

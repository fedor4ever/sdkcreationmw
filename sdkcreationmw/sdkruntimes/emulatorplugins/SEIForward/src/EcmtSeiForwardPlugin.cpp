/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Socket Forward Plugin
*
*/


// INCLUDE FILES
#include <ecom.h>
#include <in_sock.h>
#include <ImplementationProxy.h>
#include <utf.h>
#include "EcmtSeiForwardPlugin.h"
#include "EcmtSeiForwardPluginUids.h"
#include "EcmtMessageEvent.h"
#include "SocketListener.h"
#include "SymbianSocket.h"

// CONSTANTS
const TInt KMaxAcceptMsgLen = 32;
const TInt KMaxMsgPrefixLen = 16;

_LIT8( KConnect, "CONNECT" );
_LIT8( KListen, "LISTEN" );
_LIT8( KMsg, "MSG %d" );
_LIT8( KAcceptMsg, "ACCEPT %d %d" );
_LIT8( KBlanco, " " );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSeiForwardPlugin::CSeiForwardPlugin
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSeiForwardPlugin::CSeiForwardPlugin() : 
	iUid( TUid::Uid( KSEIFORWARDPLUGIN_IMPL_UID ) )
	{
	}

// -----------------------------------------------------------------------------
// CSeiForwardPlugin::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
MEcmtPlugin* CSeiForwardPlugin::NewL( )
	{
    CSeiForwardPlugin* self = new( ELeave ) CSeiForwardPlugin;
    CleanupStack::PushL( self );
    self->ConstructL( );
    
    CleanupStack::Pop();
    return self;
	}

// Destructor
CSeiForwardPlugin::~CSeiForwardPlugin()
	{
	RDebug::Print( _L( "EcmtSeiForwardPlugin::~" ) );

	if ( iSocketServer.Handle() ) 
		{
		iSocketServer.Close();	
		}
	}
	
// -----------------------------------------------------------------------------
// CSeiForwardPlugin::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSeiForwardPlugin::ConstructL()
	{
	RDebug::Print( _L( "EcmtSeiForwardPlugin::ConstructL" ) );

	TInt err = iSocketServer.Connect();
	
	if ( ( err != KErrNone ) && ( err != KErrAlreadyExists )) 
		{
		RDebug::Print( _L("Error connecting to Socket Server: %d" ), err);
		User::Leave(err);
		}
	}

// -----------------------------------------------------------------------------
// CSeiForwardPlugin::Remove
// 
// -----------------------------------------------------------------------------
//
void CSeiForwardPlugin::Remove( CSymbianSocket* aSocket )
	{
	RDebug::Print( _L( "EcmtSeiForwardPlugin::Remove" ) );

	TInt i = iSocketList.Find( aSocket );
	
	if ( i >= 0 )
		{
		RDebug::Print( _L( "EcmtSeiForwardPlugin::Remove: removing socket %d" ), i );
		
		iSocketList.Remove( i );
		iSocketChannelList.Remove( i );
		delete aSocket;
		}
	}

// -----------------------------------------------------------------------------
// CSeiForwardPlugin::ForwardL
// 
// -----------------------------------------------------------------------------
//
void CSeiForwardPlugin::ForwardL( CSymbianSocket* aSocket, TPtrC8 aMessage )
	{
	RDebug::Print( _L( "EcmtSeiForwardPlugin::ForwardL" ) );

	TInt i = iSocketList.Find( aSocket );
	
	if ( i >= 0 )
		{
		TInt channel = iSocketChannelList[ i ];

		RDebug::Print( _L( "EcmtSeiForwardPlugin::ForwardL: forwarding to channel %d" ), channel );

		ReceiveMessageL( channel, aMessage );
		}
	}

// -----------------------------------------------------------------------------
// CSeiForwardPlugin::ForwardL
// 
// -----------------------------------------------------------------------------
//
void CSeiForwardPlugin::ForwardL( CSocketListener* aListener, TPtrC8 aMessage )
	{
	RDebug::Print( _L( "EcmtSeiForwardPlugin::ForwardL" ) );

	TInt i = iListenerList.Find( aListener );
	
	if ( i >= 0 )
		{
		TInt channel = iListenerChannelList[ i ];

		RDebug::Print( _L( "EcmtSeiForwardPlugin::ForwardL: forwarding to channel %d" ), channel );

		ReceiveMessageL( channel, aMessage );
		}
	}

// -----------------------------------------------------------------------------
// CSeiForwardPlugin::BindMessagingL
// 
// -----------------------------------------------------------------------------
//
void CSeiForwardPlugin::BindMessagingL( MEcmtMessaging* aMessaging )
	{
	iMessaging = aMessaging;
	iMessaging->AddMessageEventObserverL( *this, iUid );
	}
	
// -----------------------------------------------------------------------------
// CSeiForwardPlugin::HandleNotifyL
// 
// -----------------------------------------------------------------------------
//
void CSeiForwardPlugin::HandleNotifyL( const CEcmtMessageEvent& aEvent )
	{
    const TPtrC8 m = iMessaging->Message( aEvent );

    TLex8 lexer( m );
    TPtrC8 type = lexer.NextToken();
    TPtrC8 channelStr = lexer.NextToken();
	TLex8 num ( channelStr );
	TInt channel;
	
	// Check message type	
	
    if ( ( type != KConnect ) &&
         ( type != KListen ) && 
         ( type != KMsg ) )
        {
		RDebug::Print( _L( "EcmtSeiForwardPlugin: Invalid message" ) );
        return;
        }

	// Get channel number

	if ( num.Val( channel ) ) 
		{
		RDebug::Print( _L( "EcmtSeiForwardPlugin: Invalid channel" ) );
        return;
		}

	// Process message

	if ( type == KConnect ) 
		{
		TPtrC8 portStr = lexer.NextToken();
		TInt port;
		
		num.Assign ( portStr );
		
		// Get port number
		
		if ( num.Val( port ) ) 
			{
			RDebug::Print( _L( "EcmtSeiForwardPlugin: Invalid port" ) );
        	return;
			}
		
		ConnectL( channel, port );
		}
	else if ( type == KListen )
		{
		ListenL( channel );
		}
	else if ( type == KMsg )
		{
		// Now this just sends the message back to the Java plug-in.		
		// Instead, should cancel the appropriate socket read and write
		// the message to the socket.
		
		TPtrC8 msg = lexer.Remainder();
		
		SendMessageL( channel, msg );
		}
	}

// -----------------------------------------------------------------------------
// CSeiForwardPlugin::ConnectL
// 
// 
// -----------------------------------------------------------------------------
//
void CSeiForwardPlugin::ConnectL( TInt aChannel, TInt aPort )
	{
	RDebug::Print( _L( "EcmtSeiForwardPlugin::ConnectL: channel = %d, port = %d" ), aChannel, aPort );

	iSocketList.Append( CSymbianSocket::NewL( this, &iSocketServer, aPort ) );
	iSocketChannelList.Append( aChannel );
	}

// -----------------------------------------------------------------------------
// CSeiForwardPlugin::ListenL
// 
// 
// -----------------------------------------------------------------------------
//
void CSeiForwardPlugin::ListenL( TInt aChannel )
	{
	RDebug::Print( _L( "EcmtSeiForwardPlugin::ListenL: channel = %d" ), aChannel );

	CSocketListener* listener = CSocketListener::NewL( aChannel, &iSocketServer, this );

	iListenerList.Append( listener );
	iListenerChannelList.Append( aChannel );
	
	AcceptMessageL( aChannel, listener->GetListeningPort() );
	}

// -----------------------------------------------------------------------------
// CSeiForwardPlugin::SendMessageL
// 
// 
// -----------------------------------------------------------------------------
//
void CSeiForwardPlugin::SendMessageL( TInt aChannel, TPtrC8 aMessage )
	{
	RDebug::Print( _L( "EcmtSeiForwardPlugin::SendMessageL: channel = %d" ), aChannel );

	TInt i = iListenerChannelList.Find( aChannel );
	
	if ( i >= 0 )
		{
		CSocketListener* listener = iListenerList[ i ];

		listener->SendMessageL( aMessage );
		
		return;
		}

	i = iSocketChannelList.Find( aChannel );
	
	if ( i >= 0 )
		{
		CSymbianSocket* socket = iSocketList[ i ];

		socket->SendL( aMessage );
		}
	}
	
// -----------------------------------------------------------------------------
// CSeiForwardPlugin::ReceiveMessageL
// 
// 
// -----------------------------------------------------------------------------
//
void CSeiForwardPlugin::ReceiveMessageL( TInt aChannel, TPtrC8 aMessage )
	{
	RDebug::Print( _L( "EcmtSeiForwardPlugin::ReceiveMessageL: channel = %d" ), aChannel );
	
	HBufC8* buf = HBufC8::NewLC( KMaxMsgPrefixLen + 1 + aMessage.Length() );
	TPtr8 message = buf->Des();
		
	message.Format( KMsg, aChannel );
	message.Append( KBlanco );
	message.Append( aMessage );
		
	CEcmtMessageEvent* m = iMessaging->NewMessageEvent( TUid::Uid( KSEIFORWARDPLUGIN_IMPL_UID ), message );
	
	User::LeaveIfNull( m );
	iMessaging->GetMessageSender()->SendMessage( m );
	}

// -----------------------------------------------------------------------------
// CSeiForwardPlugin::AcceptMessageL
// 
// Sends the listening port number to EPDT
// -----------------------------------------------------------------------------
//
void CSeiForwardPlugin::AcceptMessageL( TInt aChannel, TInt aPort )
	{
	RDebug::Print( _L( "EcmtSeiForwardPlugin::AcceptMessageL: channel = %d, port = %d" ), aChannel, aPort );

	TBuf8< KMaxAcceptMsgLen > message;
		
	message.Format( KAcceptMsg, aChannel, aPort );
		
	CEcmtMessageEvent* m = iMessaging->NewMessageEvent( TUid::Uid( KSEIFORWARDPLUGIN_IMPL_UID ), message );
	
	User::LeaveIfNull( m );
	iMessaging->GetMessageSender()->SendMessage( m );
	}

	
// ============================ OTHER FUNCTIONS ===============================
	
/**
 *	Bulk code needed by ECom and Symbian OS frameworks
 */

// Fix for RVCT Error #144: a value of type "MEcmtPlugin *(*)()" cannot be 
// used to initialize an entity of type "TProxyNewLPtr"
static TAny* CreateSeiForwardPluginL()
{
  return CSeiForwardPlugin::NewL();
}

const TImplementationProxy ImplementationTable[] = 
{
  { {KSEIFORWARDPLUGIN_IMPL_UID}, CreateSeiForwardPluginL }
};

// Exported proxy for instantiation method resolution
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aCount)
{
  aCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
  return ImplementationTable;
}
	

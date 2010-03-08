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
* Description:  SEI Forward Plugin
*
*/


// INCLUDE FILES
#include <ecom.h>
#include <in_sock.h>
#include <ImplementationProxy.h>
#include <utf.h>
#include "SocketListener.h"
#include "EcmtSeiForwardPlugin.h"
#include "EcmtSeiForwardPluginUids.h"
#include "EcmtMessageEvent.h"

// CONSTANTS
const TInt KMinPortNumber = 49152 + 306;		//=49459 - 1
const TInt KMaxPortNumber = 49152 + 407;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSocketListener::CSocketListener
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSocketListener::CSocketListener( TInt aChannel, RSocketServ* aSocketServ, CSeiForwardPlugin* aParent ) : 
	CActive( EPriorityStandard ),
	iParent( aParent ),
	iState( EIdle ),
	iSocketServer( aSocketServ ),
	iListeningPort( -1 ),
	iChannel( aChannel )
	{
	}

// -----------------------------------------------------------------------------
// CSocketListener::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSocketListener* CSocketListener::NewL( TInt aChannel, RSocketServ* aSocketServ, CSeiForwardPlugin* aParent )
	{
    CSocketListener* self = new( ELeave ) CSocketListener( aChannel, aSocketServ, aParent );
    CleanupStack::PushL( self );
    self->ConstructL( );
    
    CleanupStack::Pop();
    return self;
	}

// Destructor
CSocketListener::~CSocketListener()
	{
	RDebug::Print( _L( "SocketListener::~" ) );

	Cancel();

    // Delete all sockets and destroy the list
	iSocketList.ResetAndDestroy();

	if ( iBlankSocket )
		{
		iBlankSocket->CancelAll();
		iBlankSocket->Close();
		delete iBlankSocket;
		iBlankSocket = NULL;
		}
	}
	
// -----------------------------------------------------------------------------
// CSocketListener::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSocketListener::ConstructL()
	{
	RDebug::Print( _L( "SocketListener::ConstructL" ) );

	CActiveScheduler::Add( this );

	TUint numProtocols;
	User::LeaveIfError( iSocketServer->NumProtocols( numProtocols ) );

	TInt err = iSocket.Open( *iSocketServer, 
							 KAfInet,
							 KSockStream,
							 KProtocolInetTcp);						

	if ( ( err != KErrNone ) && ( err != KErrAlreadyExists )) 
		{
		RDebug::Print( _L("Error opening socket: %d" ), err);
		User::Leave(err);
		}
		
	TInetAddr sockAddr;
	
	iListeningPort = KMinPortNumber;
	sockAddr.SetAddress( KInetAddrLoop );
	err = KErrGeneral;

	do 
		{
		iListeningPort++;
		sockAddr.SetPort( iListeningPort );
		err = iSocket.Bind( sockAddr );		
		}
	while ( ( err != KErrNone) && ( iListeningPort < KMaxPortNumber ) );
		
	if ( err != KErrNone ) 
		{
		User::Leave( err );
		}

	const TUint KSizeOfListeningQueue = 1;
	
	User::LeaveIfError( err = iSocket.Listen( KSizeOfListeningQueue ) );

	AcceptL();
	}

// -----------------------------------------------------------------------------
// CSocketListener::RunError
// Called by active scheduler if our RunL leaves.
// -----------------------------------------------------------------------------
//
TInt CSocketListener::RunError( TInt /*aError*/ )
	{
	// Just ignore any errors.
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CSocketListener::DoCancel
// 
// -----------------------------------------------------------------------------
//
void CSocketListener::DoCancel( )
	{
	RDebug::Print( _L( "SocketListener::DoCancel" ) );

	if ( iSocket.SubSessionHandle() != 0 )
		{
		iSocket.CancelAll();
		iSocket.Close();
		}
		
	iState = EIdle;
	}

// -----------------------------------------------------------------------------
// CSocketListener::Remove
// 
// -----------------------------------------------------------------------------
//
void CSocketListener::Remove( CSymbianSocket* aSocket )
	{
	RDebug::Print( _L( "SocketListener::Remove" ) );

	TInt i = iSocketList.Find( aSocket );
	
	if ( i >= 0 )
		{
		RDebug::Print( _L( "SocketListener::Remove: removing socket %d" ), i );
		
		iSocketList.Remove( i );
		delete aSocket;
		}
	}

// -----------------------------------------------------------------------------
// CSocketListener::SendMessageL
// 
// -----------------------------------------------------------------------------
//
void CSocketListener::SendMessageL( TPtrC8 aMessage )
	{
	RDebug::Print( _L( "SocketListener::SendMessageL" ) );

	for( TInt i = 0; i < iSocketList.Count(); i++ )
		{
		iSocketList[ i ]->SendL( aMessage );
		}
	}

// -----------------------------------------------------------------------------
// CSocketListener::RunL
// 
// Manages the state machine.
// -----------------------------------------------------------------------------
//
void CSocketListener::RunL( )
	{
	RDebug::Print( _L( "SocketListener::RunL: state = %d, status = %d" ), (TInt)iState, iStatus.Int() );

	switch ( iState )
		{
		case EIdle:
			AcceptL();
			break;
		case EAccepting: 
			{
			iSocketList.Append( CSymbianSocket::NewL( this, iSocketServer, iBlankSocket ) );
			AcceptL();
			break;
			}
		default:
			Cancel();
			break;
    	}
	}
	
// -----------------------------------------------------------------------------
// CSocketListener::AcceptL
// 
// 
// -----------------------------------------------------------------------------
//
void CSocketListener::AcceptL()
	{
	RDebug::Print( _L( "SocketListener::AcceptL" ) );

	iBlankSocket = new RSocket();
	
	User::LeaveIfError( iBlankSocket->Open( *iSocketServer ) );
	
	iSocket.Accept( *iBlankSocket, iStatus );
	iState = EAccepting;
	
	SetActive();
	}
	
// -----------------------------------------------------------------------------
// CSocketListener::ForwardL
// -----------------------------------------------------------------------------
//
void CSocketListener::ForwardL( CSymbianSocket* /* aSocket */, TPtrC8 aMessage )
	{
	RDebug::Print( _L( "EcmtSocketListener::ForwardL" ) );

	// The CSymbianSocket parameter is ignored because currently all CSocketListener
	// sockets belong to the same channel. Ie. the cannel of "this" is determined
	// by CSeiForwardPlugin.

	iParent->ForwardL( this, aMessage );
	}

// -----------------------------------------------------------------------------
// CSocketListener::GetListeningPort
// 
// 
// -----------------------------------------------------------------------------
//
TInt CSocketListener::GetListeningPort()
	{
	return iListeningPort;
	}

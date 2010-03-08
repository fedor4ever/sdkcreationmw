/*
* ============================================================================
*  Name     : CSymbianSocket from SymbianSocket.h
*  Part of  : SEI Forward plug-in
*  Created  : 28.4.2005
*  Description:
*     Declares main application class.
*  Version  :
*  Copyright: 
* ============================================================================
*/

// INCLUDE FILES
#include 	<e32svr.h>
#include	<utf.h>
#include    "SymbianSocket.h"
#include 	"EcmtSeiForwardPlugin.h"

// CONSTANTS
const TInt KBufSize = 512;

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CSymbianSocket::NewL
// 
// Initializes Symbian Socket which has been connected to be a client.
// -----------------------------------------------------------------------------
//
CSymbianSocket* CSymbianSocket::NewL( MSocketContainer* aParent, RSocketServ* aSocketServ, RSocket* aSocket )
	{
    CSymbianSocket* self = new( ELeave ) CSymbianSocket( aParent, aSocketServ, aSocket );
    CleanupStack::PushL( self );
    self->ConstructForReadL( );
    CleanupStack::Pop();
    return self;
	}

// -----------------------------------------------------------------------------
// CSymbianSocket::NewL
// 
// Initializes Symbian Socket for connecting to a server
// -----------------------------------------------------------------------------
//
CSymbianSocket* CSymbianSocket::NewL( MSocketContainer* aParent, RSocketServ* aSocketServ, TInt aPort )
	{
    CSymbianSocket* self = new( ELeave ) CSymbianSocket( aParent, aSocketServ, NULL );
    CleanupStack::PushL( self );
    self->ConstructForWriteL( aPort );
    CleanupStack::Pop();
    return self;
	}

// -----------------------------------------------------------------------------
// CSymbianSocket::CSymbianSocket
// 
// Default constructor
// -----------------------------------------------------------------------------
//
CSymbianSocket::CSymbianSocket( MSocketContainer* aParent, RSocketServ* aSocketServ, RSocket* aSocket ) :
	CActive( EPriorityStandard ),
	iParent( aParent ),
	iSocketServer( aSocketServ ),
	iSocket( aSocket ),
	iReadBuffer( 0, 0 ),
	iState( EIdle )
	{
	}
	
// -----------------------------------------------------------------------------
// CSymbianSocket::CSymbianSocket
// 
// Default destructor
// -----------------------------------------------------------------------------
CSymbianSocket::~CSymbianSocket()
	{
	RDebug::Print( _L( "EcmtSymbianSocket::~" ) );

	Cancel();
	Deque();
	
	if ( iSocket && ( iSocket->SubSessionHandle() != 0 ) )
		{
		iSocket->Close();
		delete iSocket;
		iSocket = NULL;
		}

	if ( iData )
		{
		iReadBuffer.Zero();
		delete iData;
		iData = NULL;
		}

	RDebug::Print( _L( "EcmtSymbianSocket::~: Done" ) );
	}
	
// -----------------------------------------------------------------------------
// CSymbianSocket::ConstructForReadL
// 
// Symbian 2nd phase constructor
// Initializes read buffer and starts to read from socket.
// -----------------------------------------------------------------------------
//
void CSymbianSocket::ConstructForReadL()
	{
	RDebug::Print( _L( "EcmtSymbianSocket::ConstructL" ) );

	CActiveScheduler::Add( this );

    iData = (TUint8*)User::AllocL( KBufSize );
    iReadBuffer.Set( iData, 0, KBufSize );
    
    Read();
	}

// -----------------------------------------------------------------------------
// CSymbianSocket::ConstructForWriteL
// 
// Symbian 2nd phase constructor
// Initializes read buffer and starts to read from socket.
// -----------------------------------------------------------------------------
//
void CSymbianSocket::ConstructForWriteL( TInt aPort )
	{
	RDebug::Print( _L( "EcmtSymbianSocket::ConstructForWriteL" ) );

	CActiveScheduler::Add( this );

    ConnectL( aPort );
	}

// -----------------------------------------------------------------------------
// CSymbianSocket::RunL
// 
// Active Object state machine
// -----------------------------------------------------------------------------
//
void CSymbianSocket::RunL()
	{
	RDebug::Print( _L( "EcmtSymbianSocket::RunL: state = %d, status = %d" ), (TInt)iState, iStatus.Int() );
		
	switch ( iState )
		{
		case EIdle:
		case EConnecting:
		case EWriting:
			{
			Read();
			break;
			}
		case EReading:
			{
			ForwardL();
			break;
			}
		case EDone:
		default:
			{
			Cancel();
			break;
			}
		}
	}

// -----------------------------------------------------------------------------
// CSymbianSocket::DoCancel
// 
// Cancel all requests
// -----------------------------------------------------------------------------
//
void CSymbianSocket::DoCancel( )
	{
	RDebug::Print( _L( "EcmtSymbianSocket::DoCancel" ) );

	if ( iSocket && ( iSocket->SubSessionHandle() != 0 ) )
		{
		iSocket->CancelAll();
		}
		
	iState = EDone;
	}

// -----------------------------------------------------------------------------
// CSymbianSocket::Read
// 
// Read incoming message into iReadBuffer.
// -----------------------------------------------------------------------------
//
void CSymbianSocket::Read()
	{
	RDebug::Print( _L( "EcmtSymbianSocket::Read" ) );

	iReadBuffer.Zero();
	iState = EReading;
	iSocket->RecvOneOrMore( iReadBuffer, 0, iStatus, iReadLen );
	SetActive();
	}
	
// -----------------------------------------------------------------------------
// CSymbianSocket::ConnectL
// 
// Connect to a server port
// -----------------------------------------------------------------------------
//
void CSymbianSocket::ConnectL( TInt aPort )
	{
	RDebug::Print( _L( "EcmtSymbianSocket::ConnectL" ) );

	iSocket = new RSocket();
	
	TInt err = iSocket->Open( *iSocketServer,
							  KAfInet,
							  KSockStream,
							  KProtocolInetTcp);

	if ( ( err != KErrNone ) && ( err != KErrAlreadyExists )) 
		{
		RDebug::Print( _L("EcmtSymbianSocket::ConnectL: Error opening socket: %d" ), err);
		User::Leave(err);
		}
		
	TInetAddr sockAddr;

	sockAddr.SetAddress( KInetAddrLoop );
	sockAddr.SetPort( aPort );
		
	iState = EConnecting;
	iSocket->Connect( sockAddr, iStatus );
	SetActive();
	}
	
// -----------------------------------------------------------------------------
// CSymbianSocket::ForwardL
// 
// Send the contents of iReadBuffer to Ecmt Manager Diagnostics window Log view.
// -----------------------------------------------------------------------------
//
void CSymbianSocket::ForwardL()
	{
	RDebug::Print( _L( "EcmtSymbianSocket::ForwardL" ) );

	if ( iStatus == KErrNone )
		{
		// Read was successful. Forward message and resume reading.

		iParent->ForwardL( this, iReadBuffer );
		Read();
		}
	else 
		{
		// Read failed. Assume connection was closed and shut down the socket.
		
		iParent->Remove( this );
		}
	}

// -----------------------------------------------------------------------------
// CSymbianSocket::SendL
// 
// -----------------------------------------------------------------------------
//
void CSymbianSocket::SendL( TPtrC8 aMessage )
	{
	RDebug::Print( _L( "EcmtSymbianSocket::SendL" ) );
	
	Cancel();
	iState = EWriting;
	iSocket->Write( aMessage, iStatus );
	SetActive();
	}

// End of File  


/*
* ============================================================================
*  Name     : CSymbianSocket from SymbianSocket.h
*  Part of  : HelloEcmt
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
#include 	"EcmtSocketForwardPlugin.h"

// CONSTANTS
const TUint KSystemOutUid = 0x1020456C;
const TInt KBufSize = 512;
const TMessageChannel KChannelId = 1;

// ================= MEMBER FUNCTIONS =======================

CSymbianSocket* CSymbianSocket::NewL( CSocketForwardPlugin* aParent, RSocket* aSocket, MEcmtMessaging* aMessaging )
	{
    CSymbianSocket* self = new( ELeave ) CSymbianSocket( aParent, aSocket, aMessaging );
    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop();
    return self;
	}

// -----------------------------------------------------------------------------
// CSymbianSocket::CSymbianSocket
// 
// Default constructor
// -----------------------------------------------------------------------------
//
CSymbianSocket::CSymbianSocket( CSocketForwardPlugin* aParent, RSocket* aSocket, MEcmtMessaging* aMessaging ) :
	CActive( EPriorityStandard ),
	iParent( aParent ),
	iSocket( aSocket ),
	iMessaging( aMessaging ),
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

	if ( iData )
		{
		iReadBuffer.Zero();
		delete iData;
		iData = NULL;
		}

	RDebug::Print( _L( "EcmtSymbianSocket::~: Done" ) );
	}
	
// -----------------------------------------------------------------------------
// CSymbianSocket::ConstructL
// 
// Symbian 2nd phase constructor
// Initializes read buffer and starts to read from socket.
// -----------------------------------------------------------------------------
//
void CSymbianSocket::ConstructL()
	{
	RDebug::Print( _L( "EcmtSymbianSocket::ConstructL" ) );

	CActiveScheduler::Add( this );

    iData = (TUint8*)User::AllocL( KBufSize );
    iReadBuffer.Set( iData, 0, KBufSize );
    
    Read();
	}

// -----------------------------------------------------------------------------
// CSymbianSocket::RunL
// 
// Active Object state machine
// -----------------------------------------------------------------------------
//
void CSymbianSocket::RunL()
	{
	RDebug::Print( _L( "EcmtSymbianSocket::RunL: state = %d, status = %d" ), iState, iStatus.Int() );
		
	switch ( iState )
		{
		case EIdle:
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
// Cancel all connections and close the socket handle.
// -----------------------------------------------------------------------------
//
void CSymbianSocket::DoCancel( )
	{
	RDebug::Print( _L( "EcmtSymbianSocket::DoCancel" ) );

	if ( iSocket && ( iSocket->SubSessionHandle() != 0 ) )
		{
		iSocket->CancelAll();
		iSocket->Close();
		delete iSocket;
		iSocket = NULL;
		}
		
	iState = EDone;

	RDebug::Print( _L( "EcmtSymbianSocket::DoCancel: Done" ) );
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
// CSymbianSocket::Forward
// 
// Send the contents of iReadBuffer to Ecmt Manager Diagnostics window Log view.
// -----------------------------------------------------------------------------
//
void CSymbianSocket::ForwardL()
	{
	RDebug::Print( _L( "EcmtSymbianSocket::ForwardL" ) );

	if ( iStatus == KErrNone )
		{
		// Read was successful, send message to System Output plug-in in Ecmt Manager.
		
		CEcmtMessageEvent* m = iMessaging->NewMessageEvent( TUid::Uid( KSystemOutUid ), iReadBuffer );
		User::LeaveIfNull( m );
		iMessaging->GetMessageSender()->SendMessage( m, KChannelId );
		Read();
		}
	else 
		{
		// Read failed. Assume connection was closed and shut down the socket.
		
		iParent->Remove( this );
		}
	}

// End of File  


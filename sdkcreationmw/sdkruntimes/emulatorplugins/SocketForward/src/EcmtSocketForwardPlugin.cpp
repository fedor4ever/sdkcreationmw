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
#include <pathinfo.h>

#include "EcmtSocketForwardPlugin.h"
#include "EcmtSocketForwardPluginUids.h"
#include "EcmtMessageEvent.h"

// CONSTANTS
const TInt KLogMaxLen = 40;
const TInt KMinPortNumber = 49152 + 306;		//=49459 - 1
const TInt KMaxPortNumber = 49152 + 407;

_LIT( KBackslash, "\\" );
_LIT( KPortNumberFileFolder, "Images\\");
_LIT( KPortNumberFile, "ecmt.dat" );
_LIT( KLogFile, "ecmt.log" );
_LIT8( KConnAccepted, "Connection accepted\n" );
_LIT8( KWaiting, "Waiting for connections\n" );
_LIT8( KRemoving, "Connection closed\n" );

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSocketForwardPlugin::CSocketForwardPlugin
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSocketForwardPlugin::CSocketForwardPlugin() : 
	CActive( EPriorityStandard ),
	iUid( TUid::Uid( KSOCKETFORWARDPLUGIN_IMPL_UID ) ),
	iState( EIdle )
	{
	}

// -----------------------------------------------------------------------------
// CSocketForwardPlugin::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
MEcmtPlugin* CSocketForwardPlugin::NewL( )
	{
    CSocketForwardPlugin* self = new( ELeave ) CSocketForwardPlugin;
    CleanupStack::PushL( self );
    self->ConstructL( );
    
    CleanupStack::Pop();
    return self;
	}

// Destructor
CSocketForwardPlugin::~CSocketForwardPlugin()
	{
	RDebug::Print( _L( "EcmtSocketForwardPlugin::~" ) );

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

	if ( iSocketServer.Handle() != 0 ) 
		{
		iSocketServer.Close();	
		}

	iLogFile.Close();
	iFs.Close();

	RDebug::Print( _L( "EcmtSocketForwardPlugin::~: Done" ) );
	}
	
// -----------------------------------------------------------------------------
// CSocketForwardPlugin::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSocketForwardPlugin::ConstructL()
	{
	RDebug::Print( _L( "EcmtSocketForwardPlugin::ConstructL" ) );

	CActiveScheduler::Add( this );

	// Initialize file logging
    // File server does not accept file names containing double backslash
    TFileName fileName( PathInfo::PhoneMemoryRootPath() );
    if (fileName[fileName.Length()-1] != '\\') fileName.Append(KBackslash);
    fileName.Append( KLogFile );
    iFs.Connect();
    User::LeaveIfError(iLogFile.Replace( iFs, fileName, EFileWrite ));

	TInt err = iSocketServer.Connect();
	
	if ( ( err != KErrNone ) && ( err != KErrAlreadyExists )) 
		{
		RDebug::Print( _L("Error connecting to Socket Server: %d" ), err);
		User::Leave(err);
		}

	TUint numProtocols;
	User::LeaveIfError( err = iSocketServer.NumProtocols( numProtocols ) );

	err = iSocket.Open( iSocketServer, 
						KAfInet,
						KSockStream,
						KProtocolInetTcp);						

	if ( ( err != KErrNone ) && ( err != KErrAlreadyExists )) 
		{
		RDebug::Print( _L("Error opening socket: %d" ), err);
		User::Leave(err);
		}
		
	TInetAddr sockAddr;
	TInt portNumber = KMinPortNumber;

	sockAddr.SetAddress( KInetAddrLoop );
	err = KErrGeneral;

	do 
		{
		portNumber++;
		sockAddr.SetPort( portNumber );
		err = iSocket.Bind( sockAddr );		
		}
	while ( ( err != KErrNone) && ( portNumber < KMaxPortNumber ) );
		
	if ( err != KErrNone ) 
		{
		User::Leave( err );
		}

	// Write the port number to a file so that the Java clients 
	// can read it and connect to the socket.
	WritePortNumberL( portNumber );

	const TUint KSizeOfListeningQueue = 1;
	
	User::LeaveIfError( err = iSocket.Listen( KSizeOfListeningQueue ) );
	
	AcceptL();
	}

// -----------------------------------------------------------------------------
// CSocketForwardPlugin::BindMessagingL
// 
// -----------------------------------------------------------------------------
//
void CSocketForwardPlugin::BindMessagingL( MEcmtMessaging* aMessaging )
	{
	iMessaging = aMessaging;
       	iMessaging->AddMessageEventObserverL( *this, iUid );
	}
	
// -----------------------------------------------------------------------------
// CSocketForwardPlugin::HandleNotifyL
// 
// -----------------------------------------------------------------------------
//
void CSocketForwardPlugin::HandleNotifyL( const CEcmtMessageEvent& /* aEvent */)
	{
	// This plug-in doesn't except to receive any notifications from 
	// Ecmt Server or other plug-ins.
	}

// -----------------------------------------------------------------------------
// CSocketForwardPlugin::RunError
// Called by active scheduler if our RunL leaves.
// -----------------------------------------------------------------------------
//
TInt CSocketForwardPlugin::RunError( TInt /*aError*/ )
	{
	// Just ignore any errors.
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CSocketForwardPlugin::DoCancel
// 
// -----------------------------------------------------------------------------
//
void CSocketForwardPlugin::DoCancel( )
	{
	RDebug::Print( _L( "EcmtSocketForwardPlugin::DoCancel" ) );

	if ( iSocket.SubSessionHandle() != 0 )
		{
		iSocket.CancelAll();
		iSocket.Close();
		}
		
	iState = EIdle;
	}

// -----------------------------------------------------------------------------
// CSocketForwardPlugin::Remove
// 
// -----------------------------------------------------------------------------
//
void CSocketForwardPlugin::Remove( CSymbianSocket* aSocket )
	{
	RDebug::Print( _L( "EcmtSocketForwardPlugin::Remove" ) );

	TBuf8<KLogMaxLen> buf( KRemoving );
	iLogFile.Write( buf );

	TInt i = iSocketList.Find( aSocket );
	
	if ( i >= 0 )
		{
		RDebug::Print( _L( "EcmtSocketForwardPlugin::Remove: removing socket %d" ), i );
		
		iSocketList.Remove( i );
		delete aSocket;
		}
	}

// -----------------------------------------------------------------------------
// CSocketForwardPlugin::RunL
// 
// Manages the state machine.
// -----------------------------------------------------------------------------
//
void CSocketForwardPlugin::RunL( )
	{
	RDebug::Print( _L( "EcmtSocketForwardPlugin::RunL: state = %d, status = %d" ), iState, iStatus.Int() );

	switch ( iState )
		{
		case EIdle:
			AcceptL();
			break;
		case EAccepting: 
			{
			TBuf8<KLogMaxLen> buf( KConnAccepted );
			iLogFile.Write( buf );

			iSocketList.Append( CSymbianSocket::NewL( this, iBlankSocket, iMessaging ) );
			AcceptL();
			break;
			}
		default:
			Cancel();
			break;
    	}
	}
	
// -----------------------------------------------------------------------------
// CSocketForwardPlugin::AcceptL
// 
// 
// -----------------------------------------------------------------------------
//
void CSocketForwardPlugin::AcceptL()
	{
	RDebug::Print( _L( "EcmtSocketForwardPlugin::AcceptL" ) );

	TBuf8<KLogMaxLen> buf( KWaiting );
	iLogFile.Write( buf );

	iBlankSocket = new RSocket();
	
	User::LeaveIfError( iBlankSocket->Open( iSocketServer ) );
	
	iSocket.Accept( *iBlankSocket, iStatus );
	iState = EAccepting;
	
	SetActive();
	}
	

// -----------------------------------------------------------------------------
// CSocketForwardPlugin::WritePortNumberL
// 
// -----------------------------------------------------------------------------
//
void CSocketForwardPlugin::WritePortNumberL( TInt aPortNumber )
    {
    TBuf8<15> str;
	str.AppendNum(aPortNumber);

    // File server does not accept file names containing double backslash
    TFileName fileName( PathInfo::PhoneMemoryRootPath() );
    if (fileName[fileName.Length()-1] != '\\') fileName.Append(KBackslash);
    fileName.Append( KPortNumberFileFolder );
    fileName.Append( KPortNumberFile );

    RFile file;
    TInt err = file.Replace( iFs, fileName, EFileWrite );
    if (err == KErrNone)
        {
            file.Write( str );
            file.Close();
        }
        else
        {
            User::Leave(err);
        }
    }

// ============================ OTHER FUNCTIONS ===============================

/**
*	Bulk code needed by ECom and Symbian OS frameworks
*/

const TImplementationProxy ImplementationTable[] = 
{
    IMPLEMENTATION_PROXY_ENTRY(KSOCKETFORWARDPLUGIN_IMPL_UID, 
                               CSocketForwardPlugin::NewL)
};

// Exported proxy for instantiation method resolution
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aCount)
{
    aCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
}

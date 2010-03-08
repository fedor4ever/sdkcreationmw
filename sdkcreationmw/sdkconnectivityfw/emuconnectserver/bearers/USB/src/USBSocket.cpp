/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides access to virtual USB serial port
*
*/


#include <ecom.h>
#include <ImplementationProxy.h>
#include "EcmtUSBBearerUids.h"
#include "USBSocket.h"
#include "USBDataTerminalWaiter.h"
#include "DebugLogging.h"

/**
Message, which is sent to EcmtGw when Ecmt Server is going down and closing 
USB socket (see CUSBSocket::Close()). By sending this, EcmtGw closes the 
virtual serial port connection. 

This definition must be EXACTLY the same as ECMT_USB_DISCONNECT_MSG in 
EcmtGwP.h!! 

See ECMT_USB_DISCONNECT_MSG for more information about why EcmtGw is notified 
about Ecmt Server shut down by sending message. Serial line DSR would be 
better solution for this purpose...
*/
_LIT8(KDisconnectMsg, "ECMT-DISCONNECT");

/** AddressFamily */
_LIT( KUSBAf, "USB" );



// ============================================================================
// =========================== CUSBSocketFactory ==============================
// ============================================================================


// ======== LOCAL FUNCTIONS ========

 
// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CEcmtBiSocketFactory* SocketFactoryL()
    {
    USBRDEBUG("CEcmtBiSocketFactory* SocketFactoryL()");
    return new (ELeave) CUSBSocketFactory;
    }

// ---------------------------------------------------------------------------
// Standard ECOM factory code
// ---------------------------------------------------------------------------
//
static const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY (KUSBBEARER_IMPL_UID, SocketFactoryL)
    };

// ---------------------------------------------------------------------------
// Exported proxy for instantiation method resolution
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aCount)
    {
    USBRDEBUG("ImplementationGroupProxy");
    aCount = sizeof(ImplementationTable) / sizeof(ImplementationTable[0]);

    return ImplementationTable;
    }


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// From class CEcmtBiSocketFactory.
// With this function Ecmt Server requests a new socket. 
// ---------------------------------------------------------------------------
//
CEcmtBiSocket* CUSBSocketFactory::NewSocketL()
    {
    USBRDEBUG("CUSBSocketFactory::NewSocketL()");
    // One connection to socket server is enough for all sockets 
    // (and seems to be mandatory, because otherwise we get eSock panic 7)
    // so open it here in factory.
    
    if ( !iInitialized )
	    {
	    // Connect to comms server.	
	    User::LeaveIfError( iCommServ.Connect() );

	    iInitialized = ETrue;
	    }

    if (!iSocket)
	    {
	    iSocket = CUSBSocket::NewL( iCommServ );
	    return iSocket;
	    }  
    else
        {
        // This socket does not need connected RCommServ. So give a dummy 
        // handle for making sure that this instance does not modify connected 
        // iCommServ. 
        RCommServ dummyCommServ;
        return CUSBSocket::NewL( dummyCommServ );         
        }   
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CUSBSocketFactory::~CUSBSocketFactory( )
    {
    USBRDEBUG("CUSBSocketFactory::~CUSBSocketFactory()");
    if ( iInitialized )
	    {
        iCommServ.Close();
	    }
    }


// ============================================================================
// =============================== CUSBSocket =================================
// ============================================================================


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CUSBSocket::CUSBSocket( RCommServ& aCommServ ) :
    iCommServ( aCommServ ),
    iParameterString( NULL ),
    iRealSocket( NULL )
    {
    USBRDEBUG2("CUSBSocket::CUSBSocket [%08x]", this);
    __LOGWRITE1("CUSBSocket::CUSBSocket [%08x]",this);
    }

// ---------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------------------------
//
void CUSBSocket::ConstructL()
    {
    USBRDEBUG("CUSBSocket::ConstructL()");
    }

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CUSBSocket* CUSBSocket::NewL( RCommServ& aCommServ )
    {
    USBRDEBUG("CUSBSocket::NewL");
    CUSBSocket* self = new( ELeave ) CUSBSocket( aCommServ );    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CUSBSocket::~CUSBSocket()
    {    
    USBRDEBUG2("CUSBSocket::~CUSBSocket [%08x]",this);
    __LOGWRITE1("CUSBSocket::~CUSBSocket [%08x]",this);
    
    if( iParameterString )
        {
        __LOGWRITE("CUSBSocket::~CUSBSocket - Deleting iParameterString");
        delete iParameterString;
        }
    if( iWaiter )
        {
        __LOGWRITE("CUSBSocket::~CUSBSocket - Deleting iWaiter");
        delete iWaiter;
        }
    }


// ---------------------------------------------------------------------------
// From class CEcmtBiSocket.
// Opens USB serial connection.
// ---------------------------------------------------------------------------
//
TInt CUSBSocket::Open( TBool aBlank ) 
    {    
    USBRDEBUG2("CUSBSocket::Open [%08x]",this);
    __LOGWRITE1("CUSBSocket::Open [%08x]",this);
    TInt err( KErrNone );
    	
    if ( !aBlank )
    	{
    	// Open USB serial connection exclusively for ECMT.
    	
    	_LIT( KACM, "ACM::1" );
    	
    	err = iComm.Open( iCommServ, KACM, ECommExclusive );
    	USBRDEBUG2( "CUSBSocket::Open() - iComm.Open() error:%d", err );
    	__LOGWRITE1( "CUSBSocket::Open() - iComm.Open() error:%d", err );   
    	
    	if (err == KErrNone)
    	    {	    	        
    	    // Set up USB serial connection
    	    TCommConfig configBuf;
    	    TCommConfigV01& config = configBuf();
    	    
    	    config.iRate = EBps115200;
    	    config.iDataBits = EData8;
    	    config.iStopBits = EStop1;
    	    config.iParity = EParityNone;
    	    config.iHandshake = //KConfigFreeDSR | //Not a signal for DTE device
    	                        KConfigFreeDTR | // The output DTR line should not be controlled by the driver
    	                        KConfigObeyDSR | // Obey the input DSR control line
    	                        KConfigFailDSR | // Complete any pending operations. KErrLineFail if the DSR input ever goes low
    	                        KConfigFreeRTS;  // The output RTS line should not be controlled by the driver  	    
    	                        
    	    config.iTerminatorCount = 0;
    	    
    	    //Set serial port configuration
    	    iComm.SetConfig( configBuf );    	    
    	    }
    	}
    	
    return err;
    }

// ---------------------------------------------------------------------------
// From class CEcmtBiSocket.
// Writes to the connection.
// ---------------------------------------------------------------------------
//
void CUSBSocket::Write( const TDesC8& aDesc, TRequestStatus& aStatus )
    {
    USBRDEBUG2("CUSBSocket::Write [%08x]",this);
    __LOGWRITE1("CUSBSocket::Write [%08x]",this);
    
    if ( iRealSocket )
	    {
	    iRealSocket->Write( aDesc, aStatus );
	    }
    else
	    {
	    // Write only if DSR signal is up. Otherwise message will be left to 
	    // some queue and EcmtGw gets the message on next time when it 
	    // connects.
	    if( iWaiter->IsDSR() )
	        {
	        iComm.Write( aStatus, aDesc );
	        }
	    }	 
	USBSOCKETLOG( _L("    "), aDesc );   
    }
    
// ---------------------------------------------------------------------------
// Writes to log file C:\Logs\EcmtServer\EcmtServer.log. 
// ---------------------------------------------------------------------------
//    
#ifdef _ENABLE_USB_LOGGINGS_
void CUSBSocket::WriteToLog( const TDesC& aFName, const TDesC8& aString )
    {
    RFileLogger logger;
    logger.Connect();
    
    logger.CreateLog( _L("EcmtServer"), 
                        _L("EcmtServer.log"), 
                        EFileLoggingModeAppend );
    
    _LIT(KData, "%S Data(%d): %S");
    HBufC* temp = HBufC::NewL( aString.Length() );
    temp->Des().Copy( aString );
    
    logger.WriteFormat( KData, 
                        &aFName,
                        temp->Length(),
                        temp);
                   
    logger.Close();
    delete temp;
    }
#endif // _ENABLE_USB_LOGGINGS_ 

// ---------------------------------------------------------------------------
// From class CEcmtBiSocket.
// Cancels write requests from C32.
// ---------------------------------------------------------------------------
//
void CUSBSocket::CancelWrite()
    {
    USBRDEBUG2("CUSBSocket::CancelWrite [%08x]",this);
    __LOGWRITE1("CUSBSocket::CancelWrite [%08x]",this);
    if ( iRealSocket )
	    {
	    iRealSocket->CancelWrite();
	    }
    else
	    {
	    iComm.WriteCancel();
	    }
    }

// ---------------------------------------------------------------------------
// From class CEcmtBiSocket.
// Reads from connection. Sets CUSBDataTerminalWaiter to wait DSR change. 
// If DSR changes, iWaiter completes CEcmtSocketsReader AO with error. By 
// this way Ecmt Server closes the socket.
// ---------------------------------------------------------------------------
//
void CUSBSocket::Read( TDes8& aDesc, TRequestStatus& aStatus )
    {
    USBRDEBUG2("CUSBSocket::Read [%08x]",this);
    __LOGWRITE1("CUSBSocket::Read [%08x]",this);
    if ( iRealSocket )
	    {	        
	    iRealSocket->Read( aDesc, aStatus );	    
	    }
    else
    	{	
    	if( iWaiter->IsDSR() )
    	    {
    	    if( !iWaiter->IsActive() )
    	        {
    	        iWaiter->SetReaderTRS( aStatus );
    	        iWaiter->WaitDSRChange();
    	        }

    	    iComm.Read( aStatus, aDesc );
    		
    		USBSOCKETLOG( _L("    "), aDesc ); 
    	    }
    	else
    		{
	    	// Do not issue read to RComm if we don't have DSR signal. Only 
	    	// complete SocketsEngine AO with error. When it is completed with 
	    	// error, EcmtAgent displays "Disconnected".
	    	if( iWaiter && !iWaiter->IsActive() )
	    		{
	    		iWaiter->SetReaderTRS( aStatus );
	    		iWaiter->WaitReaderActivation();
	    		}
    		}    		
        }
    }

// ---------------------------------------------------------------------------
// From class CEcmtBiSocket.
// Cancels read requests fro C32.
// ---------------------------------------------------------------------------
//
void CUSBSocket::CancelRead()
    {
    USBRDEBUG2("CUSBSocket::CancelRead [%08x]",this);
    __LOGWRITE1("CUSBSocket::CancelRead [%08x]",this);
    if ( iRealSocket )
		{
		iRealSocket->CancelRead();
		}
    else
		{
		iComm.ReadCancel();
		}
    }

// ---------------------------------------------------------------------------
// From class CEcmtBiSocket.
// Does nothing. Not needed in USB implementation. Returns always KErrNone 
// to Ecmt Server
// ---------------------------------------------------------------------------
//
TInt CUSBSocket::Bind( )
    {
    USBRDEBUG2("CUSBSocket::Bind [%08x]",this);
    __LOGWRITE1("CUSBSocket::Bind [%08x]",this);
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// From class CEcmtBiSocket.
// Does nothing. Not needed in USB implementation. Returns always KErrNone 
// to Ecmt Server
// ---------------------------------------------------------------------------
//
TInt CUSBSocket::Listen( TUint aSize )
    {
    USBRDEBUG2("CUSBSocket::Listen [%08x]",this);
    __LOGWRITE1("CUSBSocket::Listen [%08x]",this);
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// From class CEcmtBiSocket.
// Accepts connection. If data terminal is not ready (DSR==0) it waits for
// it using CEcmtDataTerminalWaiter. 
// ---------------------------------------------------------------------------
//
void CUSBSocket::Accept( CEcmtBiSocket& aBlankSocket, TRequestStatus& aStatus )
    {
    USBRDEBUG2("CUSBSocket::Accept [%08x]",this);
    __LOGWRITE1("CUSBSocket::Accept [%08x]",this);
    CEcmtBiSocket* socket( &aBlankSocket );

    dynamic_cast< CUSBSocket* >( socket )->SetRealSocket( this );
    
    //If data terminal is not ready, wait for it
    if( !iWaiter )
    	{
    	USBRDEBUG("CUSBSocket::Accept - Creating a new CUSBDataTerminalWaiter object");
    	__LOGWRITE("CUSBSocket::Accept - Creating a new CUSBDataTerminalWaiter object");
    	iWaiter = CUSBDataTerminalWaiter::NewL( *this, iComm );
    	}
    
    iWaiter->SetEngineTRS( aStatus );   
    if( !iWaiter->IsActive() )
    	{
    	if( iWaiter->IsDSR() )
	        {
	        iWaiter->WaitEngineActivation();
	        }
    	else
	        {
	        iWaiter->WaitDSRChange();
	        }
    	}
    }

// ---------------------------------------------------------------------------
// From class CEcmtBiSocket.
// Cancels accept by cancelling read/write requests from C32.
// ---------------------------------------------------------------------------
//
void CUSBSocket::CancelAccept()
    {
    USBRDEBUG2("CUSBSocket::CancelAccept [%08x]",this);
    __LOGWRITE1("CUSBSocket::CancelAccept [%08x]",this);
 
    if (!iRealSocket)
        {
        TInt err = iComm.Cancel();
        iWaiter->CompleteAllAO();
        }
    }
    
// ---------------------------------------------------------------------------
// From class CEcmtBiSocket.
// Closes USB socket. If DSR is down (Ecmt Gateway in disconnected state), 
// it only closes iComm handle, otherwise it sends Ecmt Disconnect message 
// to the GW for informing it that server is going down. If the disconnect 
// message is sent when GW is in disconnected state, we end up to error 
// situation where Ecmt Gateway receives the message from some queue in 
// next connection immediately after that closes the connection. User::After
// calls are necessary? Without those Ecmt Server can't destroy it self and 
// EcmtAgent hangs to CUSBDataTerminalWaiter::DoCancel() (Waits for 
// completion). Might be because RComm::Close completes same AOs as 
// ~CUSBDataTerminalWaiter does.
// ---------------------------------------------------------------------------
//
void CUSBSocket::Close()
    {
    USBRDEBUG2("CUSBSocket::Close [%08x]",this);
    __LOGWRITE1("CUSBSocket::Close [%08x]",this);  

    if (!iRealSocket)
        {
        // Send Disconnect message only if EcmtGW is in connected state. 
        // Otherwise EcmtGW would get the disconnect message at the beginning 
        // of next connection from some queue.
        if( iWaiter->IsDSR() )
            {
            USBRDEBUG("CUSBSocket::Close() - ATTEMPTING TO DISCONNECT!!!!");
            __LOGWRITE("CUSBSocket::Close() - ATTEMPTING TO DISCONNECT!!!!");    
            TRequestStatus status;
            Write( KDisconnectMsg(), status );
            User::WaitForRequest( status );            
            __LOGWRITE("CUSBSocket::Close() - DISCONNECTED!!!!");
            User::After( TTimeIntervalMicroSeconds32( 150000 ) );
            USBRDEBUG("CUSBSocket::Close() - DISCONNECTED!!!!");
            }
                    
        USBRDEBUG("CUSBSocket::Close() - Closing RComm handle");        
        // And close the RComm handle
        iComm.Close();
        User::After( TTimeIntervalMicroSeconds32( 150000 ) );
        }
    }

// ---------------------------------------------------------------------------
// From class CEcmtBiSocket.
// Returns pointer to iParameterString. Used for Ecmt Server notification 
// messages which are shown on the GuiLauncher log view.
// ---------------------------------------------------------------------------
//
const TPtrC CUSBSocket::ParameterString() const
    {
    USBRDEBUG("CUSBSocket::ParameterString()");
    __LOGWRITE("CUSBSocket::ParameterString");
    if ( iParameterString )
	    {
        return *iParameterString;
	    }
    else
	    {
        return KNullDesC().Ptr();
	    }
    }

// ---------------------------------------------------------------------------
// From class CEcmtBiSocket.
// Returns pointer descriptor to string defined in literal KUSBAf. Used for 
// Ecmt Server notification messages which are shown on the GuiLauncher log 
// view.
// ---------------------------------------------------------------------------
//
const TPtrC CUSBSocket::AddressFamily() const
{
    USBRDEBUG("CUSBSocket::AddressFamily()");
    __LOGWRITE("CUSBSocket::AddressFamily");
    return KUSBAf().Ptr();
}

// ---------------------------------------------------------------------------
// From class CEcmtBiSocket.
// 
// ---------------------------------------------------------------------------
//
void CUSBSocket::SetRealSocket( CUSBSocket* aSocket )
    {
    USBRDEBUG2("CUSBSocket::SetRealSocket [%08x]", this);
    __LOGWRITE1("CUSBSocket::SetRealSocket [%08x]",this);  
    iRealSocket = aSocket;
    }


//  End of File

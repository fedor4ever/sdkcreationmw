/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
#include 	<ecom.h>
#include 	<ImplementationProxy.h>
#include    <in_sock.h>
#include    <CommDbConnPref.h> 
#include <ES_SOCK.H>
#include    "DebugLogging.h"
#include	"WLANSocket.h"
#include	"EcmtClientServerCommon.h"
#include 	"EcmtWLANBearerUids.h"

// =============================== ECOM STUFF ================================

#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
{
    return(KErrNone);
}
#endif // !EKA2
    
CEcmtBiSocketFactory* SocketFactoryL()
{
    return new (ELeave) CWLANSocketFactory;
}
    
static const TImplementationProxy ImplementationTable[] = 
{
    IMPLEMENTATION_PROXY_ENTRY (KWLANBEARER_IMPL_UID, SocketFactoryL)
};

// Exported proxy for instantiation method resolution
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aCount)
{
    aCount = sizeof(ImplementationTable) / sizeof(ImplementationTable[0]);
    return ImplementationTable;
}

// ===========================================================================
// =========================== CWLANSocketFactory ==============================
// ===========================================================================

CEcmtBiSocket* CWLANSocketFactory::NewSocketL()
{
    // One connection to socket server is enough for all sockets 
    // (and seems to be mandatory, because otherwise we get eSock panic 7)
    // so open it here in factory.
    if (!iInitialized)
    {
        User::LeaveIfError( iSocketServ.Connect() );
        iInitialized = ETrue;
    }
    return CWLANSocket::NewL( iSocketServ );
}

CWLANSocketFactory::~CWLANSocketFactory( )
{
    if (iInitialized )
    {
        iSocketServ.Close();
    }
}

// ===========================================================================
// =============================== CWLANSocket =================================
// ===========================================================================

CWLANSocket::CWLANSocket( RSocketServ& aSocketServ ) :
iSocketServ( aSocketServ )
    {
    TRACE1("CWLANSoclet[%08x] created",this);
    }

void CWLANSocket::ConstructL()
    {
    TRACE("CWLANSocket::ConstructL");
    __LOGWRITE("CWLANSocket::ConstructL()");
    iConnection.Open(iSocketServ);
    
    TCommDbConnPref prefs;
    
    
    TUint32 iapId(0);    
    if( LoadIapIdL( iapId ) == KErrNotFound )
        prefs.SetDialogPreference( ECommDbDialogPrefPrompt );
        
    else
        {
        prefs.SetDialogPreference( ECommDbDialogPrefDoNotPrompt ); 
        prefs.SetIapId( iapId );
        __LOGWRITE1("CWLANSocket::ConstructL() - IapId:%d",iapId);
        }
      
    TRequestStatus status;
    iConnection.Start( prefs, status ); 
    User::WaitForRequest( status );
    
    if( status.Int() == KErrNone )
        {
        __LOGWRITE1("CWLANSocket::ConstructL() - status:%d",status.Int());
        }
    }

CWLANSocket* CWLANSocket::NewL( RSocketServ& aSocketServ )
    {
    TRACE("CWLANSocket::NewL");
    CWLANSocket* self = new( ELeave ) CWLANSocket( aSocketServ );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }
    
CWLANSocket::~CWLANSocket()
    {
    TRACE("CWLANSocket::~CWLANSocket");
    delete iParameterString;
    }

TInt CWLANSocket::LoadIapIdL( TUint32& aIapId )
    {
    RFs fsSession;
    User::LeaveIfError( fsSession.Connect() ); 
    CleanupClosePushL( fsSession );
    _LIT( KFileName, "c:\\Data\\EcmtBearer.dat" );
    RFile file;
    
    TInt err=file.Open( fsSession, KFileName, EFileStreamText|EFileRead|EFileShareAny );
    if( err != KErrNone )
    	{
    	CleanupStack::PopAndDestroy();
    	return KErrNotFound;
    	}
    	
    CleanupClosePushL( file );
    TBuf8<30> bearerData;      
    file.Read(0, bearerData);
    
    CleanupStack::PopAndDestroy( 2 );
    
    TInt dataLength = bearerData.Length();
    
    if( dataLength < 1 )
        return KErrNotFound;  
    
    _LIT8( KLinebreak, "\n" );
    TInt offset = bearerData.Find( KLinebreak() );
    
    HBufC* buf = NULL;
    if( offset != KErrNotFound )
        {
        // Notify line break, "( offset+1 )";
        TInt newLength = dataLength - ( offset+1 );
        buf = HBufC::NewL( newLength );
        buf->Des().Copy( bearerData.Right( newLength ) );
        }
    else
        return KErrNotFound;
    
    TLex iapId( buf->Des() );
    
    TInt val = KErrNotFound;    
    if( iapId.Val( val ) != KErrNone )
        {
        delete buf;
        return KErrNotFound;
        }
    else
        aIapId = val;
        
    delete buf;
    return KErrNone;
    }
    

TInt CWLANSocket::Open( TBool aBlank )
    {
    TRACE("CWLANSocket::Open");
    TInt err;
    if (!aBlank)
        {
        err = iSocket.Open(iSocketServ, 
                            KAfInet, 
                            KSockStream, 
                            KProtocolInetTcp, 
                            iConnection);
                            
        __LOGWRITE1("CWLANSocket::Open - Socket opened with error:%d",err);
        return err;
                
        }
    else
        {
    	err = iSocket.Open(iSocketServ);
    	__LOGWRITE1("CWLANSocket::Open - BlankSocket opened with error:%d",err);
        return err;
        }
    }

void CWLANSocket::Write( const TDesC8& aDesc, TRequestStatus& aStatus )
    {
    __LOGWRITE("CWLANSocket::Write");
    TRACE("CWLANSocket::Write");
    iSocket.Write( aDesc, aStatus );
    }

void CWLANSocket::CancelWrite()
    {
    __LOGWRITE("CWLANSocket::CancelWrite");
    TRACE("CWLANSocket::CancelWrite");
    iSocket.CancelWrite( );
    }

void CWLANSocket::Read( TDes8& aDesc, TRequestStatus& aStatus )
    {
    TRACE("CWLANSocket::Read");
    __LOGWRITE("CWLANSocket::Read");
    iSocket.Read( aDesc, aStatus );
    }

void CWLANSocket::CancelRead()
    {
    __LOGWRITE("CWLANSocket::CancelRead");
    TRACE("CWLANSocket::CancelRead");
    iSocket.CancelRead( );
    }

TInt CWLANSocket::Bind( )
   {
   TRACE("CWLANSocket::Bind");
   TInt port = 40000;
   TBuf<40> ip;
   TInt err;
   
   TInetAddr inetAddr;
   //find out the device ip address
   RSocketServ socketServ;
   RSocket sock;
   User::LeaveIfError(socketServ.Connect());
   User::LeaveIfError(sock.Open(socketServ, KAfInet, KSockStream, KProtocolInetTcp));
   
   TSoInetInterfaceInfo inf;// = new TSoInetInterfaceInfo;
   TPckg<TSoInetInterfaceInfo> opt(inf);
   TSoInetIfQuery ifQuery;
   TPckg<TSoInetIfQuery> ifQueryPkg(ifQuery);
   err = sock.SetOpt( KSoInetEnumInterfaces, KSolInetIfCtrl );
   if (err == KErrNone)
       {
       while (sock.GetOpt(KSoInetNextInterface, KSolInetIfCtrl, opt) == KErrNone)
           {
            ifQuery.iName = inf.iName;
            TInt err = sock.GetOpt(KSoInetIfQueryByName, KSolInetIfQuery, ifQueryPkg);
            if( err == KErrNone )
               {
               if (!inf.iAddress.IsUnspecified() && !inf.iAddress.IsLoopback() && !inf.iAddress.IsLinkLocal())
                     {
                     inetAddr = inf.iAddress;
                     inetAddr.ConvertToV4();
                     inetAddr.Output(ip);
                     }
               }
             else if( err !=KErrNone )
                    {
                    sock.Close();
                    socketServ.Close();
                    }
            }
       } 
   
   err = KErrNotFound;
   //bind the ip address and port number
   while( err != KErrNone )
       {
       port += 10;
       inetAddr.SetPort(port);
       err = iSocket.Bind( inetAddr );        
       }
   __LOGWRITE1("CWLANSocket::Bind() - Bind error:%d",err);
   //display ip address and port number on the screen
   if( err == KErrNone )
       {
       port = inetAddr.Port(); 
       if( iParameterString )
           {
           delete iParameterString;
           iParameterString = NULL;
           }
       
       _LIT( KFormat, "%d" );
       TBuf<10> portStr;
       portStr.Format( KFormat, port  );
       iParameterString = HBufC::NewL( ip.Length() + portStr.Length() + 3 );
       iParameterString->Des().Append( ip );
       iParameterString->Des().Append( _L(" : ") );
       iParameterString->Des().Append( portStr );        
       __LOGWRITE("CWLANSocket::Bind() - ParameterString created");
       }
       sock.Close();
       socketServ.Close();   
   return err;
   }

TInt CWLANSocket::Listen( TUint aSize )
    {
    TRACE("CWLANSocket::Listen");
    __LOGWRITE("CWLANSocket::Listen");
    return iSocket.Listen( aSize );
    }

void CWLANSocket::Accept(CEcmtBiSocket& aBlankSocket, TRequestStatus& aStatus)
    {
    TRACE("CWLANSocket::Accept");
    __LOGWRITE("CWLANSocket::Accept");
    iSocket.Accept(static_cast<CWLANSocket&>(aBlankSocket).iSocket, aStatus);
    }

void CWLANSocket::CancelAccept()
    {
    __LOGWRITE("CWLANSocket::CancelAccept");
    TRACE("CWLANSocket::CancelAccept");
    iSocket.CancelAccept();
    }
    
void CWLANSocket::Close()
    {
    __LOGWRITE("CWLANSocket::Close");
    TRACE("CWLANSocket::Close");
    iSocket.Close();
    }

const TPtrC CWLANSocket::ParameterString() const
    {
    __LOGWRITE("CWLANSocket::ParameterString");
    TRACE("CWLANSocket::ParameterString");
    return iParameterString->Des();
    }

const TPtrC CWLANSocket::AddressFamily() const
    {
    __LOGWRITE("CWLANSocket::AddressFamily");
    TRACE("CWLANSocket::AddressFamily");
    _LIT(KWlan, "WLAN");
    return KWlan().Ptr();
    }

/**
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

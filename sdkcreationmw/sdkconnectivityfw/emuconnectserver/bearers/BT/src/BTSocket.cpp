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
* Description:  Provides access to Bluetooth "sockets" implemented on top of
 *                Bluetooth Serial Port service.
 *
*/



// INCLUDE FILES
#include 	<ecom.h>
#include 	<ImplementationProxy.h>
#include	<bt_sock.h>
#include	<btmanclient.h>
#include	"BTSocket.h"
#include	"BTServiceAdvertiser.h"
#include	"EcmtClientServerCommon.h"
#include 	"EcmtBTBearerUids.h"

// =============================== ECOM STUFF ================================

#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason /*aReason*/)
{
    return(KErrNone);
}
#endif // !EKA2
    
CEcmtBiSocketFactory* SocketFactoryL()
{
    return new (ELeave) CBTSocketFactory;
}
    
static const TImplementationProxy ImplementationTable[] = 
{
    IMPLEMENTATION_PROXY_ENTRY (KBTBEARER_IMPL_UID, SocketFactoryL)
};

// Exported proxy for instantiation method resolution
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aCount)
{
    aCount = sizeof(ImplementationTable) / sizeof(ImplementationTable[0]);
    return ImplementationTable;
}

// ===========================================================================
// =========================== CBTSocketFactory ==============================
// ===========================================================================

CEcmtBiSocket* CBTSocketFactory::NewSocketL()
{
    // One connection to socket server is enough for all sockets 
    // (and seems to be mandatory, because otherwise we get eSock panic 7)
    // so open it here in factory.
    if (!iInitialized)
    {
        User::LeaveIfError( iSocketServ.Connect() );
        iInitialized = ETrue;
    }
    return CBTSocket::NewL( iSocketServ );
}

CBTSocketFactory::~CBTSocketFactory( )
{
    if (iInitialized )
    {
        iSocketServ.Close();
    }
}

// ===========================================================================
// =============================== CBTSocket =================================
// ===========================================================================

CBTSocket::CBTSocket( RSocketServ& aSocketServ ) :
iSocketServ( aSocketServ )
{
}

void CBTSocket::ConstructL()
{
    iAdvertiser = new CBTServiceAdvertiser();
}

CBTSocket* CBTSocket::NewL( RSocketServ& aSocketServ )
{
    CBTSocket* self = new( ELeave ) CBTSocket( aSocketServ );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}
    
CBTSocket::~CBTSocket()
{
    delete iAdvertiser;
    delete iParameterString;
}

TInt CBTSocket::Open( TBool aBlank )
{
    if (!aBlank)
    {
        return iSocket.Open(iSocketServ, KRFCOMMDesC);
    }
    else
    {
    	return iSocket.Open(iSocketServ);
    }
}

void CBTSocket::Write( const TDesC8& aDesc, TRequestStatus& aStatus )
{
    iSocket.Write( aDesc, aStatus );
}

void CBTSocket::CancelWrite()
{
    iSocket.CancelWrite( );
}

void CBTSocket::Read( TDes8& aDesc, TRequestStatus& aStatus )
{
    iSocket.Read( aDesc, aStatus );
}

void CBTSocket::CancelRead()
{
    iSocket.CancelRead( );
}

TInt CBTSocket::Bind( )
{
    TInt ch = 0;
    TInt r = iSocket.GetOpt(KRFCOMMGetAvailableServerChannel,KSolBtRFCOMM,ch);
    if (r == KErrNone)
    {
	TBTSockAddr addr;
	addr.SetPort(ch);
	r = iSocket.Bind(addr);
        if (r == KErrNone)
        {
            TRAP(r, iAdvertiser->StartAdvertisingL(ch));
        }
    }
    return r;
}

TInt CBTSocket::Listen( TUint aSize )
{
    return iSocket.Listen( aSize );
}

void CBTSocket::Accept(CEcmtBiSocket& aBlankSocket, TRequestStatus& aStatus)
{
    iSocket.Accept(static_cast<CBTSocket&>(aBlankSocket).iSocket, aStatus);
}

void CBTSocket::CancelAccept()
{
    iSocket.CancelAccept();
}
    
void CBTSocket::Close()
{
    TRAP_IGNORE(iAdvertiser->StopAdvertisingL());
    iSocket.Close();
}

const TPtrC CBTSocket::ParameterString() const
{
    return (iParameterString ? (*iParameterString) : KNullDesC());
}

const TPtrC CBTSocket::AddressFamily() const
{
    _LIT(KBTAf, "Bluetooth");
    return KBTAf().Ptr();
}

/**
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

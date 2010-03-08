/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides access to Win32 sockets from Symbian OS
*
*/


// INCLUDE FILES
#include <in_sock.h>
#include <ecom.h>
#include <ImplementationProxy.h>
#include "W32Socket.h"
#include "EcmtW32BearerUids.h"
#include "W32BearerDebug.h"

//Include minimal set of windows headers (to speed-up compilation)
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>

#ifdef EKA2
#  include <emulator.h>
#  define ESCAPE()  Emulator::Escape()
#  define REENTER() Emulator::Reenter()
#else
#  define ESCAPE()  ((void)0)
#  define REENTER() ((void)0)
#endif

// CONSTANTS

// Range of ports to choose from. Sequentially try to bind to one of the
// ports in the following range (in the dynamic port range).
const TInt KMinPortNumber = 49152 + 207;		//=49359
const TInt KMaxPortNumber = 49152 + 207 + 100;

// ============================= LOCAL FUNCTIONS =============================

/**
 * Bulk code needed by ECom and Symbian OS frameworks
 */

CEcmtBiSocketFactory* SocketFactoryL()
{
    return new (ELeave) CW32SocketFactory;
}
    
const TImplementationProxy ImplementationTable[] = 
{
    { { KW32BEARER_IMPL_UID }, SocketFactoryL }
};

// Exported proxy for instantiation method resolution
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aCount)
{
    aCount = sizeof(ImplementationTable) / sizeof(ImplementationTable[0]);
    return ImplementationTable;
}
    
// ===========================================================================
// =========================== CW32SocketFactory =============================
// ===========================================================================

// ============================ MEMBER FUNCTIONS =============================

CW32SocketFactory::CW32SocketFactory() : iInitialized(EFalse)
{
}
	
// ---------------------------------------------------------------------------
// CW32SocketFactory::NewL
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
CEcmtBiSocket* CW32SocketFactory::NewSocketL()
{
    if ( !iInitialized )
    {
        WSADATA wsaData;
        ESCAPE();
        WSAStartup( MAKEWORD(1,1), &wsaData );
        REENTER();
        iInitialized = ETrue;
    }
    return CW32Socket::NewL();
}

CW32SocketFactory::~CW32SocketFactory()
{
}
	
// ===========================================================================
// ============================== CW32Socket::A ==============================
// ===========================================================================

// "Active" part of CW32Socket
class CW32Socket::A: public CActive
{
    CW32Socket* iSocket;
public:
    A(CW32Socket* aSocket);
    void Activate();
protected:
    virtual void DoCancel();
    virtual void RunL();
};

CW32Socket::A::A(CW32Socket* aSocket) :
    CActive(EPriorityStandard),
    iSocket(aSocket) 
{
    CActiveScheduler::Add(this);
}

void CW32Socket::A::DoCancel()
{
    CW32SelectThread::Static()->Cancel(iSocket);
    if (iStatus == KRequestPending)
    {
        TRequestStatus *status = (&iStatus);
        User::RequestComplete(status, KErrCancel);
    }
}

void CW32Socket::A::RunL()
{
    iSocket->RunL(iStatus.Int());
}

void CW32Socket::A::Activate()
{
    iStatus = KRequestPending;
    if (!IsActive()) SetActive();
}

// ===========================================================================
// =============================== CW32Socket ================================
// ===========================================================================

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// CW32Socket::CW32Socket
// C++ default constructor can NOT contain any code, that might leave.
// ---------------------------------------------------------------------------
CW32Socket::CW32Socket() : 
    iState(EStateInit), 
    iW32Socket(INVALID_SOCKET)
{
    TRACE1("CW32Socket[%08x] created",this);
    iRequestThread.Open(RThread().Id());
}

// ---------------------------------------------------------------------------
// CW32Socket::NewL
// ---------------------------------------------------------------------------
//
CW32Socket* CW32Socket::NewL()
{
    CW32Socket* self = new(ELeave)CW32Socket;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

void CW32Socket::ConstructL()
{
    iActive = new A(this);
    CW32SelectThread::StaticL();
}
    
// Destructor
CW32Socket::~CW32Socket()
{
    Close();
    iRequestThread.Close();
    delete iActive;
    TRACE1("CW32Socket[%08x] destroyed",this);
}

// ---------------------------------------------------------------------------
// CW32Socket::Open
// ---------------------------------------------------------------------------
TInt CW32Socket::Open( TBool /*aBlank*/ )
{
    if (iState == EStateInit)
    {
        TRACE1("CW32Socket[%08x]::Open",this);
        ESCAPE();
        iW32Socket = socket(AF_INET, SOCK_STREAM, 0);
        REENTER();
        if (iW32Socket != INVALID_SOCKET)
        {
            TRACE1("CW32Socket[%08x] state = EStateCreated",this);
            iState = EStateCreated;
            return KErrNone;
        }
    }
    else
    {
	    TRACE2("CW32Socket[%08x]::Open: wrong state %d",this,iState);
    }
    return KErrGeneral;
}

// ---------------------------------------------------------------------------
// CW32Socket::Connect
// ---------------------------------------------------------------------------
void CW32Socket::Connect( TSockAddr& aAddr, TRequestStatus& aStatus )
{
    if (iState == EStateBound)
    {
	    TRACE1("CW32Socket[%08x]::Connect",this);
        aStatus = KRequestPending;
    
        TInetAddr& inetAddr = TInetAddr::Cast( aAddr );
        SOCKADDR_IN remoteAddr = {0};
        remoteAddr.sin_family = AF_INET;
        remoteAddr.sin_port = htons((TUint16)inetAddr.Port());
        remoteAddr.sin_addr.s_addr = htonl(inetAddr.Address());

        ESCAPE();
        int r = connect(iW32Socket,(SOCKADDR*)&remoteAddr,sizeof(remoteAddr));
        REENTER();

        User::RequestComplete(&aStatus, (r == 0) ? KErrNone : KErrGeneral);
    } 
    else
    {
	    TRACE2("CW32Socket[%08x]::Connect: wrong state %d",this,iState);
        User::RequestComplete(&aStatus, KErrGeneral);
    }
}


// ---------------------------------------------------------------------------
// CW32Socket::CancelConnect
// ---------------------------------------------------------------------------
void CW32Socket::CancelConnect()
{
    TRACE1("CW32Socket[%08x]::CancelConnect",this);
}

// ---------------------------------------------------------------------------
// CW32Socket::Write
// ---------------------------------------------------------------------------
void CW32Socket::Write( const TDesC8& aDesc, TRequestStatus& aStatus )
{
    switch (iState)
    {
    case EStateConnected:
    case EStateReading:
        {
            TRACE1("CW32Socket[%08x]::Write",this);
            aStatus = KRequestPending;
    
            char* buf = (char*)aDesc.Ptr();
            int bytesToSend = aDesc.Length();
            int bytesSent = 0;
            TInt status = KErrNone;

            while (bytesSent < bytesToSend)
            {
                ESCAPE();
                int bytesRemaning = bytesToSend-bytesSent;
                int nbytes = send(iW32Socket,buf+bytesSent,bytesRemaning,0);
                REENTER();
                if (nbytes == SOCKET_ERROR)
                {
                    status = KErrGeneral;
                    break;
                }
                else
                {
                    bytesSent += nbytes;
                }
            }

            TRACE3("CW32Socket[%08x]::Write: Bytes sent: %d (out of %d)",this,
                bytesSent, bytesToSend);
            User::RequestComplete(&aStatus, status);
        }
        break;

    default:
	    TRACE2("CW32Socket[%08x]::Write: wrong state %d",this,iState);
        User::RequestComplete(&aStatus, KErrGeneral);
        break;
    }
}

// ---------------------------------------------------------------------------
// CW32Socket::CancelWrite
// ---------------------------------------------------------------------------
void CW32Socket::CancelWrite()
{
    TRACE1("CW32Socket[%08x]::CancelWrite",this);
}

// ---------------------------------------------------------------------------
// CW32Socket::Read
// ---------------------------------------------------------------------------
void CW32Socket::Read( TDes8& aDesc, TRequestStatus& aStatus )
{
	TRACE2("CW32Socket[%08x]::Read, %d bytes",this,aDesc.MaxLength());
    if (iState == EStateReading)
    {
        CancelRead();
    }
    if (iState == EStateConnected)
    {
        aDesc.SetLength(0);
        if (aDesc.MaxLength() > 0)
        {
            aStatus = KRequestPending;
            iRequestStatus = &aStatus;
            iReadBuffer = &aDesc;

            TRACE1("CW32Socket[%08x] state = EStateReading",this);
            iState = EStateReading;
            NonBlockingRead(EFalse);
        }
        else
        {
	        TRACE1("CW32Socket[%08x]::Read: zero size request",this);
            User::RequestComplete(&aStatus, KErrNone);
        }
    }
    else
    {
	    TRACE2("CW32Socket[%08x]::Read: wrong state %d",this,iState);
        User::RequestComplete(&aStatus, KErrGeneral);
    }
}

// ---------------------------------------------------------------------------
// CW32Socket::NonBlockingRead
// Reads as much data as possible, up to the requested amount, 
// without blocking the current thread. If more data is needed, 
// requests asynchronous read. The socket must be in the READING
// state.
// ---------------------------------------------------------------------------
void CW32Socket::NonBlockingRead(TBool aCanReadZeroBytes)
{
    TInt status = KErrGeneral;
    TUint32 avail = 0;
    
    ESCAPE();
    int err = ioctlsocket(iW32Socket, FIONREAD, &avail);
    REENTER();
    
    if (!err)
    {
        TInt len = iReadBuffer->Length();
        TInt maxBytesToRead = iReadBuffer->MaxLength() - len;
        TInt bytesToRead = Min(maxBytesToRead,avail);
        TInt bytesReceived = 0;

        // NOTE: attempt to read zarro bytes will block the current thread.
        // Don't do it unless select indicated that we can read without
        // blocking (in which case we will most likely get an error)
        if (bytesToRead > 0 || aCanReadZeroBytes)
        {
            iReadBuffer->SetLength(len + bytesToRead);
            char* buf = ((char*)iReadBuffer->Ptr()) + len;

            ESCAPE();
            bytesReceived = recv(iW32Socket, buf, bytesToRead, 0);
            REENTER();
            
            if (bytesReceived > 0)
            {
                iReadBuffer->SetLength(len + bytesReceived);
            }
            else
            {
                TRACE3("CW32Socket[%08x] requested %d bytes, received %d",
                    this, bytesToRead, bytesReceived);
                TRACE1("CW32Socket[%08x] state = EStateError",this);
                CompleteRequest(KErrGeneral);
                iState = EStateError;
                return;
            }
        }

        if (iReadBuffer->Length() == iReadBuffer->MaxLength())
        {
            TRACE2("CW32Socket[%08x] read complete, %d byte(s)",this,
                iReadBuffer->Length());
            TRACE1("CW32Socket[%08x] state = EStateConnected",this);
            CompleteRequest(KErrNone);
            iState = EStateConnected;
        }
        else
        {
            // Data is not arrived yet, will have to wait
            TRACE1("CW32Socket[%08x] asynchronous read",this);
            SubmitSelectRequest();
        }
    }
    else
    {
        CompleteRequest(KErrGeneral);
        TRACE1("CW32Socket[%08x] state = EStateError",this);
        iState = EStateError;
    }
}

// ---------------------------------------------------------------------------
// CW32Socket::CancelRead
// ---------------------------------------------------------------------------
void CW32Socket::CancelRead()
{
    if (iState == EStateReading)
    {
        TRACE1("CW32Socket[%08x]::CancelRead",this);
        CW32SelectThread::Static()->Cancel(this);
        CompleteRequest(KErrCancel);

        TRACE1("CW32Socket[%08x] state = EStateConnected",this);
        iState = EStateConnected;
    }
}

// ---------------------------------------------------------------------------
// CW32Socket::Bind
// ---------------------------------------------------------------------------
TInt CW32Socket::Bind()
{
    if (iState == EStateCreated)
    {
        TRACE1("CW32Socket[%08x]::Bind",this);
        SOCKADDR_IN winAddr = {0};
        winAddr.sin_family = AF_INET;
        winAddr.sin_addr.s_addr = INADDR_ANY;

        unsigned short portNumber;
        int r = -1;
        
        for (portNumber = KMinPortNumber; 
             portNumber < KMaxPortNumber && r != 0; 
             portNumber++ )
        {
            winAddr.sin_port = htons(portNumber);
            ESCAPE();
            r = bind(iW32Socket,(SOCKADDR*)&winAddr, sizeof(winAddr));        
            REENTER();
        }

        if (r == 0)
        {
            TRACE2("CW32Socket[%08x] port = %d",this,portNumber-1);
            TRACE1("CW32Socket[%08x] state = EStateBound",this);
            iState = EStateBound;
            iParameterString.Append( _L("port=") );
            iParameterString.AppendNum( portNumber-1 );
            return KErrNone;
        }
    }
    else
    {
        TRACE2("CW32Socket[%08x]::Bind: wrong state %d",this,iState);
    }
    return KErrGeneral;
}

// ---------------------------------------------------------------------------
// CW32Socket::Listen
// ---------------------------------------------------------------------------
TInt CW32Socket::Listen( TUint aSize )
{
    if (iState == EStateBound)
    {
        TRACE2("CW32Socket[%08x]::Listen: size = %d",this,aSize);
        ESCAPE();
        int r = listen(iW32Socket, aSize);
        REENTER();

        if (r == 0)
        {
            TRACE1("CW32Socket[%08x] state = EStateListen",this);
            iState = EStateListen;
            return KErrNone;
        }
    }
    else
    {
        TRACE2("CW32Socket[%08x]::Listen: wrong state %d",this,iState);
    }
    return KErrGeneral;
}

// ---------------------------------------------------------------------------
// CW32Socket::Accept
// ---------------------------------------------------------------------------
void CW32Socket::Accept(CEcmtBiSocket& aBlankSocket, TRequestStatus& aStatus)
{
    if (iState == EStateListen)
    {
        // Since dynamic_cast cannot be used in Symbian, simulate it
        // by checking the AddressFamily string
        __ASSERT_ALWAYS(!aBlankSocket.AddressFamily().Compare(AddressFamily()),
                        User::Invariant());

        aStatus = KRequestPending;
        iAcceptSocket = (CW32Socket*)&aBlankSocket;
        iRequestStatus = &aStatus;
        SubmitSelectRequest();

        TRACE1("CW32Socket[%08x] state = EStateAccepting",this);
        iState = EStateAccepting;
    }
    else
    {
        TRACE2("CW32Socket[%08x]::Accept: wrong state %d",this,iState);
        User::RequestComplete(&aStatus, KErrGeneral);
    }
}
    
// ---------------------------------------------------------------------------
// CW32Socket::CancelAccept
// ---------------------------------------------------------------------------
void CW32Socket::CancelAccept()
{
    if (iState == EStateAccepting)
    {
        TRACE1("CW32Socket[%08x]::CancelAccept",this);
        CW32SelectThread::Static()->Cancel(this);
        CompleteRequest(KErrCancel);
        
        TRACE1("CW32Socket[%08x] state = EStateListen",this);
        iState = EStateListen;
    } 
    else
    {
        TRACE2("CW32Socket[%08x]::CancelAccept: wrong state %d",this,iState);
    }
}

// ---------------------------------------------------------------------------
// Handles request completion
// ---------------------------------------------------------------------------

void CW32Socket::RunL(TInt aStatus)
{
    if (aStatus == KErrNone && !(iSelectMask & ESelectError))
    {
        switch (iState)
        {
            case EStateAccepting:
            {
                SOCKADDR_IN remoteAddr;
                int size = sizeof(remoteAddr);

                ESCAPE();
                SOCKET s = accept(iW32Socket, (SOCKADDR*)&remoteAddr, &size);
                REENTER();

                // Copy socket descriptor to blank socket
                iAcceptSocket->Close();
                iAcceptSocket->iW32Socket = s;
                iAcceptSocket->iState = EStateConnected;
                CompleteRequest(KErrNone);

                TRACE1("CW32Socket[%08x] state = EStateListen",this);
                iState = EStateListen;
            }
            break;

        case EStateReading:
            NonBlockingRead(ETrue);
            break;

        default:
            TRACE2("CW32Socket[%08x]:RunL: unexpected state %d",this,iState);
            break;
        }
    }
    else
    {
        CompleteRequest(KErrGeneral);
        TRACE1("CW32Socket[%08x] state = EStateError",this);
        iState = EStateError;
    }
}
    
// ---------------------------------------------------------------------------
// CW32Socket::Close
// ---------------------------------------------------------------------------
void CW32Socket::Close()
{
    TRACE1("CW32Socket[%08x]::Close",this);
    CW32SelectThread::Static()->Cancel(this);
    iActive->Cancel();
    CompleteRequest(KErrCancel);
    iParameterString.Zero();

    ESCAPE();
    shutdown(iW32Socket, SD_BOTH);
    closesocket(iW32Socket);
    REENTER();

    iW32Socket = INVALID_SOCKET;
    iState = EStateInit;
}

// ---------------------------------------------------------------------------
// CW32Socket::ParameterString
// ---------------------------------------------------------------------------
const TPtrC CW32Socket::ParameterString() const
{
    return iParameterString;
}

// ---------------------------------------------------------------------------
// CW32Socket::AddressFamily
// ---------------------------------------------------------------------------
const TPtrC CW32Socket::AddressFamily() const
{
    _LIT( KW32Af,"Winsock");
    return KW32Af().Ptr();
}

// ---------------------------------------------------------------------------
// Submits a new select request. Must be invoked on the Ecmt thread
// ---------------------------------------------------------------------------
void CW32Socket::SubmitSelectRequest()
{
    TThreadId me = RThread().Id();
    if (iRequestThread.Id() != me)
    {
        iRequestThread.Close();
        iRequestThread.Open(RThread().Id());
    }
    iActive->Activate();
    CW32SelectThread::Static()->Submit(this);
}

// ---------------------------------------------------------------------------
// Completes the current request
// ---------------------------------------------------------------------------
void CW32Socket::CompleteRequest(TInt aStatus)
{
    if (iRequestStatus)
    {
        iRequestThread.RequestComplete(iRequestStatus, aStatus);
        iRequestStatus = NULL;
        iAcceptSocket = NULL;
        iReadBuffer = NULL;
    }
}

// ---------------------------------------------------------------------------
// MSelectRequest implementation
// ---------------------------------------------------------------------------

TUint CW32Socket::Socket()
{
    return iW32Socket;
}

TInt CW32Socket::SelectMask()
{
    return (ESelectRead|ESelectError);
}

void CW32Socket::SelectComplete(TInt aSelectMask)
{
    iSelectMask = aSelectMask;
    TRequestStatus *status = (&iActive->iStatus);
    iRequestThread.RequestComplete(status, KErrNone);
}

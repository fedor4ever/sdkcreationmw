/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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


#define TRACE_PREFIX "WSOCK: SelectThread: "
#include "wsock.h"
#include "WinsockSelectThread.h"
#include <winsock2.h>

#ifdef EKA2
// In Series60 version 3.0 they (Symbian) changed the emulator 
// threading model from preemptive to cooperative. That is, only
// one emulated Symbian thread is running at any point of time.
// The threads are never interrupted (with exception of interrupt
// simulation) and must explicitely yield control by making a 
// kernel call (typically, WaitForAnyRequest or something like
// that). It means that if thread blocks on a Win32 event or
// socket, it prevents other threads from running and the emulator
// gets stuck. The Emulator::Escape() call tells the scheduler to
// temporarily remove current thread from the list of scheduled
// threads, which allows other threads to run while we are blocked
// on select. Emulator::Reenter() waits for the currently active
// Symbian thread to yeild control and makes the current thread
// a normal Symbian thread again.
#  define BEGIN_WIN32() Emulator::Escape()
#  define END_WIN32() Emulator::Reenter()
#  include <emulator.h>
#else
#  define BEGIN_WIN32() ((void)0)
#  define END_WIN32() ((void)0)
#endif // EKA2

// Symbian does not support writeable static data, but we don't care because
// this code only runs in the emulator.
static CWinsockSelectThread* instance = NULL;
const TInt ETimeoutInMicroSeconds = 5000000;

CWinsockSelectThread* CWinsockSelectThread::Static()
{
    CWinsockSelectThread* self = NULL;
    TRAPD(err,self = &StaticL());
    return self;
}

CWinsockSelectThread& CWinsockSelectThread::StaticL()
{
    if (!instance)
    {
        instance = NewL();
    }
    return *instance;
}

CWinsockSelectThread* CWinsockSelectThread::NewL()
{
    CWinsockSelectThread* self = new(ELeave)CWinsockSelectThread;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

CWinsockSelectThread::CWinsockSelectThread() : 
iUnblockSocket(INVALID_SOCKET)
{
}

CWinsockSelectThread::~CWinsockSelectThread()
{
    // Normally, this object never gets destroyed
    // Only if ConstructL() leaves
    ASSERT(!iRequests.Count());
    if (iCriticalSectionCreated) iCriticalSection.Close();
    iRequests.Reset();
    if (iUnblockSocket != INVALID_SOCKET) 
    {
        BEGIN_WIN32();
        closesocket(iUnblockSocket);
        END_WIN32();
    }
    delete iAddr.iSockAddrIn;
    delete iTimeout;
}

void CWinsockSelectThread::ConstructL()
{
    iTimeout = new(ELeave)(struct timeval);
    iTimeout->tv_sec = (ETimeoutInMicroSeconds/1000000);
    iTimeout->tv_usec = (ETimeoutInMicroSeconds%1000000);
    
    iAddr.iSockAddrIn = new(ELeave)(struct sockaddr_in);
    TInt addrSize = sizeof(struct sockaddr_in);
    Mem::FillZ(iAddr.iSockAddrIn, addrSize);
    LEAVE_IF_ERROR(iCriticalSection.CreateLocal());

    // Create a UDP socket for unblocking the select
    BEGIN_WIN32();
    iUnblockSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    END_WIN32();

    if (iUnblockSocket == INVALID_SOCKET)
    {
        TRACE1("failed to create socket, err %d", WSAGetLastError());
        LEAVE(KErrGeneral);
    }

    iAddr.iSockAddrIn->sin_family = AF_INET;
    iAddr.iSockAddrIn->sin_addr.s_addr = inet_addr("127.0.0.1");

    BEGIN_WIN32();
    int err = bind(iUnblockSocket, iAddr.iSockAddr, addrSize);
    END_WIN32();

    if (err)
    {
        TRACE1("failed to bind, err %d", WSAGetLastError());
        User::Leave(KErrGeneral);
    }

    BEGIN_WIN32();
    err = getsockname(iUnblockSocket, iAddr.iSockAddr, &addrSize);
    END_WIN32();

    if (err)
    {
        TRACE1("failed to getsockname, err %d", WSAGetLastError());
        LEAVE(KErrGeneral);
    }

    TRACE1("notification socket bound to UDP port %d",
        (int)ntohs(iAddr.iSockAddrIn->sin_port));

    // Finally, create and start the thread
    _LIT(threadName,"WinsockSelectThread");
    LEAVE_IF_ERROR(iThread.Create(threadName, Run, KDefaultStackSize,
        NULL, this, EOwnerProcess));
    iThread.Resume();
}

// Wakes up the select thread
TBool CWinsockSelectThread::Wakeup()
{
    char msg[1] = {0};

    BEGIN_WIN32();
    int bytesSent = sendto(iUnblockSocket,msg,sizeof(msg),0,
        iAddr.iSockAddr, sizeof(struct sockaddr_in));
    END_WIN32();

    if (bytesSent == SOCKET_ERROR)
    {
        TRACE1("failed to unblock select, err %d", WSAGetLastError());
        return EFalse;
    }
    return ETrue;
}

TInt CWinsockSelectThread::Submit(MSelectRequest* aRequest)
{
    TInt err = KErrNone;
    iCriticalSection.Wait();
    TInt index = iRequests.FindInAddressOrder(aRequest);
    if (index < 0) err = iRequests.InsertInAddressOrder(aRequest);
    if (err == KErrNone) Wakeup();
    iCriticalSection.Signal();
    return err;
}

TBool CWinsockSelectThread::Cancel(MSelectRequest* aRequest)
{
    TBool found = EFalse;
    iCriticalSection.Wait();
    TInt index = iRequests.FindInAddressOrder(aRequest);
    if (index >= 0)
    {
        iRequests.Remove(index);
        Wakeup();
        found = ETrue;
    }
    iCriticalSection.Signal();
    return found;
}

TInt CWinsockSelectThread::Run(TAny *aPtr)
{
    int err;
    CWinsockSelectThread* thread = (CWinsockSelectThread*)aPtr;
    CTrapCleanup* cleanup = CTrapCleanup::New();
    if (cleanup) {
        TRAP(err, thread->RunL());
        delete cleanup;
    } else {
        err = KErrNoMemory;
    }
    return err;
}

void CWinsockSelectThread::RunL()
{
    TRACE("started");
    RPointerArray<MSelectRequest> completedRequests;
    struct timeval zeroTimeout;
    Mem::FillZ(&zeroTimeout, sizeof(zeroTimeout));

    // Open handle to our protocol DLL to prevent it from being unloaded
    // while this thread is running
    RLibrary self;
    TBuf<12> libName; 
    libName.Copy(KWinsockProtocol);
    libName.Append(_L(".prt"));
    VERIFY_SUCCESS(self.Load(libName));

    for (;;)
    {
        TInt i,n,nfd;
        TUint maxSock;

        fd_set read_fd_set;
        fd_set write_fd_set;
        fd_set except_fd_set;

        fd_set * readfs = NULL;
        fd_set * writefs = NULL;
        fd_set * errfs = NULL;
        
#pragma warning(push)            // FD_SET macro in winsock.h produces warning
#pragma warning(disable : 4127)  // C4127: conditional expression is constant
        
        // Always listen for notification socket
        readfs = &read_fd_set;
        FD_ZERO(readfs);
        FD_SET(iUnblockSocket,readfs);
        maxSock = iUnblockSocket;

        // Construct the fdsets
        iCriticalSection.Wait();
        n = iRequests.Count();
        for (i=0; i<n; i++)
        {
            MSelectRequest* req = iRequests[i];
            TUint sock = req->Socket();
            TInt requestMask = req->SelectMask();
            if (maxSock < sock) maxSock = sock;
            if (requestMask & ESelectRead)
            {
                FD_SET(sock,readfs);
            }
            if (requestMask & ESelectWrite)
            {
                if (!writefs)
                {
                    writefs = &write_fd_set;
                    FD_ZERO(writefs);
                }
                FD_SET(sock,writefs);
            }
            if (requestMask & ESelectError)
            {
                if (!errfs)
                {
                    errfs = &except_fd_set;
                    FD_ZERO(errfs);
                }
                FD_SET(sock,errfs);
            }
        }

#pragma warning(pop) 

        iCriticalSection.Signal();

        BEGIN_WIN32();
        nfd = select(maxSock+1, readfs, writefs, errfs, iTimeout);
        END_WIN32();

        if (nfd == SOCKET_ERROR)
        {
            TRACE1("select err %d",WSAGetLastError());
            break;
        }

        TBool dataInNotificationSocket = EFalse;
        if (FD_ISSET(iUnblockSocket, readfs))
        {
            // Don't count the notification socket
            nfd--;

            // Read one message from the notification socket
            char msg[1];
            DEBUG_ONLY(int nbytes = )recv(iUnblockSocket,msg,sizeof(msg),0);
            ASSERT(nbytes != SOCKET_ERROR);
            dataInNotificationSocket = ETrue;
        }

        if (nfd == 0)
        {
            // Timeout or request to rebuild fdsets
            continue;
        }

        iCriticalSection.Wait();

        // Read all remaining messages from the notification socket
        // The fact that we are in a critical section guarantees that
        // we won't get stuck here forever
        if (dataInNotificationSocket)
        {
            char msg[1];
            fd_set tmp;
            FD_ZERO(&tmp);
#pragma warning(push)            // Ignore warning generated by FD_SET macro
#pragma warning(disable : 4127)  // C4127: conditional expression is constant
            FD_SET(iUnblockSocket,&tmp);
            BEGIN_WIN32();
            while (select(iUnblockSocket+1,&tmp,NULL,NULL,&zeroTimeout) == 1)
            {
                if (recv(iUnblockSocket, msg, sizeof(msg), 0) == SOCKET_ERROR)
                {
                    TRACE1("internal recv socket err %d",WSAGetLastError());
                    break;
                }
                FD_ZERO(&tmp);
                FD_SET(iUnblockSocket,&tmp);
            }
            END_WIN32();
#pragma warning(pop) 
        }

        // Move completed requests to completedRequests queue
        n = iRequests.Count();
        for (i=n-1; i>=0 && nfd > 0; i--)
        {
            MSelectRequest* req = iRequests[i];
            TUint sock = req->Socket();
            TInt requestMask = req->SelectMask();
            TInt selectMask = 0;
            if (requestMask & ESelectRead && FD_ISSET(sock, readfs))
            {
                selectMask |= ESelectRead;
            }
            if (requestMask & ESelectWrite && FD_ISSET(sock, writefs))
            {
                selectMask |= ESelectRead;
            }
            if (requestMask & ESelectError && FD_ISSET(sock, errfs))
            {
                selectMask |= ESelectError;
            }
            if (selectMask)
            {
                nfd--;
                if (completedRequests.Append(req) == KErrNone)
                {
                    iRequests.Remove(i);
                }
            }
        }

        iCriticalSection.Signal();

        // Fire completion notifications outside of the critical section
        n = completedRequests.Count();
        for (i=n-1; i>=0; i--)
        {
            MSelectRequest* req = completedRequests[i];
            TUint sock = req->Socket();
            TInt requestMask = req->SelectMask();
            TInt selectMask = 0;
            if (requestMask & ESelectRead && FD_ISSET(sock, readfs))
            {
                selectMask |= ESelectRead;
            }
            if (requestMask & ESelectWrite && FD_ISSET(sock, writefs))
            {
                selectMask |= ESelectRead;
            }
            if (requestMask & ESelectError && FD_ISSET(sock, errfs))
            {
                selectMask |= ESelectError;
            }
            ASSERT(selectMask);
            req->SelectComplete(selectMask);
            completedRequests.Remove(i);
        }
    }
    completedRequests.Reset();
}

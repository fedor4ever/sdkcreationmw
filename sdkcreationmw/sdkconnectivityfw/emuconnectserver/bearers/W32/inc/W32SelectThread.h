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


#ifndef _WSOCK_READ_THREAD_H_
#define _WSOCK_READ_THREAD_H_

#include <e32base.h>

enum TSelectMask
{
    ESelectRead  = 0x0001,
    ESelectWrite = 0x0002,
    ESelectError = 0x0004
};

class MSelectRequest
{
public:
    virtual TUint Socket() = 0;
    virtual TInt SelectMask() = 0;
    virtual void SelectComplete(TInt aMask) = 0;
};

class CW32SelectThread : public CBase
{
private:
    RCriticalSection iCriticalSection;
    RThread iThread;
    TBool iCriticalSectionCreated;
    RPointerArray<MSelectRequest> iRequests;
    TUint iUnblockSocket;
    struct timeval* iTimeout;
    union {
        struct sockaddr* iSockAddr;
        struct sockaddr_in* iSockAddrIn;
    } iAddr;

public:
    static CW32SelectThread* Static();
    static CW32SelectThread& StaticL();

    TInt Submit(MSelectRequest* aRequest);
    TBool Cancel(MSelectRequest* aRequest);
private:
    static CW32SelectThread* NewL();
    CW32SelectThread();
    virtual ~CW32SelectThread();
    void ConstructL();
    static TInt Run(TAny *aPtr);
    void RunL();
    TBool Wakeup();
};

#endif // _WSOCK_READ_THREAD_H_

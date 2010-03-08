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


#ifndef _WSOCK_READ_THREAD_H_
#define _WSOCK_READ_THREAD_H_

#include <e32std.h>

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

class CWinsockSelectThread : public CBase
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
    static CWinsockSelectThread* Static();
    static CWinsockSelectThread& StaticL();

    TInt Submit(MSelectRequest* aRequest);
    TBool Cancel(MSelectRequest* aRequest);
private:
    static CWinsockSelectThread* NewL();
    CWinsockSelectThread();
    virtual ~CWinsockSelectThread();
    void ConstructL();
    static TInt Run(TAny *aPtr);
    void RunL();
    TBool Wakeup();
};

#endif // _WSOCK_READ_THREAD_H_

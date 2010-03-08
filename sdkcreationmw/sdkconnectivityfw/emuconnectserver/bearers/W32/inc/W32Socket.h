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
* Description:  Provides access to Win32 sockets from Symbian OS
*
*/


#ifndef W32SOCKET_H
#define W32SOCKET_H

// INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <es_sock.h>
#include "EcmtBiSocket.h"
#include "W32SelectThread.h"

// CONSTANTS

const TUint KDefaultHeapSize=0x10000;

// CLASS DECLARATION

/**
*  @ingroup EcmtBearer
*/
class CW32SocketFactory : public CEcmtBiSocketFactory
{
public:
    CW32SocketFactory();
    CEcmtBiSocket* NewSocketL();
    ~CW32SocketFactory();
private:
    TBool iInitialized;
};

/**
*  Provides access to win32 sockets from Symbian OS
*  @ingroup EcmtBearer
*/
class CW32Socket : public CEcmtBiSocket, public MSelectRequest
{
    class A;
    friend class A;

public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CW32Socket* NewL();

    /**
     * Destructor.
     */
    virtual ~CW32Socket();

public:

    // CEcmtBiSocket
    virtual TInt Open(TBool aBlank);

    virtual void Connect(TSockAddr& aAddr, TRequestStatus& aStatus);
    virtual void CancelConnect();

    virtual void Write(const TDesC8& aDesc, TRequestStatus& aStatus);
    virtual void CancelWrite();
    
    virtual void Read(TDes8& aDesc, TRequestStatus& aStatus);
    virtual void CancelRead();        
    
    virtual TInt Bind();
    virtual TInt Listen(TUint aSize);
    
    virtual void Accept(CEcmtBiSocket& aBlankSocket, TRequestStatus& aStatus);
    virtual void CancelAccept();
    
    virtual void Close();
    
    virtual const TPtrC ParameterString() const;
    virtual const TPtrC AddressFamily() const; 

    // MSelectRequest
    virtual TUint Socket();
    virtual TInt SelectMask();
    virtual void SelectComplete(TInt aMask);

private:
    CW32Socket();
    void ConstructL();
    void RunL(TInt aStatus); 
    void NonBlockingRead(TBool aCanReadZeroBytes);
    void CompleteRequest(TInt aStatus);
    void SubmitSelectRequest();

private:

    enum TState
    {
        EStateInit,
        EStateCreated,
        EStateBound,
        EStateListen,
        EStateAccepting,
        EStateConnected,
        EStateReading,
        EStateError
    };

    TState iState;
    TInt iW32Socket;
    TInt iSelectMask;
    CW32Socket* iAcceptSocket;
    A* iActive;
    RThread  iRequestThread;
    TRequestStatus* iRequestStatus;
    TDes8* iReadBuffer;
    TBuf<16> iParameterString;
};

#endif  // W32SOCKET_H   

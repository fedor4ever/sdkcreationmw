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
* Description:  Provides access to Bluetooth "sockets" implemented on top of
 *                Bluetooth Serial Port service.
 *
*/


#ifndef ECMT_BTSOCKET_H
#define ECMT_BTSOCKET_H

// INCLUDES
#include <e32base.h>
#include <es_sock.h>
#include "EcmtBiSocket.h"

// FORWARD DECLARATIONS
class CBTServiceAdvertiser;

/**
 *  @ingroup EcmtBearer
 */

class CBTSocketFactory : public CEcmtBiSocketFactory
{
public:
    CEcmtBiSocket* NewSocketL( );
    ~CBTSocketFactory( );
private:
    RSocketServ iSocketServ;
    TBool iInitialized;
};

/**
 *  Provides access to bluetooth sockets.
 *  @ingroup EcmtBearer
 */
class CBTSocket : public CEcmtBiSocket
{
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CBTSocket* NewL( RSocketServ& aSocketServ );

    /**
     * Destructor.
     */
    virtual ~CBTSocket();

public: // New functions
    
    virtual TInt Open(TBool aBlank);

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

private:

    /**
     * C++ default constructor.
     */
    CBTSocket( RSocketServ& aSocketServ );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:    // Data
    RSocketServ iSocketServ;
    RSocket iSocket;
    HBufC*  iParameterString;
    CBTServiceAdvertiser* iAdvertiser;
};

#endif // ECMT_BTSOCKET_H

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

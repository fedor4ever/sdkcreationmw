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
* Description:  Interface of Bearer Independent sockets
*
*/


#ifndef MECMTMCOMMSBEARERIF_H
#define MECMTMCOMMSBEARERIF_H

//  INCLUDES
#include <e32base.h>

/**
*  @defgroup EcmtBearer Bearer
*  The Bearers are reponsible for implementing a socket like interface on 
*  top of the transport layer they support.
*/

/**
*  Interface of bearer independet sockets.
*  
*  All the beareres suchs as Bluetooth, Win32 socket, and IrDA will implement
*  this interface. The interface correspond closely to the Symbian OS RSocket
*  interface.
*
*  @ingroup EcmtBearer
*  @interface CEcmtBiSocket
*
*/
class CEcmtBiSocket: public CBase
{
public:
    virtual TInt Open(TBool aBlank) = 0;
    virtual void Write(const TDesC8& aDesc, TRequestStatus& aStatus) = 0;
    virtual void CancelWrite() = 0;
        
    virtual void Read(TDes8& aDesc, TRequestStatus& aStatus) = 0;
    virtual void CancelRead() = 0;
        
    virtual TInt Bind() = 0;
    virtual TInt Listen(TUint aSize) = 0;
        
    virtual void Accept(CEcmtBiSocket& aSocket, TRequestStatus& aStatus) = 0;
    virtual void CancelAccept() = 0;
        
    virtual void Close() = 0;
        
    /**
     *  Get the possible parameter string associated with the socket. This can
     *  be the local port number, for example. The core may shows this string
     *  in a GUI. 
     *  @return Descriptor representing a parameter of the socket. The meaning
     *  is defined by the implementation.
     */
    virtual const TPtrC ParameterString() const = 0;
        
    /**
     *  Returns a string describing the address family of the socket. Used to
     *  check that two instances are of the same kind (since reinterpret_cast
     *  is not available in Symbian OS).
     *  @return Descriptor representing the address family of the socket.
     */
    virtual const TPtrC AddressFamily() const = 0;
};

/**
 *  Abstract factory interface for creating CEcmtSocket instances.
 *
 *  @ingroup EcmtBearer
 */
class CEcmtBiSocketFactory: public CBase
{
public:
    /**
     *  Factory method for creating CEcmtBiSocket instances.
     *  @return An instance of a class derived from CEcmtBiSocket
     */
    virtual CEcmtBiSocket* NewSocketL() = 0;
};

#endif
// End of File

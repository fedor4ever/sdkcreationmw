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

#define _WLANSOCKET_DEBUG

#define __S(x) _S(x)
#undef ASSERT

#ifdef _WLANSOCKET_DEBUG

#  ifdef TRACE_PREFIX
#    if defined(_UNICODE)
#      define ___L(a,b) _L(a L##b)
#    else  // _UNICODE
#      define ___L(a,b) _L(a##b)
#    endif // _UNICODE
#    define __L(z) ___L(TRACE_PREFIX,z)
#  else
#    define __L(z) _L(z)
#  endif // TRACE_PREFIX

#  define TRACE(s)                  RDebug::Print(__L("%s"),_S(s))
#  define TRACE0(s)                 RDebug::Print(__L("%s"),_S(s))
#  define TRACE1(s,p1)              RDebug::Print(__L(s),p1)
#  define TRACE2(s,p1,p2)           RDebug::Print(__L(s),p1,p2)
#  define TRACE3(s,p1,p2,p3)        RDebug::Print(__L(s),p1,p2,p3)
#  define TRACE4(s,p1,p2,p3,p4)     RDebug::Print(__L(s),p1,p2,p3,p4)
#  define TRACE5(s,p1,p2,p3,p4,p5)  RDebug::Print(__L(s),p1,p2,p3,p4,p5)

#  define ASSERT(x) \
   ((void)((x)||(CW32SocketDebug::Assert(_S(#x),__S(__FILE__),__LINE__),0),0))
#  define LEAVE_IF_ERROR(x) \
   CW32SocketDebug::LeaveIfError(x,_S(#x),__S(__FILE__),__LINE__)
#  define VERIFY_SUCCESS(x)         ASSERT((x) == KErrNone)

#  define DEBUG_ONLY(x)             x

#endif


#ifndef ECMT_WLANSOCKET_H
#define ECMT_WLANSOCKET_H

// INCLUDES
#include <e32base.h>
#include <es_sock.h>

#include "EcmtBiSocket.h"

// FORWARD DECLARATIONS


/**
 *  @ingroup EcmtBearer
 */

class CWLANSocketFactory : public CEcmtBiSocketFactory
{
public:
    CEcmtBiSocket* NewSocketL( );
    ~CWLANSocketFactory( );
private:
    RSocketServ iSocketServ;
    TBool iInitialized;
};

/**
 *  Provides access to WLAN sockets.
 *  @ingroup EcmtBearer
 */
class CWLANSocket : public CEcmtBiSocket
{
public:  // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CWLANSocket* NewL( RSocketServ& aSocketServ );

    /**
     * Destructor.
     */
    virtual ~CWLANSocket();

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

public:
    TInt LoadIapIdL( TUint32& aIapId );

private:

    /**
     * C++ default constructor.
     */
    CWLANSocket( RSocketServ& aSocketServ );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:    // Data
    RSocketServ iSocketServ;
    RConnection iConnection;
    RSocket iSocket;
    HBufC*  iParameterString;
};

#endif // ECMT_WLANSOCKET_H

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

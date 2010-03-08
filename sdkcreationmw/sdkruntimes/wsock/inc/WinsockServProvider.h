/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef _WSOCK_SERV_PROVIDER_H_
#define _WSOCK_SERV_PROVIDER_H_

#include <nifif.h>
#include <es_prot.h>
#include <in_sock.h>
#include "WinsockSelectThread.h"

// Comms Framework Migration Guide for NIFs and v1.5 PRTs.doc
#define V1_5_PRT_INTERFACE

class CWinsockProtocol;
class CWinsockServProvider : public CServProviderBase
{
private:
    class CWriteData;
    class CWriteRequest;
    class CSelectRequest;
    class CNewDataNotifier;

    friend class CWriteRequest;
    friend class CSelectRequest;
    friend class CNewDataNotifier;

    enum TFlags
    {
        EFlagError        = 0x0001, // A socket error has occured
        EFlagListen       = 0x0002, // This is a listening TCP socket
        EFlagConnect      = 0x0004, // Socket has been connected
        EFlagShutdown     = 0x0008, // Shutdown() has been called
        EFlagEndOfData    = 0x0010, // The other side has close connection
        EFlagDisconnected = 0x0020, // We called iSocket->Disconnect
        EFlagNeedSelect   = 0x0040  // Call Select() from SetNotify
    };

private:
    CWinsockProtocol* iProtocol;
    TSoIfConnectionInfo iConnectionInfo;
    mutable struct _WinsockIpAdapterInfo* iAdapterInfo;
    mutable const struct _WinsockIpAdapterInfo* iNextAdapter;
    mutable const struct _WinsockIpAdapterInfo* iLastAdapter;
    TUint iFamily;                  // The actual address family
    TInt iWinSocket;                // Windows socket handle
    TInetAddr iRemAddr;             // Remote address for connect()
    TBool iFlags;                   // See TFlags
    CWriteRequest* iWrite;          // Asynchronous write requests
    CSelectRequest* iSelect;        // Asynchronous select request
    CNewDataNotifier* iNewData;     // Notifies us that more data is available
    TInt iDataAvailable;            // Number of bytes reported to the socket
    TDblQue<CWriteData> iWriteQ;    // Asynchronous write queue
    TInt iTcpSendWinSize;           // KSoTcpSendWinSize
    TInt iTcpRecvWinSize;           // KSoTcpRecvWinSize
    TInt iTcpMaxSegSize;            // KSoTcpMaxSegSize
    TInt iUdpReceiveICMPError;      // KSoUdpReceiveICMPError
    TInt iUdpRecvBuf;               // KSoUdpRecvBuf
    TInt iUdpSynchronousSend;       // KSoUdpSynchronousSend
#ifdef V1_5_PRT_INTERFACE
    HBufC8* iDataBuf;
#endif

public:
    static CWinsockServProvider* NewL(CWinsockProtocol* aProtocol);
    virtual ~CWinsockServProvider();

    // CServProviderBase
    virtual void Start();
    virtual void SetNotify(MSocketNotify* aSocket);
    virtual void LocalName(TSockAddr& aAddr) const;
    virtual TInt SetLocalName(TSockAddr& aAddr);
    virtual void RemName(TSockAddr& aAddr) const;
    virtual TInt SetRemName(TSockAddr& aAddr);
    virtual TInt GetOption(TUint aLevel,TUint aName,TDes8& anOption)const;
    virtual void Ioctl(TUint aLevel,TUint aName,TDes8* anOption);
    virtual void CancelIoctl(TUint aLevel,TUint aName);
    virtual TInt SetOption(TUint aLevel,TUint aName,const TDesC8 &anOption);
    virtual void ActiveOpen();
    virtual void ActiveOpen(const TDesC8& aConnectionData);
    virtual TInt PassiveOpen(TUint aQueSize);
    virtual TInt PassiveOpen(TUint aQueSize,const TDesC8& aData);
    virtual void Shutdown(TCloseType anOption);
    virtual void Shutdown(TCloseType anOption,const TDesC8& aData);
    virtual void AutoBind();
    virtual TUint Write(const TDesC8& aDesc, TUint options, TSockAddr* anAddr);
#ifndef V1_5_PRT_INTERFACE
    virtual void GetData(TDes8& aDesc, TUint aOptions, TSockAddr* anAddr);
#endif
    virtual TInt GetData(RMBufChain& aData, TUint aLength, TUint aOptions, TSockAddr* anAddr);

private:
    static CWinsockServProvider* FromSocket(CWinsockProtocol* aPr, TInt aSock, TInt aFamily);
    CWinsockServProvider(CWinsockProtocol* aProtocol);
    inline TInt Socket() const;
    TInt Socket(TUint aFamily);
    TUint Family() const;
    int ToSockAddr(union _WSockAddr* aToAddr, const TSockAddr* aFromAddr);
    CSelectRequest* CreateAndSubmitNewRequestL();
    TBool Connect();
    TBool Listen(TInt aBacklog);
    TInt  Select();
    TBool ScheduleWriteRequest(TInt aDelay); // schedules CWriteRequest
    TBool ScheduleNewDataCheck();    // schedules CNewDataNotifier
    void SelectComplete(TInt aMask); // upcall from CSelectRequest
    void NewDataCheck();             // upcall from CNewDataNotifier
    void DoWrite();                  // upcall from CWriteRequest
#ifdef V1_5_PRT_INTERFACE
    TInt Receive(TDes8& aData, TUint aOpt, TSockAddr* aAddr);
#else
    void Receive(TDes8& aDesc, TUint aOptions, TSockAddr* aAddr);
#endif
    TUint SendLater(const TDesC8& aData, TSockAddr* aAddr);
    TUint SendNow(const TDesC8& aData, TSockAddr* aAddr);
};

#endif // _WSOCK_SERV_PROVIDER_H_

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

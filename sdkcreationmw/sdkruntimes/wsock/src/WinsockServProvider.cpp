/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#define TRACE_PREFIX "WSOCK: ServProvider: "
#include <es_mbuf.h>
#include "wsock.h"
#include "WinsockProtocol.h"
#include "WinsockProtocolFamily.h"
#include "WinsockServProvider.h"
#include "WinsockUtils.h"

#define KMaxInetAddrBufSize 64

// CWinsockServProvider::CWriteData
class CWinsockServProvider::CWriteData : public CBase
{
public:
    TDblQueLink iLink;
    TSockAddr* iAddr;
    HBufC8* iData;
public:
    static CWriteData* New(const TDesC8& aData, TSockAddr* aAddr);
    ~CWriteData();
};

// CWinsockServProvider::CWriteRequest
class CWinsockServProvider::CWriteRequest : public CActive
{
private:
    CWinsockServProvider* iProvider;
    RTimer iTimer;
    TBool iTimerCreated;
    TBool iTimerActive;
public:
    CWriteRequest(CWinsockServProvider* aProvider);
    ~CWriteRequest();
    void Submit(TInt aDelay = 0);

    // CActive
    virtual void DoCancel();
    virtual void RunL();
};

// CWinsockServProvider::CSelectRequest
class CWinsockServProvider::CSelectRequest : public CActive,
                                             public MSelectRequest
{
private:
    CWinsockServProvider* iProvider;
    RThread iServerThread;
    TInt iSelectMask;
public:
    static CSelectRequest* NewLC(CWinsockServProvider* aProvider);
    virtual ~CSelectRequest();
    void SubmitL();

    // CActive
    virtual void DoCancel();
    virtual void RunL();

    // MSelectRequest
    virtual TUint Socket();
    virtual TInt SelectMask();
    virtual void SelectComplete(TInt aSelectMask);

private:
    CSelectRequest(CWinsockServProvider* aProvider);
    void ConstructL();
};

// CWinsockServProvider::CNewDataNotifier
class CWinsockServProvider::CNewDataNotifier : public CActive
{
private:
    CWinsockServProvider* iProvider;
public:
    CNewDataNotifier(CWinsockServProvider* aProvider);
    void Submit();

    // CActive
    virtual void DoCancel();
    virtual void RunL();
};

// CWinsockServProvider
#define SUPER CServProviderBase
CWinsockServProvider* CWinsockServProvider::NewL(CWinsockProtocol* aProtocol)
{
    return new(ELeave)CWinsockServProvider(aProtocol);
}

CWinsockServProvider* CWinsockServProvider::FromSocket(CWinsockProtocol* aPro,
                                                       TInt aSocket,
                                                       TInt aFamily)
{
    ASSERT(aFamily != 0);
    ASSERT(aSocket != INVALID_SOCKET);
    CWinsockServProvider* self = new CWinsockServProvider(aPro);
    if (self) {
        self->iWinSocket = aSocket;
        self->iFamily = aFamily;
        self->iFlags |= (EFlagConnect | EFlagNeedSelect);
        // Select will be called from SetNotify. Otherwise, under some rare
        // circumstances select may complete and NewDataCheck is invoked
        // before SetNotify has been invoked and therefore iSocket is still
        // NULL.
    }
    return self;
}

CWinsockServProvider::CWinsockServProvider(CWinsockProtocol* aProtocol) :
iProtocol(aProtocol),
iWinSocket(INVALID_SOCKET),
iWriteQ(_FOFF(CWriteData,iLink)),
iTcpSendWinSize(8192),
iTcpRecvWinSize(8192),
iTcpMaxSegSize(1460),
iUdpRecvBuf(8192)
{
    WTRACE("created");
}

CWinsockServProvider::~CWinsockServProvider()
{
    iProtocol->SetReadPending(this, EFalse);
    if (iWrite)
    {
        iWrite->Cancel();
        delete iWrite;
    }
    if (iNewData)
    {
        iNewData->Cancel();
        delete iNewData;
    }
    if (iSelect)
    {
        CWinsockSelectThread::Static()->Cancel(iSelect);
        iSelect->Cancel();
    }
    if (iAdapterInfo)
    {
        User::Free(iAdapterInfo);
    }
    if (iWinSocket != INVALID_SOCKET)
    {
        closesocket(iWinSocket);
    }
#ifdef V1_5_PRT_INTERFACE
    if (iDataBuf)
    {
        delete iDataBuf;
    }
#endif
    WTRACE("destroyed");
}

// Returns the socket, or creates one using default address family.
TInt CWinsockServProvider::Socket() const
{
    CWinsockServProvider* This = (CWinsockServProvider*)this;
    return This->Socket(Family());
}

// Returns the socket, creating one if necessary. Note that the requested
// address family may not match the protocol family given to us in the
// constructor. That's because some Symbian code (such as HTTP protocol
// framework) may, for example, create an IPv4 socket and then connect
// it to an IPv6 address. Also note that once we have created the Windows
// socket we can't change its address family.
TInt CWinsockServProvider::Socket(TUint aFamily)
{
    if (iWinSocket == INVALID_SOCKET)
    {
        if (!aFamily) aFamily = Family();
        TInt af = (aFamily == KAfInet6) ? AF_INET6 : AF_INET;
        TInt type = iProtocol->ProtocolDesc()->iWinSockType;
        TInt protocol = iProtocol->ProtocolDesc()->iWinProtocol;
        iWinSocket = socket(af, type, protocol);
        if (iWinSocket == INVALID_SOCKET)
        {
            WTRACE4("socket(%d,%d,%d) failed, err %d",af, type, protocol,
            WSAGetLastError());
        }
        else
        {
            // Remember the actual address family
            iFamily = aFamily;
            WTRACE2("created %s %S socket", (af == AF_INET6) ?
                _S("IPv6") : _S("IPv4"), &iProtocol->ProtocolDesc()->iName);

            // Connection-less sockets immediately start accepting
            // incoming datagrams
            if (iProtocol->IsDatagramProtocol())
            {
                Select();
            }
        }
    }
    else
    {
        // This ASSERT means that we have already created the Windows socket
        // and then we were asked to connect or bind it to the address with
        // a different address family. That's bad.
        ASSERT(!aFamily || aFamily == iFamily);
    }
    return iWinSocket;
}

// Returns address family of this socket in Symbian format,
// i.e. KAfInet or KAfInet6
TUint CWinsockServProvider::Family() const
{
    if (iFamily)
    {
        // The socket has already been created
        return iFamily;
    }
    else if (iRemAddr.Family())
    {
        // Assume the same address family as in remote address. That makes
        // URLs like http://[fe80::a00:20ff:fef9:bf8]/ work in the browser.
        // In that case we get the following sequence of calls:
        //    SetRemName -> AutoBind -> ActiveOpen
        // This allows us to create the socket of the right type in AutoBind.
        return iRemAddr.Family();
    }
    else
    {
        // With no additional information, assume the address family from
        // the protocol object.
        return iProtocol->ProtocolDesc()->iAddrFamily;
    }
}

// CServProviderBase
void CWinsockServProvider::Start()
{
    WTRACE("Start()");
}

void CWinsockServProvider::SetNotify(MSocketNotify* aSocket)
{
    WTRACE1("SetNotify(%08X)",aSocket);
    SUPER::SetNotify(aSocket);
    if (iFlags & EFlagNeedSelect)
    {
        iFlags &= ~EFlagNeedSelect;
        Select();
    }
}

typedef int (WSAAPI *GetSockNameProc)(
    SOCKET s,
    struct sockaddr* name,
    int* namelen
);

static void GetSockName(SOCKET sock, TSockAddr& aAddr,
                        GetSockNameProc aFunction,
                        const TText* DEBUG_ONLY(aFunctionName))
{
    WSockAddr sa;
    int namelen = SOCKADDR_SIZE;
    int err = aFunction(sock, &sa.Address, &namelen);
    if (!err)
    {
        WinsockUtils::ToInetAddr(&aAddr,&sa);
    }
    else
    {
        TRACE2("%s failed, error %d\n",aFunctionName,err);
    }
}

void CWinsockServProvider::LocalName(TSockAddr& aAddr) const
{
    if (Socket() == INVALID_SOCKET)
    {
        WTRACE("LocalName() - no socket");
    }
    else
    {
        GetSockName(iWinSocket,aAddr,getsockname,_S("getsockname"));
    }
}

// Wrapper around WinsockUtils::ToSockAddr which attempts to convert
// the address into the form compatible with the address family of
// this socket.
int CWinsockServProvider::ToSockAddr(union _WSockAddr* aToAddr,
                                     const TSockAddr* aFromAddr)
{
    if (Family() == KAfInet6 && aFromAddr->Family() == KAfInet)
    {
        // Convert IPv4 address to IPv4-mapped IPv6 address.
        TInetAddr ipv6(*aFromAddr);
        ipv6.ConvertToV4Mapped();
        ASSERT(ipv6.Family() == KAfInet6);
        return WinsockUtils::ToSockAddr(aToAddr, &ipv6);
    }

    if (Family() == KAfInet && aFromAddr->Family() == KAfInet6)
    {
        TInetAddr& ipv6 = TInetAddr::Cast(aFromAddr);
        if (ipv6.IsV4Mapped() || ipv6.IsV4Compat())
        {
            // Convert IPv4 compatible IPv6 address to IPv4
            TInetAddr ipv4(ipv6);
            ipv4.ConvertToV4();
            return WinsockUtils::ToSockAddr(aToAddr, &ipv4);
        }
    }

    // Do the default conversion
    return WinsockUtils::ToSockAddr(aToAddr, aFromAddr);
}

TInt CWinsockServProvider::SetLocalName(TSockAddr& aAddr)
{
#ifdef _REALLY_DEBUG
    TBuf<KMaxInetAddrBufSize> buf;
    TInetAddr::Cast(aAddr).OutputWithScope(buf);
    if (buf.Length() == 0) buf.Append('*');
    WTRACE2("SetLocalName(%S,%d)",&buf,aAddr.Port());
#endif // _REALLY_DEBUG

    if (Socket(aAddr.Family()) == INVALID_SOCKET)
    {
        WTRACE("SetLocalName - no socket");
        iFlags |= EFlagError;
        return KErrNotSupported;
    }

    // Bind new socket to the specified address
    WSockAddr sa;
    int len = ToSockAddr(&sa, &aAddr);
    int err = bind(iWinSocket, &sa.Address, len);
    if (err)
    {
        WTRACE1("bind failed, err %d",WSAGetLastError());
        return KErrArgument;
    }
    else
    {
#ifdef _REALLY_DEBUG
        TInetAddr localAddr;
        GetSockName(iWinSocket,localAddr,getsockname,_S("getsockname"));
        localAddr.OutputWithScope(buf);
        WTRACE2("bound to %S port %d",&buf,localAddr.Port());
#endif // _REALLY_DEBUG

        return KErrNone;
    }
}

void CWinsockServProvider::RemName(TSockAddr& aAddr) const
{
    if (iFlags & EFlagConnect)
    {
        GetSockName(iWinSocket,aAddr,getpeername,_S("getpeername"));
    }
    else
    {
        aAddr = iRemAddr;
    }
}

TInt CWinsockServProvider::SetRemName(TSockAddr& aAddr)
{
    iRemAddr = TInetAddr::Cast(aAddr);

#ifdef _REALLY_DEBUG
    TBuf<KMaxInetAddrBufSize> buf;
    iRemAddr.OutputWithScope(buf);
    WTRACE2("SetRemName(%S,%d)",&buf,iRemAddr.Port());
#endif // _REALLY_DEBUG

    if (iProtocol->IsDatagramProtocol())
    {
        // application called connect() on datagram-socket. Propagate call to
        // Winsock. (In case of stream-socket, connect() appears to happen
        // ActiveOpen()-call, but that is not called in case of udp-socket.)
        Connect();
    }
    return KErrNone;
}

void CWinsockServProvider::Ioctl(TUint DEBUG_ONLY(aLevel),
                                 TUint DEBUG_ONLY(aName),
                                 TDes8* /*anOption*/)
{
    WTRACE2("Ioctl(%d,%08X)",aLevel,aName);
}

void CWinsockServProvider::CancelIoctl(TUint DEBUG_ONLY(aLevel),
                                       TUint DEBUG_ONLY(aName))
{
    WTRACE2("CancelIoctl(%d,%08X)",aLevel,aName);
}

TInt CWinsockServProvider::SetOption(TUint aLevel, TUint aName,
                                     const TDesC8& aOption)
{
    switch (aLevel)
    {
    case KSOLSocket:
        WTRACE1("SetOption(KSOLSocket,%08X)",aName);
        return KErrNone;

    case KSolInetIfCtrl:
        switch (aName)
        {
        case KSoInetEnumInterfaces:
            WTRACE("SetOption(KSoInetEnumInterfaces)");
            if (iAdapterInfo)
            {
                User::Free(iAdapterInfo);
                iAdapterInfo = NULL;
            }
            iAdapterInfo = iProtocol->GetAdapterInfo();
            iNextAdapter = iAdapterInfo;
            iLastAdapter = NULL;
            return KErrNone;
        default:
            WTRACE1("SetOption(KSolInetIfCtrl,%08X)",aName);
            break;
        }

    case KSOLProvider:
        switch (aName)
        {
        case KSoConnectionInfo:
            iConnectionInfo = *((TSoIfConnectionInfo*)aOption.Ptr());
            WTRACE2("SetOption(KSoConnectionInfo) IAP %d, network %d",
                   iConnectionInfo.iIAPId, iConnectionInfo.iNetworkId);
            return KErrNone;
        default:
            WTRACE1("SetOption(KSOLProvider,%08X) - UNSUPPORTED",aName);
            break;
        }
        break;

    case KSolInetIp:
        switch (aName)
        {
        case KSoIpTTL:
            WTRACE1("SetOption(KSoIpTTL) %d",*((int*)aOption.Ptr()));
            if (!setsockopt(Socket(), IPPROTO_IP, IP_TTL,
                (char*)aOption.Ptr(), aOption.Length()))
            {
                return KErrNone;
            }
            WTRACE1("setsockopt(IP_TTL) error %d",WSAGetLastError());
            break;
        case KSoReuseAddr:
            WTRACE1("SetOption(KSoReuseAddr) %d",*((int*)aOption.Ptr()));
            if (!setsockopt(Socket(), SOL_SOCKET, SO_REUSEADDR,
                (char*)aOption.Ptr(), aOption.Length()))
            {
                return KErrNone;
            }
            WTRACE1("setsockopt(SO_REUSEADDR) error %d",WSAGetLastError());
            break;
        default:
            WTRACE1("SetOption(KSolInetIp,%08X)",aName);
            break;
        }
        break;

    case KSolInetTcp:
        if (iProtocol->ProtocolDesc()->iProtocol == KProtocolInetTcp) {
            switch (aName)
            {
            case KSoTcpSendWinSize:
                WTRACE("SetOption(KSoTcpSendWinSize)");
                if (iFlags & EFlagConnect) return KErrLocked;
                if (aOption.Length() != sizeof(TInt)) return KErrArgument;
                iTcpSendWinSize = *((TInt*)aOption.Ptr());
                return KErrNone;
            case KSoTcpRecvWinSize:
                WTRACE("SetOption(KSoTcpSendWinSize)");
                if (iFlags & EFlagConnect) return KErrLocked;
                if (aOption.Length() != sizeof(TInt)) return KErrArgument;
                iTcpRecvWinSize = *((TInt*)aOption.Ptr());
                return KErrNone;
            case KSoTcpMaxSegSize:
                WTRACE("SetOption(KSoTcpMaxSegSize)");
                if (iFlags & EFlagConnect) return KErrLocked;
                if (aOption.Length() != sizeof(TInt)) return KErrArgument;
                iTcpMaxSegSize = *((TInt*)aOption.Ptr());
                return KErrNone;
            case KSoTcpNoDelay:
                WTRACE1("SetOption(KSoTcpNoDelay) %d",*((TInt*)aOption.Ptr()));
                if (!setsockopt(Socket(), IPPROTO_TCP, TCP_NODELAY,
                    (char*)aOption.Ptr(),aOption.Length())) return KErrNone;
                WTRACE1("setsockopt(TCP_NODELAY) error %d",WSAGetLastError());
                break;
            case KSoTcpKeepAlive:
                WTRACE1("SetOption(KSoTcpKeepAlive) %d",*((TInt*)aOption.Ptr()));
                if (!setsockopt(Socket(), SOL_SOCKET, SO_KEEPALIVE,
                    (char*)aOption.Ptr(),aOption.Length())) return KErrNone;
                WTRACE1("setsockopt(SO_KEEPALIVE) error %d",WSAGetLastError());
                break;
            default:
                WTRACE1("SetOption(KSolInetTcp,%08X) - UNSUPPORTED",aName);
                break;
            }
        } else {
            WTRACE1("SetOption(KSolInetTcp,%08X) - not a TCP socket!",aName);
        }
        break;

    case KSolInetUdp:
        if (iProtocol->ProtocolDesc()->iProtocol == KProtocolInetUdp) {
            switch (aName)
            {
            case KSoUdpReceiveICMPError:
                WTRACE("SetOption(KSoUdpReceiveICMPError)");
                if (aOption.Length() != sizeof(TInt)) return KErrArgument;
                iUdpReceiveICMPError = *((TInt*)aOption.Ptr());
                return KErrNone;
            case KSoUdpRecvBuf:
                WTRACE("SetOption(KSoUdpRecvBuf)");
                if (aOption.Length() != sizeof(TInt)) return KErrArgument;
                iUdpRecvBuf = *((TInt*)aOption.Ptr());
                return KErrNone;
            case KSoUdpSynchronousSend:
                WTRACE("SetOption(KSoUdpSynchronousSend)");
                if (aOption.Length() != sizeof(TInt)) return KErrArgument;
                iUdpSynchronousSend = *((TInt*)aOption.Ptr());
                return KErrNone;
            default:
                WTRACE1("SetOption(KSolInetUdp,%08X) - UNSUPPORTED",aName);
                break;
            }
        } else {
            WTRACE1("SetOption(KSolInetUdp,%08X) - not a UDP socket!",aName);
        }
        break;

    default:
        WTRACE2("SetOption(%d,%08X) - UNSUPPORTED",aLevel,aName);
        break;
    }
    return KErrNotSupported;
}

TInt CWinsockServProvider::GetOption(TUint aLevel, TUint aName,
                                     TDes8& aOption) const
{
    TInt len;
    switch (aLevel)
    {
    case KSOLSocket:
        switch (aName)
        {
        // This one used by SIP code:
        case KSOReadBytesPending:
            if (aOption.MaxLength() < sizeof(TInt)) return KErrArgument;
            aOption.SetLength(sizeof(TInt));
            if (!ioctlsocket(Socket(), FIONREAD, (u_long*)aOption.Ptr()))
            {
                WTRACE1("GetOption(KSOReadBytesPending) %d",
                    *((TInt*)aOption.Ptr()));
                return KErrNone;
            }
            WTRACE1("GetOption(KSOReadBytesPending) - ioctl(FIONREAD) err %d",
                WSAGetLastError());
            break;

        case KSORecvBuf:
        case KSOSendBuf:
            WTRACE1("GetOption(KSOLSocket,%08X)",aName);
            if (aOption.MaxLength() < sizeof(TInt)) return KErrArgument;
            aOption.SetLength(sizeof(TInt));
            *((TInt*)aOption.Ptr()) = KSocketDefaultBufferSize;
            return KErrNone;

        default:
            WTRACE1("GetOption(KSOLSocket,%08X) - UNSUPPORTED",aName);
            break;
        }
        break;

    case KSolInetIfCtrl:
        switch (aName)
        {
        case KSoInetNextInterface:
            WTRACE("GetOption(KSoInetNextInterface)");
            iLastAdapter = iNextAdapter;
            if (iLastAdapter)
            {
                iNextAdapter = iLastAdapter->Next;

                // Copy interface information into the output buffer
                TSoInetInterfaceInfo *opt;
                aOption.SetLength(sizeof(*opt));
                opt = (TSoInetInterfaceInfo*)aOption.Ptr();
                opt->iName.Copy(TPtrC8(iLastAdapter->AdapterName));
                const WinsockIpAddrString*adr = iLastAdapter->CurrentIpAddress;
                if (!adr) adr = &iLastAdapter->IpAddressList;
                TUint ip = inet_addr((char*)adr->IpAddress.String);
                TUint mask = inet_addr((char*)adr->IpMask.String);
                opt->iAddress.SetAddress(ntohl(ip));
                opt->iNetMask.SetAddress(ntohl(mask));
                opt->iState = EIfUp;
                opt->iMtu = 1500;
                opt->iSpeedMetric = 10000;
                opt->iFeatures = 0;
                return KErrNone;
            }
            else
            {
                return KErrNotFound;
            }
        default:
            WTRACE1("GetOption(KSolInetIfCtrl,%08X)",aName);
            break;
        }
        break;

    case KSolInetIfQuery:
        switch (aName)
        {
        case KSoInetIfQueryByName:
            WTRACE("GetOption(KSoInetIfQueryByName)");
            if (!iAdapterInfo)
            {
                iAdapterInfo = iProtocol->GetAdapterInfo();
            }
            if (iAdapterInfo)
            {
                TSoInetIfQuery* query = (TSoInetIfQuery*)aOption.Ptr();
                WinsockIpAdapterInfo* adapter = iAdapterInfo;
                while (adapter)
                {
                    TUint len = User::StringLength(adapter->AdapterName);
                    HBufC* buf = HBufC::New(len);
                    if (buf)
                    {
                        TPtr16 ptr(buf->Des());
                        ptr.Copy(TPtrC8(adapter->AdapterName));
                        if (ptr.Compare(query->iName) == 0)
                        {
                            query->iIndex = adapter->Index;
                            query->iZone[0] = adapter->Index;
                            query->iZone[1] = iConnectionInfo.iIAPId;
                            query->iZone[15] = iConnectionInfo.iNetworkId;
                            for (TInt i=2; i<15; i++) query->iZone[i] = 0;
                            query->iIsUp = ETrue;
                            delete buf;
                            return KErrNone;
                        }
                        delete buf;
                    }
                    adapter = adapter->Next;
                }
            }
            return KErrNotFound;
        default:
            WTRACE1("GetOption(KSolInetIfQuery,%08X)",aName);
            break;
        }
        break;

    case KSolInetIp:
        switch (aName)
        {
        case KSoIpTTL:
            WTRACE("GetOption(KSoIpTTL)");
            if (aOption.MaxLength() < sizeof(TInt)) return KErrArgument;
            aOption.SetLength(len = sizeof(TInt));
            if (!getsockopt(Socket(), IPPROTO_IP, IP_TTL,
                (char*)aOption.Ptr(), &len))
            {
                aOption.SetLength(len);
                return KErrNone;
            }
            WTRACE1("getsockopt(IP_TTL) error %d",WSAGetLastError());
            break;
        case KSoReuseAddr:
            WTRACE("GetOption(KSoReuseAddr)");
            if (aOption.MaxLength() < sizeof(TInt)) return KErrArgument;
            aOption.SetLength(len = sizeof(TInt));
            if (!getsockopt(Socket(), SOL_SOCKET, SO_REUSEADDR,
                (char*)aOption.Ptr(), &len))
            {
                return KErrNone;
            }
            WTRACE1("getsockopt(SO_REUSEADDR) error %d",WSAGetLastError());
            break;
        default:
            WTRACE1("SetOption(KSolInetIp,%08X)",aName);
            break;
        }
        break;

    case KSolInetTcp:
        if (iProtocol->ProtocolDesc()->iProtocol == KProtocolInetTcp) {
            switch (aName)
            {
            case KSoTcpSendWinSize:
                WTRACE("GetOption(KSoTcpSendWinSize)");
                if (aOption.MaxLength() < sizeof(TInt)) return KErrArgument;
                aOption.SetLength(sizeof(TInt));
                *((TInt*)aOption.Ptr()) = iTcpSendWinSize;
                return KErrNone;
            case KSoTcpRecvWinSize:
                WTRACE("GetOption(KSoTcpSendWinSize)");
                if (aOption.MaxLength() < sizeof(TInt)) return KErrArgument;
                aOption.SetLength(sizeof(TInt));
                *((TInt*)aOption.Ptr()) = iTcpRecvWinSize;
                return KErrNone;
            case KSoTcpMaxSegSize:
                WTRACE("GetOption(KSoTcpMaxSegSize)");
                if (aOption.MaxLength() < sizeof(TInt)) return KErrArgument;
                aOption.SetLength(sizeof(TInt));
                *((TInt*)aOption.Ptr()) = iTcpMaxSegSize;
                return KErrNone;
            case KSoTcpNoDelay:
                WTRACE("GetOption(KSoTcpNoDelay)");
                if (aOption.MaxLength() < sizeof(TInt)) return KErrArgument;
                aOption.SetLength(len = sizeof(TInt));
                if (!getsockopt(Socket(), IPPROTO_TCP, TCP_NODELAY,
                    (char*)aOption.Ptr(),&len))
                {
                    aOption.SetLength(len);
                    return KErrNone;
                }
                WTRACE1("getsockopt(TCP_NODELAY) error %d",WSAGetLastError());
                break;
            case KSoTcpKeepAlive:
                WTRACE("GetOption(KSoTcpKeepAlive)");
                if (aOption.MaxLength() < sizeof(TInt)) return KErrArgument;
                aOption.SetLength(len = sizeof(TInt));
                if (!getsockopt(Socket(), SOL_SOCKET, SO_KEEPALIVE,
                    (char*)aOption.Ptr(),&len))
                {
                    aOption.SetLength(len);
                    return KErrNone;
                }
                WTRACE1("getsockopt(SO_KEEPALIVE) error %d",WSAGetLastError());
                break;
            case KSoTcpSendBytesPending:
                WTRACE("GetOption(KSoTcpSendBytesPending)");
                if (aOption.MaxLength() < sizeof(TInt)) return KErrArgument;
                aOption.FillZ(sizeof(TInt));
                return KErrNone;
            // This is a useful one:
            case KSoTcpReadBytesPending:
                WTRACE("GetOption(KSoTcpReadBytesPending)");
                if (aOption.MaxLength() < sizeof(TInt)) return KErrArgument;
                aOption.SetLength(sizeof(TInt));
                if (!ioctlsocket(Socket(), FIONREAD, (u_long*)aOption.Ptr()))
                {
                    WTRACE1("%d bytes in socket",*((TInt*)aOption.Ptr()));
                    return KErrNone;
                }
                WTRACE1("ioctl(FIONREAD) error %d",WSAGetLastError());
                break;
            case KSoTcpListening:
                WTRACE("GetOption(KSoTcpListening)");
                if (aOption.MaxLength() < sizeof(TInt)) return KErrArgument;
                aOption.SetLength(sizeof(TInt));
                *((TInt*)aOption.Ptr()) = (iFlags & EFlagListen) ? 1 : 0;
                return KErrNone;
            case KSoTcpNumSockets:
                WTRACE("GetOption(KSoTcpNumSockets)");
                if (aOption.MaxLength() < sizeof(TInt)) return KErrArgument;
                aOption.SetLength(sizeof(TInt));
                *((TInt*)aOption.Ptr()) = 1; 
                return KErrNone;
            default:
                WTRACE1("SetOption(KSolInetTcp,%08X) - UNSUPPORTED",aName);
                break;
            }
        } else {
            WTRACE1("SetOption(KSolInetTcp,%08X) - not a TCP socket!",aName);
        }
        break;

    case KSolInetUdp:
        if (iProtocol->ProtocolDesc()->iProtocol == KProtocolInetUdp) {
            switch (aName)
            {
            case KSoUdpReceiveICMPError:
                WTRACE("SetOption(KSoUdpReceiveICMPError)");
                if (aOption.MaxLength() < sizeof(TInt)) return KErrArgument;
                aOption.SetLength(sizeof(TInt));
                *((TInt*)aOption.Ptr()) = iUdpReceiveICMPError;
                return KErrNone;
            case KSoUdpRecvBuf:
                WTRACE("SetOption(KSoUdpRecvBuf)");
                if (aOption.MaxLength() < sizeof(TInt)) return KErrArgument;
                aOption.SetLength(sizeof(TInt));
                *((TInt*)aOption.Ptr()) = iUdpRecvBuf;
                return KErrNone;
            case KSoUdpSynchronousSend:
                WTRACE("SetOption(KSoUdpSynchronousSend)");
                if (aOption.MaxLength() < sizeof(TInt)) return KErrArgument;
                aOption.SetLength(sizeof(TInt));
                *((TInt*)aOption.Ptr()) = iUdpSynchronousSend;
                return KErrNone;
            default:
                WTRACE1("SetOption(KSolInetUdp,%08X) - UNSUPPORTED",aName);
                break;
            }
        } else {
            WTRACE1("SetOption(KSolInetUdp,%08X) - not a UDP socket!",aName);
        }
        break;

    default:
        WTRACE2("GetOption(%d,%08X) - UNSUPPORTED",aLevel,aName);
        break;
    }
    aOption.SetLength(0);
    return KErrNotSupported;
}

TBool CWinsockServProvider::Connect()
{
    if (Socket(iRemAddr.Family()) == INVALID_SOCKET)
    {
        WTRACE("can't connect, no socket");
        iFlags |= EFlagError;
        iSocket->Error(KErrCouldNotConnect);
        return EFalse;
    }
    else
    {
        WSockAddr sa;
        int len = ToSockAddr(&sa, &iRemAddr);
        int err = connect(iWinSocket, &sa.Address, len);
        if (err)
        {
            WTRACE1("connect failed, err %d",WSAGetLastError());
            switch (WSAGetLastError())
            {
            case WSAENETUNREACH:
                iSocket->Error(KErrNetUnreach);
                break;
            case WSAEHOSTUNREACH:
                iSocket->Error(KErrHostUnreach);
                break;
            default:
                iSocket->Error(KErrCouldNotConnect);
                break;
            }
            iFlags |= EFlagError;
            return EFalse;
        }
        else
        {
            iFlags |= EFlagConnect;
            return ETrue;
        }
    }
}

TInt CWinsockServProvider::Listen(TInt aBacklog)
{
    ASSERT(iProtocol->IsStreamProtocol());
    if (Socket() == INVALID_SOCKET)
    {
        WTRACE("can't listen, no socket");
        return KErrGeneral;
    }
    else
    {
        int err = listen(iWinSocket, aBacklog);
        if (err)
        {
            WTRACE1("listen failed, err %d",WSAGetLastError());
            return KErrArgument;
        }
        else
        {
            return KErrNone;
        }
    }
}

// Creates and submits a new request
CWinsockServProvider::CSelectRequest*
CWinsockServProvider::CreateAndSubmitNewRequestL()
{
    CSelectRequest* request = CSelectRequest::NewLC(this);
    request->SubmitL();
    CleanupStack::Pop(request);
    return request;
}

// Schedules CWriteRequest
TBool CWinsockServProvider::ScheduleWriteRequest(TInt aDelay)
{
    if (!iWrite)
    {
        iWrite = new CWriteRequest(this);
        if (!iWrite)
        {
            return EFalse;
        }
    }
    iWrite->Submit(aDelay);
    return ETrue;
}

// Schedules CNewDataNotifier
TBool CWinsockServProvider::ScheduleNewDataCheck()
{
    if (!iNewData)
    {
        iNewData = new CNewDataNotifier(this);
        if (!iNewData)
        {
            return EFalse;
        }
    }
    iNewData->Submit();
    return ETrue;
}

TInt CWinsockServProvider::Select()
{
    TInt err = KErrNone;
    if (!iSelect)
    {
        TRAP(err,iSelect = CreateAndSubmitNewRequestL());
    }

    ASSERT(iSelect || err != KErrNone);
    if (iSelect && !iSelect->IsActive())
    {
        TRAP(err, iSelect->SubmitL());
    }
    return err;
}

void CWinsockServProvider::ActiveOpen()
{
#ifdef _REALLY_DEBUG
    TBuf<KMaxInetAddrBufSize> buf;
    iRemAddr.OutputWithScope(buf);
    WTRACE2("ActiveOpen(%S,%d)",&buf,iRemAddr.Port());
#endif // _REALLY_DEBUG
    ASSERT(iProtocol->IsStreamProtocol());
    if (Connect())
    {
        iSocket->ConnectComplete();
        TInt err = Select();
        if (err != KErrNone)
        {
            iFlags |= EFlagError;
            iSocket->Error(err);
        }
    }
}

void CWinsockServProvider::ActiveOpen(const TDesC8& /*aConnectionData*/)
{
    ASSERT(FALSE);
    iSocket->Error(KErrNotSupported);
}

TInt CWinsockServProvider::PassiveOpen(TUint aQueSize)
{
    WTRACE1("PassiveOpen(%d)",aQueSize);
    ASSERT(iProtocol->IsStreamProtocol());
    TInt err = Listen(aQueSize);
    if (err == KErrNone)
    {
        iFlags |= EFlagListen;
        err = Select();
    }
    return err;
}

TInt CWinsockServProvider::PassiveOpen(TUint aQueSize,const TDesC8& /*aData*/)
{
    ASSERT(FALSE);
    return PassiveOpen(aQueSize);
}

void CWinsockServProvider::Shutdown(TCloseType anOption)
{
    WTRACE1("Shutdown(%08X)",anOption);
    int how;
    switch (anOption)
    {
    default:
    case EImmediate:
    case ENormal: how = SD_BOTH; break;
    case EStopInput: how = SD_RECEIVE; break;
    case EStopOutput: how = SD_SEND; break;
    }
    if (iWinSocket != INVALID_SOCKET)
    {
        shutdown(iWinSocket, how);
    }
    iFlags |= EFlagShutdown;
    if (!(iFlags & EFlagError) && anOption != EImmediate)
    {
        // CSocket panics if we call CanClose() after reporting an error.
        // It also gets pretty upset if we call CanClose() from Shutdown
        // call with EImmediate parameter.
        iSocket->CanClose();
    }
}

void CWinsockServProvider::Shutdown(TCloseType anOption,const TDesC8& /*aData*/)
{
    Shutdown(anOption);
}

void CWinsockServProvider::AutoBind()
{
    if (Socket() == INVALID_SOCKET)
    {
        WTRACE("AutoBind - no socket");
    }
    else
    {
        WTRACE("AutoBind");
        TInetAddr bindAddr;
        bindAddr.Init(Family());
        WSockAddr sa;
        int len = WinsockUtils::ToSockAddr(&sa, &bindAddr);
        int err = bind(iWinSocket, &sa.Address, len);
        if (err)
        {
            WTRACE1("bind failed, err %d",WSAGetLastError());
            iFlags |= EFlagError;
            iSocket->Error(err);
        }
    }
}

// The reason for not writing the data immediately is that socket server
// is processing both read and write requests on the same thread. If we
// are writing data faster than we are reading them, eventually send
// will block forever. We try to give the reader a chance.
void CWinsockServProvider::DoWrite()
{
    if (!iWriteQ.IsEmpty())
    {
        ASSERT(iWinSocket != INVALID_SOCKET);

        // Check if the socket is writable
        TBool canWrite = ETrue;
        fd_set writefs;
        FD_ZERO(&writefs);
        FD_SET(iWinSocket,&writefs);
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 0;

        BEGIN_WIN32();
        int nfd = select(iWinSocket+1, NULL, &writefs, NULL, &tv);
        END_WIN32();

        if (nfd == SOCKET_ERROR)
        {
            WTRACE1("select err %d",WSAGetLastError());
            iSocket->Error(KErrWrite);
        }
        else
        {
            if (FD_ISSET(iWinSocket,&writefs))
            {
                // Send exactly one block of data
                CWriteData* data = iWriteQ.First();
                data->iLink.Deque();
                TUint nBytes = SendNow(*data->iData, data->iAddr);
                if (nBytes)
                {
                    WTRACE1("sent %d bytes",nBytes);
                    iProtocol->DataSent(nBytes);
                }
                delete data;

                // Have more data to send?
                if (!iWriteQ.IsEmpty())
                {
                    ScheduleWriteRequest(0);
                }
            }
            else
            {
                // Delay the retry to ease the CPU load and give the
                // reader a chance to catch up
                WTRACE("socket is not writable, waiting...");
                ScheduleWriteRequest(500000);
            }
        }
    }
}

TUint CWinsockServProvider::SendLater(const TDesC8& aData, TSockAddr* aAddr)
{
    CWriteData* data = CWriteData::New(aData, aAddr);
    if (data)
    {
        iWriteQ.AddLast(*data);
        ScheduleWriteRequest(0);
        return aData.Length();
    }
    return 0;
}

TUint CWinsockServProvider::SendNow(const TDesC8& aData, TSockAddr* aAddr)
{
    int nbytes;
    const char * buf = (char*)aData.Ptr();
    int len = aData.Length();
    if (iFlags & EFlagConnect)
    {
        BEGIN_WIN32();
        nbytes = send(iWinSocket, buf, len, 0);
        END_WIN32();
    }
    else
    {
        ASSERT(aAddr);
        WSockAddr to;
        int tolen = WinsockUtils::ToSockAddr(&to, aAddr);

        BEGIN_WIN32();
        nbytes = sendto(Socket(aAddr->Family()),buf,len,0,&to.Address,tolen);
        END_WIN32();

        if (nbytes < len) nbytes = 0;
    }
    if (nbytes == SOCKET_ERROR)
    {
        WTRACE1("send failed, err %d",WSAGetLastError());
        iSocket->Error(KErrWrite);
        return 0;
    }
    else
    {
        if (nbytes < len)
        {
            iSocket->Error(KErrWrite);
        }
        return nbytes;
    }
}

TUint CWinsockServProvider::Write(const TDesC8& aData, TUint, TSockAddr* aAddr)
{
    if (Socket() == INVALID_SOCKET)
    {
        WTRACE("Write - no socket");
        iSocket->Error(KErrWrite);
        return 0;
    }
    else
    {
        WTRACE1("sending %d bytes",aData.Length());
        if (!iWriteQ.IsEmpty() || iProtocol->ReadPending())
        {
            // If have asynchronous writes pending, we have to send this
            // chunk of data asynchronously too, otherwise the order of
            // chunks will change. If there's any unread data in one of
            // the sockets, we also want to be asynchronous to avoid
            // starving the reading thread, which can result in filling
            // up TCP buffer, blocking socket server thread and finally
            // deadlock.
            return SendLater(aData, aAddr);
        }
        else
        {
            // No read or writes pending, send it now.
            return SendNow(aData, aAddr);
        }
    }
}

#ifdef V1_5_PRT_INTERFACE

TInt CWinsockServProvider::Receive(TDes8& aData, TUint aOpt, TSockAddr* aAddr)
{
    ASSERT(aData.Length() > 0);
    ASSERT(!(iFlags & (EFlagEndOfData | EFlagShutdown)));

    WSockAddr from;
    int maxbytes = Min(aData.Length(), iDataAvailable);
    char* buf = (char*)&aData[0];

    int retval;     // Return value from recv or recvfrom
    int nbytes;     // How much is actually in the buffer

    ASSERT(!(aOpt & KIpHeaderIncluded));        // Option is not implemented
    // ASSERT(!(aOpt & KSockReadContinuation)); // ignore, does not affect operation
    int opt = ((aOpt & KSockReadPeek) ? MSG_PEEK : 0);

    if (aAddr)
    {
        int fromlen = SOCKADDR_SIZE;
        retval = recvfrom(Socket(), buf, maxbytes, opt, (struct sockaddr*)
            &from, &fromlen);
    }
    else
    {
        // No address information is required, probably a TCP socket
        retval = recv(Socket(), buf, maxbytes, opt);
    }

    if (retval == SOCKET_ERROR && WSAGetLastError() == WSAEMSGSIZE)
    {
        ASSERT( EFalse ); // should not happen anymore; whole datagram is read
        WTRACE1("datagram truncated to %d bytes",maxbytes);
        nbytes = maxbytes;
    }
    else
    {
        nbytes = retval;
    }

    if (nbytes == SOCKET_ERROR)
    {
        WTRACE1("recv err %d",WSAGetLastError());
        iFlags |= EFlagError;
        aData.SetLength(0);
        retval = (iProtocol->IsStreamProtocol()) ? KErrDisconnected : KErrGeneral;
    }
    else if (retval == 0)
    {
        // the connection has been gracefully closed
        iFlags |= EFlagEndOfData;
        aData.SetLength(0);
        WTRACE("end of data");
        ScheduleNewDataCheck();
    }
    else
    {
        if (aAddr) WinsockUtils::ToInetAddr(aAddr, &from);
        aData.SetLength(nbytes);

        if (iProtocol->IsStreamProtocol())
        {
            ASSERT(iDataAvailable >= nbytes);
            iDataAvailable -= nbytes;
        }
        else
        {
            // In case of UDP we always indicate no more than one datagram.
            // After socket server has called our GetData function, it thinks
            // that it has received the only available datagram and that we
            // no longer have any data in the receive queue. We need
            // to reset iDataAvailable to zero and start from scratch.
            iDataAvailable = 0;
            
            retval = 1; // indicate, that one datagram was read
        }

        if (iDataAvailable <= 0)
        {
            // Find out how much data still left
            TUint32 size = 0;
            int err = ioctlsocket(iWinSocket, FIONREAD, &size);
            iProtocol->SetReadPending(this, !err && (size > 0));
            if (err == SOCKET_ERROR)
            {
                WTRACE1("ioctl(FIONREAD) err %d",WSAGetLastError());
                iFlags |= EFlagError;
                retval = KErrGeneral;
            }
            else
            {
                TInt bytesInSocket = size;
                WTRACE2("%d bytes in socket, %d bytes unread",
                    bytesInSocket, iDataAvailable);
                if (bytesInSocket > iDataAvailable)
                {
                    // iSocket->NewData is not reentrant. If we call it from
                    // here, it will get into an infinite loop
                    WTRACE("scheduling new data check");
                    ScheduleNewDataCheck();
                }
                else
                {
                    // Re-submit the select request
                    TInt err = Select();
                    if (err != KErrNone)
                    {
                        iFlags |= EFlagError;
                        retval = KErrGeneral;
                    }
                }
            }
        }
    }
    return retval; // error-code, nbytes (if stream) or 1 (if dgram)
}

TInt CWinsockServProvider::GetData(RMBufChain& aData, TUint aLength,
                                   TUint aOptions, TSockAddr* aAddr)
{
    if (!iDataAvailable)
    {
        // This should only happen for datagram protocols
        WTRACE1("%d bytes requested, nothing is available",
               aLength & ~KGetDataWholeDatagram);
        ASSERT(iProtocol->IsDatagramProtocol());
        return KErrNotReady;
    }
    else if (Socket() == INVALID_SOCKET)
    {
        WTRACE("GetData - no socket");
        return KErrGeneral;
    }
    else
    {
        WTRACE2("%d bytes requested, %d bytes unread", 
               (aLength & ~KGetDataWholeDatagram), iDataAvailable);

        if (aLength & KGetDataWholeDatagram)
        {
            // we're asked to read the whole dgram, even if client wants less
            aLength &= ~KGetDataWholeDatagram;
            aLength = Max(aLength, iDataAvailable);
        }
        
        // alloc buffer
        TInt extraNeeded = aLength - aData.Length();
        if (extraNeeded > 0)
        {
            TRAPD(err, aData.AppendL(extraNeeded));
            if (err != KErrNone)
            {
                return KErrNoMBufs;
            }
        }

        // the new GetData-inteface with RMBufChain-param (i.e. v1.5 PRT inter-
        // face) is somewhat problematic from Winsock's point of view.
        // RMBufChain is essentially a linked list of buffers. But especially
        // when UDP-datagram bigger than the a single buffer is being read,
        // the windows-socket recvfrom-operation has to be done in a single
        // operation - otherwise datagram gets truncated.
        // Therefore we have to have a local buffer where to read the data - 
        // and then copy it into RMBufChain.
        //
        // reference: \src\common\generic\comms-infras\documentation\
        // Comms Framework Migration Guide for NIFs and v1.5 PRTs.doc
        //
        // The code below is more or less copied from 
        // \src\common\generic\comms-infras\esock\ssock\SS_PROT.CPP

        if(!iDataBuf)
        {
            iDataBuf = HBufC8::New(aLength);
            if(!iDataBuf)
            {
                return KErrNoMemory;
            }
        }
        else if(iDataBuf->Size() < TInt(aLength))
        {
            // We need extra room for data. As ReAlloc returns NULL if fails, 
            // we need to temporary save the previous storage in order to 
            // clear it in the case of ReAlloc failure.
            HBufC8* aTempPtr = iDataBuf;
            iDataBuf = iDataBuf->ReAlloc(aLength);
            if(!iDataBuf)
            {
                delete aTempPtr;
                return KErrNoMemory;
            }
        }
        TPtr8 des = iDataBuf->Des();
        des.SetLength(aLength);

        TInt retval = Receive( des, aOptions, aAddr );

        TInt bytesReceived = des.Length();
        aData.CopyIn(des);

        if(des.Length() < TInt(aLength))
        {
            aData.TrimEnd(des.Length());
        }
        WTRACE2("received %d bytes, %d bytes left", bytesReceived, iDataAvailable);
        if (bytesReceived > 0)
        {
            iProtocol->DataReceived(bytesReceived);
        }

        return retval;
    }
}

#else // PRT v1.0 interface

void CWinsockServProvider::Receive(TDes8& aDesc, TUint aOpt, TSockAddr* aAddr)
{
    ASSERT(aDesc.Length() > 0);
    ASSERT(!(iFlags & (EFlagEndOfData | EFlagShutdown)));

    WSockAddr from;
    int maxbytes = Min(aDesc.Length(), iDataAvailable);
    char* buf = (char*)&aDesc[0];

    int retval;     // Return value from recv or recvfrom
    int nbytes;     // How much is actually in the buffer

    ASSERT(!(aOpt & KIpHeaderIncluded));            // Option is not implemented
    ASSERT(!(aOpt & KSockReadContinuation));        // Option is not implemented
    int opt = ((aOpt & KSockReadPeek) ? MSG_PEEK : 0);

    if (aAddr)
    {
        int fromlen = SOCKADDR_SIZE;
        retval = recvfrom(Socket(), buf, maxbytes, opt, (struct sockaddr*)
            &from, &fromlen);
    }
    else
    {
        // No address information is required, probably a TCP socket
        retval = recv(Socket(), buf, maxbytes, opt);
    }

    // How to handle truncated datagrams?
    if (retval == SOCKET_ERROR && WSAGetLastError() == WSAEMSGSIZE)
    {
        WTRACE1("datagram truncated to %d bytes",maxbytes);
        nbytes = maxbytes;
    }
    else
    {
        nbytes = retval;
    }

    if (nbytes == SOCKET_ERROR)
    {
        WTRACE1("recv err %d",WSAGetLastError());
        iFlags |= EFlagError;
        aDesc.SetLength(0);
        if (iProtocol->IsStreamProtocol())
        {
            iSocket->Error(KErrDisconnected);
        }
        else
        {
            iSocket->Error(KErrGeneral);
        }
    }
    else if (retval == 0)
    {
        // the connection has been gracefully closed
        iFlags |= EFlagEndOfData;
        aDesc.SetLength(0);
        WTRACE("end of data");
        ScheduleNewDataCheck();
    }
    else
    {
        if (aAddr) WinsockUtils::ToInetAddr(aAddr, &from);
        aDesc.SetLength(nbytes);

        if (iProtocol->IsStreamProtocol())
        {
            ASSERT(iDataAvailable >= nbytes);
            iDataAvailable -= nbytes;
        }
        else
        {
            // In case of UDP we always indicate no more than one datagram.
            // After socket server has called our GetData function, it thinks
            // that it has received the only available datagram and that we
            // no longer have any data in the receive queue. Also, datagram
            // may have been truncated and in that case we don't really know
            // how much data are still sitting in the receive queue. We need
            // to reset iDataAvailable to zero and start from scratch.
            iDataAvailable = 0;
        }
        if (iDataAvailable <= 0)
        {
            // Find out how much data still left
            TUint32 size = 0;
            int err = ioctlsocket(iWinSocket, FIONREAD, &size);
            iProtocol->SetReadPending(this, !err && (size > 0));
            if (err == SOCKET_ERROR)
            {
                WTRACE1("ioctl(FIONREAD) err %d",WSAGetLastError());
                iFlags |= EFlagError;
                iSocket->Error(KErrGeneral);
            }
            else
            {
                TInt bytesInSocket = size;
                WTRACE2("%d bytes in socket, %d bytes unread",
                    bytesInSocket, iDataAvailable);
                if (bytesInSocket > iDataAvailable)
                {
                    // iSocket->NewData is not reentrant. If we call it from
                    // here, it will get into an infinite loop
                    WTRACE("scheduling new data check");
                    ScheduleNewDataCheck();
                }
                else
                {
                    // Re-submit the select request
                    TInt err = Select();
                    if (err != KErrNone)
                    {
                        iFlags |= EFlagError;
                        iSocket->Error(KErrGeneral);
                    }
                }
            }
        }
    }
}

void CWinsockServProvider::GetData(TDes8& aDesc, TUint aOptions, TSockAddr* aAddr)
{
    if (Socket() == INVALID_SOCKET)
    {
        WTRACE("GetData - no socket");
        iSocket->Error(KErrGeneral);
    }
    else
    {
        WTRACE2("%d bytes requested, %d bytes unread",
               aDesc.Length(),iDataAvailable);
        if (!iDataAvailable)
        {
            aDesc.SetLength(0);
        }
        else
        {
            Receive(aDesc, aOptions, aAddr);
            TInt bytesReceived = aDesc.Length();
            WTRACE2("received %d bytes, %d bytes left",
                   bytesReceived, iDataAvailable);
            if (bytesReceived > 0)
            {
                iProtocol->DataReceived(bytesReceived);
            }
        }
    }
}

TInt CWinsockServProvider::GetData(RMBufChain& aData, TUint aLength,
                                   TUint aOptions, TSockAddr* anAddr)
{
    if (!iDataAvailable)
    {
        // This should only happen for datagram protocols
        WTRACE1("%d bytes requested, nothing is available",
               aLength & ~KGetDataWholeDatagram);
        ASSERT(iProtocol->IsDatagramProtocol());
        return KErrNotReady;
    }

    // The base class never returns an error
    TInt ret = SUPER::GetData(aData, aLength, aOptions, anAddr);
    if (iFlags & EFlagError)
    {
        if (iProtocol->IsDatagramProtocol())
        {
            return KErrGeneral;
        }
        else
        {
            return KErrDisconnected;
        }
    }
    else
    {
        return ret;
    }
}

#endif // V1_5_PRT_INTERFACE

// upcall from CSelectRequest invoked in the context of the thread
// submitted the request
void CWinsockServProvider::SelectComplete(TInt aMask)
{
    if (aMask & ESelectError)
    {
        if (!(iFlags & EFlagError))
        {
            iFlags |= EFlagError;
            iSocket->Error(KErrDisconnected);
        }
    }
    else if (!(iFlags & EFlagError) && (aMask & ESelectRead))
    {
        if (iFlags & EFlagListen)
        {
            // Accept new connection
            WSockAddr addr;
            int addrlen = SOCKADDR_SIZE;
            SOCKET sock = accept(iWinSocket,&addr.Address,&addrlen);
            if (sock == INVALID_SOCKET)
            {
                // ignore this error
                WTRACE1("accept failed, err %d",WSAGetLastError());
            }
            else
            {
                WTRACE("accepted incoming connection");
                CWinsockServProvider* newSock = CWinsockServProvider::
                    FromSocket(iProtocol, sock, iFamily);
                if (newSock)
                {
                    iSocket->ConnectComplete(*newSock);
                }
            }

            // Re-submit the request
            TInt err = Select();
            if (err != KErrNone)
            {
                iFlags |= EFlagError;
                iSocket->Error(KErrCouldNotConnect);
            }
        }
        else
        {
            NewDataCheck();
        }
    }
}

// Upcall from CNewDataNotifier. Also invoked from SelectComplete
void CWinsockServProvider::NewDataCheck()
{
    if (iFlags & EFlagShutdown)
    {
        return;
    }

    if (iFlags & EFlagEndOfData)
    {
        if (!(iFlags & EFlagDisconnected))
        {
            WTRACE("indicating end of data");
            iSocket->NewData(KNewDataEndofData);
        }
        return;
    }

    TUint32 size = 0;
    ASSERT(iWinSocket != INVALID_SOCKET);
    int err = ioctlsocket(iWinSocket, FIONREAD, &size);
    iProtocol->SetReadPending(this, !err && (size > 0));
    if (err == SOCKET_ERROR)
    {
        WTRACE1("ioctl(FIONREAD) err %d",WSAGetLastError());
        iFlags |= EFlagError;
        iSocket->Error(KErrGeneral);
    }
    else
    {
        TInt bytesInSocket = size;
        WTRACE2("%d bytes in socket, %d bytes unread",
            bytesInSocket, iDataAvailable);
        if (bytesInSocket > iDataAvailable)
        {
            // Report more data
            TInt newData = bytesInSocket - iDataAvailable;
            iDataAvailable = bytesInSocket;
            if (iProtocol->IsStreamProtocol())
            {
                WTRACE1("indicating %d bytes",newData);
                iSocket->NewData(newData);
            }
            else
            {
                WTRACE("indicating (at least) 1 datagram");
                iSocket->NewData(1);
            }
        }
        else if (iProtocol->IsStreamProtocol())
        {
            // For connection-oriented sockets, readability may indicate that
            // a request to close the socket has been received from the peer.
            // If the connection was closed gracefully, and all data was
            // received, then a recv will return immediately with zero bytes
            // read. If the connection was reset, then a recv will complete
            // immediately with an error code
            if (!bytesInSocket && !iDataAvailable)
            {
                BEGIN_WIN32();
                char buf;
                int ret = recv(iWinSocket, &buf, 1, 0);
                END_WIN32();

                if (ret == 0)
                {
                    iFlags |= EFlagEndOfData;
                    WTRACE("end of data, I guess");
                    iSocket->NewData(KNewDataEndofData);
                }
                else if (ret == SOCKET_ERROR)
                {
                    WTRACE1("recv err %d",WSAGetLastError());
                    iFlags |= EFlagError;
                    iSocket->Error(KErrDisconnected);
                }
                else
                {
                    // This means that we were wrong about the connection
                    // being either reset or close. Too bad...
                    WTRACE("oops... didn't expect any data");
                    ASSERT(FALSE);
                    iFlags |= EFlagError;
                    iSocket->Error(KErrGeneral);
                }
            }
        }
    }
}

// CWinsockServProvider::CSelectRequest
CWinsockServProvider::CSelectRequest*
CWinsockServProvider::CSelectRequest::NewLC(CWinsockServProvider* aProvider)
{
    CSelectRequest* self = new(ELeave)CSelectRequest(aProvider);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

CWinsockServProvider::CSelectRequest::CSelectRequest(CWinsockServProvider* aP) :
CActive(EPriorityStandard), iProvider(aP)
{
    CActiveScheduler::Add(this);
}

CWinsockServProvider::CSelectRequest::~CSelectRequest()
{
    iServerThread.Close();
}

void CWinsockServProvider::CSelectRequest::ConstructL()
{
    LEAVE_IF_ERROR(iServerThread.Open(RThread().Id()));
}

void CWinsockServProvider::CSelectRequest::SubmitL()
{
    if (!IsActive())
    {
        iStatus = KRequestPending;
        SetActive();
    }
    LEAVE_IF_ERROR(CWinsockSelectThread::StaticL().Submit(this));
}

TUint CWinsockServProvider::CSelectRequest::Socket()
{
    ASSERT(iProvider->iWinSocket != INVALID_SOCKET);
    return iProvider->iWinSocket;
}

TInt CWinsockServProvider::CSelectRequest::SelectMask()
{
    return (ESelectRead|ESelectError);
}

void CWinsockServProvider::CSelectRequest::SelectComplete(TInt aSelectMask)
{
    iSelectMask = aSelectMask;
    TRequestStatus *status = (&iStatus);
    iServerThread.RequestComplete(status, KErrNone);
}

void CWinsockServProvider::CSelectRequest::DoCancel()
{
    TRequestStatus *status = (&iStatus);
    User::RequestComplete(status, KErrCancel);
}

void CWinsockServProvider::CSelectRequest::RunL()
{
    iProvider->SelectComplete(iSelectMask);
}

// CWinsockServProvider::CWriteData
CWinsockServProvider::CWriteData*
CWinsockServProvider::CWriteData::New(const TDesC8& aData, TSockAddr* aAddr)
{
    CWriteData* data = new(CWriteData);
    if (data)
    {
        data->iData = aData.Alloc();
        if (data->iData)
        {
            if (aAddr)
            {
                data->iAddr = new(TSockAddr);
                if (data->iAddr) (*data->iAddr) = *aAddr;
            }
            if (!aAddr || data->iAddr)
            {
                return data;
            }
        }
        delete data;
    }
    return NULL;
}

CWinsockServProvider::CWriteData::~CWriteData()
{
    delete iAddr;
    delete iData;
}

// CWinsockServProvider::CWriteRequest
CWinsockServProvider::CWriteRequest::CWriteRequest(CWinsockServProvider* aP) :
CActive(EPriorityStandard), iProvider(aP)
{
    CActiveScheduler::Add(this);
}

CWinsockServProvider::CWriteRequest::~CWriteRequest()
{
    Cancel();
    if (iTimerCreated)
    {
        iTimer.Close();
    }
}

void CWinsockServProvider::CWriteRequest::Submit(TInt aDelay)
{
    if (aDelay)
    {
        if (!iTimerCreated)
        {
            // Create timer first time we need a delay
            TInt err = iTimer.CreateLocal();
            if (err == KErrNone) iTimerCreated = ETrue;
        }
        if (iTimerCreated)
        {
            Cancel();
            SetActive();
            iTimer.After(iStatus, aDelay);
            iTimerActive = ETrue;
            return;
        }
    }
    if (!IsActive())
    {
        // Complete request immediately
        SetActive();
        TRequestStatus *status = (&iStatus);
        User::RequestComplete(status, KErrCancel);
    }
}

void CWinsockServProvider::CWriteRequest::DoCancel()
{
    TRequestStatus *status = (&iStatus);
    if (iTimerActive)
    {
        iTimerActive = EFalse;
        iTimer.Cancel();
    }
    else
    {
        User::RequestComplete(status, KErrCancel);
    }
}

void CWinsockServProvider::CWriteRequest::RunL()
{
    iTimerActive = EFalse;
    iProvider->DoWrite();
}

// CWinsockServProvider::CNewDataNotifier
CWinsockServProvider::CNewDataNotifier::CNewDataNotifier(CWinsockServProvider* aP) :
CActive(EPriorityStandard), iProvider(aP)
{
    CActiveScheduler::Add(this);
}

void CWinsockServProvider::CNewDataNotifier::Submit()
{
    if (!IsActive())
    {
        SetActive();
        TRequestStatus *status = (&iStatus);
        User::RequestComplete(status, KErrCancel);
    }
}

void CWinsockServProvider::CNewDataNotifier::DoCancel()
{
    TRequestStatus *status = (&iStatus);
    User::RequestComplete(status, KErrCancel);
}

void CWinsockServProvider::CNewDataNotifier::RunL()
{
    iProvider->NewDataCheck();
}

/**
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

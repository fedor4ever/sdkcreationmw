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


#define TRACE_PREFIX "WSOCK: HostResolver: "
#include "wsock.h"
#include <in_sock.h>
#include <utf.h>
#include "WinsockProtocol.h"
#include "WinsockProtocolFamily.h"
#include "WinsockUtils.h"
#include "WinsockHostResolver.h"
#include "WinsockUtils.h"

//
// Structure used in getaddrinfo() call.
//
typedef struct _WinsockAddrInfo WinsockAddrInfo;
struct _WinsockAddrInfo {
    int ai_flags;              // AI_PASSIVE, AI_CANONNAME, AI_NUMERICHOST.
    int ai_family;             // PF_xxx.
    int ai_socktype;           // SOCK_xxx.
    int ai_protocol;           // 0 or IPPROTO_xxx for IPv4 and IPv6.
    size_t ai_addrlen;         // Length of ai_addr.
    char *ai_canonname;        // Canonical name for nodename.
    WSockAddr *ai_addr;        // Binary address.
    WinsockAddrInfo *ai_next;  // Next structure in linked list.
};

// getaddrinfo
typedef int (WINAPI *GetAddrInfoProc)(
    const char *aNodename,
    const char *aServname,
    const WinsockAddrInfo *aHints,
    WinsockAddrInfo **aResult
);

// freeaddrinfo
typedef void (WINAPI *FreeAddrInfoProc)(
    WinsockAddrInfo *aInfo
);

// getnameinfo
typedef int (WINAPI *GetNameInfoProc)(
    const WSockAddr  *sa,
    int salen,
    char *host,
    int hostlen,
    char *serv,
    int servlen,
    int flags
);

static const char WinsockModule[] = "ws2_32";
const TInt KMaxHostNameLen = 256;

CWinsockHostResolver* CWinsockHostResolver::NewL(const TWinProtocolDesc* aPd)
{
    CWinsockHostResolver* self = new(ELeave)CWinsockHostResolver(aPd);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

CWinsockHostResolver::CWinsockHostResolver(const TWinProtocolDesc* aProtocol):
iProtocolDesc(aProtocol)
{
}

CWinsockHostResolver::~CWinsockHostResolver()
{
    delete iHostName;
}

void CWinsockHostResolver::ConstructL()
{
    char hostname[KMaxHostNameLen];
    hostname[0] = 0;
    gethostname(hostname,N(hostname));
    TPtrC8 hostnamePtr((unsigned char*)hostname);
    iHostName = HBufC::NewL(hostnamePtr.Length()+1);
    TPtr hostNamePtr(iHostName->Des());
    hostNamePtr.Copy(hostnamePtr);
    hostNamePtr.ZeroTerminate();
    TRACE1("host name: %S",iHostName);
}

CWinsockHostResolver::TResolveErr 
CWinsockHostResolver::Resolve6(const char* aHostName,
                               TInt aIndex, TInetAddr* aAddr)
{
#define NO_GET_ADDR_INFO ((GetAddrInfoProc)(-1))
#define NO_FREE_ADDR_INFO ((FreeAddrInfoProc)(-1))

    static GetAddrInfoProc getAddrInfo = NULL;
    static FreeAddrInfoProc freeAddrInfo = NULL;

    // Initialize XP specific functions
    if (!getAddrInfo)
    {
        // We are linking with ws2_32.dll so if we are loaded, then it 
        // must be loaded too
        HMODULE hWs2 = GetModuleHandleA(WinsockModule);
        freeAddrInfo = (FreeAddrInfoProc)GetProcAddress(hWs2,"freeaddrinfo");
        if (freeAddrInfo)
        {
            TRACE1("found freeaddrinfo at 0x%08x",freeAddrInfo);
            getAddrInfo = (GetAddrInfoProc)GetProcAddress(hWs2,"getaddrinfo");
            if (getAddrInfo)
            {
                TRACE1("found getaddrinfo at 0x%08x",getAddrInfo);
            }
            else
            {
                TRACE("getaddrinfo is not supported");
                getAddrInfo = NO_GET_ADDR_INFO;
            }
        }
        else
        {
            TRACE("freeaddrinfo is not supported (getaddrinfo probably too)");
            freeAddrInfo = NO_FREE_ADDR_INFO;
            getAddrInfo = NO_GET_ADDR_INFO;
        }
    }

    if (getAddrInfo != NO_GET_ADDR_INFO)
    {
        WinsockAddrInfo hints;
        WinsockAddrInfo *addr = NULL;
        Mem::FillZ(&hints,sizeof(hints));
        hints.ai_family = AF_INET6;
        hints.ai_socktype = SOCK_STREAM;

        int err = getAddrInfo(aHostName, NULL, &hints, &addr);
        if (!err && addr)
        {
            int index = 0;
            TResolveErr status = EResolveNoMore;
            WinsockAddrInfo *ai = addr;
            while (index < aIndex && ai) ai = ai->ai_next, index++;
            if (ai) {
                WinsockUtils::ToInetAddr(aAddr, ai->ai_addr);
                status = EResolveOK;
            }
            freeAddrInfo(addr);
            return status;
        }
        else
        {
            TRACE1("getAddrInfo error %d",WSAGetLastError());
        }
    }
    return EResolveFailed;
}

TInt CWinsockHostResolver::Resolve(const char* aHostName, 
                                   TInt aIndex, 
                                   TInetAddr* aAddr)
{
    if (iProtocolDesc->iAddrFamily == KAfInet6)
    {
        TResolveErr err = Resolve6(aHostName, aIndex, aAddr);
        switch (err) {
        case EResolveOK:        return KErrNone;
        case EResolveNoMore:    return KErrNotFound;
        case EResolveFailed:    break;
        }
    }

    /* If addrFamily is KAfInet or CallGetAddrInfo failed */
    struct hostent * host = gethostbyname(aHostName);
    if (host && host->h_length == 4)
    {
        TInt n = 0;
        while (host->h_addr_list[n]) n++;
        if (aIndex < n)
        {
            struct in_addr in;
            Mem::Copy(&in, host->h_addr_list[aIndex], host->h_length);
            if (iProtocolDesc->iAddrFamily == KAfInet6)
            {
                aAddr->SetV4MappedAddress(ntohl(in.s_addr));
            }
            else
            {
                aAddr->SetAddress(ntohl(in.s_addr));
            }
            return KErrNone;
        }
    }

    return KErrNotFound;
}

TInt CWinsockHostResolver::GetHostNameFromAddr6(const TIp6Addr& aAddr,
                                                THostName& aName)
{
#define NO_GET_NAME_INFO ((GetNameInfoProc)(-1))

    static GetNameInfoProc getNameInfo = NULL;

    // Initialize XP specific functions
    if (!getNameInfo)
    {
        // We are linking with ws2_32.dll so if we are loaded, then it 
        // must be loaded too
        HMODULE hWs2 = GetModuleHandleA(WinsockModule);
        getNameInfo = (GetNameInfoProc)GetProcAddress(hWs2,"getnameinfo");
        if (getNameInfo)
        {
            TRACE1("found getNameInfo at 0x%08x",getNameInfo);
        }
        else
        {
            TRACE("getNameInfo is not supported");
            getNameInfo = NO_GET_NAME_INFO;
        }
    }

    if (getNameInfo != NO_GET_NAME_INFO)
    {
    	char name[KMaxHostNameLen] = "";
        WSockAddr sa;
        TInetAddr inetAddr(aAddr,0);
        int len = WinsockUtils::ToSockAddr(&sa, &inetAddr);
        int err = getNameInfo(&sa, len, name, sizeof(name), NULL, 0, 0);
        if (!err && name[0])
        {
            TPtrC8 hostnamePtr((unsigned char*)name);
            aName.Copy(hostnamePtr);
            return KErrNone;
        }
        else
        {
            TRACE1("getNameInfo error %d",WSAGetLastError());
        }
    }
    return KErrNotFound;
	
}

// Takes IPv4 address in network byte order
TInt CWinsockHostResolver::GetHostNameFromAddr(TUint32 aAddr,THostName& aName)
{
    struct hostent* host = gethostbyaddr((char*)&aAddr,sizeof(aAddr),AF_INET);
    if (host)
    {
        TPtrC8 hostnamePtr((unsigned char*)host->h_name);
        aName.Copy(hostnamePtr);
        return KErrNone;
    }
    return KErrNotFound;
}

// CHostResolvProvdBase
void CWinsockHostResolver::GetByName(TNameRecord& aName)
{
    TInt result = KErrNotFound;
    TInetAddr &addr = TInetAddr::Cast(aName.iAddr);
    TRACE2("GetByName(%S,%d)",&aName.iName,aName.iFlags);

    // Note that aName.iFlags is actually the index of the address 
    // to retreive. First one is zero, and so on. Sometimes we are being
    // called in a loop with aName.iFlags being incremented each time.
    // Loop terminates when we return something other than KErrNone.
    // That happens when client calls RHostResolver::Next function

    // First assume that it's an IP address string
    if (addr.Input(aName.iName) == KErrNone)
    {
        if (iProtocolDesc->iAddrFamily == KAfInet6 && 
            addr.Family() == KAfInet)
        {
            // Caller asked for IPv6 address
            addr.ConvertToV4Mapped();
        }
        result = ((aName.iFlags == 0) ? KErrNone : KErrNotFound);
    }
    else
    {
        // Do the name resolution
        char hostname[KMaxHostNameLen];
        TPtr8 hostnamePtr((unsigned char*)hostname, N(hostname));
        CnvUtfConverter::ConvertFromUnicodeToUtf8(hostnamePtr,aName.iName);
        hostnamePtr.ZeroTerminate();
        result = Resolve(hostname, aName.iFlags, &addr);
#ifdef _REALLY_DEBUG
        if (result == KErrNone)
        {
            TBuf<256> buf;
            addr.OutputWithScope(buf);
            TRACE1("%S",&buf);
        }
#endif // _REALLY_DEBUG
    }
    iNotify->QueryComplete(result);
}

void CWinsockHostResolver::GetByAddress(TNameRecord& aName)
{
    TInt result = KErrNotFound;
    TInetAddr &addr = TInetAddr::Cast(aName.iAddr);

#ifdef _REALLY_DEBUG
    TBuf<256> buf;
    addr.OutputWithScope(buf);
    TRACE2("GetByAddress(%S, %d)",&buf, aName.iFlags);
#endif // _REALLY_DEBUG

    // Again, aName.iFlags is the index of the name to retreive. It can be
    // greater than zero if the client calls RHostResolver::Next. We only
    // return one, with index zero.
    if (aName.iFlags == 0)
    {
        // Everything that converts into an IPv4 address without loss of
        // information goes through gethostbyname. Note that IPv4 address
        // returned by TInetAddr::Address() is in host byte order.
        TUint32 ipv4 = htonl(addr.Address());
        result = (ipv4 ?
            GetHostNameFromAddr(ipv4, aName.iName) :
            GetHostNameFromAddr6(addr.Ip6Address(), aName.iName));

#ifdef _REALLY_DEBUG
        if (result == KErrNone)
        {
            TRACE1("%S",&aName.iName);
        }
#endif // _REALLY_DEBUG

    }
    iNotify->QueryComplete(result);
}

void CWinsockHostResolver::SetHostName(TDes& aNameBuf)
{
    TRACE1("SetHostName(%S)",&aNameBuf);
    HBufC* newHostName = aNameBuf.Alloc();
    if (newHostName)
    {
        delete iHostName;
        iHostName = newHostName;
    }
}

void CWinsockHostResolver::GetHostName(TDes& aNameBuf)
{
    TRACE("GetHostName");
    if (iHostName) aNameBuf.Copy(*iHostName);
    iNotify->QueryComplete(KErrNone);
}

// CResolverProvdBase
void CWinsockHostResolver::CancelCurrentOperation()
{
    // Nothing to cancel, everything is synchronous
}

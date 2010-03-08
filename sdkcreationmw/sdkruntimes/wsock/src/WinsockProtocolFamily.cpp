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


#define TRACE_PREFIX "WSOCK: ProtocolFamily: "
#include "wsock.h"
#include "WinsockProtocol.h"
#include "WinsockProtocolFamily.h"
#include <winsock2.h>

#define IPPROTO_ICMPV6 58

const TUint KProtocolInetRes = 0xF01;

// Initializer for TWinProtocolDesc structure
class TWinProtocolDescInit: public TWinProtocolDesc
{
public:
    TWinProtocolDescInit(const TText * aName,
        TUint aAddrFamily, TUint aSockType, TUint aProtocol,
        TUint aServiceInfo, TInt aMessageSize, TInt aServiceTypeInfo,
        TUint aWinSockType, TUint aWinProtocol)
    {
        iName = aName;
        iAddrFamily = aAddrFamily;
        iSockType = aSockType;
        iProtocol = aProtocol;
        iVersion = TVersion(1,0,0);
        iByteOrder = EBigEndian;
        iServiceInfo = aServiceInfo;
        iNamingServices = KNSNameResolution;
        iSecurity = KSocketNoSecurity;
        iMessageSize = aMessageSize;
        iServiceTypeInfo = aServiceTypeInfo;
        iNumSockets = KUnlimitedSockets;
        iWinSockType = aWinSockType;
        iWinProtocol = aWinProtocol;
    }
};

TWinProtocolDescInit TCP(_S("tcp"),
    KAfInet,  KSockStream,  KProtocolInetTcp, KSIStreamBased | KSIInOrder |
    KSIReliable | KSIGracefulClose | KSIPeekData | KSIUrgentData, 
    KSocketMessageSizeIsStream, ESocketSupport,
    SOCK_STREAM, IPPROTO_TCP);

TWinProtocolDescInit UDP(_S("udp"),
    KAfInet,  KSockDatagram,  KProtocolInetUdp, 
    KSIConnectionLess | KSIDatagram | KSIGracefulClose | KSIPeekData,
    65516, ESocketSupport,
    SOCK_DGRAM, IPPROTO_UDP);

TWinProtocolDescInit ICMP4(_S("icmp"),
    KAfInet,  KSockDatagram,  KProtocolInetIcmp, KSIConnectionLess |
    KSIMessageBased | KSIBroadcast | KSIPeekData | KSIGracefulClose,
    65000, ESocketSupport | ETransport | EPreferDescriptors | EUseCanSend,
    SOCK_RAW, IPPROTO_ICMP);

TWinProtocolDescInit ICMP6(_S("icmp6"),
    KAfInet,  KSockDatagram,  KProtocolInet6Icmp, KSIConnectionLess |
    KSIMessageBased | KSIBroadcast | KSIPeekData | KSIGracefulClose,
    65000, ESocketSupport | ETransport | EPreferDescriptors | EUseCanSend,
    SOCK_RAW, IPPROTO_ICMPV6);

#ifdef WSOCK_IPV6
TWinProtocolDescInit TCP6(_S("tcp"),
    KAfInet6,  KSockStream,  KProtocolInetTcp, KSIStreamBased | KSIInOrder |
    KSIReliable | KSIGracefulClose | KSIPeekData | KSIUrgentData, 
    KSocketMessageSizeIsStream, ESocketSupport,
    SOCK_STREAM, IPPROTO_TCP);

TWinProtocolDescInit UDP6(_S("udp"),
    KAfInet6,  KSockDatagram,  KProtocolInetUdp, 
    KSIConnectionLess | KSIDatagram |KSIGracefulClose | KSIPeekData,
    65516, ESocketSupport,
    SOCK_DGRAM, IPPROTO_UDP);

TWinProtocolDescInit ICMP46(_S("icmp"),
    KAfInet6,  KSockDatagram,  KProtocolInetIcmp, KSIConnectionLess |
    KSIMessageBased | KSIBroadcast | KSIPeekData | KSIGracefulClose,
    65000, ESocketSupport | ETransport | EPreferDescriptors | EUseCanSend,
    SOCK_RAW, IPPROTO_ICMP);

TWinProtocolDescInit ICMP66(_S("icmp6"),
    KAfInet6,  KSockDatagram,  KProtocolInet6Icmp, KSIConnectionLess |
    KSIMessageBased | KSIBroadcast | KSIPeekData | KSIGracefulClose,
    65000, ESocketSupport | ETransport | EPreferDescriptors | EUseCanSend,
    SOCK_RAW, IPPROTO_ICMPV6);

#endif // WSOCK_IPV6

// NOTE: the order of protocols must match the order of entries in the
// ip.tcp.esk file (at the time of thie writing this file was located 
// in c:\private\101f7989\ESock directory).
static const TWinProtocolDesc* ipProtocols[] = {
    &TCP,&UDP,&ICMP4,&ICMP6,
#ifdef WSOCK_IPV6
    &TCP6,&UDP6,&ICMP46,&ICMP66
#endif // WSOCK_IPV6
};

CWinsockProtocolFamily::CWinsockProtocolFamily(TUint aAddrFamily) :
iAddrFamily(aAddrFamily)
{
    TRACE2("[%08X] created (0x%04x)",this,aAddrFamily);
}

CWinsockProtocolFamily::~CWinsockProtocolFamily()
{
    TRACE1("[%08X] destroyed",this);
}

TInt CWinsockProtocolFamily::Install()
{
    TRACE1("[%08X] Install",this);
    return KErrNone;
}

// Access to the list of protocols
TInt CWinsockProtocolFamily::ProtocolCount()
{
    return N(ipProtocols);
}

const TServerProtocolDesc* CWinsockProtocolFamily::ProtocolDesc(TInt aIndex)
{
    ASSERT(aIndex >= 0 && aIndex < N(ipProtocols));
    if (aIndex >= 0 && aIndex < N(ipProtocols))
    {
        return ipProtocols[aIndex];
    }
    else
    {
        return NULL;
    }
}

// Returns comma separated list of protocol names
void CWinsockProtocolFamily::ProtocolNames(TDes& aProtocols)
{
    aProtocols.SetLength(0);
    for (TInt i=0; i<N(ipProtocols); i++)
    {
        TBool alreadyThere = EFalse;
        for (TInt k=0; k<i; k++)
        {
            if (!ipProtocols[i]->iName.Compare(ipProtocols[k]->iName))
            {
                alreadyThere = ETrue;
                break;
            }
        }
        if (!alreadyThere)
        {
            if (i > 0) aProtocols.Append(',');
            aProtocols.Append(ipProtocols[i]->iName);
        }
    }
    TRACE1("ProtocolNames: %S",&aProtocols);
}

// CProtocolFamilyBase
TUint CWinsockProtocolFamily::ProtocolList(TServerProtocolDesc*& aProtocolList)
{
    // we allocate slightly more, but that's OK
    TInt count = 0;
    aProtocolList = new(ELeave)TServerProtocolDesc[N(ipProtocols)];
    if (aProtocolList)
    {
        for (TInt i=0; i<N(ipProtocols); i++)
        {
            if (ipProtocols[i]->iAddrFamily == iAddrFamily)
            {
                const TServerProtocolDesc* src = ipProtocols[i];
                aProtocolList[count++] = *src;
            }
        }

        if (!count)
        {
            delete aProtocolList;
            aProtocolList =  NULL;
        }
    }
    
    TRACE2("[%08X] ProtocolList - %d protocols",this,count);
    return count;
}

CProtocolBase* CWinsockProtocolFamily::NewProtocolL(TUint aSockType,
                                                    TUint aProtocol)
{
    TRACE3("[%08X] NewProtocolL(%d,%d)",this,aSockType,aProtocol);
    if (aProtocol == KUndefinedProtocol)
    {
        switch (aSockType)
        {
        case KSockDatagram:
            aProtocol = KProtocolInetUdp;
            break;
        case KSockStream:
            aProtocol = KProtocolInetTcp;
            break;
        default:
            LEAVE(KErrNotSupported);
            break;
        }
    }

    // Finds requested protocol
    for (TInt i=0; i<N(ipProtocols); i++)
    {
        if (ipProtocols[i]->iAddrFamily == iAddrFamily &&
            ipProtocols[i]->iSockType == aSockType &&
            ipProtocols[i]->iProtocol == aProtocol)
        {
            return new(ELeave)CWinsockProtocol(ipProtocols[i]);
        }
    }

    LEAVE(KErrNotSupported);
    return NULL;   
}

/**
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

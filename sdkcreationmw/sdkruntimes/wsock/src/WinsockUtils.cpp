/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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


#include "WinsockUtils.h"

// Converts TInetAddr into WSockAddr. Returns the size of the sockaddr
// structure, zero if conversion failed.
int WinsockUtils::ToSockAddr(WSockAddr* aToAddr, const TSockAddr* aFromAddr)
{
    // Windows requires that unused part of sockaddr structure is filled
    // with zeros.
    Mem::FillZ(aToAddr, SOCKADDR_SIZE);
    TInetAddr* from = &TInetAddr::Cast(aFromAddr);
    if (from->Family() == KAfInet)
    {
        aToAddr->AddressIn.sin_family = AF_INET;
        aToAddr->AddressIn.sin_port = htons((u_short)from->Port());
        aToAddr->AddressIn.sin_addr.s_addr = htonl(from->Address());
        return sizeof(struct sockaddr_in);
    }
    else if (from->Family() == KAfInet6)
    {
        aToAddr->AddressIn6.sin6_family = AF_INET6;
        aToAddr->AddressIn6.sin6_port = htons((u_short)from->Port());
        aToAddr->AddressIn6.sin6_flowinfo = from->FlowLabel();
        aToAddr->AddressIn6.sin6_scope_id = from->Scope();
        Mem::Copy(&aToAddr->AddressIn6.sin6_addr, &from->Ip6Address(),16);
        return sizeof(struct sockaddr_in6);
    }
    else if (from->Family() == KAFUnspec)
    {
        aToAddr->AddressIn.sin_family = AF_INET;
        aToAddr->AddressIn.sin_port = htons((u_short)from->Port());
        aToAddr->AddressIn.sin_addr.s_addr = 0;
        return sizeof(struct sockaddr_in);
    }
    else
    {
        return 0;
    }
}

// Converts WSockAddr into TInetAddr
TBool WinsockUtils::ToInetAddr(TSockAddr* aToAddr, const WSockAddr* aFromAddr)
{
    TIp6Addr ip6;
    TInetAddr* to = &TInetAddr::Cast(aToAddr);
    switch (aFromAddr->Address.sa_family) {
    case AF_INET:
        to->SetFamily(KAfInet);
        to->SetPort(ntohs(aFromAddr->AddressIn.sin_port));
        to->SetAddress(ntohl(aFromAddr->AddressIn.sin_addr.s_addr));
        return ETrue;
    case AF_INET6:
        to->SetFamily(KAfInet6);
        to->SetPort(ntohs(aFromAddr->AddressIn6.sin6_port));
        to->SetFlowLabel(aFromAddr->AddressIn6.sin6_flowinfo);
        to->SetScope(aFromAddr->AddressIn6.sin6_scope_id);
        Mem::Copy(&ip6,&aFromAddr->AddressIn6.sin6_addr,16);
        to->SetAddress(ip6);
        return ETrue;
    default:
        to->SetFamily(KAFUnspec);
        to->SetPort(0);
        return EFalse;
    }
}


/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

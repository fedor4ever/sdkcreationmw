/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* which accompanies this distribution, and is available
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
*
*/


#ifndef _WSOCK_UTILS_H_
#define _WSOCK_UTILS_H_

#include <in_sock.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// SetPort is defined somewhere in Windows headers as SetPortW.
// Undo the damage.
#undef SetPort

/* 
 * The right sockaddr_in6 structure. Some Microsoft's SDKs contain wrong
 * definition of sockaddr_in6 in ws2tcpip.h (without sin6_scope_id field)
 */
struct SockaddrIn6 {
    short   sin6_family;        /* AF_INET6 */
    u_short sin6_port;          /* Transport level port number */
    u_long  sin6_flowinfo;      /* IPv6 flow information */
    struct in_addr6 sin6_addr;  /* IPv6 address */
    u_long sin6_scope_id;       /* set of interfaces for a scope */
};

typedef union _WSockAddr {
    struct sockaddr Address;
    struct sockaddr_in AddressIn;
    struct SockaddrIn6 AddressIn6;
} WSockAddr;

#define SOCKADDR_SIZE sizeof(WSockAddr)

#ifdef EKA2
#  define BEGIN_WIN32() Emulator::Escape()
#  define END_WIN32() Emulator::Reenter()
#  include <emulator.h>
#else
#  define BEGIN_WIN32() ((void)0)
#  define END_WIN32() ((void)0)
#endif // EKA2

namespace WinsockUtils
{
    int ToSockAddr(WSockAddr* aToAddr, const TSockAddr* aFromAddr);
    TBool ToInetAddr(TSockAddr* aToAddr, const WSockAddr* aFromAddr);
}

#endif // _WSOCK_UTILS_H_

/*
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

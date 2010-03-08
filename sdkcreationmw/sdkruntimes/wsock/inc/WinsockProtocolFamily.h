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


#ifndef _WSOCK_PROTOCOL_FAMILY_H_
#define _WSOCK_PROTOCOL_FAMILY_H_

#include <in_sock.h>
#include <es_prot.h>

struct TServerProtocolDesc;
struct TWinProtocolDesc: public TServerProtocolDesc
{
	TUint iWinSockType;     // SOCK_STREAM, SOCK_DGRAM, SOCK_RAW
	TUint iWinProtocol;     // IPPROTO_TCP, IPPROTO_UDP, IPPROTO_ICMP
};

class CWinsockProtocolFamily : public CProtocolFamilyBase
{
private:
    TUint iAddrFamily;

public:
    CWinsockProtocolFamily(TUint aAddrFamily = KAfInet);
    virtual ~CWinsockProtocolFamily();

    // Access to the list of protocols
    static TInt ProtocolCount();
    static void ProtocolNames(TDes& aNetworks);
    static const TServerProtocolDesc* ProtocolDesc(TInt aIndex);

    // CProtocolFamilyBase
    virtual TInt Install();
    virtual TUint ProtocolList(TServerProtocolDesc *& aProtocolList);
    virtual CProtocolBase* NewProtocolL(TUint aSockType, TUint aProtocol);
};

#endif // _WSOCK_PROTOCOL_FAMILY_H_

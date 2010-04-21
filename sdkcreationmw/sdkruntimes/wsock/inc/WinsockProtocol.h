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


#ifndef _WSOCK_PROTOCOL_H_
#define _WSOCK_PROTOCOL_H_

#include <nifif.h>
#include <es_prot.h>
#include <time.h>
#include "WinsockInterface.h"
#include <es_prot_internal.h>
#include <nifif_internal.h>


// IpHlpApi.dll data structures, needed by GetAdaptersInfo
typedef struct _WinsockIpAddressString {
    TText8 String[16];
} WinsockIpAddressString;
typedef struct _WinsockIpAddrString {
    struct _WinsockIpAddrString* Next;
    WinsockIpAddressString IpAddress;
    WinsockIpAddressString IpMask;
    TUint32 Context;
} WinsockIpAddrString;
typedef struct _WinsockIpAdapterInfo {
    struct _WinsockIpAdapterInfo* Next;
    TUint32 ComboIndex;
    TText8 AdapterName[260];
    TText8 Description[132];
    TUint AddressLength;
    TUint8 Address[8];
    TUint32 Index;
    TUint Type;
    TUint DhcpEnabled;
    WinsockIpAddrString* CurrentIpAddress;
    WinsockIpAddrString IpAddressList;
    WinsockIpAddrString GatewayList;
    WinsockIpAddrString DhcpServer;
    TBool HaveWins;
    WinsockIpAddrString PrimaryWinsServer;
    WinsockIpAddrString SecondaryWinsServer;
    time_t LeaseObtained;
    time_t LeaseExpires;
} WinsockIpAdapterInfo;

// CWinsockProtocol
struct TWinProtocolDesc;
class CWinsockServProvider;
class CWinsockProtocol : public CProtocolBase, public MNifIfUser
{
    typedef TUint (__stdcall* GetAdaptersInfoProc)
        (WinsockIpAdapterInfo* aAdapterInfo, TUint* aOutBufLen);

private:
    MNifIfNotify* iIfNotify;
    CWinsockInterface* iWinsockInterface;
    const TWinProtocolDesc* iProtocolDesc;
    RPointerArray<CWinsockServProvider> iReadPending;
    void* iIpHlpApi;        // Handle to IPHLPAPI.DLL
    GetAdaptersInfoProc iGetAdapterInfo; // Pointer to GetAdaptersInfo

public:
    CWinsockProtocol(const TWinProtocolDesc* aProtocolDesc);
    virtual ~CWinsockProtocol();

    const TWinProtocolDesc* ProtocolDesc() const;
    TBool IsStreamProtocol() const;
    TBool IsDatagramProtocol() const;

    void DataSent(TUint aBytes);
    void DataReceived(TUint aBytes);
    WinsockIpAdapterInfo* GetAdapterInfo();
    void SetReadPending(CWinsockServProvider* aSocket, TBool aPending);
    TBool ReadPending() const;

    // CProtocolBase
    virtual CServProviderBase* NewSAPL(TUint aSockType);
    virtual CHostResolvProvdBase* NewHostResolverL();
    virtual void Identify(TServerProtocolDesc* aProtocolDesc) const;
    virtual TInt GetOption(TUint aLevel,TUint aName,TDes8& aOption,CProtocolBase* aSourceProtocol);

    // MNifIfUser
    virtual void IfUserBindFailure(TInt aResult, TAny* aId);
    virtual void IfUserNewInterfaceL(CNifIfBase* aIf, TAny* aId);
    virtual void IfUserInterfaceDown(TInt aResult, CNifIfBase* aIf);
    virtual void IfUserOpenNetworkLayer();
    virtual void IfUserCloseNetworkLayer();
    virtual CProtocolBase* IfUserProtocol();
    virtual TBool IfUserIsNetworkLayerActive();
};

// Inline methods
inline const TWinProtocolDesc* CWinsockProtocol::ProtocolDesc() const
    { return iProtocolDesc; }

#endif // _WSOCK_PROTOCOL_H_

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

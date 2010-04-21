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


#ifndef _WSOCK_HOST_RESOLVER_H_
#define _WSOCK_HOST_RESOLVER_H_

#include <es_prot.h>

class CWinsockHostResolver : public CHostResolvProvdBase
{
    enum TResolveErr {
        EResolveOK,
        EResolveNoMore,
        EResolveFailed
    };

private:
    HBufC* iHostName;
    const TWinProtocolDesc* iProtocolDesc;

public:
    static CWinsockHostResolver* NewL(const TWinProtocolDesc* aProtocolDesc);
    virtual ~CWinsockHostResolver();

    // CHostResolvProvdBase
    virtual void GetByName(TNameRecord& aName);
    virtual void GetByAddress(TNameRecord& aName);
    virtual void SetHostName(TDes& aNameBuf);
    virtual void GetHostName(TDes& aNameBuf);

    // CResolverProvdBase
    virtual void CancelCurrentOperation();

private:
    CWinsockHostResolver(const TWinProtocolDesc* aProtocolDesc);
    void ConstructL();
    TResolveErr Resolve6(const char* aName, TInt aIndex, TInetAddr* aAddr);
    TInt Resolve(const char* aName, TInt aIndex, TInetAddr* aAddr);
    TInt GetHostNameFromAddr6(const TIp6Addr& aAddr, THostName& aName);
    TInt GetHostNameFromAddr(TUint32 aAddr, THostName& aName);
};

#endif // _WSOCK_HOST_RESOLVER_H_

/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Wsock configurable settings
*
*/

#ifndef _ECMTWSOCKSETTINGS_H_
#define _ECMTWSOCKSETTINGS_H_

#include <e32base.h>

/**
 * Configurable WSOCK settings.
 */
class CEcmtWsockSettings : public CBase
{
public:
    enum _Flags {
        EUseWinsock = 1,
        EUseProxy = 2
    };

private:
    TBool iFlags;
    HBufC* iProxyHost;
    TUint iProxyPort;

public:
    static CEcmtWsockSettings* New(TInt aFlg, const TDesC& aHost, TUint aPort);
    static CEcmtWsockSettings* New(TInt aFlg, HBufC* aHost, TUint aPort);
    static CEcmtWsockSettings* NewL(TInt aFlg, const TDesC& aHost,TUint aPort);
    static CEcmtWsockSettings* NewL(TInt aFlgs, HBufC* aHost, TUint aPort);
    virtual ~CEcmtWsockSettings();

    TBool UseWinsock() const { return ((iFlags & EUseWinsock) != 0); }
    TBool UseProxy() const { return ((iFlags & EUseProxy) != 0); }
    TUint ProxyPort() const { return iProxyPort; }
    const TDesC* ProxyHost() const { return iProxyHost; }
    
private:
    CEcmtWsockSettings(TBool aUseProxy, HBufC* aProxyHost, TUint aProxyPort);
    void ConstructL(const TDesC& aProxyHost);
};

#endif // _ECMTWSOCKSETTINGS_H_

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

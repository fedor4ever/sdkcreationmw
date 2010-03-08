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
* Description:  Interface to the Wsock plugin
*
*/

#ifndef _ECMTWSOCKPLUGIN_H_
#define _ECMTWSOCKPLUGIN_H_

#include "EcmtPlugin.h"
#include "EcmtWsockSettings.h"

/**
 * A plugin to configure WSOCK.
 */
class CEcmtWsockPlugin : public CBase,
                         public MEcmtPlugin,
                         public MEcmtMessageEventObserver
{
private:
    enum TParseState {
        EParseStart,
        EParseKey,
        EParseValue,
        EParseStates
    };

    MEcmtMessaging* iMessaging;
    MEcmtMessageSender* iMessageSender;
    RFs iRfs;

public:
    static MEcmtPlugin* NewL();

    virtual ~CEcmtWsockPlugin();

    // MEcmtPlugin
    virtual void BindMessagingL(MEcmtMessaging* aMessaging);

    // MEcmtMessageEventObserver
    virtual void HandleNotifyL (const CEcmtMessageEvent& aEvent);

private:
    CEcmtWsockPlugin();
    void ConstructL();

    HBufC8* ReadEskFile();
    TInt WriteEskFile(const TDesC8& aData);
    void SendCurrentSettings(TBool aProxyOnly);
    void SendSettings(const CEcmtWsockSettings* aSettings, TBool aProxyOnly);
    void HandleConfigMessage(const TPtrC8& aData);
    CEcmtWsockSettings* ParseConfigMessage(const TPtrC8& aData);
    CEcmtWsockSettings* QuerySettings(TBool aAlwaysReadProxy);
    void ApplySettings(const CEcmtWsockSettings* aSettings);

    static HBufC8* Append(HBufC8* aBuf, const TDesC8& aData);
    static HBufC16* Append(HBufC16* aBuf, const TDesC16& aData);
    static HBufC8* AppendString(HBufC8* aBuf, const TDesC& aString);
    static HBufC8* AppendBool(HBufC8* aBuf, const TDesC& aName, TBool aValue);
    static TBool ParseBool(const TDesC& aString, TBool& aBool);
    static TBool ParseInt(const TDesC& aString, TUint& aNum);
};

#endif // _ECMTWSOCKPLUGIN_H_

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

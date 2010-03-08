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


#include "EcmtWsockSettings.h"

// Symbian-style static condstructor
CEcmtWsockSettings* CEcmtWsockSettings::NewL(TInt aFlag,
                                             const TDesC& aHost,
                                             TUint aPort)
{
    CEcmtWsockSettings* me = new(ELeave)CEcmtWsockSettings(aFlag,NULL,aPort);
    CleanupStack::PushL(me);
    me->ConstructL(aHost);
    CleanupStack::Pop(me);
    return me;
}

// Non-leaving variant, returns NULL on failure
CEcmtWsockSettings* CEcmtWsockSettings::New(TInt aFlags,
                                            const TDesC& aProxyHost,
                                            TUint aProxyPort)
{
    CEcmtWsockSettings* me = NULL;
    TRAPD(err, me = CEcmtWsockSettings::NewL(aFlags,aProxyHost,aProxyPort));
    return me;
}

// This one takes ownership of the aHost pointer
CEcmtWsockSettings* CEcmtWsockSettings::NewL(TInt aFlag, 
                                             HBufC* aHost, 
                                             TUint aPort)
{
    CEcmtWsockSettings* me = new(ELeave)CEcmtWsockSettings(aFlag,aHost,aPort);
    if (!aHost)
    {
        _LIT(KEmpty, "");
        CleanupStack::PushL(me);
        me->ConstructL(KEmpty());
        CleanupStack::Pop(me);
    }
    return me;
}

// Non-leaving variant, returns NULL on failure
CEcmtWsockSettings* CEcmtWsockSettings::New(TInt aFlags,
                                            HBufC* aProxyHost,
                                            TUint aProxyPort)
{
    CEcmtWsockSettings* me = NULL;
    TRAPD(err, me = CEcmtWsockSettings::NewL(aFlags,aProxyHost,aProxyPort));
    return me;
}

// Initializes CEcmtWsockSettings
CEcmtWsockSettings::CEcmtWsockSettings(TBool aFlag, HBufC* aHost, TUint aPort):
  iFlags(aFlag), iProxyHost(aHost), iProxyPort(aPort)
{
}

// Second stage constructor
void CEcmtWsockSettings::ConstructL(const TDesC& aProxyHost)
{
    iProxyHost = aProxyHost.AllocL();
}

// Destructor
CEcmtWsockSettings::~CEcmtWsockSettings()
{
    delete iProxyHost;
}

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


#ifndef _WSOCK_AGENT_H_
#define _WSOCK_AGENT_H_

#include <nifagt.h>

class CWinsockAgentCallback;
class CWinsockAgent : public CNifAgentBase
{
public:
    typedef void (CWinsockAgent::*CallbackMethod)(void);
public:
    static CWinsockAgent* NewL();
    virtual ~CWinsockAgent();
    virtual void Info(TNifAgentInfo& aInfo) const;
    virtual void Connect(TAgentConnectType aType);
    virtual void Connect(TAgentConnectType aType, CStoreableOverrideSettings* aOverrideSettings);
    virtual void CancelConnect();
    virtual void Reconnect();
    virtual void CancelReconnect();
    virtual void Authenticate(TDes& aUsername, TDes& aPassword);
    virtual void CancelAuthenticate();
    virtual void Disconnect(TInt aReason);
    virtual TInt GetExcessData(TDes8& aBuffer);

#ifndef SYMBIAN_NETWORKING_PLATSEC
    virtual TInt ReadInt(const TDesC& aField, TUint32& aValue);
    virtual TInt WriteInt(const TDesC& aField, TUint32 aValue);
    virtual TInt ReadDes(const TDesC& aField, TDes8& aValue);
    virtual TInt WriteDes(const TDesC& aField, const TDesC8& aValue);
    virtual TInt ReadDes(const TDesC& aField, TDes16& aValue);
    virtual TInt WriteDes(const TDesC& aField, const TDesC16& aValue);
    virtual TInt ReadBool(const TDesC& aField, TBool& aValue);
    virtual TInt WriteBool(const TDesC& aField, TBool aValue);
    virtual HBufC* ReadLongDesLC(const TDesC& aField);
#endif /* SYMBIAN_NETWORKING_PLATSEC */

    virtual TInt Notification(TNifToAgentEventType aEvent, TAny* aInfo);
    virtual TInt IncomingConnectionReceived();
    virtual void GetLastError(TInt& aError);
    virtual TBool IsActive() const;
    virtual TBool IsReconnect() const;
    virtual void SetConnectionSettingsL(const TConnectionSettings& aSettings);
    virtual TConnectionSettings& ConnectionSettingsL();
    virtual void SetOverridesL(CStoreableOverrideSettings* aOverrideSettings);
    virtual CStoreableOverrideSettings* OverridesL();
    virtual void RequestNotificationOfServiceChangeL(MAgentSessionNotify* aSession);
    virtual void CancelRequestNotificationOfServiceChange(MAgentSessionNotify* aSession);

protected:
#ifdef SYMBIAN_NETWORKING_PLATSEC
    virtual TInt DoReadInt(const TDesC& aField, TUint32& aValue,const RMessagePtr2* aMessage);
    virtual TInt DoWriteInt(const TDesC& aField, TUint32 aValue,const RMessagePtr2* aMessage);
    virtual TInt DoReadDes(const TDesC& aField, TDes8& aValue,const RMessagePtr2* aMessage);
    virtual TInt DoWriteDes(const TDesC& aField, const TDesC8& aValue,const RMessagePtr2* aMessage);
    virtual TInt DoReadDes(const TDesC& aField, TDes16& aValue,const RMessagePtr2* aMessage);
    virtual TInt DoWriteDes(const TDesC& aField, const TDesC16& aValue,const RMessagePtr2* aMessage);
    virtual TInt DoReadBool(const TDesC& aField, TBool& aValue,const RMessagePtr2* aMessage);
    virtual TInt DoWriteBool(const TDesC& aField, TBool aValue,const RMessagePtr2* aMessage);
    virtual HBufC* DoReadLongDesLC(const TDesC& aField,const RMessagePtr2* aMessage);
#endif /* SYMBIAN_NETWORKING_PLATSEC */

private:
    CWinsockAgent();
    void ConstructL();
    void ScheduleConnectNotification();
    void CancelConnectNotification();
    void ConnectComplete();
    void DisconnectComplete();

private:
    TConnectionSettings iSettings;
    CWinsockAgentCallback* iConnector;
    CWinsockAgentCallback* iDisconnector;
    TUint32 iIapId;
    TUint32 iIapNetwork;
    TUint32 iIapService;
    TUint32 iIapBearer;
    TInt iDisconnectReason;
};

#endif // _WSOCK_AGENT_H_

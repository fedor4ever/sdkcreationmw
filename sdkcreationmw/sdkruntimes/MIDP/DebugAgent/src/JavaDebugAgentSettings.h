/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __JAVA_DEBUG_AGENT_SETTINGS_H_
#define __JAVA_DEBUG_AGENT_SETTINGS_H_

#include <e32base.h>
#include <s32strm.h>
#include <AknSettingItemList.h>
#include "JavaDebugAgent.hrh"

#define KNoAccessPoint KMaxTUint

// Setting item created by CJavaDebugAgentSettings::TBoolParam
class CJavaDebugAgentSettingsBooleanItem : 
    public CAknEnumeratedTextPopupSettingItem
{
public:
    inline CJavaDebugAgentSettingsBooleanItem(TInt aResId, TInt& aValue) :
        CAknEnumeratedTextPopupSettingItem(aResId, aValue) {}
    inline void ChangeValueL() { SetInternalValue(!InternalValue()); }
};

class CJavaDebugAgentDriver;
class CJavaDebugAgentSettings : public CBase
{
public:
    class TParam {
    private:
        TInt iId;
    public:
        TParam(TInt aId) : iId(aId) {}
        virtual ~TParam();
        TInt Id() const { return iId; }
        virtual TInt Size() const = 0;
        virtual void ReadL(RReadStream* aStream, TInt aSize) = 0;
        virtual void WriteL(RWriteStream* aStream) const = 0;
        virtual CAknSettingItem* CreateSettingItemL() = 0;
    };

    class TPortParam : public TParam {
    private:
        enum { KSize = 2 };
        TInt iPort;
    public:
        TPortParam(TInt aId, TInt aPort = 0): TParam(aId), iPort(aPort) {}
        TInt16 Value() const { return iPort; }
        void SetValue(TInt16 aPort) { iPort = aPort; }
        void SetValue(const TPortParam& aPort) { iPort = aPort.iPort; }
        TBool Equals(const TPortParam& aPort) const
            { return Id() == aPort.Id() && iPort == aPort.iPort; }

        virtual TInt Size() const;
        virtual void ReadL(RReadStream* aStream, TInt aSize);
        virtual void WriteL(RWriteStream* aStream) const;
        virtual CAknSettingItem* CreateSettingItemL();
    private:
        TPortParam& operator=(const TPortParam&) {/*please dont*/return *this;}
    };

    class TIntParam : public TParam {
    protected:
        enum { KSize = 4 };
        TInt iInt;
    public:
        TIntParam(TInt aId, TInt aInt = 0): TParam(aId), iInt(aInt) {}
        TInt Value() const { return iInt; }
        void SetValue(TInt aInt) { iInt = aInt; }
        void SetValue(const TIntParam& aInt) { iInt = aInt.iInt; }
        TBool Equals(const TIntParam& aInt) const
            { return Id() == aInt.Id() && iInt == aInt.iInt; }

        virtual TInt Size() const;
        virtual void ReadL(RReadStream* aStream, TInt aSize);
        virtual void WriteL(RWriteStream* aStream) const;
        virtual CAknSettingItem* CreateSettingItemL();
    private:
        TIntParam& operator=(const TIntParam&) {/*please dont*/return *this;}
    };

    class TApParam : public TIntParam {
    public:
        TApParam(TInt aId, TInt aAp = KNoAccessPoint): TIntParam(aId, aAp) {}
        virtual CAknSettingItem* CreateSettingItemL();
    private:
        TApParam& operator=(const TApParam&) {/*please dont*/return *this;}
    };

    class TEnumParam : public TIntParam {
    public:
        TEnumParam(TInt aId, TInt aValue = 0): TIntParam(aId, aValue) {}
        virtual CAknSettingItem* CreateSettingItemL();
    private:
        TEnumParam& operator=(const TEnumParam&) {/*please dont*/return *this;}
    };

    class TBoolParam : public TParam {
    private:
        enum { KSize = 1 };
        TBool iBool;
    public:
        TBoolParam(TInt aId, TBool aBool=EFalse): TParam(aId),iBool(aBool) {} 
        TBool Value() const { return iBool; }
        void SetValue(TBool aBool) { iBool = aBool; }
        void SetValue(const TBoolParam& aBool) { iBool = aBool.iBool; }
        TBool Equals(const TBoolParam& aBool) const
            { return Id() == aBool.Id() && iBool == aBool.iBool; }

        virtual TInt Size() const;
        virtual void ReadL(RReadStream* aStream, TInt aSize);
        virtual void WriteL(RWriteStream* aStream) const;
        virtual CAknSettingItem* CreateSettingItemL();
    private:
        TBoolParam& operator=(const TBoolParam&) {/*please dont*/return *this;}
    };

    class TStrParam : public TParam {
    private:
        TBuf<KDebugAgentMaxStrParamLength> iBuf;
    public:
        TStrParam(TInt aId): TParam(aId) {} 
        const TDesC& Value() const { return iBuf; }
        void SetValueL(const TDesC* aValue);
        void SetValueL(const TStrParam& aStr) { SetValueL(&aStr.Value()); }
        TBool Equals(const TStrParam& aStr) const
            { return Id() == aStr.Id() && !Value().Compare(aStr.Value()); }

        virtual TInt Size() const;
        virtual void ReadL(RReadStream* aStream, TInt aSize);
        virtual void WriteL(RWriteStream* aStream) const;
        virtual CAknSettingItem* CreateSettingItemL();
    private:
        TStrParam& operator=(const TStrParam&) {/*please dont*/return *this;}
    };

private:
    RPointerArray<TParam> iParams;
    TBoolParam iAutoClean;
    TBoolParam iFileLog;
    TStrParam iPCHost;
    TPortParam iPCPort;
    TPortParam iAgentPort;
    TBoolParam iConnectToPC;
    TApParam iApId;
    TEnumParam iKeepAlivePeriod;
    TEnumParam iFontSize;

public:
    static CJavaDebugAgentSettings* NewL();
    ~CJavaDebugAgentSettings();

    void ApplyTo(CJavaDebugAgentDriver* aDriver) const;
    void CopyFrom(const CJavaDebugAgentSettings* aSettings);
    TBool Equals(const CJavaDebugAgentSettings* aSettings) const;
    void LoadL();
    void SaveL() const;

    TBool AutoClean() const { return iAutoClean.Value(); }
    TBool FileLog() const  { return iFileLog.Value(); }
    const TDesC& PCHost() const  { return iPCHost.Value(); }
    TInt PCPort() const  { return iPCPort.Value(); }
    TInt AgentPort() const  { return iAgentPort.Value(); }
    TBool ConnectToPC() const { return iConnectToPC.Value(); }
    TInt ApId() const { return iApId.Value(); }
    TInt KeepAlivePeriod() const { return iKeepAlivePeriod.Value(); }
    TInt FontSize() const { return iFontSize.Value(); }

    const TParam* FindParam(TInt aId) const;
    TParam* FindParam(TInt aId);

private:
    CJavaDebugAgentSettings();
    void ConstructL();
    void LoadL(const TDesC& aPrefsFile);
    void LoadL(RReadStream* aStream);
    void SaveL(const TDesC& aPrefsFile) const;
    void SaveL(RWriteStream* aStream) const;
};

#endif // __JAVA_DEBUG_AGENT_SETTINGS_H_

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

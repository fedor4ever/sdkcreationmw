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


#include <s32file.h>
#include "JavaDebugAgentDriver.h"
#include "JavaDebugAgentSettings.h"
#include "JavaDebugAgentSettingsApItem.h"

// Note that Driver.h defines KPrefsFile as "c:\midp2\jdebugagent\agent.ini"
// which is located inside the temporary directory c:\midp2\jdebugagent.
// This directory is deleted by CCleanupTable::CleanupL together with the
// prefs file. This is the reason why we have moved the prefs file one 
// level up.
_LIT(KDebugAgentPrefsFile, "c:\\midp2\\agent.ini");

// Prefs file signature
_LIT8(KPrefsFileSignature,"DAPF");

// CJavaDebugAgentSettings::TParam
CJavaDebugAgentSettings::TParam::~TParam() {}

// CJavaDebugAgentSettings::TPortParam
TInt CJavaDebugAgentSettings::TPortParam::Size() const { return KSize; }
void CJavaDebugAgentSettings::TPortParam::ReadL(RReadStream* aIn, TInt aSize)
{
    if (aSize != KSize) User::Leave(KErrCorrupt);
    iPort = aIn->ReadUint16L();
}
void CJavaDebugAgentSettings::TPortParam::WriteL(RWriteStream* aOut) const
{
    aOut->WriteUint16L(iPort);
}
CAknSettingItem* CJavaDebugAgentSettings::TPortParam::CreateSettingItemL()
{
    return new(ELeave)CAknIntegerEdwinSettingItem(Id(), iPort);
}

// CJavaDebugAgentSettings::TIntParam
TInt CJavaDebugAgentSettings::TIntParam::Size() const { return KSize; }
void CJavaDebugAgentSettings::TIntParam::ReadL(RReadStream* aIn, TInt aSize)
{
    if (aSize != KSize) User::Leave(KErrCorrupt);
    iInt = aIn->ReadInt32L();
}
void CJavaDebugAgentSettings::TIntParam::WriteL(RWriteStream* aOut) const
{
    aOut->WriteInt32L(iInt);
}
CAknSettingItem* CJavaDebugAgentSettings::TIntParam::CreateSettingItemL()
{
    return new(ELeave)CAknIntegerEdwinSettingItem(Id(), iInt);
}

// CJavaDebugAgentSettings::TApParam
CAknSettingItem* CJavaDebugAgentSettings::TApParam::CreateSettingItemL()
{
    return new(ELeave)CJavaDebugAgentSettingsApItem(Id(), &iInt);
}

// CJavaDebugAgentSettings::TEnumParam
CAknSettingItem* CJavaDebugAgentSettings::TEnumParam::CreateSettingItemL()
{
    return new(ELeave)CAknEnumeratedTextPopupSettingItem(Id(), iInt);
}

// CJavaDebugAgentSettings::TBoolParam
TInt CJavaDebugAgentSettings::TBoolParam::Size() const
{
    return KSize;
}
void CJavaDebugAgentSettings::TBoolParam::ReadL(RReadStream* aIn, TInt aSize)
{
    if (aSize != KSize) User::Leave(KErrCorrupt);
    iBool = aIn->ReadUint8L();
}
void CJavaDebugAgentSettings::TBoolParam::WriteL(RWriteStream* aOut) const
{
    aOut->WriteUint8L(iBool);
}
CAknSettingItem* CJavaDebugAgentSettings::TBoolParam::CreateSettingItemL()
{
    return new(ELeave)CJavaDebugAgentSettingsBooleanItem(Id(), iBool);
}

// CJavaDebugAgentSettings::TStrParam
TInt CJavaDebugAgentSettings::TStrParam::Size() const
{
    return Value().Size(); 
}
void CJavaDebugAgentSettings::TStrParam::SetValueL(const TDesC* aValue) 
{
    if (aValue && aValue->Length()) {
        iBuf.Copy(aValue->Left(iBuf.MaxLength()));
    } else {
        iBuf.SetLength(0);
    }
}
void CJavaDebugAgentSettings::TStrParam::ReadL(RReadStream* aIn, TInt aSize)
{
    // Note that zero is a valid record size 
    if (aSize % 2) User::Leave(KErrCorrupt);
    iBuf.SetLength(Min(aSize/2,iBuf.MaxLength()));
    for (TInt i=0; i<iBuf.Length(); i++) {
        iBuf[i] = aIn->ReadInt16L();
    }
    // Skip the rest
    aIn->ReadL(aSize - iBuf.Size());
}
void CJavaDebugAgentSettings::TStrParam::WriteL(RWriteStream* aOut) const
{
    aOut->WriteL(iBuf);
}
CAknSettingItem* CJavaDebugAgentSettings::TStrParam::CreateSettingItemL()
{
    return new(ELeave)CAknTextSettingItem(Id(), iBuf);
}

// CJavaDebugAgentSettings
CJavaDebugAgentSettings* CJavaDebugAgentSettings::NewL()
{
    CJavaDebugAgentSettings* self = new(ELeave)CJavaDebugAgentSettings;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

CJavaDebugAgentSettings::CJavaDebugAgentSettings() :
    iAutoClean(EJavaDebugAgentAutoCleanItem, ETrue),
    iFileLog(EJavaDebugAgentFileLogItem, EFalse),
    iPCHost(EJavaDebugAgentPCHostItem),
    iPCPort(EJavaDebugAgentPCPortItem, KPCPort),
    iAgentPort(EJavaDebugAgentAgentPortItem, KSeiSlavePort),
    iConnectToPC(EJavaDebugAgentConnectToPCItem, ETrue),
    iApId(EJavaDebugAgentApItem),
    iKeepAlivePeriod(EJavaDebugAgentKeepAliveItem, KDefaultKeepAlivePeriod),
    iFontSize(EJavaDebugAgentFontSizeItem, KDefaultDebugAgentFontSize)
{
}

void CJavaDebugAgentSettings::ConstructL()
{
    iParams.AppendL(&iAutoClean);
    iParams.AppendL(&iFileLog);
    iParams.AppendL(&iPCHost);
    iParams.AppendL(&iPCPort);
    iParams.AppendL(&iAgentPort);
    iParams.AppendL(&iConnectToPC);
    iParams.AppendL(&iApId);
    iParams.AppendL(&iKeepAlivePeriod);
    iParams.AppendL(&iFontSize);
}

CJavaDebugAgentSettings::~CJavaDebugAgentSettings()
{
    iParams.Reset();
}

void CJavaDebugAgentSettings::ApplyTo(CJavaDebugAgentDriver* aDriver) const
{
    aDriver->SetAutoCleanEnabled(iAutoClean.Value());
    aDriver->SetFileLog(iFileLog.Value());
    TRAP_IGNORE(aDriver->SetPCHostL(iPCHost.Value()));
    aDriver->SetPCPort(iPCPort.Value());
    aDriver->SetAgentPort(iAgentPort.Value());
    aDriver->SetApId(iApId.Value());
    aDriver->SetKeepAlivePeriod(iKeepAlivePeriod.Value());
}

void CJavaDebugAgentSettings::CopyFrom(const CJavaDebugAgentSettings* aSrc)
{
    iAutoClean.SetValue(aSrc->iAutoClean);
    iFileLog.SetValue(aSrc->iFileLog);
    TRAP_IGNORE(iPCHost.SetValueL(&aSrc->iPCHost.Value()));
    iPCPort.SetValue(aSrc->iPCPort);
    iAgentPort.SetValue(aSrc->iAgentPort);
    iConnectToPC.SetValue(aSrc->iConnectToPC);
    iApId.SetValue(aSrc->iApId);
    iKeepAlivePeriod.SetValue(aSrc->iKeepAlivePeriod);
    iFontSize.SetValue(aSrc->iFontSize);
}

TBool CJavaDebugAgentSettings::Equals(const
      CJavaDebugAgentSettings* aSettings) const
{
    return iAutoClean.Equals(aSettings->iAutoClean) &&
           iFileLog.Equals(aSettings->iFileLog) &&
           iPCHost.Equals(aSettings->iPCHost) &&
           iPCPort.Equals(aSettings->iPCPort) &&
           iAgentPort.Equals(aSettings->iAgentPort) &&
           iConnectToPC.Equals(aSettings->iConnectToPC) &&
           iApId.Equals(aSettings->iApId) &&
           iKeepAlivePeriod.Equals(aSettings->iKeepAlivePeriod) &&
           iFontSize.Equals(aSettings->iFontSize);
}

const CJavaDebugAgentSettings::TParam* CJavaDebugAgentSettings::FindParam(
    TInt aId) const
{
    for (TInt i=0; i<iParams.Count(); i++)
    {
        if (iParams[i]->Id() == aId)
        {
            return iParams[i];
        }
    }
    return NULL;
}

CJavaDebugAgentSettings::TParam* CJavaDebugAgentSettings::FindParam(TInt aId)
{
    for (TInt i=0; i<iParams.Count(); i++)
    {
        if (iParams[i]->Id() == aId)
        {
            return iParams[i];
        }
    }
    return NULL;
}

void CJavaDebugAgentSettings::LoadL()
{
    LoadL(KDebugAgentPrefsFile);
}

void CJavaDebugAgentSettings::SaveL() const
{
    SaveL(KDebugAgentPrefsFile);
}


void CJavaDebugAgentSettings::LoadL(const TDesC& aPrefsFile)
{
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);

    RFileReadStream in;
    in.PushL();
    User::LeaveIfError(in.Open(fs, aPrefsFile, EFileRead));
    TRAPD(err, LoadL(&in));
    in.Close();
    in.Pop();

    if (err != KErrNone) {
        err = fs.Delete(aPrefsFile);
    }
    CleanupStack::PopAndDestroy(&fs);
}

void CJavaDebugAgentSettings::SaveL(const TDesC& aPrefsFile) const
{
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);

    RFileWriteStream out;
    out.PushL();
    TInt err = out.Create(fs, aPrefsFile, EFileWrite);
    if (err != KErrNone) {
        // Try to eliminate the problem if we can
        switch (err) {
        case KErrAlreadyExists:
            fs.Delete(aPrefsFile);
            break;
        case KErrPathNotFound:
            fs.MkDirAll(aPrefsFile);
            break;
        default:
            User::Leave(err);
            break;
        }
        User::LeaveIfError(out.Create(fs, aPrefsFile, EFileWrite));
    }

    SaveL(&out);
    out.Close();
    out.Pop();
    CleanupStack::PopAndDestroy(&fs);
}

void CJavaDebugAgentSettings::LoadL(RReadStream* aStream)
{
    // Read in and verify the signature
    for (TInt k=0; k<KPrefsFileSignature().Length(); k++) {
        TUint8 sig = aStream->ReadUint8L();
        if (sig != KPrefsFileSignature()[k]) {
            User::Leave(KErrCorrupt);
        }
    }

    // Read individual settings
    for (;;) {
        TInt len = 0;
        TRAPD(err, len = aStream->ReadInt32L());
        if (err != KErrNone) {
            if (err == KErrEof) break;
            User::Leave(err);
        }
        if (len < 4) User::Leave(KErrCorrupt);
        TParam* param = FindParam(aStream->ReadInt32L());
        if (param) {
            param->ReadL(aStream, len-4);
        } else {
            aStream->ReadL(len-4);
        }
    }
}

void CJavaDebugAgentSettings::SaveL(RWriteStream* aStream) const
{
    TInt i;
    aStream->WriteL(KPrefsFileSignature());
    for (i=0; i<iParams.Count(); i++) {
        TInt size = iParams[i]->Size();
        if (size >= 0) {
            aStream->WriteInt32L(size+4);
            aStream->WriteInt32L(iParams[i]->Id());
            iParams[i]->WriteL(aStream);
        }
    }
}

/**
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

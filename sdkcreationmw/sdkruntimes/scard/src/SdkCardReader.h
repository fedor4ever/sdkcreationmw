/*
* Copyright (c) 2003-2005 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef _SDK_CARDREADER_H_
#define _SDK_CARDREADER_H_

#include <emulator.h>
#include <winscard.h>
#include <ScardReader.h>

/**
 * Smart card reader. 
 */
class SdkScardLauncher;
class CSdkCardReader : public CActive, public MScardReader
{
private:
    SdkScardLauncher* iLauncher;
    SCARD_READERSTATE iReaderState;
    SCARDHANDLE iCardHandle;
    TRequestStatus iEventThreadStatus;
    RSemaphore iRequest;
    RThread iEventThread;
    RThread iRequestThread;
    TPtrC iName;
    TReaderID iId;
    TInt iRefCount;
    TBool iFlags; // See below:
    enum TFlags {
        EFlagHaveSemaphore  = 0x0001,
        EFlagRequestThread  = 0x0002,
        EFlagEventThread    = 0x0004,
        EFlagCardIn         = 0x0008,
        EFlagOpen           = 0x0010,
        EFlagHasId          = 0x0020,
        EFlagDead           = 0x0040
    };

public: 
    static CSdkCardReader* NewL(SdkScardLauncher* aLauncher,
                                const TText* aName);
    virtual ~CSdkCardReader();

    inline TBool HasId() const;
    inline void SetId(TReaderID aId);
    inline TReaderID Id() const;
    inline TBool MatchId(TReaderID aId) const;
    inline const TDesC& Name() const;
    inline void AddRef();
    TInt ReleaseRef();

    // CActive
    virtual void DoCancel();
    virtual void RunL();

    // MScardReader
    virtual void Open(TRequestStatus& aStatus); 
    virtual TInt Close();
    virtual void CancelTransmit();
    virtual TInt GetATR(TScardATR& anATR);
    virtual TBool GetCapabilities(TRequestStatus& aStatus, 
                                  const TInt32 aTag,
                                  TPtr8& aValue,
                                  const TInt32 aTimeout);
    virtual void TransmitToCard(TRequestStatus& aStatus, 
                                const TPtrC8& aCommand, 
                                TPtr8& aResponse,
                                const TInt32 aTimeout);

private:
    CSdkCardReader(SdkScardLauncher* aLauncher, const TText* aName);
    void ConstructL();
    void EventThread();
    static TInt EventThread(TAny* aParam);
    void Notify(TScardServiceStatus aEvent);
    void SubmitEventRequest();
};

inline TBool CSdkCardReader::HasId() const
    { return ((iFlags & EFlagHasId) != 0); }
inline void CSdkCardReader::SetId(TReaderID aId)
    { iId = aId; iFlags |= EFlagHasId; }
inline TReaderID CSdkCardReader::Id() const
    { ASSERT(HasId()); return iId; }
inline TBool CSdkCardReader::MatchId(TReaderID aId) const
    { return (iFlags & EFlagHasId) && (iId == aId); }
inline const TDesC& CSdkCardReader::Name() const
    { return iName; }
inline void CSdkCardReader::AddRef()
    { iRefCount++; }
    
#endif // _SDK_CARDREADER_H_

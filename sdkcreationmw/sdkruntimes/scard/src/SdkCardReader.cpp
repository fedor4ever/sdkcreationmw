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


#define TRACE_PREFIX "SCARD: [Reader] "
#include "SdkCardDebug.h"

#define WINSCARDAPI
#define WINSCARDDATA
#include <windows.h>
#include <winscard.h>

#include "SdkCardReader.h"
#include "SdkCardLauncher.h"

#define EVENT_WAIT_TIMEOUT 10000

/* copied from wim_apdu.h */
#define READER_REMOVABLE   0x08
#define READER_PRESENT     0x10
#define READER_ID1_SIZE    0x20
#define CARD_PRESENT       0x40
#define CARD_POWERED       0x80

/* status byte received in WIM_ReceiveList,
   reader present, card present, card powered */
#define WIM_CARD_STATUS_AVAILABLE  (READER_PRESENT|CARD_PRESENT|CARD_POWERED)

/**
 * Static constructor. This class may (and does) keep pointer to the reader
 * name passsed in as a parameter. The creator of this class is responsible
 * for keeping track of that memory.
 */
CSdkCardReader* CSdkCardReader::NewL(SdkScardLauncher* aLauncher,
                                     const TText* aName)
{
    CSdkCardReader* self = new CSdkCardReader(aLauncher, aName);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

/**
 * Private constructor.
 */
CSdkCardReader::CSdkCardReader(SdkScardLauncher* aLauncher, 
                               const TText* aName) :
CActive(EPriorityNormal),
iLauncher(aLauncher),
iName(aName),
iRefCount(1)
{
    CActiveScheduler::Add(this);
    iReaderState.szReader = aName;
}

/**
 * Destructor.
 */
CSdkCardReader::~CSdkCardReader()
{
    Close();
    Cancel();
    if (iFlags & EFlagHaveSemaphore) iRequest.Close();
    if (iFlags & EFlagRequestThread) iRequestThread.Close();
    TRACE1("CSdkCardReader[%08x] destroyed",this);
}

/**
 * Second stage constructor
 */
void CSdkCardReader::ConstructL()
{
    LEAVE_IF_ERROR(iRequest.CreateLocal(0));
    iFlags |= EFlagHaveSemaphore;
    LEAVE_IF_ERROR(iRequestThread.Open(RThread().Id()));
    iFlags |= EFlagRequestThread;
    TRACE3("CSdkCardReader[%08x] created, id=%u, \"%S\"",this,iId,&iName);
}

/**
 * Releases a reference to the reader object
 */
TInt CSdkCardReader::ReleaseRef()
{
    TInt ref = (--iRefCount);
    ASSERT(ref >= 0);
    if (ref < 1)
    {
        delete this;
        return 0;
    }
    else
    {
        return ref;
    }
}

/**
 * Notifies SdkScardLauncher that something has happened.
 */
void CSdkCardReader::Notify(TScardServiceStatus aEvent)
{
    if (iFlags & EFlagOpen)
    {
        iLauncher->NotifyCardEvent(aEvent, this); 
    }
}

/**
 * Initializes the Card Reader. Implementations of this method must carry
 * out all steps required to set the concrete reader into a proper state.
 * After invoking this method it should be possible to communicate with 
 * the reader properly.
 */
void CSdkCardReader::Open(TRequestStatus& aStatus)
{
    TInt err = KErrNotReady;
    TRequestStatus *req = (&aStatus);
    ASSERT(!(iFlags & EFlagOpen));
    if (!(iFlags & EFlagOpen))
    {
        DWORD prot;
        LONG scerr = SCardConnect(iLauncher->ServiceContext(), iName.Ptr(),
            SCARD_SHARE_EXCLUSIVE, SCARD_PROTOCOL_T0, &iCardHandle, &prot);
        if (scerr == SCARD_S_SUCCESS)
        {
            ASSERT(iCardHandle);
            TBuf<32> name;
            ASSERT(HasId());
            _LIT(KThreadNameFormat, "SdkCardReaderThread%d");
            name.Format(KThreadNameFormat,iId);
            ASSERT(!(iFlags & EFlagEventThread));
            err = iEventThread.Create(name,EventThread,KDefaultStackSize,0,this);
            if (err == KErrNone)
            {
                iFlags |= EFlagEventThread;
                iEventThread.Logon(iEventThreadStatus);
                iEventThread.Resume();
                SubmitEventRequest();
                iFlags |= EFlagOpen;
            }
        }
        else
        {
            TRACE1("SCardConnect error 0x%08x",scerr);
        }
    }
    User::RequestComplete(req, err);
}

/**
 * Closes the Card Reader. Implementations of this method must carry out
 * all steps required to close the concrete terminal and free resources 
 * held by it.
 */
TInt CSdkCardReader::Close()
{
    CancelTransmit();
    iFlags &= ~EFlagOpen;
    if (iFlags & EFlagEventThread)
    {
        iEventThread.Close();
        User::WaitForRequest(iEventThreadStatus);
        iFlags &= ~EFlagEventThread;
    }
    if (iCardHandle)
    {
        SCardDisconnect(iCardHandle, SCARD_RESET_CARD);
        iCardHandle = NULL;
    }
    return KErrNone;
}

/**
 * Cancel any pending requests (if any)
 */
void CSdkCardReader::CancelTransmit()
{
}

/**
 * Return the ATR bytes. 
 */
TInt CSdkCardReader::GetATR(TScardATR& anATR)
{
    anATR.Copy(iReaderState.rgbAtr, KMaxATRBytes);
    return KErrNone;
}

/**
 * Return the value corresponding to the specified TAG parameter
 */
TBool CSdkCardReader::GetCapabilities(TRequestStatus& aStatus, 
                                      const TInt32 aTag,
                                      TPtr8& aValue,
                                      const TInt32 /*aTimeout*/)
{
    TUint8 tmp;
    TRequestStatus *req = (&aStatus);
    switch (aTag)
    {
    case KCardPrecence:
        TRACE("GetCapabilities(KCardPrecence)");
        tmp = 1;
        aValue.Copy(&tmp, 1);
        User::RequestComplete(req, KErrNone);
        break;
    case KCardStatus:
        TRACE("GetCapabilities(KCardStatus)");
        tmp = WIM_CARD_STATUS_AVAILABLE;
        aValue.Copy(&tmp, 1);
        User::RequestComplete(req, KErrNone);
        break;
    default:
        TRACE1("GetCapabilities - unsupported tag 0x%08x",aTag);
        User::RequestComplete(req, KErrNotSupported);
        break;
    }
    return ETrue;
}

/**
 * Send a Command(APDU) to the card and return Response(APDU)
 * received from card.
 */
void CSdkCardReader::TransmitToCard(TRequestStatus& aStatus, 
                                    const TPtrC8& aCommand, 
                                    TPtr8& aResponse,
                                    const TInt32 /*aTimeout*/ )
{
    TRequestStatus *req = (&aStatus);
    const TUint8* cmd = aCommand.Ptr();
    TUint cmdlen = aCommand.Length();

    // WIM library sometimes adds an extra byte to the end of APDU.
    // Take it out.
    if (cmdlen > 5 && cmd[4] < (cmdlen-5))
    {
        cmdlen--;
    }

    // Prepare output buffer
    DWORD recvLen = aResponse.MaxLength();
    aResponse.SetLength(recvLen);
    TUint8* resp = &aResponse[0];

    // Submit APDU and get the response
    LONG err = SCardTransmit(iCardHandle, SCARD_PCI_T0, cmd, cmdlen,
        NULL, resp, &recvLen);
    if (err == SCARD_S_SUCCESS)
    {
        if (recvLen == 2 && resp[0] == 0x61)
        {
            // That wasn't the real reaponse. We need to tell the card that 
            // we REALLY want the response data.
            TUint8 getRespApdu[5];
            getRespApdu[0] = (cmd[0] & 0x03); // copy logical channel
            getRespApdu[1] = 0xc0;
            getRespApdu[2] = 0;
            getRespApdu[3] = 0;
            getRespApdu[4] = resp[1];
            recvLen = aResponse.MaxLength();
            err = SCardTransmit(iCardHandle, SCARD_PCI_T0, getRespApdu, 5,
                NULL, resp, &recvLen);
        }
    }

    // Check the status
    if (err == SCARD_S_SUCCESS)
    {
        aResponse.SetLength(recvLen);
        User::RequestComplete(req, KErrNone);
    }
    else
    {
        aResponse.SetLength(0);
        TRACE1("SCardTransmit error 0x%08x",err);
        User::RequestComplete(req, (err == SCARD_E_TIMEOUT) ? 
            KErrTimedOut : KErrHardwareNotAvailable);
    }
}

// CActive
void CSdkCardReader::DoCancel()
{
    TRACE("DoCancel");
    ASSERT(!(iFlags & EFlagDead));
    iFlags |= EFlagDead;
    SCardCancel(iLauncher->ServiceContext());
    iRequest.Signal();
    // Waiter will complete the request
}

void CSdkCardReader::RunL()
{
    TInt oldFlags = iReaderState.dwCurrentState & ~iReaderState.dwEventState;
    TInt newFlags = iReaderState.dwEventState & ~iReaderState.dwCurrentState;
    if (newFlags & (SCARD_STATE_UNKNOWN|SCARD_STATE_UNAVAILABLE))
    {
        iFlags |= EFlagDead;
        iRequest.Signal();

        // The following call may deallocaet this object. Don't touch the
        // 'this' pointer after this call returns.
        Notify(EReaderRemoved);
    }
    else
    {
        if ((newFlags & (SCARD_STATE_UNPOWERED|SCARD_STATE_EMPTY)) ||
            (oldFlags & SCARD_STATE_PRESENT))
        {
            Notify(EScardRemoved);
        }
        else if (newFlags & SCARD_STATE_PRESENT)
        {
            Notify(EScardInserted);
        }
        else if (oldFlags & SCARD_STATE_UNPOWERED|SCARD_STATE_EMPTY)
        {
            Notify(EScardReseted);
        }
        SubmitEventRequest();
    }
}

// Event thread
void CSdkCardReader::EventThread()
{
    TRACE("EventThread started");
    for(;;)
    {
        // Wait for event request
        iRequest.Wait();
        LONG err = SCARD_E_TIMEOUT;
        while (err == SCARD_E_TIMEOUT && !(iFlags & EFlagDead))
        {
            Emulator::Escape();
            err = SCardGetStatusChange(iLauncher->ServiceContext(),
                EVENT_WAIT_TIMEOUT, &iReaderState, 1);
            Emulator::Reenter();
        }
        if (iFlags & EFlagDead) break;
        TRequestStatus *req = (&iStatus);
        iRequestThread.RequestComplete(req, (err == SCARD_S_SUCCESS) ? 
            KErrNone : KErrHardwareNotAvailable);
        // RunL will analyze the event and submit new request by signaling
        // the request semaphore
    }
    TRequestStatus *req = (&iStatus);
    iRequestThread.RequestComplete(req, KErrCancel);
    TRACE("EventThread exiting");
}

TInt CSdkCardReader::EventThread(TAny* aParam)
{
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if (cleanupStack)
    {
        CSdkCardReader* that = (CSdkCardReader*)aParam;
        that->EventThread();
        delete cleanupStack;
    }
    return KErrNone;
}

void CSdkCardReader::SubmitEventRequest()
{
    iReaderState.dwCurrentState = iReaderState.dwEventState;
    ASSERT(!(iFlags & EFlagDead));
    iStatus = KRequestPending;
    SetActive();
    iRequest.Signal();
}

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
* Description:  Push support
 *
*/


#include "EcmtMenubar.h"
#include "EcmtWin32.h"
#include "httpfile.h"

#include <e32base.h>
#include <apgtask.h>
#include <apgcli.h>
#include <vwsdef.h>
#include <viewcli.h>
#include <cmanobserver.h>
#include <e32property.h>
#include <PSVariables.h>
#include <EscapeUtils.h>
#include <emulator.h>
#include <sacls.h>
#include <msvids.h>
#include <msvstd.hrh>
#include <mmssettings.h>
#include <PushLog.h>
#include <PushMessage.h>
#include <PushDispatcher.h>
#include <CPushHandlerBase.h>

// We define CWatcherLog locally because CWatcherLog::NewL is not
// exported from watcher.dll
#undef IMPORT_C
#define IMPORT_C
#include <watcher.h>
  
// MACROS
#define __S8(x) _S8(x)

// Some view ids
#define KMessagingUid TUid::Uid(0x100058C5)
#define KMessagingCentreMainViewUid TVwsViewId(KMessagingUid,TUid::Uid(0x01))
#define KMessagingCentreInboxView TVwsViewId(KMessagingUid,TUid::Uid(0x02))

// Known push content types
typedef enum _TPushMessageType {
    EWapPush,
    EMultipart,
    EMmsMessage
} TPushMessageType;

typedef struct _PushType {
    const TText* ext;
    const TText8* contentType;
    TPushMessageType msgType;
    TUint8 wapContentType;
} PushType;

// WAP encoded content types must have 0x80 bit set
const TUint8 WAP_MP_MIX = 0xA3; // application/vnd.wap.multipart.mixed
const TUint8 WAP_MP_ALT = 0xA6; // application/vnd.wap.multipart.alternative
const TUint8 WAP_MP_REL = 0xB3; // application/vnd.wap.multipart.related
static const PushType pushTypes[] = {
    {_S("sic"),__S8("application/vnd.wap.sic"), EWapPush, 0xAE},
    {_S("slc"),__S8("application/vnd.wap.slc"), EWapPush, 0xB0},
    {_S("coc"),__S8("application/vnd.wap.coc"), EWapPush, 0xB2},
    {_S("mms"),__S8("application/vnd.wap.mms-message"), EMmsMessage, 0},
    {_S("mpc"),__S8("application/vnd.wap.multipart.mixed"), EMultipart, WAP_MP_MIX},
    {_S("mpc"),__S8("application/vnd.wap.multipart.related"), EMultipart, WAP_MP_REL},
    {_S("mpc"),__S8("application/vnd.wap.multipart.alternative"), EMultipart, WAP_MP_ALT}
};

// --------------------------------------------------------------------------
// This class implements MWapPushLog and MConnManObserver interfaces
// --------------------------------------------------------------------------

class CEcmtMenubar::CWapPushSupport :
    public MWapPushLog, 
    public MConnManObserver
{
public:
    // MWapPushLog
    void WPLPrintf(const TDesC&);
    void WPLPrintfL(CPushMessage&);
    void WPLLogBinaryAsHex(const TDesC&);
    void WPLLogError(const TDesC&,TInt );

    // MConnManObserver
    void CMOpenConnectionL(TPushConnPoint&);
    void CMWatcherComplete(CCOWatcherBase&, TInt);
};

// MWapPushLog
void CEcmtMenubar::CWapPushSupport::WPLPrintf(const TDesC&) {}
void CEcmtMenubar::CWapPushSupport::WPLPrintfL(CPushMessage&) {}
void CEcmtMenubar::CWapPushSupport::WPLLogBinaryAsHex(const TDesC&) {}
void CEcmtMenubar::CWapPushSupport::WPLLogError(const TDesC&,TInt ) {}

// MConnManObserver
void CEcmtMenubar::CWapPushSupport::CMOpenConnectionL(TPushConnPoint&) {}
void CEcmtMenubar::CWapPushSupport::CMWatcherComplete(CCOWatcherBase&,TInt) {}

// --------------------------------------------------------------------------
// CWatcherLog
// --------------------------------------------------------------------------
CWatcherLog* CWatcherLog::NewL(RFs& aFs)
{
    CWatcherLog* self = new(ELeave)CWatcherLog(aFs);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

CWatcherLog::CWatcherLog(RFs& aFs) : CBase(), iFs(aFs)
{
}

CWatcherLog::~CWatcherLog()
{
}

void CWatcherLog::ConstructL()
{
}

void CWatcherLog::Printf(TRefByValue<const TDesC16>,...)
{
}

void CWatcherLog::Printf(TRefByValue<const TDesC8>,...)
{
}

TBool CWatcherLog::IsLogging() const
{
    return EFalse;
}

// --------------------------------------------------------------------------
// Some obscure code
// --------------------------------------------------------------------------

inline void LaunchViewL(const TVwsViewId& aViewId,
                        TUid aCustomMessageId,
                        const TDesC8& aCustomMessage)
{
    CVwsSessionWrapper* view = CVwsSessionWrapper::NewLC();
    view->CreateActivateViewEvent(aViewId, aCustomMessageId, aCustomMessage);
    CleanupStack::PopAndDestroy(view);
}

inline void LaunchMmsViewL()
{
    LaunchViewL(KMessagingCentreInboxView, 
        TUid::Uid(KMsvGlobalInBoxIndexEntryIdValue), 
        KNullDesC8());
}

inline void LaunchPushViewL()
{
    // The same view handles Wap Push messages as well as MMS messages
    LaunchMmsViewL();
}

inline void InboxStatusNotifyL()
{
    TInt r =  RProperty::Define(KUidSystemCategory, 
                                KUidInboxStatus.iUid, 
                                RProperty::EInt);

    if (r == KErrNone)
    {
        RProperty::Set(KUidSystemCategory, 
                       KUidInboxStatus.iUid, 
                       ESADocumentsInInbox );
    }
}

// --------------------------------------------------------------------------
// MMS support
// --------------------------------------------------------------------------

/**
 * Reads the location of the mmsin directory from localmode.ini
 * This is a Java-style property file, only in Unicode.
 * See CMmsWatcher::ReadLocalModeConfigData() method in
 * S60\mmsengine\mmswatcher\src\mmswatcher.cpp
 * The algorithm is reproduced here, with all its problems,
 * because we want it to exactly match what CMmsWatcher does
 */
HBufC* CEcmtMenubar::ReadMmsInDir(RFs& aFs)
{
    HBufC* mmsInBuf = NULL;
    RFileReadStream reader;

    // Actually, if localmode.ini file does not exist, the MMS watcher
    // does not poll the mmsin directory and everything we are doing
    // here is useless. Should we create localmode.ini file if it 
    // doesn't exist? But then SDK would have to be restarted because
    // MMS watcher only attempts to read this file on startup... Hmm.
    TInt err = reader.Open(aFs,KMmsLocalModeConfigFile,EFileShareReadersOnly);
    if (err == KErrNone)
    {
        TChar delim = 0x000A;
        TBuf<128> line;
        while (err == KErrNone)
        {
            TRAP(err, reader.ReadL( line, delim ) );
            if (err == KErrNone)
            {
                TInt length = line.Length();
                if (length > 2)
                {
                    // Check for comment line
                    if (line[0] != '#')
                    {
                        // Check for byte order mark
                        if (line[0] == 0xFEFF)
                        {
                            line.Delete( 0, 1 );
                            length = line.Length();
                        }
                        // Drop CR+LF from the end of line
                        line.Delete(length-2, 2);
                        TInt sep = line.Locate('=');
                        if (sep > 0)
                        {
                            TPtrC key = line.Left(sep);
                            if (key.CompareF(KMmsLocalmodeInDirectory) == 0)
                            {
                                mmsInBuf = line.Mid(sep+1).Alloc();
                                break;
                            }
                        }
                    }
                }
            }
        }
        reader.Close();
    }
    return mmsInBuf;
}

// Returns reference to the mmsin directory 
const TDesC& CEcmtMenubar::MmmInDirL()
{
    if (!iMmsInDir)
    {
        iMmsInDirBuf = ReadMmsInDir(FsSessionL());
        if (iMmsInDirBuf)
        {
            iMmsInDir = iMmsInDirBuf;
        }
        else
        {
            iMmsInDir = &KMmsDefaultLocalModeDir;
        }
    }
    return *iMmsInDir;
}

// Simulates an MMS message
void CEcmtMenubar::SimulateMmsL(const TDesC8& aMessageBody)
{
    // First copy the file to the phone's file system
    const TDesC& mmsDir = MmmInDirL();
    RFs& fs = FsSessionL();

    // Make sure the directory exists
    TUint attr = 0;
    if (fs.Att(mmsDir,attr) != KErrNone || !(attr & KEntryAttDir))
    {
        fs.Delete(mmsDir); // Ignore errors
        User::LeaveIfError(fs.MkDir(mmsDir));
    }

    // Create a temporary file. The extension must be .mms
    RFile file;
    TInt err = KErrAlreadyExists;
    for (TInt i=0; err == KErrAlreadyExists && i<1000; i++)
    {
        _LIT(KMmsFilePrefix,"TMP");
        _LIT(KMmsExt,".mms");
        TFileName name;
        name.Copy(mmsDir);
        name.Append(KMmsFilePrefix());
        name.AppendFormat(_L("%04d"),i);
        name.Append(KMmsExt());
        err = file.Create(fs,name,EFileWrite);
    }

    // Write the file
    User::LeaveIfError(err);
    err = file.Write(aMessageBody);
    file.Close();
    User::LeaveIfError(err);

    // Open the Messaging application
    InboxStatusNotifyL();
    LaunchMmsViewL();
}

// --------------------------------------------------------------------------
// Push support
// --------------------------------------------------------------------------

/**
 * Returns a reference to CWatcherLog.
 */
CWatcherLog& CEcmtMenubar::WatcherLogL()
{
    if (!iWatcherLog)
    {
        iWatcherLog = CWatcherLog::NewL(FsSessionL());
    }
    return *iWatcherLog;
}

/**
 * Wap push simulator. Takes care of destroying CPushMessage
 */
void CEcmtMenubar::SimulateWapPushL(CPushMessage* aPushMessage)
{
    TPtrC8 rAppURI;
    TInt rAppID;
    TBool rIsAnInt;
    CPushHandlerBase* appHandlerPtr = NULL;

    CleanupStack::PushL(aPushMessage);
    if (!iWapPushSupport) iWapPushSupport = new(ELeave)CWapPushSupport;
    if (aPushMessage->GetAppIdL(rAppURI, rAppID, rIsAnInt))
    {
        if(rIsAnInt)
        {
            appHandlerPtr = &PushAppDispatcher::GetHandlerL(rAppID,
                *iWapPushSupport, *iWapPushSupport);
        }
        else
        {
            appHandlerPtr = &PushAppDispatcher::GetHandlerL(rAppURI,
                *iWapPushSupport, *iWapPushSupport);
        }
    }
    else
    {   // If no AppID defined, use the default User Agent
        appHandlerPtr= &PushAppDispatcher::GetHandlerL(KUserAgentAppHandler,
            *iWapPushSupport, *iWapPushSupport);
    }

    if (appHandlerPtr)
    {
        appHandlerPtr->HandleMessageL(aPushMessage);
        CleanupStack::Pop(aPushMessage);
    }
    else
    {
        CleanupStack::PopAndDestroy(aPushMessage);
    }
    
    // Show the indicator
    InboxStatusNotifyL();
    LaunchPushViewL();
}


// --------------------------------------------------------------------------
// The main routine that does all the Push/MMS work
// --------------------------------------------------------------------------

/**
 * Detects whether the file is an MMS or a Wap Push message and
 * attempts to push it to the phone. Returns ETrue on success,
 * EFalse on failure or if the file does not seem to contain
 * push content.
 */
TBool CEcmtMenubar::LoadFileAsPushL(const TDesC& aFileName)
{
    // First check if this is an HTTP response file.
    ParseHttpResponse* resp = NULL;
    const PushType* pushType = NULL;
    char* fname = STRING_ToMultiByteN(aFileName.Ptr(),aFileName.Length());
    if (fname)
    {
        resp = HTTP_ParseFile(fname, HttpFalse);
        if (resp)
        {
            if (HTTP_ResponseStatus(resp) == HTTP_STATUS_CODE_OK)
            {
                const HttpContent* content = HTTP_ResponseContent(resp);
                if (content && content->type)
                {
                    for (TInt i=0; i<COUNT(pushTypes) && !pushType; i++)
                    {
                        const char* type = (char*)pushTypes[i].contentType;
                        if (!strcmp(type, content->type))
                        {
                            pushType = pushTypes + i;
                        }
                    }
                }
            }
            if (!pushType)
            {
                HTTP_ResponseDelete(resp);
                resp = NULL;
            }
        }
        MEM_Free(fname);
    }

    // Now check if this file has one of the DRM extensions
    if (!pushType)
    {
        TPtrC fileExt(FileExt(aFileName));
        for (TInt i=0; i<COUNT(pushTypes) && !pushType; i++)
        {
            if (fileExt.CompareF(TPtrC(pushTypes[i].ext)) == 0) {
                pushType = pushTypes + i;
            }
        }
    }

    // If we suspect that this is a push or MMS message, load the file
    // into a memory buffer
    HBufC8* msgBody = NULL;
    if (pushType)
    {
        if (resp)
        {
            const HttpContent* hc = HTTP_ResponseContent(resp);
            msgBody = HBufC8::New(hc->size); // No leaving, please
            if (msgBody)
            {
                msgBody->Des().Append(TPtrC8((TText8*)hc->data, hc->size));
            }
            HTTP_ResponseDelete(resp);
        }
        else
        {
            msgBody = ReadExtFile(aFileName);
        }
    }

    if (msgBody && msgBody->Length() > 0)
    {
        // File has been loaded. Do something with it
        CleanupStack::PushL(msgBody);
        if (pushType->msgType == EWapPush || 
            pushType->msgType == EMultipart)
        {
            HBufC8* msgHeader = NULL;
            TUint8 wapContentType = pushType->wapContentType;
            if (pushType->msgType == EMultipart)
            {
                const TUint8 * msgData = msgBody->Ptr();
                TInt msgLen = msgBody->Length();
                if (msgLen > 1 && msgData[0] >= 1 && msgData[0] < msgLen && (
                    msgData[1] == WAP_MP_MIX || 
                    msgData[1] == WAP_MP_ALT ||
                    msgData[1] == WAP_MP_REL))
                {
                    // Looks like multipart message contains encoded headers
                    // Separate body from the headers
                    HBufC8* msgBody2 = msgBody->Mid(msgData[0]+1,
                        msgLen-msgData[0]-1).AllocLC();
                    msgHeader = msgBody->Mid(1,msgData[0]).AllocL();

                    // Switch to the new body
                    CleanupStack::Pop(msgBody2);
                    CleanupStack::PopAndDestroy(msgBody);
                    msgBody = msgBody2;
                    msgLen = msgBody->Length();
                    CleanupStack::PushL(msgBody);
                    CleanupStack::PushL(msgHeader);
                }
                else
                {
                    // Assume application/vnd.wap.multipart.mixed
                    wapContentType = WAP_MP_MIX;
                }
            }

            if (!msgHeader)
            {
                // We have no headers, allocate a fake one with just
                // the content type
                msgHeader = HBufC8::NewLC(1);
                TPtr8 msgHeaderDes(msgHeader->Des());
                msgHeaderDes.SetLength(1);
                msgHeaderDes[0] = wapContentType;
            }

            CPushMessage* msg = CPushMessage::NewL(msgHeader, msgBody);
            CleanupStack::Pop(msgHeader);
            CleanupStack::Pop(msgBody);
            SimulateWapPushL(msg);
        }
        else
        {
            SimulateMmsL(*msgBody);
            CleanupStack::PopAndDestroy(msgBody);
        }

        return ETrue;
    }
    return EFalse;
}

/**
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

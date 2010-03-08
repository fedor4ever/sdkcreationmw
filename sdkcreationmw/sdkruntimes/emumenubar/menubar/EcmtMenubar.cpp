/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Symbian specific part of EcmtMenubar.dll
 *
*/


// INCLUDE FILES

#include "EcmtMenubarUids.h"
#include "EcmtMenubar.h"
#include "EcmtMenubarConstants.h"
#include "MEcmtMenubarCallbacks.h"
#include "EcmtSdkPlugin.h"
#include <eikenv.h>
#include <apgcli.h>
#include <apgtask.h>
#include <EscapeUtils.h>
#include <DocumentHandler.h>
#include <AiwGenericParam.h>

typedef struct _ExtMimeMap {
    const TText* ext;
    const TText8* contentType;
} ExtMimeMap;

static const ExtMimeMap BrowserTypes[] = {
    _S("html"),  _S8("text/html"),
    _S("htm"),   _S8("text/html"),
    _S("xhtml"), _S8("text/xhtml"),
    _S("xml"),   _S8("text/xml"),
    _S("wml"),   _S8("text/vnd.wap.wml"),
    _S("wmlc"),  _S8("application/vnd.wap.wmlc"),
    _S("wmlsc"), _S8("application/vnd.wap.wmlscriptc"),
    _S("wmls"),  _S8("text/vnd.wap.wmlscript"),
    _S("dtd"),   _S8("text/vnd.symbian.wml.dtd"),
    _S("wbxml"), _S8("application/vnd.wap.wbxml")
};

// MACROS

#define COUNT(a) (sizeof(a)/sizeof((a)[0]))
RWsSession iWs; 
// CONSTANTS

// Retry delay when trying to connect to the window server or
// waiting for an application to come up
const TInt KConnectRetryDelay = 500000;

// File URL schemes
#define __LIT(x,y) _LIT(x,y)
__LIT(KFileScheme, FILE_SCHEME);

// UID of the some standard applications
const TUid KPhoneAppUid     = {0x100058B3};
const TUid KMenuAppUid      = {0x101F4CD2};

// Default browser
const TUint KDefaultBrowser = KWmlcHandler;             // 0x10008D39

// UID of this Menubar
const TUid KEcmtMenubarUid  = { KECMT_MENUBAR_UID };

// uid of the launcher application used in file open -functionality
const TUid KSDKLauncherUid = { 0x20006EF9 };

CEcmtMenubar::CEcmtMenubar(MEcmtMenubarCallbacks* aCallbacks) :
iFlags(0),
iCallbacks(aCallbacks)
{
}

CEcmtMenubar::~CEcmtMenubar()
{
    if (iFlags & EEcmtConnected) iEcmt.Close();
    if (iFlags & EFsConnected) iFs.Close();
    if (iFlags & EWsConnected) iWs.Close();
    if (iFlags & EApaLsConnected) iApaLs.Close();
    delete iDocHandler;
#ifdef PUSH_SUPPORT
    delete iMmsInDirBuf;
#endif // PUSH_SUPPORT
}

// --------------------------------------------------------------------------
// CEcmtMenubar::operator new
// Overloaded new operator
// --------------------------------------------------------------------------
void* CEcmtMenubar::operator new(unsigned int aSize)
{
    return EcmtOsUtils::Alloc(aSize);  //Alloc(aSize);
}

// --------------------------------------------------------------------------
// CEcmtMenubar::operator delete
// overloaded delete operator
// --------------------------------------------------------------------------
void CEcmtMenubar::operator delete(void* aPtr)
{
    EcmtOsUtils::Free(aPtr);
}

// --------------------------------------------------------------------------
// CEcmtMenubar::FreeOnLeave
// Cleanup support
// --------------------------------------------------------------------------
void CEcmtMenubar::FreeOnLeave(TAny* aPtr)
{
  EcmtOsUtils::Free(aPtr);
}

// --------------------------------------------------------------------------
// CEcmtMenubar::LoadFileL
// Loads a file into the emulator, typically (but not necessarily) into the
// browser. Requires connection to the window server.
// --------------------------------------------------------------------------
void CEcmtMenubar::LoadFileL(unsigned int aBrowserUid,
                             const TDesC& aFileName,
                             const TDesC& aFragment)
{
    // Only browser can do fragments
    if (!aFragment.Length())
    {
#ifdef PUSH_SUPPORT
        if (LoadFileAsPushL(aFileName))
        {
            return; // Content has been pushed
        }
#endif // PUSH_SUPPORT

#ifdef DRM_SUPPORT
        if (LoadFileAsDrmL(aFileName))
        {
            return;
        }
#endif // DRM_SUPPORT

        if (IsBrowserFile(aFileName))
        {
            LoadFileToBrowserL(aBrowserUid, aFileName, aFragment);
        }
        else
        {
            _LIT(KJad,"jad");
            HBufC* winJar = NULL;
            TPtrC ext = FileExt(aFileName);
            if (!ext.CompareF(KJad))
            {
                // Special case for JAD files
                winJar = JarFileNameFromJadL(aFileName);
                if (winJar)
                {
                    // Copy the JAR file to the same place where we are
                    // going to copy the JAD file. Deallocate the return
                    // value (if any). The JAR file should exist, otherwise
                    // JarFileNameFromJadL would have returned NULL. Even
                    // if CopyExtFileToEpocTempDirL does return NULL, delete
                    // will ignore it.
                    CleanupStack::PushL(winJar);
                    delete CopyExtFileToEpocTempDirL(*winJar, FsSessionL());
                }
            }

            LoadFileToDocHandlerL(aFileName);
            //LoadFileToAppArcL(aFileName);

            if (winJar)
            {
                CleanupStack::PopAndDestroy(winJar);
            }
        }
    }
}

// --------------------------------------------------------------------------
// CEcmtMenubar::ConvertFileNameToUrlL
// Converts PC filename into local:// URL. The returned buffer has the
// requested amount of extra space.
// --------------------------------------------------------------------------
TText* CEcmtMenubar::ConvertFileNameToLocalUrlL(const TDesC& aFileName,
                                                TInt aExtraSpace /* =0 */)
{
    unsigned short* fileUrl = ConvertFileNameToUrl(aFileName.Ptr(),
                                                   aFileName.Length(),
                                                   LOCAL_FILE_SCHEME,
                                                   aExtraSpace);
    User::LeaveIfNull(fileUrl);
    return fileUrl;
}

// --------------------------------------------------------------------------
// CEcmtMenubar::LoadFileToBrowserL
// Loads a file into the emulator's built-in browser. Requires connection
// to the window server, which is thread-specific.
// --------------------------------------------------------------------------
void CEcmtMenubar::LoadFileToBrowserL(unsigned int aBrowserUid,
                                      const TDesC& aFileName,
                                      const TDesC& aFragment)
{
    TText* fileUrl = ConvertFileNameToLocalUrlL(aFileName, aFragment.Length());
    CleanupStack::PushL(TCleanupItem(FreeOnLeave,fileUrl));
    TInt len = User::StringLength(fileUrl);
    TInt maxLen = len + aFragment.Length() + 1;
    TPtr fileUrlPtr(fileUrl, len, maxLen);
    fileUrlPtr.Append(aFragment);

    LaunchBrowserL(aBrowserUid, fileUrlPtr);
    CleanupStack::Pop(fileUrl);
}

// --------------------------------------------------------------------------
// CEcmtMenubar::LaunchBrowserL
// Loads URL into the built-in browser. This code is based on Forum Nokia
// example. Requires connection to the window server, which is thread-specific.
// --------------------------------------------------------------------------
void CEcmtMenubar::LaunchBrowserL(unsigned int aBrowserUid, const TDesC& aUrl)
{
    HBufC* param = HBufC::NewLC(aUrl.Length()+2);
    TPtr ptr = param->Des();
    ptr.Format(_L("4 %S"),&aUrl);

    // Find the browser among running applications
    TApaTaskList taskList(WsSession());
    TUid browserAppUid(TUid::Uid(aBrowserUid?aBrowserUid:KDefaultBrowser));
    TApaTask task = taskList.FindApp(browserAppUid);
    if (task.Exists())
    {
        // Handle issue with image viewer
        // If image is being viewed need to force switch
        // back to browser...
        task.SwitchOpenFile(aUrl);

        HBufC8* param8 = HBufC8::NewLC(param->Length());
        param8->Des().Append(*param);
        task.SendMessage(TUid::Uid(0), *param8); // Uid is not used
        CleanupStack::PopAndDestroy(param8);
    }
    else
    {
        TThreadId tid;
        ApaLs().StartDocument(*param, browserAppUid, tid);
    }
    CleanupStack::PopAndDestroy(param);
}

// --------------------------------------------------------------------------
// CEcmtMenubar::CheckEmulatorStarted()
// Tests whether the emulator is up and running using the specified session
// to the window server. Note that the sessions are thread-specific. If a
// session is used in context of a thread different from the one that
// created the session, it panics. This function assumes that the session
// is connected.
//
// We assume that if either menu app or the phone app has been started,
// the startup process is complete. Note that depending on the build
// environment, either one or the other gets started automatically, not
// both of them. The other one does not start until the uses presses the
// menu button.
// --------------------------------------------------------------------------
TBool CEcmtMenubar::CheckEmulatorStarted(RWsSession& aWs)
{
    TApaTaskList taskList(aWs);
    return (taskList.FindApp(KMenuAppUid).Exists() ||
            taskList.FindApp(KPhoneAppUid).Exists());
}

// --------------------------------------------------------------------------
// CEcmtMenubar::WsServerConnected()
// Attempts to connect to the window server. If connection has been
// successfully established, returns True. Doesn't wait for window
// server to come up. This function must be invoked on the window thread.
// --------------------------------------------------------------------------
TBool CEcmtMenubar::WsServerConnected()
{
		
    if (!(iFlags & EWsConnected))
    {
        TInt err = iWs.Connect();
        if (err == KErrNone)
        {
            iFlags |= EWsConnected;
        }
    }
    return ((iFlags & EWsConnected) != 0);
}

// --------------------------------------------------------------------------
// CEcmtMenubar::WsSession()
// Returns the session to the window server, does not return until the
// session is connected. Note that this session must only be used in
// context of the SDK Plugin thread.
// --------------------------------------------------------------------------
RWsSession& CEcmtMenubar::WsSession()
{
    while (!WsServerConnected())
    {
        User::After(KConnectRetryDelay);
    }
    return iWs;
}

// --------------------------------------------------------------------------
// CEcmtMenubar::ApaLsServerConnected()
// Attempts to connect to the application architecture server.
// If connection has  been successfully established, returns True.
// Doesn't wait for the server to come up.
// --------------------------------------------------------------------------
TBool CEcmtMenubar::ApaLsServerConnected()
{
    if (!(iFlags & EApaLsConnected))
    {
        TInt err = iApaLs.Connect();
        if (err == KErrNone)
        {
            iFlags |= EApaLsConnected;
        }
    }
    return ((iFlags & EApaLsConnected) != 0);
}

// --------------------------------------------------------------------------
// CEcmtMenubar::ApaLs()
// Returns the session to the application architecture server, does not
// return until the session is connected. Note that this session must only
// be used in context of the SDK Plugin thread.
// --------------------------------------------------------------------------
RApaLsSession& CEcmtMenubar::ApaLs()
{
    while (!ApaLsServerConnected())
    {
        User::After(KConnectRetryDelay);
    }
    return iApaLs;
}

// --------------------------------------------------------------------------
// CEcmtMenubar::FsSessionL()
// Returns the session to the file server. Leaves if the session cannot be
// connected
// --------------------------------------------------------------------------
RFs& CEcmtMenubar::FsSessionL()
{
    if (!(iFlags & EFsConnected))
    {
        TInt err = iFs.Connect();
        User::LeaveIfError(err);
        iFlags |= EFsConnected;
        iFs.ShareProtected();  // Required by DRM
    }
    return iFs;
}

// --------------------------------------------------------------------------
// Implements MEcmtServerObserver
// --------------------------------------------------------------------------
void CEcmtMenubar::HandleEcmtServerNotification(const TDesC& aMsg, TInt)
{
    __DEBUG_ONLY(RDebug::Print(_L("EcmtMenubar: %S"),&aMsg));
    if (iCallbacks)
    {
        const unsigned short* params = NULL;
        HBufC* paramBuf = NULL;

        if (!iHandledListeningNotification ||
            aMsg.Left(KEcmtServerListening().Length()) ==
            KEcmtServerListening())
        {
            // Prepare NULL terminated parameter string
            TInt lastSpace = aMsg.LocateReverse(' ');
            if (lastSpace > 0)
            {
                TPtrC right = aMsg.Right(aMsg.Length() - lastSpace - 1);
                paramBuf = HBufC::New(right.Length() + 1);
                if (paramBuf)
                {
                    TPtr paramPtr = paramBuf->Des();
                    paramPtr.Copy(right);
                    params = paramPtr.PtrZ();
                }
            }
        }

        if (aMsg.Left(KEcmtServerListening().Length()) ==
            KEcmtServerListening())
        {
            iHandledListeningNotification = ETrue;
            iCallbacks->EcmtServerListening(params);
        }
        else if (aMsg.Left(KEcmtServerConnected().Length()) ==
                 KEcmtServerConnected())
        {
            if (!iHandledListeningNotification)
            {
                iHandledListeningNotification = ETrue;
                iCallbacks->EcmtServerListening(params);
            }
            iCallbacks->EcmtServerConnected();
        }
        else if (aMsg.Left(KEcmtServerDisconnected().Length()) ==
                 KEcmtServerDisconnected())
        {
            if (!iHandledListeningNotification)
            {
                iHandledListeningNotification = ETrue;
                iCallbacks->EcmtServerListening(params);
                iCallbacks->EcmtServerConnected();
            }
            iCallbacks->EcmtServerDisconnected();
        }

        delete paramBuf;
    }
}

// --------------------------------------------------------------------------
// Sends a Ecmt message with the specified UID to the Java side.
// --------------------------------------------------------------------------
bool CEcmtMenubar::SendEcmtMessage(unsigned int aUid,
                                   const void* aMessageData,
                                   int aMessageSize)
{
    if (IsCoreUp())
    {
        TPtrC8 ptr((TUint8*)aMessageData,aMessageSize);
        iEcmt.Send(aUid, ptr);
        return true;
    }
    else
    {
        return false;
    }
}

// --------------------------------------------------------------------------
// Loads a URL into the browser. File URLs are handled differently
// from other URLs, they get transformed into local: URLs, and file
// name gets URL-encoded. The fragment part of the URL is preserved.
// Other URLs are passed to the browser as is.
// --------------------------------------------------------------------------
bool CEcmtMenubar::SmartLoad(unsigned int aBrowserUid,
                             const unsigned short* aUrl)
{
    TPtrC url(aUrl);
    TInt err;
    const TDesC* browseUrl = &url;
    if (url.FindF(KFileScheme) == 0)
    {
        // File URLs get special treatment
        HBufC* decodedFileName = NULL;
        TInt filePartStart = KFileScheme().Length();
        TInt colon = url.Right(url.Length() - filePartStart).LocateF(':');
        if (colon > 0) filePartStart += colon - 1;

        // Locate postfield or anchors in the URL
        TInt filePartEnd = url.Length();
        TInt pos = url.LocateReverse('#');
        if (pos > filePartStart) filePartEnd = pos;
        pos = url.LocateReverse('?');
        if (pos > filePartStart && pos < filePartEnd) filePartEnd = pos;

        // The file name may (and should be) URL encoded
        TPtrC filePart(url.Mid(filePartStart,filePartEnd-filePartStart));
        if (filePart.Locate('%') >= 0)
        {
            decodedFileName = EscapeUtils::EscapeDecodeL(filePart);
            filePart.Set(*decodedFileName);
        }

        TPtrC fragment(url.Right(url.Length()-filePartEnd));
        TRAP(err,LoadFileL(aBrowserUid, filePart, fragment));
        delete decodedFileName;
        ErrorMsg(err);
    }
    else
    {
        // Send URL "as is" relying on the browser to provide a meaningful
        // error message if something is wrong with the URL.
        TRAP(err,LaunchBrowserL(aBrowserUid, *browseUrl));
    }
    return (err == KErrNone);
}

// --------------------------------------------------------------------------
// CEcmtMenubar::LoadFile
// Loads a file into the emulator, typically (but not necessarily)
// into the built-in browser.
// --------------------------------------------------------------------------
bool CEcmtMenubar::LoadFile(unsigned int aBrowserUid,
                            const unsigned short* aFileName,
                            const unsigned short* aFragment)
{
    TPtrC file(aFileName);
    TPtrC frag(aFragment ? aFragment : L"");
    TRAPD(err, LoadFileL(aBrowserUid, file, frag));
    return ErrorMsg(err);
}

// --------------------------------------------------------------------------
// CEcmtMenubar::LaunchBrowser()
// Launches the browser with the specified URL.
// --------------------------------------------------------------------------
bool CEcmtMenubar::LaunchBrowser(unsigned int aUid,const unsigned short* aUrl)
{
    TRAPD(err, LaunchBrowserL(aUid, TPtrC(aUrl)));
    return (err == KErrNone);
}

// --------------------------------------------------------------------------
//
// Runs a Symbian executable.
// --------------------------------------------------------------------------
bool CEcmtMenubar::RunExe(const unsigned short* aExe,
                          const unsigned short** aArgs, int aArgCount,
                          unsigned long long* aProcessId)
{
    HBufC* argBuf = NULL;
    const TDesC* args = &(KNullDesC());
    TInt i, totalLen = 0;
    for (i=0; i<aArgCount; i++) {
        totalLen += User::StringLength(aArgs[i]);
    }
    if (totalLen > 0) {
        argBuf = HBufC::New(totalLen + aArgCount - 1);
        if (argBuf) {
            TPtr des(argBuf->Des());
            for (i=0; i<aArgCount; i++) {
                if (des.Length() > 0) des.Append(' ');
                des.Append(TPtrC(aArgs[i]));
            }
            args = argBuf;
        } else {
            return false;
        }
    }

    // Start the process
    TPtrC exe(aExe);
    RProcess process;
    TInt err = process.Create(exe, *args);
    delete argBuf;
    if (err == KErrNone)
    {
        if (aProcessId) *aProcessId = process.Id().Id();
        process.Resume();
        process.Close();
        return true;
    }
    else
    {
        return false;
    }
}

// --------------------------------------------------------------------------
// CEcmtMenubar::IsEmulatorStarted()
// Tests whether the emulator is up and running. Must be invoked on the
// window thread. If invoked during the startup process, returns EFalse.
// --------------------------------------------------------------------------
bool CEcmtMenubar::IsEmulatorStarted()
{
    return WsServerConnected() && CheckEmulatorStarted(iWs);
}

// --------------------------------------------------------------------------
// CEcmtMenubar::IsCoreUp()
// Tests whether Ecmt core (server) is up.
// --------------------------------------------------------------------------
bool CEcmtMenubar::IsCoreUp()
{
    if (CEcmtSdkPlugin::IsStarted())
    {
        if (!(iFlags & EEcmtConnected))
        {
            TInt err = iEcmt.Connect();
            if (err == KErrNone)
            {
                iFlags |= EEcmtConnected;
                TRAP(err,iEcmt.SetServerObserverL(KEcmtMenubarUid, this));
            }
        }
        return true;
    }
    else
    {
        return false;
    }
}

// --------------------------------------------------------------------------
// CEcmtMenubar::Release()
// Implements self-destruction mechanism.
// --------------------------------------------------------------------------
void CEcmtMenubar::Release()
{
    delete this;
}

// --------------------------------------------------------------------------
// CEcmtMenubar::LoadFileToDocHandler
// --------------------------------------------------------------------------
void CEcmtMenubar::LoadFileToDocHandlerL(const TDesC& aFileName)
{
    // menubar does not anymore launch app via DocumentHandler directly, but
    // instead it starts up a help-application (SDK Launcher) to do that.
    // Why?
    // 1) We need the actual target application to be run as embedded, because
    //    that is the only way to pass AIW generic parameters to it.
    // 2) We want to have application icon visible in active application list,
    //    so that we can switch back to it, if it ends in background. However,
    //    if menubar starts embedded application directly, its icon is not
    //    shown in the list.
    // Thus, menubar starts standalone launcher-app, which launches actual
    // target app as embedded. SDK launcher will exit, when the embedded
    // application exits. And SDK Launcher -icon can be seen in active app list.

    HBufC* tmpPath = CopyExtFileToEpocTempDirLC(aFileName, FsSessionL ());
    User::LeaveIfNull(tmpPath);

    TApaTaskList taskList(WsSession());
    TApaTask task = taskList.FindApp(KSDKLauncherUid);
    if (task.Exists())
    {
        RDebug::Printf("Ecmt menubar: SDK Launcher: SwitchOpenFile");
        TInt err = task.SwitchOpenFile(*tmpPath);
        if (err)
        {
            RDebug::Printf("Ecmt menubar: switching SDK Launcher failed: %i", err);
        }
    }
    else
    {
        TThreadId tid;
        RApaLsSession& apaLs = ApaLs();
        RDebug::Printf("Ecmt menubar: SDK Launcher: StartDocument");
        TInt err = apaLs.StartDocument(*tmpPath, KSDKLauncherUid, tid);
        if (err)
        {
            RDebug::Printf("Ecmt menubar: starting SDK Launcher failed: %i", err);
        }
    }
    CleanupStack::PopAndDestroy(tmpPath);
}

// --------------------------------------------------------------------------
// CEcmtMenubar::LoadFileToAppArc
// --------------------------------------------------------------------------
void CEcmtMenubar::LoadFileToAppArcL(const TDesC& aFileName)
{
    TThreadId threadId;
    HBufC* tmpPath = CopyExtFileToEpocTempDirLC(aFileName, FsSessionL ());
    User::LeaveIfNull(tmpPath);

    TUid appUid;
    TDataType dataType;
    RApaLsSession& apaLs = ApaLs();
    apaLs.AppForDocument(*tmpPath,appUid,dataType);

    // Find the handler app among running applications
    TApaTaskList taskList(WsSession());
    TApaTask task = taskList.FindApp(appUid);
    if (task.Exists())
    {
        // Works with Image Viewer but not much else...
        task.SwitchOpenFile(*tmpPath);
    }
    else
    {
        RFile f;
        User::LeaveIfError(f.Open(FsSessionL(), *tmpPath, EFileRead|EFileWrite));
        TThreadId tid;
        apaLs.StartDocument(f, appUid, tid);
        f.Close();
    }
    CleanupStack::PopAndDestroy(tmpPath);
}

// --------------------------------------------------------------------------
// CEcmtMenubar::CanOpenInBrowser
// --------------------------------------------------------------------------
TBool CEcmtMenubar::IsBrowserFile(const TDesC& aFileName)
{
    TPtrC fileExt = FileExt(aFileName);
    for (TInt i=0; i<COUNT(BrowserTypes); i++)
    {
        if (fileExt.CompareF(TPtrC(BrowserTypes[i].ext)) == 0)
        {
            return ETrue;
        }
    }
    return EFalse;
}

// --------------------------------------------------------------------------
// CEcmtMenubar::ErrorMsg
// Shows an error message in a subtle and and non-intrusive way :)
// Returns false if aError equals KErrNone, true otherwise
// --------------------------------------------------------------------------
bool CEcmtMenubar::ErrorMsg(TInt aError)
{
    if (aError != KErrNone)
    {
        CEikonEnv* env = CEikonEnv::Static();
        if (env)
        {
            _LIT(KFormat,"Error %d");
            TBuf<32> buf;
            buf.Format(KFormat, aError);
            env->InfoMsg(buf);
        }
        return true;
    }
    return false;
}

/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

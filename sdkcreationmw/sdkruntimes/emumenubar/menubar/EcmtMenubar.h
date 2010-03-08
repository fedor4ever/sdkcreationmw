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


#ifndef ECMTMENUBAR_H
#define ECMTMENUBAR_H

//  INCLUDES

#include <e32std.h>
#include <w32std.h>
#include <apgcli.h>
#include "MEcmtMenubar.h"
#include "EcmtClient.h"
#include "EcmtServerObserver.h"
#include "EcmtMenubarFileUtils.h"

// CLASS DECLARATION

class CWatcherLog;
class CPushMessage;
class RApaLsSession;
class MEcmtMessaging;
class MEcmtMenubarCallbacks;
class CDocumentHandler;


/**
*  Class that implements Symbian specific functionality of Emulator menubar.
*  @ingroup Menubar
*/

class CEcmtMenubar : 
    public EcmtMenubarFileUtils, 
    public MEcmtMenubar,
    public MEcmtServerObserver
{
private:

    enum _Flags {
        EEcmtConnected  = 0x0001,
        EWsConnected    = 0x0002,
        EFsConnected    = 0x0004,
        EApaLsConnected = 0x0008
    };

    TInt iFlags;
    REcmt iEcmt;
    
    RFs iFs;
    RApaLsSession iApaLs;
    MEcmtMenubarCallbacks* iCallbacks;
    CDocumentHandler* iDocHandler;
    TBool iHandledListeningNotification;

#ifdef PUSH_SUPPORT
    // MMS/Push support
    class CWapPushSupport;
    class CSimulateMms;
    friend class CSimulateMms;

    CWapPushSupport* iWapPushSupport;
    CWatcherLog* iWatcherLog;
    HBufC* iMmsInDirBuf;
    const TDesC* iMmsInDir;
#endif // PUSH_SUPPORT
public:
    CEcmtMenubar(MEcmtMenubarCallbacks* aCallbacks);
    ~CEcmtMenubar();

    /**
     * Overload operators new and delete to ensure that this class is safe
     * to allocate and deallocate in any context.
     */
    void* operator new(unsigned int aSize);
    void operator delete(void* aPtr);

private:
#ifdef PUSH_SUPPORT

    // MMS/Push support
    class CWapPushSupport;
    class CSimulateWapPush;
    class CSimulateMms;

    /**
     * Detects whether the file is an MMS or a Wap Push message and
     * attempts to push it to the phone. Returns ETrue on success,
     * EFalse on failure or if the file does not seem to contain
     * push content. Leaves in case of a catastrophic failure.
     */
    TBool LoadFileAsPushL(const TDesC& aFileName);

#endif // PUSH_SUPPORT

#ifdef DRM_SUPPORT

    /**
     * Detects whether the file is a DRM file and attempts to handle it.
     * Returns ETrue on success, EFalse on failure or if the file does 
     * not seem to be a DRM file. Leaves in case of a catastrophic failure.
     */
    TBool LoadFileAsDrmL(const TDesC& aFileName);

#endif // PUSH_SUPPORT

public:

    // MEcmtMenubar

    /**
     * Sends a Ecmt message with the specified UID to the Java side.
     */
    virtual bool SendEcmtMessage(unsigned int aUid, 
                                 const void* aMessageData,
                                 int aMessageSize);
    /**
     * Loads a URL into the browser. File URLs are handled differently
     * from other URLs, they get transformed into local: URLs, and file
     * name gets URL-encoded. The fragment part of the URL is preserved.
     * Other URLs are passed to the browser as is.
     *
     * If browserUid parameter is zero, then the default browser is used.
     */
    virtual bool SmartLoad(unsigned int aBrowserUid,
                           const unsigned short* aUrl);

    /**
     * Loads a file into the emulator, typically (but not necessarily) 
     * into the built-in browser.
     *
     * If browserUid parameter is zero, then the default browser is used.
     */
    virtual bool LoadFile(unsigned int aBrowserUid,
                          const unsigned short* aFileName, 
                          const unsigned short* aFragment);

    /**
     * Launches the browser with the specified URL. 
     *
     * If browserUid parameter is zero, then the default browser is used.
     */
    virtual bool LaunchBrowser(unsigned int aBrowserUid,
                               const unsigned short* aUrl);

    /**
     * Runs a Symbian executable.
     */
    virtual bool RunExe(const unsigned short* aExe,
                        const unsigned short** aArgs, int aArgCount,
                        unsigned long long* aProcessId);

    /**
     * Tests whether the emulator is up and running. If invoked during the 
     * startup process, returns EFalse.
     */
    virtual bool IsEmulatorStarted();

    /**
     * Tests whether Ecmt core (server) is up. 
     */
    virtual bool IsCoreUp();

    /**
     * Self-destructs
     */
    virtual void Release();

    // MEcmtServerObserver
    
    virtual void HandleEcmtServerNotification(const TDesC& aMsg, TInt aError);

private:

    /**
     * Tests whether the emulator is up and running, using the specified
     * session to the window server. Note that the Symbian sessions are
     * thread-specific. If a session is used in context of q thread 
     * different from the one that created the session, it panics.
     * This function assumes that the session is connected.
     * 
     * We assume that if either menu app or the phone app has been
     * started, the startup process is complete. Note that depending
     * on the build environment, either one or the other gets started
     * automatically, not both of them. The other one does not start
     * until the uses presses the menu button.
     */
    static TBool CheckEmulatorStarted(RWsSession& aWs);

    /**
     * Attempts to connect to the window server. If connection has 
     * been successfully established, returns True. Doesn't wait
     * for window server to come up. 
     */
    TBool WsServerConnected();

    /**
     * Attempts to connect to the application architecture server.
     * If connection has  been successfully established, returns True.
     * Doesn't wait for the server to come up. 
     */
    TBool ApaLsServerConnected();

    /**
     * Returns the session to the window server. Does not return until 
     * the session is connected. 
     */
    RWsSession& WsSession();

    /** 
     * Returns the session to the application architecture server.
     * Does not return until  the session is connected. 
     */
    RApaLsSession& ApaLs();

    /**
     * Returns the session to the file server. Leaves if the session 
     * cannot be connected
     */
    RFs& FsSessionL();

    /**
     * Loads a file into the emulator, typically (but not necessarily) 
     * into the browser.
     */
    void LoadFileL(unsigned int aBrowserUid,
                   const TDesC& aFileName,
                   const TDesC& aFragment);

    /**
     * Loads a file into the browser. If aBrowserUid is zero, uses the
     * default browser.
     */
    void LoadFileToBrowserL(unsigned int aBrowserUid,
                            const TDesC& aFileName,
                            const TDesC& aFragment);

    /**
     * Loads a file into S60 document handler.
     */
    void LoadFileToDocHandlerL(const TDesC& aFileName);


    /**
     * Loads a file using AppArc session
     */
    void LoadFileToAppArcL(const TDesC& aFileName);

    /**
     * Launches the browser with the specified URL. Requires connection
     * to the window server, which is thread-specific.
     */
    void LaunchBrowserL(unsigned int aBrowserUid, const TDesC& aUrl);

    /**
     * Checks if the file extension suggests that the file can be opened
     * in the browser.
     */
    static TBool IsBrowserFile(const TDesC& aFileName);

    /** 
     * Shows an error message in a subtle and and non-intrusive way :)
     * Returns false if aError equals KErrNone, true otherwise
     */
    static bool ErrorMsg(TInt aError);

    /**
     * Converts PC filename into local:  URL. The returned buffer has the
     * requested amount of extra space. Leave on error.
     */
    static TText* ConvertFileNameToLocalUrlL(const TDesC& aFile, 
                                             TInt aExtra=0);

#ifdef PUSH_SUPPORT

    static HBufC* ReadMmsInDir(RFs& aFs);
    const TDesC& MmmInDirL();
    void SimulateMmsL(const TDesC8& aMessageBody);
    CWatcherLog& WatcherLogL();
    void SimulateWapPushL(CPushMessage* aPushMessage);

#endif // PUSH_SUPPORT

private:

    /**
     * Cleanup support
     */
    static void FreeOnLeave(TAny* aPtr);
};
    
#endif	// ECMTMENUBAR_H

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

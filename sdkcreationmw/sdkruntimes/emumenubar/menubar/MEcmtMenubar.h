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
* Description:  Windows specific class and utilities
 *
*/


#ifndef _MECMTMENUBAR_H_
#define _MECMTMENUBAR_H_

/**
 * Interface between the UI-side menubar code and the Symbian side
 * functionality.
 */
class MEcmtMenubar
{
public:

    /**
     * Sends a Ecmt message with the specified UID to the Java side.
     */
    virtual bool SendEcmtMessage(unsigned int aUid, 
                                 const void* aMessageData,
                                 int aMessageSize) = 0;

    /**
     * Loads a URL into the browser. File URLs are handled differently
     * from other URLs, they get transformed into local: URLs, and file
     * name gets URL-encoded. The fragment part of the URL is preserved.
     * Other URLs are passed to the browser as is.
     *
     * If browserUid parameter is zero, then the default browser is used.
     */
    virtual bool SmartLoad(unsigned int aBrowserUid,
                           const unsigned short* aUrl) = 0;

    /**
     * Loads a file into the emulator, typically (but not necessarily) 
     * into the built-in browser.
     *
     * If browserUid parameter is zero, then the default browser is used.
     */
    virtual bool LoadFile(unsigned int aBrowserUid,
                          const unsigned short* aFileName,
                          const unsigned short* aFragment = 0) = 0;

    /**
     * Launches the browser with the specified URL. The URL is passed into
     * the browser without any modifications.
     *
     * If browserUid parameter is zero, then the default browser is used.
     */
    virtual bool LaunchBrowser(unsigned int aBrowserUid,
                               const unsigned short* aUrl) = 0;

    /**
     * Runs a Symbian executable.
     */
    virtual bool RunExe(const unsigned short* aExe,
                        const unsigned short** aArgs, int aArgCount,
                        unsigned long long* aProcessId) = 0;

    /**
     * Tests whether the emulator is up and running. If invoked during the 
     * startup process, returns EFalse.
     */
    virtual bool IsEmulatorStarted() = 0;

    /**
     * Tests whether Ecmt core (server) is up. 
     */
    virtual bool IsCoreUp() = 0;

    /**
     * Self-destructs
     */
    virtual void Release() = 0;
};
    
#endif	// _MECMTMENUBAR_H_

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

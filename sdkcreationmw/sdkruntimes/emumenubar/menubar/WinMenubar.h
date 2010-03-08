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


#ifndef _WIN_MENUBAR_H_
#define _WIN_MENUBAR_H_

//  INCLUDES

#include "EcmtWin32.h"
#include "EcmtResource.h"
#include "EcmtMenubarUtils.h"
#include "MEcmtMenubarCallbacks.h"

#ifdef SDK_REGISTRATION
#include "reglib.h"
extern RegProduct * gRegProductContext;
#endif // SDK_REGISTRATION

#ifdef NMIT_INTEGRATION
#include "nmitlib.h"
#endif // NMIT_INTEGRATION

// MACROS

// The file containing the product name and the text for the about box
#define PRODUCTNAME_FILE TEXT("productname.txt")
#define ABOUTBOX_RTF_FILE TEXT("aboutbox.rtf")
#define SDKINFO_FILE TEXT("sdk_info.properties")

// Keys in the sdk_info.properties file
#define FEEDBACK_URL_KEY_1   "sdk.feedback.url"
#define FEEDBACK_MENU_KEY_1  "sdk.feedback.menu"
#define FEEDBACK_URL_KEY_2   "sdk.feedback2.url"
#define FEEDBACK_MENU_KEY_2  "sdk.feedback2.menu"
#define FEEDBACK_URL_KEY_3   "sdk.feedback3.url"
#define FEEDBACK_MENU_KEY_3  "sdk.feedback3.menu"
#define FEEDBACK_URL_KEY_4   "sdk.feedback4.url"
#define FEEDBACK_MENU_KEY_4  "sdk.feedback4.menu"
#define FEEDBACK_URL_KEY_5   "sdk.feedback5.url"
#define FEEDBACK_MENU_KEY_5  "sdk.feedback5.menu"

#define JAVAVERIFIED_URL_KEY    "sdk.javaverified.url"
#define JAVAVERIFIED_MENU_KEY   "sdk.javaverified.menu"
// CONSTANTS

/// Maximum length of text to be displayed in Wait Window.
const int KMaxWaitText = 32;
// Maximum count for EpdtCommands. Basically this means the number of tools
// under Tools menu. Meanwhile EcmtManager is launching user can select all 
// items from tools menu. These selections are stored into an array which 
// maximum length is this constant.
const int KMaxEpdtCommandCount = 3;

// FORWARD DECLARATIONS

class MEcmtMenubar;

// CLASS DECLARATION

/**
*  @defgroup EcmtMenubar Menubar
*  The menubar component provides the Windows standard menubar functionality
*  to the emulator.
*/

/**
*  Class that implements Windows specific functionality of Emulator menubar.
*  @ingroup Menubar
*/

class CWinMenubar : public EcmtMenubarUtils, public MEcmtMenubarCallbacks
{
public:

    enum TMessageStatus 
    {
        EMessageHandledByMenubar,   // Don't pass message to Symbian
        EMessagePassToSymbian,      // Pass message to Symbian
    };

    ~CWinMenubar();

    /**
     * Creates an instance of the menubar associated with the specified
     * window.
     *
     * @param hWnd Window handle of emulator.
     */
    static CWinMenubar* New( HWND hWnd );

    /**
     * Returns the registration product id
     */
    const char * GetProductId() const;

    /**
     * Handles a window messahe
     */		
    TMessageStatus HandleMessage(UINT message, WPARAM wPar, LPARAM lPar);

    /**
     * Overloaded operators new and delete
     */
    void* operator new (unsigned int aSize);
    void operator delete (void* aPtr);

    /** Checks if various components (emulator itself, ECMT server in
     * particular) are started.
     */
    BOOL CheckIfStarted(BOOL aCheckEcmt  = TRUE);

public:
    // MEcmtMenubarCallbacks

    virtual void EcmtServerListening(const unsigned short* params);
    virtual void EcmtServerConnected();
    virtual void EcmtServerDisconnected();

private:

    /**
     * Reads a text file from the epoc32/data directory. Returns NULL if 
     * file is not found. The returned must be deallocated by the caller
     * using the free() C-runtime function.
     */
    static char* ReadTextFile(LPCTSTR aFileName);

    /**
     * Runs the startup scripts runonce.cmd and autorun.cmd from 
     * \\epoc32\\tools\\ecmt. This proc is called once early in epoc.exe
     * lauch process. 
     */
    static void RunStartups();

    /**
     * Sets the one-way flag iEmulatorStarted to ETrue.
     */
    void SetEmulatorStarted();

    /**
     * Sets the one-way flag iCoreUp to ETrue.
     */
    void SetCoreUp();

    /**
     * In launcher mode show a window to the user so that it is clear
     * that we are working. Launching cores and the view takes quite
     * some time so it is important to provide a visual feedback (and 
     * the possibility to abort) 
     * @return Handle of wait window 
     */
    void ShowWaitWindow();

    /**
     * Launches epdt core if it is not already running and then the view 
     * given as parameter.
     * @param aCommand View to launch, either the constant 
     * IDM_PREFERENCES, IDM_UTILITIES or IDM_DIAGNOSTICS.
     */
    void LaunchView( WPARAM aCommand );

    /**
     * Launches the Ecmt Manager application.
     */
    void LaunchEcmtMan();

    /**
     * Runs the .cmd specified with cmdFile (if it exists) and deletes 
     * it if desired.
     * @param cmdFile Full path to .cmd file to run.
     * @param del Delete file after running it if TRUE.
     */
    static void RunCmd( TCHAR* cmdFile, BOOL del );

    /**
     * Pops up file open dialog and allow the user to select a file
     */
    TCHAR* SelectFile(HWND hWndOwner);

    /**
     * Returns handle to the EcmtMenubar dll
     */
    static HMODULE ModuleHandle();

    /**
     * Dialog proc for the about box
     */
    static BOOL CALLBACK AboutProc(HWND hDlg, UINT message, 
                                   WPARAM wParam, LPARAM lParam);

    /**
     * Dialog proc for the Load dialog
     */
    static BOOL CALLBACK LoadDialogProc(HWND hDlg, UINT message, 
                                        WPARAM wParam, LPARAM lParam);

private:

    /**
     * C++ constructor
     */
    CWinMenubar(HWND hWnd);

    /**
     * Second stage constructor
     */
    BOOL Initialize();

    /**
     * In launcher mode, the WaitWindow is shown instead of real Emulator
     * window.
     */
    TMessageStatus WM_WINDOWPOSCHANGING_Handler(WPARAM wPar, LPARAM lPar);

    /**
     * In launcher mode, the focus is forwarded to the Wait Window 
     * (the only one that is visible).
     */
    TMessageStatus WM_SETFOCUS_Handler( WPARAM wParam, LPARAM lParam);

    /**
     * Enable tools->preferences, tools->utilities and tools->diagnostics
     * only after Ecmt core is running.
     */
    TMessageStatus WM_INITMENUPOPUP_Handler( WPARAM wParam, LPARAM lParam);

    /**
     * Handles timer messages in the launcher mode 
     */
    TMessageStatus WM_TIMER_Handler( WPARAM wParam, LPARAM lParam);

    /**
     * Handle communications related notification from Ecmt Core.
     */
    TMessageStatus WM_ECMT_LISTENING_Handler( WPARAM wParam, LPARAM lParam);

    /**
     * Handle communications related notification from Ecmt Core.
     */
    TMessageStatus WM_ECMT_CONNECTED_Handler( WPARAM wParam, LPARAM lParam);

    /**
     * Handle communications related notification from Ecmt Core.
     */
    TMessageStatus WM_ECMT_DISCONNECTED_Handler( WPARAM wParam, LPARAM lParam);

    /**
     * Shutdown epdt core when emulator window closes.
     */
    TMessageStatus WM_DESTROY_Handler( WPARAM wParam, LPARAM lParam);

    /**
     * Change cursor to hourglass while epdt is starting up.
     */
    TMessageStatus WM_MOUSEMOVE_Handler( WPARAM wParam, LPARAM lParam);

    /**
     * First level handler for menu commands.
     */
    TMessageStatus WM_COMMAND_Handler( WPARAM wParam, LPARAM lParam);
	
    /**
     * Handles File->Load URL...
     */
    TMessageStatus IDM_LOAD_Handler( WPARAM wParam, LPARAM lParam);

    /**
     * Handles File->Open...
     */
    TMessageStatus IDM_OPEN_Handler( WPARAM wParam, LPARAM lParam);

    /**
     * Handles File->Exit
     */
    TMessageStatus IDM_EXIT_Handler( WPARAM wParam, LPARAM lParam);

    /**
     * Handles Tools->Preferences
     */
    TMessageStatus IDM_PREFERENCES_Handler( WPARAM wParam, LPARAM lParam);

    /**
     * Handles Tools->Utilities
     */
    TMessageStatus IDM_UTILITIES_Handler( WPARAM wParam, LPARAM lParam);

    /**
     * Handles Tools->Diagnostics
     */
    TMessageStatus IDM_DIAGNOSTICS_Handler( WPARAM wParam, LPARAM lParam);

    /**
     * Handles Help->User's Guide
     */
    TMessageStatus IDM_USERS_GUIDE_Handler( WPARAM wParam, LPARAM lParam);

    /**
     * Handles Help->Release Notes
     */
    TMessageStatus IDM_RELEASE_NOTES_Handler( WPARAM wParam, LPARAM lPar);

    /**
     * Handles Help->About
     */
    TMessageStatus IDM_ABOUT_Handler( WPARAM wParam, LPARAM lParam);

    /**
     * Handles Help->Startupmode Notification
     */
    TMessageStatus IDM_STARTUPMODE_Handler( WPARAM wParam, LPARAM lParam);

    /**
     * Sends cmd to EPDT via ECMT :)
     */
    void SendEpdtCommand( LPWSTR lpCommand );

#ifdef NMIT_INTEGRATION

    /**
     * Loads the specified URL into the appropriate application,
     * typically the browser.
     */        
    static NmitBool NMIT_API NmitLoad(void* aThis, NmitStr aUrl, 
                                      unsigned int aBrowserUid);

    /**
     * Runs an executable in the emulator.
     */
    static NmitBool NMIT_API NmitRun(void* aThis, NmitStr aExe, 
                                     NmitStr* aArgs, int aArgCount,
                                     NmitProcessId* aProcessId);

    /**
     * Pushes the emulator window to front.
     */
    static void NMIT_API NmitToFront(void* aThis);

    /**
     * Exits the emulator.
     */
    static void NMIT_API NmitExit(void* aThis);

#endif // NMIT_INTEGRATION

private:

    /// Handle to main window.
    HWND iHwnd;
    /// Handle to window menu.
    HMENU iMenu;
    /// Handle to the accelerator table
    HACCEL iAccTable;
    /// Handle to Wait Window in launcher mode.
    HWND iWaitWindow;
    /// Connected to epdt core via TCP if TRUE.
    BOOL iEcmtConnected;
    /// Handle of the EcmtMan process
    HANDLE iEcmtManProcess;
    /// True if this is a MIDP SDK
    BOOL iMidpSDK;
    /// Pending command from tools->preferences.
    WPARAM iEpdtCommand[KMaxEpdtCommandCount];
    /// Ecmt Core is running if TRUE.
    BOOL iCoreUp;
    LPTSTR iParamString;
    /// The text to be displayed in Wait Window, i.e. the window that
    /// is displayed in launcher mode while the core is starting and 
    /// connecting to epdt.
    TCHAR iWaitWindowText[KMaxWaitText];
    /// This turns into True when the system has been initialized
    BOOL iEmulatorStarted;
    // Interface to the Symbian side of the menubar
    MEcmtMenubar* iEcmtMenubar;
    // UID of the default browser
    UINT iDefaultBrowser;
    // Handle to the RichEdit control dll
    HMODULE iRichEdit;

    /// These are used by the file open dialog
    char  iOpenDir[MAX_PATH];
    DWORD iOpenFilterIndex;
        
#ifdef NMIT_INTEGRATION
    /// NMIT integration
    NmitInterface* iNmit;
#endif // NMIT_INTEGRATION

    // Menu URL links (feedback)
    char * iMenuURL[ECMTMENUBAR_LINK_COUNT];
};

    
#endif	// _WIN_MENUBAR_H_

/**
 * Local Variables:
 * mode: c++
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

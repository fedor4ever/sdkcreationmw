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
* Description:  Windows specific part of the  menubar. Does not include
 *                any Symbian header files.
 *
*/


static char rcsid[] = "$Id: WinMenubar.cpp,v 1.38 2006/04/20 21:17:03 slava Exp $";

// INCLUDE FILES

#include "WinMenubar.h"
#include "MEcmtMenubar.h"
#include "EcmtMenubarProxy.h"
#include "EcmtMenubarConstants.h"
#include "SdkProductInfo.h"

// CONSTANTS

WCHAR KShowPrefs[] = L"show_preferences";
WCHAR KShowUtilities[] = L"show_utilities";
WCHAR KShowDiagnostics[] = L"show_diagnostics";
WCHAR KHidePrefs[] = L"hide_preferences";
WCHAR KExitCore[]  = L"exit";

CHAR  KJarExt[] = ".jar";
CHAR  KJadExt[] = ".jad";

#define LAUNCH_TIMER 9384

// MACROS
#define MENUBAR_API __cdecl
#define MESSAGE_HANDLER(a) case a: { return a##_Handler(wParam,lParam); }

// Window messages
#define WM_ECMT_LISTENING         (WM_APP+0x100) // LPARAM -> params string
#define WM_ECMT_CONNECTED         (WM_APP+0x101)
#define WM_ECMT_DISCONNECTED      (WM_APP+0x102)

#ifdef SDK_REGISTRATION
#  define WM_REGISTRATION_CHECK   (WM_APP+0x103)
#endif // SDK_REGISTRATION


// =========================== STATIC VARIABLES  ============================

// Handle the the DLL. We need it to access the resources
static HMODULE gModuleHandle = NULL;

// Product registration specific data

#ifdef SDK_REGISTRATION
//
// Wonder why the following two variables are not members of the CWinMenubar
// class? Primarily, for two reasons:
//
// 1. CWinMenubar is associated with a window. If there is more than 
//    one window, each of them will have to have its own instance of
//    CWinMenubar associated with it, or its equivalent. It's just so
//    hapens that we currently have only one window, and therefore we
//    don't need more than one instance of this class. That may change.
//    The variables below, on the other hand, are truely global, no matter
//    how many windows we have. Of course these variables could be static
//    members of the class, but... here comes the second reason.
//
// 2. The header file currently has no dependency on the product registration
//    functionality and I would like to keep it that way. All the details of
//    the registration process are here, in this file. All changes are
//    well contained and never affect anything outside of this file.
//
RegProduct * gRegProductContext = NULL;
static int gRegInitCount = 0;
#endif  // SDK_REGISTRATION

// ============================ GLOBAL FUNCTIONS ============================

// --------------------------------------------------------------------------
// MenubarInit()
// Export #1
// Initialization routine, called right after the DLL has been loaded.
// --------------------------------------------------------------------------
extern "C" EXPORT_C void MENUBAR_API MenubarInit(HMODULE hModule)
{
    gModuleHandle = hModule;
    
}

// --------------------------------------------------------------------------
// MenubarCWinMenubar()
// Export #2
// Per-window initialization routine, called right after the window has 
// been created but before it has been made visible. Returns the menubar 
// context to be passed to MenubarWndProc
// --------------------------------------------------------------------------
extern "C" EXPORT_C void * MENUBAR_API MenubarAttach(HWND hWnd)
{ 	
	//Disclaimer display for internal release
	#ifdef INTERNAL_RELEASE
		TCHAR disclaimer[500],cmd[50];
		_tcscpy( cmd,TEXT("perl.exe"));	 
		_tcscpy( disclaimer, CWinMenubar::Epocroot() );
		_tcscat( disclaimer,TEXT("\\epoc32\\RELEASE\\WINSCW\\udeb\\Z\\data\\disclaimer\\disclaimer.pl" ));
		 ShellExecute( 0,0,cmd,disclaimer,0, SW_SHOWNORMAL );
	#endif	
	
    return CWinMenubar::New(hWnd);
}

// --------------------------------------------------------------------------
// MenubarDetach()
// Export #3
// Per-window cleanup routine, called after the window has been destroyed.
// --------------------------------------------------------------------------
extern "C" EXPORT_C void MENUBAR_API MenubarDetach(void* context)
{
    CWinMenubar* menubar = (CWinMenubar*)context;
    delete menubar;
}

// --------------------------------------------------------------------------
// MenubarWndProc()
// Export #4
// Window procedure for handling menu commands and other window events.
// @return EMessagePassToSymbian (1) if the message should be passed to 
// Symbian kernel window proc also and EMessageHandledByMenubar (0) otherwise
// --------------------------------------------------------------------------
extern "C" EXPORT_C int MENUBAR_API
MenubarWndProc(void * context, UINT message, WPARAM wParam, LPARAM lParam)
{
    CWinMenubar* menubar = (CWinMenubar*)context;
    if (menubar) {
        return menubar->HandleMessage(message, wParam, lParam);
    } else {
        return CWinMenubar::EMessagePassToSymbian;
    }
}

// --------------------------------------------------------------------------
// MenubarStartupComplete()
// Export #5
//
// --------------------------------------------------------------------------
extern "C" EXPORT_C int MENUBAR_API
MenubarStartupComplete(void* context)
{
    CWinMenubar* menubar = (CWinMenubar*)context;
    return (int) menubar->CheckIfStarted( 0 ); /* 0 == false */
}

// --------------------------------------------------------------------------
// ModuleHandle()
// Returns handle to the EcmtMenubar dll.
// --------------------------------------------------------------------------
HMODULE CWinMenubar::ModuleHandle()
{
    return gModuleHandle;
}

// --------------------------------------------------------------------------
// WaitWndProc()
// Window proc for wait window.
// --------------------------------------------------------------------------
static LRESULT CALLBACK WaitWndProc( HWND hWnd, UINT message, 
                                     WPARAM wParam, LPARAM lParam )
{
    switch (message) 
    {
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hDC = BeginPaint( hWnd, &ps );
            SelectObject( hDC, GetStockObject( DEFAULT_GUI_FONT ) );
            RECT rc;
            GetClientRect( hWnd, &rc );
            SetBkMode( hDC, TRANSPARENT );
            TCHAR* text = (TCHAR*) GetWindowLong( hWnd, 0 );
            DrawText( hDC, text, _tcslen(text), &rc, DT_CENTER|DT_SINGLELINE|DT_VCENTER );
            EndPaint(hWnd, &ps); 
            return 0;
        }
    case WM_CLOSE:
        // If the user closes our wait window, close whole program
        PostQuitMessage(0);
        return 0;

    default:
        return( DefWindowProc( hWnd, message, wParam, lParam ) );
    }
}


// =========================== NMITLIB CALLBACKS ============================

#ifdef NMIT_INTEGRATION

NmitBool NMIT_API CWinMenubar::NmitLoad(void* aThis, const NmitUcs2* aUrl, 
                                        unsigned int aBrowserUid)
{
    CWinMenubar* self = (CWinMenubar*)aThis;
    while (!self->iEcmtMenubar->IsEmulatorStarted())
    {
        // Wait for emulator to come up
        Sleep(1000);
    }
    if (self->iEcmtMenubar->SmartLoad(aBrowserUid, aUrl)) {
        return NmitTrue;
    } else {
        return NmitFalse;
    }
}

NmitBool NMIT_API CWinMenubar::NmitRun(void* aThis, NmitStr aExe, 
                                       NmitStr* aArgs, int aArgCount,
                                       NmitProcessId* aProcessId)
{
    CWinMenubar* self = (CWinMenubar*)aThis;
    while (!self->iEcmtMenubar->IsEmulatorStarted())
    {
        // Wait for emulator to come up
        Sleep(1000);
    }
    if (self->iEcmtMenubar->RunExe(aExe, aArgs, aArgCount, aProcessId)) {
        return NmitTrue;
    } else {
        return NmitFalse;
    }
}

void NMIT_API CWinMenubar::NmitToFront(void* aThis)
{
    CWinMenubar* self = (CWinMenubar*)aThis;
    if (IsWindowVisible(self->iHwnd))
    {
        // Restore the window if it's minimized
        ShowWindow(self->iHwnd, SW_NORMAL);
        SetForegroundWindow(self->iHwnd);
    }
}

void NMIT_API CWinMenubar::NmitExit(void* aThis)
{
    // Do the same thing as if the user has clicked the "close window"
    // button,allowing the application to cleanly shutdown
    CWinMenubar* self = (CWinMenubar*)aThis;
    SendMessageA(self->iHwnd, WM_CLOSE, 0, 0);    
}

#endif // NMIT_INTEGRATION

// ============================ MEMBER FUNCTIONS ============================

void* CWinMenubar::operator new(unsigned int aSize)
{
    return Alloc(aSize);
}

void CWinMenubar::operator delete(void* aPtr)
{
    Free(aPtr);
}

// --------------------------------------------------------------------------
// CWinMenubar::New
// Creates an instance of the menubar associated with the specified window.
// --------------------------------------------------------------------------
CWinMenubar* CWinMenubar::New( HWND hWnd )
{
    CWinMenubar* self = new CWinMenubar(hWnd);
    if (self)
    {
        if (self->Initialize())
        {
            return self;
        }
        delete self;
    }
    return NULL;
}

// --------------------------------------------------------------------------
// CWinMenubar C++ constructor
// --------------------------------------------------------------------------
CWinMenubar::CWinMenubar( HWND hWnd )
{
    iHwnd = hWnd;
}

// --------------------------------------------------------------------------
// CWinMenubar destructor
// --------------------------------------------------------------------------
CWinMenubar::~CWinMenubar()
{
    if (iEcmtManProcess) CloseHandle(iEcmtManProcess);
    if (iMenu) DestroyMenu(iMenu);
    if (iEcmtMenubar) iEcmtMenubar->Release();
    if (iRichEdit) FreeLibrary(iRichEdit);
    Free(iParamString);
#ifdef HAVE_WLIB
    for (int i=0; i<ECMTMENUBAR_LINK_COUNT; i++) MEM_Free(iMenuURL[i]);
#endif // HAVE_WLIB
}

// --------------------------------------------------------------------------
// CWinMenubar constructor
// Adds menubar to the window. Also removes the IDM_REGISTER_NOW menu item if
// registration is not required. 
// --------------------------------------------------------------------------
BOOL CWinMenubar::Initialize()
{
#ifdef HAVE_WLIB   
    MEM_InitModule();   
    HASH_InitModule();   
#endif // HAVE_WLIB

    // Figure out whether this is a MIDP or C++ SDK. If midp.txt
    // is present under epoc32\tools\ecmt\, then we assume that this 
		// is a MIDP SDK. Otherwise, we assume this is a C++ SDK.

    iMidpSDK = FALSE;
    
		TCHAR path[MAX_PATH];
		_tcscpy( path, EcmtDir() );
    _tcscat( path, TEXT( "\\midp.txt" ) );

    DWORD attr = GetFileAttributes(path);
    if (attr != ((DWORD)-1)) 
		iMidpSDK = TRUE;
            
    iMenu = LoadMenuA(ModuleHandle(), MAKEINTRESOURCEA(IDM_MENU));
    iEcmtMenubar = EcmtMenubarProxy::New(this);
    if (!iMenu || !iEcmtMenubar) return FALSE;

    iAccTable = LoadAcceleratorsA(ModuleHandle(), 
                                  MAKEINTRESOURCEA(IDR_ACCELERATOR));
    RunStartups();
    iEmulatorStarted = FALSE;

    iOpenDir[0] = 0;
    iOpenFilterIndex = 1;   // Index is one-based
    
    char* productName = ReadTextFile(PRODUCTNAME_FILE);
    if (productName)
    {
#ifdef _DEBUG
        // NOTE: sizeof(DEBUG_SUFFIX) includes the NULL terminator
        static const char DEBUG_SUFFIX[] = " (debug)";
        int size = strlen(productName) + sizeof(DEBUG_SUFFIX);
        char* fullProductName = (char*)Alloc(size);
        if (fullProductName)
        {
            strcpy(fullProductName, productName);
            strcat(fullProductName, DEBUG_SUFFIX);
            Free(productName);
            productName = fullProductName;
        }
#endif // _DEBUG
        SetWindowTextA(iHwnd, productName);
        Free(productName);
    }

#ifdef NMIT_INTEGRATION
    const char* pid = GetProductId();
    iNmit = NMIT_Create(pid, NmitLoad, NmitRun, NmitToFront, NmitExit, this);
#endif // NMIT_INTEGRATION

    ZeroMemory(iMenuURL, sizeof(iMenuURL));

#ifdef HAVE_WLIB
    // Use ReadTextFile to read the file into the memory to avoid
    // UNICODE -> ASCII conversion. We are using non-UNICODE build 
    // of slib.
    char* info = ReadTextFile(SDKINFO_FILE);
    if (info)
    {
        // Create a stream that reads from a buffer in memory
        File* mem = FILE_MemIn(info,strlen(info));
        if (mem)
        {
            // Initialize the properties object
            Prop prop;
            if (PROP_Init(&prop))
            {
                // Parse the buffer
                if (PROP_Read(&prop, mem))
                {
                    char* javaVerifiedKey = "";
                    char* javaVerifiedMenu = "";
                    if(iMidpSDK){
                        javaVerifiedKey = JAVAVERIFIED_URL_KEY;        
                        javaVerifiedMenu = JAVAVERIFIED_MENU_KEY;
                    }
                    
                    static const Str urlKey[ECMTMENUBAR_LINK_COUNT] = {
                        FEEDBACK_URL_KEY_1,
                        javaVerifiedKey,
                        FEEDBACK_URL_KEY_2,
                        FEEDBACK_URL_KEY_3,
                        FEEDBACK_URL_KEY_4,
                        FEEDBACK_URL_KEY_5                        
                    };
                    static const Str menuKey[ECMTMENUBAR_LINK_COUNT] = {
                        FEEDBACK_MENU_KEY_1,
                        javaVerifiedMenu,
                        FEEDBACK_MENU_KEY_2,
                        FEEDBACK_MENU_KEY_3,
                        FEEDBACK_MENU_KEY_4,
                        FEEDBACK_MENU_KEY_5                       
                    };
                    for (int i=0; i<ECMTMENUBAR_LINK_COUNT; i++)
                    {
                        Str menu = PROP_Get(&prop, menuKey[i]);
                        iMenuURL[i] = STRING_Dup(PROP_Get(&prop, urlKey[i]));
                        if (menu && iMenuURL[i])
                        {
                            UINT cmd = IDM_LINK1+i;
                            ModifyMenuA(iMenu, cmd, MF_BYCOMMAND, cmd, menu);
                        }
                        else
                        {
                            // Will delete the menu item later
                            MEM_Free(iMenuURL[i]);
                            iMenuURL[i] = NULL;
                        }
                    }
                }
                PROP_Destroy(&prop);
            }
            FILE_Close(mem);
        }
        Free(info);
    }
#endif // HAVE_WLIB

    for (int i=0; i<ECMTMENUBAR_LINK_COUNT; i++)
    {
        if (!iMenuURL[i])
        {
            DeleteMenu(iMenu, IDM_LINK1+i, MF_BYCOMMAND);
        }
    }

    // Update window size to make spece for the menu
    RECT rect, windowRect;
    GetClientRect(iHwnd, &rect);
    GetWindowRect(iHwnd, &windowRect);
    AdjustWindowRect(&rect, GetWindowLong(iHwnd, GWL_STYLE), TRUE);
    windowRect.bottom = windowRect.top + (rect.bottom - rect.top);

    // Associate menu with the window
    SetMenu(iHwnd, iMenu);

    // Start timer in launcher mode. Normally, we check whether ECMT server
    // and specifically, SDK plugin have started on WM_INITMENUPOPUP message
    // (i.e. when user pullls down the menu). In launcher mode, there is no
    // menu, so we check it periodically on timer.
    if (LauncherMode())
    {
        SetTimer(iHwnd, LAUNCH_TIMER, 500, NULL);
    }

    // Resize the window
    MoveWindow(iHwnd, 
               windowRect.left, windowRect.top, 
               windowRect.right - windowRect.left, 
               windowRect.bottom - windowRect.top, 
               TRUE);

#ifdef SDK_REGISTRATION
    // Schedule startup registration check. We can't do it here because
    // that would block CreateWindow call and Symbian would quickly panic
    // because the native window initialization does not complete by the
    // time window server needs a window to draw on. PostMessage allows
    // startup to continue. The Registration wizard appears as a modal
    // dialog on top of the emulator window.
    PostMessage(iHwnd, WM_REGISTRATION_CHECK, 0, 0);
#endif // !SDK_REGISTRATION

    return TRUE;
}

// --------------------------------------------------------------------------
// CEcmtMenubar::GetProductId()
// Returns the registration product id
// --------------------------------------------------------------------------
const char * CWinMenubar::GetProductId() const
{
    return iMidpSDK ? SDK_PRODUCT_ID_MIDP : SDK_PRODUCT_ID_CPP;
}

// --------------------------------------------------------------------------
// CWinMenubar::HandleMessage
// Window procedure for handling menu commands and other window events.
// @return EMessagePassToSymbian (1) if the message should be passed to 
// Symbian kernel window proc also and EMessageHandledByMenubar (0) otherwise
// --------------------------------------------------------------------------
CWinMenubar::TMessageStatus 
CWinMenubar::HandleMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    MSG msg = {iHwnd, message,wParam,lParam,GetMessageTime(),GetMessagePos()};
    if (TranslateAcceleratorA(iHwnd, iAccTable, &msg))
    {
        return CWinMenubar::EMessageHandledByMenubar;
    }
    else
    {
        switch (message)
        {
            // List all message handlers here
            MESSAGE_HANDLER( WM_WINDOWPOSCHANGING )
            MESSAGE_HANDLER( WM_SETFOCUS )
            MESSAGE_HANDLER( WM_COMMAND )
            MESSAGE_HANDLER( WM_ECMT_LISTENING )
            MESSAGE_HANDLER( WM_ECMT_CONNECTED )
            MESSAGE_HANDLER( WM_ECMT_DISCONNECTED )
            MESSAGE_HANDLER( WM_INITMENUPOPUP )
            MESSAGE_HANDLER( WM_TIMER )
            MESSAGE_HANDLER( WM_DESTROY )
            MESSAGE_HANDLER( WM_MOUSEMOVE )

#ifdef SDK_REGISTRATION

            // Startup registration check
            case WM_REGISTRATION_CHECK:
                if ((++gRegInitCount) == 1) 
                {
                    // It's OK to call REG_Init more than once, but it isn't
                    // necessary. Only call it when we get here for the first
                    // time
                    REG_Init();
                }
                if (!gRegProductContext) 
                {
                    // Create the registration context and keep it in a static
                    // variable
                    const char* pid = GetProductId();
                    gRegProductContext = REG_CreateProductContext(pid);
                }
                if (gRegProductContext)
                {
                    // Check the registration status of the product, popping up
                    // the GUI if necessary
                    RegStatus r = REG_CheckStatus(gRegProductContext, iHwnd);
                    switch (r)
                    {
                    case RegRegistered:
                        // Product is already registered
                        DeleteMenu(iMenu, IDM_REGISTER_NOW, MF_BYCOMMAND);
                        break;

                    case RegExpired:
                        TerminateProcess(GetCurrentProcess(),(UINT)-1);
                        break;

                    default:
                        // First run or within trial period, allow to continue
                        break;
                    }
                }
                else
                {
                    // Registration is not available for some obscure reason
                    DeleteMenu(iMenu, IDM_REGISTER_NOW, MF_BYCOMMAND);
                }
                return EMessageHandledByMenubar;
            
#endif // SDK_REGISTRATION

        }
        return EMessagePassToSymbian;
    }
}

// --------------------------------------------------------------------------
// CEcmtMenubar::SetEmulatorStarted()
// Sets the one-way flag iEmulatorStarted to TRUE. Also, enables the File
// menu items.
// --------------------------------------------------------------------------
void CWinMenubar::SetEmulatorStarted()
{
    iEmulatorStarted = TRUE;
    EnableMenuItem(iMenu, IDM_LOAD, MF_ENABLED);
    EnableMenuItem(iMenu, IDM_OPEN, MF_ENABLED);
}

// --------------------------------------------------------------------------
// CEcmtMenubar::SetCoreUp()
// Sets the one-way flag iCoreUp to TRUE. Also, enables the Tools menu items
// --------------------------------------------------------------------------
void CWinMenubar::SetCoreUp()
{
    iCoreUp = TRUE;

    // Enable menu items
    EnableMenuItem(iMenu, IDM_PREFERENCES, MF_ENABLED);
    EnableMenuItem(iMenu, IDM_UTILITIES, MF_ENABLED);
    EnableMenuItem(iMenu, IDM_DIAGNOSTICS, MF_ENABLED);
}

// ---------------------------------------------------------------------------
// CWinMenubar::ReadTextFile()
// Reads a text file from the epoc32/data directory. Returns NULL if 
// file is not found. The returned must be deallocated by the caller
// using the Free function.
// ---------------------------------------------------------------------------
char* CWinMenubar::ReadTextFile(LPCTSTR fileName)
{
    char* text = NULL;
    TCHAR fname[MAX_PATH];
    _tcscpy(fname, Epocroot());
    _tcscat(fname, TEXT( "epoc32\\data\\"));
    _tcscat(fname, fileName);

    HANDLE hFile = CreateFile( fname, GENERIC_READ, FILE_SHARE_READ, NULL,
                OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (hFile != INVALID_HANDLE_VALUE)
    {
        // Alloc buffer for file contents plus terminating zero
        DWORD size = GetFileSize(hFile, NULL);
        text = (char*)Alloc(size+1); // one for NULL terminator
        if (text)
        {
            DWORD len;
            if (ReadFile(hFile, text, size, &len, NULL))
            {
                text[len] = '\0';
            }
        }
        CloseHandle(hFile);
    }
    return text;
}

// --------------------------------------------------------------------------
// CWinMenubar::RunStartups()
// 
// --------------------------------------------------------------------------
void CWinMenubar::RunStartups()
{
    TCHAR cmdFile[MAX_PATH];

    // Execute \epoc32\tools\ecmt\runonce.cmd (if it exists) and then delete it
    _tcscpy( cmdFile, EcmtDir() );
    _tcscat( cmdFile, TEXT( "\\runonce.cmd" ) );
    RunCmd( cmdFile, TRUE );

    // Execute \\epoc32\\tools\\ecmt\autorun.cmd (if it exists)
    _tcscpy( cmdFile, EcmtDir() );
    _tcscat( cmdFile, TEXT( "\\autorun.cmd" ) );
    RunCmd( cmdFile, FALSE );
}

// --------------------------------------------------------------------------
// CWinMenubar::ShowWaitWindow()
// 
// --------------------------------------------------------------------------
void CWinMenubar::ShowWaitWindow()
{
    WNDCLASS wc;
    static LPCTSTR KWaitWindowClassName = TEXT( "EcmtLauncherWaitWindow" );
	
    memset( &wc, 0, sizeof( wc ) );
    wc.lpszClassName = KWaitWindowClassName;
    wc.hInstance = ModuleHandle();
    wc.lpfnWndProc = WaitWndProc;
    wc.hCursor = LoadCursor( NULL, IDC_APPSTARTING );
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
    wc.style = CS_HREDRAW|CS_VREDRAW;
    wc.cbWndExtra = sizeof( TCHAR* );
	
    if ( !RegisterClass( &wc ) )
        return;
	
    TCHAR titleText[MAX_PATH];
    GetWindowText( iHwnd, titleText, COUNT( titleText ) );

    iWaitWindow = CreateWindowEx( WS_EX_TOOLWINDOW, 
                                  KWaitWindowClassName,
                                  titleText,
                                  WS_OVERLAPPEDWINDOW,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  (HWND)NULL,
                                  (HMENU)NULL,
                                  ModuleHandle(),
                                  NULL );

    LoadString(ModuleHandle(),IDS_WAIT_WINDOW,iWaitWindowText,COUNT(iWaitWindowText));
    SetWindowLong( iWaitWindow, 0, (LONG)iWaitWindowText ); 

    HDC hDC = GetDC( iWaitWindow );
    SelectObject( hDC, GetStockObject( DEFAULT_GUI_FONT ) );

    RECT rc = { 0,0,0,0	};
    SIZE sz, sz2;

    GetTextExtentPoint( hDC, iWaitWindowText, _tcslen(iWaitWindowText), &sz );
    GetTextExtentPoint( hDC, titleText, _tcslen( titleText ), &sz2 );
    ReleaseDC( iWaitWindow, hDC );
	
    sz2.cx = sz2.cx*4/3;	// Title is in bold so add 33%
    if ( sz2.cx > sz.cx )
        sz.cx = sz2.cx;
    sz.cy = sz.cy*2;
		
    rc.top = 0;
    rc.left = 0;
    rc.right = sz.cx;
    rc.bottom = sz.cy;

    AdjustWindowRectEx( &rc, 
                        GetWindowLong(iWaitWindow, GWL_STYLE),
                        FALSE,
                        GetWindowLong(iWaitWindow, GWL_EXSTYLE) );
    InflateRect( &rc, 5, 5 );
    SetWindowPos( iWaitWindow, 
                  0,
                  rc.left,
                  rc.top,
                  rc.right - rc.left,
                  rc.bottom - rc.top,
                  SWP_NOMOVE|SWP_NOZORDER|SWP_SHOWWINDOW );
    UpdateWindow( iWaitWindow );
}

// --------------------------------------------------------------------------
// CWinMenubar::LaunchEcmtMan
// Launches EcmtMan.exe process. If the previously launched process has died,
// restarts it.
// --------------------------------------------------------------------------
void CWinMenubar::LaunchEcmtMan()
{
    if (iEcmtManProcess)
    {
        if (WaitForSingleObject(iEcmtManProcess, 0) != WAIT_TIMEOUT)
        {
            CloseHandle(iEcmtManProcess);
            iEcmtManProcess = NULL;
        }
    }

    if (!iEcmtManProcess)
    {
        TCHAR exe[512];
        TCHAR cmd[1024];
        _tcscpy(exe, EcmtDir());
        _tcscat(exe, TEXT("\\"));
        _tcscat(exe, TEXT("EcmtMan.exe"));
        _tcscpy(cmd, TEXT("\""));
        _tcscat(cmd, exe);
        _tcscat(cmd, TEXT("\""));
        if (iParamString) {
            _tcscat(cmd, TEXT(" -D"));
            _tcscat(cmd, iParamString);
        }
        // Add -Demulator.home parameter (if applicable)
        TCHAR emulatorHome[MAX_PATH];
        _tcscpy(emulatorHome, Epocroot());
        TCHAR* p = _tcsstr(emulatorHome, TEXT("\\bin"));
        if ( p )
        {
            *p = TCHAR(0);
            _tcscat(cmd, TEXT(" -Demulator.home=\""));
            _tcscat(cmd, emulatorHome );
            _tcscat(cmd, TEXT("\""));
        }
        if ( LauncherMode() )
        {
            _tcscat(cmd, TEXT(" -Dexitonclose=true"));
        }

        PROCESS_INFORMATION pInfo;
        if (Launch( cmd, EcmtDir(), EKeepHandlesOpen, &pInfo))
        {
            CloseHandle(pInfo.hThread);
            iEcmtManProcess = pInfo.hProcess;
            WaitForSingleObject(iEcmtManProcess, 0);
        }
        else
        {
            TCHAR* text = cmd;
            _tcscpy(text, TEXT("Failed to start "));
            _tcscat(text, exe);
            LPCTSTR caption = iMidpSDK ? 
                TEXT(SDK_PRODUCT_ID_MIDP) : 
                TEXT(SDK_PRODUCT_ID_CPP);
            MessageBox(iHwnd, text, caption, MB_ICONHAND);
        }
    }
}

// --------------------------------------------------------------------------
// CWinMenubar::LaunchView
// 
// --------------------------------------------------------------------------
void CWinMenubar::LaunchView( WPARAM aCommand )
{
    // Unless core is connected or core is already launching, launch it now
    if (!iEcmtConnected)
    {
        // If EcmtManager is allready starting up. In this case we have to 
        // store aCommand, so all commands done before EcmtManager start up 
        // can be sent using SendEpdtCommand (see below). First command, 
        // which launces EcmtManager is stored to the index 0.
		for (int i = 0; i < KMaxEpdtCommandCount; ++i)
		{
			if(iEpdtCommand[i] == 0)
			{
				iEpdtCommand[i] = aCommand;
				break;
			}
		}
        LaunchEcmtMan();
    }
    // If core is connected just command it to show the desired window
    else if (iEcmtConnected)
    {
        if ( aCommand == IDM_PREFERENCES )
        {
            SendEpdtCommand( KShowPrefs );
        }
        else if ( aCommand == IDM_DIAGNOSTICS )
        {
            SendEpdtCommand( KShowDiagnostics );
        }
        else if ( aCommand == IDM_UTILITIES )
        {
            SendEpdtCommand( KShowUtilities );
        }
    }
}


// --------------------------------------------------------------------------
// CWinMenubar::RunCmd
// 
// --------------------------------------------------------------------------
void CWinMenubar::RunCmd( TCHAR* cmdFile, BOOL del )
{
    TCHAR cmd[MAX_PATH*2];

    HANDLE h = CreateFile( cmdFile, GENERIC_READ, FILE_SHARE_READ, NULL,
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
				    
    if ( h != INVALID_HANDLE_VALUE )
    {
        CloseHandle( h );
		
        // Quote cmd file name with "" in case it has a blanko
        _tcscpy( cmd, TEXT( "\"" ) );
        _tcscat( cmd, cmdFile );
        // Pass current epocroot as parameter #1 and quote that also in case it has a blanko
        _tcscat( cmd, TEXT( "\" \"" ) );

        // Change backslash to forward slash in epocroot (java needs this)
        TCHAR epocroot[MAX_PATH];
        _tcscpy( epocroot, Epocroot() );
        TCHAR* backslash;
        while( ( backslash = _tcspbrk( epocroot, TEXT("\\") ) ) != 0 )
        {
            *backslash = '/';
        }
        _tcscat( cmd, epocroot );
        _tcscat( cmd, TEXT( "\"" ) );
		
        // Pass platform as parameter #2
#ifdef __WINSCW__
        _tcscat( cmd, TEXT(" WINSCW") );
#else
        _tcscat( cmd, TEXT(" WINS") );
#endif
	
        PROCESS_INFORMATION pInfo;
        BOOL launchOK = Launch( cmd, EcmtDir(), 
            TEcmtLaunchFlags(EKeepHandlesOpen|EShowConsole), 
            &pInfo);
        if ( launchOK )
        {
            CloseHandle(pInfo.hThread);
            WaitForSingleObject(pInfo.hProcess, INFINITE);
            CloseHandle(pInfo.hProcess);
            if ( del )
            {
                DeleteFile( cmdFile );
            }
        }
    }
}

// --------------------------------------------------------------------------
// CWinMenubar::SelectFile
// Sends cmd to EPDT via ECMT :)
// --------------------------------------------------------------------------
void CWinMenubar::SendEpdtCommand( LPWSTR lpCommand )
{
    // The body contains null separated key-value pairs of UCS2
    // strings in network (big endian, MSB first) byte order
    Buffer buf;
    BUFFER_Init(&buf);
    buf.order = BIG_ENDIAN;

    static WCHAR KEY[] = L"command";
    static WCHAR TOP[] = L"top";
    static WCHAR LEFT[] = L"left";
 
    // "command" parameter
    int i;
    for (i=0; i<COUNT(KEY); i++) BUFFER_PutI16(&buf, KEY[i]);
    while (*lpCommand) BUFFER_PutI16(&buf, *lpCommand++);
    BUFFER_PutI16(&buf, 0);

    if (!LauncherMode())
    {
        WCHAR value[16];
        LPCWSTR p;

        // Get screen coordinates of the top-left corner of the client area
        RECT rect;
        GetClientRect(iHwnd, &rect);
        ClientToScreen(iHwnd, (LPPOINT)&rect);

        // "top" parameter
        wsprintfW(value, L"%d", rect.top);
        for (i=0; i<COUNT(TOP); i++) BUFFER_PutI16(&buf, TOP[i]);
        for (p=value; *p; p++) BUFFER_PutI16(&buf, *p);
        BUFFER_PutI16(&buf, 0);

        // "left" parameter
        wsprintfW(value, L"%d", rect.left);
        for (i=0; i<COUNT(LEFT); i++) BUFFER_PutI16(&buf, LEFT[i]);
        for (p=value; *p; p++) BUFFER_PutI16(&buf, *p);
        BUFFER_PutI16(&buf, 0);
    }

    // Send the message
    static const unsigned int uid = 0x10206FEF;
    iEcmtMenubar->SendEcmtMessage(uid,BUFFER_Access(&buf),BUFFER_Size(&buf));
    BUFFER_Destroy(&buf);
}


// --------------------------------------------------------------------------
// CWinMenubar::SelectFile
// Pops up the Open File dialog. Returns NULL selection was canceled or 
// failed  for some reason. Does not leave.
// --------------------------------------------------------------------------

// File extensions
#define BROWSER_FILES "*.html;*.htm;*.xhtml;*.wml;*.wmlc;*.cod;*.dd"
#define MEDIA_FILES "*.au;*.wav;*.mid;*.midi"   
#define IMAGE_FILES "*.gif;*.jpeg;*.jpg;*.wbmp;*.png;*.bmp;*.wmf;*.mbm;*.svg;*.jp2"
#define MMS_FILES   "*.mms"
#ifdef DRM_SUPPORT
#define DRM_FILES   "*.dm;*.dcf;*.drc"
#endif //DRM_SUPPORT
#define MIDP_FILES  "*.jar;*.jad"
#define TEXT_FILES  "*.txt"
#define OTHER_FILES "*.zip;*.sis;*.wgz;*.sisx"

TCHAR* CWinMenubar::SelectFile(HWND hWndOwner)
{
    TCHAR * selection = NULL;
    char szFile[MAX_PATH];
    OPENFILENAMEA ofn;
    szFile[0] = 0;

    static LPCSTR cppFilter = 
        "All Supported Files\0"
        BROWSER_FILES  ";" 
        TEXT_FILES ";"
        OTHER_FILES ";"
#ifdef PUSH_SUPPORT
        MMS_FILES   ";"
#endif // PUSH_SUPPORT
        IMAGE_FILES ";"
#ifdef MEDIA_FILES
        MEDIA_FILES ";"
#endif // MEDIA_FILES
#ifdef DRM_SUPPORT
        DRM_FILES   "\0"
#endif //DRM_SUPPORT
        "Browser Files\0"   BROWSER_FILES  "\0"
#ifdef PUSH_SUPPORT
        "MMS Files\0"       MMS_FILES   "\0"
#endif // PUSH_SUPPORT
        "Images\0"          IMAGE_FILES "\0"
#ifdef MEDIA_FILES
        "Media\0"           MEDIA_FILES "\0"
#endif // MEDIA_FILES
#ifdef DRM_SUPPORT
//        "DRM Files\0"       DRM_FILES   "\0"
#endif //DRM_SUPPORT
        "All Files\0*.*\0";

    static LPCSTR midpFilter = 
        "All Supported Files\0"
        BROWSER_FILES  ";" 
        TEXT_FILES ";"
        OTHER_FILES ";"
#ifdef PUSH_SUPPORT
        MMS_FILES   ";"
#endif // PUSH_SUPPORT
        MIDP_FILES  ";"
        IMAGE_FILES ";"
#ifdef MEDIA_FILES
        MEDIA_FILES ";"
#endif // MEDIA_FILES
#ifdef DRM_SUPPORT
        DRM_FILES   "\0"
#endif //DRM_SUPPORT
        "Browser Files\0"   BROWSER_FILES  "\0"
#ifdef PUSH_SUPPORT
        "MMS Files\0"       MMS_FILES   "\0"
#endif // PUSH_SUPPORT
        "MIDP Files\0"      MIDP_FILES  "\0"
        "Images\0"          IMAGE_FILES "\0"
#ifdef MEDIA_FILES
        "Media\0"           MEDIA_FILES "\0"
#endif // MEDIA_FILES
#ifdef DRM_SUPPORT
//        "DRM Files\0"       DRM_FILES   "\0"
#endif //DRM_SUPPORT
        "All Files\0*.*\0";

    // Initialize OPENFILENAME
    memset(&ofn, 0, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWndOwner;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = iMidpSDK ? midpFilter : cppFilter;
        
    ofn.nFilterIndex = iOpenFilterIndex;
    ofn.lpstrInitialDir = iOpenDir;
    ofn.Flags = OFN_PATHMUSTEXIST |
                OFN_FILEMUSTEXIST |
                OFN_ENABLESIZING  |
                OFN_HIDEREADONLY;

    // Switch to "kernel mode" while OpenFile dialog is running
    BOOL ok = GetOpenFileNameA(&ofn);
    if (ok)
    {
        // Filter out MIDP files if SystemAMS is not available.
        if (iMidpSDK || 
           (!STRING_EndsWithNoCase(szFile, KJarExt) && 
            !STRING_EndsWithNoCase(szFile, KJadExt)))
        {
            // NOTE: we can allocate more than we need, because one byte
            // can't possibly translate into more than one Unicode character,
            // while one Unicode character can translate into more than one
            // byte (consider UTF-8 for example).
            selection = (TCHAR*)Alloc((strlen(szFile)+1) * sizeof(TCHAR));
        }
        if (selection)
        {
#ifdef _UNICODE
            // Convert file name to Unicode
            selection[0] = 0;
            MultiByteToWideChar(CP_ACP, 0, szFile, -1, selection, MAX_PATH);
#else
            strcpy(selection, szFile);
#endif
        }
    }

    iOpenFilterIndex = ofn.nFilterIndex;
    return selection;
}
 
// --------------------------------------------------------------------------
// MEcmtMenubarCallbacks
// --------------------------------------------------------------------------

void CWinMenubar::EcmtServerListening(const unsigned short* params)
{
    int len = wcslen(params);
    LPTSTR paramString = (LPTSTR)Alloc(sizeof(TCHAR)*(len+1));
    if (paramString)
    {
        // NOTE: WCHAR and TCHAR isn't necessarily the same thing
        LPCWSTR src = params;
        LPTSTR dest = paramString;
        while ((*dest++ = *src++) != 0) (void)0;
    }
    PostMessageA(iHwnd, WM_ECMT_LISTENING, 0, (LPARAM)paramString);
}

void CWinMenubar::EcmtServerConnected()
{
    PostMessageA(iHwnd, WM_ECMT_CONNECTED, 0, 0);
}

void CWinMenubar::EcmtServerDisconnected()
{
    PostMessageA(iHwnd, WM_ECMT_DISCONNECTED, 0, 0);
}

// --------------------------------------------------------------------------
// CWinMenubar::WM_SETFOCUS_Handler
// 
// --------------------------------------------------------------------------
CWinMenubar::TMessageStatus 
CWinMenubar::WM_SETFOCUS_Handler(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    if ( LauncherMode() )
    {
       	SetActiveWindow( iWaitWindow );
    }
    return EMessagePassToSymbian;
}

// --------------------------------------------------------------------------
// CWinMenubar::WM_WINDOWPOSCHANGING_Handler
// 
// --------------------------------------------------------------------------
CWinMenubar::TMessageStatus 
CWinMenubar::WM_WINDOWPOSCHANGING_Handler(WPARAM /*wParam*/, LPARAM lParam)
{
    WINDOWPOS* p = (WINDOWPOS*)lParam;
    // In launcher mode never show the emulator window
    if ( LauncherMode() )
    {
        p->flags &= !SWP_SHOWWINDOW;
        if ( !iWaitWindow )
        {
            ShowWaitWindow();
        }
    }
    return EMessagePassToSymbian;
}

// --------------------------------------------------------------------------
// CWinMenubar::CheckIfStarted
// Checks if various components (emulator itself, ECMT server in particular)
// are started.
// --------------------------------------------------------------------------
BOOL CWinMenubar::CheckIfStarted(BOOL aCheckEcmt /* = TRUE */)
{
    if (!iEmulatorStarted)
    {
        if (iEcmtMenubar->IsEmulatorStarted())
        {
            SetEmulatorStarted();
        }
    }
    if (!iCoreUp && iEmulatorStarted && aCheckEcmt)
    {
        if (iEcmtMenubar->IsCoreUp())
        {
            SetCoreUp();
        }
    }
    return (iCoreUp || ((!aCheckEcmt) && iEmulatorStarted ));
}

// --------------------------------------------------------------------------
// CWinMenubar::WM_INITMENUPOPUP_Handler
// 
// --------------------------------------------------------------------------
CWinMenubar::TMessageStatus 
CWinMenubar::WM_INITMENUPOPUP_Handler(WPARAM /*wParam*/, LPARAM lParam)
{
    if (lParam == 0 || lParam == 1)
    {
        CheckIfStarted(lParam == 1);
    }
    return EMessagePassToSymbian;
}

// --------------------------------------------------------------------------
// CWinMenubar::WM_TIMER_Handler ()
// Handles timer messages in the launcher mode
// --------------------------------------------------------------------------
CWinMenubar::TMessageStatus 
CWinMenubar::WM_TIMER_Handler(WPARAM wParam, LPARAM /*lParam*/)
{
    if (wParam == LAUNCH_TIMER)
    {
        CheckIfStarted();
        if (iCoreUp)
        {
            KillTimer(iHwnd, (UINT_PTR)wParam);
        }
        return EMessageHandledByMenubar;
    }
    return EMessagePassToSymbian;
}

// --------------------------------------------------------------------------
// CWinMenubar::WM_ECMT_LISTENING_Handler
// --------------------------------------------------------------------------
CWinMenubar::TMessageStatus 
CWinMenubar::WM_ECMT_LISTENING_Handler(WPARAM /*wParam*/, LPARAM lParam)
{
    Free(iParamString);
    iParamString = (LPTSTR)lParam;

    // In launcher mode automatically launch preferences when Ecmt 
    // server is up
    if (iParamString && LauncherMode())
    {
        IDM_PREFERENCES_Handler(IDM_PREFERENCES, 0);
    }

    return EMessageHandledByMenubar;
}

// --------------------------------------------------------------------------
// CWinMenubar::WM_ECMT_CONNECTED_Handler
// --------------------------------------------------------------------------
CWinMenubar::TMessageStatus 
CWinMenubar::WM_ECMT_CONNECTED_Handler(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    iEcmtConnected = TRUE;
    // Launch all user selected views (Diagnostics, Utilities, Preferences) 
    // from the epdt command queue and null the queue.
	for (int i = 0; i < KMaxEpdtCommandCount; ++i)
	{
		if (iEpdtCommand[i] != 0)
		{
			LaunchView( iEpdtCommand[i] );
			iEpdtCommand[i] = 0;
		}
	}
	
	if ( iWaitWindow )
    {
        DestroyWindow( iWaitWindow ); // Wait window can now be dismissed
    }
    return EMessageHandledByMenubar;
}

// --------------------------------------------------------------------------
// CWinMenubar::WM_ECMT_DISCONNECTED_Handler
// --------------------------------------------------------------------------
CWinMenubar::TMessageStatus 
CWinMenubar::WM_ECMT_DISCONNECTED_Handler(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    iEcmtConnected = FALSE;
    if (LauncherMode())
    {
        PostQuitMessage( 0 );         // epdt closed connection
    }
    return EMessageHandledByMenubar;
}

// --------------------------------------------------------------------------
// CWinMenubar::WM_DESTROY_Handler
// 
// --------------------------------------------------------------------------
CWinMenubar::TMessageStatus 
CWinMenubar::WM_DESTROY_Handler(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    if (iEcmtConnected)
    {
        SendEpdtCommand( KHidePrefs );
        SendEpdtCommand( KExitCore );
    }
	
#ifdef SDK_REGISTRATION
    if ((gRegInitCount--) == 1) 
    {
        REG_DeleteProductContext(gRegProductContext);
        REG_Deinit();   
        gRegProductContext = NULL;
    }
#endif // !SDK_REGISTRATION

    return EMessagePassToSymbian;
}

// --------------------------------------------------------------------------
// CWinMenubar::WM_MOUSEMOVE_Handler
// 
// --------------------------------------------------------------------------
CWinMenubar::TMessageStatus 
CWinMenubar::WM_MOUSEMOVE_Handler(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    // If we have first epdt command on the queue, show arrow+hourglass
    if ( iEpdtCommand[0] != 0 )
    {
        // Set cursor to arrow+hourglass and don't pass message to symbian 
        // kernel because it would change it back to arrow (or hand)
        SetCursor( LoadCursor( 0, IDC_APPSTARTING ) );
        return EMessageHandledByMenubar;
    }
    return EMessagePassToSymbian;
}
	
// --------------------------------------------------------------------------
// CWinMenubar::WM_COMMAND_Handler
// 
// --------------------------------------------------------------------------
CWinMenubar::TMessageStatus 
CWinMenubar::WM_COMMAND_Handler(WPARAM wParam, LPARAM lParam)
{
    TMessageStatus status = EMessagePassToSymbian;
    UINT cmd = LOWORD(wParam);

    switch (cmd)
    {
         MESSAGE_HANDLER( IDM_LOAD )
         MESSAGE_HANDLER( IDM_OPEN )
         MESSAGE_HANDLER( IDM_EXIT )
         MESSAGE_HANDLER( IDM_PREFERENCES )
         MESSAGE_HANDLER( IDM_UTILITIES )
         MESSAGE_HANDLER( IDM_DIAGNOSTICS )
         MESSAGE_HANDLER( IDM_USERS_GUIDE )
         MESSAGE_HANDLER( IDM_RELEASE_NOTES )
         MESSAGE_HANDLER( IDM_ABOUT )
         MESSAGE_HANDLER( IDM_STARTUPMODE )
        case IDM_SWITCH_CONFIG:
            {
                // Sys command ID 1 is hardcoded in gui.cpp
                SendMessageA(iHwnd, WM_SYSCOMMAND, 1, 0);
                status = EMessageHandledByMenubar;
            }
            break;

#ifdef SDK_REGISTRATION
        case IDM_REGISTER_NOW:
            if (gRegProductContext) 
            {
                REG_RegisterNow(gRegProductContext, iHwnd);
                if (REG_ProductStatus(gRegProductContext) == RegRegistered)
                {
                    // The product is registered, remove the menu item
                    DeleteMenu(iMenu, IDM_REGISTER_NOW, MF_BYCOMMAND);
                }
            }
            status = EMessageHandledByMenubar;
            break;
#endif // SDK_REGISTRATION
		
        default:
#ifdef HAVE_WLIB
            if (cmd >= IDM_LINK1 && cmd < (IDM_LINK1+ECMTMENUBAR_LINK_COUNT))
            {
                Str url = iMenuURL[cmd-IDM_LINK1];
                if (url)
                {
                    WIN32_BrowseURL(iMenuURL[cmd-IDM_LINK1]);
                    return EMessageHandledByMenubar;
                }
            }
#endif // HAVE_WLIB
            break;
		
    }
    
    return status;
}

// --------------------------------------------------------------------------
// CWinMenubar::IDM_LOAD_Handler
// 
// --------------------------------------------------------------------------
CWinMenubar::TMessageStatus 
CWinMenubar::IDM_LOAD_Handler(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    LPCSTR lpTemplate = MAKEINTRESOURCEA(IDD_LOAD);
    DialogBoxParamA(ModuleHandle(),lpTemplate,iHwnd,&LoadDialogProc,(long)this);
    return EMessageHandledByMenubar;
}

// --------------------------------------------------------------------------
// CWinMenubar::IDM_OPEN_Handler
// 
// --------------------------------------------------------------------------
CWinMenubar::TMessageStatus 
CWinMenubar::IDM_OPEN_Handler(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    TCHAR* fname = SelectFile(iHwnd);
    if (fname) 
    {
        iEcmtMenubar->LoadFile(iDefaultBrowser, fname);
        Free(fname);
    }
    return EMessageHandledByMenubar;
}
 
// --------------------------------------------------------------------------
// CWinMenubar::IDM_EXIT_Handler
// 
// --------------------------------------------------------------------------
CWinMenubar::TMessageStatus 
CWinMenubar::IDM_EXIT_Handler(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    //DestroyWindow( iHwnd );
	SendMessageA(iHwnd, WM_CLOSE, 0, 0);
    return EMessageHandledByMenubar;
}
 
// --------------------------------------------------------------------------
// CWinMenubar::IDM_PREFERENCES_Handler
// 
// --------------------------------------------------------------------------
CWinMenubar::TMessageStatus 
CWinMenubar::IDM_PREFERENCES_Handler(WPARAM wParam, LPARAM /*lParam*/)
{
    LaunchView(LOWORD(wParam));
    return EMessageHandledByMenubar;
}

// -----------------------------------------------------------------------------
// CWinMenubar::IDM_UTILITIES_Handler
// 
// -----------------------------------------------------------------------------
CWinMenubar::TMessageStatus 
CWinMenubar::IDM_UTILITIES_Handler(WPARAM wParam, LPARAM /*lParam*/)
{
    LaunchView(LOWORD(wParam));
    return EMessageHandledByMenubar;
}

// --------------------------------------------------------------------------
// CWinMenubar::IDM_DIAGNOSTICS_Handler
// 
// --------------------------------------------------------------------------
CWinMenubar::TMessageStatus 
CWinMenubar::IDM_DIAGNOSTICS_Handler(WPARAM wParam, LPARAM /*lParam*/)
{
    LaunchView(LOWORD(wParam));
    return EMessageHandledByMenubar;
}

// --------------------------------------------------------------------------
// CWinMenubar::IDM_USERS_GUIDE_Handler
// 
// --------------------------------------------------------------------------
CWinMenubar::TMessageStatus 
CWinMenubar::IDM_USERS_GUIDE_Handler(WPARAM /*wParam*/, LPARAM /*lParam*/)
{	
	TCHAR helpFileToLaunch[MAX_PATH];
	TCHAR workDir[MAX_PATH];
	// Error message, size of MAX_PATH for the parh + 30 for actual error 
	// message.
	TCHAR errorMsg[MAX_PATH+30];
	int error = 0;

	if(!iMidpSDK)
	{
		_tcscpy(helpFileToLaunch, Epocroot());
		_tcscat(helpFileToLaunch, TEXT("S60Doc\\eclipse.exe"));

		_tcscpy(workDir, Epocroot());
		_tcscat(workDir, TEXT("S60Doc"));

		PROCESS_INFORMATION pInfo;
		if (Launch( helpFileToLaunch, workDir, EKeepHandlesOpen, &pInfo))
		{
			CloseHandle(pInfo.hThread);
			WaitForSingleObject(pInfo.hProcess, 0);
		}
		else
		{
			//Show the error message
			error = 1;
		}
	}
	else
	{
	    //Try to launch MIDP index.html
	    _tcscpy( helpFileToLaunch, Epocroot() );
	 		_tcscat(helpFileToLaunch, TEXT("docs\\eclipse.exe"));
	 		_tcscpy(workDir, Epocroot());
			_tcscat(workDir, TEXT("docs"));
			
			PROCESS_INFORMATION pInfo;
			if (Launch( helpFileToLaunch, workDir, EKeepHandlesOpen, &pInfo))
			{
				CloseHandle(pInfo.hThread);
				WaitForSingleObject(pInfo.hProcess, 0);
			}
			else
			{
				//Show the error message
				error = 1;
			}
  }      

	if(error)
	{
		//Show the error message
		_tcscpy(errorMsg, TEXT("Failed to start "));
		_tcscat(errorMsg, helpFileToLaunch);
		LPCTSTR caption = iMidpSDK ? 
		TEXT(SDK_PRODUCT_ID_MIDP) : 
		TEXT(SDK_PRODUCT_ID_CPP);
		MessageBox(iHwnd, errorMsg, caption, MB_ICONHAND);
	}

    return EMessageHandledByMenubar;
}

// --------------------------------------------------------------------------
// CWinMenubar::IDM_RELEASE_NOTES_Handler
// 
// --------------------------------------------------------------------------
CWinMenubar::TMessageStatus 
CWinMenubar::IDM_RELEASE_NOTES_Handler(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    TCHAR relNotes[MAX_PATH];
    _tcscpy( relNotes, Epocroot() );
    _tcscat( relNotes, TEXT( "\\releasenotes.txt" ) );
    ShellExecute( iHwnd, 0, relNotes,0, 0, SW_SHOWNORMAL );
    return EMessageHandledByMenubar;
}

// --------------------------------------------------------------------------
// CWinMenubar::IDM_STARTUPMODE_Handler
// 
// --------------------------------------------------------------------------
CWinMenubar::TMessageStatus 
CWinMenubar::IDM_STARTUPMODE_Handler(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    TCHAR ModeNotification[MAX_PATH];
    _tcscpy( ModeNotification, Epocroot() );
    _tcscat( ModeNotification, TEXT( "epoc32\\data\\StartupMode_Notification.rtf" ) );
    ShellExecute( iHwnd, 0, ModeNotification, 0, 0, SW_SHOWNORMAL );
    return EMessageHandledByMenubar;
}

// --------------------------------------------------------------------------
// CWinMenubar::IDM_ABOUT_Handler
// 
// --------------------------------------------------------------------------
CWinMenubar::TMessageStatus 
CWinMenubar::IDM_ABOUT_Handler(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
    // Load the RichEdit control dll
    if (!iRichEdit) iRichEdit = LoadLibraryA("riched20.dll");

    // Pop up the dialog
    DialogBoxParamA(ModuleHandle(), MAKEINTRESOURCEA(IDD_ABOUT), iHwnd, 
                    &AboutProc, (LPARAM)this);

#ifdef SDK_REGISTRATION
    // The About box the "Register" button. The registration status may 
    // have changed
    if (gRegProductContext)
    {
        // Check the registration status of the product
        if (REG_ProductStatus(gRegProductContext) == RegRegistered)
        {
            // Make sure the "Register Now" button is not visible
            DeleteMenu(iMenu, IDM_REGISTER_NOW, MF_BYCOMMAND);
        }
    } 
#endif  // SDK_REGISTRATION

    return EMessageHandledByMenubar;
}

/*
 * Local Variables:
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * End:
 */

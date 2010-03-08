/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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


#include "EcmtGwUI.h"
#include "SdkProductInfo.h"

#define NBUTTONS 3

/* EcmtManager commands */
static const WCHAR KShowPrefs[] = L"show_preferences";
static const WCHAR KShowUtilities[] = L"show_utilities";
static const WCHAR KShowDiagnostics[] = L"show_diagnostics";
static const WCHAR KHidePrefs[] = L"hide_preferences";
static const WCHAR KExitCore[]  = L"exit";

/* Location (relative to EPOCROOT) and name of the EcmtManager executable */
static const Char KEcmtManDir[] = "epoc32\\tools\\ecmt";
static const Char KEcmtManExe[] = 
#if DEBUG
"EcmtManD.exe"  /* debug executable */
#else /* !DEBUG */
"EcmtMan.exe"
#endif /* !DEBUG */
;

/* MIDP specific file to tell MIDP SDK from C++ SDK, relative to EPOCROOT */
static const Char KMIDPFile[] = "midp.txt";
static const Str KBinDirs[] = {
    "epoc32\\release\\winscw\\udeb\\",
    "epoc32\\release\\winscw\\urel\\",
    "epoc32\\release\\wins\\udeb\\",
    "epoc32\\release\\wins\\udrel\\"
};

/* Ecmt manager UID that receives the above commands */
static const unsigned int EcmtManagerUid = 0x10206FEF;

/***************************************************************************
 * Callbacks
 ***************************************************************************/

/**
 * Invoked when a new Ecmt client is connected. It's ok if this callback
 * if only invoked once after several clients have connected. The actual
 * number of currently connected clients can be retrieved by calling the
 * ECMTGW_CountEcmtClients function.
 */
STATIC void GUI_ClientConnectCB(EcmtGateway* gw, void* ctx)
{
    EcmtGwUI* ui = ctx;
    TRACE("GWUI: ClientConnectCB()\n");
    PostMessage(ui->mainWindow, WM_ECMTGW_NEW_CLIENT, 0, 0);
}

/**
 * Invoked when gateway has been successfully activated and attached
 * to the specified port.
 */
STATIC void GUI_ActivateCB(EcmtGateway* gw, EcmtGatewayPort* port, void* ctx)
{
    EcmtGwUI* ui = ctx;
    TRACE1("GWUI: ActivateCB(%s)\n",ECMTGW_GetPortName(port));
    PostMessage(ui->mainDialog, WM_ECMTGW_ACTIVATE, (WPARAM)port, 0);
    PostMessage(ui->mainWindow, WM_ECMTGW_ACTIVATE, (WPARAM)port, 0);
}

/**
 * Invoked when gateway has been deactivated, either as a result of calling
 * ECMTGW_Deactivate, port I/O error or Ecmt protocol violation.
 */
STATIC void GUI_DeactivateCB(EcmtGateway* gw, EcmtGwDeactReason r, void* ctx)
{
    EcmtGwUI* ui = ctx;
    TRACE("GWUI: DeactivateCB\n");
    PostMessage(ui->mainDialog, WM_ECMTGW_DEACTIVATE, r, 0);
    PostMessage(ui->mainWindow, WM_ECMTGW_DEACTIVATE, r, 0);
}

/**
 * Invoked when gateway detects a new communication port in the system.
 */
STATIC void GUI_PortFoundCB(EcmtGateway* gw, EcmtGatewayPort* port, void* ctx)
{
    EcmtGwUI* ui = ctx;
    TRACE1("GWUI: GUI_PortFoundCB(%s)\n",ECMTGW_GetPortName(port));
    SendMessage(ui->mainDialog, WM_ECMTGW_PORT_FOUND, (WPARAM)port, 0);
}

/**
 * Invoked when gateway detects a new communication port in the system.
 */
STATIC void GUI_PortChangeCB(EcmtGateway* gw, EcmtGatewayPort* port, void* ctx)
{
    EcmtGwUI* ui = ctx;
    TRACE1("GWUI: GUI_PortChangeCB(%s)\n",ECMTGW_GetPortName(port));
    SendMessage(ui->mainDialog, WM_ECMTGW_PORT_MODIFIED, (WPARAM)port, 0);
}

/**
 * Invoked when gateway detects a new communication port in the system.
 */
STATIC void GUI_PortGoneCB(EcmtGateway* gw, EcmtGatewayPort* port, void* ctx)
{
    EcmtGwUI* ui = ctx;
    TRACE1("GWUI: GUI_PortGoneCB(%s)\n",ECMTGW_GetPortName(port));
    SendMessage(ui->mainDialog, WM_ECMTGW_PORT_DISAPPEARED, (WPARAM)port, 0);
}

/**
 * Invoked when port scan starts.
 */
STATIC void GUI_RefreshStart(EcmtGateway* gw, void* ctx)
{
    EcmtGwUI* ui = ctx;
    TRACE("GWUI: GUI_RefreshStart\n");
    PostMessage(ui->mainWindow, WM_ECMTGW_REFRESH_STARTED, 0, 0);
}

/**
 * Invoked when port scan finishes.
 */
STATIC void GUI_RefreshDone(EcmtGateway* gw, void* ctx)
{
    EcmtGwUI* ui = ctx;
    TRACE("GWUI: GUI_RefreshDone\n");
    ui->refreshDone = True;
    PostMessage(ui->mainWindow, WM_ECMTGW_REFRESH_DONE, 0, 0);
}

/**
 * Invoked when gateway starts sending Ecmt packet to the COM port.
 */
STATIC void GUI_SendPacketCB(EcmtGateway* gw, int uid, const void* data, 
                             int len, void* ctx)
{
    EcmtGwUI* ui = ctx;
    if (ECMTGW_IsActive(gw)) {
        TRACE2("GWUI: SendPacketCB (0x%08x, %d bytes)\n",uid,len);
        PostMessage(ui->mainDialog, WM_ECMTGW_SEND_PACKET, 0, 0);
    }
}

/**
 * Invoked when gateway has finished sending Ecmt packet to the COM port.
 */
STATIC void GUI_SendPacketCompleteCB(EcmtGateway* gw, void* ctx)
{
    EcmtGwUI* ui = ctx;
    if (ECMTGW_IsActive(gw)) {
        TRACE("GWUI: SendPacketCompleteCB\n");
        PostMessage(ui->mainDialog, WM_ECMTGW_SEND_PACKET_COMPLETE, 0, 0);
    }
}

/**
 * Invoked when gateway has received Ecmt header from the COM port and starts
 * receiving the body of the Ecmt message.
 */
STATIC void GUI_ReceivePacketCB(EcmtGateway* gw, int uid, int len, void* ctx)
{
    EcmtGwUI* ui = ctx;
    if (ECMTGW_IsActive(gw)) {
        TRACE2("GWUI: ReceivePacketCB (0x%08x, %d bytes)\n",uid,len);
        PostMessage(ui->mainDialog, WM_ECMTGW_RECEIVE_PACKET, 0, 0);
    }
}

/**
 * Invoked when gateway has finished receiving Ecmt packet from the COM port.
 */
STATIC void GUI_ReceivePacketCompleteCB(EcmtGateway* gw, int uid, 
                                        const void* data, int len, void* ctx)
{
    EcmtGwUI* ui = ctx;
    if (ECMTGW_IsActive(gw)) {
        TRACE2("GWUI: ReceivePacketCompleteCB (0x%08x, %d bytes)\n",uid,len);
        PostMessage(ui->mainDialog, WM_ECMTGW_RECEIVE_PACKET_COMPLETE, 0, 0);
    }
}

/**
 * Invoked when the UI needs to attract user's attention. Usually, it's done
 * by bringing main window to front, flashing application icon on the task
 * bar on Windows or "jumping" application icon in the Dock on Mac.
 */
STATIC void GUI_ToFrontCB(EcmtGateway* gw, void* ctx)
{
    EcmtGwUI* ui = ctx;
    TRACE("GWUI: ToFrontCB\n");
    ShowWindow(ui->mainWindow, SW_NORMAL);
    SetForegroundWindow(ui->mainWindow);
}

/**
 * Request to close the shop and exit.
 */
STATIC void GUI_ExitCB(EcmtGateway* gw, void* ctx)
{
    EcmtGwUI* ui = ctx;
    TRACE("GWUI: ExitCB\n");
    PostMessage(ui->mainWindow, WM_CLOSE, 0, 0);
}

/***************************************************************************
 * Updates refresh related pieces of the UI
 ***************************************************************************/
STATIC void GUI_WndUpdateRefreshUI2(EcmtGwUI* ui, Bool active)
{
    HMENU hMenu = GetMenu(ui->mainWindow);
    Bool enable = (!ui->refreshInProgress && !active);
    Bool animate = (ui->refreshInProgress && !active);
    Bool animationVisible = (GetWindowStyle(ui->refreshAnimation)&WS_VISIBLE);
    ShowWindow(ui->refreshAnimation, animate ? SW_SHOW : SW_HIDE);
    if (animationVisible && !animate) {
        HWND hToolTips = (HWND)SendMessage(ui->toolBar,TB_GETTOOLTIPS,0,0);
        SendMessage(hToolTips, TTM_POP, 0, 0); /* Hide the tooltip */
        Animate_Stop(ui->refreshAnimation);
        Animate_Close(ui->refreshAnimation);
        ShowWindow(ui->refreshAnimation, SW_HIDE);
    } else if (!animationVisible && animate) {
        ShowWindow(ui->refreshAnimation, SW_SHOW);
        Animate_Open(ui->refreshAnimation, MAKEINTRESOURCE(IDR_REFRESH));
        Animate_Play(ui->refreshAnimation, 0, -1, -1);
    }
    SendMessage(ui->toolBar,TB_ENABLEBUTTON,IDM_REFRESH,MAKELONG(enable,0));
    EnableMenuItem(hMenu, IDM_REFRESH, MF_BYCOMMAND | 
        (enable ? MF_ENABLED : MF_GRAYED));
}

STATIC void GUI_WndUpdateRefreshUI(EcmtGwUI* ui)
{
    GUI_WndUpdateRefreshUI2(ui, ECMTGW_IsActive(ui->engine));
}

/***************************************************************************
 * Updates the COM port related pieces of the UI
 ***************************************************************************/
STATIC void GUI_WndUpdateComPortUI(EcmtGwUI* ui, Bool active)
{
    Bool connect = (!active && ui->port);
    HMENU hMenu = GetMenu(ui->mainWindow);
    GUI_WndUpdateRefreshUI2(ui, active);
    SendMessage(ui->toolBar,TB_ENABLEBUTTON,IDM_CONNECT,MAKELONG(connect,0));
    SendMessage(ui->toolBar,TB_ENABLEBUTTON,IDM_STOP,MAKELONG(active,0));
    EnableMenuItem(hMenu, IDM_CONNECT, MF_BYCOMMAND | 
        (connect ? MF_ENABLED : MF_GRAYED));
    EnableMenuItem(hMenu, IDM_STOP, MF_BYCOMMAND | 
        (active ? MF_ENABLED : MF_GRAYED));
}

/***************************************************************************
 * Handles WM_ECMTGW_SELECT_PORT message.
 ***************************************************************************/
STATIC void GUI_WndSelectPort(EcmtGwUI* ui, EcmtGwPort* port)
{
    if (!ECMTGW_IsActive(ui->engine)) {
        BOOL canConnect = (port != NULL);
        SendMessage(ui->toolBar,TB_ENABLEBUTTON, IDM_CONNECT, 
            MAKELONG(canConnect,0));
        EnableMenuItem(GetMenu(ui->mainWindow), IDM_CONNECT, 
            MF_BYCOMMAND | (canConnect ? MF_ENABLED : MF_GRAYED));
    }
}

/***************************************************************************
 * Handles WM_ECMTGW_ACTIVATE message.
 ***************************************************************************/
STATIC void GUI_WndActivate(EcmtGwUI* ui, EcmtGatewayPort* port)
{
    GUI_WndUpdateComPortUI(ui, True);
    SetFocus(ui->mainDialog);
}

/***************************************************************************
 * Handles WM_ECMTGW_DEACTIVATE message.
 ***************************************************************************/
STATIC void GUI_WndDeactivate(EcmtGwUI* ui, EcmtGwDeactReason why)
{
    GUI_WndUpdateComPortUI(ui, False);
    SetFocus(ui->mainDialog);
}

/***************************************************************************
 * Handles WM_ECMTGW_REFRESH_STARTED message.
 ***************************************************************************/
STATIC void GUI_WndRefreshStarted(EcmtGwUI* ui)
{
    ASSERT(!ui->refreshInProgress);
    if (!ui->refreshDone) {
        ui->refreshInProgress = True;
        GUI_WndUpdateRefreshUI(ui);
    }
}

/***************************************************************************
 * Handles WM_ECMTGW_REFRESH_DONE message.
 ***************************************************************************/
STATIC void GUI_WndRefreshDone(EcmtGwUI* ui)
{
    /* ui->refreshInProgress can be False, see GUI_WndRefreshStarted */ 
    if (ui->refreshInProgress) {
        ui->refreshInProgress = False;
        GUI_WndUpdateRefreshUI(ui);
    }
}

/***************************************************************************
 * Fills buffer with the specified Ecmt command
 ***************************************************************************/
STATIC void GUI_FormatEcmtCommand(EcmtGwUI* ui, Buffer* buf, LPCWSTR cmd)
{
    int i;
    RECT rect;
    LPCWSTR p;
    WCHAR value[16];

    static WCHAR KEY[] = L"command";
    static WCHAR TOP[] = L"top";
    static WCHAR LEFT[] = L"left";
 
    // The body contains null separated key-value pairs of UCS2
    // strings in network (big endian, MSB first) byte order
    buf->order = BIG_ENDIAN;
    BUFFER_Clear(buf);

    // "command" parameter
    for (i=0; i<COUNT(KEY); i++) BUFFER_PutI16(buf, KEY[i]);
    while (*cmd) BUFFER_PutI16(buf, *cmd++);
    BUFFER_PutI16(buf, 0);

    // Get screen coordinates of the top-left corner of the client area
    GetClientRect(ui->mainWindow, &rect);
    ClientToScreen(ui->mainWindow, (LPPOINT)&rect);

    // "top" parameter
    swprintf(value, L"%d", rect.top);
    for (i=0; i<COUNT(TOP); i++) BUFFER_PutI16(buf, TOP[i]);
    for (p=value; *p; p++) BUFFER_PutI16(buf, *p);
    BUFFER_PutI16(buf, 0);

    // "left" parameter
    swprintf(value, L"%d", rect.left);
    for (i=0; i<COUNT(LEFT); i++) BUFFER_PutI16(buf, LEFT[i]);
    for (p=value; *p; p++) BUFFER_PutI16(buf, *p);
    BUFFER_PutI16(buf, 0);
}

/***************************************************************************
 * Sends one Ecmt command
 ***************************************************************************/
STATIC Bool GUI_SendOneEcmtCommand(EcmtGwUI* ui, LPCWSTR cmd)
{
    Bool ok = False;
    if (ECMTGW_CountEcmtClients(ui->engine) > 0) {
        /* somebody is connected, then the command to them */
        Buffer buf;
        BUFFER_Init(&buf);
        ASSERT(VECTOR_IsEmpty(&ui->ecmtCommands));
        GUI_FormatEcmtCommand(ui, &buf, cmd);
        ok = ECMTGW_SendMessage(ui->engine, EcmtManagerUid, 
            BUFFER_Access(&buf), BUFFER_Size(&buf), 
            EcmtDirectionToClients);
        BUFFER_Destroy(&buf);
    }
    return ok;
}

/***************************************************************************
 * Handles WM_ECMTGW_NEW_CLIENT message.
 ***************************************************************************/
STATIC void GUI_WndNewClient(EcmtGwUI* ui)
{
    ASSERT(ECMTGW_CountEcmtClients(ui->engine) > 0);
    if (ui->waitDialog) {
        SendMessage(ui->waitDialog, WM_COMMAND, IDOK, 0);
    }
    if (ECMTGW_CountEcmtClients(ui->engine) > 0 &&
        !VECTOR_IsEmpty(&ui->ecmtCommands)) {
        int i;
        Buffer buf;
        BUFFER_Init(&buf);

        /* send pending Ecmt commands */
        for (i=0; i<VECTOR_Size(&ui->ecmtCommands); i++) {
            GUI_FormatEcmtCommand(ui, &buf, VECTOR_Get(&ui->ecmtCommands,i));
            ECMTGW_SendMessage(ui->engine, EcmtManagerUid, 
                BUFFER_Access(&buf), BUFFER_Size(&buf), 
                EcmtDirectionToClients);
        }

        VECTOR_Clear(&ui->ecmtCommands);
        BUFFER_Destroy(&buf);
    }
}

/***************************************************************************
 * Destroys the EcmtGwUI and all associated data structures
 ***************************************************************************/
STATIC void GUI_Destroy(EcmtGwUI* ui)
{
    if (ui) {
        QEntry* e;

        ListView_DeleteAllItems(ui->listView);
        SetWindowLong(ui->mainWindow, GWL_USERDATA, 0);

        if (ui->statusFont) DeleteFont(ui->statusFont);
        if (ui->comPortFont) DeleteFont(ui->comPortFont);

        while ((e = QUEUE_RemoveTail(&ui->ports)) != NULL) {
            EcmtGwPort* port = QCAST(e, EcmtGwPort, entry);
            MEM_Free(port);
        }

        if (ui->ecmtManHandle) {
            DWORD exitCode = 0;
            if (GUI_SendOneEcmtCommand(ui, KExitCore)) {
                /* Give it 5 sec to exit */
                TRACE("GUI: waiting for EcmtManager to exit\n");
                WaitForSingleObject(ui->ecmtManHandle,5000);
            }
            if (GetExitCodeProcess(ui->ecmtManHandle,&exitCode) &&
                exitCode == STILL_ACTIVE) {
                /* If it's still alive, kill it */
                TRACE("GUI: terminating EcmtManager process\n");
                TerminateProcess(ui->ecmtManHandle, 0);
            }
            CloseHandle(ui->ecmtManHandle);
        }

        ECMTGW_Destroy(ui->engine);
        VECTOR_Destroy(&ui->ecmtCommands);
        MEM_Free(ui->ecmtManExe);
        MEM_Free(ui->emulatorHome);
        MEM_Free(ui);
    }
}

/***************************************************************************
 * Returns full path to the directory where the Ecmt Gateway executable
 * is located.
 ***************************************************************************/
STATIC Str GUI_ExeDir(StrBuf* sb)
{
    if (WIN32_GetModuleFileName(NULL, sb)) {
        STRBUF_SetLength(sb, FILE_FilePart(sb->s) - sb->s);
        return STRBUF_Text(sb);
    } else {
        return NULL;
    }
}

/***************************************************************************
 * Returns full path to EPOCROOT.
 ***************************************************************************/
STATIC Str GUI_EpocRoot2(StrBuf* sb)
{
    Str ret = NULL;
    if (GUI_ExeDir(sb)) {
        int i = STRBUF_FindLastNoCase(sb, "epoc32");
        if (i >= 0) {
            STRBUF_SetLength(sb, i);
            ret = STRBUF_Text(sb);
        } else {
            Str s = getenv("EPOCROOT");
            if (s && FILE_IsDir(s) && STRBUF_Copy(sb, s)) {
                ret = STRBUF_Text(sb);
            }
        }
    }
    return ret;
}

/* The caller must deallocated the returned string with MEM_Free */
STATIC Char* GUI_EpocRoot()
{
    Char* epocRoot = NULL;
    StrBuf128 buf;
    STRBUF_InitBufXXX(&buf);
    if (GUI_EpocRoot2(&buf.sb)) {
        epocRoot = STRBUF_Dup(&buf.sb);
    }
    STRBUF_Destroy(&buf.sb);
    return epocRoot;
}

/***************************************************************************
 * Determines the product id. If midp.exe is found from epoc32\tools\ecmt, 
 * following code assumes that this is MIDP SDK. The returned string is 
 * static and does not need to be deallocated. 
 ***************************************************************************/
STATIC Str GUI_ProductId()
{
    Str productId = SDK_PRODUCT_ID_CPP;
    StrBuf128 buf;
    STRBUF_InitBufXXX(&buf);
    if (GUI_EpocRoot2(&buf.sb)) {
        int len;
        if (!STRBUF_EndsWith(&buf.sb, "\\")) {
            STRBUF_Append(&buf.sb, "\\");
        }
        len = STRBUF_Length(&buf.sb);
        
        STRBUF_SetLength(&buf.sb, len);
        STRBUF_Append(&buf.sb, KEcmtManDir);
        if (!STRBUF_EndsWith(&buf.sb, "\\")) {
            STRBUF_Append(&buf.sb, "\\");
        }
		STRBUF_Append(&buf.sb, KMIDPFile);
		
        if (FILE_Exist(STRBUF_Text(&buf.sb))) {
            TRACE1("GUI: found MIDP file %s\n",STRBUF_Text(&buf.sb));
            
			productId = SDK_PRODUCT_ID_MIDP;			
         }       
    }
    STRBUF_Destroy(&buf.sb);
    TRACE1("GUI: product id \"%s\"\n",productId);
    return productId;
}

/***************************************************************************
 * Returns full path to the root of the SDK installation (one level above
 * EPOCROOT). The caller must deallocated the returned string with MEM_Free
 ***************************************************************************/
STATIC Char* GUI_EmulatorHome()
{
    Char* home = NULL;
    StrBuf128 buf;
    STRBUF_InitBufXXX(&buf);
    if (GUI_EpocRoot2(&buf.sb)) {
        int i;
        if (STRBUF_EndsWith(&buf.sb, "\\")) {
            STRBUF_SetLength(&buf.sb, STRBUF_Length(&buf.sb)-1);
        }
        i = STRBUF_LastIndexOf(&buf.sb, '\\');
        if (i >= 0) {
            /* Handle root directory */
            STRBUF_SetLength(&buf.sb, (i>2) ? i : (i+1));
            home = STRBUF_Dup(&buf.sb);
        }
    }
    STRBUF_Destroy(&buf.sb);
    return home;
}

/***************************************************************************
 * Returns full path to the tools/ecmt directory. Does not check whether
 * the directory actually exists.
 ***************************************************************************/
STATIC Str GUI_EcmtManDir(StrBuf* sb)
{
    if (GUI_EpocRoot2(sb)) {
        if ((STRBUF_EndsWith(sb, "\\") || STRBUF_Append(sb, "\\")) &&
            STRBUF_Append(sb, KEcmtManDir)) {
            return STRBUF_Text(sb);
        }
    } else {
        if (GUI_ExeDir(sb) && 
            (STRBUF_EndsWith(sb, "\\") || STRBUF_Append(sb, "\\"))) {
            return STRBUF_Text(sb);
        }
    }
    STRBUF_Clear(sb);
    return NULL;
}

/***************************************************************************
 * Returns full path to the EcmtManager executable. Does NOT check whether
 * the executable actually exists. The caller must deallocated the returned
 * string with MEM_Free
 ***************************************************************************/
STATIC Char* GUI_EcmtManExePath()
{
    Char* exe = NULL;
    StrBuf128 buf;
    STRBUF_InitBufXXX(&buf);
    if (GUI_EcmtManDir(&buf.sb)) {
        if ((STRBUF_EndsWith(&buf.sb,"\\") || STRBUF_Append(&buf.sb,"\\")) &&
            STRBUF_Append(&buf.sb, KEcmtManExe)) {
            exe = STRBUF_Dup(&buf.sb);
        }
    }
    STRBUF_Destroy(&buf.sb);
    return exe;
}

/***************************************************************************
 * Comparison routine for WCHAR strings
 ***************************************************************************/
static Bool GUI_EqualPWSTR(VElementC e1, VElementC e2)
{
    LPCWSTR s1 = e1;
    LPCWSTR s2 = e2;
    return (wcscmp(s1,s2) == 0);
}

/***************************************************************************
 * Handles WM_CREATE message. Returns True if the window has been created
 * successfully, False on failure
 ***************************************************************************/
STATIC Bool GUI_WndCreate(HWND hWnd, CREATESTRUCT* createParams)
{
    EcmtGwUI* ui = MEM_New(EcmtGwUI);
    if (ui) {
        EcmtGwCallbacks cb;
        HANDLE hInst = (HANDLE)GetWindowLong(hWnd, GWL_HINSTANCE);

        ZeroMemory(ui, sizeof(*ui));
        ui->mainWindow = hWnd;
        ui->module = hInst;
        QUEUE_Init(&ui->ports);
        VECTOR_Init(&ui->ecmtCommands,0, GUI_EqualPWSTR, NULL);
        ui->ecmtManExe = GUI_EcmtManExePath();
        ui->emulatorHome = GUI_EmulatorHome();

        /* Create the engine fist */
        ZeroMemory(&cb, sizeof(cb));
        cb.activate = GUI_ActivateCB;
        cb.deactivate = GUI_DeactivateCB;
        cb.portFound = GUI_PortFoundCB;
        cb.portModified = GUI_PortChangeCB;
        cb.portDisappeared = GUI_PortGoneCB;
        cb.refreshStart = GUI_RefreshStart;
        cb.refreshDone = GUI_RefreshDone;
        cb.connect = GUI_ClientConnectCB;
        cb.sendPacket = GUI_SendPacketCB;
        cb.sendPacketComplete = GUI_SendPacketCompleteCB;
        cb.receivePacket = GUI_ReceivePacketCB;
        cb.receivePacketComplete = GUI_ReceivePacketCompleteCB;
        cb.toFront = GUI_ToFrontCB;
        cb.exit = GUI_ExitCB;
        ui->engine = ECMTGW_Create(GUI_ProductId(), &cb, ui);
        if (ui->engine) {

            /* create the dialog */
            HWND hDlg = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_MAIN),
                hWnd, GUI_MainDlgProc, (LPARAM)ui);

            TRACE1("GUI: ECMT port = %d\n",ECMTGW_GetEcmtPort(ui->engine));
            ASSERT(ui->mainDialog == hDlg);
            ASSERT(ui->listView);

            if (ui->mainDialog && ui->listView) {

                /* set up fonts */
                LOGFONT logFont;
                char faceName[LF_FACESIZE];
                HWND hStatusLabel = GetDlgItem(ui->mainDialog, IDC_STATUS);
                HWND hPortLabel = GetDlgItem(ui->mainDialog, IDC_PORT_NAME);
                HWND hPortLabel2 = GetDlgItem(ui->mainDialog, IDC_PORT_NAME2);
                HFONT hFont = GetWindowFont(hPortLabel);
                if (GetObject(hFont, sizeof(logFont), &logFont)) {

                    logFont.lfWeight = FW_BOLD;
                    logFont.lfQuality = ANTIALIASED_QUALITY;
                    ui->statusFont = CreateFontIndirect(&logFont);
                    ASSERT(ui->statusFont);
                    if (ui->statusFont) {
                        SetWindowFont(hStatusLabel, ui->statusFont, FALSE);
                    }

                    logFont.lfHeight = 24;
                    strcpy(faceName, logFont.lfFaceName);
                    strcpy(logFont.lfFaceName, "Tahoma");
                    ui->comPortFont = CreateFontIndirect(&logFont);
                    ASSERT(ui->comPortFont);
                    if (!ui->comPortFont) {
                        /* restore the sace name */
                        strcpy(logFont.lfFaceName, faceName);
                        ui->comPortFont = CreateFontIndirect(&logFont);
                        ASSERT(ui->comPortFont);
                    }
                    if (ui->comPortFont) {
                        SetWindowFont(hPortLabel, ui->comPortFont, FALSE);
                        SetWindowFont(hPortLabel2, ui->comPortFont, FALSE);
                    }
                }

                /* create the toolbar */
                ui->toolBar = CreateToolbarEx(hWnd, 
                    WS_CHILD | WS_VISIBLE | CCS_TOP | TBSTYLE_FLAT | 
                    TBSTYLE_WRAPABLE | TBSTYLE_AUTOSIZE | TBSTYLE_TOOLTIPS,
                    ID_TOOLBAR, 1, NULL, 0, NULL, 0,
                    0, 100, 0, 0, sizeof (TBBUTTON));

                if (ui->toolBar) {
                    int i, dx, dy;
                    StrBuf sb;
                    TBBUTTON buttons[NBUTTONS];
                    static COLORREF BGCOLOR = RGB(255,0,255);
                    static COLORREF BGCOLOR2 = RGB(255,255,255);

                    RECT barRect, dlgRect, wndRect, clientRect;
                    ui->toolBarImg = ImageList_Create(32,26,
                        ILC_MASK|ILC_COLOR8,NBUTTONS,0);
                    ui->toolBarImg2 = ImageList_Create(32,26,
                        ILC_MASK|ILC_COLOR4,NBUTTONS,0);

                    ImageList_AddMasked(ui->toolBarImg,LoadBitmap(hInst,
                        MAKEINTRESOURCE(IDB_TOOLBAR)),BGCOLOR);
                    ImageList_AddMasked(ui->toolBarImg2,LoadBitmap(hInst,
                        MAKEINTRESOURCE(IDB_TOOLBAR2)),BGCOLOR2);
                    SendMessage(ui->toolBar, TB_SETHOTIMAGELIST, 0, 
                        (LPARAM)ui->toolBarImg);
                    SendMessage(ui->toolBar, TB_SETIMAGELIST, 0, 
                        (LPARAM)ui->toolBarImg2);

                    STRBUF_Init(&sb);
                    ZeroMemory(buttons, sizeof(buttons));

                    /* configure toolbar */
                    SendMessage(ui->toolBar, TB_SETEXTENDEDSTYLE, 0, 
                        TBSTYLE_EX_MIXEDBUTTONS);

                    i = 0;
                    VERIFY(WIN32_LoadString(hInst, IDS_TOOLBAR_CONNECT, &sb));
                    buttons[i].iBitmap = i;
                    buttons[i].iString = SendMessage(ui->toolBar, TB_ADDSTRING, 0,
                        (LPARAM)STRBUF_Text(&sb)); 
                    buttons[i].idCommand = IDM_CONNECT;
                    buttons[i].fsStyle = BTNS_SHOWTEXT | TBSTYLE_BUTTON;

                    i++;
                    VERIFY(WIN32_LoadString(hInst, IDS_TOOLBAR_STOP, &sb));
                    buttons[i].iBitmap = i;
                    buttons[i].iString = SendMessage(ui->toolBar, TB_ADDSTRING, 0,
                        (LPARAM)STRBUF_Text(&sb)); 
                    buttons[i].idCommand = IDM_STOP;
                    buttons[i].fsStyle = BTNS_SHOWTEXT | TBSTYLE_BUTTON;
                    
                    i++;
                    VERIFY(WIN32_LoadString(hInst, IDS_TOOLBAR_REFRESH, &sb));
                    buttons[i].iBitmap = i;
                    buttons[i].iString = SendMessage(ui->toolBar, TB_ADDSTRING, 0,
                        (LPARAM)STRBUF_Text(&sb)); 
                    buttons[i].idCommand = IDM_REFRESH;
                    buttons[i].fsStyle = BTNS_SHOWTEXT | TBSTYLE_BUTTON;
                    SendMessage(ui->toolBar,TB_ADDBUTTONS,COUNT(buttons),
                        (LPARAM)&buttons);
                    
                    STRBUF_Destroy(&sb);

                    /* position the dialog */
                    GetWindowRect(ui->toolBar, &barRect);
                    GetWindowRect(ui->mainDialog, &dlgRect);
                    MoveWindow(ui->mainDialog, 0, RECT_Height(&barRect),
                        RECT_Width(&dlgRect), RECT_Height(&dlgRect), FALSE);

                    /* set the right window size */
                    GetWindowRect(ui->mainWindow, &wndRect);
                    GetClientRect(ui->mainWindow, &clientRect);
                    dx = RECT_Width(&wndRect) - RECT_Width(&clientRect);
                    dy = RECT_Height(&wndRect) - RECT_Height(&clientRect);
                    MoveWindow(ui->mainWindow, 
                        createParams->x, createParams->y,
                        RECT_Width(&dlgRect) + dx,
                        RECT_Height(&dlgRect) + RECT_Height(&barRect) + dy,
                        FALSE);

                    /* Create "Refresh" animation control on the toolbar */
                    ui->refreshAnimation = CreateWindow(ANIMATE_CLASS, NULL,
                        WS_CHILD | ACS_TRANSPARENT | ACS_TIMER, 0, 0, 0, 0,
                        ui->toolBar, (HMENU)ID_REFRESH_ANIMATION, hInst, NULL);
                    if (ui->refreshAnimation) {
                        TOOLINFO ti;
                        HANDLE hToolTips;
                        RECT toolbarRect, refreshRect;
                        SendMessage(ui->toolBar,TB_AUTOSIZE,0,0);
                        Animate_Open(ui->refreshAnimation, 
                            MAKEINTRESOURCE(IDR_REFRESH));

                        /* Position the animation at the bottom right corner */
                        GetClientRect(ui->toolBar, &toolbarRect);
                        GetWindowRect(ui->refreshAnimation, &refreshRect);
                        MoveWindow(ui->refreshAnimation, 
                            toolbarRect.right - RECT_Width(&refreshRect) - 6,
                            toolbarRect.bottom - RECT_Height(&refreshRect) - 4,
                            RECT_Width(&refreshRect),
                            RECT_Height(&refreshRect),
                            FALSE);

                        /* Set up tooltip for the animation control */
                        hToolTips = (HWND)SendMessage(ui->toolBar,
                            TB_GETTOOLTIPS,0,0);
                        memset(&ti, 0, sizeof(ti));
                        ti.cbSize = sizeof(ti);
                        ti.hwnd = ui->toolBar;
                        ti.uId = ID_REFRESH_ANIMATION;
                        ti.lpszText = LPSTR_TEXTCALLBACK;
                        GetWindowRect(ui->refreshAnimation, &ti.rect);
                        ScreenToClient(ti.hwnd, ((POINT*)&ti.rect));
                        ScreenToClient(ti.hwnd, ((POINT*)&ti.rect)+1);
                        SendMessage(hToolTips,TTM_ADDTOOL,0,(WPARAM)&ti);
                    }

                    /* associate pointer with the window */
                    SetLastError(NO_ERROR);
                    SetWindowLong(hWnd, GWL_USERDATA, (LONG)ui);
                    if (GetLastError() == NO_ERROR) {
                        GUI_WndUpdateComPortUI(ui,ECMTGW_IsActive(ui->engine));
                        return True;
                    }
                }
            }
        }
        GUI_Destroy(ui);
    }
    return False;
}

/***************************************************************************
 * Handles WM_DESTROY message.
 ***************************************************************************/
STATIC void GUI_WndDestroy(EcmtGwUI* ui)
{
    /*
     * WM_DESTROY comes to the child windows after the parent window has been
     * destroyed. It seems wrong to me, but that's how it is. Whatever cleanup
     * the child windows have to do, it needs to be done here, before EcmtGwUI
     * context is destroyed.
     */
    ASSERT(!ui->waitDialog);
    SetWindowLong(ui->mainDialog, DWL_USER, 0);
    SetWindowLong(ui->listView, GWL_USERDATA, (LONG)ui);
    SetWindowLong(ui->listView, GWL_WNDPROC, (LONG)ui->listWndProc);
    ui->listWndProc = NULL;

    GUI_Destroy(ui);
    PostQuitMessage(0);
}

/***************************************************************************
 * Dialog proc for the "Please wait..." dialog.
 ***************************************************************************/
STATIC BOOL CALLBACK GUI_WaitProc(HWND hDlg, UINT m, WPARAM w, LPARAM l)
{
    EcmtGwUI* ui;
    switch (m)
    {
    case WM_INITDIALOG:
        ui = (EcmtGwUI*)l;
        ui->waitDialog = hDlg;
        SetWindowLong(hDlg, DWL_USER, l);
        WIN32_CenterWindow(hDlg, NULL);
        Animate_Open(GetDlgItem(hDlg, IDC_ANIMATE), MAKEINTRESOURCE(IDR_WAIT));
        return TRUE;

    case WM_COMMAND:
        ui = (EcmtGwUI*)GetWindowLong(hDlg, DWL_USER);
        switch (w) {
        case IDCANCEL:
            VECTOR_Clear(&ui->ecmtCommands);
            /* no break */
        case IDOK:
            EndDialog(hDlg, w);
            return TRUE;
        }
        return FALSE;

    case WM_DESTROY:
        ui = (EcmtGwUI*)GetWindowLong(hDlg, DWL_USER);
        ASSERT(ui->waitDialog == hDlg);
        SetWindowLong(hDlg, DWL_USER, 0);
        ui->waitDialog = NULL;
        return TRUE;

    default:
        return FALSE;
    }
}

/***************************************************************************
 * Submits Ecmt command
 ***************************************************************************/
STATIC void GUI_SubmitEcmtCommand(EcmtGwUI* ui, LPCWSTR cmd)
{
    if (ECMTGW_CountEcmtClients(ui->engine) > 0) {
        /* somebody is connected, then the command to them */
        GUI_SendOneEcmtCommand(ui, cmd);
    } else {
        /* place this command to the the command queue */
        if (!VECTOR_Contains(&ui->ecmtCommands, cmd)) {
            VECTOR_Add(&ui->ecmtCommands, cmd);
        }
        
        /* Check if we have already started the process */
        if (ui->ecmtManHandle) {
            DWORD exitCode = 0;
            if (!GetExitCodeProcess(ui->ecmtManHandle,&exitCode) ||
                exitCode != STILL_ACTIVE) {
                /* Close stale process handle */
                TRACE("GUI: EcmtManager has apparently died\n");
                VERIFY(CloseHandle(ui->ecmtManHandle));
                ui->ecmtManHandle = NULL;
            }
        }
        if (ui->ecmtManHandle) {
            /* ECMT Manager has been started, but didn't connect yet */
            DialogBoxParam(ui->module, MAKEINTRESOURCE(IDD_WAIT), 
                ui->mainWindow, GUI_WaitProc, (LPARAM)ui);
        } else if (ui->ecmtManExe) {
            if (FILE_IsFile(ui->ecmtManExe)) {
                STARTUPINFO info;
                PROCESS_INFORMATION pi;
                Bool hasSpace = (STRING_IndexOf(ui->ecmtManExe,' ') >= 0);
                StrBuf wd;
                StrBuf cmdline;
                STRBUF_Init(&wd);
                STRBUF_Init(&cmdline);

                if (hasSpace) STRBUF_Append(&cmdline,"\"");
                STRBUF_Append(&cmdline, ui->ecmtManExe);
                if (hasSpace) STRBUF_Append(&cmdline,"\"");
#if DEBUG
                STRBUF_Append(&cmdline, " -DlogLevel=DEBUG");
                STRBUF_Append(&cmdline, " -Dnokia.wtk.verbose=true");
#endif /* DEBUG */
                STRBUF_Append(&cmdline, " -Dstart=core");
                STRBUF_Append(&cmdline, " -Dcommand=open");
                STRBUF_Append(&cmdline, " -Dhost=127.0.0.1");
                STRBUF_AppendFormat(&cmdline," -Dport=%d", 
                    ECMTGW_GetEcmtPort(ui->engine));
                if (ui->emulatorHome) {
                    STRBUF_AppendFormat(&cmdline," -Demulator.home=\"%s\"",
                        ui->emulatorHome);
                }

                ZeroMemory(&info, sizeof(info));
                info.cb = sizeof(info);
                if (CreateProcess(ui->ecmtManExe, cmdline.s, NULL, NULL,
                        FALSE, 0, NULL, GUI_EcmtManDir(&wd), &info, &pi)) {
                    ui->ecmtManHandle = pi.hProcess;    
                    TRACE("GUI: started EcmtManager\n");
                    DialogBoxParam(ui->module, MAKEINTRESOURCE(IDD_WAIT), 
                        ui->mainWindow, GUI_WaitProc, (LPARAM)ui);
                } else {
                    ECMTGW_FormatError(ui,IDS_ECMTMAN_NOT_STARTED);
                }
                STRBUF_Destroy(&cmdline);
                STRBUF_Destroy(&wd);
            } else {
                ECMTGW_FormatError(ui,IDS_ECMTMAN_NOT_FOUND,ui->ecmtManExe);
            }
        } else {
            /* ECMT Manager executable is nowhere to be found */
            ECMTGW_FormatError(ui,IDS_ECMTMAN_MISSING,ui->ecmtManExe);
        }
    }
}

/***************************************************************************
 * Handles WM_COMMAND message. Return True if the command has been handled,
 * False otherwise.
 ***************************************************************************/
STATIC Bool GUI_WndCommand(EcmtGwUI* ui, UINT code, UINT cmd)
{
    switch (cmd) {
    case IDM_CONNECT:
        if (ui->port && !ECMTGW_IsActive(ui->engine)) {
            EcmtGatewayPort* gp = ui->port->port;
            TRACE1("GUI: connecting to %s\n",ECMTGW_GetPortName(gp));
            ECMTGW_Activate(gp);
        }
        return True;

    case IDM_STOP:
        ECMTGW_Deactivate(ui->engine);
        return True;

    case IDM_REFRESH:
        if (!ECMTGW_IsActive(ui->engine)) {
            ui->refreshDone = False;
            ECMTGW_RefreshPorts(ui->engine);
        }
        return True;

    case IDM_EXIT:
        DestroyWindow(ui->mainWindow);
        return True;

    case IDM_DIAGNOSTICS:
        GUI_SubmitEcmtCommand(ui, KShowDiagnostics);
        return True;
	case IDM_WLAN_SETTINGS:
		PostMessage(ui->mainDialog, WM_WLAN_SET, 0, 0);
		return True;
    }
    return False;
}

/***************************************************************************
 * WM_NOTIFY handler
 ***************************************************************************/
STATIC void GUI_WndNotify(EcmtGwUI* ui, int idCtrl, LPNMHDR pnmh)
{
    switch (pnmh->code) {
    case TTN_NEEDTEXT:
        { 
            LPTOOLTIPTEXT lpttt = CAST(pnmh,TOOLTIPTEXT,hdr); 
            lpttt->hinst = ui->module;
            switch (lpttt->hdr.idFrom) {
            case IDM_CONNECT: 
                lpttt->lpszText = MAKEINTRESOURCE(IDS_TIP_CONNECT);
                break;
            case IDM_STOP:
                lpttt->lpszText = MAKEINTRESOURCE(IDS_TIP_STOP);
                break;
            case IDM_REFRESH:
                lpttt->lpszText = MAKEINTRESOURCE(IDS_TIP_REFRESH);
                break; 
            case ID_REFRESH_ANIMATION:
                if (GetWindowStyle(ui->refreshAnimation) & WS_VISIBLE) {
                    lpttt->lpszText = MAKEINTRESOURCE(IDS_TIP_SEARCHING);
                } else {
                    /* Animation control is hidden, no tooltip */
                    lpttt->lpszText = NULL;
                }
                break; 
            } 
        } 
        break; 
    }
}

/***************************************************************************
 * Main window proc
 ***************************************************************************/
LRESULT CALLBACK GUI_MainWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
    if (msg == WM_CREATE) {
        return GUI_WndCreate(hWnd,(CREATESTRUCT*)lp) ? 0 : -1;
    } else {
        EcmtGwUI* ui = (EcmtGwUI*)GetWindowLong(hWnd, GWL_USERDATA);
        if (ui) {
            ASSERT(ui->mainWindow == hWnd);
            switch (msg) {
            case WM_ECMTGW_SELECT_PORT:
                GUI_WndSelectPort(ui, (EcmtGwPort*)wp);
                return 0;

            case WM_ECMTGW_ACTIVATE:
                GUI_WndActivate(ui, (EcmtGatewayPort*)wp);
                return 0;

            case WM_ECMTGW_DEACTIVATE:
                GUI_WndDeactivate(ui, wp);
                return 0;

            case WM_ECMTGW_NEW_CLIENT:
                GUI_WndNewClient(ui);
                return 0;

            case WM_ECMTGW_REFRESH_STARTED:
                GUI_WndRefreshStarted(ui);
                return 0;

            case WM_ECMTGW_REFRESH_DONE:
                GUI_WndRefreshDone(ui);
                return 0;

            case WM_SETFOCUS:
                SetFocus(ui->mainDialog);
                return 0;

            case WM_COMMAND:
                if (GUI_WndCommand(ui, HIWORD(wp), LOWORD(wp))) return 0;
                break;

            case WM_NOTIFY:
                GUI_WndNotify(ui, (int)wp, (LPNMHDR)lp);
                return 0;

            case WM_CLOSE:
                DestroyWindow(ui->mainWindow);
                return 0;

            case WM_DESTROY:
                GUI_WndDestroy(ui);
                return 0;
            }
        }
        return DefWindowProc(hWnd, msg, wp, lp);
    }
}

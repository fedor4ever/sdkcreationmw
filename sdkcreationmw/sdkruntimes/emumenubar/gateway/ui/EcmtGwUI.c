/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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

#define ECMTGW_WINDOW_STYLE (WS_OVERLAPPED     | \
                             WS_CAPTION        | \
                             WS_SYSMENU        | \
                             WS_MINIMIZEBOX)

/* the application title is replaced with localized version when we start */
static Str ecmtAppTitle = "Device Connectivity Tool for S60 SDK";

/***************************************************************************
 * Formats error message and shows it in a message box. uMsgID is the
 * resource id of the format string.
 ***************************************************************************/
void ECMTGW_FormatError(EcmtGwUI* ui, UINT uMsgID, ...)
{
    StrBuf64 buf;
    Str msg;
    va_list va;
    va_start(va, uMsgID);
    STRBUF_InitBufXXX(&buf);
    msg = WIN32_FormatMessageVa(ui->module, uMsgID, &buf.sb, va);
    if (!msg) {
        STRBUF_Format(&buf.sb, "ERROR %u", uMsgID);
        msg = STRBUF_Text(&buf.sb);
    }
    MessageBox(ui->mainWindow, msg, ecmtAppTitle, MB_OK|MB_ICONINFORMATION);
    STRBUF_Destroy(&buf.sb);
}

/***************************************************************************
 * Creates the UI
 ***************************************************************************/
STATIC HWND ECMTGW_CreateUI(HMODULE hModule, Str param)
{
    static const LPCTSTR className = TEXT("EcmtGwUI");

    WNDCLASS wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.lpfnWndProc = GUI_MainWndProc;
    wc.hInstance = hModule;
    wc.hIcon = LoadIcon(hModule, MAKEINTRESOURCE(IDI_APP));
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
    wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
    wc.lpszClassName = "EcmtGwUI";
    if (RegisterClass(&wc)) {

        /* Load window size and position from a prefs file? */
        int x = CW_USEDEFAULT;
        int y = CW_USEDEFAULT;
        int w = CW_USEDEFAULT;
        int h = CW_USEDEFAULT;
        HWND hWnd = CreateWindowEx(WS_EX_CONTROLPARENT, wc.lpszClassName,
            ecmtAppTitle, ECMTGW_WINDOW_STYLE, x, y, w, h, NULL, NULL, 
            hModule, (LPVOID)param);

        if (hWnd) {
            return hWnd;
        }
        UnregisterClass(wc.lpszClassName, wc.hInstance);
    }
    return NULL;
}

/***************************************************************************
 * Entry point
 ***************************************************************************/
int main(int argc, char* argv[])
{
    int ret = 1;
    HWND hWnd;
    StrBuf title;
    INITCOMMONCONTROLSEX init;
    Str param = ((argc > 1) ? argv[1] : NULL);
    HMODULE hModule = GetModuleHandle(NULL);
    
    MEM_InitModule();

    /* Load the title */
    STRBUF_Init(&title);
    if (WIN32_LoadString(hModule, IDS_TITLE, &title)) {
        ecmtAppTitle = STRBUF_Text(&title);
    }

    /* Initialize common controls */
    init.dwSize = sizeof(init);
    init.dwICC = ICC_BAR_CLASSES|ICC_LISTVIEW_CLASSES|ICC_ANIMATE_CLASS;
    InitCommonControlsEx(&init);

    /* Initialize the UI */
    hWnd = ECMTGW_CreateUI(hModule, param);
    if (hWnd) {

        /* Main message loop */
        MSG msg;
        HACCEL hAccel = LoadAccelerators(hModule, MAKEINTRESOURCE(IDC_ACCEL));
        ShowWindow(hWnd, SW_SHOWNORMAL);
        while (GetMessage(&msg, NULL, 0, 0)) {
            if (!TranslateAccelerator(msg.hwnd, hAccel, &msg)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        ret = msg.wParam;
    }
    
    /* Cleanup */
    STRBUF_Destroy(&title);
    MEM_Shutdown();
    return ret;
}
